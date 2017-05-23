#include "stdafx.h"

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL)
VTK_MODULE_INIT(vtkRenderingFreeType)

#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include <vtkLine.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

int main(int, char *[])
{
	vtkSmartPointer<vtkPoints> points =
		vtkSmartPointer<vtkPoints>::New();
	points->SetNumberOfPoints(4);
	points->SetPoint(0, 90, 90, 0);
	points->SetPoint(1, 90, 120, 0);
	points->SetPoint(2, 120, 120, 0);
	points->SetPoint(3, 120, 90, 0);
	vtkSmartPointer<vtkCellArray> lines =
		vtkSmartPointer<vtkCellArray>::New();
	lines->InsertNextCell(5);
	lines->InsertCellPoint(0);
	lines->InsertCellPoint(1);
	lines->InsertCellPoint(2);
	lines->InsertCellPoint(3);
	lines->InsertCellPoint(0);
	vtkSmartPointer<vtkPolyData> polyData =
		vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(points);
	polyData->SetLines(lines);
	// Setup actor and mapper
	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();

	mapper->SetInputData(polyData);

	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	// Setup render window, renderer, and interactor
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);
	renderer->AddActor(actor);

	renderWindow->Render();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}