
// #include "stdafx.h"

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType);

#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>

#include <vtkLineSource.h>
#include <vtkSmartPointer.h>
#include <vtkRendererCollection.h>
#include <vtkWorldPointPicker.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkProperty.h>
#include <vtkMath.h>
#include <vtkCamera.h>

#define vtkNew(type, name)\
    vtkSmartPointer<type> name = vtkSmartPointer<type>::New()


double m_device_pick_position_sequence[100][2];
int m_pick_counter=0;
vtkSmartPointer<vtkPoints> m_poly_points3D;
vtkActor **m_polypoints;
vtkActor **m_polylines;

struct Point {
	float x;
	float y;
};
// Define interaction style
class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera
{

public:
	static MouseInteractorStyle* New();
	vtkTypeMacro(MouseInteractorStyle, vtkInteractorStyleTrackballCamera)

		virtual void OnLeftButtonDown()
	{
		vtkRenderWindowInteractor* I = this->Interactor;
		int device_x = I->GetEventPosition()[0];
		int device_y = I->GetEventPosition()[1];
		double device_position[2] = { device_x, device_y };
		vtkRenderer* ren = I->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
		I->GetPicker()->Pick(device_x, device_y, 0, ren);
		double picked[3];
		I->GetPicker()->GetPickPosition(picked);
		if (picked[0] && picked[1] && picked[2]) {
			m_device_pick_position_sequence[m_pick_counter][0] = device_x;
			m_device_pick_position_sequence[m_pick_counter][1] = device_y;
			m_pick_counter++;

			m_poly_points3D->InsertNextPoint(picked);
			AddMarker(ren, picked);
			AddGraphicsPolyLine(ren);
		}
		vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
	}

	void AddMarker(vtkRenderer* ren,double picked[3]) {
		vtkNew(vtkSphereSource, sphereSource);
		sphereSource->SetRadius(2);
		sphereSource->SetPhiResolution(50);
		sphereSource->SetThetaResolution(50);
		sphereSource->SetCenter(picked[0], picked[1], picked[2]);
		sphereSource->Update();

		// Create a mapper and actor
		vtkNew(vtkPolyDataMapper, mapper);
		mapper->SetInputConnection(sphereSource->GetOutputPort());
		vtkNew(vtkActor, actor);
		actor->SetMapper(mapper);
		double maroon[] = { 136.0 / 255, 0, 21.0 / 255.0 };
		actor->GetProperty()->SetColor(maroon);
		ren->AddActor(actor);
	}

	void AddGraphicsPolyLine(vtkRenderer* ren)
	{
		int current_index = m_poly_points3D->GetNumberOfPoints() - 1;
		if (current_index) {
			int prev_index = current_index - 1;
			double p1[3];
			double p2[3];
			m_poly_points3D->GetPoint(prev_index, p1);
			m_poly_points3D->GetPoint(current_index, p2);
			OffsetInCameraDirection(ren,p1, p1);
			OffsetInCameraDirection(ren,p2, p2);

			vtkNew(vtkLineSource, lineSource);
			lineSource->SetPoint1(p1);
			lineSource->SetPoint2(p2);
			lineSource->Update();

			// Visualize
			vtkNew(vtkPolyDataMapper, mapper);
			mapper->SetInputConnection(lineSource->GetOutputPort());
			vtkNew(vtkActor, lineactor);
			lineactor->SetMapper(mapper);
			lineactor->GetProperty()->SetLineWidth(4);
			lineactor->GetProperty()->SetColor(0, 1, 0);
			ren->AddActor(lineactor);
			m_polylines[prev_index] = lineactor;
		}
	}

	// ----------------------------------------------------------------------------------------
	// Offset given point position in camera normal direction and return results
	// ----------------------------------------------------------------------------------------
	void OffsetInCameraDirection(vtkRenderer* ren, double p1[3], double p2[3]) {
		// Calculate offsetted position for labels so that labels do not deep inside STL on which user clicks
		double dir_pos[3];
		ren->GetActiveCamera()->GetPosition(dir_pos);
		vtkMath::Normalize(dir_pos);
		vtkMath::MultiplyScalar(dir_pos, 1);
		vtkMath::Add(p1, dir_pos, p2);
	}

	virtual void OnRightButtonDown() {
		vtkInteractorStyleTrackballCamera::OnRightButtonDown();
	}


};
vtkStandardNewMacro(MouseInteractorStyle)

void CalculatePickPositions()
{
	// -------------------------------------------------
	// Calculate bounding box of all picked positions
	// -------------------------------------------------
	int minx = 10000;
	int miny = 10000;
	int maxx = -1;
	int maxy = -1;
	Point polygon[100];
	for (int i = 0; i<m_pick_counter; i++) {
		Point mypoint;
		mypoint.x = m_device_pick_position_sequence[i][0];
		mypoint.y = m_device_pick_position_sequence[i][1];
		polygon[i] = mypoint;

		if (mypoint.x>maxx) {
			maxx = mypoint.x;
		}
		if (mypoint.x<minx) {
			minx = mypoint.x;
		}
		if (mypoint.y>maxy) {
			maxy = mypoint.y;
		}
		if (mypoint.y<miny) {
			miny = mypoint.y;
		}
	}

	//m_surf_reg_stl_points->Reset();
	// -------------------------------------------------------------------------
	// iterate on all points within the bounds of minimum and maximum X and Y
	// and check if they are within the polygon entered by user
	// -------------------------------------------------------------------------
	int xinc = (maxx - minx) / 8;
	int yinc = (maxy - miny) / 8;
	if (xinc<1) {
		xinc = 1;
	}
	if (yinc<1) {
		yinc = 1;
	}
	if (xinc < yinc) {
		yinc = xinc;
	}
	else {
		xinc = yinc;
	}
	int counter = 0;
	int num_iterations = 0;
	for (int x = minx + 1; x<maxx; x += xinc) {
		for (int y = miny + 1; y<maxy; y += yinc) {
			num_iterations++;
			Point p;
			p.x = x;
			p.y = y;

			bool inside = isInside(polygon, m_pick_counter, p);

			if (inside) {
				counter++;
				double devicepos[2];
				double worldpos[3];
				devicepos[0] = p.x;
				devicepos[1] = p.y;
				//GetPickPosition(devicepos, worldpos);
				if (counter % 2 == 0) {
					//AddMarkerForSurfaceRegistration(worldpos);
				}
				//m_surf_reg_stl_points->InsertNextPoint(worldpos);
			}
		}
	}

}

int main(int argc, char *argv[])
{
	m_poly_points3D = vtkSmartPointer<vtkPoints>::New();
	m_polypoints = new vtkActor*[100];
	m_polylines = new vtkActor*[100];
	for (int i = 0; i<100; i++) {
		m_polypoints[i] = 0;
		m_polylines[i] = 0;
	}

	std::string inputFilename("D:\\full_transformed.stl");

	vtkSmartPointer<vtkSTLReader> reader =
		vtkSmartPointer<vtkSTLReader>::New();
	reader->SetFileName(inputFilename.c_str());
	reader->Update();

	// Visualize
	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(reader->GetOutputPort());

	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);


	vtkSmartPointer<MouseInteractorStyle> style =
		vtkSmartPointer<MouseInteractorStyle>::New();
	renderWindowInteractor->SetInteractorStyle(style);


	renderer->AddActor(actor);
	renderWindow->SetSize(600, 600);
	renderWindow->Render();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}
