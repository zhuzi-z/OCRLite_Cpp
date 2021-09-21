#include <QByteArray>
#include <QPixmap>
#include <QBuffer>

#include "pixmap2byteArray.h"

QByteArray pixmap_2_byteArray(const QPixmap& pixmap)
{
    QByteArray byte_array;
    QBuffer buffer(&byte_array);
    if (buffer.open(QIODevice::WriteOnly))
    {
        pixmap.save(&buffer, "png");
    }
    return byte_array;
}
