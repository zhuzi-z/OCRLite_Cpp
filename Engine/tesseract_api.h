#ifndef TESSERACT_API_H
#define TESSERACT_API_H

#include "base.h"

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

class Tesseract : public BaseEngine
{
private:
    tesseract::TessBaseAPI* api;
    QString TESSDATA_PREFIX;
    static Pix* qPixmap2Pix(QPixmap& pixmap);
public:
    Tesseract(QString lang);
    virtual ~Tesseract();
    void available_lang();
    virtual bool set_lang(QString lang);
    virtual QString get_text(QPixmap pixmap);
    virtual QString sort_text(QString text);
};

#endif // TESSERACT_API_H
