
#include "stdafx.h"

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
#define USER_MATRIX
#include <vtkArrowSource.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkMath.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <time.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLineSource.h>
#include <vtkArcSource.h>

#define vtkNew(type, name)\
    vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

vtkSmartPointer<vtkRenderer> renderer;

void Add2DReamer()
{
	// Create two points, P0 and P1
	double r = 10.0;
	double d = r * 2;
	double p0[3] = { r*-1, 0.0, 0.0 };
	double p1[3] = { r, 0.0, 0.0 };
	double p2[3] = { 0, r, 0 };
	double p3[3] = { 0, r*-3, 0 };
	double p4[3] = { r*-2, r, 0 };
	double p5[3] = { r * 2, r, 0 };

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
}

void Add2DReamer2(double center[3],double normal[3],double up_vec[3],double radius)
{
	// Create two points, P0 and P1
	double x_axis[3];
	double y_axis[3];
	double z_axis[3];
	y_axis[0] = up_vec[0];
	y_axis[1] = up_vec[1];
	y_axis[2] = up_vec[2];

	z_axis[0] = normal[0];
	z_axis[1] = normal[1];
	z_axis[2] = normal[2];
	vtkMath::Cross(y_axis, z_axis, x_axis);
	vtkMath::Normalize(x_axis);
	vtkMath::Normalize(y_axis);
	vtkMath::Normalize(z_axis);

	double p1[3] = { 0 };
	double p2[3] = { 0 };
	double p3[3] = { 0 };
	double p4[3] = { 0 };
	double p5[3] = { 0 };
	double p6[3] = { 0 };
	vtkMath::MultiplyScalar(x_axis, 2*radius);
	vtkMath::Add(center, x_axis, p1);
	vtkMath::Subtract(center, x_axis, p2);
	
	vtkMath::MultiplyScalar(y_axis, radius);
	vtkMath::Add(center, y_axis, p3);
	vtkMath::MultiplyScalar(y_axis, 4);
	vtkMath::Add(center, y_axis, p4);

	vtkMath::MultiplyScalar(x_axis, 0.5);
	vtkMath::Add(p3, x_axis, p5);
	vtkMath::Subtract(p3, x_axis, p6);

	vtkNew(vtkArcSource, arc1);
	arc1->SetCenter(p3);
	arc1->SetPoint1(p5);
	arc1->SetPoint2(center);
	arc1->SetResolution(32);
	arc1->Update();

	vtkNew(vtkArcSource, arc2);
	arc2->SetCenter(p3);
	arc2->SetPoint1(p6);
	arc2->SetPoint2(center);
	arc2->SetResolution(32);
	arc2->Update();

	vtkNew(vtkSphereSource, sphereSource);
	sphereSource->SetCenter(center);
	sphereSource->SetThetaResolution(32);
	sphereSource->SetPhiResolution(32);
	sphereSource->SetRadius(radius*0.05);

	vtkNew(vtkLineSource, lineSource1);
	lineSource1->SetPoint1(p1);
	lineSource1->SetPoint2(p2);
	lineSource1->Update();

	vtkNew(vtkLineSource, lineSource2);
	lineSource2->SetPoint1(p5);
	lineSource2->SetPoint2(p6);
	lineSource2->Update();

	vtkNew(vtkLineSource, lineSource3);
	lineSource3->SetPoint1(center);
	lineSource3->SetPoint2(p4);
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
	vtkNew(vtkPolyDataMapper, sphere_mapper);
	sphere_mapper->SetInputConnection(sphereSource->GetOutputPort());
	vtkNew(vtkActor, sphere_actor);
	sphere_actor->SetMapper(sphere_mapper);
	sphere_actor->GetProperty()->SetLineWidth(1);

	line_actor1->GetProperty()->SetColor(242.0 / 255, 140.0 / 255, 47.0 / 255);
	line_actor2->GetProperty()->SetColor(242.0 / 255, 140.0 / 255, 47.0 / 255);
	line_actor3->GetProperty()->SetColor(242.0 / 255, 140.0 / 255, 47.0 / 255);

	// Visualize
	vtkNew(vtkPolyDataMapper, arc_mapper1);
	arc_mapper1->SetInputConnection(arc1->GetOutputPort());
	vtkNew(vtkActor, arc_actor1);
	arc_actor1->SetMapper(arc_mapper1);
	arc_actor1->GetProperty()->SetLineWidth(1);
	vtkNew(vtkPolyDataMapper, arc_mapper2);
	arc_mapper2->SetInputConnection(arc2->GetOutputPort());
	vtkNew(vtkActor, arc_actor2);
	arc_actor2->SetMapper(arc_mapper2);
	arc_actor2->GetProperty()->SetLineWidth(1);
	arc_actor1->GetProperty()->SetColor(242.0 / 255, 140.0 / 255, 47.0 / 255);
	arc_actor2->GetProperty()->SetColor(242.0 / 255, 140.0 / 255, 47.0 / 255);

	renderer->AddActor(arc_actor1);
	renderer->AddActor(arc_actor2);

	renderer->AddActor(line_actor1);
	renderer->AddActor(line_actor2);
	renderer->AddActor(line_actor3);
	renderer->AddActor(sphere_actor);
	renderer->ResetCamera();
}

