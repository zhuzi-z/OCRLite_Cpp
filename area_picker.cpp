#include "area_picker.h"
#include <QCursor>
#include <QPoint>
#include <QDesktopWidget>
#include <QDebug>

AreaPicker::AreaPicker(QWidget *parent) : QWidget(parent)
{

}

AreaPicker::AreaPicker(QColor color, float opacity, QWidget *parent) : QWidget(parent)
{
    this->background_color = color;
    this->opacity = opacity;

    this->qp = new QPainter(this);
//    this->qp->begin(this);
//    this->qp->setBrush(this->background_color);
//    this->qp->end();

//    this->setCursor(Qt::CrossCursor);  // 改变游标类型
    this->setCursor(QCursor(QPixmap(":/icons/cursor.svg")));

    this->setWindowFlag(Qt::Window);
    this->setWindowTitle("Area PIcker");
    this->setWindowOpacity(this->opacity);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
}

void AreaPicker::show_fullscreen_at(QScreen* screen)
{
    this->move(screen->geometry().topLeft());
    this->showFullScreen();
    this->rect_area.setRect(0, 0, 0, 0);  // 清除上一次区域
    this->update();
}

QRect AreaPicker::get_area()
{
    return this->rect_area;
}

void AreaPicker::set_config(QColor color, float opacity)
{
    this->background_color = color;
    this->setWindowOpacity(opacity);
}

void AreaPicker::paintEvent(QPaintEvent *event)
{
    qp->begin(this);
    qp->setBrush(this->background_color);
    qp->drawRect(0, 0, this->width(), this->height());  // 全屏绘制掩膜
    qp->eraseRect(this->rect_area);  // 消除选中区域的掩膜
    qp->end();
}

void AreaPicker::mousePressEvent(QMouseEvent *event)
{
//    if (event->buttons() == Qt::LeftButton)
    this->rect_area.setTopLeft(event->pos());  // 获取矩形区域的左上角
}

void AreaPicker::mouseReleaseEvent(QMouseEvent *event)
{
    this->close();  // 先关闭再释放信号，这样可以保证外部处理时区域选择器已经退出
    emit this->area_ready();  // 放开左键是释放信号同时关闭窗口
}

void AreaPicker::mouseMoveEvent(QMouseEvent *event)
{
    this->rect_area.setBottomRight(event->pos());  // 实时保存区域右下角
    this->update();
}

void AreaPicker::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        this->close();
}







