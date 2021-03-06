#include "mainwindow.h"

#include <QSettings.h>
#include <QBoxLayout.h>
#include <QComboBox.h>
#include <QDir.h>
#include <QFileDialog.h>
#include <QGridLayout.h>
#include <QSplitter.h>
#include <QVTKWidget.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdebug.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCamera.h>
#include <vtkCellPicker.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapper.h>
#include <vtkImageViewer2.h>
#include <vtkInteractorStyleImage.h>
#include <vtkLineSource.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkPolyLine.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
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

#include "avTopoObserver.h"
#include "avTopoViewer.h"
#include "avTopoViewerEx.h"
#include "dicominteractionstyle.h"
#include "modelinteractionstyle.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_vtkImageViewer(nullptr) {
    // Setup window and resize it
    ui->setupUi(this);
    this->showMaximized();
    this->setWindowTitle("Demo of Topo view in 2X2 layout");

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
    QPushButton* b1 = new QPushButton("Load Default Series", this);
    connect(b1, SIGNAL(clicked(bool)), this, SLOT(test1()));
    QPushButton* b2 = new QPushButton("Toggle Visibility", this);
    connect(b2, SIGNAL(clicked(bool)), this, SLOT(test2()));
    QPushButton* b3 = new QPushButton("Toggle Synchronization", this);
    connect(b3, SIGNAL(clicked(bool)), this, SLOT(test3()));
    QPushButton* b4 = new QPushButton("Reset Position", this);
    connect(b4, SIGNAL(clicked(bool)), this, SLOT(test4()));

    SetupLayoutsCombobox();
    tablayout->addWidget(b1);
    tablayout->addWidget(b2);
    tablayout->addWidget(b3);
    tablayout->addWidget(b4);
    tablayout->addWidget(m_layoutsCombo);

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

    ReadTopoImage();
    ReadConfigurationFile();
}

void MainWindow::initialiseWithDICOM() {
    ReadImageVolume();
    createMultipleViewports(1, 1);
}

MainWindow::~MainWindow() { delete ui; }
void MainWindow::on_actionExit_triggered() { QApplication::quit(); }
bool is_not_digit(char c) { return !std::isdigit(c); }

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

void MainWindow::on_actionOpen_DICOM_file_triggered() {
    QString fileName =
        QFileDialog::getOpenFileName(this, "Select DICOM file", "../models",
            "DICOM Files (*.dcm);;All Files (*.*)");

    if (fileName.isNull()) {
        cerr << "Could not open file" << endl;
        return;
    }
    m_file_name = fileName;

    ProcessInputData();
}

void MainWindow::sliderChanged(int value) {
    m_currentSliceNumber = value;
    UpdateImagesInViewports();
}

void MainWindow::updateSlider(int value) {  // m_slider->setValue(value);
}

void MainWindow::displyRendererDetails(vtkRenderer* renderer) {
    int* pOrigin = renderer->GetOrigin();
    int* pSize = renderer->GetSize();
    cout << "\n Origin and size:" << pOrigin[0] << "  " << pOrigin[1]
        << "  Size:" << pSize[0] << "  " << pSize[1];
}

void MainWindow::test1() {
    m_file_name =
        "../../data/series201/"
        "1.3.6.1.4.1.5962.99.1.2786334768.1849416866.1385765836848.617.0.dcm";

    if (!QFile(m_file_name).exists()) {
        m_file_name =
            "../../../data/series201/"
            "1.3.6.1.4.1.5962.99.1.2786334768.1849416866.1385765836848.617.0.dcm";
    }
    ProcessInputData();
}

void MainWindow::test2() {
    ToggleVisibility();
}

void MainWindow::test3() {
    ToggleSynchronization();
}

void MainWindow::test4() {
    ResetPosition();
}

void MainWindow::ReadTopoImage() {
    QString dicom_dir_path = "../../data/SingleImage";
    std::string filepath =
        dicom_dir_path.toStdString() + "/" + "LocalizerPatient.dcm";
    vtkNew(vtkDICOMImageReader, reader);
    reader->SetFileName(filepath.c_str());
    reader->Update();

    m_topoImage = reader->GetOutput();
}

