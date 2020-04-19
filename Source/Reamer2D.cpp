// #include "stdafx.h"

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType);
#include <vtkSmartPointer.h>
#include <vtkLineSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkArcSource.h>
#include <vtkMath.h>
#include <vtkSphereSource.h>

#define vtkNew(type, name)\
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

int main(int, char *[])
{
	// Get Basic things 
	vtkNew(vtkRenderer, renderer);
	vtkNew(vtkRenderWindow, renderWindow);
	renderWindow->AddRenderer(renderer);
	vtkNew(vtkRenderWindowInteractor, renderWindowInteractor);
	renderWindowInteractor->SetRenderWindow(renderWindow);
	vtkNew(vtkInteractorStyleTrackballCamera, style);
	renderWindowInteractor->SetInteractorStyle(style);

	// Create two points, P0 and P1
	double r = 10.0;
	double d = r * 2;
	double p0[3] = { r*-1, 0.0, 0.0 };
	double p1[3] = { r, 0.0, 0.0 };
	double p2[3] = { 0, r, 0 };
	double p3[3] = { 0, r*-3, 0 };
	double p4[3] = { r*-2, r, 0 };
	double p5[3] = { r*2, r, 0 };

	double c[3] = { 0, 0, 0 };
	double z[3] = { 0, 0, 1 };

	vtkNew(vtkSphereSource, sphereSource);
	sphereSource->SetCenter(c);
	sphereSource->SetRadius(0.2);

	vtkNew(vtkArcSource, arcSource);
	arcSource->SetPolarVector(p1);
	arcSource->SetResolution(32);
	arcSource->SetAngle(180);
	arcSource->UseNormalAndAngleOn();
	arcSource->SetNormal(z);
	arcSource->Update();

	vtkNew(vtkLineSource, lineSource1);
	lineSource1->SetPoint1(p0);
	lineSource1->SetPoint2(p1);
	lineSource1->Update();

	vtkNew(vtkLineSource, lineSource2);
	lineSource2->SetPoint1(p2);
	lineSource2->SetPoint2(p3);
	lineSource2->Update();

	vtkNew(vtkLineSource, lineSource3);
	lineSource3->SetPoint1(p4);
	lineSource3->SetPoint2(p5);
	lineSource3->Update();

	// Visualize
	vtkNew(vtkPolyDataMapper, line_mapper1);
	line_mapper1->SetInputConnection(lineSource1->GetOutputPort());
	vtkNew(vtkActor, line_actor1);
	line_actor1->SetMapper(line_mapper1);
	line_actor1->GetProperty()->SetLineWidth(1);

	// Visualize
	vtkNew(vtkPolyDataMapper, line_mapper2);
	line_mapper2->SetInputConnection(lineSource2->GetOutputPort());
	vtkNew(vtkActor, line_actor2);
	line_actor2->SetMapper(line_mapper2);
	line_actor2->GetProperty()->SetLineWidth(1);

	// Visualize
	vtkNew(vtkPolyDataMapper, line_mapper3);
	line_mapper3->SetInputConnection(lineSource3->GetOutputPort());
	vtkNew(vtkActor, line_actor3);
	line_actor3->SetMapper(line_mapper3);
	line_actor3->GetProperty()->SetLineWidth(1);

	// Visualize
	vtkNew(vtkPolyDataMapper, arc_mapper);
	arc_mapper->SetInputConnection(arcSource->GetOutputPort());
	vtkNew(vtkActor, arc_actor);
	arc_actor->SetMapper(arc_mapper);
	arc_actor->GetProperty()->SetLineWidth(1);

	// Visualize
	vtkNew(vtkPolyDataMapper, sphere_mapper);
	sphere_mapper->SetInputConnection(sphereSource->GetOutputPort());
	vtkNew(vtkActor, sphere_actor);
	sphere_actor->SetMapper(sphere_mapper);
	sphere_actor->GetProperty()->SetLineWidth(1);

	line_actor1->GetProperty()->SetColor(242.0 / 255, 140.0 / 255, 47.0 / 255);
	line_actor2->GetProperty()->SetColor(242.0 / 255, 140.0 / 255, 47.0 / 255);
	line_actor3->GetProperty()->SetColor(242.0 / 255, 140.0 / 255, 47.0 / 255);
	arc_actor->GetProperty()->SetColor(242.0 / 255, 140.0 / 255, 47.0 / 255);

	renderer->AddActor(line_actor1);
	renderer->AddActor(line_actor2);
	renderer->AddActor(line_actor3);
	renderer->AddActor(arc_actor);
	renderer->AddActor(sphere_actor);

	renderWindow->SetSize(1200, 800);
	renderWindow->Render();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}