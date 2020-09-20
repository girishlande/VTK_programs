#include "TopoViewer.h"

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <sstream>

#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkImageMapper.h"
#include "vtkLineSource.h"
#include "vtkNamedColors.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPolyLine.h"
#include "vtkProperty2D.h"

TopoViewer::TopoViewer(vtkImageData* data, vtkRenderer* renderer)
    : m_imagedata(data), m_parent_renderer(renderer) {
  Initialise();
}

TopoViewer::~TopoViewer() {}

void TopoViewer::Initialise() {
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

void TopoViewer::setDirectionAxis(DirectionAxis viewingaxis,
                                  DirectionAxis topoaxis) {
  m_topoaxis = topoaxis;
  m_viewingaxis = viewingaxis;
  Initialise();
}

void TopoViewer::Start() {
  vtkNew<vtkImageData> output;
  fetchYZImage(m_imagedata, output);

  vtkNew<vtkImageMapper> imageMapper;
  imageMapper->SetInputData(output);
  imageMapper->SetColorWindow(255);
  imageMapper->SetColorLevel(127.5);
  imageMapper->SetRenderToRectangle(true);

  vtkNew<vtkActor2D> imageActor;
  imageActor->SetMapper(imageMapper);

  double bottomLeftX = m_topoMarginLeft;
  double bottomLeftY = 1.0 - (m_topoViewHeight + m_topoMarginTop);
  double topRightX = m_topoMarginLeft + m_topoViewWidth;
  double topRightY = 1.0f - m_topoMarginTop;
  vtkCoordinate* p2 = imageActor->GetPosition2Coordinate();
  p2->SetValue(1.0, 1.0);

  m_renderer = vtkSmartPointer<vtkRenderer>::New();
  m_renderer->SetViewport(bottomLeftX, bottomLeftY, topRightX, topRightY);
  m_renderer->AddActor2D(imageActor);
  m_parent_renderer->GetRenderWindow()->AddRenderer(m_renderer);

  ViewportBorder();
  drawAxialLine();

  m_parent_renderer->GetRenderWindow()->Render();
}

// -------------------------
// Create viewport border
// -------------------------
void TopoViewer::ViewportBorder() {
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
  actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());
  actor->GetProperty()->SetLineWidth(4.0);

  m_renderer->AddViewProp(actor);
}

// ---------------------------------------------------------------------
// Draw axial line indicating slide position in side view of the image
// ---------------------------------------------------------------------
void TopoViewer::drawAxialLine() {
  vtkNew<vtkPoints> points;
  points->SetNumberOfPoints(2);
  points->InsertPoint(0, 0, 0, 0);
  points->InsertPoint(1, 1, 0, 0);
  m_axialPoints = points.Get();

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
  actor->GetProperty()->SetColor(colors->GetColor3d("red").GetData());
  actor->GetProperty()->SetLineWidth(1.0);
  m_axialLineActor = actor.Get();

  m_renderer->AddViewProp(actor);
}

// --------------------
// Fetch YZ image
// --------------------
void TopoViewer::fetchYZImage(vtkSmartPointer<vtkImageData> input,
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

// ------------------------------------
// Update axial line for topo image
// ------------------------------------
void TopoViewer::updateAxialLine(int current) {
  if (current < m_minSliceNumber || current > m_maxSliceNumber) return;

  float val = (float)current / m_maxSliceNumber;
  if (m_axialPoints) {
    double p1[3] = {0, val, 0};
    double p2[3] = {1, val, 0};
    m_axialPoints->SetPoint(0, p1);
    m_axialPoints->SetPoint(1, p2);
    if (m_axialLineActor) m_axialLineActor->Modified();
  }
}

// ---------------------------------------
// Display details about Image actor
// ---------------------------------------
void TopoViewer::displayImageActorDetails(vtkImageActor* imageActor) {
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
void TopoViewer::displyRendererDetails(vtkRenderer* renderer) {
  int* pOrigin = renderer->GetOrigin();
  int* pSize = renderer->GetSize();
  cout << "\n Origin and size:" << pOrigin[0] << "  " << pOrigin[1]
       << "  Size:" << pSize[0] << "  " << pSize[1];
}

// ----------------------------------------------
// Calculate how big viewport should be for image
// ----------------------------------------------
void TopoViewer::calculateViewportDetails(vtkImageActor* imageActor) {
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