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

  // createMultipleViewports();
}

void MainWindow::initialiseWithDICOM() {
  m_dicom_dir_path = "D:/git/QtProjects/12vtkQTDemo3/Source/models/series201";
  QTimer::singleShot(100, this, SLOT(ProcessInput()));
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
  QTimer::singleShot(100, this, SLOT(ProcessInput()));
}

#include "vtkImageActor.h"
// -----------------------
// Test functions
// -----------------------
void MainWindow::test1() {
  vtkSmartPointer<vtkImageData> imageData = m_dicom_reader->GetOutput();
  vtkNew(vtkImageData, output);
  fetchYZImage(imageData, output);

  vtkNew(vtkImageMapper, imageMapper);
  imageMapper->SetInputData(output);
  imageMapper->SetColorWindow(255);
  imageMapper->SetColorLevel(127.5);

  vtkNew<vtkImageActor> imageActor;
  imageActor->SetInputData(output);
  displayImageActorDetails(imageActor);

  vtkNew<vtkNamedColors> colors;
  m_renderer = vtkSmartPointer<vtkRenderer>::New();
  calculateViewportDetails(imageActor);
  m_vtkView->GetRenderWindow()->AddRenderer(m_renderer);
  m_renderer->AddActor2D(imageActor);

  displyRendererDetails(m_renderer);

  ViewportBorder();
  drawAxialLine();

  m_renderer->ResetCamera();
  m_vtkView->GetRenderWindow()->Render();
}

void MainWindow::test2() {
  vtkSmartPointer<vtkImageData> imageData = m_dicom_reader->GetOutput();
  vtkNew(vtkImageData, output);
  fetchYZImage(imageData, output);

  vtkNew(vtkImageMapper, imageMapper);
  imageMapper->SetInputData(output);
  imageMapper->SetColorWindow(255);
  imageMapper->SetColorLevel(127.5);
  imageMapper->SetRenderToRectangle(true);

  vtkNew<vtkActor2D> imageActor;
  imageActor->SetMapper(imageMapper);
  imageActor->SetPosition(0, 0);
  vtkCoordinate * p1 = imageActor->GetPositionCoordinate();
  p1->SetValue(0, 0);
  vtkCoordinate * p2 = imageActor->GetPosition2Coordinate();
  p2->SetValue(0.3, 0.2);

  vtkNew<vtkNamedColors> colors;
  m_vtkImageViewer->GetRenderer()->AddActor2D(imageActor);

  m_vtkImageViewer->GetRenderer()->ResetCamera();
  m_vtkView->GetRenderWindow()->Render();
}

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
  m_minSliceNumber = imageViewer->GetSliceMin();
  m_maxSliceNumber = imageViewer->GetSliceMax();
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

// ------------------------------------------
// Read DICOM image
// ------------------------------------------
void MainWindow::ReadInputDICOM() {
  m_dicom_reader = vtkSmartPointer<vtkDICOMImageReader>::New();
  m_dicom_reader->SetDirectoryName(m_dicom_dir_path.toLatin1());
  m_dicom_reader->Update();

  m_dicom_image = m_dicom_reader->GetOutput();
}

void MainWindow::ViewportBorder() {
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

  vtkNew(vtkNamedColors, colors);
  vtkNew(vtkActor2D, actor);
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());
  actor->GetProperty()->SetLineWidth(4.0);  // Line Width

  m_renderer->AddViewProp(actor);
}

void MainWindow::drawAxialLine() {
  // points start at upper right and proceed anti-clockwise
  vtkNew(vtkPoints, points);
  points->SetNumberOfPoints(2);
  points->InsertPoint(0, 0, 0.1, 0);
  points->InsertPoint(1, 1, 0.1, 0);
  m_axialPoints = points.Get();

  // create cells, and lines
  vtkNew(vtkCellArray, cells);
  cells->Initialize();
  vtkNew(vtkPolyLine, lines);
  lines->GetPointIds()->SetNumberOfIds(2);
  for (unsigned int i = 0; i < 2; ++i) {
    lines->GetPointIds()->SetId(i, i);
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

  vtkNew(vtkNamedColors, colors);
  vtkNew(vtkActor2D, actor);
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("red").GetData());
  actor->GetProperty()->SetLineWidth(1.0);  // Line Width
  m_axialLineActor = actor.Get();
  m_renderer->AddViewProp(actor);
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
    updateAxialLine();
  }
}

void MainWindow::updateSlider(int value) { m_slider->setValue(value); }

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

void MainWindow::updateAxialLine() {
  int current = m_vtkImageViewer->GetSlice();
  int min = m_vtkImageViewer->GetSliceMin();
  int max = m_vtkImageViewer->GetSliceMax();
  float val = (float)current / max;
  if (m_axialPoints) {
    double p1[3] = {0, val, 0};
    double p2[3] = {1, val, 0};
    m_axialPoints->SetPoint(0, p1);
    m_axialPoints->SetPoint(1, p2);
    if (m_axialLineActor) m_axialLineActor->Modified();
  }
}

void MainWindow::displayImageActorDetails(vtkImageActor* imageActor) {
  cout << "\n\n bounds:";
  double b[6];
  imageActor->GetDisplayBounds(b);
  for (int i = 0; i < 6; i++) cout << b[i] << "  ";

  if (false) {
    cout << "\n Position:";
    double p[3];
    imageActor->GetPosition(p);
    for (int i = 0; i < 3; i++) cout << p[i] << "  ";
  }
}

void MainWindow::displyRendererDetails(vtkRenderer* renderer) {
  int* pOrigin = renderer->GetOrigin();
  int* pSize = renderer->GetSize();
  cout << "\n Origin and size:" << pOrigin[0] << "  " << pOrigin[1]
       << "  Size:" << pSize[0] << "  " << pSize[1];
}

void MainWindow::calculateViewportDetails(vtkImageActor* imageActor) {
  double b[6];
  imageActor->GetDisplayBounds(b);
  double xsize = b[1];
  double ysize = b[3];
  cout << "\nImage size:" << xsize << " " << ysize;

  int* p = m_vtkImageViewer->GetRenderer()->GetSize();
  double rxsize = p[0];
  double rysize = p[1];
  cout << "\nViewer size:" << rxsize << " " << rysize;

  double vxsize = (xsize / rxsize);
  double vysize = (ysize / rysize);
  double vminx = 0.05;
  double vminy = 0.95 - vysize;
  m_renderer->SetViewport(vminx, vminy, vminx + vxsize, vminy + vysize);
  int* p1 = m_renderer->GetSize();
  double rxsize1 = p1[0];
  double rysize1 = p1[1];
  cout << "\nViewport size:" << rxsize1 << " " << rysize1;
  m_renderer->GetActiveCamera()->SetParallelScale(ysize);
}