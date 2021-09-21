#ifndef AREAPICKER_H
#define AREAPICKER_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QScreen>

class AreaPicker : public QWidget
{
    Q_OBJECT
private:
    QRect rect_area;  // 截取的矩形区域
    QPainter* qp;
    QColor background_color;  // 背景色
    float opacity;  // 不透明度
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
public:
    explicit AreaPicker(QWidget *parent = nullptr);
    AreaPicker(QColor color, float opacity, QWidget *parent = nullptr);
    void show_fullscreen_at(QScreen* screen);  // 选取指定的屏幕
    QRect get_area();  // 在截取完，后调用获取区域
    void set_config(QColor color, float opacity);
signals:
    void area_ready(void);  // 截取后释放的信号
};

#endif // AREAPICKER_H
