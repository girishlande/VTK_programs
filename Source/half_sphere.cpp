#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType);
#include "vtkActor.h"
#include "vtkClipPolyData.h"
#include "vtkPlane.h"
#include "vtkInteractorObserver.h"
#include "vtkInteractorStyleSwitch.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"
using namespace std;
int main(int argc, char ** argv) {
  vtkSmartPointer<vtkSphereSource> sphere =
    vtkSmartPointer<vtkSphereSource>::New();
  vtkSmartPointer<vtkClipPolyData> clip =
    vtkSmartPointer<vtkClipPolyData>::New();
  clip->SetValue(0);
  clip->GenerateClippedOutputOn();
  clip->SetInputConnection(sphere->GetOutputPort());
  vtkSmartPointer<vtkPlane> plane =
    vtkSmartPointer<vtkPlane>::New();
  //plane->SetNormal(-1.0, 0.0, 0.0);
  plane->SetNormal(1.0, 0.0, 0.0);
  clip->SetClipFunction (plane);
  vtkSmartPointer<vtkPolyDataMapper> polyDataMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  polyDataMapper->SetInputConnection(clip->GetOutputPort());
  vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(polyDataMapper);
  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->SetSize(800,600);
  renderWindow->SetWindowName("VTK");
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);
  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  renderer->AddActor(actor);
  renderWindow->AddRenderer(renderer);
  vtkInteractorStyleSwitch * styleSwitch
    = vtkInteractorStyleSwitch::SafeDownCast(
        renderWindowInteractor->GetInteractorStyle());
  if (styleSwitch)
    styleSwitch->SetCurrentStyleToTrackballCamera();
  renderWindow->Render();
  renderWindowInteractor->Start();
}