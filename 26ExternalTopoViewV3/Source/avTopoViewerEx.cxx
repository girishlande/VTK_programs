#include "avTopoViewerEx.h"

#include <math.h>
#include <vtkActor2D.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapper.h>
#include <vtkImageResize.h>
#include <vtkLineSource.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkPolyLine.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <sstream>

#include "avTopoObserver.h"

std::mutex m_topoViewerMutex;

avTopoViewerEx::avTopoViewerEx(vtkSmartPointer<vtkImageData> topoImage,
                               vtkSmartPointer<vtkRenderer> renderer,
    const QString& seriesId)
    : m_topoImage(topoImage), m_renderer(renderer), m_serieID(seriesId) {
  RegisterCallbacks();
  avTopoObserver::getInstance().RegisterTopo(this);
}

avTopoViewerEx::~avTopoViewerEx() {
  avTopoObserver::getInstance().UnRegisterTopo(this);
  RemoveActors();
}

void avTopoViewerEx::Start() {
  // Please dont change order of calling following functions.
  CacheWindowDimension();
  AddTopoImage();
  AddTopoBorder();
  AddTopoline(1);
  AddTopoline(2);
}

bool avTopoViewerEx::IsVisible() { return m_visible; }

void avTopoViewerEx::SetVisibility(bool flag) {
  if (m_visible == flag) return;
  m_visible = flag;
  if (m_topoActor) m_topoActor->SetVisibility(m_visible);
  if (m_LineActor1) m_LineActor1->SetVisibility(m_visible);
  if (m_LineActor2) m_LineActor2->SetVisibility(m_visible);
  if (m_borderActor) m_borderActor->SetVisibility(m_visible);
}

void avTopoViewerEx::SetWindowLevel(double window, double level) {
  m_window = window;
  m_level = level;
}

void avTopoViewerEx::SetResize(bool flag) { m_resizeTopo = flag; }
void avTopoViewerEx::SetAspectRatio(bool flag) { m_maintainAspectRatio = flag; }

void avTopoViewerEx::UpdateTopoImage(vtkSmartPointer<vtkImageData> topoimage) {
  if (m_topoActor) {
    m_topoImage = topoimage;

    vtkNew<vtkImageMapper> imageMapper;
    PrepareImageMapper(imageMapper);

    m_topoActor->SetMapper(imageMapper);
    m_topoActor->Modified();
  }
}

void avTopoViewerEx::SetTopoViewSize(double width, double height) {
  if (width > 0.0 && width < 1.0) m_topoWidth = width;
  if (height > 0.0 && height < 1.0) m_topoHeight = height;
}

void avTopoViewerEx::AddTopoline(int index) {
  vtkNew<vtkPoints> points;
  points->SetNumberOfPoints(2);
  points->InsertPoint(0, 0, 0, 0);
  points->InsertPoint(1, m_topoWidth, 0, 0);

  vtkNew<vtkPolyLine> lines;
  lines->GetPointIds()->SetNumberOfIds(2);
  lines->GetPointIds()->SetId(0, 0);
  lines->GetPointIds()->SetId(1, 1);
  vtkNew<vtkCellArray> cells;
  cells->Initialize();
  cells->InsertNextCell(lines);

  vtkNew<vtkPolyData> poly;
  poly->Initialize();
  poly->SetPoints(points);
  poly->SetLines(cells);

  vtkNew<vtkCoordinate> coordinate;
  coordinate->SetCoordinateSystemToNormalizedViewport();
  vtkNew<vtkPolyDataMapper2D> mapper;
  mapper->SetInputData(poly);
  mapper->SetTransformCoordinate(coordinate);

  vtkNew<vtkActor2D> actor;
  actor->SetMapper(mapper);
  if (index == 1) {
    m_linePoints1 = points.Get();
    m_LineActor1 = actor.Get();
    m_LineActor1->SetDisplayPosition(m_topoX_DC, m_topoY_DC);
    actor->GetProperty()->SetColor(m_colors->GetColor3d(m_lineColor).GetData());
    actor->GetProperty()->SetLineWidth(2.0);
  } else {
    m_linePoints2 = points.Get();
    m_LineActor2 = actor.Get();
    m_LineActor2->SetDisplayPosition(m_topoX_DC, m_topoY_DC);
    actor->GetProperty()->SetColor(
        m_colors->GetColor3d(m_shadowColor).GetData());
    actor->GetProperty()->SetLineWidth(1.0);
  }

  m_renderer->AddViewProp(actor);
}

