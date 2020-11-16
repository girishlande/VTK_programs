#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "avTopoViewerEx.h"
#include "ui_mainwindow.h"
#include "vtkDICOMImageReader.h"
#include "vtkImageData.h"
#include "vtkImagePlaneWidget.h"
#include "vtkSmartPointer.h"
#include "vtkEventQtSlotConnect.h"

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
  void test3();
  void test4();
  void sliderChanged(int value);
  void LayoutChanged(int index);
  void initialiseWithDICOM();
  void slot_clicked(vtkObject*, unsigned long, void*, void*);
  void slot_released(vtkObject*, unsigned long, void*, void*);
  void slot_moved(vtkObject*, unsigned long, void*, void*);

 protected:
  void SetConnections();

  void displyRendererDetails(vtkRenderer* renderer);
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
  void ToggleVisibility();
  void ResetPosition();
  void ToggleSynchronization();
  void DrawTopoEx();
  void UpdateTopoEx(int sliceIndex, int topoIndex);
  void ReadTopoImage();
  void ReadConfigurationFile();
  void ProcessInputData();

 private:
  Ui::MainWindow* ui;
  vtkImageViewer2* m_vtkImageViewer;
  QVBoxLayout* vboxLayout;
  QWidget* centralwidget;
  QVTKWidget* m_vtkView;
  QGridLayout* m_container_layout;

  QString m_dicom_dir_path = "";
  QString m_file_name = "";

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
  std::vector<std::shared_ptr<avTopoViewerEx>> m_topoEx;

  std::vector<vtkSmartPointer<vtkImageData>> m_imagedata;
  vtkSmartPointer<vtkImageData> m_imageVolume;
  vtkSmartPointer<vtkImageData> m_topoImage;
  vtkNew<vtkEventQtSlotConnect> m_connections;

  int m_row = 1;
  int m_col = 1;

  double m_topoWidth = 0.2;
  double m_topoLeftMargin = 0.2;
  double m_topoTopMargin = 0.2;

  bool m_visibility = true;
};

#endif  // MAINWINDOW_H
