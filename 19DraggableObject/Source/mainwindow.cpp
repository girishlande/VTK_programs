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
}


void MainWindow::InitialiseView() {
  vtkNew(vtkSphereSource, sphere);
  sphere->SetCenter(20, 20, 0);
  sphere->SetRadius(10);
  sphere->Update();
  vtkNew(vtkPolyDataMapper, mapper);
  mapper->SetInputConnection(sphere->GetOutputPort());
  vtkNew(vtkActor, actor);
  actor->SetMapper(mapper);
  vtkNew(vtkRenderer, renderer);
  renderer->AddActor(actor);
  this->m_vtkView->GetRenderWindow()->AddRenderer(renderer);
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

  vtkNew(vtkDICOMImageReader, reader);
  reader->SetDirectoryName(m_dicom_dir_path.toLatin1());
  reader->Update();

  vtkNew(vtkImageViewer2, imageViewer);
  imageViewer->SetInputConnection(reader->GetOutputPort());
  imageViewer->SetSliceOrientationToXY();
  m_vtkImageViewer = imageViewer.Get();
  m_slider->setMinimum(imageViewer->GetSliceMin());
  m_slider->setMaximum(imageViewer->GetSliceMax());
  m_slider->setValue(imageViewer->GetSlice());

  vtkNew(vtkTextProperty, sliceTextProp);
  sliceTextProp->SetFontFamilyToCourier();
  sliceTextProp->SetFontSize(20);
  sliceTextProp->SetVerticalJustificationToBottom();
  sliceTextProp->SetJustificationToLeft();

  vtkNew(vtkTextMapper, sliceTextMapper);
  std::string msg = StatusMessage::Format(imageViewer->GetSliceMin(),
                                          imageViewer->GetSliceMax());
  sliceTextMapper->SetInput(msg.c_str());
  sliceTextMapper->SetTextProperty(sliceTextProp);
  vtkNew(vtkActor2D, sliceTextActor);
  sliceTextActor->SetMapper(sliceTextMapper);
  sliceTextActor->SetPosition(15, 10);

  // create an interactor with our own style (inherit from
  // vtkInteractorStyleImage) in order to catch mousewheel and key events
  vtkNew(vtkRenderWindowInteractor, renderWindowInteractor);
  vtkNew(myVtkInteractorStyleImage, myInteractorStyle);
  myInteractorStyle->setWindow(this);
  myInteractorStyle->SetImageViewer(imageViewer);
  myInteractorStyle->SetStatusMapper(sliceTextMapper);
  renderWindowInteractor->SetInteractorStyle(myInteractorStyle);

  // add slice status message and usage hint message to the renderer
  imageViewer->GetRenderer()->AddActor2D(sliceTextActor);
  imageViewer->SetRenderWindow(m_vtkView->GetRenderWindow());
  m_vtkView->GetRenderWindow()->SetInteractor(renderWindowInteractor);

  // initialize rendering and interaction
  imageViewer->Render();
  imageViewer->GetRenderer()->ResetCamera();
  renderWindowInteractor->Start();
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

  vtkNew(vtkDICOMImageReader, reader);
  reader->SetDirectoryName(m_dicom_dir_path.toLatin1());
  reader->Update();

  vtkSmartPointer<vtkImageData> imageData = reader->GetOutput();
  vtkNew(vtkImageData, output);
  fetchYZImage(imageData, output);
  printImageDetails(imageData);
  printImageDetails(output);

  vtkNew(vtkImageMapper, imageMapper);
  imageMapper->SetInputData(output);
  imageMapper->SetColorWindow(255);
  imageMapper->SetColorLevel(127.5);

  vtkNew(vtkActor2D, imageActor);
  imageActor->SetMapper(imageMapper);

  vtkNew(vtkNamedColors, colors);
  vtkNew(vtkRenderer, renderer);
  renderer->SetViewport(0.1, 0.7, 0.3, 0.9);
  renderer->SetBackground(0, 0, 1);
  ViewportBorder(renderer, colors->GetColor3d("Gold").GetData(), true);

  m_vtkView->GetRenderWindow()->AddRenderer(renderer);
  renderer->AddActor2D(imageActor);

  m_vtkView->GetRenderWindow()->Render();
}


