#ifndef TOPOVIEWEREX_H
#define TOPOVIEWEREX_H

#include <vtkImageData.h>
#include <vtkSmartPointer.h>

class vtkRenderer;
class vtkActor2D;
class vtkPoints;
class vtkImageActor;
class vtkProperty2D;
class vtkRenderWindowInteractor;

class avTopoViewerEx {
 public:
  enum class DirectionAxis { X_AXIS, Y_AXIS, Z_AXIS };

  explicit avTopoViewerEx(vtkSmartPointer<vtkImageData> data,
                      vtkSmartPointer<vtkRenderer> renderer);
  ~avTopoViewerEx();

  // Setup Viewer configuration 
  void SetWindowLevel(double window, double level);

  // Note: Specify values in normalised viewport (0.0 to 1.0)
  void SetViewSize(double width, double height); 
  void SetTopoPositionNormalised(double top, double left);
  void SetBorderColor(std::string& color);
  void SetTopoLineColor(std::string& color);

  void SetMaxSlice(int sliceCount);
  void UpdateTopoView(int viewingslice);

  void Start();
  void SetTopoPositionDevice(int mx, int my);
  bool IsPointWithinTopo(int x, int y);
  void SelectTopo();
  void DeselectTopo();
  
  void ProcessMousePoint(int mx, int my);
  void LeftButtonDown(int mx, int my);
  void LeftButtonUp(int mx, int my);

 protected:
  void ViewportBorder();

  void DrawTopoline();
  void DisplyRendererDetails(vtkRenderer* renderer);
  void CalculateViewportDetails(vtkImageActor* actor);
  void CalculateViewportSize();
  void DrawActorBorder(double* color); 

  void CacheWindowDimension();

  void NormalisedToDeviceCoordinates(double nx, double ny, int& dx, int& dy);
  void DeviceToNormalised(int dx, int dy, double& nx, double& ny);

  void RestrictWithinViewport(int& dx, int& dy);

 private:

  vtkSmartPointer<vtkImageData> m_topoImage;
  vtkSmartPointer<vtkRenderer> m_renderer;

  vtkPoints* m_linePoints = nullptr;
  vtkActor2D* m_LineActor = nullptr;
  
  int m_minSliceNumber;
  int m_maxSliceNumber;

  int m_topoX_device = 0;
  int m_topoY_device = 0;
  int m_topoWidth_device = 10;
  int m_topoHeight_device = 10;
  int m_topoMinX = 0;
  int m_topoMinY = 0;
  int m_topoMaxX = 10;
  int m_topoMaxY = 10;

  double m_topoX = 0.0;
  double m_topoY = 0.0;
  double m_topoWidth = 0.2;
  double m_topoHeight = 0.2;
  double m_topMargin = 0.0;
  double m_leftMargin = 0.0;

  double m_viewport[4] = {0, 0, 1, 1}; // viewport bounds in normalised

  DirectionAxis m_viewingaxis = DirectionAxis::Z_AXIS;
  DirectionAxis m_topoaxis = DirectionAxis::X_AXIS;

  double m_window = 255.0;
  double m_level = 127.5;

  std::string m_borderColor = "gold";
  std::string m_lineColor = "green";

  vtkActor2D* m_image2DActor = nullptr;
  vtkActor2D* m_topoActor = nullptr;
  vtkRenderWindowInteractor* m_interactor = nullptr;

  vtkProperty2D* m_topoBorderProperty = nullptr;

  bool m_topoHighlighted = false;
  bool m_topoDragging = false;
  int m_dragStartX;
  int m_dragStartY;

  int m_windowWidth;
  int m_windowHeight;
};

#endif  // MAINWINDOW_H
