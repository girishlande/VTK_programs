
// #include "stdafx.h"

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkRenderingOpenGL2)
//VTK_MODULE_INIT(vtkDomainsChemistryOpenGL2)
//VTK_MODULE_INIT(vtkRenderingContextOpenGL2)
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL)

#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkSphere.h>
#include <vtkSampleFunction.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVolumeProperty.h>
#include <vtkCamera.h>
#include <vtkImageShiftScale.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkXMLImageDataReader.h>
#include <vtkInteractorStyleTrackballCamera.h>

static void CreateImageData(vtkImageData* im);

#include <vtkConeSource.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkSuperquadricSource.h>
#include <vtkClipPolyData.h>
#include <vtkPlane.h>
#include <vtkProperty.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSphereSource.h>
#include <vtkCallbackCommand.h>
#include <string>
using namespace std;

vtkSmartPointer<vtkPlane> clipPlane;

void KeypressCallbackFunction(vtkObject* caller, long unsigned int vtkNotUsed(eventId), void* vtkNotUsed(clientData), void* vtkNotUsed(callData))
{
	vtkRenderWindowInteractor *iren =
		static_cast<vtkRenderWindowInteractor*>(caller);

	string keysym(iren->GetKeySym());
	if (keysym.compare("Up") == 0) {
		std::cout << "\nUP";
		double origin[3];
		double inc[3] = { 4, 0, 0 };
		clipPlane->GetOrigin(origin);
		vtkMath::Add(origin, inc, origin);
		clipPlane->SetOrigin(origin);
		clipPlane->Modified();
		iren->GetRenderWindow()->Render();
	}
	else {
		std::cout << "\nDOWN";
		double origin[3];
		double inc[3] = { -4, 0, 0 };
		clipPlane->GetOrigin(origin);
		vtkMath::Add(origin, inc, origin);
		clipPlane->SetOrigin(origin);
		clipPlane->Modified();
		iren->GetRenderWindow()->Render();
	}
}

int main(int argc, char *argv[])
{
	// Create a sphere
	vtkSmartPointer<vtkSphereSource> sphereSource =
		vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetCenter(0.0, 0.0, 0.0);
	sphereSource->SetRadius(110.0);
	sphereSource->SetThetaResolution(50);
	sphereSource->SetPhiResolution(50);

	// Define a clipping plane
	clipPlane =
		vtkSmartPointer<vtkPlane>::New();
	clipPlane->SetNormal(1.0, 0, 0);
	clipPlane->SetOrigin(0.0, 0.0, 0.0);

	// Clip the source with the plane
	vtkSmartPointer<vtkClipPolyData> clipper =
		vtkSmartPointer<vtkClipPolyData>::New();
	clipper->SetInputConnection(sphereSource->GetOutputPort());
	clipper->SetClipFunction(clipPlane);

	//Create a mapper and actor
	vtkSmartPointer<vtkPolyDataMapper> superquadricMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	superquadricMapper->SetInputConnection(clipper->GetOutputPort());

	vtkSmartPointer<vtkActor> superquadricActor =
		vtkSmartPointer<vtkActor>::New();
	superquadricActor->SetMapper(superquadricMapper);


	vtkSmartPointer<vtkImageData> imageData =
		vtkSmartPointer<vtkImageData>::New();
	CreateImageData(imageData);

	vtkSmartPointer<vtkRenderWindow> renWin =
		vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderer> ren1 =
		vtkSmartPointer<vtkRenderer>::New();
	ren1->SetBackground(0.1, 0.4, 0.2);

	renWin->AddRenderer(ren1);

	renWin->SetSize(800, 800); // intentional odd and NPOT  width/height

	vtkSmartPointer<vtkRenderWindowInteractor> iren =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renWin);

	renWin->Render(); // make sure we have an OpenGL context.

	vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper =
		vtkSmartPointer<vtkSmartVolumeMapper>::New();
	volumeMapper->SetBlendModeToComposite(); // composite first
	volumeMapper->SetInputData(imageData);
	vtkSmartPointer<vtkVolumeProperty> volumeProperty =
		vtkSmartPointer<vtkVolumeProperty>::New();
	volumeProperty->ShadeOff();
	volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

	vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity =
		vtkSmartPointer<vtkPiecewiseFunction>::New();
	compositeOpacity->AddPoint(0.0, 0.0);
	compositeOpacity->AddPoint(80.0, 1.0);
	compositeOpacity->AddPoint(80.1, 0.0);
	compositeOpacity->AddPoint(255.0, 0.0);
	volumeProperty->SetScalarOpacity(compositeOpacity); // composite first.

	vtkSmartPointer<vtkColorTransferFunction> color =
		vtkSmartPointer<vtkColorTransferFunction>::New();
	color->AddRGBPoint(0.0, 0.0, 0.0, 1.0);
	color->AddRGBPoint(40.0, 1.0, 0.0, 0.0);
	color->AddRGBPoint(255.0, 1.0, 1.0, 1.0);
	volumeProperty->SetColor(color);

	vtkSmartPointer<vtkVolume> volume =
		vtkSmartPointer<vtkVolume>::New();
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);
	ren1->AddViewProp(volume);
	
	ren1->AddActor(superquadricActor);

	// Render composite. In default mode. For coverage.
	renWin->Render();

	// 3D texture mode. For coverage.
