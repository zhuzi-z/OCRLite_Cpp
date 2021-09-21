#ifndef ENGINEFACTORY_H
#define ENGINEFACTORY_H

#include <QList>
#include <QPair>
#include <QString>

#include "base.h"

class EngineFactory
{
private:
    QList<QPair<QString, QString>> engines_info;  // name, icon
public:
    EngineFactory();
    QList<QPair<QString, QString>> get_engines_info();
    BaseEngine* get_engine(QString name, QString lang="");
};

#endif // ENGINEFACTORY_H
