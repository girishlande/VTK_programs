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
  setUpSlider(&m_sliderX);
  connect(m_sliderX, SIGNAL(valueChanged(int)), this, SLOT(sliderChangedX(int)));
  setUpSlider(&m_sliderY);
  connect(m_sliderY, SIGNAL(valueChanged(int)), this, SLOT(sliderChangedY(int)));
  setUpSlider(&m_sliderZ);
  connect(m_sliderZ, SIGNAL(valueChanged(int)), this, SLOT(sliderChangedZ(int)));


  tablayout->addWidget(b1);
  tablayout->addWidget(b2);
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

void MainWindow::test1() { AddSphereActor(m_renderer); }

void MainWindow::AddSphereActor(vtkRenderer* renderer) {
  vtkNew(vtkSphereSource, sphere);
  sphere->SetCenter(100, 100, 0);
  sphere->SetRadius(20);
  sphere->Update();

  vtkNew(vtkPolyDataMapper, mapper);
  mapper->SetInputConnection(sphere->GetOutputPort());
  vtkNew(vtkActor, actor);
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(1, 0, 0);
  renderer->AddActor(actor);
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

void MainWindow::refreshView() { m_vtkView->GetRenderWindow()->Render(); }