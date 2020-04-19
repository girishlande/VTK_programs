
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType);
//VTK_MODULE_INIT(vtkInteractionStyle)
//VTK_MODULE_INIT(vtkDomainsChemistryOpenGL2)
//VTK_MODULE_INIT(vtkRenderingContextOpenGL2)
//VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)

#include <vtkSmartPointer.h>

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkFollower.h>
#include <vtkVectorText.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkFollower.h>
#include <vtkTransform.h>
#include <vtkPolyDataMapper.h>
#include <vtkVectorText.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkPlaneSource.h>
#define vtkNew(type, name)\
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()
int main(int, char *[])
{
	// Create a renderer, render window, and interactor
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	vtkNew(vtkFollower, xLabel);
	//vtkNew(vtkActor, xLabel);

	vtkNew(vtkVectorText, xText);
	vtkNew(vtkPolyDataMapper, xTextMapper);
	xText->SetText("Hello");

	vtkNew(vtkTransform, transform);
	transform->RotateZ(90.0);

	vtkNew(vtkTransformPolyDataFilter, filter);
	filter->SetInputConnection(xText->GetOutputPort());
	filter->SetTransform(transform);
	filter->Update();

	double position[3] = { 0, 0, 0 };
	xTextMapper->SetInputConnection(filter->GetOutputPort());
	xLabel->SetMapper(xTextMapper);
	xLabel->SetCamera(renderer->GetActiveCamera());
	xLabel->SetPosition(position);
	xLabel->PickableOff();
	xLabel->GetProperty()->SetColor(1, 0, 0);


	renderer->AddActor(xLabel);

	// Create a plane
	vtkSmartPointer<vtkPlaneSource> planeSource =
		vtkSmartPointer<vtkPlaneSource>::New();
	planeSource->SetCenter(0.0, 0.0, 0.0);
	planeSource->SetNormal(0.0, 0.0, 1.0);
	planeSource->Update();

	vtkPolyData* plane = planeSource->GetOutput();

	// Create a mapper and actor
	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(plane);

	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	renderer->AddActor(actor);

	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	renderWindowInteractor->SetInteractorStyle(style);

	renderer->ResetCamera();
	// Render and interact

	renderWindow->SetSize(800, 800);
	renderWindow->Render();
	renderer->ResetCamera();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}