#ifndef TOPOVIEWERCALLBACKS_H
#define TOPOVIEWERCALLBACKS_H

#include "avTopoViewer.h"

class avTopoCallbackMouseMove : public vtkCommand {
 public:
  static avTopoCallbackMouseMove* New() { return new avTopoCallbackMouseMove; }

  virtual void Execute(vtkObject* caller, unsigned long eventId, void*) {
    if (m_viewer) {
      int dx = m_viewer->getInteractor()->GetEventPosition()[0];
      int dy = m_viewer->getInteractor()->GetEventPosition()[1];
      //std::cout << "\nInteraction " << dx << " " << dy << "  eventID:"
      //          << eventId << std::flush;
      m_viewer->ProcessMousePoint(dx, dy);
    }
  }
  avTopoCallbackMouseMove() {}
  void SetViewer(avTopoViewer* viewer) { m_viewer = viewer; }

  avTopoViewer* m_viewer = nullptr;
};

class avTopoCallbackMousePress : public vtkCommand {
 public:
  static avTopoCallbackMousePress* New() {
    return new avTopoCallbackMousePress;
  }

  virtual void Execute(vtkObject* caller, unsigned long, void*) {
    if (m_viewer) {
      int dx = m_viewer->getInteractor()->GetEventPosition()[0];
      int dy = m_viewer->getInteractor()->GetEventPosition()[1];
      std::cout << "\nLeft Button Down " << dx << " " << dy << std::flush;
      //m_viewer->LeftButtonDown(dx, dy);
    }
  }
  avTopoCallbackMousePress() {}
  void SetViewer(avTopoViewer* viewer) { m_viewer = viewer; }

  avTopoViewer* m_viewer = nullptr;
};

class avTopoCallbackMouseRelease : public vtkCommand {
 public:

  static avTopoCallbackMouseRelease* New() {
    return new avTopoCallbackMouseRelease;
  }

  virtual void Execute(vtkObject* caller, unsigned long eventId, void*) {
    if (m_viewer) {
      int dx = m_viewer->getInteractor()->GetEventPosition()[0];
      int dy = m_viewer->getInteractor()->GetEventPosition()[1];
      std::cout << "\n Left Button Up " << dx << " " << dy << std::flush;
      //m_viewer->LeftButtonUp(dx, dy);
    }
  }

  avTopoCallbackMouseRelease() {}
  void SetViewer(avTopoViewer* viewer) { m_viewer = viewer; }

  avTopoViewer* m_viewer = nullptr;
};


#endif