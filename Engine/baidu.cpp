#include "baidu.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>
#include <QJsonArray>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>

#include "pixmap2byteArray.h"


static const QString request_url = "https://aip.baidubce.com/rest/2.0/ocr/v1/general_basic";


Baidu::Baidu(QString lang)
{
    this->lang_list = {"中文", "英文", "日文"};
    this->lang_dict = {{"中文","CHN_ENG"}, {"英文","ENG"}, {"日文","JAP"}};

    if (lang_dict.contains(lang)) {
        this->lang = lang_dict[lang];
    } else {
       this->lang = lang_dict[lang_list[0]];
    }

    this->net_request = new QNetworkRequest();
    this->net_manager = new QNetworkAccessManager();

    QSettings* config = new QSettings(config_path, QSettings::IniFormat);
    config->setIniCodec("UTF-8");

    state = config->value("/Setting/Baidu/api_state", QVariant(1)).toInt();  // 缺省一定请求公用key
    if (state==(int)BaiduState::CHANGE2PUBLIC || state==(int)BaiduState::KEEP_PUBLIC) {
        ak = "your key";
        sk = "your key";
        if (state==(int)BaiduState::CHANGE2PUBLIC)
            access_token = "";
    } else if (state == (int)BaiduState::CHANGE2PRIVATE || state==(int)BaiduState::KEEP_PRIVATE) {
        ak = config->value("/Setting/Baidu/ak", "").toString();
        sk = config->value("/Setting/Baidu/sk", "").toString();
        if (state==(int)BaiduState::CHANGE2PRIVATE)
            access_token = "";
    }
    if (state==(int)BaiduState::KEEP_PUBLIC || state==(int)BaiduState::KEEP_PRIVATE){
        this->access_token = config->value("/Engine/Baidu/access_token", "").toString();  // 读取上一次的 token
        this->token_expires_in = config->value("/Engine/Baidu/token_expires_in", 0).toInt();
    }

    delete  config;

    QTimer::singleShot(250, [=](){  // 延迟，第一次打开，让主窗口先出现
       get_access_token(ak, sk);
    });
}

void Baidu::get_access_token(QString ak, QString sk)
{
    if (QDateTime::currentSecsSinceEpoch() > this->token_expires_in) // 过期
        this->access_token = "";

    if (this->access_token != "")
        return;

    QString token_host = QString("https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials"
        "&client_id=%1&client_secret=%2").arg(ak, sk);

    this->net_request->setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=UTF-8");
    this->net_request->setUrl(QUrl(token_host));

    QEventLoop event_loop;

    QObject::connect(this->net_manager, SIGNAL(finished(QNetworkReply*)), &event_loop, SLOT(quit()));

    QNetworkReply *reply = net_manager->post(QNetworkRequest(*this->net_request),"");
    event_loop.exec();

    switch(reply->error()) {

    case QNetworkReply::NoError : {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject response_json = doc.object();
        this->access_token = response_json.value("access_token").toString();
        this->token_expires_in = QDateTime::currentSecsSinceEpoch() + response_json.value("expires_in").toInt();
        return;
    }
    case QNetworkReply::HostNotFoundError: {
        QMessageBox(QMessageBox::Warning, "Warning", "初始化失败！！！\n网络错误，请检查网络或使用本地引擎").exec();
    }
    default: {
    }

    }
    this->access_token = "";
    return ;
}

QString Baidu::get_text(QPixmap pixmap)
{
    if (access_token == "") {
        get_access_token(ak, sk);
        return "请求令牌（access_token）中，请重试！！！";
    }
    this->net_request->setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    this->net_request->setUrl(request_url);

    QByteArray image_b64 = pixmap_2_byteArray(pixmap).toBase64();

    QByteArray data;
    data.append("image=");
    data.append(QUrl::toPercentEncoding(image_b64));
    data.append("&");
    data.append("language_type=" + this->lang.toUtf8() + "&");
    data.append("access_token=" + this->access_token.toUtf8());

    QEventLoop event_loop;
    QObject::connect(this->net_manager, SIGNAL(finished(QNetworkReply*)), &event_loop, SLOT(quit()));
    QNetworkReply *reply = net_manager->post(QNetworkRequest(*this->net_request), data);
    event_loop.exec();

    QString text;

    switch (reply->error()) {
    case QNetworkReply::NoError: {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject response_json = doc.object();

        if (response_json.contains("words_result")) {
            QJsonArray words_result = response_json.value("words_result").toArray();
            QJsonArray::Iterator it;
            for (it=words_result.begin(); it!=words_result.end(); it++)
                text += (*it).toObject().value("words").toString() + "\n";
        }
        if (text != "")
            return text;
        else {
            // {    "error_code": 110,    "error_msg": "Access token invalid or no longer valid"}
            if (response_json.contains("error_code")) {
                int error_code = response_json.value("error_code").toInt();
                if (error_code == 110) {
                    access_token = "";
                    return "请求文本：出现错误，令牌（access_token）无效，"
"                           已清空令牌，请重试，"
"                           返回结果如下：\n" + doc.toJson();
                }
                get_access_token(ak, sk);
            }

        }
        return "";
    }
    case QNetworkReply::HostNotFoundError: {
         return reply->readAll().toStdString().c_str();
         QMessageBox(QMessageBox::Warning, "Warning", "请求失败！！！\n网络错误，请检查网络或使用本地引擎").exec();
    }
    default: {

    }
    }

    return "";
}

QString Baidu::sort_text(QString text)
{
    int idx = 0;
    while (idx < text.size()) {
        if (text[idx] == '\n') {
            text.remove(idx, 1);
            idx--;
        }
        idx++;
    }
    return text;
}

Baidu::~Baidu()
{
    QSettings* config = new QSettings(config_path, QSettings::IniFormat);
    config->setIniCodec("UTF-8");
    // acceess_token == "", 下次重新请求
    if (state==(int)BaiduState::CHANGE2PUBLIC && this->access_token!="")
        config->setValue("/Setting/Baidu/api_state", (int)BaiduState::KEEP_PUBLIC);
    if (state==(int)BaiduState::CHANGE2PRIVATE && this->access_token!="")
        config->setValue("/Setting/Baidu/api_state", (int)BaiduState::KEEP_PRIVATE);

    config->setValue("/Engine/Baidu/access_token", this->access_token);
    config->setValue("/Engine/Baidu/token_expires_in", this->token_expires_in);
    delete config;
    delete this->net_request;
    delete  this->net_manager;
}