void MainWindow::SetupLayoutsCombobox() {
    m_layoutconfig.push_back({ "1x1", 1, 1 });
    m_layoutconfig.push_back({ "1x2", 1, 2 });
    m_layoutconfig.push_back({ "1x3", 1, 3 });
    m_layoutconfig.push_back({ "1x4", 1, 4 });
    m_layoutconfig.push_back({ "2x1", 2, 1 });
    m_layoutconfig.push_back({ "2x2", 2, 2 });
    m_layoutconfig.push_back({ "2x3", 2, 3 });
    m_layoutconfig.push_back({ "2x4", 2, 4 });
    m_layoutconfig.push_back({ "3x1", 3, 1 });
    m_layoutconfig.push_back({ "3x2", 3, 2 });
    m_layoutconfig.push_back({ "3x3", 3, 3 });
    m_layoutconfig.push_back({ "3x4", 3, 4 });
    m_layoutconfig.push_back({ "4x3", 4, 3 });
    m_layoutconfig.push_back({ "4x4", 4, 4 });
    m_layoutconfig.push_back({ "6x5", 6, 5 });
    m_layoutconfig.push_back({ "7x6", 7, 6 });
    m_layoutconfig.push_back({ "8x5", 8, 5 });
    m_layoutconfig.push_back({ "8x6", 8, 6 });

    m_layoutsCombo = new QComboBox(this);
    m_layoutsCombo->setFixedHeight(30);
    m_layoutsCombo->setFixedWidth(100);

    for (auto x : m_layoutconfig) {
        m_layoutsCombo->addItem(x.name);
    }

    connect(m_layoutsCombo, SIGNAL(currentIndexChanged(int)), this,
        SLOT(LayoutChanged(int)));
}

void MainWindow::LayoutChanged(int index) {
    LayoutConfig config = m_layoutconfig[index];
    m_row = config.row;
    m_col = config.col;
    m_currentSliceNumber = 0;
    createMultipleViewports(config.row, config.col);
    SynchronizeSlider();
    UpdateImagesInViewports();
}

#include "vtkEventQtSlotConnect.h"
void MainWindow::createMultipleViewports(int rows, int cols) {
    ClearViewports();

    vtkNew(vtkNamedColors, colors);
    vtkSmartPointer<vtkRenderWindow> renderWindow = m_vtkView->GetRenderWindow();

    double xstep = 1.0 / cols;
    double ystep = 1.0 / rows;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            vtkNew(vtkSphereSource, sphere);
            sphere->SetThetaResolution(100);
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
            m_renderers.push_back(renderer.Get());

            vtkNew<vtkImageActor> imageactor;
            renderer->AddActor2D(imageactor);
            m_imageActors.push_back(imageactor.Get());

            AddSliceNumberCornerText(0, renderer);

            double viewport[4];
            viewport[0] = j * xstep;
            viewport[1] = i * ystep;
            viewport[2] = (j + 1) * xstep;
            viewport[3] = (i + 1) * ystep;
            renderer->SetViewport(viewport);
            ViewportBorder(renderer, colors->GetColor3d("Gray").GetData(), true);
            renderWindow->AddRenderer(renderer);

            std::shared_ptr<avTopoViewerEx> topo =
                std::make_shared<avTopoViewerEx>(m_topoImage, renderer, QString::number(i));
            topo->SetMaxSlice(m_imagedata.size());
            topo->SetAspectRatio(true);
            topo->SetTopoViewSize(m_topoWidth, m_topoWidth);
            topo->SetTopoPositionNormalised(m_topoTopMargin, m_topoLeftMargin);
            m_topoEx.push_back(topo);
        }
    }

    AddImagesInViewports();

    vtkNew(myVtkInteractorStyleImage, style);
    m_vtkView->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
    m_interaction = style.Get();
    style->setMax(m_imageFiles.size() - 1);
    style->setWindow(this);

    DrawTopoEx();
    renderWindow->Render();
}

void MainWindow::SetConnections() {
    m_connections->Connect(
        m_vtkView->GetRenderWindow()->GetInteractor(),
        vtkCommand::LeftButtonPressEvent, this,
        SLOT(slot_clicked(vtkObject*, unsigned long, void*, void*)));
    m_connections->Connect(
        m_vtkView->GetRenderWindow()->GetInteractor(),
        vtkCommand::LeftButtonReleaseEvent, this,
        SLOT(slot_released(vtkObject*, unsigned long, void*, void*)));
    m_connections->Connect(
        m_vtkView->GetRenderWindow()->GetInteractor(), vtkCommand::MouseMoveEvent,
        this, SLOT(slot_moved(vtkObject*, unsigned long, void*, void*)));
}

void MainWindow::slot_clicked(vtkObject*, unsigned long, void*, void*) {
    std::cout << "Clicked." << std::endl;
}

void MainWindow::slot_released(vtkObject*, unsigned long, void*, void*) {
    std::cout << "released." << std::endl;
}

void MainWindow::slot_moved(vtkObject*, unsigned long, void*, void*) {
    std::cout << "moved." << std::endl;
}

