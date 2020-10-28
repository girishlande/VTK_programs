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
#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
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
#include "vtkActor2D.h"
#include "vtkImageData.h"
#include "vtkImageMapper.h"
#include "vtkImageResize.h"
#include "vtkImageSincInterpolator.h"
#include "vtkLineSource.h"
#include "vtkNamedColors.h"
#include "vtkPNGReader.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPolyLine.h"
#include "vtkProperty2D.h"

#define AV_MIN_INT16 -32768L

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
  QPushButton* b1 = new QPushButton("Resize Using Aspect Ration", this);
  connect(b1, SIGNAL(clicked(bool)), this, SLOT(test1()));
  QPushButton* b2 = new QPushButton("Test Button2", this);
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

  InitialiseView();
  // initialiseWithDICOM();
}

void MainWindow::initialiseWithDICOM() {
  m_dicom_dir_path = "D:/git/QtProjects/12vtkQTDemo3/Source/models/series201";
  QTimer::singleShot(100, this, SLOT(UpdateViewForDICOM()));
}

void MainWindow::InitialiseView() { MultipleViewports(); }

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

vtkSmartPointer<vtkImageData> MainWindow::resizeImage(
    vtkSmartPointer<vtkImageData>& input) {
  vtkSmartPointer<vtkImageSincInterpolator> interpolator =
      vtkSmartPointer<vtkImageSincInterpolator>::New();

  // interpolator->SetWindowFunction(0);  // 0 to 10

  vtkSmartPointer<vtkImageResize> resize =
      vtkSmartPointer<vtkImageResize>::New();
  double spacing[3];
  input->GetSpacing(spacing);
  int extent[6];
  input->GetExtent(extent);

  resize->SetInputData(input);
  if (spacing[0] != spacing[1] || extent[1] != extent[3]) {
    resize->SetInputData(input);
    resize->SetResizeMethodToOutputSpacing();
    resize->SetOutputSpacing(1, 1, 1);
    resize->InterpolateOn();
    resize->SetInterpolator(interpolator);
    resize->Update();
    vtkSmartPointer<vtkImageData> t = vtkSmartPointer<vtkImageData>::New();
    t->DeepCopy(resize->GetOutput());
    int dim[3];
    t->GetDimensions(dim);
    int dim2[3] = {dim[0], dim[1], dim[2]};
    int ygap = 0;
    int xgap = 0;
    if (dim2[0] > dim2[1]) {
      ygap = (dim2[0] - dim2[1]) / 2;
      dim2[1] = dim2[0];
    } else {
      xgap = (dim2[1] - dim2[0]) / 2;
      dim2[0] = dim2[1];
    }
    vtkSmartPointer<vtkImageData> t2 = vtkSmartPointer<vtkImageData>::New();
    t2->SetExtent(0, dim2[0] - 1, 0, dim2[1] - 1, 0, dim2[2] - 1);
    t2->SetSpacing(1, 1, 1);
    t2->AllocateScalars(VTK_SHORT, 1);
    for (int z = 0; z < dim2[2]; z++)
      for (int y = 0; y < dim2[1]; y++)
        for (int x = 0; x < dim2[0]; x++) {
          qint16 inputVoxelv = AV_MIN_INT16;
          if ((x - xgap) < dim[0] && (x - xgap) >= 0 && (y - ygap) < dim[1] &&
              (y - ygap) >= 0) {
            qint16* inputVoxel =
                (qint16*)t->GetScalarPointer(x - xgap, (y - ygap), z);
            inputVoxelv = inputVoxel[0];
            qint16* outputVoxel = (qint16*)t2->GetScalarPointer(x, y, z);
            outputVoxel[0] = inputVoxelv;

            double* i1 = static_cast<double*>(
                t->GetScalarPointer(x - xgap, (y - ygap), z));
            double* o1 = static_cast<double*>(t2->GetScalarPointer(x, y, z));
            o1[0] = i1[0];

          } else {
            qint16* outputVoxel = (qint16*)t2->GetScalarPointer(x, y, z);
            outputVoxel[0] = inputVoxelv;
          }
        }

    resize = vtkSmartPointer<vtkImageResize>::New();
    resize->SetInputData(t2);
  }

  resize->SetOutputDimensions(200, 200, 1);
  resize->InterpolateOn();
  resize->SetInterpolator(interpolator);
  resize->Update();
  vtkSmartPointer<vtkImageData> out = resize->GetOutput();

  return out;
}

