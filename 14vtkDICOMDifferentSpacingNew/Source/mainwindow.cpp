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
  this->setWindowTitle(
      "Demo of reading Dicom images with different pixel spacing");

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

void MainWindow::initialiseWithDICOM() {}

// ------------------------
// destructor and exit
// ------------------------
MainWindow::~MainWindow() { delete ui; }
void MainWindow::on_actionExit_triggered() { QApplication::quit(); }
bool is_not_digit(char c) { return !std::isdigit(c); }

// --------------------------------
// String comparison function
// --------------------------------
bool numeric_string_compare(const std::string& s1, const std::string& s2) {
  std::string::const_iterator it1 = s1.begin(), it2 = s2.begin();

  if (std::isdigit(s1[0]) && std::isdigit(s2[0])) {
    int n1, n2;
    std::stringstream ss(s1);
    ss >> n1;
    ss.clear();
    ss.str(s2);
    ss >> n2;

    if (n1 != n2) return n1 < n2;

    it1 = std::find_if(s1.begin(), s1.end(), is_not_digit);
    it2 = std::find_if(s2.begin(), s2.end(), is_not_digit);
  }

  return std::lexicographical_compare(it1, s1.end(), it2, s2.end());
}

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

  QStringList stringlist;
  stringlist << "*.dcm";
  QStringList images = dirname.entryList(stringlist, QDir::Files);
  foreach (QString filename, images) {
    m_imageFiles.push_back(filename.toStdString());
  }
  sort(m_imageFiles.begin(), m_imageFiles.end(), numeric_string_compare);

  m_slider->setMinimum(0);
  m_slider->setMaximum(m_imageFiles.size()-1);
  m_slider->setValue(0);

  QTimer::singleShot(100, this, &MainWindow::readSingleImage);
}

// -------------------------
// Create viewport border
// -------------------------
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

// ---------------------------------------------------------------------
// Draw axial line indicating slide position in side view of the image
// ---------------------------------------------------------------------
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

// ----------------------------------
// Handle slider position change
// ----------------------------------
void MainWindow::sliderChanged(int value) {
  if (m_interaction) {
    m_interaction->updateSliceMsg(value);
  }
  readImage(value);
}

void MainWindow::updateSlider(int value) { m_slider->setValue(value); }

// ---------------------------------------
// Display details about Image actor
// ---------------------------------------
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

// ---------------------------------------
// Display details about renderer
// ---------------------------------------
void MainWindow::displyRendererDetails(vtkRenderer* renderer) {
  int* pOrigin = renderer->GetOrigin();
  int* pSize = renderer->GetSize();
  cout << "\n Origin and size:" << pOrigin[0] << "  " << pOrigin[1]
       << "  Size:" << pSize[0] << "  " << pSize[1];
}

// ----------------------------------------------
// Calculate how big viewport should be for image
// ----------------------------------------------
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

void MainWindow::readSingleImage() {
  std::string filepath = m_dicom_dir_path.toStdString() + "/" + m_imageFiles[0];
  vtkNew(vtkDICOMImageReader, reader);
  reader->SetFileName(filepath.c_str());
  reader->Update();

  vtkNew<vtkImageActor> imageActor;
  m_imageactor = imageActor.Get();
  imageActor->SetInputData(reader->GetOutput());
  displayImageActorDetails(imageActor);

  vtkNew<vtkNamedColors> colors;
  m_renderer = vtkSmartPointer<vtkRenderer>::New();
  m_vtkView->GetRenderWindow()->AddRenderer(m_renderer);
  m_renderer->AddActor2D(imageActor);
  InitialiseCornerText();

  m_renderer->ResetCamera();
  m_vtkView->GetRenderWindow()->Render();
}

void MainWindow::readImage(int index) {
  if (!(index >= 0 && index < m_imageFiles.size())) return;
  std::string filepath =
      m_dicom_dir_path.toStdString() + "/" + m_imageFiles[index];
  vtkNew(vtkDICOMImageReader, reader);
  reader->SetFileName(filepath.c_str());
  reader->Update();

  m_imageactor->SetInputData(reader->GetOutput());
  m_imageactor->Modified();

  m_renderer->ResetCamera();
  m_vtkView->GetRenderWindow()->Render();
}

void MainWindow::test1() {}

void MainWindow::test2() {}

void MainWindow::InitialiseCornerText() {
  vtkNew(vtkTextProperty, sliceTextProp);
  sliceTextProp->SetFontFamilyToCourier();
  sliceTextProp->SetFontSize(20);
  sliceTextProp->SetVerticalJustificationToBottom();
  sliceTextProp->SetJustificationToLeft();

  vtkNew(vtkTextMapper, sliceTextMapper);
  std::string msg = StatusMessage::Format(0, m_imageFiles.size()-1);
  sliceTextMapper->SetInput(msg.c_str());
  sliceTextMapper->SetTextProperty(sliceTextProp);
  vtkNew(vtkActor2D, sliceTextActor);
  sliceTextActor->SetMapper(sliceTextMapper);
  sliceTextActor->SetPosition(15, 10);

  vtkNew(myVtkInteractorStyleImage, style);
  m_vtkView->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
  m_interaction = style.Get();
  style->setMax(m_imageFiles.size()-1);
  style->SetStatusMapper(sliceTextMapper);
  style->setWindow(this);

  m_renderer->AddActor2D(sliceTextActor);
}