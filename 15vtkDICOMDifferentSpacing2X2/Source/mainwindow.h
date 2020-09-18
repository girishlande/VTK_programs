#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ui_mainwindow.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkImagePlaneWidget.h"
#include "vtkDICOMImageReader.h"

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

 protected slots:
  void on_actionOpen_DICOM_file_triggered();
  void on_actionExit_triggered();
  void test1();
  void test2();
  void sliderChanged(int value);
  void readSingleImage();
  void readImage(int index);
  void LayoutChanged(int index);
  void readAllImages();
  void initialiseWithDICOM();

 protected:
  void ViewportBorder();
  void drawAxialLine();
  void displyRendererDetails(vtkRenderer* renderer);
  void calculateViewportDetails(vtkImageActor* actor);
  void InitialiseCornerText();
  void SetupLayoutsCombobox();
  void createMultipleViewports(int rows,int cols);
  void ViewportBorder(vtkSmartPointer<vtkRenderer>& renderer,
                                  double* color, bool last);
  void AddImagesInViewports();
  void UpdateImagesInViewports();
  void UpdateSliceNumberCornerText(int sliceNumber, int RendererIndex);
  void AddSliceNumberCornerText(int sliceNumber, vtkRenderer* renderer);
  int MapViewportNumber(int index);
  void SynchronizeSlider();

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
  int m_currentSliceNumber=0;
  QComboBox* m_layoutsCombo;

  double m_topoViewWidth = 0.3;
  double m_topoViewHeight = 0.2;
  double m_topoMarginTop = 0.05;
  double m_topoMarginLeft = 0.03;

  std::vector<std::string> m_imageFiles;
  vtkImageActor* m_imageactor = nullptr;
  myVtkInteractorStyleImage* m_interaction = nullptr;

  std::vector<LayoutConfig> m_layoutconfig;
  std::vector<vtkRenderer*> m_renderers;
  std::vector<vtkImageActor*> m_imageActors;
  std::vector<vtkTextMapper*> m_sliceNumbers;
  std::vector<vtkSmartPointer<vtkImageData>> m_imagedata;

  int m_row = 1;
  int m_col = 1;
};

#endif  // MAINWINDOW_H
