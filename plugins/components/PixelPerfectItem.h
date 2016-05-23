#ifndef PIXELPERFECTITEM_H
#define PIXELPERFECTITEM_H

#include <QObject>
#include <QQuickItem>
#include <QTimer>
#include <QVector>

class PixelPerfectItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *content READ content WRITE setContent NOTIFY contentChanged)
public:
    explicit PixelPerfectItem(QQuickItem *parent = 0);

    QQuickItem *content() const { return m_content; }
    void classBegin() {}
    void componentComplete();

signals:
    void contentChanged(QQuickItem *item);

public slots:
    void setContent(QQuickItem *item);

protected:
    void updatePolish();

protected slots:
    void triggerUpdate();

private:
    QQuickItem *m_content;
    QVector<QQuickItem *> m_ancestors;
    QTimer m_timer;
};

#endif // PIXELPERFECTITEM_H
