#ifndef __QT_STL_H__
#define __QT_STL_H__
#include <string>
#include <QString>
#include <Common/utf_gbk.h>

namespace qtstl {
    inline std::string ToStr(const QString& qstr){
        return qstr.toStdString();
    }

    inline QString ToQStr(const std::string& str){
        return QString(str.c_str());
    }

    inline std::string ToGbkStr(const QString& qstr){
        return utf_gbk::toGbk(qstr.toStdString());
    }

    inline QString ToUtf8QStr(const std::string& str){
        return QString(utf_gbk::toUtf8(str).c_str());
    }
}


#endif //!__QT_STL_H__
