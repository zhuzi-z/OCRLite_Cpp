#include "engine_factory.h"

#include "baidu.h"
#include "latexms.h"
#include "tesseract_api.h"

EngineFactory::EngineFactory()
{
    engines_info.append(QPair<QString, QString>("百度", ":/icons/baidu.svg"));
    engines_info.append(QPair<QString, QString>("Tesseract", ":/icons/google.svg"));
    engines_info.append(QPair<QString, QString>("微软数学", ":/icons/tex.svg"));
}

QList<QPair<QString, QString>> EngineFactory::get_engines_info()
{
    return engines_info;
}

BaseEngine* EngineFactory::get_engine(QString name, QString lang)
{

    if (name == "百度")
        return new Baidu(lang);
    else if (name == "微软数学")
        return new LatexMS();
    else if (name == "Tesseract")
        return new Tesseract(lang);

    return nullptr;
}


