#ifndef SENDERIDENTITIES_H
#define SENDERIDENTITIES_H

#include <QObject>
#include <QmlObjectListModel.h>
#include <Account.h>

class SenderIdentities : public QObject
{
    Q_OBJECT
    // Takes Accounts::model with filter type Accounts::CanSend
    Q_PROPERTY(QObject *accountsModel READ accountsModel WRITE setAccountsModel NOTIFY modelsChanged)
    Q_PROPERTY(QObject *selectedAccount READ selectedAccount NOTIFY selectedIndexChanged)
    Q_PROPERTY(int selectedIndex READ selectedIndex WRITE setSelectedIndex NOTIFY selectedIndexChanged)
public:
    explicit SenderIdentities(QObject *parent = 0);

    int selectedIndex() const;
    QObject *selectedAccount() const;
    QObject *accountsModel() const;
    bool isEmpty();

signals:
    void selectedIndexChanged();
    void modelsChanged();

public slots:
    void setSelectedIndex(int selectedIndex);
    void setAccountsModel(QObject * accountsModel);
    void setSelectedIndexFromAccountId(quint64 accountId);
    void reset();

private slots:
    void accountsChanged(void);

private:
    int m_selectedIndex;
    QQmlObjectListModel<Account> *m_accountsModel;
};

#endif // SENDERIDENTITIES_H
