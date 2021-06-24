#ifndef PLANEWIDGETCALLBACK_H
#define PLANEWIDGETCALLBACK_H
#include <vtkCommand.h>
#include <vtkImagePlaneWidget.h>
#include "mainwindow.h"

class PlaneWidgetCallback : public vtkCommand {
 public:
  static PlaneWidgetCallback *New() { return new PlaneWidgetCallback; }

  virtual void Execute(vtkObject *caller, unsigned long, void *) {
    vtkImagePlaneWidget *planewidget =
        reinterpret_cast<vtkImagePlaneWidget *>(caller);
    double pos[4];
    planewidget->GetCursorData(pos);
    if (pos[0] < 0) return;
    cout << "\nposition:" << pos[0] << " " << pos[1] << " " << pos[2];
    if (m_window) {
      m_window->setCurrentPosition(pos);
    }
  }
  PlaneWidgetCallback() {}
  void setWindow(MainWindow* window) { m_window = window;
  }
  private:
  MainWindow *m_window;
};

#endif