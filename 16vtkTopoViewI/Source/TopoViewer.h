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

  explicit TopoViewer(vtkImageData* data,vtkRenderer* renderer);
  ~TopoViewer();

  void Start();
  void updateAxialLine(int slice);
  void setDirectionAxis(DirectionAxis viewingaxis, DirectionAxis topoaxis);

 protected:
  void Initialise();

  void ViewportBorder();
  void fetchYZImage(vtkSmartPointer<vtkImageData> input,
                                vtkSmartPointer<vtkImageData> output);
  void drawAxialLine();
  void displyRendererDetails(vtkRenderer* renderer);
  void calculateViewportDetails(vtkImageActor* actor);
  void displayImageActorDetails(vtkImageActor* imageActor);

 private:

  vtkSmartPointer<vtkRenderer> m_renderer;
  vtkSmartPointer<vtkImageData> m_imagedata;

  vtkPoints* m_axialPoints = nullptr;
  vtkActor2D* m_axialLineActor = nullptr;
  vtkRenderer* m_parent_renderer;
  
  int m_minSliceNumber;
  int m_maxSliceNumber;

  double m_topoViewWidth = 0.3;
  double m_topoViewHeight = 0.2;
  double m_topoMarginTop = 0.05;
  double m_topoMarginLeft = 0.03;

  DirectionAxis m_viewingaxis = DirectionAxis::Z_AXIS;
  DirectionAxis m_topoaxis = DirectionAxis::X_AXIS;
};

#endif  // MAINWINDOW_H
