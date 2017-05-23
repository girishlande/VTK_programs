#include <QCoreApplication>
#include <vtkAutoInit.h>
 VTK_MODULE_INIT(vtkRenderingOpenGL2)

#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkSphereSource.h>
#include <vtkRendererCollection.h>
#include <vtkCellArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkPlaneSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropPicker.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkTransform.h>

// Handle mouse events
class MouseInteractorStyle2 : public vtkInteractorStyleTrackballCamera
{
  public:
    static MouseInteractorStyle2* New();
    vtkTypeMacro(MouseInteractorStyle2, vtkInteractorStyleTrackballCamera);

    MouseInteractorStyle2()
    {
        cursor_ready = false;
    }

    virtual void OnLeftButtonDown()
    {
      int* clickPos = this->GetInteractor()->GetEventPosition();

      // Pick from this location.
      vtkSmartPointer<vtkPropPicker>  picker =
        vtkSmartPointer<vtkPropPicker>::New();
      picker->Pick(clickPos[0], clickPos[1], 0, this->GetDefaultRenderer());

      double* pos = picker->GetPickPosition();
      std::cout << "Pick position (world coordinates) is: "
                << pos[0] << " " << pos[1]
                << " " << pos[2] << std::endl;

      std::cout << "Picked actor: " << picker->GetActor() << std::endl;
      //Create a sphere
      vtkSmartPointer<vtkSphereSource> sphereSource =
        vtkSmartPointer<vtkSphereSource>::New();
      sphereSource->SetCenter(pos[0], pos[1], pos[2]);
      sphereSource->SetRadius(1);

      //Create a mapper and actor
      vtkSmartPointer<vtkPolyDataMapper> mapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
      mapper->SetInputConnection(sphereSource->GetOutputPort());

      vtkSmartPointer<vtkActor> actor =
        vtkSmartPointer<vtkActor>::New();
      actor->SetMapper(mapper);
      actor->GetProperty()->SetColor(1,0,0);

      //this->GetInteractor()->GetRenderWindow()->GetRenderers()->GetDefaultRenderer()->AddActor(actor);
      this->GetDefaultRenderer()->AddActor(actor);
      // Forward events
      vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }

    void add_sphere()
    {
        //Create a sphere
        vtkSmartPointer<vtkSphereSource> sphereSource =
          vtkSmartPointer<vtkSphereSource>::New();
        sphereSource->SetRadius(40);
        sphereSource->SetPhiResolution(50);
        sphereSource->SetThetaResolution(50);

        //Create a mapper and actor
        vtkSmartPointer<vtkPolyDataMapper> mapper =
          vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(sphereSource->GetOutputPort());

        vtkSmartPointer<vtkActor> actor =
          vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);

        this->GetDefaultRenderer()->AddActor(actor);
    }

    void add_sphere1()
    {
        //Create a sphere
        vtkSmartPointer<vtkSphereSource> sphereSource =
          vtkSmartPointer<vtkSphereSource>::New();
        sphereSource->SetCenter(0,0,0);
        sphereSource->SetPhiResolution(10);
        sphereSource->SetThetaResolution(10);
        sphereSource->SetRadius(1);

        //Create a mapper and actor
        vtkSmartPointer<vtkPolyDataMapper> mapper =
          vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(sphereSource->GetOutputPort());

        actor = vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(0,1,0);
        actor->Modified();

        this->GetDefaultRenderer()->AddActor(actor);
        cursor_ready = true;
    }

    virtual void OnMouseMove()
    {
        if (!cursor_ready) {
            return;
        }
        int* clickPos = this->GetInteractor()->GetEventPosition();

        // Pick from this location.
        vtkSmartPointer<vtkPropPicker>  picker =
          vtkSmartPointer<vtkPropPicker>::New();
        picker->Pick(clickPos[0], clickPos[1], 0, this->GetDefaultRenderer());

        double* pos = picker->GetPickPosition();
        std::cout << "Pick position (world coordinates) is: "
                  << pos[0] << " " << pos[1]
                  << " " << pos[2] << std::endl;

        vtkSmartPointer<vtkTransform> translation =
           vtkSmartPointer<vtkTransform>::New();
         translation->Translate(pos[0],pos[1],pos[2]);

        actor->SetUserTransform(translation);
        actor->Modified();

        //this->GetDefaultRenderer()->Render();
        this->GetInteractor()->GetRenderWindow()->Render();

        // Forward events
        vtkInteractorStyleTrackballCamera::OnMouseMove();
    }

  private:
vtkSmartPointer<vtkActor> actor;
bool cursor_ready;
};

vtkStandardNewMacro(MouseInteractorStyle2)

// Execute application.
int main(int, char *[])
{
  vtkSmartPointer<vtkPlaneSource> planeSource =
    vtkSmartPointer<vtkPlaneSource>::New();
  planeSource->Update();

  // Create a polydata object
  vtkPolyData* polydata = planeSource->GetOutput();

  // Create a mapper
  vtkSmartPointer<vtkPolyDataMapper> mapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
#if VTK_MAJOR_VERSION <= 5
  mapper->SetInput ( polydata );
#else
  mapper->SetInputData ( polydata );
#endif

  // Create an actor
  vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();
  actor->SetMapper ( mapper );

  std::cout << "Actor address: " << actor << std::endl;

  // A renderer and render window
  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer ( renderer );

  // An interactor
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow ( renderWindow );

  // Set the custom stype to use for interaction.
  vtkSmartPointer<MouseInteractorStyle2> style =
    vtkSmartPointer<MouseInteractorStyle2>::New();
  style->SetDefaultRenderer(renderer);

  renderWindowInteractor->SetInteractorStyle( style );
  style->add_sphere();
  style->add_sphere1();

  // Add the actors to the scene
  renderer->AddActor ( actor );
  renderer->SetBackground ( 0,0,1 );

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
