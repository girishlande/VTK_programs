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
#include <vtkTransformFilter.h>

#include <iostream>
using namespace std;

#define vtkNew(type, name)\
    vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

vtkSmartPointer<vtkRenderer> renderer;

// Add line in graphics renderer for given 2 points with given color 
void AddLine(double pt1[3], double pt2[3],double color[3])
{
	vtkNew(vtkLineSource, lineSource);
	lineSource->SetPoint1(pt1);
	lineSource->SetPoint2(pt2);
	lineSource->Update();

	// Visualize
	vtkNew(vtkPolyDataMapper, mapper);
	mapper->SetInputConnection(lineSource->GetOutputPort());
	vtkNew(vtkActor, actor);
	actor->SetMapper(mapper);
	actor->GetProperty()->SetLineWidth(4);
	actor->GetProperty()->SetColor(color);
	renderer->AddActor(actor);
}

// Transform given 2 points with given transformation 
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
	renderWindow->SetSize(800, 800);
	renderWindow->AddRenderer(renderer);
	vtkNew(vtkRenderWindowInteractor, renderWindowInteractor);
	renderWindowInteractor->SetRenderWindow(renderWindow);

	// Define center point and axis vector of coordinate system 
	double center[3] = { 0, 0, 0 };
	double pt1[3] = { 10, 0, 0 };
	double pt2[3] = { 0, 10, 0 };
	double pt3[3] = { 0, 0, 10 };

	// Define colors 
	double red[] = { 1, 0, 0 };
	double green[] = { 0, 1, 0 };
	double blue[] = { 0, 0, 1 };
	double yellow[] = { 1, 1, 0 };

	// Add 3 lines corresponding to each axis of the coordinate system 
	AddLine(center, pt1,red);
	AddLine(center, pt2,green);
	AddLine(center, pt3,blue);

	// Rotate about the center of the image
	vtkNew(vtkTransform, T1);

	double angle = 45;
	T1->Translate(center[0], center[1], center[2]);
	T1->RotateWXYZ(angle, pt3[0],pt3[1],pt3[2]);
	T1->Translate(-center[0], -center[1], -center[2]);
	
	vtkNew(vtkTransform, T2);
	angle *= -1;
	T2->Translate(center[0], center[1], center[2]);
	T2->RotateWXYZ(angle, pt2[0], pt2[1], pt2[2]);
	T2->Translate(-center[0], -center[1], -center[2]);

	double pt4[3] = { 0, 0, 0 };
	double pt5[3] = { 10, 0, 0 }; // x 
	
	TransformPoints(pt4, pt5, T1);
	TransformPoints(pt4, pt5, T2);
	AddLine(pt4, pt5, yellow);

	cout << endl << "Output points:";
	cout << endl << "(" << pt4[0] << "," << pt4[1] << "," << pt4[2] << ")";
	cout << endl << "(" << pt5[0] << "," << pt5[1] << "," << pt5[2] << ")";

	vtkNew(vtkInteractorStyleTrackballCamera, style);
	renderWindowInteractor->SetInteractorStyle(style);
	
	//Render and interact
	renderWindow->Render();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}
