
// #include "stdafx.h"

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType);

#include <vtkSmartPointer.h>
// For the rendering pipeline setup:
#include <vtkConeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
// For vtkBoxWidget2:
#include <vtkBoxWidget2.h>
#include <vtkBoxRepresentation.h>
#include <vtkCommand.h>
#include <vtkTransform.h>
#define vtkNew(type, name)\
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

class vtkBoxCallback : public vtkCommand
{
public:
	static vtkBoxCallback *New()
	{
		return new vtkBoxCallback;
	}

	vtkSmartPointer<vtkActor> m_actor;

	void SetActor(vtkSmartPointer<vtkActor> actor)
	{
		m_actor = actor;
	}

	virtual void Execute(vtkObject *caller, unsigned long, void*)
	{
		vtkSmartPointer<vtkBoxWidget2> boxWidget =
			vtkBoxWidget2::SafeDownCast(caller);

		vtkSmartPointer<vtkTransform> t =
			vtkSmartPointer<vtkTransform>::New();

		vtkBoxRepresentation::SafeDownCast(boxWidget->GetRepresentation())->GetTransform(t);
		this->m_actor->SetUserTransform(t);
	}

	vtkBoxCallback(){}
};

#include <vtkDICOMImageReader.h>
#include <vtkImagePlaneWidget.h>
#include <vtkCellPicker.h>
#include <vtkProperty.h>
#include <vtkImageData.h>
#include <vtkSTLReader.h>

vtkSmartPointer<vtkDICOMImageReader> m_dicom_reader;
vtkImageData* m_dicom_image;
vtkSmartPointer<vtkRenderer> renderer;
vtkSmartPointer<vtkImagePlaneWidget> m_3DPlaneWidget[3];
vtkRenderWindowInteractor *iren;
vtkSmartPointer<vtkRenderWindow> renderWindow;

void ReadInputDICOM()
{
	// Read all the DICOM files from User Input directory
	m_dicom_reader = vtkSmartPointer<vtkDICOMImageReader>::New();
	m_dicom_reader->SetDirectoryName("../data/Brain");
	m_dicom_reader->Update();

	// Read patient 
	m_dicom_image = m_dicom_reader->GetOutput();
}

void AddConeGraphics() {
	vtkSmartPointer<vtkConeSource> coneSource =
		vtkSmartPointer<vtkConeSource>::New();
	coneSource->SetHeight(1.5);

	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(coneSource->GetOutputPort());

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	renderer->AddActor(actor);

	vtkSmartPointer<vtkBoxWidget2> boxWidget =
		vtkSmartPointer<vtkBoxWidget2>::New();
	//boxWidget->SetInteractor(renderWindowInteractor);
	boxWidget->GetRepresentation()->SetPlaceFactor(1); // Default is 0.5
	boxWidget->GetRepresentation()->PlaceWidget(actor->GetBounds());

	// Set up a callback for the interactor to call so we can manipulate the actor
	vtkSmartPointer<vtkBoxCallback> boxCallback =
		vtkSmartPointer<vtkBoxCallback>::New();
	boxCallback->SetActor(actor);
	boxWidget->AddObserver(vtkCommand::InteractionEvent, boxCallback);

	boxWidget->On();
}

void Create3DImagePlaneWidgets()
{
	int imageDims[3];
	m_dicom_image->GetDimensions(imageDims);

	vtkNew(vtkCellPicker, picker);
	picker->SetTolerance(0.005);
	vtkNew(vtkProperty, ipwProp);

	for (int i = 0; i < 3; i++)
	{
		m_3DPlaneWidget[i] = vtkSmartPointer<vtkImagePlaneWidget>::New();
		m_3DPlaneWidget[i]->SetInteractor(renderWindow->GetInteractor());
		m_3DPlaneWidget[i]->SetPicker(picker);
		m_3DPlaneWidget[i]->RestrictPlaneToVolumeOn();
		double black[3] = { 1, 0, 0 };
		m_3DPlaneWidget[i]->GetPlaneProperty()->SetAmbientColor(black);

		m_3DPlaneWidget[i]->SetTexturePlaneProperty(ipwProp);
		m_3DPlaneWidget[i]->TextureInterpolateOff();
		m_3DPlaneWidget[i]->SetResliceInterpolateToLinear();
		m_3DPlaneWidget[i]->SetInputConnection(m_dicom_reader->GetOutputPort());
		m_3DPlaneWidget[i]->SetPlaneOrientation(i);
		m_3DPlaneWidget[i]->SetSliceIndex(imageDims[i] / 2);
		m_3DPlaneWidget[i]->DisplayTextOn();
		m_3DPlaneWidget[i]->SetDefaultRenderer(renderer);
		//m_3DPlaneWidget[i]->SetWindowLevel(1708, -709);
		m_3DPlaneWidget[i]->On();
		m_3DPlaneWidget[i]->InteractionOn();
	}
}

void AddSTL() {
	vtkNew(vtkSTLReader, reader);
	reader->SetFileName("../Data/Brain/pelvic_girdle_smooth.stl");
	reader->Update();

	vtkNew(vtkPolyDataMapper, sm);
	sm->SetInputConnection(reader->GetOutputPort());
	vtkNew(vtkActor, sa);
	sa->SetMapper(sm);
	renderer->AddActor(sa);
}


int main(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
	vtkObject::GlobalWarningDisplayOff();

	renderer =
		vtkSmartPointer<vtkRenderer>::New();
	renderer->ResetCamera(); // Reposition camera so the whole scene is visible

	renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	renderWindow->SetSize(800, 800);
	
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	// Use the "trackball camera" interactor style, rather than the default "joystick camera"
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	renderWindowInteractor->SetInteractorStyle(style);
	iren = renderWindowInteractor;
	
	ReadInputDICOM();
	AddSTL();
	Create3DImagePlaneWidgets();

	renderer->ResetCamera(); // Reposition camera so the whole scene is visible
	renderWindow->Render();

	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}