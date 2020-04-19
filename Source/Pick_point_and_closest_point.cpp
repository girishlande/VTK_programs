
// #include "stdafx.h"

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType);

#include <vtkVersion.h>
#include <vtkSmartPointer.h>

#include <vtkActor.h>
#include <vtkDelaunay2D.h>
#include <vtkLookupTable.h>
#include <vtkMath.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkXMLPolyDataWriter.h>

// For compatibility with new VTK generic data arrays
#ifdef vtkGenericDataArray_h
#define InsertNextTupleValue InsertNextTypedTuple
#endif

//#include "modelinteractionstyle.h"
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
#include <vtkCellLocator.h>
#include <vtkIdList.h>
#include <vtkSTLReader.h>
#include <vtkTriangleFilter.h>

#include <set>
#include <iostream>
using namespace std;

#define vtkNew(type, name)\
    vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

// Create a grid of points (height/terrian map)
vtkPolyData* outputPolyData;
vtkSmartPointer<vtkUnsignedCharArray> colors;

vtkSmartPointer<vtkTriangleFilter> triangleFilter;

set<int> processed_points;

// Define interaction style
class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera
{

public:
	static MouseInteractorStyle* New();
	vtkTypeMacro(MouseInteractorStyle, vtkInteractorStyleTrackballCamera)

		bool m_stl_pressed;

	virtual void OnLeftButtonDown()
	{
		m_pressed = true;

		vtkRenderWindowInteractor* I = this->Interactor;
		int device_x = I->GetEventPosition()[0];
		int device_y = I->GetEventPosition()[1];
		double device_position[2] = { device_x, device_y };
		vtkRenderer* ren = I->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
		I->GetPicker()->Pick(device_x, device_y, 0, ren);
		double picked[3];
		I->GetPicker()->GetPickPosition(picked);

		if (picked[0] == 0 && picked[1] == 0 && picked[2] == 0) {
			m_stl_pressed = false;
			vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
			return;
		}
		m_stl_pressed = true;

		//Find the closest points to TestPoint
		double closestPoint[3];//the coordinates of the closest point will be returned here
		double closestPointDist2; //the squared distance to the closest point will be returned here
		vtkIdType cellId; //the cell id of the cell containing the closest point will be returned here
		int subId; //this is rarely used (in triangle strips only, I believe)
		cellLocator->FindClosestPoint(picked, closestPoint, cellId, subId, closestPointDist2);

		double dist = sqrt(vtkMath::Distance2BetweenPoints(picked, closestPoint));
		cout << "\n Distance : " << dist;

		// Create a sphere
		{
			vtkSmartPointer<vtkSphereSource> sphereSource =
				vtkSmartPointer<vtkSphereSource>::New();
			sphereSource->SetCenter(picked);
			sphereSource->SetRadius(5.0);

			vtkSmartPointer<vtkPolyDataMapper> mapper =
				vtkSmartPointer<vtkPolyDataMapper>::New();
			mapper->SetInputConnection(sphereSource->GetOutputPort());

			vtkSmartPointer<vtkActor> actor =
				vtkSmartPointer<vtkActor>::New();
			actor->SetMapper(mapper);
			I->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);
		}


		I->GetRenderWindow()->Render();
	}

	virtual void OnLeftButtonUp() {
		m_pressed = false;
		m_stl_pressed = false;
	}

	void buildLocator() {
		m_pressed = false;
		m_stl_pressed = false;
		// Create the tree
		cellLocator = vtkSmartPointer<vtkCellLocator>::New();
		cellLocator->SetDataSet(outputPolyData);
		cellLocator->BuildLocator();
	}

	virtual void OnRightButtonDown() {
		vtkInteractorStyleTrackballCamera::OnRightButtonDown();
	}

	virtual void OnMouseMove() {
		if (!m_pressed) {
			return;
		}
		if (!m_stl_pressed) {
			vtkInteractorStyleTrackballCamera::OnMouseMove();
			return;
		}
	}


	vtkSmartPointer<vtkCellLocator> cellLocator;

	bool m_pressed;

};
vtkStandardNewMacro(MouseInteractorStyle)


int main(int, char *[])
{
	std::string inputFilename("D:\\full_transformed.stl");

	vtkSmartPointer<vtkSTLReader> reader =
		vtkSmartPointer<vtkSTLReader>::New();
	reader->SetFileName(inputFilename.c_str());
	reader->Update();

	triangleFilter =
		vtkSmartPointer<vtkTriangleFilter>::New();
	triangleFilter->SetInputConnection(reader->GetOutputPort());
	triangleFilter->Update();
	
	// Create a grid of points (height/terrian map)
	vtkSmartPointer<vtkPoints> points;
	points = vtkSmartPointer<vtkPoints>::New();

	outputPolyData = reader->GetOutput();

	double bounds[6];
	outputPolyData->GetBounds(bounds);

	// Find min and max z
	double minz = bounds[4];
	double maxz = bounds[5];

	// Create the color map
	vtkSmartPointer<vtkLookupTable> colorLookupTable =
		vtkSmartPointer<vtkLookupTable>::New();
	colorLookupTable->SetTableRange(minz, maxz);
	colorLookupTable->Build();

	// Generate the colors for each point based on the color map
	colors =
		vtkSmartPointer<vtkUnsignedCharArray>::New();
	colors->SetNumberOfComponents(3);
	colors->SetName("Colors");

	for (int i = 0; i < outputPolyData->GetNumberOfPoints(); i++)
	{
		double p[3];
		outputPolyData->GetPoint(i, p);

		double dcolor[3];
		colorLookupTable->GetColor(p[2], dcolor);
		unsigned char color[3];
		for (unsigned int j = 0; j < 3; j++)
		{
			color[j] = static_cast<unsigned char>(255.0 * dcolor[j]);
		}
		color[0] = 255;
		color[1] = 0;
		color[2] = 0;
		colors->InsertNextTupleValue(color);
	}

	outputPolyData->GetPointData()->SetScalars(colors);

	// Create a mapper and actor
	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(outputPolyData);

	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	// Create a renderer, render window, and interactor
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
	style->buildLocator();

	// Add the actor to the scene
	renderer->AddActor(actor);
	//renderer->AddActor(stlactor);
	renderer->SetBackground(.1, .2, .3);

	renderWindow->SetSize(600, 600);
	// Render and interact
	renderWindow->Render();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}