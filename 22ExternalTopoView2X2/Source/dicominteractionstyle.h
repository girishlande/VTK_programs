#ifndef DICOMINTERACTIONSTYLE_H
#define DICOMINTERACTIONSTYLE_H

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageViewer2.h>
#include <vtkInteractorStyleImage.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
// needed to easily convert int to std::string
#include <sstream>

// helper class to format slice status message
class StatusMessage {
 public:
  static std::string Format(int slice, int maxSlice) {
    std::stringstream tmp;
    tmp << "Slice Number  " << slice + 1 << "/" << maxSlice + 1;
    return tmp.str();
  }
};

// Define own interaction style
class myVtkInteractorStyleImage : public vtkInteractorStyleImage {
 public:
  static myVtkInteractorStyleImage* New();
  vtkTypeMacro(myVtkInteractorStyleImage, vtkInteractorStyleImage)

      void setWindow(MainWindow* w) {
    m_window = w;
  }

  MainWindow* getWindow() { return m_window; }
  void setMax(int max) { _MaxSlice = max; }

 protected:
  int _Slice = 0;
  int _MinSlice = 0;
  int _MaxSlice = 0;
  MainWindow* m_window;

 protected:
  void MoveSliceForward() {
    if (_Slice < _MaxSlice) {
      _Slice++;
      if (m_window) {
        m_window->updateSlider(_Slice);
      }
    }
  }

  void MoveSliceBackward() {
    if (_Slice > _MinSlice) {
      _Slice--;
      if (m_window) {
        m_window->updateSlider(_Slice);
      }
    }
  }

  virtual void OnKeyDown() {
    std::string key = this->GetInteractor()->GetKeySym();
    if (key.compare("Up") == 0) {
      MoveSliceForward();
    } else if (key.compare("Down") == 0) {
      MoveSliceBackward();
    }
    // forward event
    vtkInteractorStyleImage::OnKeyDown();
  }

  virtual void OnMouseWheelForward() { MoveSliceForward(); }

  virtual void OnMouseWheelBackward() {
    if (_Slice > _MinSlice) {
      MoveSliceBackward();
    }
    // don't forward events, otherwise the image will be zoomed
    // in case another interactorstyle is used (e.g. trackballstyle, ...)
    // vtkInteractorStyleImage::OnMouseWheelBackward();
  }

  virtual void OnLeftButtonDown() {
    int dx = this->Interactor->GetEventPosition()[0];
    int dy = this->Interactor->GetEventPosition()[1];

    if (m_window) m_window->LeftButtonDown(dx, dy);

    // vtkInteractorStyleImage::OnLeftButtonDown();
  }

  virtual void OnLeftButtonUp() {
    int dx = this->Interactor->GetEventPosition()[0];
    int dy = this->Interactor->GetEventPosition()[1];

    if (m_window) m_window->LeftButtonUp(dx, dy);
    // vtkInteractorStyleImage::OnLeftButtonUp();
  }

  virtual void OnMouseMove() {
    int dx = this->Interactor->GetEventPosition()[0];
    int dy = this->Interactor->GetEventPosition()[1];

    if (m_window) m_window->ProcessMousePoint(dx, dy);
    // vtkInteractorStyleImage::OnMouseMove();
  }
};

vtkStandardNewMacro(myVtkInteractorStyleImage)

#endif  // DICOMINTERACTIONSTYLE_H
