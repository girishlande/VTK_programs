#include "GPLView.h"
#include "MouseEventHandler.h"

// QT
#include <QColor>
#include <QCoreApplication>
#include <QFrame>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QSlider>

// vtk
#include <QVTKOpenGLNativeWidget.h>
#include <vtkCornerAnnotation.h>
#include <vtkDataArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkPointData.h>
#include <vtkRenderWindow.h>
#include <vtkTextProperty.h>

GPLView::GPLView(QWidget* parent)
  : QWidget(parent),
    is_bordered_(true),
    is_focused_(false),
    is_hovered_(false),
    is_maximized_(false),
    show_maxmin_button_(true),
    show_slider_(true),
    compass_on_(true),
    is_playing_(false),
    border_color_(150, 150, 150), // from metro spec
    hover_color_(255, 255, 255),  // from metro spec
    focus_color_(238, 117, 24),   // from metro spec
    background_color_(0, 0, 0),   // black
    initialized_(false),
    event_target_(nullptr)
{
  setMouseTracking(true);
  this->setStyleSheet("background-color: black;");

  vtk_widget_ = new QVTKOpenGLNativeWidget(this);
  vtk_widget_->SetRenderWindow(
    vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New());

  mouse_interceptor_ = new MouseEventHandler(true, true, true, this);
  mouse_interceptor_->SetWatched(vtk_widget_);
  vtk_widget_->installEventFilter(mouse_interceptor_);

  frame_ = new QFrame(this);
  frame_->setStyleSheet("background-color: transparent; border: 1px solid #969696;");
  frame_->resize(this->width(), this->height());
  frame_->setAttribute(Qt::WA_TransparentForMouseEvents);

  button_maxmin_ = new QPushButton(this);
  button_maxmin_->resize(50,50);
  button_maxmin_->move(this->width()- 100, 1);

  // slider
  slider_ = new QSlider(this);
  
  // compass
  compass_ = vtkSmartPointer<vtkCornerAnnotation>::New();
  compass_->GetTextProperty()->SetFontSize(12);

  // connect slider change to slice change
  connect(slider_, &QAbstractSlider::valueChanged,
          this, &GPLView::SetSliderValue);

  // connect interceptor to get events and set focus
  connect(mouse_interceptor_, &MouseEventHandler::LeftClickIntercepted,
          this, &GPLView::ViewClicked);

  connect(mouse_interceptor_, &MouseEventHandler::LeftDblClickIntercepted,
          this, &GPLView::DoubleClickProcess);
  

  ShowMaxMinButton(show_maxmin_button_);
  ShowSlider(show_slider_);

  label_ = new QPushButton(this);
  label_->setText(tr(""));
  label_->move(1, height() - label_->height());
  label_->setStyleSheet("background-color: transparent; color: white; \
                         font: 20px Arial; text-align: left;");
  label_->setCursor(Qt::PointingHandCursor);

  // cursor to cross when over view
  setCursor(Qt::CrossCursor);

  // size this according to parent, else give some throwaway defaults
  if (parent) {
    resize(parent->width()/2, parent->height()/2);
  } else {
    resize(512, 512);
  }

}

GPLView::~GPLView()
{
}

void GPLView::SetSliderRange(int min, int max)
{
  slider_->setRange(min, max);
}

void GPLView::SetBackgroundColor(QColor color)
{
  background_color_ = color;
  repaint();
}

void GPLView::SetBorderColor(QColor color)
{
  border_color_ = color;
  repaint();
}

void GPLView::SetFocusColor(QColor color)
{
  focus_color_ = color;
  repaint();
}

void GPLView::SetHoverColor(QColor color)
{
  hover_color_ = color;
  repaint();
}

void GPLView::ReRender()
{
  vtk_widget_->GetRenderWindow()->Render();
}

void GPLView::SetBorderOn(bool b)
{
  is_bordered_ = b;
  repaint();
}

void GPLView::SetIsFocused(bool b)
{
  is_focused_ = b;
  if (b) {
    frame_->setStyleSheet("background-color: transparent;\
                           border: 1px solid #EE7518;");
    mouse_interceptor_->InterceptNone();
  } else {
    frame_->setStyleSheet("background-color: transparent;\
                           border: 1px solid #969696;");
    mouse_interceptor_->InterceptAll();
  }
  ShowMaxMinButton(b);
  ShowSlider(b);
  repaint();
}

void GPLView::TurnBorderOn()
{
  is_bordered_ = true;
  repaint();
}

void GPLView::TurnBorderOff()
{
  is_bordered_ = false;
  repaint();
}

void GPLView::ShowMaxMinButton(bool b)
{
  show_maxmin_button_ = b;
  if (show_maxmin_button_) {
    button_maxmin_->show();
  } else {
    button_maxmin_->hide();
  }
}


void GPLView::ShowSlider(bool b) {
  show_slider_ = b;
  if (show_slider_) {
    slider_->show();
  } else {
    slider_->hide();
  }
}

void GPLView::SetSliderValue(int value) {
  slider_->setValue(value);
}

