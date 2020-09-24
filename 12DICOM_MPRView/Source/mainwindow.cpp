#include "mainwindow.h"

#include <QBoxLayout.h>
#include <QComboBox.h>
#include <QDir.h>
#include <QFileDialog.h>
#include <QGridLayout.h>
#include <QSplitter.h>
#include <qdebug.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageViewer2.h>
#include <vtkInteractorStyleImage.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

#include <QPushButton>
#include <QStringList>
#include <QTimer>
#include <sstream>

#include "PlaneWidgetCallback.h"
#include "QVTKWidget.h"
#include "dicominteractionstyle.h"
#include "modelinteractionstyle.h"
#include "vtkCamera.h"
#include "vtkCellPicker.h"
#include "vtkImageData.h"
#include "vtkImageMapper.h"
#include "vtkLineSource.h"
#include "vtkNamedColors.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPolyLine.h"
#include "vtkProperty2D.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_vtkImageViewer(nullptr) {
  // Setup window and resize it
  ui->setupUi(this);
  this->showMaximized();

  // Apply style sheet on it for Black theme appearance
  QFile file(":/stylesheets/style2.qss");
  file.open(QFile::ReadOnly);
  QString styleSheet = QLatin1String(file.readAll());
  this->setStyleSheet(styleSheet);

  // Create central widget
  centralwidget = new QWidget(this);
  centralwidget->setObjectName(QStringLiteral("centralwidget"));

  // Create layout for central widget
  vboxLayout = new QVBoxLayout(centralwidget);
  vboxLayout->setSpacing(1);
  vboxLayout->setObjectName(QStringLiteral("vboxLayout"));

  m_vtkView = new QVTKWidget(this);

  m_container_layout = new QGridLayout;
  m_container_layout->addWidget(m_vtkView, 0, 0, 1, 1);
  QWidget* container = new QWidget;
  container->setLayout(m_container_layout);

  QVBoxLayout* tablayout = new QVBoxLayout();
  QPushButton* b1 = new QPushButton("Test Button", this);
  connect(b1, SIGNAL(clicked(bool)), this, SLOT(test1()));
  QPushButton* b2 = new QPushButton("Show/Hide Planes", this);
  connect(b2, SIGNAL(clicked(bool)), this, SLOT(test2()));
  tablayout->addWidget(b1);
  tablayout->addWidget(b2);
  QWidget* controls_widget = new QWidget;
  controls_widget->setLayout(tablayout);

  // -------------------------------------------------
  // Add splitter between VTK Views and controls
  // -------------------------------------------------
  QSplitter* splitter = new QSplitter;
  splitter->setOrientation(Qt::Horizontal);
  splitter->addWidget(container);
  splitter->addWidget(controls_widget);

  QList<int> sizes;
  sizes << 1600 << 400;
  splitter->setSizes(sizes);

  vboxLayout->addWidget(splitter);
  this->setCentralWidget(centralwidget);

  createMultipleViewports();
}

void MainWindow::initialiseWithDICOM() {
  m_dicom_dir_path = "D:/git/QtProjects/12vtkQTDemo3/Source/models/series201";
  QTimer::singleShot(100, this, SLOT(displayPlaneWidgets()));
}

// ------------------------
// destructor and exit
// ------------------------
MainWindow::~MainWindow() { delete ui; }
void MainWindow::on_actionExit_triggered() { QApplication::quit(); }

// --------------------------------
// Read DICOM file name from user
// --------------------------------
void MainWindow::on_actionOpen_DICOM_file_triggered() {
  QString fileName =
      QFileDialog::getOpenFileName(this, "Select DICOM file", "../models",
                                   "DICOM Files (*.dcm);;All Files (*.*)");

  if (fileName.isNull()) {
    cerr << "Could not open file" << endl;
    return;
  }

  QFileInfo fileinfo(fileName);
  QDir dirname = fileinfo.absoluteDir();
  QString dirpath = dirname.absolutePath();
  m_dicom_dir_path = dirpath;
  QTimer::singleShot(100, this, SLOT(displayPlaneWidgets()));
}

