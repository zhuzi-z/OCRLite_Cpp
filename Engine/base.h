#ifndef BASE_H
#define BASE_H

#include <QString>
#include <QList>
#include <QMap>
#include <QPixmap>
#include <QDebug>

#include "pixmap2byteArray.h"

class BaseEngine
{
protected:
    QString lang;
    QList<QString> lang_list;
    QMap<QString, QString> lang_dict;
public:
    BaseEngine();
    QList<QString> get_lang_list(void);
    virtual bool set_lang(QString lang);
    virtual QString get_text(QPixmap pixmap) = 0;
    virtual QString sort_text(QString text) = 0;
    virtual ~BaseEngine(){};
};

#endif // BASE_H
