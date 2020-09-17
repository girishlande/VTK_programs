#include "mainwindow.h"

#include <QBoxLayout.h>
#include <QComboBox.h>
#include <QDir.h>
#include <QFileDialog.h>
#include <QGridLayout.h>
#include <QSplitter.h>
#include <QStackedWidget.h>
#include <qdebug.h>
#include <qscrollbar.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkConeSource.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageViewer2.h>
#include <vtkInteractorStyleImage.h>
#include <vtkOBJReader.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSTLReader.h>
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
#include "vtkImageData.h"
#include "vtkImageMapper.h"
#include "vtkLineSource.h"
#include "vtkNamedColors.h"
#include <vtkCamera.h>

#include <vtkArrowSource.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkMath.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <time.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLineSource.h>

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
  m_slider = new QScrollBar(this);
  m_slider->setFixedWidth(20);
  m_slider->setMaximum(360);
  m_slider->setMinimum(0);
  connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged(int)));

  m_container_layout = new QGridLayout;
  m_container_layout->addWidget(m_vtkView, 0, 0, 1, 1);
  m_container_layout->addWidget(m_slider, 0, 1, 1, 1);
  QWidget* container = new QWidget;
  container->setLayout(m_container_layout);

  QVBoxLayout* tablayout = new QVBoxLayout();
  QPushButton* b1 = new QPushButton("Test Button", this);
  connect(b1, SIGNAL(clicked(bool)), this, SLOT(test1()));
  QPushButton* b2 = new QPushButton("Test Button2", this);
  connect(b2, SIGNAL(clicked(bool)), this, SLOT(test2()));
  QPushButton* b3 = new QPushButton("X View", this);
  connect(b3, SIGNAL(clicked(bool)), this, SLOT(XView()));
  QPushButton* b4 = new QPushButton("Y View", this);
  connect(b4, SIGNAL(clicked(bool)), this, SLOT(YView()));
  QPushButton* b5 = new QPushButton("Z View", this);
  connect(b5, SIGNAL(clicked(bool)), this, SLOT(ZView()));


  setUpSlider(&m_sliderX);
  connect(m_sliderX, SIGNAL(valueChanged(int)), this, SLOT(sliderChangedX(int)));
  setUpSlider(&m_sliderY);
  connect(m_sliderY, SIGNAL(valueChanged(int)), this, SLOT(sliderChangedY(int)));
  setUpSlider(&m_sliderZ);
  connect(m_sliderZ, SIGNAL(valueChanged(int)), this, SLOT(sliderChangedZ(int)));


  tablayout->addWidget(b1);
  tablayout->addWidget(b2);
  tablayout->addWidget(b3);
  tablayout->addWidget(b4);
  tablayout->addWidget(b5);
  tablayout->addWidget(m_sliderX);
  tablayout->addWidget(m_sliderY);
  tablayout->addWidget(m_sliderZ);

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

  InitialiseView();
  addCoordinateSystem();

}

#include <vtkArrowSource.h>
void MainWindow::InitialiseView() {
  /*vtkNew(vtkSphereSource, sphere);
  sphere->SetCenter(20, 20, 0);
  sphere->SetRadius(20);
  sphere->Update();*/

  vtkNew<vtkArrowSource> arrow;
  vtkNew(vtkPolyDataMapper, mapper);
  mapper->SetInputConnection(arrow->GetOutputPort());
  vtkNew(vtkActor, actor);
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(0, 1, 0);
  vtkNew(vtkRenderer, renderer);
  m_renderer = renderer.Get();
  renderer->AddActor(actor);
  this->m_vtkView->GetRenderWindow()->AddRenderer(renderer);
  m_vtkView->GetRenderWindow()->Render();

  cacheCameraPosition();
}

MainWindow::~MainWindow() { delete ui; }
void MainWindow::on_actionExit_triggered() { QApplication::quit(); }

void MainWindow::on_actionOpen_DICOM_file_triggered() {
  QDir dir;
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
  std::cout << "Girish:" << m_dicom_dir_path.toStdString();

  QTimer::singleShot(100, this, SLOT(UpdateViewForDICOM()));
}

void MainWindow::UpdateViewForDICOM() {

}

void MainWindow::sliderChanged(int value) {
    // get the camera
    // get camera position 
    // get camera focal point
    // Change position and change focal point
 
    vtkCamera* camera = m_renderer->GetActiveCamera();
    double z = camera->GetParallelScale();
    camera->Zoom(0.3 + (double)value*0.001);
    refreshView();

}

