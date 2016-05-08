#ifndef STRETCHCOLUMN_H
#define STRETCHCOLUMN_H

#include <QObject>
#include <QQuickItem>

class StretchColumn : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY (int spacing READ getSpacing WRITE setSpacing NOTIFY spacingChanged)

public:
    explicit StretchColumn (QQuickItem * parent = Q_NULLPTR);

    int getSpacing (void) const;

public slots:
    void setSpacing (int spacing);

signals:
    void spacingChanged (int spacing);

protected:
    void classBegin        (void);
    void componentComplete (void);
    void updatePolish      (void);
    void itemChange        (ItemChange change, const ItemChangeData & value);

private:
    int m_spacing;
};

#endif // STRETCHCOLUMN_H
