#ifndef MAIL_QMLENUMS
#define MAIL_QMLENUMS
#include <QObject>
#include <QQmlEnumClassHelper.h>

//QML_ENUM_CLASS(RecipientType, To = 1, Cc, Bcc)

class RecipientType : public QObject
{
    Q_OBJECT
    Q_ENUMS(Type)
public:
    explicit RecipientType(QObject *parent = 0) : QObject(parent) {}
    enum Type {To, Cc, Bcc};
};

#endif // QMLENUMS