vtkSmartPointer<vtkImageData> MainWindow::ResizeMe(
    vtkSmartPointer<vtkImageData> input) {
  int newWidth = 200;
  int* D = input->GetDimensions();
  double nh = (double)(D[1] * newWidth) / D[0];
  int newHeight = ceil(nh);
  vtkNew<vtkImageResize> resize;
  resize->SetInputData(input);
  resize->SetOutputDimensions(newWidth, newHeight, 1);
  resize->Update();
  vtkSmartPointer<vtkImageData> output = resize->GetOutput();
  return output;
}

void MainWindow::UpdateViewForDICOM() {
  vtkNew(vtkDICOMImageReader, reader);
  reader->SetDirectoryName(m_dicom_dir_path.toLatin1());
  reader->Update();

  m_imageData = reader->GetOutput();
  ResizeUsingInterpolation();
}

void MainWindow::ResizeUsingInterpolation() {
  vtkSmartPointer<vtkImageData> v1 = m_imageData;
  AddImageInLeftRenderer(v1);
  PrintImageDetails(v1);

  vtkSmartPointer<vtkImageData> v2 = resizeImage(v1);
  AddImageInRightRenderer(v2);
  PrintImageDetails(v2);
}

void MainWindow::sliderChanged(int value) {
  if (m_vtkImageViewer) {
    m_vtkImageViewer->SetSlice(value);
    myVtkInteractorStyleImage* style =
        (myVtkInteractorStyleImage*)m_vtkView->GetRenderWindow()
            ->GetInteractor()
            ->GetInteractorStyle();
    if (style) {
      style->updateSliceMsg(value);
    }
  }
}

void MainWindow::updateSlider(int value) { m_slider->setValue(value); }

void MainWindow::test1() {
  vtkSmartPointer<vtkImageData> v1 = m_imageData;
  AddImageInLeftRenderer(v1);
  PrintImageDetails(v1);

  vtkSmartPointer<vtkImageData> v2 = ResizeMe(v1);
  AddImageInRightRenderer(v2);
  PrintImageDetails(v2);
}

void MainWindow::prepareImageData(vtkSmartPointer<vtkImageData>& input,
                                  int direction,
                                  vtkSmartPointer<vtkImageData>& output) {
  short* data = static_cast<short*>(input->GetScalarPointer(0, 0, 0));
  int* dims = input->GetDimensions();
  int xdim = dims[0];
  int ydim = dims[1];
  int zdim = dims[2];

  // Specify the size of the image data
  output->SetDimensions(xdim, ydim, 1);
  output->AllocateScalars(VTK_SHORT, 1);
  output->SetSpacing(input->GetSpacing());
  output->SetOrigin(input->GetOrigin());

  // Fill every entry of the image data with "2.0"
  for (int y = 0; y < ydim; y++) {
    for (int x = 0; x < xdim; x++) {
      short* pixel = static_cast<short*>(output->GetScalarPointer(x, y, 0));
      short* sourcepixel =
          static_cast<short*>(input->GetScalarPointer(x, y, 0));
      pixel[0] = sourcepixel[0];
    }
  }
}

