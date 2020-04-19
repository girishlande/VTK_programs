// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

// #include "stdafx.h"
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)


#include <vtkSmartPointer.h>
#include <vtkRendererCollection.h>
#include <vtkPointPicker.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>

// Define interaction style
class MouseInteractorStylePP : public vtkInteractorStyleTrackballCamera
{
public:
	static MouseInteractorStylePP* New();
	vtkTypeMacro(MouseInteractorStylePP, vtkInteractorStyleTrackballCamera);

	virtual void OnLeftButtonDown()
	{
		std::cout << "Picking pixel: " << this->Interactor->GetEventPosition()[0] << " " << this->Interactor->GetEventPosition()[1] << std::endl;
		this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0],
			this->Interactor->GetEventPosition()[1],
			0,  // always zero.
			this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
		double picked[3];
		this->Interactor->GetPicker()->GetPickPosition(picked);
		std::cout << "Picked value: " << picked[0] << " " << picked[1] << " " << picked[2] << std::endl;
		// Forward events
		vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
	}

};

vtkStandardNewMacro(MouseInteractorStylePP);

int main(int, char *[])
{

	vtkObject::GlobalWarningDisplayOff();

	vtkSmartPointer<vtkCylinderSource> s1 =
		vtkSmartPointer<vtkCylinderSource>::New();
	s1->SetRadius(50);
	s1->SetHeight(100);
	s1->SetResolution(50);
	s1->Update();

	vtkSmartPointer<vtkPointPicker> pointPicker =
		vtkSmartPointer<vtkPointPicker>::New();

	// Create a mapper and actor
	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(s1->GetOutputPort());
	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	// Create a renderer, render window, and interactor
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetPicker(pointPicker);
	renderWindowInteractor->SetRenderWindow(renderWindow);

	vtkSmartPointer<MouseInteractorStylePP> style =
		vtkSmartPointer<MouseInteractorStylePP>::New();
	renderWindowInteractor->SetInteractorStyle(style);

	// Add the actor to the scene
	renderer->AddActor(actor);
	renderer->SetBackground(1, 1, 1); // Background color white

	// Render and interact
	renderWindow->Render();
	renderWindowInteractor->Start();
	return EXIT_SUCCESS;
}


