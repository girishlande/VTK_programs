#include "avTopoViewerEx.h"

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

void avTopoViewerEx::SetWindowLevel(double window, double level) {
  m_window = window;
  m_level = level;
}

void avTopoViewerEx::SetTopoViewSize(double width, double height) {
  if (width > 0.0 && width < 1.0) m_topoWidth = width;
  if (height > 0.0 && height < 1.0) m_topoHeight = height;
}

void avTopoViewerEx::AddTopoline() {
  vtkNew<vtkPoints> points;
  points->SetNumberOfPoints(2);
  points->InsertPoint(0, 0, 0, 0);
  points->InsertPoint(1, m_topoWidth, 0, 0);
  m_linePoints = points.Get();

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
  actor->GetProperty()->SetColor(m_colors->GetColor3d(m_lineColor).GetData());
  actor->GetProperty()->SetLineWidth(1.0);
  m_LineActor = actor.Get();
  m_LineActor->SetDisplayPosition(m_topoX_DC, m_topoY_DC);

  m_renderer->AddViewProp(actor);
}

void avTopoViewerEx::SetCurrentSlice(int slice_number) {
  if (slice_number < m_minSliceNumber || slice_number > m_maxSliceNumber)
    return;
  if (m_sliceNumber != slice_number) {
    m_sliceNumber = slice_number;
    UpdateTopoLine();
  }
}

void avTopoViewerEx::UpdateTopoLine() {
  float val = (float)m_sliceNumber / m_maxSliceNumber;

  if (m_linePoints) {
    double p1[3] = {0, val * m_topoHeight, 0};
    double p2[3] = {m_topoWidth, val * m_topoHeight, 0};
    m_linePoints->SetPoint(0, p1);
    m_linePoints->SetPoint(1, p2);
    if (m_LineActor) m_LineActor->Modified();
  }
}

void avTopoViewerEx::DisplyRendererDetails(vtkRenderer* renderer) {
  int* pOrigin = renderer->GetOrigin();
  int* pSize = renderer->GetSize();
  cout << "\n Origin and size:" << pOrigin[0] << "  " << pOrigin[1]
       << "  Size:" << pSize[0] << "  " << pSize[1];
}

void avTopoViewerEx::CalculateViewportDetails(vtkImageActor* imageActor) {
  double b[6];
  imageActor->GetDisplayBounds(b);
  double xsize = b[1];
  double ysize = b[3];
  cout << "\nImage size:" << xsize << " " << ysize;

  int* p = m_renderer->GetSize();
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

void avTopoViewerEx::SetTopoPositionNormalised(double top, double left) {
  m_topMargin = top;
  m_leftMargin = left;
}

void avTopoViewerEx::SetBorderColor(std::string& color) {
  m_borderColor = color;
}
void avTopoViewerEx::SetTopoLineColor(std::string& color) {
  m_lineColor = color;
}

void avTopoViewerEx::CalculateViewportSize() {
  assert(m_renderer);

  double v[4];  // minx, miny, maxx, maxy;
  m_renderer->GetViewport(v);
  std::cout << std::endl << "Viewport \n";
  for (int i = 0; i < 4; i++) cout << std::endl << v[i];

  double vw = v[2] - v[0];
  assert(vw > 0);
  double vh = v[3] - v[1];
  assert(vh > 0);
  m_topoWidth = vw * m_topoWidth;
  m_topoHeight = vh * m_topoHeight;
  m_leftMargin = vw * m_leftMargin;
  m_topMargin = vh * m_topMargin;

  double tminx = v[0] + m_leftMargin;
  double tminy = v[1] + (vh - (m_topoHeight + m_topMargin));
  double tmaxx = tminx + m_topoWidth;
  double tmaxy = tminy + m_topoHeight;
  m_renderer->SetViewport(tminx, tminy, tmaxx, tmaxy);
}

void avTopoViewerEx::Start() {
  // Please dont change order of calling following functions.
  CacheWindowDimension();
  AddTopoImage();
  AddTopoBorder();
  AddTopoline();
}

void avTopoViewerEx::AddTopoImage() {
  vtkNew<vtkImageResize> resize;
  resize->SetInputData(m_topoImage);
  resize->SetOutputDimensions(100, 100, 1);
  resize->Update();

  vtkNew<vtkImageMapper> imageMapper;
  imageMapper->SetInputData(resize->GetOutput());
  imageMapper->SetColorWindow(255);
  imageMapper->SetColorLevel(127);
  imageMapper->SetRenderToRectangle(true);

  vtkNew<vtkActor2D> imageActor;
  imageActor->SetMapper(imageMapper);

  vtkCoordinate* p1 = imageActor->GetPositionCoordinate();
  p1->SetValue(0, 0);
  vtkCoordinate* p2 = imageActor->GetPosition2Coordinate();
  p2->SetValue(m_topoWidth, m_topoHeight);
  m_topoActor = imageActor.Get();
  m_topoActor->SetDisplayPosition(m_topoX_DC, m_topoY_DC);

  m_renderer->AddActor2D(imageActor);
  m_renderer->ResetCamera();
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

void avTopoViewerEx::SetTopoPositionDevice(int mx, int my) {
  if (m_topoActor) {
    m_topoActor->SetDisplayPosition(mx, my);
    m_borderActor->SetDisplayPosition(mx, my);
    m_renderer->GetRenderWindow()->Render();
  }
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
  int viewportWidthDC = m_windowWidth * viewportWidth;
  int viewportHeightDC = m_windowHeight * viewportHeight;

  m_topoWidth_DC = m_topoWidth * viewportWidthDC;
  m_topoHeight_DC = m_topoHeight * viewportHeightDC;

  int offset = 2;
  m_topoMinXDC = v[0] * m_windowWidth + offset;
  m_topoMinYDC = v[1] * m_windowHeight + offset;

  int viewportMaxXDC = v[2] * m_windowWidth;
  int viewportMaxYDC = v[3] * m_windowHeight;

  m_topoMaxXDC = viewportMaxXDC - m_topoWidth_DC - offset;
  m_topoMaxYDC = viewportMaxYDC - m_topoHeight_DC - offset;

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
      m_LineActor->SetDisplayPosition(newXdevice, newYdevice);
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