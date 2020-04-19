// #include "stdafx.h"
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType);

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
#include <vtkSTLReader.h>
#include <vtkPlaneSource.h>
#include <vtkMath.h>
#include <vtkRegularPolygonSource.h>

// Define interaction style
class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
	static MouseInteractorStyle* New();
	vtkTypeMacro(MouseInteractorStyle, vtkInteractorStyleTrackballCamera);

	virtual void OnLeftButtonDown()
	{
		if (this->Interactor->GetControlKey()) {
			if (counter >= 4) {
				// Create a plane 
				vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
				return;
			}
			std::cout << "Picking pixel: " << this->Interactor->GetEventPosition()[0] << " " << this->Interactor->GetEventPosition()[1] << std::endl;
			this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0],
				this->Interactor->GetEventPosition()[1],
				0,  // always zero.
				this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
			double picked[3];
			this->Interactor->GetPicker()->GetPickPosition(picked);
			std::cout << "Picked value: " << picked[0] << " " << picked[1] << " " << picked[2] << std::endl;

			// Forward events
			pick_position[0] = picked[0];
			pick_position[1] = picked[1];
			pick_position[2] = picked[2];

			AddMarker();

			counter++;
			if (counter == 4) {
				// Create a plane 
				//CreatePlane();
				CreateLargePlane();
				//CreatePlaneX();
				//CreatePlaneY();
			}
		}

		vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
	}

	void AddMarker()
	{
		vtkSmartPointer<vtkSphereSource> sphereSource =
			vtkSmartPointer<vtkSphereSource>::New();
		sphereSource->SetRadius(3);
		sphereSource->SetPhiResolution(50);
		sphereSource->SetThetaResolution(50);

		points->InsertNextPoint(pick_position);
		sphereSource->SetCenter(pick_position[0], pick_position[1], pick_position[2]);
		sphereSource->Update();

		// Create a mapper and actor
		vtkSmartPointer<vtkPolyDataMapper> mapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputConnection(sphereSource->GetOutputPort());
		vtkSmartPointer<vtkActor> actor =
			vtkSmartPointer<vtkActor>::New();
		actor->GetProperty()->SetColor(1, 0, 0);
		actor->SetMapper(mapper);

		// Add the actor to the scene
		this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);
	}

	void CreatePlane()
	{
		// Read points from array 
		double A[3], B[3], C[3], D[4];
		double AB[3], BC[3];
		points->GetPoint(0, A);
		points->GetPoint(1, B);
		points->GetPoint(2, C);
		points->GetPoint(3, D);
		vtkMath::Subtract(A, B, AB);
		vtkMath::Subtract(B, C, BC);
		double N[3];
		vtkMath::Cross(AB, BC, N);

		// Calculate center of plane 
		double M[3];
		M[0] = (A[0] + B[0] + C[0] + D[0]) / 4;
		M[1] = (A[1] + B[1] + C[1] + D[1]) / 4;
		M[2] = (A[2] + B[2] + C[2] + D[2]) / 4;

		// Create a plane
		vtkSmartPointer<vtkPlaneSource> planeSource =
			vtkSmartPointer<vtkPlaneSource>::New();
		planeSource->SetCenter(M);
		planeSource->SetNormal(N);
		planeSource->Update();

		vtkPolyData* plane = planeSource->GetOutput();

		// Create a mapper and actor
		vtkSmartPointer<vtkPolyDataMapper> mapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputData(plane);

		vtkSmartPointer<vtkActor> actor =
			vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);

		// Add the actor to the scene
		this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);

	}

	void CreateLargePlane()
	{
		// Read points from array 
		double A[3], B[3], C[3], D[4];
		double AB[3], BC[3];
		points->GetPoint(0, A);
		points->GetPoint(1, B);
		points->GetPoint(2, C);
		points->GetPoint(3, D);
		vtkMath::Subtract(A, B, AB);
		vtkMath::Subtract(B, C, BC);
		vtkMath::Cross(AB, BC, N1);

		// Calculate center of plane 
		M[0] = (A[0] + B[0] + C[0] + D[0]) / 4;
		M[1] = (A[1] + B[1] + C[1] + D[1]) / 4;
		M[2] = (A[2] + B[2] + C[2] + D[2]) / 4;

		vtkSmartPointer<vtkRegularPolygonSource> polygonSource =
			vtkSmartPointer<vtkRegularPolygonSource>::New();

		//polygonSource->GeneratePolygonOff();
		polygonSource->SetNumberOfSides(4);
		polygonSource->SetRadius(200);
		polygonSource->SetCenter(M);
		polygonSource->SetNormal(N1);
		polygonSource->Update();

		vtkSmartPointer<vtkPolyDataMapper> mapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputConnection(polygonSource->GetOutputPort());
		vtkSmartPointer<vtkActor> actor =
			vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);
		actor->GetProperty()->SetColor(70.0 / 255, 130.0 / 255, 180.0 / 255);

		actor->GetProperty()->SetOpacity(0.5);
		// Add the actor to the scene
		this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);
	}

	void CreatePlaneX()
	{
		vtkMath::Perpendiculars(N1, N2, NULL, 0);

		vtkSmartPointer<vtkRegularPolygonSource> polygonSource =
			vtkSmartPointer<vtkRegularPolygonSource>::New();

		//polygonSource->GeneratePolygonOff();
		polygonSource->SetNumberOfSides(4);
		polygonSource->SetRadius(200);
		polygonSource->SetCenter(M);
		polygonSource->SetNormal(N2);
		polygonSource->Update();

		vtkSmartPointer<vtkPolyDataMapper> mapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputConnection(polygonSource->GetOutputPort());
		vtkSmartPointer<vtkActor> actor =
			vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);
		actor->GetProperty()->SetColor(70.0 / 255, 130.0 / 255, 180.0 / 255);

		actor->GetProperty()->SetOpacity(0.5);
		// Add the actor to the scene
		this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);
	}

	void CreatePlaneY()
	{
		vtkMath::Cross(N1, N2, N3);

		vtkSmartPointer<vtkRegularPolygonSource> polygonSource =
			vtkSmartPointer<vtkRegularPolygonSource>::New();

		//polygonSource->GeneratePolygonOff();
		polygonSource->SetNumberOfSides(4);
		polygonSource->SetRadius(200);
		polygonSource->SetCenter(M);
		polygonSource->SetNormal(N3);
		polygonSource->Update();

		vtkSmartPointer<vtkPolyDataMapper> mapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputConnection(polygonSource->GetOutputPort());
		vtkSmartPointer<vtkActor> actor =
			vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);
		actor->GetProperty()->SetColor(70.0 / 255, 130.0 / 255, 180.0 / 255);

		actor->GetProperty()->SetOpacity(0.5);
		// Add the actor to the scene
		this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);
	}

	void initialise_counter() {
		counter = 0;
		points = vtkSmartPointer<vtkPoints>::New();
	}

	double pick_position[3];
	int counter;
	vtkSmartPointer<vtkPoints> points;

	// Define center point of plane
	double M[3];

	// Define Normal vectors of planes 
	double N1[3];
	double N2[3];
	double N3[3];
};
vtkStandardNewMacro(MouseInteractorStyle);

int main(int, char *[])
{
	vtkObject::GlobalWarningDisplayOff();

	std::string inputFilename = "D:\\Stryker\\Stldata\\pelvic_girdle_smooth.stl";

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

	vtkSmartPointer<vtkWorldPointPicker> worldPointPicker =
		vtkSmartPointer<vtkWorldPointPicker>::New();

	// Create a renderer, render window, and interactor
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->SetSize(1200, 800);
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetPicker(worldPointPicker);
	renderWindowInteractor->SetRenderWindow(renderWindow);

	vtkSmartPointer<MouseInteractorStyle> style =
		vtkSmartPointer<MouseInteractorStyle>::New();
	renderWindowInteractor->SetInteractorStyle(style);
	style->initialise_counter();

	// Add the actor to the scene
	renderer->AddActor(actor);
	renderer->SetBackground(0, 0, 0); // Background color white

	// Render and interact
	renderWindow->Render();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}