void MainWindow::sliderChangedX(int value) {
  // get the camera
  // get camera position
  // get camera focal point
  // Change position and change focal point

  vtkCamera* camera = m_renderer->GetActiveCamera();
  double camerapos[3];
  camera->GetPosition(camerapos);
  double focalpos[3];
  camera->GetFocalPoint(focalpos);

  double newPos[3];
  double direction[3] = {0, 0, -1};
  vtkMath::MultiplyScalar(direction, value*.001);
  vtkMath::Add(m_cameraPosition, direction, newPos);

  camera->SetDistance(m_distance + value);

  m_vtkView->GetRenderWindow()->Render();
  refreshView();

}

void MainWindow::sliderChangedY(int value) {
  // get the camera
  // get camera position
  // get camera focal point
  // Change position and change focal point
  vtkCamera* camera = m_renderer->GetActiveCamera();
  camera->SetRoll(m_roll + value);
  refreshView();
}

void MainWindow::sliderChangedZ(int value) {
  // get the camera
  // get camera position
  // get camera focal point
  // Change position and change focal point
  vtkCamera* camera = m_renderer->GetActiveCamera();
  camera->SetViewAngle(m_viewAngle + value);
  refreshView();
}

void MainWindow::updateSlider(int value) { m_slider->setValue(value); }

void MainWindow::test1() { 
    m_sphereCounter++;
    double dir[3] = {m_radius*m_sphereCounter, 0, 0};
    double position[3];
    vtkMath::Add(m_position, dir, position);
    AddSphereActor(m_renderer,position); 

}

void MainWindow::AddSphereActor(vtkRenderer* renderer,double position[3]) {
  vtkNew(vtkSphereSource, sphere);
  sphere->SetCenter(position);
  sphere->SetRadius(m_radius/2);
  sphere->Update();

  vtkNew(vtkPolyDataMapper, mapper);
  mapper->SetInputConnection(sphere->GetOutputPort());
  vtkNew(vtkActor, actor);
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(1, 0, 0);
  renderer->AddActor(actor);

  refreshView();
}


void MainWindow::test2() {
  m_renderer->ResetCamera();
  refreshView();
}


void MainWindow::setUpSlider(QScrollBar** slider) {
  *slider = new QScrollBar(this);
  (*slider)->setFixedHeight(20);
  (*slider)->setMaximum(360);
  (*slider)->setMinimum(0);
  (*slider)->setOrientation(Qt::Horizontal);
}

void MainWindow::cacheCameraPosition() {
  vtkCamera* c = m_renderer->GetActiveCamera();
  c->GetPosition(m_cameraPosition);
  c->GetFocalPoint(m_cameraFocalPoint);
  m_distance = c->GetDistance();
  m_roll = c->GetRoll();
  m_viewAngle = c->GetViewAngle();
  m_parallelScale = c->GetParallelScale();
}

void MainWindow::refreshView() {
  m_renderer->ResetCamera();
  m_vtkView->GetRenderWindow()->Render();
}

void MainWindow::XView() {
  vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
  camera->SetPosition(50, 0, 0);
  camera->SetFocalPoint(0, 0, 0);
  m_renderer->SetActiveCamera(camera);

  refreshView();
}
void MainWindow::ZView() {
  vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
  camera->SetPosition(0, 0, 50);
  camera->SetFocalPoint(0, 0, 0);
  m_renderer->SetActiveCamera(camera);

  refreshView();
}
void MainWindow::YView() {
  vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
  camera->SetPosition(0, 50, 0);
  camera->SetFocalPoint(0, 0, 0);
  m_renderer->SetActiveCamera(camera);
  refreshView();
}

void MainWindow::AddLine(double pt1[3], double pt2[3], double color[3]) {
  double vec[3];
  vtkMath::Subtract(pt1, pt2, vec);
  vtkMath::Normalize(vec);
  vtkMath::MultiplyScalar(vec, 10);
  double new_pt2[3];
  vtkMath::Subtract(pt1, vec, new_pt2);

  vtkSmartPointer<vtkLineSource> lineSource =
      vtkSmartPointer<vtkLineSource>::New();
  lineSource->SetPoint1(pt1);
  lineSource->SetPoint2(new_pt2);
  lineSource->Update();

  // Visualize
  vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(lineSource->GetOutputPort());
  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetLineWidth(4);
  actor->GetProperty()->SetColor(color);
  m_renderer->AddActor(actor);
}

void MainWindow::AddCS1() {
  double center[3] = {0, 0, 0};
  double pt1[3] = {10, 0, 0};
  double pt2[3] = {0, 10, 0};
  double pt3[3] = {0, 0, 10};

  double color1[3] = {1, 0, 0};
  double color2[3] = {0, 1, 0};
  double color3[3] = {0, 0, 1};

  AddLine(center, pt1, color1);
  AddLine(center, pt2, color2);
  AddLine(center, pt3, color3);
}

