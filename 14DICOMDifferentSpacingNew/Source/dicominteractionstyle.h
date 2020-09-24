#ifndef DICOMINTERACTIONSTYLE_H
#define DICOMINTERACTIONSTYLE_H

#include <vtkActor.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
// headers needed for this example
#include <vtkActor2D.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageViewer2.h>
#include <vtkInteractorStyleImage.h>
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
  void setMax(int max) { _MaxSlice = max;
  }

 protected:
  vtkTextMapper* _StatusMapper;
  int _Slice = 0;
  int _MinSlice = 0;
  int _MaxSlice = 0;
  MainWindow* m_window;

 public:

  void SetStatusMapper(vtkTextMapper* statusMapper) {
    _StatusMapper = statusMapper;
  }

  void updateSliceMsg(int sliceNumber) {
    std::string msg = StatusMessage::Format(sliceNumber, _MaxSlice);
    _StatusMapper->SetInput(msg.c_str());
  }

 protected:
  void MoveSliceForward() {
    if (_Slice < _MaxSlice) {
      _Slice++;
      std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
      _StatusMapper->SetInput(msg.c_str());
      if (m_window) {
        m_window->updateSlider(_Slice);
      }
    }
  }

  void MoveSliceBackward() {
    if (_Slice > _MinSlice) {
      _Slice--;
      std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
      _StatusMapper->SetInput(msg.c_str());
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

  virtual void OnMouseWheelForward() {
    MoveSliceForward();
  }

  virtual void OnMouseWheelBackward() {
    if (_Slice > _MinSlice) {
      MoveSliceBackward();
    }
    // don't forward events, otherwise the image will be zoomed
    // in case another interactorstyle is used (e.g. trackballstyle, ...)
    // vtkInteractorStyleImage::OnMouseWheelBackward();
  }
};

vtkStandardNewMacro(myVtkInteractorStyleImage)

#endif  // DICOMINTERACTIONSTYLE_H
