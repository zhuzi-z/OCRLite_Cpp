#ifndef BAIDU_H
#define BAIDU_H

#include "base.h"
#include "CONFIGURE.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>

enum class BaiduState {
    KEEP_PUBLIC = 0,
    CHANGE2PUBLIC = 1,
    KEEP_PRIVATE = 2,
    CHANGE2PRIVATE = 3
};

class Baidu : public BaseEngine
{
private:
    QNetworkRequest* net_request;
    QNetworkAccessManager* net_manager;
    int state;
    QString ak, sk;
    QString access_token;
    qint64 token_expires_in;
    void get_access_token(QString ak, QString sk);
public:
//    Baidu();
    Baidu(QString lang="中文");
    virtual QString get_text(QPixmap pixmap);
    virtual QString sort_text(QString text);
    virtual ~Baidu();
public slots:
    void api_forced_changed();
};

#endif // BAIDU_H
