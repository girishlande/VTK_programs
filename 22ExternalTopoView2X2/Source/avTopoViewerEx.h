#ifndef TOPOVIEWEREX_H
#define TOPOVIEWEREX_H

#include <vtkImageData.h>
#include <vtkNamedColors.h>
#include <vtkSmartPointer.h>

class vtkRenderer;
class vtkActor2D;
class vtkPoints;
class vtkImageActor;
class vtkProperty2D;
class vtkRenderWindowInteractor;

class avTopoViewerEx {
 public:
  explicit avTopoViewerEx(vtkSmartPointer<vtkImageData> topoimage,
                          vtkSmartPointer<vtkRenderer> renderer);
  ~avTopoViewerEx();

  void SetWindowLevel(double window, double level);

  // Note: Specify values in normalised viewport (0.0 to 1.0)
  void SetTopoViewSize(double width, double height);
  void SetTopoPositionNormalised(double top, double left);
  void SetBorderColor(std::string& color);
  void SetTopoLineColor(std::string& color);

  void SetMaxSlice(int sliceCount);
  void SetCurrentSlice(int slice_number);

  void Start();
  void SetTopoPositionDevice(int mx, int my);
  bool IsPointWithinTopo(int x, int y);
  void SelectTopo();
  void DeselectTopo();

  void ProcessMousePoint(int mx, int my);
  void LeftButtonDown(int mx, int my);
  void LeftButtonUp(int mx, int my);

 protected:

  void AddTopoImage();
  void AddTopoline();
  void AddTopoBorder();
  
  void DisplyRendererDetails(vtkRenderer* renderer);
  void CalculateViewportDetails(vtkImageActor* actor);
  void CalculateViewportSize();
  
  void CacheWindowDimension();
  void NormalisedToDeviceCoordinates(double nx, double ny, int& dx, int& dy);
  void DeviceToNormalised(int dx, int dy, double& nx, double& ny);
  void RestrictWithinViewport(int& dx, int& dy);
  void UpdateTopoLine();

 private:
  
  int m_sliceNumber = 0;
  int m_minSliceNumber = 0;
  int m_maxSliceNumber = 0;

  int m_topoX_DC = 0;
  int m_topoY_DC = 0;
  int m_topoWidth_DC = 0;
  int m_topoHeight_DC = 0;
  int m_topMargin_DC = 10;
  int m_leftMargin_DC = 10;

  // TopoView position (left,bottom) and (top,right) in device coordinates
  int m_topoMinXDC = 0;
  int m_topoMinYDC = 0;
  int m_topoMaxXDC = 10;
  int m_topoMaxYDC = 10;

  // TopoView position in normalised viewport coordinates
  double m_topoX = 0.0;
  double m_topoY = 0.0;
  double m_topoWidth = 0.2;
  double m_topoHeight = 0.2;
  double m_topMargin = 0.0;
  double m_leftMargin = 0.0;

  double m_window = 255.0;
  double m_level = 127.5;

  std::string m_topoActiveColor = "white";
  std::string m_borderColor = "gray";
  std::string m_lineColor = "white";

  vtkProperty2D* m_topoBorderProperty = nullptr;
  vtkPoints* m_linePoints = nullptr;
  vtkActor2D* m_LineActor = nullptr;
  vtkActor2D* m_topoActor = nullptr;
  vtkActor2D* m_borderActor = nullptr;
  
  vtkSmartPointer<vtkImageData> m_topoImage;
  vtkSmartPointer<vtkRenderer> m_renderer;

  bool m_topoHighlighted = false;
  bool m_topoDragging = false;
  int m_dragStartX;
  int m_dragStartY;

  int m_windowWidth;
  int m_windowHeight;

  vtkNew<vtkNamedColors> m_colors;
};

#endif  // MAINWINDOW_H