void GPLView::SetImageData(vtkSmartPointer<vtkImageData> img) {

  // Set the data scalar range based on CT Hounsfield units.
  // This helps improve first frame performance
  if (img == nullptr) return;

  vtkDataArray* da = img->GetPointData()->GetScalars();
  vtkInformation* info = da->GetInformation();
  vtkInformationDoubleVectorKey* rkey = vtkDataArray::COMPONENT_RANGE();

  vtkInformationVector* infoVec = vtkInformationVector::New();
  vtkInformationVector* infoVec1 = vtkInformationVector::New();
  info->Set(vtkDataArray::PER_FINITE_COMPONENT(), infoVec);
  info->Set(vtkDataArray::PER_COMPONENT(), infoVec1);
  infoVec->SetNumberOfInformationObjects(da->GetNumberOfComponents());
  infoVec1->SetNumberOfInformationObjects(da->GetNumberOfComponents());
  infoVec->FastDelete();
  infoVec1->FastDelete();
  
  if (!initialized_) InitializePipeline();
}

void GPLView::SetCompassTextTop(const QString& text) {
  compass_->SetText(vtkCornerAnnotation::UpperEdge,
                    text.toStdString().c_str());
}

void GPLView::SetCompassTextBottom(const QString& text) {
  compass_->SetText(vtkCornerAnnotation::LowerEdge,
                    text.toStdString().c_str());
}

void GPLView::SetCompassTextLeft(const QString& text) {
  compass_->SetText(vtkCornerAnnotation::LeftEdge,
                    text.toStdString().c_str());
  
}

void GPLView::SetCompassTextRight(const QString& text) {
  compass_->SetText(vtkCornerAnnotation::RightEdge,
                    text.toStdString().c_str());
}

void GPLView::ClearCompassTextTop() {
  compass_->SetText(vtkCornerAnnotation::UpperEdge, "");
}

void GPLView::ClearCompassTextBottom() {
  compass_->SetText(vtkCornerAnnotation::LowerEdge, "");
}

void GPLView::ClearCompassTextLeft() {
  compass_->SetText(vtkCornerAnnotation::LeftEdge, "");
}

void GPLView::ClearCompassTextRight() {
  compass_->SetText(vtkCornerAnnotation::RightEdge, "");
}

void GPLView::ClearCompassTextAll() {
  compass_->ClearAllTexts();
}


QObject* GPLView::FindParentByName(QObject* object, const QString& name) {
  object = object->parent();
  if (object && object->objectName() != name) {
    return FindParentByName(object, name);
  }
  return object;
}

void GPLView::PostEventToTarget(QEvent* event) {
  if (!event_target_) return;
  QCoreApplication::postEvent(event_target_, event);
}

void GPLView::enterEvent(QEvent* event)
{
  is_hovered_ = true;
  if (!is_focused_) {
    frame_->setStyleSheet("background-color: transparent; \
                           border: 1px solid white;");
  }

  QWidget::enterEvent(event);
}

void GPLView::hideEvent(QHideEvent* event) {
  QWidget::hideEvent(event);
}

void GPLView::leaveEvent(QEvent* event)
{
  is_hovered_ = false;
  if (!is_focused_) {
    frame_->setStyleSheet("background-color: transparent; \
                           border: 1px solid #969696;");
  }

  QWidget::leaveEvent(event);
}

void GPLView::mousePressEvent(QMouseEvent* event) {
  if (Qt::LeftButton == event->button() && !is_focused_) {
    emit ViewClicked();
  }
}

void GPLView::paintEvent(QPaintEvent* event)
{
  // pass thru for now
  QWidget::paintEvent(event);
}

void GPLView::resizeEvent(QResizeEvent* event) {
  QWidget::resizeEvent(event);
  vtk_widget_->resize(this->width(), this->height());
  frame_->resize(this->width(), this->height());

  // move button to new spot
  button_maxmin_->move(this->width() - button_maxmin_->width(), 1);

  // also move slider
  slider_->resize(static_cast<int>(0.85*this->width()), slider_->height());
  slider_->move(this->width()/2 - slider_->width()/2,
                this->height() - 25);
                
  // also move label and guide
  label_->move(1, height() - label_->height() - 1);
}

void GPLView::InitializePipeline() {
  initialized_ = true;
}

double GPLView::GuessScaleFactor(vtkSmartPointer<vtkImageData> img) {
  int    dims[3];
  double spacing[3];
  img->GetDimensions(dims);
  img->GetSpacing(spacing);

  const double height = dims[1]*spacing[1];
  return height/2.;
}

void GPLView::SetDefaultWindow(double value) {
  this->default_window_ = value;
}

void GPLView::SetDefaultLevel(double value) {
  this->default_level_ = value;
}

vtkRenderWindow* GPLView::GetRenderWindow()
{
    return vtk_widget_->GetRenderWindow();
}

void GPLView::ResetWindowLevelToDefault() {
}

void GPLView::DoubleClickProcess() {
    cout << "Girish:: Double click process"; 
}

