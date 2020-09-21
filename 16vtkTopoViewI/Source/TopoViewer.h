#ifndef TOPOVIEWER_H
#define TOPOVIEWER_H

#include <vtkImageData.h>
#include <vtkSmartPointer.h>

class vtkRenderer;
class vtkActor2D;
class vtkPoints;
class vtkImageActor;

class TopoViewer {
 public:
  enum class DirectionAxis { X_AXIS, Y_AXIS, Z_AXIS };

  explicit TopoViewer(vtkSmartPointer<vtkImageData> data,
                      vtkSmartPointer<vtkRenderer> renderer);
  ~TopoViewer();

  // Setup Viewer configuration 
  void SetDirectionAxis(DirectionAxis viewingaxis, DirectionAxis topoaxis);
  void SetWindowLevel(double window, double level);
  // Note: Specify values in normalised viewport (0.0 to 1.0)
  void SetViewSize(double width, double height); 
  void SetTopoPosition(double top, double left);
  void SetBorderColor(std::string& color);
  void SetTopoLineColor(std::string& color);

  void Start();
  void UpdateTopoView(int viewingslice);

 protected:
  void Initialise();
  void ViewportBorder();
  void FetchTopoImage(vtkSmartPointer<vtkImageData> output);
  void FetchYZImage(vtkSmartPointer<vtkImageData> input,
                    vtkSmartPointer<vtkImageData> output);
  void FetchXZImage(vtkSmartPointer<vtkImageData> input,
                    vtkSmartPointer<vtkImageData> output);
  void FetchXYImage(vtkSmartPointer<vtkImageData> input,
                    vtkSmartPointer<vtkImageData> output);

  void DrawTopoline();
  void DisplyRendererDetails(vtkRenderer* renderer);
  void CalculateViewportDetails(vtkImageActor* actor);
  void DisplayImageActorDetails(vtkImageActor* imageActor);

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

  std::string m_borderColor = "gold";
  std::string m_lineColor = "red";
};

#endif  // MAINWINDOW_H