#if !defined(VTK_LEGACY_REMOVE) && !defined(VTK_OPENGL2)
//	volumeMapper->SetRequestedRenderModeToRayCastAndTexture();
#endif // VTK_LEGACY_REMOVE
	renWin->Render();

	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	iren->SetInteractorStyle(style);

	vtkSmartPointer<vtkCallbackCommand> keypressCallback =
		vtkSmartPointer<vtkCallbackCommand>::New();
	keypressCallback->SetCallback(KeypressCallbackFunction);
	iren->AddObserver(vtkCommand::KeyPressEvent, keypressCallback);

	// Software mode, for coverage. It also makes sure we will get the same
	// regression image on all platforms.
	volumeMapper->SetRequestedRenderModeToRayCast();
	ren1->ResetCamera();
	renWin->Render();
	
	iren->Start();
	return EXIT_SUCCESS;
}

void CreateImageData(vtkImageData* imageData)
{
	// Create a spherical implicit function.
	vtkSmartPointer<vtkSphere> sphere =
		vtkSmartPointer<vtkSphere>::New();
	sphere->SetRadius(20);
	sphere->SetCenter(0.0, 0.0, 0.0);

	vtkSmartPointer<vtkSampleFunction> sampleFunction =
		vtkSmartPointer<vtkSampleFunction>::New();
	sampleFunction->SetImplicitFunction(sphere);
	sampleFunction->SetOutputScalarTypeToDouble();
	sampleFunction->SetSampleDimensions(127, 127, 127); // intentional NPOT dimensions.
	sampleFunction->SetModelBounds(-100.0, 100.0, -100.0, 100.0, -100.0, 100.0);
	sampleFunction->SetCapping(false);
	sampleFunction->SetComputeNormals(true);
	sampleFunction->SetScalarArrayName("values");
	sampleFunction->Update();

	vtkDataArray* a = sampleFunction->GetOutput()->GetPointData()->GetScalars("values");
	double range[2];
	a->GetRange(range);

	vtkSmartPointer<vtkImageShiftScale> t =
		vtkSmartPointer<vtkImageShiftScale>::New();
	t->SetInputConnection(sampleFunction->GetOutputPort());

	t->SetShift(-range[0]);
	double magnitude = range[1] - range[0];
	if (magnitude == 0.0)
	{
		magnitude = 1.0;
	}
	t->SetScale(255.0 / magnitude);
	t->SetOutputScalarTypeToUnsignedChar();

	t->Update();

	imageData->ShallowCopy(t->GetOutput());
}