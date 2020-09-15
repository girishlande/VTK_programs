#ifndef MODELINTERACTIONSTYLE_H
#define MODELINTERACTIONSTYLE_H
#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkWorldPointPicker.h>

#include "mainwindow.h"

#define SIDES 50
#define PLANE_RADIUS 150

class MainWindow;

// Define interaction style
class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera {
 public:
  static MouseInteractorStyle* New();
  vtkTypeMacro(MouseInteractorStyle, vtkInteractorStyleTrackballCamera)

      void setWindow(MainWindow* w) {
    m_window = w;
  }

  virtual void OnLeftButtonDown() {
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    if (m_window) {
      int x = this->Interactor->GetEventPosition()[0];
      int y = this->Interactor->GetEventPosition()[1];
      m_window->hightlightActivePort(x, y);
    }
  }

 private:
  MainWindow* m_window;
  bool m_full_screen_mode;
};
vtkStandardNewMacro(MouseInteractorStyle)

#endif  // MODELINTERACTIONSTYLE_H
