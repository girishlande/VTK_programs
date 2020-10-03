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
#include <vtkBorderRepresentation.h>
#include <vtkBorderWidget.h>
#include <vtkCommand.h>
#include <vtkConeSource.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageActor.h>
#include <vtkImageResize.h>
#include <vtkImageViewer2.h>
#include <vtkInteractorStyleImage.h>
#include <vtkMath.h>
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
#include <vtkWidgetCallbackMapper.h>
#include <vtkWidgetEvent.h>

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

  InitialiseDICOM();
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

void MainWindow::InitialiseDICOM() {
  m_dicom_dir_path = "../../data/SingleImage";
  ReadTopoImage();
}

class vtkCustomBorderWidget : public vtkBorderWidget {
 public:
  static vtkCustomBorderWidget* New();
  vtkTypeMacro(vtkCustomBorderWidget, vtkBorderWidget);

  static void EndSelectAction(vtkAbstractWidget* w);

  vtkCustomBorderWidget();
};

vtkStandardNewMacro(vtkCustomBorderWidget);

vtkCustomBorderWidget::vtkCustomBorderWidget() {
  this->CallbackMapper->SetCallbackMethod(
      vtkCommand::MiddleButtonReleaseEvent, vtkWidgetEvent::EndSelect, this,
      vtkCustomBorderWidget::EndSelectAction);
}

void vtkCustomBorderWidget::EndSelectAction(vtkAbstractWidget* w) {
  vtkBorderWidget* borderWidget = dynamic_cast<vtkBorderWidget*>(w);

  // Get the actual box coordinates/planes
  // vtkSmartPointer<vtkPolyData> polydata =
  //  vtkSmartPointer<vtkPolyData>::New();

  // Get the bottom left corner
  auto lowerLeft =
      static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())
          ->GetPosition();
  std::cout << "Lower left: " << lowerLeft[0] << " " << lowerLeft[1]
            << std::endl;

  auto upperRight =
      static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())
          ->GetPosition2();
  std::cout << "Upper right: " << upperRight[0] << " " << upperRight[1]
            << std::endl;

  vtkBorderWidget::EndSelectAction(w);
}

void MainWindow::ReadTopoImage() {
  std::string filepath = m_dicom_dir_path.toStdString() + "/" + "1.dcm";
  vtkNew(vtkDICOMImageReader, reader);
  reader->SetFileName(filepath.c_str());
  reader->Update();

  m_topoImage = reader->GetOutput();
}

void MainWindow::UpdateViewForDICOM() {
  vtkNew(vtkDICOMImageReader, reader);
  reader->SetDirectoryName(m_dicom_dir_path.toLatin1());
  reader->Update();
  m_imageVolume = reader->GetOutput();

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
    if (m_topoviewer) m_topoviewer->UpdateTopoView(value);

    if (style) {
      style->updateSliceMsg(value);
    }
  }
}

void MainWindow::updateSlider(int value) { m_slider->setValue(value); }

void MainWindow::test1() {
  vtkRenderer* ren = m_renderer;
  if (m_vtkImageViewer) ren = m_vtkImageViewer->GetRenderer();

  if (!m_topoviewer) {
    m_topoviewer = new avTopoViewerEx(m_topoImage, ren);
    int* d = m_imageVolume->GetDimensions();
    m_topoviewer->SetMaxSlice(d[2]);
    m_topoviewer->Start();
    }
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

void MainWindow::ViewportBorder(vtkSmartPointer<vtkRenderer> renderer,
                                double* color) {
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
  actor->GetProperty()->SetLineWidth(4.0);  // Line Width
  m_topoBorderProperty = actor->GetProperty();

  renderer->AddViewProp(actor);
}

void MainWindow::printImageDetails(vtkImageData* image) {
  int* dims = image->GetDimensions();
  int xdim = dims[0];
  int ydim = dims[1];
  int zdim = dims[2];
  cout << "\nX:" << xdim << " Ydim:" << ydim << " Zdim:" << zdim;
  double* origin = image->GetOrigin();
  cout << "\nOrigin:" << origin[0] << "," << origin[0] << "," << origin[0];

  double* spacing = image->GetSpacing();
  cout << "\n SPacing:";
  for (int i = 0; i < 3; i++) {
    cout << spacing[i] << " ";
  }

  cout << "\n Image Length:";
  for (int i = 0; i < 3; i++) {
    cout << spacing[i] * dims[i] << "  ";
  }
}

void MainWindow::test2() {}

void MainWindow::NormalisedToDeviceCoordinates(double x, double y, int& dx,
                                               int& dy) {
  dx = vtkMath::Round(x * m_width);
  dy = vtkMath::Round(y * m_height);
}

void MainWindow::DeviceToNormalised(int dx, int dy, double& nx, double& ny) {
  nx = dx / m_width;
  ny = dy / m_height;
}

void MainWindow::ProcessMousePoint(int mx, int my) {
  if (m_topoviewer) m_topoviewer->ProcessMousePoint(mx, my);
}

void MainWindow::LeftButtonDown(int mx, int my) {
  if (m_topoviewer) m_topoviewer->LeftButtonDown(mx, my);
}

void MainWindow::LeftButtonUp(int mx, int my) {
  if (m_topoviewer) m_topoviewer->LeftButtonUp(mx, my);
}

vtkRenderWindowInteractor* MainWindow::getInteractor() { return m_interactor; }