void avTopoViewerEx::SetCurrentSlice(int slice_number) {
  if (slice_number < m_minSliceNumber || slice_number > m_maxSliceNumber)
    return;
  if (m_sliceNumber != slice_number) {
    m_sliceNumber = slice_number;
    UpdateTopoLineUsingSliceNumber();
  }
}

void avTopoViewerEx::UpdateTopoLineUsingSliceNumber() {
  float val = (float)m_sliceNumber / m_maxSliceNumber;

  if (m_linePoints1 && m_linePoints2) {
    double p1[3] = {0, val * m_topoHeight, 0};
    double p2[3] = {m_topoWidth, val * m_topoHeight, 0};
    m_linePoints1->SetPoint(0, p1);
    m_linePoints1->SetPoint(1, p2);
    m_linePoints2->SetPoint(0, p1);
    m_linePoints2->SetPoint(1, p2);
    if (m_LineActor1 && m_LineActor2) {
      m_LineActor1->Modified();
      m_LineActor2->Modified();
    }
  }
}

void avTopoViewerEx::SetTopoPositionNormalised(double top, double left) {
  if (top >= 0.0 && top <= 1.0) m_topMargin = top;
  if (left >= 0.0 && left <= 1.0) m_leftMargin = left;
}

void avTopoViewerEx::SetBorderColor(std::string& color) {
  m_borderColor = color;
}
void avTopoViewerEx::SetTopoLineColor(std::string& color) {
  m_lineColor = color;
}
void avTopoViewerEx::SetTopoLineShadowColor(std::string& color) {
  m_shadowColor = color;
}

void avTopoViewerEx::SetLineReferencePointNormalised(double minPos,
                                                     double maxPos) {
  if (minPos >= 0 && minPos <= 1.0) {
    m_sliceMinPos = minPos;
    if (maxPos >= 0.0 && minPos <= 1.0) {
      m_sliceMaxPos = maxPos;
    } else {
      m_sliceMaxPos = minPos;
    }
    UpdateTopoLineUsingReferencePoints();
  }
}

void avTopoViewerEx::UpdateTopoLineUsingReferencePoints() {
  if (m_linePoints1 && m_linePoints2) {
    double p1[3] = {0, m_sliceMinPos * m_topoHeight, 0};
    double p2[3] = {m_topoWidth, m_sliceMaxPos * m_topoHeight, 0};
    m_linePoints1->SetPoint(0, p1);
    m_linePoints1->SetPoint(1, p2);
    m_linePoints2->SetPoint(0, p1);
    m_linePoints2->SetPoint(1, p2);
    if (m_LineActor1 && m_LineActor2) {
      m_LineActor1->Modified();
      m_LineActor2->Modified();
    }
  }
}

void avTopoViewerEx::AddTopoImage() {
  vtkNew<vtkImageMapper> imageMapper;
  PrepareImageMapper(imageMapper);

  vtkNew<vtkActor2D> imageActor;
  imageActor->SetMapper(imageMapper);

  vtkCoordinate* p1 = imageActor->GetPositionCoordinate();
  p1->SetValue(0, 0);
  vtkCoordinate* p2 = imageActor->GetPosition2Coordinate();
  p2->SetValue(m_topoWidth, m_topoHeight);
  m_topoActor = imageActor.Get();
  m_topoActor->SetDisplayPosition(m_topoX_DC, m_topoY_DC);
  m_originalPositionX_DC = m_topoX_DC;
  m_originalPositionY_DC = m_topoY_DC;

  m_renderer->AddActor2D(imageActor);
}

void avTopoViewerEx::AddTopoBorder() {
  vtkNew<vtkPoints> points;
  points->SetNumberOfPoints(4);
  points->InsertPoint(0, 1 * m_topoWidth, 1 * m_topoHeight, 0);
  points->InsertPoint(1, 0, 1 * m_topoHeight, 0);
  points->InsertPoint(2, 0, 0, 0);
  points->InsertPoint(3, 1 * m_topoWidth, 0, 0);

  vtkNew<vtkCellArray> cells;
  cells->Initialize();
  vtkNew<vtkPolyLine> lines;
  lines->GetPointIds()->SetNumberOfIds(5);
  for (unsigned int i = 0; i < 4; ++i) {
    lines->GetPointIds()->SetId(i, i);
  }
  lines->GetPointIds()->SetId(4, 0);
  cells->InsertNextCell(lines);

  vtkNew<vtkPolyData> poly;
  poly->Initialize();
  poly->SetPoints(points);
  poly->SetLines(cells);

  vtkNew<vtkCoordinate> coordinate;
  coordinate->SetCoordinateSystemToNormalizedViewport();

  vtkNew<vtkPolyDataMapper2D> mapper;
  mapper->SetInputData(poly);
  mapper->SetTransformCoordinate(coordinate);

  vtkNew<vtkActor2D> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(m_colors->GetColor3d(m_borderColor).GetData());
  actor->GetProperty()->SetLineWidth(1.0);  // Line Width
  m_topoBorderProperty = actor->GetProperty();
  m_borderActor = actor.Get();
  m_borderActor->SetDisplayPosition(m_topoX_DC, m_topoY_DC);

  m_renderer->AddViewProp(actor);
}

