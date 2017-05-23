
#include "stdafx.h"

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
#define vtkNew(type, name)\
    vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

// Create a grid of points (height/terrian map)
vtkPolyData* outputPolyData;
vtkSmartPointer<vtkUnsignedCharArray> colors;

// Define interaction style
class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera
{

public:
	static MouseInteractorStyle* New();
	vtkTypeMacro(MouseInteractorStyle, vtkInteractorStyleTrackballCamera)

		virtual void OnLeftButtonDown()
	{
		m_pressed = true;
		vtkRenderWindowInteractor* I = this->Interactor;
		int device_x = I->GetEventPosition()[0];
		int device_y = I->GetEventPosition()[1];
		CalculateDevicePoints(device_x, device_y, 5);
		return;
		double device_position[2] = { device_x, device_y };
		vtkRenderer* ren = I->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
		I->GetPicker()->Pick(device_x, device_y, 0, ren);
		double picked[3];
		I->GetPicker()->GetPickPosition(picked);

		//Find the closest points to TestPoint
		double closestPoint[3];//the coordinates of the closest point will be returned here
		double closestPointDist2; //the squared distance to the closest point will be returned here
		vtkIdType cellId; //the cell id of the cell containing the closest point will be returned here
		int subId; //this is rarely used (in triangle strips only, I believe)
		//cellLocator->FindClosestPoint(picked, closestPoint, cellId, subId, closestPointDist2);
		vtkSmartPointer<vtkIdList> cellIds =
			vtkSmartPointer<vtkIdList>::New();
		cellLocator->FindCellsWithinBounds(bounds, cellIds);

		int number_of_triangles = cellIds->GetNumberOfIds();
		for (int j = 0; j < number_of_triangles; ++j) {
			cellId = cellIds->GetId(j);

			unsigned char color[3];
			color[0] = static_cast<unsigned char>(0);
			color[1] = static_cast<unsigned char>(255);
			color[2] = static_cast<unsigned char>(0);

			vtkSmartPointer<vtkIdList> cellPointIds =
				vtkSmartPointer<vtkIdList>::New();
			outputPolyData->GetCellPoints(cellId, cellPointIds);
			for (int i = 0; i < cellPointIds->GetNumberOfIds(); i++) {
				vtkIdType id = cellPointIds->GetId(i);
				colors->SetTupleValue(id, color);
			}
		}
		colors->Modified();
		I->Render();
		I->GetRenderWindow()->Render();

		//vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
	}

	virtual void OnMouseMove() {
		if (!m_pressed) {
			return;
		}
		vtkRenderWindowInteractor* I = this->Interactor;
		int device_x = I->GetEventPosition()[0];
		int device_y = I->GetEventPosition()[1];
		CalculateDevicePoints(device_x, device_y, 5);
		return;
		double device_position[2] = { device_x, device_y };
		vtkRenderer* ren = I->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
		I->GetPicker()->Pick(device_x, device_y, 0, ren);
		double picked[3];
		I->GetPicker()->GetPickPosition(picked);

		CalculateBounds(picked);

		double testPoint[3] = { 2.0, 0.0, 0.0 };

		//Find the closest points to TestPoint
		double closestPoint[3];//the coordinates of the closest point will be returned here
		double closestPointDist2; //the squared distance to the closest point will be returned here
		vtkIdType cellId; //the cell id of the cell containing the closest point will be returned here
		int subId; //this is rarely used (in triangle strips only, I believe)
		//cellLocator->FindClosestPoint(picked, closestPoint, cellId, subId, closestPointDist2);
		vtkSmartPointer<vtkIdList> cellIds =
			vtkSmartPointer<vtkIdList>::New();
		cellLocator->FindCellsWithinBounds(bounds, cellIds);

		int number_of_triangles = cellIds->GetNumberOfIds();
		for (int j = 0; j < number_of_triangles; ++j) {
			cellId = cellIds->GetId(j);

			unsigned char color[3];
			color[0] = static_cast<unsigned char>(0);
			color[1] = static_cast<unsigned char>(255);
			color[2] = static_cast<unsigned char>(0);

			vtkSmartPointer<vtkIdList> cellPointIds =
				vtkSmartPointer<vtkIdList>::New();
			outputPolyData->GetCellPoints(cellId, cellPointIds);
			for (int i = 0; i < cellPointIds->GetNumberOfIds(); i++) {
				vtkIdType id = cellPointIds->GetId(i);
				colors->SetTupleValue(id, color);
			}
		}
		colors->Modified();

		I->Render();
		I->GetRenderWindow()->Render();

		//vtkInteractorStyleTrackballCamera::OnMouseMove();
	}

	virtual void OnLeftButtonUp() {
		m_pressed = false;
	}

