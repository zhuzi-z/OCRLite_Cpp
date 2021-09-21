#include "main_window.h"
#include "ui_main_window.h"
#include <QClipboard>
#include <QScreen>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include <QSystemTrayIcon>


Main_Window::Main_Window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Main_Window)
{
    area_picker = new AreaPicker(QColor(135, 167, 179), 0.2, nullptr);
    engine_factory = new EngineFactory();
    setting = nullptr;
    ocr_engine = nullptr;  // 指针一定要初始化，否则指向位置将不定(delete时引发异常)
    clipboard = QGuiApplication::clipboard();

    this->sys_tray = new QSystemTrayIcon(this);

    ui->setupUi(this);
    init_sys_tray();

    connect(area_picker, SIGNAL(area_ready()), this, SLOT(screenshot_ac()));

    init_stage();

    ui->image_viewer->close();
}


void Main_Window::init_sys_tray() {

//    sys_tray->setIcon(QIcon(":/icons/OCRLite.svg"));  // QIcon 不可见
    sys_tray->setIcon(QPixmap(":/icons/OCRLite.svg"));
    sys_tray->show();
    QMenu* tray_menu = new QMenu(this);

    connect(sys_tray, &QSystemTrayIcon::activated, this, &Main_Window::showNormal);

    QAction* show_ac = new QAction("主窗口", this);
    connect(show_ac, &QAction::triggered, this, &Main_Window::showNormal);
    tray_menu->addAction(show_ac);

    QAction* quit_ac = new QAction("退出", this);
//    connect(quit_ac, &QAction::triggered, [=](bool){QApplication::quit();});
    connect(quit_ac, &QAction::triggered, this, &Main_Window::quit_ac);
    tray_menu->addAction(quit_ac);

    sys_tray->setContextMenu(tray_menu);
}

void Main_Window::init_stage()
{
    QSettings* config = new QSettings("./config.ini", QSettings::IniFormat);
    config->setIniCodec("UTF-8");
    // Engine
    QList<QPair<QString, QString>> engines_info = engine_factory->get_engines_info();
    ui->box_engine->blockSignals(true);
    ui->box_engine->clear();
    for (int i=0; i<engines_info.size(); i++) {
        ui->box_engine->addItem(QIcon(engines_info[i].second), engines_info[i].first);
    }
    ui->box_engine->setCurrentText(config->value("/MainWindow/engine/", "Tesseract").toString());  // 会触发信号
    ui->box_engine->blockSignals(false);

    ocr_engine = engine_factory->get_engine(ui->box_engine->currentText(), config->value("/MainWindow/lang", "").toString()); // 语言栏需要

    if (ocr_engine == nullptr) {
        qDebug() << "ocr_engine is nullptr: " << ui->box_engine->currentText();
        return;
    }

    // language
    ui->box_lang->blockSignals(true);
    ui->box_lang->clear();
    QList<QString> lang_list = ocr_engine->get_lang_list();
    for (int i=0; i<lang_list.size(); i++) {
        ui->box_lang->addItem(QIcon(":/icons/language.svg"), lang_list[i]);
    }
    ui->box_lang->setCurrentText(config->value("/MainWindow/lang", lang_list[0]).toString());
    ui->box_lang->blockSignals(false);

    // screen
    ui->box_screen->blockSignals(true);
    ui->box_screen->clear();
    auto screens = QApplication::screens();
    for (int i=0; i<screens.size(); i++) {
        ui->box_screen->addItem(QIcon(":/icons/screen.svg"), "屏幕" + QString(i+'1'));
    }
    // shot_screen = screens[0];
    ui->box_screen->setCurrentText(config->value("MainWindow/shot_screen", "屏幕2").toString());
    ui->box_screen->blockSignals(false);

    // sort_option
    ui->box_sort->setCurrentText(config->value("MainWindow/sort_option", "复制时").toString());

    // append button
    is_append = config->value("/MainWindow/is_append", QVariant(false)).toBool();
    ui->btn_append->setChecked(is_append);

    emit ui->box_lang->currentTextChanged(ui->box_lang->currentText()); // 5个信号；
    emit ui->box_screen->currentIndexChanged(ui->box_screen->currentIndex());
    emit ui->box_sort->currentTextChanged(ui->box_sort->currentText());
    emit ui->btn_append->clicked();

    // Font
    QString font_name = config->value("/Setting/UI/font", "").toString();
    QFont font;
    if (font_name != "")
        font = QFont(font_name);
    else
        font = font.defaultFamily();
    font.setPointSize(config->value("/Setting/UI/fontsize", QVariant(12)).toInt());
    this->setFont(font);

    delete config;
}

Main_Window::~Main_Window()
{
    delete ui;
    delete area_picker;
    delete engine_factory;
    delete ocr_engine;
    delete setting;

    delete sys_tray;
}

void Main_Window::closeEvent(QCloseEvent *event)
{
    this->close();
}

void Main_Window::quit_ac()
{
    QSettings* config = new QSettings("./config.ini", QSettings::IniFormat);
    config->setIniCodec("UTF-8");
    config->setValue("/MainWindow/engine", ui->box_engine->currentText());
    config->setValue("/MainWindow/lang", ui->box_lang->currentText());
    config->setValue("MainWindow/shot_screen", ui->box_screen->currentText());
    config->setValue("MainWindow/sort_option", ui->box_sort->currentText());
    config->setValue("/MainWindow/is_append", ui->btn_append->isChecked());

    delete config;

    QApplication::quit();
}