// -----------------------
// Test functions
// -----------------------
void MainWindow::test1() { createMultipleViewports(); }
void MainWindow::test2() {
  m_planeVisible = !m_planeVisible;
  Show3DPlaneWidgets(m_planeVisible);
}

// ------------------------------
// Display 3D plane widgets
// ------------------------------
void MainWindow::displayPlaneWidgets() {
  ReadInputDICOM();
  //set2DInteractionStyle();
  set3DInteractionStyle();
  Create2DImagePlaneWidgets();
  Create3DImagePlaneWidgets();
  m_vtkView->GetRenderWindow()->Render();
}

// ------------------------------------------
// Read DICOM image
// ------------------------------------------
void MainWindow::ReadInputDICOM() {
  m_dicom_reader = vtkSmartPointer<vtkDICOMImageReader>::New();
  m_dicom_reader->SetDirectoryName(m_dicom_dir_path.toLatin1());
  m_dicom_reader->Update();

  m_dicom_image = m_dicom_reader->GetOutput();
}

// -------------------------------------
// Create 3D image plane widgets
// -------------------------------------
void MainWindow::Create3DImagePlaneWidgets() {
  int imageDims[3];
  m_dicom_image->GetDimensions(imageDims);
  vtkRenderWindowInteractor* iren = m_vtkView->GetInteractor();

  vtkNew(vtkCellPicker, picker);
  picker->SetTolerance(0.005);
  vtkNew(vtkProperty, ipwProp);

  for (int i = 0; i < 3; i++) {
    m_3DPlaneWidget[i] = vtkSmartPointer<vtkImagePlaneWidget>::New();
    m_3DPlaneWidget[i]->SetInteractor(iren);
    m_3DPlaneWidget[i]->SetPicker(picker);
    m_3DPlaneWidget[i]->RestrictPlaneToVolumeOn();
    double color[3] = {0, 0, 0};
    color[i] = 1;
    m_3DPlaneWidget[i]->GetPlaneProperty()->SetColor(color);
    double black[3] = {0};
    m_3DPlaneWidget[i]->GetPlaneProperty()->SetAmbientColor(black);
    m_3DPlaneWidget[i]->SetTexturePlaneProperty(ipwProp);
    m_3DPlaneWidget[i]->TextureInterpolateOff();
    m_3DPlaneWidget[i]->SetResliceInterpolateToLinear();
    m_3DPlaneWidget[i]->SetInputConnection(m_dicom_reader->GetOutputPort());
    m_3DPlaneWidget[i]->SetPlaneOrientation(i);
    m_3DPlaneWidget[i]->SetSliceIndex(imageDims[i] / 2);
    m_3DPlaneWidget[i]->DisplayTextOn();
    m_3DPlaneWidget[i]->SetDefaultRenderer(m_renderers[3]);
    m_3DPlaneWidget[i]->SetWindowLevel(1708, -709);
    m_3DPlaneWidget[i]->On();
    m_3DPlaneWidget[i]->InteractionOn();
  }
  m_renderers[3]->ResetCamera();
}

