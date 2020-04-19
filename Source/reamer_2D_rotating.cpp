


#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType);
//VTK_MODULE_INIT(vtkInteractionStyle)
//VTK_MODULE_INIT(vtkDomainsChemistryOpenGL2)
//VTK_MODULE_INIT(vtkRenderingContextOpenGL2)
//VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)

#define USER_MATRIX
#include <vtkSphereSource.h>
#include <vtkTextProperty.h>
#include <vtkProperty2D.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkSliderWidget.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkCommand.h>
#include <vtkWidgetEvent.h>
#include <vtkCallbackCommand.h>
#include <vtkWidgetEventTranslator.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSliderWidget.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkProperty.h>
#include <vtkTransform.h>
#include <vtkArcSource.h>
#include <vtkMath.h>
#include <vtkLineSource.h>

#define vtkNew(type, name)\
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()


// This does the actual work.
// Callback for the interaction
class vtkSliderCallback : public vtkCommand
{
public:
	static vtkSliderCallback *New()
	{
		return new vtkSliderCallback;
	}
	virtual void Execute(vtkObject *caller, unsigned long, void*)
	{
		vtkSliderWidget *sliderWidget =
			reinterpret_cast<vtkSliderWidget*>(caller);
		//this->SphereSource->SetPhiResolution(static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue());
		//this->SphereSource->SetThetaResolution(static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue());
		double val = static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue();
		//this->transform->Translate(val, 0, 0);
		this->transform_obj->RotateZ(val - current_val);
		current_val = val;
		cout << "\n val " << val;
	}
	vtkSliderCallback():current_val(0) {

	}
	vtkSmartPointer<vtkTransform> transform_obj;
	double current_val;
};

vtkSmartPointer<vtkRenderer> renderer;
vtkSmartPointer<vtkSliderCallback> callback;

void Add2DReamer2(double center[3], double normal[3], double up_vec[3], double radius)
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
	vtkMath::MultiplyScalar(x_axis, 2 * radius);
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

	vtkSmartPointer<vtkTransform> translation =
		vtkSmartPointer<vtkTransform>::New();
	translation->Translate(1.0, 2.0, 3.0);
	callback->transform_obj = translation;

	arc_actor1->SetUserTransform(translation);
	arc_actor2->SetUserTransform(translation);
	line_actor1->SetUserTransform(translation);
	line_actor2->SetUserTransform(translation);
	line_actor3->SetUserTransform(translation);

	renderer->ResetCamera();
}

void AddLine(double pt1[3], double pt2[3], double color[3])
{
	double vec[3];
	vtkMath::Subtract(pt1, pt2, vec);
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
	callback = vtkSmartPointer<vtkSliderCallback>::New();

	renderer =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);

	double center[3] = { 0, 0, 0 };
	double normal[3] = { 0, 0, 1 };
	double upvec[3] = { 0, -1, 0 };
	double radius = 10;
	Add2DReamer2(center, normal, upvec, radius);

	// An interactor
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	vtkSmartPointer<vtkSliderRepresentation2D> sliderRep =
		vtkSmartPointer<vtkSliderRepresentation2D>::New();

	sliderRep->SetMinimumValue(0.0);
	sliderRep->SetMaximumValue(360.0);
	sliderRep->SetValue(0.0);
	sliderRep->SetTitleText("Sphere Resolution");

	// Set color properties:
	// Change the color of the knob that slides
	sliderRep->GetSliderProperty()->SetColor(1, 0, 0);//red
	// Change the color of the text indicating what the slider controls
	sliderRep->GetTitleProperty()->SetColor(1, 0, 0);//red
	// Change the color of the text displaying the value
	sliderRep->GetLabelProperty()->SetColor(1, 0, 0);//red
	// Change the color of the knob when the mouse is held on it
	sliderRep->GetSelectedProperty()->SetColor(0, 1, 0);//green
	// Change the color of the bar
	sliderRep->GetTubeProperty()->SetColor(1, 1, 0);//yellow
	// Change the color of the ends of the bar
	sliderRep->GetCapProperty()->SetColor(1, 1, 0);//yellow

	sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToDisplay();
	sliderRep->GetPoint1Coordinate()->SetValue(40, 40);
	sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToDisplay();
	sliderRep->GetPoint2Coordinate()->SetValue(600, 40);

	vtkSmartPointer<vtkSliderWidget> sliderWidget =
		vtkSmartPointer<vtkSliderWidget>::New();
	sliderWidget->SetInteractor(renderWindowInteractor);
	sliderWidget->SetRepresentation(sliderRep);
	sliderWidget->SetAnimationModeToAnimate();
	sliderWidget->EnabledOn();

	sliderWidget->AddObserver(vtkCommand::InteractionEvent, callback);

	renderWindow->Render();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}