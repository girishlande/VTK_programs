#include "avTopoViewer.h"

#include <vtkActor2D.h>
#include <vtkCamera.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapper.h>
#include <vtkLineSource.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkPolyLine.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <sstream>

avTopoViewer::avTopoViewer(vtkSmartPointer<vtkImageData> imagedata,
                           vtkSmartPointer<vtkRenderer> renderer)
    : m_imagedata(imagedata), m_parent_renderer(renderer) {
  Initialise();
}

avTopoViewer::~avTopoViewer() {}

void avTopoViewer::Initialise() {
  if (m_imagedata) {
    int* dims = m_imagedata->GetDimensions();
    m_minSliceNumber = 0;

    switch (m_viewingaxis) {
      case DirectionAxis::X_AXIS:
        m_maxSliceNumber = dims[0] - 1;
        break;
      case DirectionAxis::Y_AXIS:
        m_maxSliceNumber = dims[1] - 1;
        break;
      case DirectionAxis::Z_AXIS:
        m_maxSliceNumber = dims[2] - 1;
        break;
    }
  }
}

void avTopoViewer::SetDirectionAxis(DirectionAxis viewingaxis,
                                    DirectionAxis topoaxis) {
  assert(viewingaxis != topoaxis);
  if (viewingaxis != topoaxis) {
    m_topoaxis = topoaxis;
    m_viewingaxis = viewingaxis;
    Initialise();
  }
}

void avTopoViewer::SetWindowLevel(double window, double level) {
  m_window = window;
  m_level = level;
}

void avTopoViewer::SetViewSize(double width, double height) {
  if (width > 0.0 && width < 1.0) m_width = width;
  if (height > 0.0 && height < 1.0) m_height = height;
}

// -----------------------------
// Start displaying topo view
// -----------------------------
void avTopoViewer::Start() {
  if (!m_imagedata) return;

  if (!m_topoImage) {
    m_topoImage = vtkSmartPointer<vtkImageData>::New();
    FetchTopoImage(m_topoImage);
  }

  vtkNew<vtkImageMapper> imageMapper;
  imageMapper->SetInputData(m_topoImage);
  imageMapper->SetColorWindow(m_window);
  imageMapper->SetColorLevel(m_level);
  imageMapper->SetRenderToRectangle(true);

  vtkNew<vtkActor2D> imageActor;
  imageActor->SetMapper(imageMapper);
  vtkCoordinate* p2 = imageActor->GetPosition2Coordinate();
  p2->SetValue(1.0, 1.0);

  m_renderer = vtkSmartPointer<vtkRenderer>::New();
  CalculateViewportSize();
  m_renderer->AddActor2D(imageActor);
  m_parent_renderer->GetRenderWindow()->AddRenderer(m_renderer);

  ViewportBorder();
  DrawTopoline();

  m_parent_renderer->GetRenderWindow()->Render();
}

// -------------------------
// Create viewport border
// -------------------------
void avTopoViewer::ViewportBorder() {
  vtkNew<vtkPoints> points;
  points->SetNumberOfPoints(4);
  points->InsertPoint(0, 1, 1, 0);
  points->InsertPoint(1, 0, 1, 0);
  points->InsertPoint(2, 0, 0, 0);
  points->InsertPoint(3, 1, 0, 0);

  vtkNew<vtkPolyLine> lines;
  lines->GetPointIds()->SetNumberOfIds(5);
  for (int i = 0; i < 4; ++i) {
    lines->GetPointIds()->SetId(i, i);
  }
  lines->GetPointIds()->SetId(4, 0);
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

  vtkNew<vtkNamedColors> colors;
  vtkNew<vtkActor2D> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d(m_borderColor).GetData());
  actor->GetProperty()->SetLineWidth(4.0);

  m_renderer->AddViewProp(actor);
}

// ---------------------------------------------------------------------
// Draw topo line indicating slide position in side view of the image
// ---------------------------------------------------------------------
void avTopoViewer::DrawTopoline() {
  vtkNew<vtkPoints> points;
  points->SetNumberOfPoints(2);
  points->InsertPoint(0, 0, 0, 0);
  points->InsertPoint(1, 1, 0, 0);
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

  vtkNew<vtkNamedColors> colors;
  vtkNew<vtkActor2D> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d(m_lineColor).GetData());
  actor->GetProperty()->SetLineWidth(1.0);
  m_LineActor = actor.Get();

  m_renderer->AddViewProp(actor);
}

