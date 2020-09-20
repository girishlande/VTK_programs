#ifndef TOPOVIEWER_H
#define TOPOVIEWER_H

#include "vtkImageData.h"
#include "vtkSmartPointer.h"

class vtkRenderer;
class vtkActor2D;
class vtkPoints;
class vtkImageActor;

class TopoViewer {

 public:
  enum class DirectionAxis {X_AXIS,Y_AXIS,Z_AXIS};

  explicit TopoViewer(vtkSmartPointer<vtkImageData> data,
                      vtkSmartPointer<vtkRenderer> renderer);
  ~TopoViewer();

  void setDirectionAxis(DirectionAxis viewingaxis, DirectionAxis topoaxis);
  void setWindowLevel(double window, double level);
  void setViewSize(double width, double height);

  void Start();
  void UpdateTopo(int slice);
  
 protected:
  void Initialise();

  void ViewportBorder();
  void fetchTopoImage(vtkSmartPointer<vtkImageData> output);

  void fetchYZImage(vtkSmartPointer<vtkImageData> input,
                    vtkSmartPointer<vtkImageData> output);
  void fetchXZImage(vtkSmartPointer<vtkImageData> input,
                    vtkSmartPointer<vtkImageData> output);
  void fetchXYImage(vtkSmartPointer<vtkImageData> input,
                    vtkSmartPointer<vtkImageData> output);

  void drawAxialLine();
  void displyRendererDetails(vtkRenderer* renderer);
  void calculateViewportDetails(vtkImageActor* actor);
  void displayImageActorDetails(vtkImageActor* imageActor);

 private:

  vtkSmartPointer<vtkRenderer> m_renderer;
  vtkSmartPointer<vtkImageData> m_imagedata;

  vtkPoints* m_linePoints = nullptr;
  vtkActor2D* m_LineActor = nullptr;
  vtkRenderer* m_parent_renderer;
  
  int m_minSliceNumber;
  int m_maxSliceNumber;

  double m_topoViewWidth = 0.3;
  double m_topoViewHeight = 0.2;
  double m_topoMarginTop = 0.05;
  double m_topoMarginLeft = 0.03;

  DirectionAxis m_viewingaxis = DirectionAxis::Z_AXIS;
  DirectionAxis m_topoaxis = DirectionAxis::X_AXIS;

  double m_window = 255.0;
  double m_level = 127.5;
};

#endif  // MAINWINDOW_H
