#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "avTopoViewerEx.h"
#include "ui_mainwindow.h"
#include "vtkDICOMImageReader.h"
#include "vtkImageData.h"
#include "vtkImagePlaneWidget.h"
#include "vtkSmartPointer.h"

class QVTKWidget;
class QVBoxLayout;
class vtkImageViewer2;
class QScrollBar;
class QComboBox;

class QGridLayout;
class vtkRenderer;
class vtkActor2D;
class vtkPoints;
class vtkImageActor;
class myVtkInteractorStyleImage;
class vtkTextMapper;
class avTopoViewer;
class avTopoViewerEx;

namespace Ui {
class MainWindow;
}

#define vtkNew(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

struct LayoutConfig {
  QString name;
  int row;
  int col;
};
class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = 0);
  ~MainWindow();

  void updateSlider(int value);
  void ProcessMousePoint(int mx, int my);
  void LeftButtonDown(int mx, int my);
  void LeftButtonUp(int mx, int my);

 protected slots:
  void on_actionOpen_DICOM_file_triggered();
  void on_actionExit_triggered();
  void test1();
  void test2();
  void sliderChanged(int value);
  void LayoutChanged(int index);
  void initialiseWithDICOM();

 protected:
  void ViewportBorder();
  void drawAxialLine();
  void displyRendererDetails(vtkRenderer* renderer);
  void calculateViewportDetails(vtkImageActor* actor);
  void SetupLayoutsCombobox();
  void createMultipleViewports(int rows, int cols);
  void ViewportBorder(vtkSmartPointer<vtkRenderer>& renderer, double* color,
                      bool last);
  void AddImagesInViewports();
  void UpdateImagesInViewports();
  void UpdateSliceNumberCornerText(int sliceNumber, int RendererIndex);
  void AddSliceNumberCornerText(int sliceNumber, vtkRenderer* renderer);
  int MapViewportNumber(int index);
  void SynchronizeSlider();
  void ClearViewports();
  void ReadImageVolume();
  void ConvertImageVolumeToSeparateImages();
  void FetchXYImage(vtkSmartPointer<vtkImageData> output, int Zindex);
  void DrawTopo();
  void DrawTopoEx();
  void UpdateTopo(int sliceIndex, int topoIndex);
  void UpdateTopoEx(int sliceIndex, int topoIndex);
  void ReadTopoImage();

 private:
  Ui::MainWindow* ui;
  vtkImageViewer2* m_vtkImageViewer;
  QVBoxLayout* vboxLayout;
  QWidget* centralwidget;
  QVTKWidget* m_vtkView;
  QGridLayout* m_container_layout;

  QString m_dicom_dir_path = "";

  vtkSmartPointer<vtkRenderer> m_renderer;
  vtkSmartPointer<vtkDICOMImageReader> m_dicom_reader;
  vtkImageData* m_dicom_image;
  bool m_planeVisible = false;

  vtkPoints* m_axialPoints = nullptr;
  vtkActor2D* m_axialLineActor = nullptr;

  QScrollBar* m_slider;
  int m_minSliceNumber;
  int m_maxSliceNumber;
  int m_currentSliceNumber = 0;
  QComboBox* m_layoutsCombo;

  std::vector<std::string> m_imageFiles;
  vtkImageActor* m_imageactor = nullptr;
  myVtkInteractorStyleImage* m_interaction = nullptr;

  std::vector<LayoutConfig> m_layoutconfig;
  std::vector<vtkRenderer*> m_renderers;
  std::vector<vtkImageActor*> m_imageActors;
  std::vector<vtkTextMapper*> m_sliceNumbers;
  std::vector<std::shared_ptr<avTopoViewer>> m_topo;
  std::vector<std::shared_ptr<avTopoViewerEx>> m_topoEx;

  std::vector<vtkSmartPointer<vtkImageData>> m_imagedata;
  vtkSmartPointer<vtkImageData> m_imageVolume;
  vtkSmartPointer<vtkImageData> m_topoImage;

  int m_row = 1;
  int m_col = 1;
};

#endif  // MAINWINDOW_H