bool avTopoViewerEx::IsPointWithinTopo(int dx, int dy) {
  if (dx >= m_topoX_DC && dx <= (m_topoX_DC + m_topoWidth_DC) &&
      dy >= m_topoY_DC && dy <= (m_topoY_DC + m_topoHeight_DC)) {
    SelectTopo();
    return true;
  }

  DeselectTopo();
  return false;
}

void avTopoViewerEx::SelectTopo() {
  return;
  if (m_topoHighlighted) return;
  m_topoHighlighted = true;
  m_topoBorderProperty->SetColor(
      m_colors->GetColor3d(m_topoActiveColor).GetData());
  m_renderer->GetRenderWindow()->Render();
}

void avTopoViewerEx::DeselectTopo() {
  return;
  if (!m_topoHighlighted) return;
  m_topoHighlighted = false;
  m_topoBorderProperty->SetColor(m_colors->GetColor3d(m_borderColor).GetData());
  m_renderer->GetRenderWindow()->Render();
}

void avTopoViewerEx::NormalisedToDeviceCoordinates(double x, double y, int& dx,
                                                   int& dy) {
  dx = vtkMath::Round(x * m_windowWidth);
  dy = vtkMath::Round(y * m_windowHeight);
}

void avTopoViewerEx::DeviceToNormalised(int dx, int dy, double& nx,
                                        double& ny) {
  nx = dx / m_windowWidth;
  ny = dy / m_windowHeight;
}

void avTopoViewerEx::CacheWindowDimension() {
  // window width and height in Device space
  int* size = m_renderer->GetRenderWindow()->GetSize();
  m_windowWidth = size[0];
  m_windowHeight = size[1];

  // Viewport width and height in device space
  double* v = m_renderer->GetViewport();
  double vMinX = v[0];
  double vMinY = v[1];
  double vMaxX = v[2];
  double vMaxY = v[3];
  double viewportWidth = vMaxX - vMinX;
  double viewportHeight = vMaxY - vMinY;
  m_viewportWidthDC = m_windowWidth * viewportWidth;
  m_viewportHeightDC = m_windowHeight * viewportHeight;
  m_viewportX = ceil(vMinX * m_windowWidth);
  m_viewportY = ceil(vMinY * m_windowHeight);

  // topmargin and leftmargin in device space
  m_topMargin_DC = m_topMargin * m_viewportHeightDC;
  m_leftMargin_DC = m_leftMargin * m_viewportWidthDC;

  // by default try to have topo height same as width.
  m_topoHeight = (m_topoWidth * m_viewportWidthDC) / m_viewportHeightDC;

  // Calculate topo width and height in device space
  m_topoWidth_DC = ceil(m_topoWidth * m_viewportWidthDC);
  m_topoHeight_DC = ceil(m_topoHeight * m_viewportHeightDC);

  // Calculate minimum value for topo X and Y in device coordinates
  m_topoMinXDC = vMinX * m_windowWidth + m_viewportMargin;
  m_topoMinYDC = vMinY * m_windowHeight + m_viewportMargin;

  m_viewportMaxXDC = vMaxX * m_windowWidth;
  m_viewportMaxYDC = vMaxY * m_windowHeight;

  m_topoMaxXDC = m_viewportMaxXDC - m_topoWidth_DC - m_viewportMargin;
  m_topoMaxYDC = m_viewportMaxYDC - m_topoHeight_DC - m_viewportMargin;

  m_topoX_DC = m_topoMinXDC + m_leftMargin_DC;
  m_topoY_DC = m_topoMaxYDC - m_topMargin_DC;
}

void avTopoViewerEx::ProcessMousePoint(int mx, int my) {
  if (m_topoDragging) {
    int moveX = mx - m_dragStartX;
    int moveY = my - m_dragStartY;
    int newXdevice = m_topoX_DC + moveX;
    int newYdevice = m_topoY_DC + moveY;
    if (m_topoActor) {
      RestrictWithinViewport(newXdevice, newYdevice);

      avTopoObserver::getInstance().PositionChanging(this,moveX, moveY);

      UpdateActorPosition(newXdevice, newYdevice);

      m_renderer->GetRenderWindow()->Render();
    }
  } else {
    IsPointWithinTopo(mx, my);
  }
}

