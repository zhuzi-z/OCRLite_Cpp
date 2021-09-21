#ifndef LATEXMS_H
#define LATEXMS_H

#include "base.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QJsonObject>

class LatexMS : public BaseEngine
{
private:
    QNetworkRequest* net_request;
    QNetworkAccessManager* net_manager;
    QJsonObject* post_data;
public:
    virtual QString get_text(QPixmap pixmap);
    virtual QString sort_text(QString text);
    LatexMS();
    virtual ~LatexMS();
};

#endif // LATEXMS_H
