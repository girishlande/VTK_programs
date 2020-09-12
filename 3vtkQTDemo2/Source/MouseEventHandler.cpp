#include "MouseEventHandler.h"

#include <QEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include <iostream>
using namespace std;

MouseEventHandler::MouseEventHandler(bool     intercept_left_click,
                                             bool     intercept_right_click,
                                             bool     intercept_wheel,
                                             QObject* parent)
    : QObject(parent),
      intercept_left_click_(intercept_left_click),
      intercept_right_click_(intercept_right_click),
      intercept_wheel_(intercept_wheel),
      watched_(nullptr) {}

void MouseEventHandler::SetWatched(const QObject* watched) {
  watched_ = watched;
}

void MouseEventHandler::SetInterceptLeftClick(bool intercept) {
  intercept_left_click_ = intercept;
}

void MouseEventHandler::SetInterceptRightClick(bool intercept) {
  intercept_right_click_ = intercept;
}

void MouseEventHandler::SetInterceptWheel(bool intercept) {
  intercept_wheel_ = intercept;
}

bool MouseEventHandler::eventFilter(QObject* watched, QEvent* event) {
  if (watched_ && watched_ != watched) return false;

  if (QEvent::MouseButtonPress == event->type() ||
      QEvent::MouseButtonDblClick == event->type()) {
    QMouseEvent* e = static_cast<QMouseEvent*>(event);
    if (intercept_left_click_ && Qt::LeftButton == e->button()) {
      emit LeftClickIntercepted();
      emit MouseEventIntercepted();
      return true;
    }
    if (intercept_right_click_ && Qt::RightButton == e->button()) {
      emit RightClickIntercepted();
      emit MouseEventIntercepted();
      return true;
    }
  }
  if (intercept_wheel_ && QEvent::Wheel == event->type()) {
    QWheelEvent* e = static_cast<QWheelEvent*>(event);
    emit         WheelEventIntercepted();
    emit         MouseEventIntercepted();
    return true;
  }

  return false;
}

void MouseEventHandler::InterceptAll() {
  SetInterceptLeftClick(true);
  SetInterceptRightClick(true);
  SetInterceptWheel(true);
}

void MouseEventHandler::InterceptNone() {
  SetInterceptLeftClick(false);
  SetInterceptRightClick(false);
  SetInterceptWheel(false);
}