void avTopoViewerEx::LeftButtonDown(int mx, int my) {
  if (IsPointWithinTopo(mx, my)) {
    m_topoDragging = true;
    m_dragStartX = mx;
    m_dragStartY = my;
    avTopoObserver::getInstance().BeginPositioning(this);
  }
}

void avTopoViewerEx::LeftButtonUp(int mx, int my) {
  if (m_topoDragging) {
    m_topoDragging = false;
    int moveX = mx - m_dragStartX;
    int moveY = my - m_dragStartY;
    int newXdevice = m_topoX_DC + moveX;
    int newYdevice = m_topoY_DC + moveY;
    RestrictWithinViewport(newXdevice, newYdevice);
    m_topoX_DC = newXdevice;
    m_topoY_DC = newYdevice;
    avTopoObserver::getInstance().PositionUpdated(this, moveX, moveY);
  }
}

void avTopoViewerEx::RestrictWithinViewport(int& dx, int& dy) {
  if (dx <= m_topoMinXDC) dx = m_topoMinXDC;
  if (dy <= m_topoMinYDC) dy = m_topoMinYDC;
  if (dx >= m_topoMaxXDC) dx = m_topoMaxXDC;
  if (dy >= m_topoMaxYDC) dy = m_topoMaxYDC;
}

void avTopoViewerEx::SetMaxSlice(int sliceCount) {
  m_minSliceNumber = 0;
  m_maxSliceNumber = sliceCount;
}

void avTopoViewerEx::CalculateTopoImageHeightDC() {
  int* D = m_topoImage->GetDimensions();
  int width = D[0];
  int height = D[1];

  double newHeight = static_cast<double>((height * m_topoWidth_DC) / width);
  m_topoHeight_DC = ceil(newHeight);
  m_topoHeight = static_cast<double>(m_topoHeight_DC) / m_viewportHeightDC;
  m_topoMaxYDC = m_viewportMaxYDC - m_topoHeight_DC - m_viewportMargin;
  m_topoY_DC = m_topoMaxYDC - m_topMargin_DC;
}

vtkSmartPointer<vtkImageData> avTopoViewerEx::ResizeTopo() {
  if (m_maintainAspectRatio) CalculateTopoImageHeightDC();

  vtkNew<vtkImageResize> resize;
  resize->SetInputData(m_topoImage);
  resize->SetOutputDimensions(m_topoWidth_DC, m_topoHeight_DC, 1);

  // TODO: Avoid resizing here. The incoming TOPO image (vtkImageData) has low
  // resolution as needed by the thumbnail. avTopoViewerEx should be made
  // capable of showing the incoming vtkImageData without any further data
  // manipulation. Reason: A design decision is to perform all the data
  // manipulations at the source (data model), not during viewing. (data view).
  resize->Update();
  vtkSmartPointer<vtkImageData> output = resize->GetOutput();
  return output;
}

void avTopoViewerEx::PrepareImageMapper(
    vtkSmartPointer<vtkImageMapper> imageMapper) {
  if (m_resizeTopo) {
    imageMapper->SetInputData(ResizeTopo());
  } else {
    imageMapper->SetInputData(m_topoImage);
  }
  imageMapper->SetColorWindow(m_window);
  imageMapper->SetColorLevel(m_level);
  imageMapper->SetRenderToRectangle(true);
}

void avTopoViewerEx::RemoveActors() {
  m_renderer->RemoveActor(m_topoActor);
  m_renderer->RemoveActor(m_borderActor);
  m_renderer->RemoveActor(m_LineActor1);
  m_renderer->RemoveActor(m_LineActor2);
}

void avTopoViewerEx::RegisterCallbacks() {
  if (m_renderer && m_interactionEnabled) {
    m_leftButtonPress = vtkSmartPointer<vtkCallbackCommand>::New();
    m_leftButtonPress->SetCallback(&avTopoViewerEx::callbackLeftButtonDown);
    m_leftButtonPress->SetClientData(this);
    m_leftButtonRelease = vtkSmartPointer<vtkCallbackCommand>::New();
    m_leftButtonRelease->SetCallback(&avTopoViewerEx::callbackLeftButtonUp);
    m_leftButtonRelease->SetClientData(this);
    m_mouseMove = vtkSmartPointer<vtkCallbackCommand>::New();
    m_mouseMove->SetCallback(&avTopoViewerEx::callbackMouseMove);
    m_mouseMove->SetClientData(this);

    m_renderer->GetRenderWindow()->GetInteractor()->AddObserver(
        vtkCommand::LeftButtonPressEvent, m_leftButtonPress);
    m_renderer->GetRenderWindow()->GetInteractor()->AddObserver(
        vtkCommand::LeftButtonReleaseEvent, m_leftButtonRelease);
    m_renderer->GetRenderWindow()->GetInteractor()->AddObserver(
        vtkCommand::MouseMoveEvent, m_mouseMove);
  }
}