//#include <vtkSmartPointer.h>
//
//#include "vtkSmartPointer.h"
//#include "vtkVersion.h"
//
//#include "vtkPolyhedron.h"
//#include "vtkPolyData.h"
//
//#include "vtkPolyDataMapper.h"
//#include "vtkCamera.h"
//#include <vtkActor.h>
//#include <vtkRenderWindow.h>
//#include <vtkRenderer.h>
//#include <vtkRenderWindowInteractor.h>
//#include <vtkInteractorStyleTrackballCamera.h>
//
//vtkSmartPointer<vtkPolyhedron> MakeDodecahedron();
//
//int main(int, char *[])
//{
//	vtkSmartPointer<vtkPolyhedron> dodecahedron = MakeDodecahedron();
//
//	// Visualize
//	vtkSmartPointer<vtkPolyDataMapper> mapper =
//		vtkSmartPointer<vtkPolyDataMapper>::New();
//#if VTK_MAJOR_VERSION <= 5
//	mapper->SetInput(dodecahedron->GetPolyData());
//#else
//	mapper->SetInputData(dodecahedron->GetPolyData());
//#endif
//	vtkSmartPointer<vtkActor> actor =
//		vtkSmartPointer<vtkActor>::New();
//	actor->SetMapper(mapper);
//
//	vtkSmartPointer<vtkRenderer> renderer =
//		vtkSmartPointer<vtkRenderer>::New();
//	vtkSmartPointer<vtkRenderWindow> renderWindow =
//		vtkSmartPointer<vtkRenderWindow>::New();
//	renderWindow->AddRenderer(renderer);
//	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
//		vtkSmartPointer<vtkRenderWindowInteractor>::New();
//	renderWindowInteractor->SetRenderWindow(renderWindow);
//
//	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
//		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
//
//	renderWindowInteractor->SetInteractorStyle(style);
//
//	renderer->AddActor(actor);
//	renderer->SetBackground(.2, .3, .4);
//	renderer->GetActiveCamera()->Azimuth(30);
//	renderer->GetActiveCamera()->Elevation(30);
//
//	renderer->ResetCamera();
//
//	renderWindow->Render();
//	renderWindowInteractor->Start();
//
//	return EXIT_SUCCESS;
//}
//
//vtkSmartPointer<vtkPolyhedron>MakeDodecahedron()
//{
//	vtkSmartPointer<vtkPolyhedron> aDodecahedron =
//		vtkSmartPointer<vtkPolyhedron>::New();
//
//	for (int i = 0; i < 20; ++i)
//	{
//		aDodecahedron->GetPointIds()->InsertNextId(i);
//	}
//
//	aDodecahedron->GetPoints()->InsertNextPoint(1.21412, 0, 1.58931);
//	aDodecahedron->GetPoints()->InsertNextPoint(0.375185, 1.1547, 1.58931);
//	aDodecahedron->GetPoints()->InsertNextPoint(-0.982247, 0.713644, 1.58931);
//	aDodecahedron->GetPoints()->InsertNextPoint(-0.982247, -0.713644, 1.58931);
//	aDodecahedron->GetPoints()->InsertNextPoint(0.375185, -1.1547, 1.58931);
//	aDodecahedron->GetPoints()->InsertNextPoint(1.96449, 0, 0.375185);
//	aDodecahedron->GetPoints()->InsertNextPoint(0.607062, 1.86835, 0.375185);
//	aDodecahedron->GetPoints()->InsertNextPoint(-1.58931, 1.1547, 0.375185);
//	aDodecahedron->GetPoints()->InsertNextPoint(-1.58931, -1.1547, 0.375185);
//	aDodecahedron->GetPoints()->InsertNextPoint(0.607062, -1.86835, 0.375185);
//	aDodecahedron->GetPoints()->InsertNextPoint(1.58931, 1.1547, -0.375185);
//	aDodecahedron->GetPoints()->InsertNextPoint(-0.607062, 1.86835, -0.375185);
//	aDodecahedron->GetPoints()->InsertNextPoint(-1.96449, 0, -0.375185);
//	aDodecahedron->GetPoints()->InsertNextPoint(-0.607062, -1.86835, -0.375185);
//	aDodecahedron->GetPoints()->InsertNextPoint(1.58931, -1.1547, -0.375185);
//	aDodecahedron->GetPoints()->InsertNextPoint(0.982247, 0.713644, -1.58931);
//	aDodecahedron->GetPoints()->InsertNextPoint(-0.375185, 1.1547, -1.58931);
//	aDodecahedron->GetPoints()->InsertNextPoint(-1.21412, 0, -1.58931);
//	aDodecahedron->GetPoints()->InsertNextPoint(-0.375185, -1.1547, -1.58931);
//	aDodecahedron->GetPoints()->InsertNextPoint(0.982247, -0.713644, -1.58931);
//
//	vtkIdType faces[73] =
//	{ 12,                   // number of faces
//	5, 0, 1, 2, 3, 4,     // number of ids on face, ids
//	5, 0, 5, 10, 6, 1,
//	5, 1, 6, 11, 7, 2,
//	5, 2, 7, 12, 8, 3,
//	5, 3, 8, 13, 9, 4,
//	5, 4, 9, 14, 5, 0,
//	5, 15, 10, 5, 14, 19,
//	5, 16, 11, 6, 10, 15,
//	5, 17, 12, 7, 11, 16,
//	5, 18, 13, 8, 12, 17,
//	5, 19, 14, 9, 13, 18,
//	5, 19, 18, 17, 16, 15 };
//
//	aDodecahedron->SetFaces(faces);
//	aDodecahedron->Initialize();
//
//	return aDodecahedron;
//}