// --------------------
// Fetch topo Image
// --------------------
void avTopoViewer::FetchTopoImage(vtkSmartPointer<vtkImageData> output) {
  switch (m_viewingaxis) {
    case avTopoViewer::DirectionAxis::X_AXIS:
      if (m_topoaxis == DirectionAxis::Y_AXIS)
        FetchXZImage(m_imagedata, output);
      else
        FetchXYImage(m_imagedata, output);
      break;
    case avTopoViewer::DirectionAxis::Y_AXIS:
      if (m_topoaxis == DirectionAxis::Z_AXIS)
        FetchXYImage(m_imagedata, output);
      else
        FetchYZImage(m_imagedata, output);
      break;
    case avTopoViewer::DirectionAxis::Z_AXIS:
      if (m_topoaxis == DirectionAxis::X_AXIS)
        FetchYZImage(m_imagedata, output);
      else
        FetchXZImage(m_imagedata, output);
      break;
    default:
      break;
  }
}

void avTopoViewer::FetchYZImage(vtkSmartPointer<vtkImageData> input,
                                vtkSmartPointer<vtkImageData> output) {
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

void avTopoViewer::FetchXZImage(vtkSmartPointer<vtkImageData> input,
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

void avTopoViewer::FetchXYImage(vtkSmartPointer<vtkImageData> input,
                                vtkSmartPointer<vtkImageData> output) {
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
          static_cast<short*>(input->GetScalarPointer(x, y, zdim / 2));
      pixel[0] = sourcepixel[0];
    }
  }
}

// ------------------------------------
// Update axial line for topo image
// ------------------------------------
void avTopoViewer::UpdateTopoView(int current) {
  if (current < m_minSliceNumber || current > m_maxSliceNumber) return;

  float val = (float)current / m_maxSliceNumber;
  if (m_linePoints) {
    double p1[3] = {0, val, 0};
    double p2[3] = {1, val, 0};
    m_linePoints->SetPoint(0, p1);
    m_linePoints->SetPoint(1, p2);
    if (m_LineActor) m_LineActor->Modified();
  }
}

// ---------------------------------------
// Display details about Image actor
// ---------------------------------------
void avTopoViewer::DisplayImageActorDetails(vtkImageActor* imageActor) {
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
void avTopoViewer::DisplyRendererDetails(vtkRenderer* renderer) {
  int* pOrigin = renderer->GetOrigin();
  int* pSize = renderer->GetSize();
  cout << "\n Origin and size:" << pOrigin[0] << "  " << pOrigin[1]
       << "  Size:" << pSize[0] << "  " << pSize[1];
}

// ----------------------------------------------
// Calculate how big viewport should be for image
// ----------------------------------------------
void avTopoViewer::CalculateViewportDetails(vtkImageActor* imageActor) {
  double b[6];
  imageActor->GetDisplayBounds(b);
  double xsize = b[1];
  double ysize = b[3];
  cout << "\nImage size:" << xsize << " " << ysize;

  int* p = m_parent_renderer->GetSize();
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

void avTopoViewer::SetTopoPosition(double top, double left) {
  m_topMargin = top;
  m_leftMargin = left;
}

void avTopoViewer::SetBorderColor(std::string& color) { m_borderColor = color; }
void avTopoViewer::SetTopoLineColor(std::string& color) { m_lineColor = color; }

void avTopoViewer::SetTopoImage(vtkSmartPointer<vtkImageData> topoimage) {
  m_topoImage = topoimage;
}

void avTopoViewer::CalculateViewportSize() {
  assert(m_renderer);
  assert(m_parent_renderer);

  double v[4];  // minx, miny, maxx, maxy;
  m_parent_renderer->GetViewport(v);
  std::cout << std::endl << "Viewport \n";
  for (int i = 0; i < 4; i++) cout << std::endl << v[i];

  double vw = v[2] - v[0];
  assert(vw > 0);
  double vh = v[3] - v[1];
  assert(vh > 0);
  m_width = vw * m_width;
  m_height = vh * m_height;
  m_leftMargin = vw * m_leftMargin;
  m_topMargin = vh * m_topMargin;

  double tminx = v[0] + m_leftMargin;
  double tminy = v[1] + (vh - (m_height + m_topMargin));
  double tmaxx = tminx + m_width;
  double tmaxy = tminy + m_height;
  m_renderer->SetViewport(tminx, tminy, tmaxx, tmaxy);
}