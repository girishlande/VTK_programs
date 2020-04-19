
// #include "stdafx.h"

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType);

#include <vtkSmartPointer.h>
// For the rendering pipeline setup:
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
// For vtkBoxWidget2:
#include <vtkBoxWidget2.h>
#include <vtkBoxRepresentation.h>
#include <vtkCommand.h>
#include <vtkTransform.h>

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
#include <vtkMath.h>

#define vtkNew(type, name)\
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

vtkSmartPointer<vtkPlane> clipPlane1;
vtkSmartPointer<vtkPlane> clipPlane2;
vtkSmartPointer<vtkPlane> clipPlane3;
vtkSmartPointer<vtkPlane> clipPlane4;
vtkSmartPointer<vtkPlane> clipPlane5;
vtkSmartPointer<vtkPlane> clipPlane6;
vtkRenderWindowInteractor* iren;

class vtkBoxCallback : public vtkCommand
{
public:
	static vtkBoxCallback *New()
	{
		return new vtkBoxCallback;
	}

	vtkSmartPointer<vtkActor> m_actor;

	void SetActor(vtkSmartPointer<vtkActor> actor)
	{
		m_actor = actor;
	}

	virtual void Execute(vtkObject *caller, unsigned long, void*)
	{
		vtkSmartPointer<vtkBoxWidget2> boxWidget =
			vtkBoxWidget2::SafeDownCast(caller);

		vtkSmartPointer<vtkTransform> t =
			vtkSmartPointer<vtkTransform>::New();

		vtkBoxRepresentation* r = vtkBoxRepresentation::SafeDownCast(boxWidget->GetRepresentation());
		r->GetTransform(t);

		vtkNew(vtkPolyData, polydata);
		r->GetPolyData(polydata);
		double pt1[3];
		polydata->GetPoint(8, pt1);
		double pt2[3];
		polydata->GetPoint(9, pt2);
		double pt3[3];
		polydata->GetPoint(10, pt3);
		double pt4[3];
		polydata->GetPoint(11, pt4);
		double pt5[3];
		polydata->GetPoint(12, pt5);
		double pt6[3];
		polydata->GetPoint(13, pt6);
	
		clipPlane1->SetOrigin(pt1);
		clipPlane2->SetOrigin(pt2);
		clipPlane3->SetOrigin(pt3);
		clipPlane4->SetOrigin(pt4);
		clipPlane5->SetOrigin(pt5);
		clipPlane6->SetOrigin(pt6);
		clipPlane6->Modified();
		iren->GetRenderWindow()->Render();
	}

	vtkBoxCallback(){}
};

