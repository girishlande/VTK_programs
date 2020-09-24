#ifndef MODELINTERACTIONSTYLE_H
#define MODELINTERACTIONSTYLE_H
#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMath.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSmartPointer.h>
#include <vtkWorldPointPicker.h>

#include "mainwindow.h"

// Define interaction style
class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera {
 public:
  static MouseInteractorStyle* New();
  vtkTypeMacro(MouseInteractorStyle, vtkInteractorStyleTrackballCamera)

      void setWindow(MainWindow* window) {
    m_window = window;
  }
  virtual void OnMouseMove() {
    
  }

  virtual void OnMouseWheelForward() {
  }

  virtual void OnLeftButtonDown() {
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    if (m_window) {
    }
  }

  virtual void OnLeftButtonUp() {  }

  MainWindow* m_window;
  bool m_full_screen_mode;
};
vtkStandardNewMacro(MouseInteractorStyle)

#endif  // MODELINTERACTIONSTYLE_H
