// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

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

// Define interaction style
class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
	static MouseInteractorStyle* New();
	vtkTypeMacro(MouseInteractorStyle, vtkInteractorStyleTrackballCamera);

	virtual void OnLeftButtonDown()
	{

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

		vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
	}

	void AddMarker()
	{
		vtkSmartPointer<vtkSphereSource> sphereSource =
			vtkSmartPointer<vtkSphereSource>::New();
		sphereSource->SetRadius(10);
		sphereSource->SetPhiResolution(50);
		sphereSource->SetThetaResolution(50);

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

	double pick_position[3];
};
vtkStandardNewMacro(MouseInteractorStyle);

int main(int, char *[])
{
	vtkObject::GlobalWarningDisplayOff();

	vtkSmartPointer<vtkSphereSource> sphereSource =
		vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetRadius(100);
	sphereSource->SetPhiResolution(50);
	sphereSource->SetThetaResolution(50);
	sphereSource->SetCenter(0, 0, -100);
	sphereSource->Update();

	vtkSmartPointer<vtkWorldPointPicker> worldPointPicker =
		vtkSmartPointer<vtkWorldPointPicker>::New();

	// Create a mapper and actor
	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphereSource->GetOutputPort());
	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

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

	// Add the actor to the scene
	renderer->AddActor(actor);
	renderer->SetBackground(0, 0, 0); // Background color white

	// Render and interact
	renderWindow->Render();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}