#ifndef DICOMINTERACTIONSTYLE_H
#define DICOMINTERACTIONSTYLE_H

#include <vtkAbstractPicker.h>
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
class DicomInteractionStyle : public vtkInteractorStyleImage {
 public:
  static DicomInteractionStyle* New();
  vtkTypeMacro(DicomInteractionStyle, vtkInteractorStyleImage)

      void setWindow(MainWindow* w) {
    m_window = w;
  }

  MainWindow* getWindow() { return m_window; }

 protected:
  vtkImageViewer2* _ImageViewer;
  vtkTextMapper* _StatusMapper;
  int _Slice;
  int _MinSlice;
  int _MaxSlice;
  MainWindow* m_window;

 public:
  void SetImageViewer(vtkImageViewer2* imageViewer) {
    _ImageViewer = imageViewer;
    _MinSlice = imageViewer->GetSliceMin();
    _MaxSlice = imageViewer->GetSliceMax();
    _Slice = (_MinSlice + _MaxSlice) / 2;
  }

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
      _Slice += 1;
      _ImageViewer->SetSlice(_Slice);
      std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
      _StatusMapper->SetInput(msg.c_str());
      _ImageViewer->Render();
      if (m_window) {
        // m_window->updateSlider(_Slice);
      }
    }
  }

  void MoveSliceBackward() {
    if (_Slice > _MinSlice) {
      _Slice -= 1;
      _ImageViewer->SetSlice(_Slice);
      std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
      _StatusMapper->SetInput(msg.c_str());
      _ImageViewer->Render();
      if (m_window) {
        // m_window->updateSlider(_Slice);
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

  virtual void OnMouseMove() {
    
  }

  void printMousePosition() {
    cout << "\n Mouse pos: " << this->Interactor->GetEventPosition()[0] << " "
         << this->Interactor->GetEventPosition()[1];
    double picked[3];
    this->Interactor->GetPicker()->GetPickPosition(picked);
    std::cout << "\n Picked value: " << picked[0] << " " << picked[1] << " "
              << picked[2];
  }

  virtual void OnLeftButtonDown() {
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    if (m_window) {
      int x = this->Interactor->GetEventPosition()[0];
      int y = this->Interactor->GetEventPosition()[1];
      m_window->hightlightActivePort(x, y);
    }
  }
};

vtkStandardNewMacro(DicomInteractionStyle)

#endif  // DICOMINTERACTIONSTYLE_H