void Main_Window::screenshot_ac()
{
    QRect area = area_picker->get_area();
    QTimer::singleShot(200, [=]() {  // 保证area_picker退出
        screenshot = shot_screen->grabWindow(0, area.x(), area.y(), area.width(), area.height());
        this->ui->image_viewer->setPixmap(screenshot);
        if (this->isHidden())
            this->show();
        get_text();
    });
}

void Main_Window::config_changed_ac()
{
    QSettings* config = new QSettings("./config.ini", QSettings::IniFormat);
    config->setIniCodec("UTF-8");
    // screenshot
    QColor color = config->value("/Setting/Screenshot/color", QVariant(QColor(135, 167, 179))).value<QColor>();
    double opacity = config->value("/Setting/Screenshot/opacity", 0.2).toDouble();
    area_picker->set_config(color, opacity);
    // Font
    QFont font(config->value("/Setting/UI/font", "").toString());
    font.setPointSize(config->value("/Setting/UI/fontsize", QVariant(12)).toInt());
    this->setFont(font);
    // engine
    if (ui->box_engine->currentText() == "百度") {
        delete ocr_engine;
        ocr_engine = engine_factory->get_engine("百度");
    }
}

void Main_Window::screens_changed_ac()
{
    qDebug() << "screen change!";
    int screen_idx = ui->box_screen->currentIndex();

    ui->box_screen->blockSignals(true);
    ui->box_screen->clear();
    auto screens = QApplication::screens();
    for (int i=0; i<screens.size(); i++) {
        ui->box_screen->addItem(QIcon(":/icons/screen.svg"), "屏幕" + QString(i+'1'));
    }
    ui->box_screen->blockSignals(false);

    ui->box_screen->setCurrentIndex(screen_idx);
    emit ui->box_screen->currentIndexChanged(ui->box_screen->currentText());
}

void Main_Window::on_btn_screenshot_clicked()
{
    if (shot_screen == this->screen())
        this->hide();
    area_picker->show_fullscreen_at(shot_screen);
}


void Main_Window::on_btn_append_clicked()
{
    is_append = ui->btn_append->isChecked();
}


void Main_Window::on_btn_copy_clicked()
{
    QString text = ui->text_editor->toPlainText();

    if (sort_option == SortOption::COPY)
        clipboard->setText(ocr_engine->sort_text(text));
    else
        clipboard->setText(text);
}

void Main_Window::on_box_screen_currentIndexChanged(int index)
{
    auto screens = QApplication::screens();
    if (index>=0 && index<screens.size())
        shot_screen = screens[index];
    else {  // 显示器中途插拔，以及 init_stage 中 clear 的调用会影响
        shot_screen = screens[0];
        ui->box_screen->setCurrentIndex(0);
    }
}


void Main_Window::on_box_lang_currentIndexChanged(const QString &arg1)
{

    if (!ocr_engine->set_lang(arg1)) {
        // 失败时会设置为第一个，引擎初始化触发combox clear() 时
        ui->box_lang->setCurrentIndex(0);
    }
}


void Main_Window::on_box_engine_currentIndexChanged(const QString &arg1)
{
    delete ocr_engine;
    ocr_engine = engine_factory->get_engine(arg1, ui->box_lang->currentText());
    auto lang_list = ocr_engine->get_lang_list();

    QString lang = ui->box_lang->currentText();  // 保留上一次引擎的语言状态
    ui->box_lang->blockSignals(true);
    ui->box_lang->clear();
    for (int i=0; i<lang_list.size(); i++) {
        ui->box_lang->addItem(QIcon(":/icons/language.svg"), lang_list[i]);
    }
    ui->box_lang->blockSignals(false);

    if (ocr_engine->set_lang(lang))
        ui->box_lang->setCurrentIndex(lang_list.indexOf(lang));
    else {
        ui->box_lang->setCurrentIndex(0);
    }
}


void Main_Window::on_btn_setting_clicked()
{
    if (setting == nullptr) {
        setting = Setting::get_instance();
        connect(setting, SIGNAL(config_changed()), this, SLOT(config_changed_ac()));
    }

//    Setting setting(this); // this->destory() 自行销毁
    setting->show_dialog();
}

void Main_Window::on_box_sort_currentIndexChanged(const QString &arg1)
{
    if (arg1 == "复制时")
        sort_option = SortOption::COPY;
    else if (arg1 == "即时") {
        sort_option = SortOption::NOW;
        QString text = ocr_engine->sort_text(ui->text_editor->toPlainText());
        ui->text_editor->setText(text);
    }
    else if (arg1 == "永不")
        sort_option = SortOption::NEVER;
    else
        sort_option = SortOption::COPY;
}

void Main_Window::get_text()
{
    QString text = ocr_engine->get_text(screenshot);

    if (sort_option == SortOption::NOW)
        text = ocr_engine->sort_text(text);

    if (is_append)
        ui->text_editor->setText(ui->text_editor->toPlainText() + text);
    else
        ui->text_editor->setText(text);
}

