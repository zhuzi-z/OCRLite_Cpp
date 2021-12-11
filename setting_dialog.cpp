#include "setting_dialog.h"
#include "ui_setting_dialog.h"

#include <QDialog>
#include <QColorDialog>
#include <QDebug>
#include <QFont>

Setting* Setting::m_instance = nullptr;

Setting::Setting(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::SettingDialog())
{
    ui->setupUi(this);
    config = new QSettings(config_path, QSettings::IniFormat);
    config->setIniCodec("UTF-8");
}

void Setting::init_states()
{

    BaiduState api_states[] = {
        BaiduState::KEEP_PUBLIC,
        BaiduState::CHANGE2PUBLIC,
        BaiduState::KEEP_PRIVATE,
        BaiduState::CHANGE2PRIVATE
    };
    // Baidu
    api_state = api_states[config->value("/Setting/Baidu/api_state", QVariant(1)).toInt()];
    if (api_state == BaiduState::KEEP_PRIVATE || api_state == BaiduState::CHANGE2PRIVATE)
        ui->checkBox_baidu_api->setCheckState(Qt::CheckState::Checked);
    else
        ui->checkBox_baidu_api->setCheckState(Qt::CheckState::Unchecked);
    // 初始化时手动触发状态改变 -> 与ak，sk输入框相关
    emit ui->checkBox_baidu_api->stateChanged(ui->checkBox_baidu_api->checkState());
    ui->ledit_ak->setText(config->value("/Setting/Baidu/ak", "").toString());
    ui->ledit_sk->setText(config->value("/Setting/Baidu/sk", "").toString());
    // screenshot
    QColor color = config->value("/Setting/Screenshot/color", QVariant(QColor(135, 167, 179))).value<QColor>();
    auto pal = ui->btn_color->palette();
    pal.setColor(QPalette::Button, color);
    ui->btn_color->setPalette(pal);
    ui->opacity_box->setValue(config->value("/Setting/Screenshot/opacity", 0.2).toDouble());
    // Font
    QString font_name = config->value("/Setting/UI/font", "").toString();
    if (font_name == "")
        ui->fontComboBox->setCurrentFont(QFont().defaultFamily());
    else
        ui->fontComboBox->setCurrentFont(QFont(font_name));
    ui->spinBox->setValue(config->value("/Setting/UI/fontsize", QVariant(12)).toInt());
}

void Setting::show_dialog()
{
    init_states();
    this->show();
}

Setting::~Setting()
{
    delete config;
    delete ui;
}


void Setting::on_checkBox_baidu_api_stateChanged(int arg1)
{
    if (arg1 == 0) {
        ui->ledit_ak->setEnabled(false);
        ui->ledit_sk->setEnabled(false);
        if (api_state==BaiduState::CHANGE2PRIVATE || api_state==BaiduState::KEEP_PRIVATE)
            api_state = BaiduState::CHANGE2PUBLIC;
    } else if (arg1 == 2) {
        ui->ledit_ak->setEnabled(true);
        ui->ledit_sk->setEnabled(true);
        if (api_state==BaiduState::CHANGE2PUBLIC || api_state==BaiduState::KEEP_PUBLIC)
            api_state = BaiduState::CHANGE2PRIVATE;
    }
}

void Setting::on_btn_color_clicked()
{

    QColorDialog *color_dialog = new QColorDialog(this);
    color_dialog->show();

    QColor color = color_dialog->getColor();

    if (color.isValid()) {
        auto pal = ui->btn_color->palette();
        pal.setColor(QPalette::Button, color);
        ui->btn_color->setPalette(pal);
    }

    delete color_dialog;
}


void Setting::on_dialog_btn_box_accepted()
{
    config->setValue("/Setting/Baidu/api_state", (int)api_state);
    config->setValue("/Setting/Baidu/ak", ui->ledit_ak->text());
    config->setValue("/Setting/Baidu/sk", ui->ledit_sk->text());
    QColor color = ui->btn_color->palette().color(QPalette::Button).toRgb();
    config->setValue("/Setting/Screenshot/color", color);
    config->setValue("/Setting/Screenshot/opacity", ui->opacity_box->value());
    config->setValue("/Setting/UI/font", ui->fontComboBox->currentText());
    config->setValue("/Setting/UI/fontsize", ui->spinBox->value());
    emit config_changed();
    this->close();
}


void Setting::on_dialog_btn_box_rejected()
{
    this->close();
}