void MainWindow::MultipleViewports() {
  vtkNew(vtkRenderer, ren1);
  ren1->SetBackground(1.0, 1.0, 0);
  ren1->SetViewport(0, 0, 0.498, 1.0);
  m_leftrenderer = ren1.Get();

  vtkNew(vtkRenderer, ren2);
  ren2->SetBackground(0, 1.0, 1.0);
  ren2->SetViewport(0.502, 0, 1.0, 1.0);
  m_rightrenderer = ren2.Get();

  vtkSmartPointer<vtkRenderWindow> renderWindow = m_vtkView->GetRenderWindow();
  renderWindow->AddRenderer(ren1);
  renderWindow->AddRenderer(ren2);

  vtkNew(myVtkInteractorStyleImage, myInteractorStyle);
  m_vtkView->GetRenderWindow()->GetInteractor()->SetInteractorStyle(
      myInteractorStyle);
}

void MainWindow::AddImageInLeftRenderer(vtkSmartPointer<vtkImageData> input) {
  bool twoD = false;
  if (twoD) {
    if (m_left_actor) m_leftrenderer->RemoveActor2D(m_left_actor);
    vtkNew(vtkImageMapper, imageMapper);
    imageMapper->SetInputData(input);
    imageMapper->SetColorWindow(255);
    imageMapper->SetColorLevel(127.5);

    vtkNew(vtkActor2D, imageActor);
    imageActor->SetMapper(imageMapper);
    imageActor->SetPosition(20, 20);
    m_leftrenderer->AddActor2D(imageActor);
    m_left_actor = imageActor.Get();
  } else {
    if (m_leftImageActor) m_leftrenderer->RemoveActor(m_leftImageActor);
    vtkNew<vtkImageActor> imageActor;
    imageActor->GetMapper()->SetInputData(input);
    m_leftrenderer->AddActor(imageActor);
    m_leftImageActor = imageActor.Get();
  }

  m_leftrenderer->ResetCamera();
  m_leftrenderer->GetRenderWindow()->Render();
}

void MainWindow::AddImageInRightRenderer(vtkSmartPointer<vtkImageData> input) {
  bool twoD = false;
  if (twoD) {
    if (m_right_actor) m_leftrenderer->RemoveActor2D(m_right_actor);
    vtkNew(vtkImageMapper, imageMapper);
    imageMapper->SetInputData(input);
    imageMapper->SetColorWindow(255);
    imageMapper->SetColorLevel(127.5);

    vtkNew(vtkActor2D, imageActor);
    imageActor->SetMapper(imageMapper);
    imageActor->SetPosition(20, 20);
    m_rightrenderer->AddActor2D(imageActor);
    m_right_actor = imageActor.Get();
  } else {
    if (m_rightImageActor) m_rightrenderer->RemoveActor(m_rightImageActor);
    vtkNew<vtkImageActor> imageActor;
    imageActor->GetMapper()->SetInputData(input);
    m_rightrenderer->AddActor(imageActor);
    m_rightImageActor = imageActor.Get();
  }

  m_rightrenderer->ResetCamera();
  m_rightrenderer->GetRenderWindow()->Render();
}

void MainWindow::test2() { ReadPng(); }

void MainWindow::ReadPng() {
  vtkSmartPointer<vtkPNGReader> reader = vtkSmartPointer<vtkPNGReader>::New();
  reader->SetFileName("D:/1.png");
  reader->Update();

  m_imageData = reader->GetOutput();
  ResizeUsingInterpolation();
}

void MainWindow::PrintImageDetails(vtkSmartPointer<vtkImageData> input) {
  int dim[3];
  input->GetDimensions(dim);

  std::cout << "\n\nImage size:";
  for (int i = 0; i < 3; i++) {
    cout << dim[i] << "  ";
  }

  int* e = input->GetExtent();
  std::cout << "\nImage Extent:";
  for (int i = 0; i < 6; i++) {
    cout << e[i] << "  ";
  }

  double* s = input->GetSpacing();
  std::cout << "\nImage Spacing:";
  for (int i = 0; i < 3; i++) {
    cout << s[i] << "  ";
  }

  double* B = input->GetBounds();
  std::cout << "\nImage Bounds:";
  for (int i = 0; i < 6; i++) {
    cout << B[i] << "  ";
  }
}