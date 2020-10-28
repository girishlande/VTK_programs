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

avTopoViewerEx::avTopoViewerEx(vtkSmartPointer<vtkImageData> topoImage,
                               vtkSmartPointer<vtkRenderer> renderer)
    : m_topoImage(topoImage), m_renderer(renderer) {}

avTopoViewerEx::~avTopoViewerEx() {}

void avTopoViewerEx::Start() {
  // Please dont change order of calling following functions.
  CacheWindowDimension();
  AddTopoImage();
  AddTopoBorder();
  AddTopoline(1);
  AddTopoline(2);
  AddSphere();
  m_renderer->GetRenderWindow()->Render();
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
  if (m_topoHighlighted) return;
  m_topoHighlighted = true;
  m_topoBorderProperty->SetColor(
      m_colors->GetColor3d(m_topoActiveColor).GetData());
  m_renderer->GetRenderWindow()->Render();
}

void avTopoViewerEx::DeselectTopo() {
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
  int* size = m_renderer->GetRenderWindow()->GetSize();
  m_windowWidth = size[0];
  m_windowHeight = size[1];

  double* v = m_renderer->GetViewport();
  double viewportWidth = v[2] - v[0];
  double viewportHeight = v[3] - v[1];
  m_viewportWidthDC = m_windowWidth * viewportWidth;
  m_viewportHeightDC = m_windowHeight * viewportHeight;

  m_topMargin_DC = m_topMargin * m_viewportWidthDC;
  m_leftMargin_DC = m_leftMargin * m_viewportHeightDC;

  // make sure our topo is always square
  m_topoHeight = (m_topoWidth * m_viewportWidthDC) / m_viewportHeightDC;

  m_topoWidth_DC = ceil(m_topoWidth * m_viewportWidthDC);
  m_topoHeight_DC = ceil(m_topoHeight * m_viewportHeightDC);

  // Offset so that User cannot drag topo to overlap with viewport border
  m_topoMinXDC = v[0] * m_windowWidth + m_viewportMargin;
  m_topoMinYDC = v[1] * m_windowHeight + m_viewportMargin;

  m_viewportMaxXDC = v[2] * m_windowWidth;
  m_viewportMaxYDC = v[3] * m_windowHeight;

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
      m_topoActor->SetDisplayPosition(newXdevice, newYdevice);
      m_borderActor->SetDisplayPosition(newXdevice, newYdevice);
      m_LineActor1->SetDisplayPosition(newXdevice, newYdevice);
      m_LineActor2->SetDisplayPosition(newXdevice, newYdevice);
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
}

vtkSmartPointer<vtkImageData> avTopoViewerEx::ResizeTopo() {
  vtkNew<vtkImageResize> resize;
  resize->SetInputData(m_topoImage);
  resize->SetOutputDimensions(m_topoWidth_DC, m_topoHeight_DC, 1);
  resize->Update();
  vtkSmartPointer<vtkImageData> output = resize->GetOutput();
  return output;
}

void avTopoViewerEx::PrepareImageMapper(
    vtkSmartPointer<vtkImageMapper> imageMapper) {
  if (m_maintainAspectRatio) CalculateTopoImageHeightDC();

  if (m_resizeTopo) {
    imageMapper->SetInputData(ResizeTopo());
  } else {
    imageMapper->SetInputData(m_topoImage);
  }
  imageMapper->SetColorWindow(m_window);
  imageMapper->SetColorLevel(m_level);
  imageMapper->SetRenderToRectangle(true);
}

#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>

void avTopoViewerEx::AddSphere() {
  vtkNew<vtkSphereSource> s1;
  s1->SetThetaResolution(100);
  s1->SetRadius(1000);
  s1->SetCenter(0, 0, 10000);
  s1->Update();

  vtkNew<vtkPolyDataMapper> sm1;
  sm1->SetInputConnection(s1->GetOutputPort());
  vtkSmartPointer<vtkActor> sa1 = vtkSmartPointer<vtkActor>::New();
  sa1->SetMapper(sm1);
  sa1->GetProperty()->SetColor(1, 0, 0);

  m_renderer->AddViewProp(sa1);
}