//
//#include <vtkPolyData.h>
//#include <vtkSTLReader.h>
//#include <vtkSmartPointer.h>
//#include <vtkPolyDataMapper.h>
//#include <vtkActor.h>
//#include <vtkRenderWindow.h>
//#include <vtkRenderer.h>
//#include <vtkRenderWindowInteractor.h>
//#include <vtkInteractorStyleTrackballCamera.h>
//
//int main(int argc, char *argv[])
//{
//	if (argc != 2)
//	{
//		cout << "Required parameters: Filename" << endl;
//		return EXIT_FAILURE;
//	}
//
//	std::string inputFilename = argv[1];
//
//	vtkSmartPointer<vtkSTLReader> reader =
//		vtkSmartPointer<vtkSTLReader>::New();
//	reader->SetFileName(inputFilename.c_str());
//	reader->Update();
//
//	// Visualize
//	vtkSmartPointer<vtkPolyDataMapper> mapper =
//		vtkSmartPointer<vtkPolyDataMapper>::New();
//	mapper->SetInputConnection(reader->GetOutputPort());
//
//	vtkSmartPointer<vtkActor> actor =
//		vtkSmartPointer<vtkActor>::New();
//	actor->SetMapper(mapper);
//
//	vtkSmartPointer<vtkRenderer> renderer =
//		vtkSmartPointer<vtkRenderer>::New();
//	vtkSmartPointer<vtkRenderWindow> renderWindow =
//		vtkSmartPointer<vtkRenderWindow>::New();
//	renderWindow->AddRenderer(renderer);
//	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
//		vtkSmartPointer<vtkRenderWindowInteractor>::New();
//	renderWindowInteractor->SetRenderWindow(renderWindow);
//
//	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
//		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
//
//	renderWindowInteractor->SetInteractorStyle(style);
//
//	renderer->AddActor(actor);
//	renderer->SetBackground(.3, .6, .3); // Background color green
//
//	renderWindow->Render();
//	renderWindowInteractor->Start();
//
//	return EXIT_SUCCESS;
//}

//#include <vtkSmartPointer.h>
//
//#include <vtkVersion.h>
//#include <vtkSmartPointer.h>
//#include <vtkPolyDataMapper.h>
//#include <vtkActor.h>
//#include <vtkRenderWindow.h>
//#include <vtkRenderer.h>
//#include <vtkRenderWindowInteractor.h>
//#include <vtkPolyData.h>
//#include <vtkSphereSource.h>
//#include <vtkTextActor.h>
//#include <vtkTextProperty.h>
//
//int main(int, char *[])
//{
//	// Create a sphere
//	vtkSmartPointer<vtkSphereSource> sphereSource =
//		vtkSmartPointer<vtkSphereSource>::New();
//	sphereSource->SetCenter(0.0, 0.0, 0.0);
//	sphereSource->SetRadius(5.0);
//	sphereSource->Update();
//
//	// Create a mapper
//	vtkSmartPointer<vtkPolyDataMapper> mapper =
//		vtkSmartPointer<vtkPolyDataMapper>::New();
//#if VTK_MAJOR_VERSION <= 5
//	mapper->SetInput(sphereSource->GetOutput());
//#else
//	mapper->SetInputData(sphereSource->GetOutput());
//#endif
//
//	// Create an actor
//	vtkSmartPointer<vtkActor> actor =
//		vtkSmartPointer<vtkActor>::New();
//	actor->SetMapper(mapper);
//
//	// Create a renderer
//	vtkSmartPointer<vtkRenderer> renderer =
//		vtkSmartPointer<vtkRenderer>::New();
//	renderer->SetBackground(1, 1, 1); // Set background color to white
//	renderer->AddActor(actor);
//
//	// Create a render window
//	vtkSmartPointer<vtkRenderWindow> renderWindow =
//		vtkSmartPointer<vtkRenderWindow>::New();
//	renderWindow->AddRenderer(renderer);
//
//	// Create an interactor
//	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
//		vtkSmartPointer<vtkRenderWindowInteractor>::New();
//	renderWindowInteractor->SetRenderWindow(renderWindow);
//
//	// Setup the text and add it to the renderer
//	vtkSmartPointer<vtkTextActor> textActor =
//		vtkSmartPointer<vtkTextActor>::New();
//	textActor->SetInput("Hello world");
//	textActor->SetPosition2(10, 40);
//	textActor->GetTextProperty()->SetFontSize(24);
//	textActor->GetTextProperty()->SetColor(1.0, 0.0, 0.0);
//	renderer->AddActor2D(textActor);
//
//	// Render and interact
//	renderWindow->Render();
//	renderWindowInteractor->Start();
//
//	return EXIT_SUCCESS;
//}

