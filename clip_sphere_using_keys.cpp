
#include "stdafx.h"

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL)
VTK_MODULE_INIT(vtkRenderingFreeType);

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkSuperquadricSource.h>
#include <vtkClipPolyData.h>
#include <vtkPlane.h>
#include <vtkProperty.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSphereSource.h>
#include <vtkCallbackCommand.h>
#include <string>
using namespace std;

vtkSmartPointer<vtkPlane> clipPlane;

void KeypressCallbackFunction(
	vtkObject* caller,
	long unsigned int eventId,
	void* clientData,
	void* callData);

void KeypressCallbackFunction(vtkObject* caller, long unsigned int vtkNotUsed(eventId), void* vtkNotUsed(clientData), void* vtkNotUsed(callData))
{
	vtkRenderWindowInteractor *iren =
		static_cast<vtkRenderWindowInteractor*>(caller);

	string keysym(iren->GetKeySym());
	if (keysym.compare("Up") == 0) {
		double origin[3];
		double inc[3] = { 0.1, 0, 0 };
		clipPlane->GetOrigin(origin);
		vtkMath::Add(origin, inc, origin);
		clipPlane->SetOrigin(origin);
		clipPlane->Modified();
		iren->GetRenderWindow()->Render();
	}
	else {
		double origin[3];
		double inc[3] = { -0.1, 0, 0 };
		clipPlane->GetOrigin(origin);
		vtkMath::Add(origin, inc, origin);
		clipPlane->SetOrigin(origin);
		clipPlane->Modified();
		iren->GetRenderWindow()->Render();
	}
}

int main(int, char *[])
{
	// Create a sphere
	vtkSmartPointer<vtkSphereSource> sphereSource =
		vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetCenter(0.0, 0.0, 0.0);
	sphereSource->SetRadius(15.0);
	sphereSource->SetThetaResolution(50);
	sphereSource->SetPhiResolution(50);

	// Define a clipping plane
	clipPlane =
		vtkSmartPointer<vtkPlane>::New();
	clipPlane->SetNormal(1.0, 0, 0);
	clipPlane->SetOrigin(0.0, 0.0, 0.0);

	// Clip the source with the plane
	vtkSmartPointer<vtkClipPolyData> clipper =
		vtkSmartPointer<vtkClipPolyData>::New();
	clipper->SetInputConnection(sphereSource->GetOutputPort());
	clipper->SetClipFunction(clipPlane);

	//Create a mapper and actor
	vtkSmartPointer<vtkPolyDataMapper> superquadricMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	superquadricMapper->SetInputConnection(clipper->GetOutputPort());

	vtkSmartPointer<vtkActor> superquadricActor =
		vtkSmartPointer<vtkActor>::New();
	superquadricActor->SetMapper(superquadricMapper);

	// Create a property to be used for the back faces. Turn off all
	// shading by specifying 0 weights for specular and diffuse. Max the
	// ambient.
	vtkSmartPointer<vtkProperty> backFaces =
		vtkSmartPointer<vtkProperty>::New();
	backFaces->SetSpecular(0.0);
	backFaces->SetDiffuse(0.0);
	backFaces->SetAmbient(1.0);
	backFaces->SetAmbientColor(1.0000, 0.3882, 0.2784);

	superquadricActor->SetBackfaceProperty(backFaces);

	// Create a renderer
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();

	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->SetWindowName("SolidClip");
	renderWindow->SetSize(1200, 800);

	renderWindow->AddRenderer(renderer);

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	//Add actors to the renderers
	renderer->AddActor(superquadricActor);
	renderWindow->Render();

	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	renderWindowInteractor->SetInteractorStyle(style);

	vtkSmartPointer<vtkCallbackCommand> keypressCallback =
		vtkSmartPointer<vtkCallbackCommand>::New();
	keypressCallback->SetCallback(KeypressCallbackFunction);
	renderWindowInteractor->AddObserver(vtkCommand::KeyPressEvent, keypressCallback);

	//Interact with the window
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}