void MainWindow::AddCS2() {
  double center[3] = {10, 10, 10};
  double pt1[3] = {20, 0, 0};
  double pt2[3] = {0, 20, 0};
  double pt3[3] = {0, 0, 20};

  double v1[3] = {1.0, 1.0, 0};
  double v2[3];
  double v3[3];
  vtkMath::Perpendiculars(v1, v2, v3, 45);
  vtkMath::Normalize(v1);
  vtkMath::Normalize(v2);
  vtkMath::Normalize(v3);

  double length = 10;
  vtkMath::MultiplyScalar(v1, length);
  vtkMath::MultiplyScalar(v2, length);
  vtkMath::MultiplyScalar(v3, length);

  vtkMath::Add(center, v1, pt1);
  vtkMath::Add(center, v2, pt2);
  vtkMath::Add(center, v3, pt3);

  double color[3] = {0, 1, 0};
  AddLine(center, pt1, color);
  AddLine(center, pt2, color);
  AddLine(center, pt3, color);
}



void MainWindow::addCoordinateSystem() {
  // Create an arrow.
  vtkSmartPointer<vtkArrowSource> arrowSource =
      vtkSmartPointer<vtkArrowSource>::New();

  // Generate a random start and end point
  double startPoint[3], endPoint[3];
#ifndef main
  vtkMath::RandomSeed(time(NULL));
#else
  vtkMath::RandomSeed(8775070);
#endif
  startPoint[0] = vtkMath::Random(-10, 10);
  startPoint[1] = vtkMath::Random(-10, 10);
  startPoint[2] = vtkMath::Random(-10, 10);
  endPoint[0] = vtkMath::Random(-10, 10);
  endPoint[1] = vtkMath::Random(-10, 10);
  endPoint[2] = vtkMath::Random(-10, 10);

  // Compute a basis
  double normalizedX[3];
  double normalizedY[3];
  double normalizedZ[3];

  // The X axis is a vector from start to end
  vtkMath::Subtract(endPoint, startPoint, normalizedX);
  double length = vtkMath::Norm(normalizedX);
  vtkMath::Normalize(normalizedX);

  // The Z axis is an arbitrary vector cross X
  double arbitrary[3];
  arbitrary[0] = vtkMath::Random(-10, 10);
  arbitrary[1] = vtkMath::Random(-10, 10);
  arbitrary[2] = vtkMath::Random(-10, 10);
  vtkMath::Cross(normalizedX, arbitrary, normalizedZ);
  vtkMath::Normalize(normalizedZ);

  // The Y axis is Z cross X
  vtkMath::Cross(normalizedZ, normalizedX, normalizedY);
  vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();

  // Create the direction cosine matrix
  matrix->Identity();
  for (unsigned int i = 0; i < 3; i++) {
    matrix->SetElement(i, 0, normalizedX[i]);
    matrix->SetElement(i, 1, normalizedY[i]);
    matrix->SetElement(i, 2, normalizedZ[i]);
  }

  // Apply the transforms
  vtkSmartPointer<vtkTransform> transform =
      vtkSmartPointer<vtkTransform>::New();
  transform->Translate(startPoint);
  transform->Concatenate(matrix);
  transform->Scale(length, length, length);

  // Transform the polydata
  vtkSmartPointer<vtkTransformPolyDataFilter> transformPD =
      vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  transformPD->SetTransform(transform);
  transformPD->SetInputConnection(arrowSource->GetOutputPort());

  // Create a mapper and actor for the arrow
  vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
#ifdef USER_MATRIX
  mapper->SetInputConnection(arrowSource->GetOutputPort());
  actor->SetUserMatrix(transform->GetMatrix());
#else
  mapper->SetInputConnection(transformPD->GetOutputPort());
#endif
  actor->SetMapper(mapper);

  // Create spheres for start and end point
  vtkSmartPointer<vtkSphereSource> sphereStartSource =
      vtkSmartPointer<vtkSphereSource>::New();
  sphereStartSource->SetCenter(startPoint);
  vtkSmartPointer<vtkPolyDataMapper> sphereStartMapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  sphereStartMapper->SetInputConnection(sphereStartSource->GetOutputPort());
  vtkSmartPointer<vtkActor> sphereStart = vtkSmartPointer<vtkActor>::New();
  sphereStart->SetMapper(sphereStartMapper);
  sphereStart->GetProperty()->SetColor(1.0, 1.0, .3);

  vtkSmartPointer<vtkSphereSource> sphereEndSource =
      vtkSmartPointer<vtkSphereSource>::New();
  sphereEndSource->SetCenter(endPoint);
  vtkSmartPointer<vtkPolyDataMapper> sphereEndMapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  sphereEndMapper->SetInputConnection(sphereEndSource->GetOutputPort());
  vtkSmartPointer<vtkActor> sphereEnd = vtkSmartPointer<vtkActor>::New();
  sphereEnd->SetMapper(sphereEndMapper);
  sphereEnd->GetProperty()->SetColor(1.0, .3, .3);

  
  AddCS1();
  //AddCS2();

}