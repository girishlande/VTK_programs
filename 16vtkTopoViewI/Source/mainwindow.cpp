#include <QBoxLayout.h>
#include <QComboBox.h>
#include <QDir.h>
#include <QFileDialog.h>
#include <QGridLayout.h>
#include <QSplitter.h>
#include <QGroupBox.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <QPushButton>
#include <QScrollBar>
#include <QStringList>
#include <QTimer>
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
#include <sstream>

#include <QVTKWidget.h>
#include <vtkCamera.h>
#include <vtkCellPicker.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapper.h>
#include <vtkLineSource.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkPolyLine.h>
#include <vtkProperty2D.h>

#include "mainwindow.h"
#include "avTopoViewer.h"
#include "dicominteractionstyle.h"
#include "modelinteractionstyle.h"

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

  QRadioButton* rx = new QRadioButton("X Axis", this);
  QRadioButton* ry = new QRadioButton("Y Axis", this);
  QRadioButton* rz = new QRadioButton("Z Axis", this);
  QGroupBox* groupBox = new QGroupBox(tr("Viewing Direction"));
  QVBoxLayout* vbox = new QVBoxLayout;
  vbox->addWidget(rx);
  vbox->addWidget(ry);
  vbox->addWidget(rz);
  groupBox->setLayout(vbox);
  tablayout->addWidget(groupBox);

  QRadioButton* tx = new QRadioButton("X Axis", this);
  QRadioButton* ty = new QRadioButton("Y Axis", this);
  QRadioButton* tz = new QRadioButton("Z Axis", this);
  QGroupBox* topogroupBox = new QGroupBox(tr("Topo Direction"));
  QVBoxLayout* vbox1 = new QVBoxLayout;
  vbox1->addWidget(tx);
  vbox1->addWidget(ty);
  vbox1->addWidget(tz);
  topogroupBox->setLayout(vbox1);
  tablayout->addWidget(topogroupBox);

  QVBoxLayout* vbox2 = new QVBoxLayout;
  QLabel *label = new QLabel("test", this);
  vbox2->addStretch(1);
  label->setLayout(vbox2);
  tablayout->addWidget(label);

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

void FetchXZImage(vtkSmartPointer<vtkImageData> input,
                                vtkSmartPointer<vtkImageData> output) {
  short* data = static_cast<short*>(input->GetScalarPointer(0, 0, 0));
  int* dims = input->GetDimensions();
  int xdim = dims[0];
  int ydim = dims[1];
  int zdim = dims[2];

  // Specify the size of the image data
  output->SetDimensions(xdim, zdim, 1);
  output->AllocateScalars(VTK_SHORT, 1);
  output->SetSpacing(input->GetSpacing());
  output->SetOrigin(input->GetOrigin());

  // Fill every entry of the image data with "2.0"
  for (int z = 0; z < zdim; z++) {
    for (int x = 0; x < xdim; x++) {
      short* pixel = static_cast<short*>(output->GetScalarPointer(x, z, 0));
      short* sourcepixel =
          static_cast<short*>(input->GetScalarPointer(x, ydim / 2, z));
      pixel[0] = sourcepixel[0];
    }
  }
}

// -----------------------
// Test functions
// -----------------------
void MainWindow::test1() {
  if (!m_topoviewer) {
    m_topoviewer =
        new avTopoViewer(m_dicom_image, m_vtkImageViewer->GetRenderer());
    m_topoviewer->SetDirectionAxis(avTopoViewer::DirectionAxis::Z_AXIS,
                                   avTopoViewer::DirectionAxis::Y_AXIS);

    vtkNew<vtkImageData> topoImage;
    FetchXZImage(m_dicom_image, topoImage);
    m_topoviewer->SetTopoImage(topoImage);

    //m_topoviewer->SetViewSize(0.2, 0.2);
    //m_topoviewer->SetTopoPosition(0.05, 0.05);
    //m_topoviewer->SetBorderColor(std::string("pink"));
    //m_topoviewer->SetTopoLineColor(std::string("green"));
    

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

  imageViewer->GetRenderer()->SetViewport(0.5, 0, 1, 0.5);
  imageViewer->GetRenderer()->SetBackground(1, 1, 0);
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
    if (m_topoviewer) m_topoviewer->UpdateTopoView(value);
  }
  m_vtkImageViewer->GetRenderWindow()->Render();
}

void MainWindow::updateSlider(int value) { m_slider->setValue(value); }

void MainWindow::SetViewDirection(int direction) {

}