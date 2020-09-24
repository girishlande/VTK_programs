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

#include "QVTKWidget.h"
#include "dicominteractionstyle.h"
#include "modelinteractionstyle.h"
#include "vtkCellPicker.h"
#include "vtkImageData.h"
#include "vtkImageMapper.h"
#include "vtkLineSource.h"
#include "vtkNamedColors.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPolyLine.h"
#include "vtkProperty2D.h"
#include "vtkCamera.h"

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

  // Get interactor and set Interaction style
  vtkRenderWindowInteractor* iren = m_vtkView->GetInteractor();
  vtkNew(MouseInteractorStyle, style);
  iren->SetInteractorStyle(style);

  // Create Dicom image plane widgets in view no 4
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

  int numberOfFiles = 4;
  vtkNew(vtkNamedColors, colors);
  vtkSmartPointer<vtkRenderWindow> renderWindow = m_vtkView->GetRenderWindow();

  double size = 1.0 / numberOfFiles;
  for (unsigned int i = 0; static_cast<int>(i) < numberOfFiles; ++i) {
    m_renderers[i] = vtkSmartPointer<vtkRenderer>::New();
    m_renderers[i]->SetBackground(colors->GetColor3d("black").GetData());
    m_renderers[i]->SetViewport(viewportlimits[i]);
    ViewportBorder(m_renderers[i], colors->GetColor3d("Gold").GetData(), true);
    renderWindow->AddRenderer(m_renderers[i]);
  }

  renderWindow->Render();
}

void MainWindow::ViewportBorder(vtkSmartPointer<vtkRenderer>& renderer,
                                double* color, bool last) {
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

  // only draw last line if this is the last viewport
  // this prevents double vertical lines at right border
  // if different colors are used for each border, then do
  // not specify last
  if (last) {
    lines->GetPointIds()->SetNumberOfIds(5);
  } else {
    lines->GetPointIds()->SetNumberOfIds(4);
  }
  for (unsigned int i = 0; i < 4; ++i) {
    lines->GetPointIds()->SetId(i, i);
  }
  if (last) {
    lines->GetPointIds()->SetId(4, 0);
  }
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
  actor->GetProperty()->SetLineWidth(4.0);  // Line Width

  renderer->AddViewProp(actor);
}