// -------------------------------------
// Create 2D image plane widgets
// -------------------------------------
void MainWindow::Create2DImagePlaneWidgets() {
  int imageDims[3];
  m_dicom_image->GetDimensions(imageDims);
  vtkRenderWindowInteractor* iren = m_vtkView->GetInteractor();

  vtkNew(vtkCellPicker, picker);
  picker->SetTolerance(0.005);
  vtkNew(vtkProperty, ipwProp);

  for (int i = 0; i < 3; i++) {
    m_2DPlaneWidget[i] = vtkSmartPointer<vtkImagePlaneWidget>::New();
    m_2DPlaneWidget[i]->SetInteractor(iren);
    m_2DPlaneWidget[i]->SetPicker(picker);
    m_2DPlaneWidget[i]->RestrictPlaneToVolumeOn();
    double color[3] = {0, 0, 0};
    color[i] = 1;
    m_2DPlaneWidget[i]->GetPlaneProperty()->SetColor(color);
    double black[3] = {0};
    m_2DPlaneWidget[i]->GetPlaneProperty()->SetAmbientColor(black);
    m_2DPlaneWidget[i]->SetTexturePlaneProperty(ipwProp);
    m_2DPlaneWidget[i]->TextureInterpolateOff();
    m_2DPlaneWidget[i]->SetResliceInterpolateToLinear();
    m_2DPlaneWidget[i]->SetInputConnection(m_dicom_reader->GetOutputPort());
    m_2DPlaneWidget[i]->SetPlaneOrientation(i);
    m_2DPlaneWidget[i]->SetSliceIndex(imageDims[i] / 2);
    m_2DPlaneWidget[i]->DisplayTextOn();
    m_2DPlaneWidget[i]->SetDefaultRenderer(m_renderers[i]);
    m_2DPlaneWidget[i]->SetWindowLevel(1708, -709);
    m_2DPlaneWidget[i]->On();
    m_2DPlaneWidget[i]->InteractionOn();

    vtkNew<PlaneWidgetCallback> callback;
    callback->setWindow(this);
    m_2DPlaneWidget[i]->AddObserver(vtkCommand::InteractionEvent, callback);

    // setup camera
    vtkCamera* camera = m_renderers[i]->GetActiveCamera();
    double camera_pos[3] = {0, 0, 0};
    camera_pos[i] = -100;
    double focal_pos[3] = {0, 0, 0};
    focal_pos[i] = 100;
    camera->SetPosition(camera_pos);
    camera->SetFocalPoint(focal_pos);

    m_renderers[i]->ResetCamera();
  }
}

// ------------------------------------------
// Show / Hide 3D planes in model window
// ------------------------------------------
void MainWindow::Show3DPlaneWidgets(bool flag) {
  for (int i = 0; i < 3; i++) {
    if (flag) {
      m_3DPlaneWidget[i]->On();
    } else {
      m_3DPlaneWidget[i]->Off();
    }
  }
}

void MainWindow::createMultipleViewports() {
  double viewportlimits[4][4] = {
      {0, 0, 0.5, 0.5}, {0.5, 0, 1, 0.5}, {0, 0.5, 0.5, 1}, {0.5, 0.5, 1, 1}};

  int numViewports = 4;
  vtkNew(vtkNamedColors, colors);
  vtkSmartPointer<vtkRenderWindow> renderWindow = m_vtkView->GetRenderWindow();

  double size = 1.0 / numViewports;
  for (unsigned int i = 0; static_cast<int>(i) < numViewports; ++i) {
    m_renderers[i] = vtkSmartPointer<vtkRenderer>::New();
    m_renderers[i]->SetBackground(colors->GetColor3d("black").GetData());
    m_renderers[i]->SetViewport(viewportlimits[i]);
    ViewportBorder(m_renderers[i], colors->GetColor3d("Gold").GetData(), i);
    renderWindow->AddRenderer(m_renderers[i]);
  }

  renderWindow->Render();
}

void MainWindow::ViewportBorder(vtkSmartPointer<vtkRenderer>& renderer,
                                double* color, int index) {
  // points start at upper right and proceed anti-clockwise
  vtkNew(vtkPoints, points);
  points->SetNumberOfPoints(4);
  points->InsertPoint(0, 1, 1, 0);
  points->InsertPoint(1, 0, 1, 0);
  points->InsertPoint(2, 0, 0, 0);
  points->InsertPoint(3, 1, 0, 0);

  // create cells, and lines
  vtkNew(vtkCellArray, cells);
  cells->Initialize();
  vtkNew(vtkPolyLine, lines);

  lines->GetPointIds()->SetNumberOfIds(5);
  for (unsigned int i = 0; i < 4; ++i) {
    lines->GetPointIds()->SetId(i, i);
  }
  lines->GetPointIds()->SetId(4, 0);
  cells->InsertNextCell(lines);

  // now make tge polydata and display it
  vtkNew(vtkPolyData, poly);
  poly->Initialize();
  poly->SetPoints(points);
  poly->SetLines(cells);

  // use normalized viewport coordinates since
  // they are independent of window size
  vtkNew(vtkCoordinate, coordinate);
  coordinate->SetCoordinateSystemToNormalizedViewport();

  vtkNew(vtkPolyDataMapper2D, mapper);
  mapper->SetInputData(poly);
  mapper->SetTransformCoordinate(coordinate);

  vtkNew(vtkActor2D, actor);
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(color);
  actor->GetProperty()->SetLineWidth(2.0);  // Line Width
  m_viewBorders[index] = actor.Get();

  renderer->AddViewProp(actor);
}

