#include "latexms.h"

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

#include "pixmap2byteArray.h"

const QString request_url = "https://www.bing.com/cameraexp/api/v1/getlatex";

LatexMS::LatexMS()
{
    this->lang_list = {"Latex"};
    this->lang_dict = {{"Latex", "latex"}};

    this->net_request = new QNetworkRequest();
    this->net_request->setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    this->net_request->setHeader(QNetworkRequest::UserAgentHeader, "Math/1 CFNetwork/1121.2.2 Darwin/19.3.0");
    this->net_request->setUrl(request_url);

    this->net_manager = new QNetworkAccessManager();
    this->post_data = new QJsonObject();

    /*
    data = {
        "data": image,
        "inputForm": "Image",
        "clientInfo": {
            "app": "Math",
            "platform": "ios",
            "configuration": "UnKnown",
            "version": "1.8.0",
            "mkt": "zh-cn"
        },
        "timestamp": int(time.time())
    }
    */
    QJsonObject clientInfo;
    clientInfo.insert("app", "Math");
    clientInfo.insert("platform", "ios");
    clientInfo.insert("configuration", "UnKnown");
    clientInfo.insert("version", "1.8.0");
    clientInfo.insert("mkt", "zh-cn");

//    post_data->insert("data", "");
    post_data->insert("inputForm", "Image");
    post_data->insert("clientInfo", clientInfo);
    post_data->insert("timestamp", QDateTime::currentSecsSinceEpoch());

}

QString LatexMS::get_text(QPixmap pixmap)
{
    QByteArray image_b64 = pixmap_2_byteArray(pixmap).toBase64();

    post_data->insert("data",  QJsonValue::fromVariant(image_b64));
    QJsonDocument doc;
    doc.setObject(*post_data);

//    data.append("data=" + QUrl::toPercentEncoding(image_b64) + "$");
//    data.append("inputForm=Image$");
//    data.append("clientInfo="
//                    "app=Math$"
//                    "platform=ios$"
//                    "configuration=UnKnown$"
//                    "version=1.8.0$"
//                    "mkt=zh-cn$");
//    data.append("timestamp=");
//    data.append(QDateTime::currentSecsSinceEpoch());

    QByteArray data;
    data.append("data=");
    data.append(QUrl::toPercentEncoding(image_b64));
    data.append("&");
    data.append("inputForm=Image");

    QEventLoop event_loop;
    QObject::connect(this->net_manager, SIGNAL(finished(QNetworkReply*)), &event_loop, SLOT(quit()));
    QNetworkReply *reply = net_manager->post(QNetworkRequest(*this->net_request), doc.toJson(QJsonDocument::Compact));
    event_loop.exec();

    QString text = "";    

    switch (reply->error()) {
    case QNetworkReply::NoError: {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject response_json = doc.object();
        text = response_json.value("latex").toString();
        break;
    }
    case QNetworkReply::HostNotFoundError: {
         QMessageBox(QMessageBox::Warning, "Warning", "网络错误，请检查网络或使用本地引擎").exec();
         break;
    }
    default: {
        text = reply->errorString();
    }
    }
    return text;
}

QString LatexMS::sort_text(QString text)
{
    return text;
}

LatexMS::~LatexMS()
{
    delete net_request;
    delete net_manager;
    delete post_data;
}