int main(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
	vtkSmartPointer<vtkSphereSource> spheresource =
		vtkSmartPointer<vtkSphereSource>::New();
	spheresource->SetRadius(10);
	spheresource->SetPhiResolution(30);
	spheresource->SetThetaResolution(30);

	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(spheresource->GetOutputPort());

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	//renderer->AddActor(actor);
	renderer->ResetCamera(); // Reposition camera so the whole scene is visible

	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->SetSize(800, 800);
	renderWindow->AddRenderer(renderer);

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	// Use the "trackball camera" interactor style, rather than the default "joystick camera"
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	renderWindowInteractor->SetInteractorStyle(style);

	vtkSmartPointer<vtkBoxWidget2> boxWidget =
		vtkSmartPointer<vtkBoxWidget2>::New();
	boxWidget->SetInteractor(renderWindowInteractor);
	boxWidget->GetRepresentation()->SetPlaceFactor(1); // Default is 0.5
	boxWidget->GetRepresentation()->PlaceWidget(actor->GetBounds());
	
	

	// Set up a callback for the interactor to call so we can manipulate the actor
	vtkSmartPointer<vtkBoxCallback> boxCallback =
		vtkSmartPointer<vtkBoxCallback>::New();
	boxCallback->SetActor(actor);
	boxWidget->AddObserver(vtkCommand::InteractionEvent, boxCallback);

	boxWidget->On();

	vtkBoxRepresentation* r = vtkBoxRepresentation::SafeDownCast(boxWidget->GetRepresentation());
	vtkNew(vtkPolyData, polydata);
	r->GetPolyData(polydata);
	double pt1[3];
	polydata->GetPoint(8, pt1);
	double pt2[3];
	polydata->GetPoint(9, pt2);
	double pt3[3];
	polydata->GetPoint(10, pt3);
	double pt4[3];
	polydata->GetPoint(11, pt4);
	double pt5[3];
	polydata->GetPoint(12, pt5);
	double pt6[3];
	polydata->GetPoint(13, pt6);

	cout << "\npt1: " << pt1[0] << " " << pt1[1] << " " << pt1[2];
	cout << "\npt2: " << pt2[0] << " " << pt2[1] << " " << pt2[2];
	cout << "\npt3: " << pt3[0] << " " << pt3[1] << " " << pt3[2];
	cout << "\npt4: " << pt4[0] << " " << pt4[1] << " " << pt4[2];
	cout << "\npt5: " << pt5[0] << " " << pt5[1] << " " << pt5[2];
	cout << "\npt6: " << pt6[0] << " " << pt6[1] << " " << pt6[2];

	double dir12[3];
	vtkMath::Subtract(pt2, pt1, dir12);
	vtkMath::Normalize(dir12);

	// Define a clipping plane
	clipPlane1 =
		vtkSmartPointer<vtkPlane>::New();
	clipPlane1->SetNormal(dir12);
	clipPlane1->SetOrigin(pt1);

	// Define a clipping plane
	vtkMath::MultiplyScalar(dir12, -1);
	clipPlane2 =
		vtkSmartPointer<vtkPlane>::New();
	clipPlane2->SetNormal(dir12);
	clipPlane2->SetOrigin(pt2);

	double dir34[3];
	vtkMath::Subtract(pt4, pt3, dir34);
	vtkMath::Normalize(dir34);

	// Define a clipping plane
	clipPlane3 =
		vtkSmartPointer<vtkPlane>::New();
	clipPlane3->SetNormal(dir34);
	clipPlane3->SetOrigin(pt3);

	// Define a clipping plane
	vtkMath::MultiplyScalar(dir34, -1);
	clipPlane4 =
		vtkSmartPointer<vtkPlane>::New();
	clipPlane4->SetNormal(dir34);
	clipPlane4->SetOrigin(pt4);

	double dir56[3];
	vtkMath::Subtract(pt6, pt5, dir56);
	vtkMath::Normalize(dir56);

	// Define a clipping plane
	clipPlane5 =
		vtkSmartPointer<vtkPlane>::New();
	clipPlane5->SetNormal(dir56);
	clipPlane5->SetOrigin(pt5);

	// Define a clipping plane
	vtkMath::MultiplyScalar(dir56, -1);
	clipPlane6 =
		vtkSmartPointer<vtkPlane>::New();
	clipPlane6->SetNormal(dir56);
	clipPlane6->SetOrigin(pt6);

	// Clip the source with the plane
	vtkSmartPointer<vtkClipPolyData> clipper1 =
		vtkSmartPointer<vtkClipPolyData>::New();
	clipper1->SetInputConnection(spheresource->GetOutputPort());
	clipper1->SetClipFunction(clipPlane1);

	// Clip the source with the plane
	vtkSmartPointer<vtkClipPolyData> clipper2 =
		vtkSmartPointer<vtkClipPolyData>::New();
	clipper2->SetInputConnection(clipper1->GetOutputPort());
	clipper2->SetClipFunction(clipPlane2);

	// Clip the source with the plane
	vtkSmartPointer<vtkClipPolyData> clipper3 =
		vtkSmartPointer<vtkClipPolyData>::New();
	clipper3->SetInputConnection(clipper2->GetOutputPort());
	clipper3->SetClipFunction(clipPlane3);

	// Clip the source with the plane
	vtkSmartPointer<vtkClipPolyData> clipper4 =
		vtkSmartPointer<vtkClipPolyData>::New();
	clipper4->SetInputConnection(clipper3->GetOutputPort());
	clipper4->SetClipFunction(clipPlane4);

	// Clip the source with the plane
	vtkSmartPointer<vtkClipPolyData> clipper5 =
		vtkSmartPointer<vtkClipPolyData>::New();
	clipper5->SetInputConnection(clipper4->GetOutputPort());
	clipper5->SetClipFunction(clipPlane5);

	// Clip the source with the plane
	vtkSmartPointer<vtkClipPolyData> clipper6 =
		vtkSmartPointer<vtkClipPolyData>::New();
	clipper6->SetInputConnection(clipper5->GetOutputPort());
	clipper6->SetClipFunction(clipPlane6);


	//Create a mapper and actor
	vtkSmartPointer<vtkPolyDataMapper> superquadricMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	superquadricMapper->SetInputConnection(clipper6->GetOutputPort());

	vtkSmartPointer<vtkActor> superquadricActor =
		vtkSmartPointer<vtkActor>::New();
	superquadricActor->SetMapper(superquadricMapper);

	renderer->AddActor(superquadricActor);
	renderer->ResetCamera();

	iren = renderWindowInteractor;
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}