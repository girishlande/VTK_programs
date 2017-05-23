#include "stdafx.h"

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL)
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

vtkSmartPointer<vtkRenderer> renderer;

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

void AddCS1() {
	double center[3] = { 0, 0, 0 };
	double pt1[3] = { 10, 0, 0 };
	double pt2[3] = { 0, 10, 0 };
	double pt3[3] = { 0, 0, 10 };

	double color[3] = { 1, 0, 0 };
	AddLine(center, pt1,color);
	AddLine(center, pt2,color);
	AddLine(center, pt3,color);
}

void AddCS2() {
	double center[3] = { 10, 30, 40 };
	double pt1[3] = { 0, 10, 10 };
	double pt2[3] = { 32, 10, 10 };
	double pt3[3] = { 50, 10, 0 };

	double v1[3] = { 1.0, 1.0, 0 };
	double v2[3];
	double v3[3];
	vtkMath::Perpendiculars(v1, v2, v3, 45);
	vtkMath::Normalize(v1);
	vtkMath::Normalize(v2);
	vtkMath::Normalize(v3);

	double length = 10;
	vtkMath::MultiplyScalar(v1, length);
	vtkMath::MultiplyScalar(v2, length);
	vtkMath::MultiplyScalar(v3, length);

	vtkMath::Add(center, v1, pt1);
	vtkMath::Add(center, v2, pt2);
	vtkMath::Add(center, v3, pt3);

	double color[3] = { 0, 1, 0 };
	AddLine(center, pt1, color);
	AddLine(center, pt2, color);
	AddLine(center, pt3, color);
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

	// Create spheres for start and end point
	vtkSmartPointer<vtkSphereSource> sphereStartSource =
		vtkSmartPointer<vtkSphereSource>::New();
	sphereStartSource->SetCenter(startPoint);
	vtkSmartPointer<vtkPolyDataMapper> sphereStartMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	sphereStartMapper->SetInputConnection(sphereStartSource->GetOutputPort());
	vtkSmartPointer<vtkActor> sphereStart =
		vtkSmartPointer<vtkActor>::New();
	sphereStart->SetMapper(sphereStartMapper);
	sphereStart->GetProperty()->SetColor(1.0, 1.0, .3);

	vtkSmartPointer<vtkSphereSource> sphereEndSource =
		vtkSmartPointer<vtkSphereSource>::New();
	sphereEndSource->SetCenter(endPoint);
	vtkSmartPointer<vtkPolyDataMapper> sphereEndMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	sphereEndMapper->SetInputConnection(sphereEndSource->GetOutputPort());
	vtkSmartPointer<vtkActor> sphereEnd =
		vtkSmartPointer<vtkActor>::New();
	sphereEnd->SetMapper(sphereEndMapper);
	sphereEnd->GetProperty()->SetColor(1.0, .3, .3);

	//Create a renderer, render window, and interactor
	renderer =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	AddCS1();
	AddCS2();

	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	renderWindowInteractor->SetInteractorStyle(style);

	//Render and interact
	renderWindow->Render();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}
