#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QClipboard>
#include <QIcon>
#include <QSystemTrayIcon>

#include "area_picker.h"
#include "Engine/engine_factory.h"
#include "Engine/base.h"
#include "Engine/baidu.h"
#include "setting_dialog.h"
#include "CONFIGURE.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Main_Window; }
QT_END_NAMESPACE

enum class SortOption{
    NOW,
    COPY,
    NEVER
};

class Main_Window : public QMainWindow
{
    Q_OBJECT
private:
    AreaPicker* area_picker;
    EngineFactory* engine_factory;
    BaseEngine* ocr_engine;
    QClipboard* clipboard;
    QSystemTrayIcon *sys_tray;

    QPixmap screenshot;
    QScreen* shot_screen;  // 需要截图的屏幕

    Setting* setting;

    /* 配置 */
    QString engine;
    QString lang;
    SortOption sort_option;
    bool is_append;
    QByteArray w_geometry;  // 记录窗口位置，关闭但未退出时使用

private:
    void init_sys_tray();

public:
    Main_Window(QWidget *parent = nullptr);
    ~Main_Window();

    void get_text();
    void init_stage();

    virtual void closeEvent(QCloseEvent *event);
    void quit_ac();

public slots:
    void screens_changed_ac();  // 监测屏幕变化，由 QApplication 管理

private slots:
    // 自定义槽函数
    void screenshot_ac();
    void config_changed_ac();

    // 控件槽函数
    void on_btn_screenshot_clicked();

    void on_btn_append_clicked();

    void on_btn_copy_clicked();

    void on_btn_setting_clicked();

    void on_box_sort_currentIndexChanged(const QString &arg1);

    void on_box_screen_currentIndexChanged(int index);

    void on_box_lang_currentIndexChanged(const QString &arg1);

    void on_box_engine_currentIndexChanged(const QString &arg1);

private:
    Ui::Main_Window *ui;
};
#endif // MAIN_WINDOW_H
