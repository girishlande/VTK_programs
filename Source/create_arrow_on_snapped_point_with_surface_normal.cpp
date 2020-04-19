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
#include <vtkCellLocator.h>
#include <vtkArrowSource.h>
#include <vtkMath.h>
#include <vtkTransformPolyDataFilter.h>

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

      double testPoint[3] = {pos[0],pos[1],pos[2]};

      //Find the closest points to TestPoint
      double closestPoint[3];//the coordinates of the closest point will be returned here
      double closestPointDist2; //the squared distance to the closest point will be returned here
      vtkIdType cellId; //the cell id of the cell containing the closest point will be returned here
      int subId; //this is rarely used (in triangle strips only, I believe)
      cellLocator->FindClosestPoint(testPoint, closestPoint, cellId, subId, closestPointDist2);

      std::cout << "Coordinates of closest point: " << closestPoint[0] << " " << closestPoint[1] << " " << closestPoint[2] << std::endl;
      std::cout << "Squared distance to closest point: " << closestPointDist2 << std::endl;
      std::cout << "CellId: " << cellId << std::endl;

      endPoint[0]=pos[0];
      endPoint[1]=pos[1];
      endPoint[2]=pos[2];
      startPoint[0]=closestPoint[0];
      startPoint[1]=closestPoint[1];
      startPoint[2]=closestPoint[2];
        AddArrow();

      // Forward events
      vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }

    void add_base_sphere()
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

    void add_snapping_sphere()
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

        this->GetInteractor()->GetRenderWindow()->Render();

        // Forward events
        vtkInteractorStyleTrackballCamera::OnMouseMove();
    }

    void BuildCellLocator() {
        //Create a sphere
        basesphereSource =
                vtkSmartPointer<vtkSphereSource>::New();
        basesphereSource->SetRadius(40);
        basesphereSource->SetPhiResolution(50);
        basesphereSource->SetThetaResolution(50);
        basesphereSource->Update();

        // Create the tree
        cellLocator =
                vtkSmartPointer<vtkCellLocator>::New();
        cellLocator->SetDataSet(basesphereSource->GetOutput());
        cellLocator->BuildLocator();
    }

    void AddArrow() {
        //Create an arrow.
         vtkSmartPointer<vtkArrowSource> arrowSource =
           vtkSmartPointer<vtkArrowSource>::New();

         // Compute a basis
         double normalizedX[3];
         double normalizedY[3];
         double normalizedZ[3];

         // The X axis is a vector from start to end
         vtkMath::Subtract(endPoint, startPoint, normalizedX);
         double length = vtkMath::Norm(normalizedX);
         length=10;
         vtkMath::Normalize(normalizedX);

         // The Z axis is an arbitrary vector cross X
         double arbitrary[3];
         arbitrary[0] = vtkMath::Random(-10,10);
         arbitrary[1] = vtkMath::Random(-10,10);
         arbitrary[2] = vtkMath::Random(-10,10);
         vtkMath::Cross(normalizedX, arbitrary, normalizedZ);
         vtkMath::Normalize(normalizedZ);

         // The Y axis is Z cross X
         vtkMath::Cross(normalizedZ, normalizedX, normalizedY);
         vtkSmartPointer<vtkMatrix4x4> matrix =
           vtkSmartPointer<vtkMatrix4x4>::New();

         // Create the direction cosine matrix
         matrix->Identity();
         for (unsigned int i = 0; i < 3; i++)
           {
           matrix->SetElement(i, 0, normalizedX[i]);
           matrix->SetElement(i, 1, normalizedY[i]);
           matrix->SetElement(i, 2, normalizedZ[i]);
           }

         // Apply the transforms
         vtkSmartPointer<vtkTransform> transform =
           vtkSmartPointer<vtkTransform>::New();
         transform->Translate(startPoint);
         transform->Concatenate(matrix);
         transform->Scale(length, length, length);

         // Transform the polydata
         vtkSmartPointer<vtkTransformPolyDataFilter> transformPD =
           vtkSmartPointer<vtkTransformPolyDataFilter>::New();
         transformPD->SetTransform(transform);
         transformPD->SetInputConnection(arrowSource->GetOutputPort());

         //Create a mapper and actor for the arrow
         vtkSmartPointer<vtkPolyDataMapper> mapper =
           vtkSmartPointer<vtkPolyDataMapper>::New();
         vtkSmartPointer<vtkActor> actor =
           vtkSmartPointer<vtkActor>::New();
       #ifdef USER_MATRIX
         mapper->SetInputConnection(arrowSource->GetOutputPort());
         actor->SetUserMatrix(transform->GetMatrix());
       #else
         mapper->SetInputConnection(transformPD->GetOutputPort());
       #endif
         actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(1,0,0);
        this->GetDefaultRenderer()->AddActor(actor);

    }

    private:
    vtkSmartPointer<vtkActor> actor;
    vtkSmartPointer<vtkSphereSource> basesphereSource;
    vtkSmartPointer<vtkCellLocator> cellLocator;

    // Start and End point for Arrow
    double startPoint[3], endPoint[3];
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
  style->add_base_sphere();
  style->add_snapping_sphere();
  style->BuildCellLocator();

  // Add the actors to the scene
  renderer->AddActor ( actor );
  renderer->SetBackground ( 0,0,1 );

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