void MainWindow::ViewportBorder(vtkSmartPointer<vtkRenderer>& renderer,
    double* color, bool last) {
    vtkNew(vtkPoints, points);
    points->SetNumberOfPoints(4);
    points->InsertPoint(0, 1, 1, 0);
    points->InsertPoint(1, 0, 1, 0);
    points->InsertPoint(2, 0, 0, 0);
    points->InsertPoint(3, 1, 0, 0);

    vtkNew(vtkCellArray, cells);
    cells->Initialize();
    vtkNew(vtkPolyLine, lines);

    if (last) {
        lines->GetPointIds()->SetNumberOfIds(5);
    }
    else {
        lines->GetPointIds()->SetNumberOfIds(4);
    }
    for (unsigned int i = 0; i < 4; ++i) {
        lines->GetPointIds()->SetId(i, i);
    }
    if (last) {
        lines->GetPointIds()->SetId(4, 0);
    }
    cells->InsertNextCell(lines);

    vtkNew(vtkPolyData, poly);
    poly->Initialize();
    poly->SetPoints(points);
    poly->SetLines(cells);

    vtkNew(vtkCoordinate, coordinate);
    coordinate->SetCoordinateSystemToNormalizedViewport();

    vtkNew(vtkPolyDataMapper2D, mapper);
    mapper->SetInputData(poly);
    mapper->SetTransformCoordinate(coordinate);

    vtkNew(vtkActor2D, actor);
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(color);
    actor->GetProperty()->SetLineWidth(1.0);  // Line Width

    renderer->AddViewProp(actor);
}

void MainWindow::AddImagesInViewports() {
    int index = m_currentSliceNumber;
    for (int i = 0; i < m_renderers.size(); i++) {
        if (index >= m_imageFiles.size()) break;

        int rendererIndex = MapViewportNumber(i);
        vtkRenderer* r = m_renderers[rendererIndex];
        vtkImageActor* imageActor = m_imageActors[rendererIndex];
        imageActor->SetInputData(m_imagedata[index]);
        imageActor->Modified();
        UpdateSliceNumberCornerText(index, rendererIndex);
        index++;
    }
}

void MainWindow::UpdateImagesInViewports() {
    int index = m_currentSliceNumber;
    for (int i = 0; i < m_renderers.size(); i++) {
        if (index > m_imageFiles.size()) break;

        int rendererIndex = MapViewportNumber(i);
        vtkImageActor* imageActor = m_imageActors[rendererIndex];
        if (imageActor) {
            imageActor->SetInputData(m_imagedata[index]);
            imageActor->Modified();
        }
        UpdateSliceNumberCornerText(index, rendererIndex);
        UpdateTopoEx(index, rendererIndex);

        index++;
    }
    m_vtkView->GetRenderWindow()->Render();
}

void MainWindow::AddSliceNumberCornerText(int sliceNumber,
    vtkRenderer* renderer) {
    vtkNew(vtkTextProperty, sliceTextProp);
    sliceTextProp->SetFontFamilyToCourier();
    sliceTextProp->SetFontSize(20);
    sliceTextProp->SetVerticalJustificationToBottom();
    sliceTextProp->SetJustificationToLeft();

    vtkNew(vtkTextMapper, sliceTextMapper);
    std::string msg = StatusMessage::Format(sliceNumber, m_imageFiles.size() - 1);
    sliceTextMapper->SetInput(msg.c_str());
    sliceTextMapper->SetTextProperty(sliceTextProp);
    vtkNew(vtkActor2D, sliceTextActor);
    sliceTextActor->SetMapper(sliceTextMapper);
    sliceTextActor->SetPosition(15, 10);

    m_sliceNumbers.push_back(sliceTextMapper.Get());

    renderer->AddActor2D(sliceTextActor);
}

int MainWindow::MapViewportNumber(int index) {
    int r = index / m_col;
    int c = index - r * m_col;
    int newr = m_row - r - 1;
    int newIndex = newr * m_col + c;
    return newIndex;
}

void MainWindow::SynchronizeSlider() {
    int max = m_imageFiles.size() > (m_row * m_col)
        ? m_imageFiles.size() - (m_row * m_col)
        : 0;
    m_slider->setMinimum(0);
    m_slider->setMaximum(max);
    m_slider->setValue(m_currentSliceNumber);
}

void MainWindow::UpdateSliceNumberCornerText(int sliceNumber,
    int RendererIndex) {
    vtkTextMapper* sliceMapper = m_sliceNumbers[RendererIndex];
    if (sliceMapper) {
        std::string msg =
            StatusMessage::Format(sliceNumber, m_imageFiles.size() - 1);
        sliceMapper->SetInput(msg.c_str());
        sliceMapper->Update();
    }
}

