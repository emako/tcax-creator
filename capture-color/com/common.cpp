#include "common.h"

QByteArray Common::getResource(const QString &a_filename)
{
    QResource resource(a_filename);

    return QByteArray(reinterpret_cast<const char *>(resource.data()), static_cast<int>(resource.size()));
}

QString Common::fromResource(const QString &a_filename)
{
    return QString::fromUtf8(getResource(a_filename));
}
