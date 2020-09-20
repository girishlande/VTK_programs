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
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

#include <QPushButton>
#include <QScrollBar>
#include <QStringList>
#include <QTimer>
#include <sstream>

#include "QVTKWidget.h"
#include "TopoViewer.h"
#include "dicominteractionstyle.h"
#include "modelinteractionstyle.h"
#include "vtkCamera.h"
#include "vtkCellPicker.h"
#include "vtkImageActor.h"
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
  m_slider = new QScrollBar(this);
  m_slider->setFixedWidth(20);
  connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged(int)));

  m_container_layout = new QGridLayout;
  m_container_layout->addWidget(m_vtkView, 0, 0, 1, 1);
  m_container_layout->addWidget(m_slider, 0, 1, 1, 1);
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
}

// ------------------------
// destructor and exit
// ------------------------
MainWindow::~MainWindow() {
  delete ui;
  if (m_topoviewer) {
    delete m_topoviewer;
  }
}

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
  QTimer::singleShot(100, this, SLOT(ProcessInput()));
}

// -----------------------
// Test functions
// -----------------------
void MainWindow::test1() {
  if (!m_topoviewer) {
    vtkRenderer* r = m_vtkImageViewer->GetRenderer();
    m_topoviewer = new TopoViewer(m_dicom_image, r);
    m_topoviewer->Start();
  }
}

void MainWindow::test2() {}

    // ------------------------------
// Display 3D plane widgets
// ------------------------------
void MainWindow::ProcessInput() {
  // Read DICOM file
  ReadInputDICOM();

  // Get interactor and set Interaction style
  vtkRenderWindowInteractor* iren = m_vtkView->GetInteractor();
  vtkNew(myVtkInteractorStyleImage, style);
  iren->SetInteractorStyle(style);
  style->setWindow(this);

  UpdateViewForDICOM();

  m_vtkView->GetRenderWindow()->Render();
}

void MainWindow::UpdateViewForDICOM() {
  vtkNew(vtkDICOMImageReader, reader);
  reader->SetDirectoryName(m_dicom_dir_path.toLatin1());
  reader->Update();

  vtkNew(vtkImageViewer2, imageViewer);
  imageViewer->SetInputConnection(reader->GetOutputPort());
  imageViewer->SetSliceOrientationToXY();
  m_vtkImageViewer = imageViewer.Get();

  // create an interactor with our own style (inherit from
  // vtkInteractorStyleImage) in order to catch mousewheel and key events
  vtkNew(vtkRenderWindowInteractor, renderWindowInteractor);
  vtkNew(myVtkInteractorStyleImage, myInteractorStyle);
  m_interaction = myInteractorStyle.Get();
  myInteractorStyle->setWindow(this);
  myInteractorStyle->SetImageViewer(imageViewer);
  renderWindowInteractor->SetInteractorStyle(myInteractorStyle);

  InitialiseSlider();
  InitialiseCornerText();

  imageViewer->SetRenderWindow(m_vtkView->GetRenderWindow());
  m_vtkView->GetRenderWindow()->SetInteractor(renderWindowInteractor);

  // initialize rendering and interaction
  imageViewer->Render();
  imageViewer->GetRenderer()->ResetCamera();
  renderWindowInteractor->Start();
}

void MainWindow::InitialiseCornerText() {
  vtkNew(vtkTextProperty, sliceTextProp);
  sliceTextProp->SetFontFamilyToCourier();
  sliceTextProp->SetFontSize(20);
  sliceTextProp->SetVerticalJustificationToBottom();
  sliceTextProp->SetJustificationToLeft();

  vtkNew(vtkTextMapper, sliceTextMapper);
  std::string msg = StatusMessage::Format(m_vtkImageViewer->GetSliceMin(),
                                          m_vtkImageViewer->GetSliceMax());
  sliceTextMapper->SetInput(msg.c_str());
  sliceTextMapper->SetTextProperty(sliceTextProp);
  vtkNew(vtkActor2D, sliceTextActor);
  sliceTextActor->SetMapper(sliceTextMapper);
  sliceTextActor->SetPosition(15, 10);

  m_interaction->SetStatusMapper(sliceTextMapper);
  m_vtkImageViewer->GetRenderer()->AddActor2D(sliceTextActor);
}

void MainWindow::InitialiseSlider() {
  m_minSliceNumber = m_vtkImageViewer->GetSliceMin();
  m_maxSliceNumber = m_vtkImageViewer->GetSliceMax();
  m_slider->setMinimum(m_minSliceNumber);
  m_slider->setMaximum(m_maxSliceNumber);
  m_slider->setValue(m_minSliceNumber);
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

// ----------------------------------
// Handle slider position change
// ----------------------------------
void MainWindow::sliderChanged(int value) {
  if (m_vtkImageViewer) {
    m_vtkImageViewer->SetSlice(value);
    if (m_interaction) {
      m_interaction->updateSliceMsg(value);
    }
    if (m_topoviewer) m_topoviewer->updateAxialLine(value);
  }
  m_vtkImageViewer->GetRenderWindow()->Render();
}

void MainWindow::updateSlider(int value) { m_slider->setValue(value); }

