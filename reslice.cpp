#include "stdafx.h"

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
#define USER_MATRIX
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkVolume16Reader.h>
#include <vtkOutlineFilter.h>
#include <vtkImageResliceMapper.h>
#include <vtkInteractorStyleImage.h>
#include <vtkCamera.h>
#include <vtkImageProperty.h>
#include <vtkImageSlice.h>
#include <vtkProperty.h>

int main(int argc, char *argv[])
{
	if (argc < 2) {
		cout << "Usage: " << argv[0] << " DATADIR/prefix" << endl;
		return EXIT_FAILURE;
	}

	vtkSmartPointer<vtkRenderer> aRenderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
	renWin->AddRenderer(aRenderer);
	vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renWin);

	aRenderer->SetBackground(.2, .3, .4);
	renWin->SetSize(640, 480);

	// vtkVolume16Reader Setting 
	vtkSmartPointer<vtkVolume16Reader> v16 = vtkSmartPointer<vtkVolume16Reader>::New();
	v16->SetDataDimensions(426, 426);
	v16->SetImageRange(1, 318);
	v16->SetDataByteOrderToLittleEndian();
	v16->SetFilePrefix(argv[1]);
	v16->SetDataSpacing(1.0, 1.0, 1.0);
	v16->Update();

	// outline provides context around the data. 
	vtkSmartPointer<vtkOutlineFilter> outlineData = vtkSmartPointer<vtkOutlineFilter>::New();
	outlineData->SetInputConnection(v16->GetOutputPort());
	outlineData->Update();

	vtkSmartPointer<vtkPolyDataMapper> mapOutline = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapOutline->SetInputConnection(outlineData->GetOutputPort());

	vtkSmartPointer<vtkActor> outline = vtkSmartPointer<vtkActor>::New();
	outline->SetMapper(mapOutline);
	outline->GetProperty()->SetColor(0, 0, 0);

	// Setting of vtkImageResliceMapper 
	vtkSmartPointer<vtkImageResliceMapper> im = vtkSmartPointer<vtkImageResliceMapper>::New();
	im->SetInputConnection(v16->GetOutputPort());
	im->SliceFacesCameraOn();
	im->SliceAtFocalPointOn();
	im->BorderOff();

	// Setting of vtkImageProperty 
	vtkSmartPointer<vtkImageProperty> ip = vtkSmartPointer<vtkImageProperty>::New();
	ip->SetColorWindow(2000);
	ip->SetColorLevel(1000);
	ip->SetAmbient(0.0);
	ip->SetDiffuse(1.0);
	ip->SetOpacity(1.0);
	ip->SetInterpolationTypeToLinear();

	// Try of vtkImageSlice 
	vtkSmartPointer<vtkImageSlice> imageSlice = vtkSmartPointer<vtkImageSlice>::New();
	vtkSmartPointer<vtkInteractorStyleImage > styleImage = vtkSmartPointer<vtkInteractorStyleImage >::New();

	imageSlice->SetMapper(im);
	imageSlice->SetProperty(ip);

	styleImage->SetInteractionModeToImage3D();
	iren->SetInteractorStyle(styleImage);
	renWin->SetInteractor(iren);

	// Actors are added to the renderer. 
	aRenderer->AddActor(outline);
	aRenderer->AddViewProp(imageSlice);

	// camera Setting 
	vtkSmartPointer<vtkCamera> aCamera = vtkSmartPointer<vtkCamera>::New();
	aCamera->SetViewUp(0, 0, -1);
	aCamera->SetPosition(0, 1, 0);
	aCamera->SetFocalPoint(0, 0, 0);
	aCamera->ComputeViewPlaneNormal();
	aCamera->Azimuth(30.0);
	aCamera->Elevation(30.0);
	aCamera->ParallelProjectionOn();
	aRenderer->SetActiveCamera(aCamera);


	renWin->Render();
	aRenderer->ResetCamera();
	aRenderer->ResetCameraClippingRange();

	// interact with data 
	iren->Initialize();
	iren->Start();

	return EXIT_SUCCESS;
}