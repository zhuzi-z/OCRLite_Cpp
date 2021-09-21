#include "base.h"

BaseEngine::BaseEngine()
{

}

QList<QString> BaseEngine::get_lang_list()
{
    return lang_list;
}

bool BaseEngine::set_lang(QString lang)
{
    if (lang_dict.contains(lang)) {
        this->lang = lang_dict[lang];
        return true;
    } else {
       this->lang = lang_dict[lang_list[0]];
       return false;
    }
}
