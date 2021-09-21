#include "tesseract_api.h"
#include <tesseract/publictypes.h>
#include <QDir>

Tesseract::Tesseract(QString lang)
{
    TESSDATA_PREFIX = "/usr/share/tessdata";
    available_lang();

//    lang_list = {"中文", "英文"};
//    lang_dict = {{"中文", "chi_sim"}, {"英文", "eng"}};

    api = new tesseract::TessBaseAPI();
//    this->lang = lang_dict[lang];  // 不存在时会添加新内容
//    char* configs = QString("preserve_interword_spaces=1").toLatin1().data();
;
    if (lang_dict.contains(lang)) {
        this->lang = lang_dict[lang];
        api->Init(TESSDATA_PREFIX.toLatin1(), this->lang.toLatin1().data());
//        api->Init("/usr/share/tessdata", this->lang.toLatin1().data(), tesseract::OEM_DEFAULT, &configs, 1, nullptr, nullptr, false);
    } else {
        this->lang = lang_dict[lang_list[0]];
        api->Init(TESSDATA_PREFIX.toLatin1(), this->lang.toLatin1().data());
//        api->Init("/usr/share/tessdata", this->lang.toLatin1().data(), tesseract::OEM_DEFAULT, &configs, 1, nullptr, nullptr, false);
    }
}

Tesseract::~Tesseract()
{
    delete api;
}

void Tesseract::available_lang()
{
    QDir tessdata_path(TESSDATA_PREFIX);
    QStringList traineddatas = tessdata_path.entryList(QStringList({"*.traineddata"}), QDir::Files);
    for (auto traineddata: traineddatas) {
        auto lang = traineddata.split(".")[0];
        this->lang_list.append(lang);
        this->lang_dict[lang] = lang;
    }
}

bool Tesseract::set_lang(QString lang) {
    if (lang_dict.contains(lang)) {
        this->lang = lang_dict[lang];
        api->Init(TESSDATA_PREFIX.toLatin1(),this->lang.toLatin1().data());
        return true;
    } else {
       this->lang = lang_dict[lang_list[0]];
       api->Init(TESSDATA_PREFIX.toLatin1(),this->lang.toLatin1().data());
       return false;
    }
}

QString Tesseract::get_text(QPixmap pixmap){

    Pix *image = qPixmap2Pix(pixmap);
    api->SetImage(image);

    QString text = api->GetUTF8Text();

    pixDestroy(&image);

    return text;
}

QString Tesseract::sort_text(QString text)
{
    if (this->lang == "eng")
        return text.replace('\n', ' ');
    else if (this->lang == "chi_sim") {
        text.replace(' ', "");
        text.replace('\n', "");
        return text;
    }
    return "";
}

Pix* Tesseract::qPixmap2Pix(QPixmap& pixmap) {
  PIX * pixs;
  l_uint32 *lines;

  QImage qImage = pixmap.toImage();
  qImage = qImage.rgbSwapped();
  int width = qImage.width();
  int height = qImage.height();
  int depth = qImage.depth();
  int wpl = qImage.bytesPerLine() / 4;

  pixs = pixCreate(width, height, depth);
  pixSetWpl(pixs, wpl);
  pixSetColormap(pixs, NULL);
  l_uint32 *datas = pixs->data;

  for (int y = 0; y < height; y++) {
    lines = datas + y * wpl;
    QByteArray a((const char*)qImage.scanLine(y), qImage.bytesPerLine());
    for (int j = 0; j < a.size(); j++) {
      *((l_uint8 *)lines + j) = a[j];
    }
  }
  return pixEndianByteSwapNew(pixs);
}