void MainWindow::addDicomImageInViewport() {
  vtkNew(vtkDICOMImageReader, reader);
  reader->SetDirectoryName(m_dicom_dir_path.toLatin1());
  reader->Update();
  vtkSmartPointer<vtkImageData> imageData = reader->GetOutput();

  vtkNew(vtkImageMapper, imageMapper);
  imageMapper->SetInputData(imageData);
  imageMapper->SetColorWindow(255);
  imageMapper->SetColorLevel(127.5);

  vtkNew(vtkActor2D, imageActor);
  imageActor->SetMapper(imageMapper);
  imageActor->SetPosition(20, 20);

  // Setup renderers
  vtkNew(vtkRenderer, renderer);
  renderer->SetViewport(0, 0.7, 0.3, 1);

  // Setup render window
  vtkSmartPointer<vtkRenderWindow> renderWindow = m_vtkView->GetRenderWindow();
  renderWindow->AddRenderer(renderer);

  renderer->AddActor2D(imageActor);
  vtkNew(vtkNamedColors, colors);
  renderer->SetBackground(colors->GetColor3d("Silver").GetData());

  renderWindow->Render();
  renderer->ResetCamera();
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

void MainWindow::fetchXYImage(vtkSmartPointer<vtkImageData>& input,
                              vtkSmartPointer<vtkImageData>& output) {
  short* data = static_cast<short*>(input->GetScalarPointer(0, 0, 0));
  int* dims = input->GetDimensions();
  int xdim = dims[0];
  int ydim = dims[1];
  int zdim = dims[2];

  output->SetDimensions(xdim, ydim, 1);
  output->AllocateScalars(VTK_SHORT, 1);
  output->SetSpacing(input->GetSpacing());
  output->SetOrigin(input->GetOrigin());

  for (int y = 0; y < ydim; y++) {
    for (int x = 0; x < xdim; x++) {
      short* pixel = static_cast<short*>(output->GetScalarPointer(x, y, 0));
      short* sourcepixel =
          static_cast<short*>(input->GetScalarPointer(x, y, 0));
      pixel[0] = sourcepixel[0];
    }
  }
}

void MainWindow::fetchYZImage(vtkSmartPointer<vtkImageData>& input,
                              vtkSmartPointer<vtkImageData>& output) {
  short* data = static_cast<short*>(input->GetScalarPointer(0, 0, 0));
  int* dims = input->GetDimensions();
  int xdim = dims[0];
  int ydim = dims[1];
  int zdim = dims[2];

  // Specify the size of the image data
  output->SetDimensions(ydim, zdim, 1);
  output->AllocateScalars(VTK_SHORT, 1);
  output->SetSpacing(input->GetSpacing());
  output->SetOrigin(input->GetOrigin());

  // Fill every entry of the image data with "2.0"
  for (int z = 0; z < zdim; z++) {
    for (int y = 0; y < ydim; y++) {
      short* pixel = static_cast<short*>(output->GetScalarPointer(y, z, 0));
      short* sourcepixel =
          static_cast<short*>(input->GetScalarPointer(xdim / 2, y, z));
      pixel[0] = sourcepixel[0];
    }
  }
}

void MainWindow::fetchXZImage(vtkSmartPointer<vtkImageData>& input,
                              vtkSmartPointer<vtkImageData>& output) {
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

void MainWindow::AddLineActor(vtkRenderer* renderer) {
  double p0[3] = {0.0, 0.0, 0.0};
  double p1[3] = {20.0, 0.0, 0.0};

  vtkNew(vtkLineSource, lineSource);
  lineSource->SetPoint1(p0);
  lineSource->SetPoint2(p1);

  // Visualize
  vtkNew(vtkPolyDataMapper, mapper);
  mapper->SetInputConnection(lineSource->GetOutputPort());
  vtkNew(vtkActor, actor);
  actor->SetMapper(mapper);
  actor->GetProperty()->SetLineWidth(2);
  vtkNew(vtkNamedColors, colors);
  actor->GetProperty()->SetColor(colors->GetColor3d("Peacock").GetData());

  renderer->AddActor(actor);
}

void MainWindow::AddSphereActor(vtkRenderer* renderer) {
  vtkNew(vtkSphereSource, sphere);
  sphere->SetCenter(0, 0, 0);
  sphere->SetRadius(2);
  sphere->Update();

  vtkNew(vtkPolyDataMapper, mapper);
  mapper->SetInputConnection(sphere->GetOutputPort());
  vtkNew(vtkActor, actor);
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(1, 0, 0);
  renderer->AddActor(actor);
}

void MainWindow::MultipleViewports() {
  vtkNew(vtkRenderer, ren1);
  ren1->SetBackground(1.0, 1.0, 0);
  ren1->SetViewport(0, 0, 0.498, 1.0);

  vtkNew(vtkRenderer, ren2);
  ren2->SetBackground(0, 1.0, 1.0);
  ren2->SetViewport(0.502, 0, 1.0, 1.0);

  vtkSmartPointer<vtkRenderWindow> renderWindow = m_vtkView->GetRenderWindow();
  renderWindow->AddRenderer(ren1);
  renderWindow->AddRenderer(ren2);
}

#include "vtkActor2D.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPolyLine.h"
#include "vtkProperty2D.h"

void MainWindow::createMultipleViewports() {
  int numberOfFiles = 4;
  vtkNew(vtkNamedColors, colors);
  vtkSmartPointer<vtkRenderWindow> renderWindow = m_vtkView->GetRenderWindow();

  double size = 1.0 / numberOfFiles;
  for (unsigned int i = 0; static_cast<int>(i) < numberOfFiles; ++i) {
    vtkNew(vtkSphereSource, sphere);
    sphere->SetCenter(0, 0, 0);
    sphere->SetRadius(2);
    sphere->Update();

    vtkNew(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(sphere->GetOutputPort());

    vtkNew(vtkActor, actor);
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("Silver").GetData());

    vtkNew(vtkRenderer, renderer);
    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    double viewport[4];
    viewport[0] = size * i;
    viewport[1] = 0.0;
    viewport[2] = size * (i + 1);
    viewport[3] = 1.0;
    renderer->SetViewport(viewport);
    ViewportBorder(renderer, colors->GetColor3d("Gold").GetData(),
                   static_cast<int>(i) == numberOfFiles - 1);
    renderWindow->AddRenderer(renderer);
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

void MainWindow::printImageDetails(vtkSmartPointer<vtkImageData>& image) {
  int* dims = image->GetDimensions();
  int xdim = dims[0];
  int ydim = dims[1];
  int zdim = dims[2];
  cout << "\nX:" << xdim << " Ydim:" << ydim << " Zdim:" << zdim;
  double* origin = image->GetOrigin();
  cout << "\nOrigin:" << origin[0] << "," << origin[0] << "," << origin[0];
}

#include <vtkCellPicker.h>
#include <vtkImagePlaneWidget.h>
void MainWindow::createPlaneWidget() {

  vtkNew(vtkSphereSource, sphere);
  sphere->SetCenter(20, 20, 0);
  sphere->SetRadius(10);
  sphere->Update();
  vtkNew(vtkPolyDataMapper, mapper);
  mapper->SetInputConnection(sphere->GetOutputPort());
  vtkNew(vtkActor, actor);
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(1, 0, 0);
 
  vtkNew(vtkRenderWindowInteractor, renderWindowInteractor);
  vtkNew(vtkInteractorStyleTrackballCamera, myInteractorStyle);
  renderWindowInteractor->SetInteractorStyle(myInteractorStyle);

  vtkNew(vtkRenderer, renderer);
  renderer->AddActor(actor);

  vtkNew(vtkDICOMImageReader, reader);
  reader->SetDirectoryName(m_dicom_dir_path.toLatin1());
  reader->Update();

  vtkSmartPointer<vtkImageData> image = reader->GetOutput();
  int imageDims[3];
  image->GetDimensions(imageDims);

  vtkNew(vtkCellPicker, picker);
  picker->SetTolerance(0.005);
  vtkNew(vtkProperty, ipwProp);

  //vtkNew(vtkImagePlaneWidget, plane);
  m_plane = vtkSmartPointer<vtkImagePlaneWidget>::New();
  m_plane->SetInteractor(renderWindowInteractor);
  m_plane->SetPicker(picker);
  m_plane->RestrictPlaneToVolumeOn();
  double color[3] = {1, 0, 0};
  m_plane->GetPlaneProperty()->SetColor(color);
  double ambColor[3] = {0,1,0};
  m_plane->GetPlaneProperty()->SetAmbientColor(ambColor);
  m_plane->SetOrigin(image->GetOrigin());
  m_plane->SetTexturePlaneProperty(ipwProp);
  m_plane->TextureInterpolateOff();
  m_plane->SetResliceInterpolateToLinear();
  m_plane->SetInputConnection(reader->GetOutputPort());
  m_plane->SetPlaneOrientation(2);
  m_plane->SetSliceIndex(imageDims[2] / 2);
  m_plane->DisplayTextOn();
  m_plane->SetDefaultRenderer(renderer);
  m_plane->SetWindowLevel(1708, -709);
  m_plane->On();
  m_plane->InteractionOn();

  m_vtkView->GetRenderWindow()->AddRenderer(renderer);
  m_vtkView->GetRenderWindow()->Render();
}


void MainWindow::calculateKeyPoints() {
  double XX[3] = {m_normal_X[0], m_normal_X[1], m_normal_X[2]};
  double YY[3] = {m_normal_Y[0], m_normal_Y[1], m_normal_Y[2]};
  double ZZ[3] = {m_normal_Z[0], m_normal_Z[1], m_normal_Z[2]};

  vtkMath::Normalize(XX);
  vtkMath::MultiplyScalar(XX, 512);
  vtkMath::Normalize(YY);
  vtkMath::MultiplyScalar(YY, 512);
  vtkMath::Normalize(ZZ);
  vtkMath::MultiplyScalar(ZZ, 512);

  vtkMath::Add(m_plane_center, XX, m_XX_1);
  vtkMath::Add(m_plane_center, YY, m_YY_1);
  vtkMath::Add(m_plane_center, ZZ, m_ZZ_1);
  vtkMath::Subtract(m_plane_center, XX, m_XX_2);
  vtkMath::Subtract(m_plane_center, YY, m_YY_2);
  vtkMath::Subtract(m_plane_center, ZZ, m_ZZ_2);

  vtkMath::Subtract(m_XX_2, YY, m_XY_origin);
  vtkMath::Subtract(m_YY_2, ZZ, m_YZ_origin);
  vtkMath::Subtract(m_XX_2, ZZ, m_XZ_origin);
}


void MainWindow::test2() {
  vtkPlaneSource* ps =
      static_cast<vtkPlaneSource*>(m_plane->GetPolyDataAlgorithm());
  ps->SetCenter(0,0,0);
  ps->SetPoint1(200,200,0);
  ps->SetPoint2(200,0,0);
  m_plane->On();
}