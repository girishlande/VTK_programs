// #include "stdafx.h"

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
#include <vtkTransformFilter.h>

#include <iostream>
using namespace std;

#define vtkNew(type, name)\
    vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

vtkSmartPointer<vtkRenderer> renderer;

void AddLine(double pt1[3], double pt2[3],double color[3])
{
	vtkSmartPointer<vtkLineSource> lineSource =
		vtkSmartPointer<vtkLineSource>::New();
	lineSource->SetPoint1(pt1);
	lineSource->SetPoint2(pt2);
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

void TransformPoints(double (&pt1)[3], double (&pt2)[3], vtkSmartPointer<vtkTransform> transform)
{
	cout << endl << "Input points :";
	cout << endl << "(" << pt1[0] << "," << pt1[1] << "," << pt1[2] << ")";
	cout << endl << "(" << pt2[0] << "," << pt2[1] << "," << pt2[2] << ")";

	vtkNew(vtkPoints, data);
	data->SetDataTypeToDouble();
	data->SetNumberOfPoints(2);
	data->SetPoint(0, pt1);
	data->SetPoint(1, pt2);

	vtkNew(vtkCellArray, cell);
	cell->InsertNextCell(2);
	cell->InsertCellPoint(0);
	cell->InsertCellPoint(1);

	vtkNew(vtkPolyData, pdata);
	pdata->SetPoints(data);
	pdata->SetLines(cell);

	vtkNew(vtkTransformFilter, Tf);
	Tf->SetInputData(pdata);
	Tf->SetTransform(transform);
	Tf->Update();

	vtkPoints *Tdata = Tf->GetOutput()->GetPoints();
	double *pt = Tdata->GetPoint(0);
	pt1[0] = pt[0]; pt1[1] = pt[1]; pt1[2] = pt[2];
	pt = Tdata->GetPoint(1);
	pt2[0] = pt[0]; pt2[1] = pt[1]; pt2[2] = pt[2];

	cout << endl << "Output points:";
	cout << endl << "(" << pt1[0] << "," << pt1[1] << "," << pt1[2] << ")";
	cout << endl << "(" << pt2[0] << "," << pt2[1] << "," << pt2[2] << ")";

}

int main(int, char *[])
{
	vtkObject::GlobalWarningDisplayOff();

	//Create a renderer, render window, and interactor
	renderer =
		vtkSmartPointer<vtkRenderer>::New();
	vtkNew(vtkRenderWindow, renderWindow);
	renderWindow->AddRenderer(renderer);
	vtkNew(vtkRenderWindowInteractor, renderWindowInteractor);
	renderWindowInteractor->SetRenderWindow(renderWindow);

	double center[3] = { 0, 0, 0 };
	double pt1[3] = { 10, 0, 0 };
	double pt2[3] = { 0, 10, 0 };
	double pt3[3] = { 0, 0, 10 };

	double red[] = { 1, 0, 0 };
	double green[] = { 0, 1, 0 };
	double blue[] = { 0, 0, 1 };
	double yellow[] = { 1, 1, 0 };

	AddLine(center, pt1,red);
	AddLine(center, pt2,green);
	AddLine(center, pt3,blue);

	// Rotate about the center of the image
	double angle = 45;

	// Rotate about the center
	vtkNew(vtkTransform, T1);
	T1->Translate(center[0], center[1], center[2]);
	T1->RotateWXYZ(angle, pt3[0],pt3[1],pt3[2]);
	T1->Translate(-center[0], -center[1], -center[2]);
	double pt4[3] = { 0, 0, 0 };
	double pt5[3] = { 10, 0, 0 }; // x 

	vtkNew(vtkTransform, T2);
	T2->Translate(center[0], center[1], center[2]);
	T2->RotateWXYZ(angle, pt1[0], pt1[1], pt1[2]);
	T2->Translate(-center[0], -center[1], -center[2]);

	
	
	TransformPoints(pt4, pt5, T1);
	AddLine(pt4, pt5,yellow);

	cout << endl << "Output points:";
	cout << endl << "(" << pt4[0] << "," << pt4[1] << "," << pt4[2] << ")";
	cout << endl << "(" << pt5[0] << "," << pt5[1] << "," << pt5[2] << ")";

	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	renderWindowInteractor->SetInteractorStyle(style);
	
	//Render and interact
	renderWindow->Render();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}