void AddLine(double pt1[3], double pt2[3],double color[3])
{
	double vec[3];
	vtkMath::Subtract(pt1, pt2,vec);
	vtkMath::Normalize(vec);
	vtkMath::MultiplyScalar(vec, 10);
	double new_pt2[3];
	vtkMath::Subtract(pt1, vec, new_pt2);

	vtkSmartPointer<vtkLineSource> lineSource =
		vtkSmartPointer<vtkLineSource>::New();
	lineSource->SetPoint1(pt1);
	lineSource->SetPoint2(new_pt2);
	lineSource->Update();

	// Visualize
	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(lineSource->GetOutputPort());
	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetLineWidth(4);
	actor->GetProperty()->SetColor(color);
	renderer->AddActor(actor);

}


int main(int, char *[])
{
	//Create an arrow.
	vtkSmartPointer<vtkArrowSource> arrowSource =
		vtkSmartPointer<vtkArrowSource>::New();

	// Generate a random start and end point
	double startPoint[3], endPoint[3];
#ifndef main
	vtkMath::RandomSeed(time(NULL));
#else
	vtkMath::RandomSeed(8775070);
#endif
	startPoint[0] = vtkMath::Random(-10, 10);
	startPoint[1] = vtkMath::Random(-10, 10);
	startPoint[2] = vtkMath::Random(-10, 10);
	endPoint[0] = vtkMath::Random(-10, 10);
	endPoint[1] = vtkMath::Random(-10, 10);
	endPoint[2] = vtkMath::Random(-10, 10);

	// Compute a basis
	double normalizedX[3];
	double normalizedY[3];
	double normalizedZ[3];

	// The X axis is a vector from start to end
	vtkMath::Subtract(endPoint, startPoint, normalizedX);
	double length = vtkMath::Norm(normalizedX);
	vtkMath::Normalize(normalizedX);

	// The Z axis is an arbitrary vector cross X
	double arbitrary[3];
	arbitrary[0] = vtkMath::Random(-10, 10);
	arbitrary[1] = vtkMath::Random(-10, 10);
	arbitrary[2] = vtkMath::Random(-10, 10);
	vtkMath::Cross(normalizedX, arbitrary, normalizedZ);
	vtkMath::Normalize(normalizedZ);

	// The Y axis is Z cross X
	vtkMath::Cross(normalizedZ, normalizedX, normalizedY);
	vtkSmartPointer<vtkMatrix4x4> matrix =
		vtkSmartPointer<vtkMatrix4x4>::New();

	// Create the direction cosine matrix
	matrix->Identity();
	for (unsigned int i = 0; i < 3; i++)
	{
		matrix->SetElement(i, 0, normalizedX[i]);
		matrix->SetElement(i, 1, normalizedY[i]);
		matrix->SetElement(i, 2, normalizedZ[i]);
	}

	// Apply the transforms
	vtkSmartPointer<vtkTransform> transform =
		vtkSmartPointer<vtkTransform>::New();
	transform->Translate(startPoint);
	transform->Concatenate(matrix);
	transform->Scale(length, length, length);

	// Transform the polydata
	vtkSmartPointer<vtkTransformPolyDataFilter> transformPD =
		vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformPD->SetTransform(transform);
	transformPD->SetInputConnection(arrowSource->GetOutputPort());

	//Create a mapper and actor for the arrow
	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
#ifdef USER_MATRIX
	mapper->SetInputConnection(arrowSource->GetOutputPort());
	actor->SetUserMatrix(transform->GetMatrix());
#else
	mapper->SetInputConnection(transformPD->GetOutputPort());
#endif
	actor->SetMapper(mapper);

	//Create a renderer, render window, and interactor
	renderer =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	double center[3] = { 10, 20, 30 };
	double normal[3] = { 0, 10, 11 };
	double upvec[3] = { 0, -1, 0 };
	double radius = 10;
	Add2DReamer2(center,normal,upvec,radius);

	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	renderWindowInteractor->SetInteractorStyle(style);

	//Render and interact
	renderWindow->Render();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}