	void buildLocator() {
		m_pressed = false;
		// Create the tree
		cellLocator = vtkSmartPointer<vtkCellLocator>::New();
		cellLocator->SetDataSet(outputPolyData);
		cellLocator->BuildLocator();
	}

	virtual void OnRightButtonDown() {
		vtkInteractorStyleTrackballCamera::OnRightButtonDown();
	}

	void CalculateBounds(double position[3]) {
		vtkSmartPointer<vtkSphereSource> sphereSource =
			vtkSmartPointer<vtkSphereSource>::New();
		sphereSource->SetCenter(position);
		sphereSource->SetRadius(2.0);
		sphereSource->Update();
		sphereSource->GetOutput()->GetBounds(bounds);
	}

	void CalculateDevicePoints(int mx,int my,int radius) {
		int x1 = mx - radius;
		int x2 = mx + radius;
		int y1 = my - radius;
		int y2 = my + radius;

		for (int _x = x1, deltax = -radius; _x <= x2; _x+=2, deltax++)
		{
			for (int _y = y1, deltay = -radius; _y <= y2; _y+=2, deltay++)
			{
				float dist = sqrtf(deltax*deltax + deltay*deltay);
				if (dist <= radius)
				{
					// this Point(_x,_y) can be picked 
					PaintOnThisPosition(_x, _y);
				}
			}
		}
		vtkRenderWindowInteractor* I = this->Interactor;
		colors->Modified();
		I->GetRenderWindow()->Render();
	}

	void PaintOnThisPosition(int device_x, int device_y) {
		vtkRenderWindowInteractor* I = this->Interactor;
		double device_position[2] = { device_x, device_y };
		vtkRenderer* ren = I->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
		I->GetPicker()->Pick(device_x, device_y, 0, ren);
		double picked[3];
		I->GetPicker()->GetPickPosition(picked);

		//Find the closest points to TestPoint
		double closestPoint[3];//the coordinates of the closest point will be returned here
		double closestPointDist2; //the squared distance to the closest point will be returned here
		vtkIdType cellId; //the cell id of the cell containing the closest point will be returned here
		int subId; //this is rarely used (in triangle strips only, I believe)
		cellLocator->FindClosestPoint(picked, closestPoint, cellId, subId, closestPointDist2);
		unsigned char color[3];
		color[0] = static_cast<unsigned char>(0);
		color[1] = static_cast<unsigned char>(255);
		color[2] = static_cast<unsigned char>(0);

		vtkSmartPointer<vtkIdList> cellPointIds =
			vtkSmartPointer<vtkIdList>::New();
		outputPolyData->GetCellPoints(cellId, cellPointIds);
		for (int i = 0; i < cellPointIds->GetNumberOfIds(); i++) {
			vtkIdType id = cellPointIds->GetId(i);
			colors->SetTupleValue(id, color);
		}
		
	}


	vtkSmartPointer<vtkCellLocator> cellLocator;
	bool m_pressed;
	double bounds[6];

};
vtkStandardNewMacro(MouseInteractorStyle)



int main(int, char *[])
{
	// Create a grid of points (height/terrian map)
	vtkSmartPointer<vtkPoints> points;
	points = vtkSmartPointer<vtkPoints>::New();

	unsigned int GridSize = 200;
	double xx, yy, zz;
	for (unsigned int x = 0; x < GridSize; x++)
	{
		for (unsigned int y = 0; y < GridSize; y++)
		{
			xx = x + vtkMath::Random(-.2, .2);
			yy = y + vtkMath::Random(-.2, .2);
			zz = vtkMath::Random(-.5, .5);
			points->InsertNextPoint(xx, yy, zz);
		}
	}

	// Add the grid points to a polydata object
	vtkSmartPointer<vtkPolyData> inputPolyData =
		vtkSmartPointer<vtkPolyData>::New();
	inputPolyData->SetPoints(points);

	// Triangulate the grid points
	vtkSmartPointer<vtkDelaunay2D> delaunay =
		vtkSmartPointer<vtkDelaunay2D>::New();
	delaunay->SetInputData(inputPolyData);
	delaunay->Update();
	outputPolyData = delaunay->GetOutput();

	double bounds[6];
	outputPolyData->GetBounds(bounds);

	// Find min and max z
	double minz = bounds[4];
	double maxz = bounds[5];

	std::cout << "minz: " << minz << std::endl;
	std::cout << "maxz: " << maxz << std::endl;

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

	std::cout << "There are " << outputPolyData->GetNumberOfPoints()
		<< " points." << std::endl;

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
	renderer->SetBackground(.1, .2, .3);

	renderWindow->SetSize(600, 600);
	// Render and interact
	renderWindow->Render();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}