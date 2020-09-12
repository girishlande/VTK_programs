#ifndef VCTGUI_GPLView_H_
#define VCTGUI_GPLView_H_

// QT
class QColor;
class QFrame;
class QPushButton;
class QScrollbar;
class QSlider;
#include <QWidget>

// vtk and QT
class QVTKOpenGLNativeWidget;

// vtk
class vtkCornerAnnotation;
class vtkImageData;
class vtkObject;
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

/*! \class GPLView
    \brief A base wrapper for QVTKOpenGLNativeWidget to provide styling
           and functions for use in Metro

    GPLView holds a QVTKOpenGLNativeWidget to provide viewing functionality
    and also adds the slider and max/min corner button for those views and
    also provides a border and focus/hover functionality
*/


// Forward declarations
class MetroScrollBarH;
class MouseEventHandler;
class MultistateHvrBtn;

class GPLView : public QWidget {
    Q_OBJECT

public:
    GPLView(QWidget* parent = nullptr);
    ~GPLView() override;
    void SetViewName(const std::string& name) { view_name_ = name; }

    bool IsInitialized() const { return initialized_; }

    virtual bool IsAlreadyLoaded(
        const vtkSmartPointer<vtkImageData>& image) { return false; }

    enum Orientation {
        X_Plus,
        Y_Plus,
        Z_Plus,
        X_Minus,
        Y_Minus,
        Z_Minus,
    };

    QColor ColorFromOrientation(Orientation o)
    {
        switch (o) {
        case X_Minus:
            return Qt::yellow;
        case Y_Minus:
            return QColor(255, 165, 0);
        case Z_Minus:
            return Qt::red;
        case X_Plus:
            return Qt::green;
        case Y_Plus:
            return Qt::blue;
        case Z_Plus:
        default:
            return Qt::black;
        }
    }

    void SetSliderRange(int min, int max);

    void SetBackgroundColor(QColor color = QColor(0, 0, 0));

    void SetBorderColor(QColor color = QColor(150, 150, 150));
    void SetFocusColor(QColor color = QColor(238, 117, 24));
    void SetHoverColor(QColor color = QColor(255, 255, 255));
    virtual double GetWindow() const { return 65535.0; }
    virtual double GetLevel() const { return 0.0; }
    virtual double GetZoom() const { return 1.0; }
    virtual void SetDefaultWindow(double w);
    virtual void SetDefaultLevel(double l);
    vtkRenderWindow* GetRenderWindow();

        signals:
    void ViewClicked();

public slots:
    void DoubleClickProcess();
    void ReRender();
    void SetBorderOn(bool b = true);
    virtual void SetIsFocused(bool b = false);
    void TurnBorderOn();
    void TurnBorderOff();
    void ShowMaxMinButton(bool b = false);
    virtual void ShowSlider(bool b = false);
    virtual void SetSliderValue(int value);

    // these will make it easier to broadcast these things to derived image
    // views even if they don't have meaning in some views
    virtual void SetImageData(vtkSmartPointer<vtkImageData> img);
    virtual void ResetWindowLevelToDefault();

    // compass slots
    virtual void SetCompassTextTop(const QString& text);
    virtual void SetCompassTextBottom(const QString& text);
    virtual void SetCompassTextLeft(const QString& text);
    virtual void SetCompassTextRight(const QString& text);
    virtual void ClearCompassTextTop();
    virtual void ClearCompassTextBottom();
    virtual void ClearCompassTextLeft();
    virtual void ClearCompassTextRight();
    virtual void ClearCompassTextAll();


protected:
    GPLView() = delete;

    QObject* event_target_;
    QObject* FindParentByName(QObject* object, const QString& name);
    void PostEventToTarget(QEvent* event);

    void enterEvent(QEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;

    std::string view_name_;

    virtual void InitializePipeline();
    bool initialized_ = false;

    MouseEventHandler* mouse_interceptor_;
    QVTKOpenGLNativeWidget* vtk_widget_;

    QSlider* slider_;
    QPushButton* button_maxmin_;

    bool is_bordered_;
    bool is_focused_;
    bool is_hovered_;
    bool is_maximized_;
    bool show_maxmin_button_;
    bool show_slider_;

    double default_window_ = 65535.0;
    double default_level_ = 0.0;

    QColor border_color_;
    QColor hover_color_;
    QColor focus_color_;
    QColor background_color_;
    QFrame* frame_;
    QPushButton* label_;
    QPushButton* guide_;
    QPushButton* play_pause_;

    vtkSmartPointer<vtkCornerAnnotation> compass_;
    bool compass_on_;
    bool is_playing_;

    double parallel_scale_factor_;
    static double GuessScaleFactor(vtkSmartPointer<vtkImageData> img);
};

#endif
