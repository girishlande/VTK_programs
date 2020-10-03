#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ui_mainwindow.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkImagePlaneWidget.h"
#include "avTopoViewerEx.h"

class QVTKWidget;
class QVBoxLayout;
class vtkImageViewer2;
class QScrollBar;
class QGridLayout;
class vtkRenderer;
class vtkImageActor;
class vtkProperty2D;
class vtkActor2D;
    
namespace Ui {
class MainWindow;
}

#define vtkNew(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = 0);
  ~MainWindow();

  void updateSlider(int value);
  void ProcessMousePoint(int mx, int my);
  void LeftButtonDown(int mx, int my);
  void LeftButtonUp(int mx, int my);

  void ReadTopoImage();
  vtkRenderWindowInteractor* getInteractor();


 protected:
  void addDicomImageInViewport();
  void prepareImageData(vtkSmartPointer<vtkImageData>& input, int direction,
                        vtkSmartPointer<vtkImageData>& output);
  void fetchXYImage(vtkSmartPointer<vtkImageData>& input,
                    vtkSmartPointer<vtkImageData>& output);
  void fetchYZImage(vtkSmartPointer<vtkImageData>& input,
                    vtkSmartPointer<vtkImageData>& output);
  void fetchXZImage(vtkSmartPointer<vtkImageData>& input,
                    vtkSmartPointer<vtkImageData>& output);

  void InitialiseDICOM();

 private slots:

  void on_actionOpen_DICOM_file_triggered();
  void on_actionExit_triggered();
  void sliderChanged(int value);
  void UpdateViewForDICOM();
  void test1();
  void test2();
  void ViewportBorder(vtkSmartPointer<vtkRenderer> renderer,
                                  double* color);
  void printImageDetails(vtkImageData* image);
  void NormalisedToDeviceCoordinates(double nx, double ny, int& dx, int& dy);
  void DeviceToNormalised(int dx, int dy, double& nx, double& ny);

 private:
  Ui::MainWindow* ui;
  QScrollBar* m_slider;
  vtkImageViewer2* m_vtkImageViewer;
  QVBoxLayout* vboxLayout;
  QWidget* centralwidget;
  QVTKWidget* m_vtkView;
  QGridLayout* m_container_layout;

  QString m_dicom_dir_path = "";

  int m_width;
  int m_height;

  vtkRenderer* m_topoRenderer = nullptr;
  double m_topoX = 0.1;
  double m_topoY = 0.7;
  double m_topoWidth = 0.2;
  double m_topoHeight = 0.2;
  vtkProperty2D* m_topoBorderProperty = nullptr;
  bool m_topoHighlighted = false;
  bool m_topoDragging = false;
  int m_dragStartX;
  int m_dragStartY;

  vtkImageActor* m_imageactor = nullptr;
  vtkSmartPointer<vtkRenderer> m_renderer;

  vtkActor2D* m_image2DActor = nullptr;
  vtkActor2D* m_topoActor = nullptr;
  vtkRenderWindowInteractor* m_interactor = nullptr;

  avTopoViewerEx* m_topoviewer = nullptr;
  vtkSmartPointer<vtkImageData> m_topoImage;
  vtkSmartPointer<vtkImageData> m_imageVolume;

};

#endif  // MAINWINDOW_H