//#include <vtkSmartPointer.h>
//#include <vtkObjectFactory.h>
//#include <vtkRenderWindow.h>
//#include <vtkRenderWindowInteractor.h>
//#include <vtkRenderer.h>
//#include <vtkActor.h>
//// headers needed for this example
//#include <vtkImageViewer2.h>
//#include <vtkDICOMImageReader.h>
//#include <vtkInteractorStyleImage.h>
//#include <vtkActor2D.h>
//#include <vtkTextProperty.h>
//#include <vtkTextMapper.h>
//// needed to easily convert int to std::string
//#include <sstream>
//
//
//// helper class to format slice status message
//class StatusMessage {
//public:
//	static std::string Format(int slice, int maxSlice) {
//		std::stringstream tmp;
//		tmp << "Slice Number  " << slice + 1 << "/" << maxSlice + 1;
//		return tmp.str();
//	}
//};
//
//
//// Define own interaction style
//class myVtkInteractorStyleImage : public vtkInteractorStyleImage
//{
//public:
//	static myVtkInteractorStyleImage* New();
//	vtkTypeMacro(myVtkInteractorStyleImage, vtkInteractorStyleImage);
//
//protected:
//	vtkImageViewer2* _ImageViewer;
//	vtkTextMapper* _StatusMapper;
//	int _Slice;
//	int _MinSlice;
//	int _MaxSlice;
//
//public:
//	void SetImageViewer(vtkImageViewer2* imageViewer) {
//		_ImageViewer = imageViewer;
//		_MinSlice = imageViewer->GetSliceMin();
//		_MaxSlice = imageViewer->GetSliceMax();
//		_Slice = _MinSlice;
//		cout << "Slicer: Min = " << _MinSlice << ", Max = " << _MaxSlice << std::endl;
//	}
//
//	void SetStatusMapper(vtkTextMapper* statusMapper) {
//		_StatusMapper = statusMapper;
//	}
//
//
//protected:
//	void MoveSliceForward() {
//		if (_Slice < _MaxSlice) {
//			_Slice += 1;
//			cout << "MoveSliceForward::Slice = " << _Slice << std::endl;
//			_ImageViewer->SetSlice(_Slice);
//			std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
//			_StatusMapper->SetInput(msg.c_str());
//			_ImageViewer->Render();
//		}
//	}
//
//	void MoveSliceBackward() {
//		if (_Slice > _MinSlice) {
//			_Slice -= 1;
//			cout << "MoveSliceBackward::Slice = " << _Slice << std::endl;
//			_ImageViewer->SetSlice(_Slice);
//			std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
//			_StatusMapper->SetInput(msg.c_str());
//			_ImageViewer->Render();
//		}
//	}
//
//
//	virtual void OnKeyDown() {
//		std::string key = this->GetInteractor()->GetKeySym();
//		if (key.compare("Up") == 0) {
//			//cout << "Up arrow key was pressed." << endl;
//			MoveSliceForward();
//		}
//		else if (key.compare("Down") == 0) {
//			//cout << "Down arrow key was pressed." << endl;
//			MoveSliceBackward();
//		}
//		// forward event
//		vtkInteractorStyleImage::OnKeyDown();
//	}
//
//
//	virtual void OnMouseWheelForward() {
//		//std::cout << "Scrolled mouse wheel forward." << std::endl;
//		MoveSliceForward();
//		// don't forward events, otherwise the image will be zoomed 
//		// in case another interactorstyle is used (e.g. trackballstyle, ...)
//		// vtkInteractorStyleImage::OnMouseWheelForward();
//	}
//
//
//	virtual void OnMouseWheelBackward() {
//		//std::cout << "Scrolled mouse wheel backward." << std::endl;
//		if (_Slice > _MinSlice) {
//			MoveSliceBackward();
//		}
//		// don't forward events, otherwise the image will be zoomed 
//		// in case another interactorstyle is used (e.g. trackballstyle, ...)
//		// vtkInteractorStyleImage::OnMouseWheelBackward();
//	}
//};
//
//vtkStandardNewMacro(myVtkInteractorStyleImage);
//
//
//int main(int argc, char* argv[])
//{
//	// Verify input arguments
//	if (argc != 2)
//	{
//		std::cout << "Usage: " << argv[0]
//			<< " FolderName" << std::endl;
//		return EXIT_FAILURE;
//	}
//
//	std::string folder = argv[1];
//	//std::string folder = "C:\\VTK\\vtkdata-5.8.0\\Data\\DicomTestImages";
//
//	// Read all the DICOM files in the specified directory.
//	vtkSmartPointer<vtkDICOMImageReader> reader =
//		vtkSmartPointer<vtkDICOMImageReader>::New();
//	reader->SetDirectoryName(folder.c_str());
//	reader->Update();
//
//	// Visualize
//	vtkSmartPointer<vtkImageViewer2> imageViewer =
//		vtkSmartPointer<vtkImageViewer2>::New();
//	imageViewer->SetInputConnection(reader->GetOutputPort());
//
//	// slice status message
//	vtkSmartPointer<vtkTextProperty> sliceTextProp = vtkSmartPointer<vtkTextProperty>::New();
//	sliceTextProp->SetFontFamilyToCourier();
//	sliceTextProp->SetFontSize(20);
//	sliceTextProp->SetVerticalJustificationToBottom();
//	sliceTextProp->SetJustificationToLeft();
//
//	vtkSmartPointer<vtkTextMapper> sliceTextMapper = vtkSmartPointer<vtkTextMapper>::New();
//	std::string msg = StatusMessage::Format(imageViewer->GetSliceMin(), imageViewer->GetSliceMax());
//	sliceTextMapper->SetInput(msg.c_str());
//	sliceTextMapper->SetTextProperty(sliceTextProp);
//
//	vtkSmartPointer<vtkActor2D> sliceTextActor = vtkSmartPointer<vtkActor2D>::New();
//	sliceTextActor->SetMapper(sliceTextMapper);
//	sliceTextActor->SetPosition(15, 10);
//
//	// usage hint message
//	vtkSmartPointer<vtkTextProperty> usageTextProp = vtkSmartPointer<vtkTextProperty>::New();
//	usageTextProp->SetFontFamilyToCourier();
//	usageTextProp->SetFontSize(14);
//	usageTextProp->SetVerticalJustificationToTop();
//	usageTextProp->SetJustificationToLeft();
//
//	vtkSmartPointer<vtkTextMapper> usageTextMapper = vtkSmartPointer<vtkTextMapper>::New();
//	usageTextMapper->SetInput("- Slice with mouse wheel\n  or Up/Down-Key\n- Zoom with pressed right\n  mouse button while dragging");
//	usageTextMapper->SetTextProperty(usageTextProp);
//
//	vtkSmartPointer<vtkActor2D> usageTextActor = vtkSmartPointer<vtkActor2D>::New();
//	usageTextActor->SetMapper(usageTextMapper);
//	usageTextActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
//	usageTextActor->GetPositionCoordinate()->SetValue(0.05, 0.95);
//
//	// create an interactor with our own style (inherit from vtkInteractorStyleImage)
//	// in order to catch mousewheel and key events
//	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
//		vtkSmartPointer<vtkRenderWindowInteractor>::New();
//
//	vtkSmartPointer<myVtkInteractorStyleImage> myInteractorStyle =
//		vtkSmartPointer<myVtkInteractorStyleImage>::New();
//
//	// make imageviewer2 and sliceTextMapper visible to our interactorstyle
//	// to enable slice status message updates when scrolling through the slices
//	myInteractorStyle->SetImageViewer(imageViewer);
//	myInteractorStyle->SetStatusMapper(sliceTextMapper);
//
//	imageViewer->SetupInteractor(renderWindowInteractor);
//	// make the interactor use our own interactorstyle
//	// cause SetupInteractor() is defining it's own default interatorstyle 
//	// this must be called after SetupInteractor()
//	renderWindowInteractor->SetInteractorStyle(myInteractorStyle);
//	// add slice status message and usage hint message to the renderer
//	imageViewer->GetRenderer()->AddActor2D(sliceTextActor);
//	imageViewer->GetRenderer()->AddActor2D(usageTextActor);
//
//	// initialize rendering and interaction
//	//imageViewer->GetRenderWindow()->SetSize(400, 300);
//	//imageViewer->GetRenderer()->SetBackground(0.2, 0.3, 0.4);
//	imageViewer->Render();
//	imageViewer->GetRenderer()->ResetCamera();
//	imageViewer->Render();
//	renderWindowInteractor->Start();
//	return EXIT_SUCCESS;
//}