int MainWindow::activeViewport(int mouseX, int mouseY) {
  // return index of active viewport fro given mouse position
  vtkSmartPointer<vtkRenderWindow> renderWindow = m_vtkView->GetRenderWindow();
  int* size = renderWindow->GetActualSize();
  int X = size[0];
  int Y = size[1];
  // cout << "\nmx:" << mouseX << " my:" << mouseY << " screen:" << X << " " <<
  // Y;
  int midX = X / 2;
  int midY = Y / 2;
  int viewportIndex = 0;
  if (mouseX <= midX && mouseY <= midY) {
    viewportIndex = 0;
  } else if (mouseX > midX && mouseY <= midY) {
    viewportIndex = 1;
  } else if (mouseX <= midX && mouseY > midY) {
    viewportIndex = 2;
  } else {
    viewportIndex = 3;
  }
  cout << "\n ViewportIndex:" << viewportIndex;
  return viewportIndex;
}

void MainWindow::hightlightActivePort(int mouseX, int mouseY) {
  int activePortIndex = activeViewport(mouseX, mouseY);
  if (activePortIndex != m_activeViewportIndex) {
    m_activeViewportIndex = activePortIndex;
    resetViewportBorders(activePortIndex);
    vtkActor2D* activeBorder = m_viewBorders[activePortIndex];
    activeBorder->GetProperty()->SetColor(0, 1, 1);
    activeBorder->GetProperty()->SetLineWidth(4.0);  // Line Width
    activeBorder->Modified();

    if (m_activeViewportIndex == 3) {
      set3DInteractionStyle();
    } else {
      set2DInteractionStyle();
    }
    m_vtkView->GetRenderWindow()->Render();
  }
}

void MainWindow::resetViewportBorders(int exception) {
  for (int i = 0; i < 4; i++) {
    if (i == exception) continue;
    vtkActor2D* activeBorder = m_viewBorders[i];
    activeBorder->GetProperty()->SetColor(1, 1, 0);
    activeBorder->GetProperty()->SetLineWidth(2.0);  // Line Width
    activeBorder->Modified();
  }
}

void MainWindow::setCurrentPosition(double pos[4]) {
  int imageDims[3];
  m_dicom_image->GetDimensions(imageDims);
  for (int i = 0; i < 3; i++) {
    int newpos = (int)pos[i];
    int oldpos = m_2DPlaneWidget[i]->GetSliceIndex();
    if (oldpos != newpos && newpos < imageDims[i] && newpos>=0) {
      m_2DPlaneWidget[i]->SetSliceIndex(newpos);
      m_2DPlaneWidget[i]->Modified();

      m_3DPlaneWidget[i]->SetSliceIndex(newpos);
      m_3DPlaneWidget[i]->Modified();
    }
  }
}

void MainWindow::set3DInteractionStyle() {
  vtkNew(MouseInteractorStyle, style);
  m_vtkView->GetInteractor()->SetInteractorStyle(style);
  style->setWindow(this);
  m_vtkView->GetInteractor()->Render();
}

void MainWindow::set2DInteractionStyle() {
  vtkNew<DicomInteractionStyle> style;
  m_vtkView->GetInteractor()->SetInteractorStyle(style);
  style->setWindow(this);
  m_vtkView->GetInteractor()->Render();
}