void avTopoViewerEx::callbackLeftButtonDown(vtkObject* caller,
                                            long unsigned int eventId,
                                            void* clientData, void* callData) {
  vtkRenderWindowInteractor* iren =
      static_cast<vtkRenderWindowInteractor*>(caller);
  if (iren) {
    int m_X = iren->GetEventPosition()[0];
    int m_Y = iren->GetEventPosition()[1];
    avTopoViewerEx* client = static_cast<avTopoViewerEx*>(clientData);
    if (client) {
      client->LeftButtonDown(m_X, m_Y);
    }
  }
}

void avTopoViewerEx::callbackLeftButtonUp(vtkObject* caller,
                                          long unsigned int eventId,
                                          void* clientData, void* callData) {
  vtkRenderWindowInteractor* iren =
      static_cast<vtkRenderWindowInteractor*>(caller);
  if (iren) {
    int m_X = iren->GetEventPosition()[0];
    int m_Y = iren->GetEventPosition()[1];
    avTopoViewerEx* client = static_cast<avTopoViewerEx*>(clientData);
    if (client) {
      client->LeftButtonUp(m_X, m_Y);
    }
  }
}

void avTopoViewerEx::callbackMouseMove(vtkObject* caller,
                                       long unsigned int eventId,
                                       void* clientData, void* callData) {
  vtkRenderWindowInteractor* iren =
      static_cast<vtkRenderWindowInteractor*>(caller);
  if (iren) {
    int m_X = iren->GetEventPosition()[0];
    int m_Y = iren->GetEventPosition()[1];
    avTopoViewerEx* client = static_cast<avTopoViewerEx*>(clientData);
    if (client) {
      client->ProcessMousePoint(m_X, m_Y);
    }
  }
}

void avTopoViewerEx::SetInteraction(bool flag) { m_interactionEnabled = flag; }

void avTopoViewerEx::PositionChanging(int moveX, int moveY) {
  int newXdevice = m_topoX_DC + moveX;
  int newYdevice = m_topoY_DC + moveY;
  if (m_topoActor) {
    RestrictWithinViewport(newXdevice, newYdevice);
    UpdateActorPosition(newXdevice, newYdevice);
    m_renderer->GetRenderWindow()->Render();
  }
}

void avTopoViewerEx::PositionUpdated(int moveX, int moveY) {
  int newXdevice = m_topoX_DC + moveX;
  int newYdevice = m_topoY_DC + moveY;
  RestrictWithinViewport(newXdevice, newYdevice);
  m_topoX_DC = newXdevice;
  m_topoY_DC = newYdevice;
}

void avTopoViewerEx::ResetPosition() {
    std::lock_guard<std::mutex> guard(m_topoViewerMutex);

    m_topoX_DC = m_originalPositionX_DC;
    m_topoY_DC = m_originalPositionY_DC;
    UpdateActorPosition(m_topoX_DC, m_topoY_DC);
}

void avTopoViewerEx::SetPositionRelativeToViewport(int relativeX, int relativeY) {
    std::lock_guard<std::mutex> guard(m_topoViewerMutex);

    m_topoX_DC = m_viewportX + relativeX;
    m_topoY_DC = m_viewportY + relativeY;
    UpdateActorPosition(m_topoX_DC, m_topoY_DC);
}

void avTopoViewerEx::GetPositionRelativeToViewport(int& relativeX, int& relativeY) {
    relativeX = m_topoX_DC - m_viewportX;
    relativeY = m_topoY_DC - m_viewportY;
}

void avTopoViewerEx::UpdateActorPosition(int dx, int dy) {
    m_topoActor->SetDisplayPosition(dx, dy);
    m_borderActor->SetDisplayPosition(dx, dy);
    m_LineActor1->SetDisplayPosition(dx, dy);
    m_LineActor2->SetDisplayPosition(dx, dy);
}

void avTopoViewerEx::SetGroupId(int id) { m_groupId = id; }
int avTopoViewerEx::GroupId() { return m_groupId; }
QString avTopoViewerEx::SeriesId() { return m_serieID; }
