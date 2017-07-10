
#include "stdafx.h"

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType);

#include <vtkConeSource.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkProperty.h>
#include <vtkLineSource.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include "macros.h"

double m_camera_focal_length = 100;
double length = 100;
vtkSmartPointer<vtkRenderer> renderer;
vtkSmartPointer<vtkTransform> m_camera_cone_scale_xform;
void AddLine() {
	double p0[3] = { 0, 0, 0 };
	double p1[3] = { length, 0, 0 };

	vtkSmartPointer<vtkLineSource> lineSource =
		vtkSmartPointer<vtkLineSource>::New();
	lineSource->SetPoint1(p0);
	lineSource->SetPoint2(p1);
	lineSource->Update();

	// Visualize
	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(lineSource->GetOutputPort());
	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetLineWidth(4);
	renderer->AddActor(actor);
}

void BuildTransformForCameraConeScale() {

	double x_angle = 60;
	double y_angle = 45;
	double length = m_camera_focal_length;
	double x2_angle = (90 - x_angle)*PI / 180;
	double y2_angle = (90 - y_angle)*PI / 180;
	x_angle = (x_angle / 2.0)*PI / 180;
	y_angle = (y_angle / 2.0)*PI/180;

	double x_rad = (length* sin(x_angle)) / sin(x2_angle);
	double y_rad = (length * sin(y_angle)) / sin(y2_angle);
	m_camera_cone_scale_xform = vtkSmartPointer<vtkTransform>::New();
	m_camera_cone_scale_xform->Scale(length, x_rad, y_rad);
}

int main(int, char *[])
{
	//Create a cone
	vtkSmartPointer<vtkConeSource> coneSource =
		vtkSmartPointer<vtkConeSource>::New();
	coneSource->SetResolution(100);
	coneSource->SetHeight(1);
	coneSource->SetRadius(1);
	coneSource->Update();

	BuildTransformForCameraConeScale();
	vtkNew(vtkTransformFilter, transformFilter);
	transformFilter->SetInputConnection(coneSource->GetOutputPort());
	transformFilter->SetTransform(m_camera_cone_scale_xform);

	//Create a mapper and actor
	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(transformFilter->GetOutputPort());

	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetOpacity(0.5);

	//Create a renderer, render window, and interactor
	renderer =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	//Add the actors to the scene
	renderer->AddActor(actor);
	AddLine();

	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	renderWindowInteractor->SetInteractorStyle(style);


	//Render and interact
	renderWindow->SetSize(800, 800);
	renderWindow->Render();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}