//#include <vtkSmartPointer.h>
//#include <vtkActor.h>
//#include <vtkCubeSource.h>
//#include <vtkPolyData.h>
//#include <vtkPolyDataMapper.h>
//#include <vtkRenderWindow.h>
//#include <vtkRenderWindowInteractor.h>
//#include <vtkRenderer.h>
//#include <vtkInteractorStyleTrackballActor.h>
//#include <vtkInteractorStyleTrackballCamera.h>
//
//int main(int, char *[])
//{
//	// Create a cube.
//	vtkSmartPointer<vtkCubeSource> cubeSource =
//		vtkSmartPointer<vtkCubeSource>::New();
//
//	// Create a mapper and actor.
//	vtkSmartPointer<vtkPolyDataMapper> mapper =
//		vtkSmartPointer<vtkPolyDataMapper>::New();
//	mapper->SetInputConnection(cubeSource->GetOutputPort());
//
//	vtkSmartPointer<vtkActor> actor =
//		vtkSmartPointer<vtkActor>::New();
//	actor->SetMapper(mapper);
//
//	// Create a renderer, render window, and interactor
//	vtkSmartPointer<vtkRenderer> renderer =
//		vtkSmartPointer<vtkRenderer>::New();
//	vtkSmartPointer<vtkRenderWindow> renderWindow =
//		vtkSmartPointer<vtkRenderWindow>::New();
//	renderWindow->AddRenderer(renderer);
//	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
//		vtkSmartPointer<vtkRenderWindowInteractor>::New();
//	renderWindowInteractor->SetRenderWindow(renderWindow);
//
//	vtkSmartPointer<vtkInteractorStyleTrackballActor> style =
//		vtkSmartPointer<vtkInteractorStyleTrackballActor>::New();
//	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style1 =
//		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
//
//	renderWindowInteractor->SetInteractorStyle(style1);
//
//	// Add the actors to the scene
//	renderer->AddActor(actor);
//	renderer->SetBackground(.3, .2, .1);
//
//	// Render and interact
//	renderWindow->Render();
//	renderWindowInteractor->Start();
//
//	return EXIT_SUCCESS;
//}
