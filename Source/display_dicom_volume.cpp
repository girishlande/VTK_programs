// #include "stdafx.h"

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)
#include <vtkVersion.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderer.h>
#include <vtkImageMapper.h>
#include <vtkImageResliceMapper.h>
#include <vtkImageSlice.h>
#include <vtkCamera.h>
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
#include <vtkInteractorStyleTrackballCamera.h>

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkDICOMImageReader.h>

void CreateColorImage(vtkImageData* image)
{
	image->SetDimensions(10, 10, 10);
#if VTK_MAJOR_VERSION <= 5
	image->SetScalarTypeToUnsignedChar();
	image->SetNumberOfScalarComponents(3);
	image->AllocateScalars();
#else
	image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
#endif
	for (unsigned int z = 0; z < 10; z++)
	{
		for (unsigned int x = 0; x < 10; x++)
		{
			for (unsigned int y = 0; y < 10; y++)
			{
				unsigned char* pixel = static_cast<unsigned char*>(image->GetScalarPointer(x, y, z));
				switch (z) {
				case 0:	pixel[0] = 0 ;					pixel[1] = 0;					pixel[2] = 255; break;
				case 1:	pixel[0] = 0;					pixel[1] = 0;					pixel[2] = 255; break;
				
				case 2:	pixel[0] = 255;					pixel[1] = 0;					pixel[2] = 255; break;
				case 3:	pixel[0] = 255;					pixel[1] = 0;					pixel[2] = 255; break;
				
				case 4:	pixel[0] = 0;					pixel[1] = 0;					pixel[2] = 0; break;
				case 5:	pixel[0] = 0;					pixel[1] = 0;					pixel[2] = 0; break;
				
				case 6:	pixel[0] = 0;					pixel[1] = 250;					pixel[2] = 0; break;
				case 7:	pixel[0] = 0;					pixel[1] = 250;					pixel[2] = 0; break;

				case 8:	pixel[0] = 250;					pixel[1] = 250;					pixel[2] = 0; break;
				case 9:	pixel[0] = 250;					pixel[1] = 250;					pixel[2] = 0; break;
				}
			}
		}
	}
}

int main(int, char *[])
{

	// Create a cube.
	vtkSmartPointer<vtkCubeSource> cubeSource =
		vtkSmartPointer<vtkCubeSource>::New();

	// Create a mapper and actor.
	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(cubeSource->GetOutputPort());

	vtkSmartPointer<vtkActor> sactor =
		vtkSmartPointer<vtkActor>::New();
	sactor->SetMapper(mapper);

	std::string folder = "../data/Brain";

	// Read all the DICOM files in the specified directory.
	vtkSmartPointer<vtkDICOMImageReader> reader =
		vtkSmartPointer<vtkDICOMImageReader>::New();
	reader->SetDirectoryName(folder.c_str());
	reader->Update();

	vtkImageData* colorImage = reader->GetOutput();


	//vtkSmartPointer<vtkImageData> colorImage = vtkSmartPointer<vtkImageData>::New();
	//CreateColorImage(colorImage);

	vtkSmartPointer<vtkImageResliceMapper> imageResliceMapper = vtkSmartPointer<vtkImageResliceMapper>::New();
#if VTK_MAJOR_VERSION <= 5
	imageResliceMapper->SetInputConnection(colorImage->GetProducerPort());
#else
	imageResliceMapper->SetInputData(colorImage);
#endif
	imageResliceMapper->SliceFacesCameraOn();
	imageResliceMapper->SliceAtFocalPointOn();
	imageResliceMapper->BorderOn();
	imageResliceMapper->SetSlabThickness(10);

	vtkSmartPointer<vtkImageSlice> imageSlice = vtkSmartPointer<vtkImageSlice>::New();
	imageSlice->SetMapper(imageResliceMapper);

	// Setup renderers
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->AddViewProp(imageSlice);
	renderer->ResetCamera();
	renderer->AddActor(sactor);
	renderer->SetBackground(1, 1, 1);

	// Setup render window
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->SetSize(300, 300);
	renderWindow->AddRenderer(renderer);

	// Setup render window interactor
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();

	/*vtkSmartPointer<vtkInteractorStyleImage> style =
		vtkSmartPointer<vtkInteractorStyleImage>::New();

	renderWindowInteractor->SetInteractorStyle(style);*/

	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	renderWindowInteractor->SetInteractorStyle(style);

	// Render and start interaction
	renderWindowInteractor->SetRenderWindow(renderWindow);
	renderWindowInteractor->Initialize();

	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}