void MainWindow::ClearViewports() {
    vtkSmartPointer<vtkRenderWindow> renderWindow = m_vtkView->GetRenderWindow();

    vtkRendererCollection* col = renderWindow->GetRenderers();
    int size = col->GetNumberOfItems();
    for (int p = 0; p < size; p++) {
        vtkRenderer* ren = (vtkRenderer*)col->GetItemAsObject(p);
        if (ren) renderWindow->RemoveRenderer(ren);
    }
    m_renderers.clear();
    m_imageActors.clear();
    m_sliceNumbers.clear();
    m_topoEx.clear();
}

void MainWindow::ReadImageVolume() {
    vtkNew<vtkDICOMImageReader> reader;
    reader->SetDirectoryName(m_dicom_dir_path.toLatin1());
    reader->Update();

    m_imageVolume = reader->GetOutput();

    ConvertImageVolumeToSeparateImages();
}

void MainWindow::ConvertImageVolumeToSeparateImages() {
    int* D = m_imageVolume->GetDimensions();
    int xDim = D[0];
    int yDim = D[1];
    int zDim = D[2];

    m_imagedata.clear();

    for (int i = 0; i < D[2]; i++) {
        vtkNew<vtkImageData> output;
        FetchXYImage(output, i);
        m_imagedata.push_back(output);
    }
}

void MainWindow::FetchXYImage(vtkSmartPointer<vtkImageData> output,
    int Zindex) {
    vtkSmartPointer<vtkImageData> input = m_imageVolume;
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

    for (int y = 0; y < ydim; y++) {
        for (int x = 0; x < xdim; x++) {
            short* d = static_cast<short*>(output->GetScalarPointer(x, y, 0));
            short* s = static_cast<short*>(input->GetScalarPointer(x, y, Zindex));
            d[0] = s[0];
        }
    }
}

void MainWindow::ToggleVisibility() {
    m_visibility = !m_visibility;
    avTopoObserver::getInstance().SetVisibility(m_visibility);
    m_vtkView->GetRenderWindow()->Render();
}

void MainWindow::ResetPosition() {
    avTopoObserver::getInstance().ResetPosition();
    m_vtkView->GetRenderWindow()->Render();
}

void MainWindow::ToggleSynchronization() {
    avTopoObserver::getInstance().SetEnabled(
        !avTopoObserver::getInstance().Enabled());
    m_vtkView->GetRenderWindow()->Render();
}

void MainWindow::DrawTopoEx() {
    for (auto t : m_topoEx) {
        t->Start();
    }
}

void MainWindow::UpdateTopoEx(int sliceIndex, int topoIndex) {
    std::shared_ptr<avTopoViewerEx> topo = m_topoEx[topoIndex];
    topo->SetCurrentSlice(sliceIndex);
}

void MainWindow::ProcessMousePoint(int mx, int my) {
    // std::cout << "\nX:" << mx << " Y:" << my;
    for (auto t : m_topoEx) {
        t->ProcessMousePoint(mx, my);
    }
}

void MainWindow::LeftButtonDown(int mx, int my) {
    for (auto t : m_topoEx) {
        t->LeftButtonDown(mx, my);
    }
}

void MainWindow::LeftButtonUp(int mx, int my) {
    for (auto t : m_topoEx) {
        t->LeftButtonUp(mx, my);
    }
}

void MainWindow::ReadConfigurationFile() {
    QString pathToFile;
    pathToFile = QCoreApplication::applicationDirPath() + "/";
    QString f;
    f = QString(pathToFile + "../../../data/viewing.ini");

    QFile file(f);
    if (!file.exists()) return;

    QSettings s(f, QSettings::IniFormat);
    int width = s.value("TOPO_THUMBNAIL_WIDTH_PERCENTAGE").toInt();
    int leftMargin = s.value("TOPO_THUMBNAIL_LEFT_MARGIN_PERCENTAGE").toInt();
    int topMargin = s.value("TOPO_THUMBNAIL_TOP_MARGIN_PERCENTAGE").toInt();
    m_topoLeftMargin = (double)leftMargin / 100.0;
    m_topoTopMargin = (double)topMargin / 100.0;
    m_topoWidth = (double)width / 100;
}

void MainWindow::ProcessInputData() {
    QFile file(m_file_name);
    if (!file.exists()) {
        return;
    }

    QFileInfo fileinfo(m_file_name);
    QDir dirname = fileinfo.absoluteDir();
    QString dirpath = dirname.absolutePath();
    m_dicom_dir_path = dirpath;

    m_imageFiles.clear();
    m_currentSliceNumber = 0;

    QStringList stringlist;
    stringlist << "*.dcm";
    QStringList images = dirname.entryList(stringlist, QDir::Files);
    foreach(QString filename, images) {
        m_imageFiles.push_back(filename.toStdString());
    }
    sort(m_imageFiles.begin(), m_imageFiles.end(), numeric_string_compare);

    SynchronizeSlider();

    QTimer::singleShot(100, this, &MainWindow::initialiseWithDICOM);
}