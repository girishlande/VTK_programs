#ifndef VCTGUI_MouseEventHandler_H_
#define VCTGUI_MouseEventHandler_H_

#include <QObject>

class MouseEventHandler : public QObject
{
  Q_OBJECT

public:
  MouseEventHandler(bool intercept_left_click = true,
                        bool intercept_right_click = true,
                        bool intercept_wheel = true,
                        QObject* parent = nullptr);
  ~MouseEventHandler() {}

  void SetWatched(const QObject* watched);
  void SetInterceptLeftClick(bool intercept);
  void SetInterceptRightClick(bool intercept);
  void SetInterceptWheel(bool intercept);

  bool eventFilter(QObject* watched, QEvent* event) override;

public slots:
  void InterceptAll();
  void InterceptNone();

signals:
  void LeftClickIntercepted();
  void RightClickIntercepted();
  void WheelEventIntercepted();
  void MouseEventIntercepted();
  void LeftDblClickIntercepted();

protected:
  bool intercept_left_click_;
  bool intercept_right_click_;
  bool intercept_wheel_;

  const QObject* watched_;
};

#endif