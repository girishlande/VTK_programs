#ifndef TOPOVIEWEREX_H
#define TOPOVIEWEREX_H

#include <qstring.h>
#include <vtkCallbackCommand.h>
#include <vtkImageData.h>
#include <vtkNamedColors.h>
#include <vtkSmartPointer.h>

#include <mutex>

class vtkRenderer;
class vtkActor2D;
class vtkPoints;
class vtkImageActor;
class vtkProperty2D;
class vtkRenderWindowInteractor;
class vtkImageMapper;

class avTopoViewerEx {
public:
    explicit avTopoViewerEx(vtkSmartPointer<vtkImageData> topoimage,
        vtkSmartPointer<vtkRenderer> renderer,
        const QString& seriesId);
    ~avTopoViewerEx();

    void Start();
    void SetVisibility(bool flag);
    bool IsVisible();

    // Configure topoview
    // Note: Specify values in normalised viewport (0.0 to 1.0)
    void SetTopoViewSize(double width, double height);
    void SetTopoPositionNormalised(double top, double left);
    void SetBorderColor(std::string& color);
    void SetTopoLineColor(std::string& color);
    void SetTopoLineShadowColor(std::string& color);
    void SetWindowLevel(double window, double level);
    void SetResize(bool flag);
    void SetAspectRatio(bool flag);
    void UpdateTopoImage(vtkSmartPointer<vtkImageData> topoimage);

    // Updating topoview reference line
    void SetLineReferencePointNormalised(double minPos, double maxPos = -1.0);
    void SetMaxSlice(int sliceCount);
    void SetCurrentSlice(int slice_number);

    // drag and move topoview
    void ProcessMousePoint(int mx, int my);
    void LeftButtonDown(int mx, int my);
    void LeftButtonUp(int mx, int my);
    
    // For update using observer
    void PositionChanging(int moveX, int moveY);
    void PositionUpdated(int moveX, int moveY);
    void ResetPosition();
    void GetPositionRelativeToViewport(int& relativeX, int& relativeY);
    void SetPositionRelativeToViewport(int relativeX, int relativeY);

    // Synchronization data
    void SetGroupId(int id);
    int GroupId();
    QString SeriesId();

    // Handle Mouse Interaction using observers
    void SetInteraction(bool flag);
    static void callbackLeftButtonDown(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
    static void callbackLeftButtonUp(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
    static void callbackMouseMove(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);

protected:
    bool IsPointWithinTopo(int x, int y);
    void SelectTopo();
    void DeselectTopo();

    void AddTopoImage();
    void AddTopoline(int index);
    void AddTopoBorder();

    void CacheWindowDimension();
    void NormalisedToDeviceCoordinates(double nx, double ny, int& dx, int& dy);
    void DeviceToNormalised(int dx, int dy, double& nx, double& ny);
    void RestrictWithinViewport(int& dx, int& dy);
    void UpdateTopoLineUsingSliceNumber();
    void UpdateTopoLineUsingReferencePoints();
    void CalculateTopoImageHeightDC();
    vtkSmartPointer<vtkImageData> ResizeTopo();
    void PrepareImageMapper(vtkSmartPointer<vtkImageMapper> mapper);

    void RemoveActors();
    void RegisterCallbacks();
    void UpdateActorPosition(int X, int Y);

private:
    int m_sliceNumber = 0;
    int m_minSliceNumber = 0;
    int m_maxSliceNumber = 0;

    // Reference line End points height in topoView on scale of (0.0 to 1.0)
    double m_sliceMinPos = 0.2;
    double m_sliceMaxPos = 0.4;

    int m_topoX_DC = 0;
    int m_topoY_DC = 0;
    int m_topoWidth_DC = 0;
    int m_topoHeight_DC = 0;
    int m_topMargin_DC = 100;
    int m_leftMargin_DC = 10;
    int m_originalPositionX_DC = 0;
    int m_originalPositionY_DC = 0;

    // TopoView position (left,bottom) and (top,right) in device coordinates
    int m_topoMinXDC = 0;
    int m_topoMinYDC = 0;
    int m_topoMaxXDC = 10;
    int m_topoMaxYDC = 10;

    // TopoView position in normalised viewport coordinates
    double m_topoX = 0.0;
    double m_topoY = 0.0;
    double m_topoWidth = 0.20;
    double m_topoHeight = 0.2;
    double m_topMargin = 0.1;
    double m_leftMargin = 0.05;

    // viewport details
    int m_viewportWidthDC = 0;
    int m_viewportHeightDC = 0;
    int m_viewportMaxXDC = 0;
    int m_viewportMaxYDC = 0;
    int m_viewportMargin = 2;
    int m_viewportX = 0;
    int m_viewportY = 0;

    double m_window = 255.0;
    double m_level = 127.5;

    std::string m_topoActiveColor = "white";
    std::string m_borderColor = "gray";
    std::string m_lineColor = "white";
    std::string m_shadowColor = "black";

    vtkProperty2D* m_topoBorderProperty = nullptr;
    vtkPoints* m_linePoints1 = nullptr;
    vtkPoints* m_linePoints2 = nullptr;
    vtkActor2D* m_LineActor1 = nullptr;
    vtkActor2D* m_LineActor2 = nullptr;

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
    bool m_visible = true;
    bool m_resizeTopo = true;
    bool m_maintainAspectRatio = true;

    bool m_interactionEnabled = false;
    std::mutex m_mutex;
    vtkSmartPointer<vtkCallbackCommand> m_leftButtonPress;
    vtkSmartPointer<vtkCallbackCommand> m_leftButtonRelease;
    vtkSmartPointer<vtkCallbackCommand> m_mouseMove;

    int m_groupId = 0;
    QString m_serieID = "";
};

#endif  // MAINWINDOW_H
