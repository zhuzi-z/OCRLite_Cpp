#ifndef SETTING_DIALOG_H
#define SETTING_DIALOG_H

#include <QDialog>
#include <QSettings>

#include "Engine/baidu.h"
// 百度意外错误时，是否需要强制更新？？？

QT_BEGIN_NAMESPACE
namespace Ui { class SettingDialog; }
QT_END_NAMESPACE

class Setting: public QDialog
{
    Q_OBJECT
private:
    static Setting* m_instance;
    QSettings *config;
    BaiduState api_state;
    Setting(QWidget *parent = nullptr);
public:
    static Setting* get_instance(QWidget* parent=nullptr)
    {
        if (m_instance != nullptr) {
            qDebug() << "instance";
            return m_instance;
        }
        else {
            return new Setting(parent);
        }
    }

    void init_states();
    void show_dialog();
    ~Setting();
private slots:
    void on_checkBox_baidu_api_stateChanged(int arg1);

    void on_btn_color_clicked();

    void on_dialog_btn_box_accepted();

    void on_dialog_btn_box_rejected();

private:
    Ui::SettingDialog *ui;

signals:
    void config_changed(void);
};

#endif // SETTING_DIALOG_H
