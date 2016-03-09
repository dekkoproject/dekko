#ifndef TRGETTEXT
#define TRGETTEXT
#include <libintl.h>
#include <QString>

#define trGettext(text) QString::fromUtf8(gettext(text))

#endif // TRGETTEXT

