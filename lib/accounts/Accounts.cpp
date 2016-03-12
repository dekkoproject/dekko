#include "Accounts.h"
#include <QDebug>
#include "qmailstore.h"

Accounts::Accounts(QObject *parent) : QObject(parent),
    m_filter(Enabled), m_model(0)
{
    m_model = new QQmlObjectListModel<Account>(this);
    emit modelChanged();
    connect(QMailStore::instance(),
            SIGNAL(accountsAdded(QMailAccountIdList)),
            this,
            SLOT(accountsAdded(QMailAccountIdList)));
    connect(QMailStore::instance(),
            SIGNAL(accountsRemoved(QMailAccountIdList)),
            this,
            SLOT(accountsRemoved(QMailAccountIdList)));
    connect(QMailStore::instance(),
            SIGNAL(accountsUpdated(QMailAccountIdList)),
            this,
            SLOT(accountsUpdated(QMailAccountIdList)));
    reset();
}

quint64 Accounts::maskForFilter(Accounts::Filters filter)
{
    switch(filter) {
    case Enabled:
        return QMailAccount::Enabled;
    case CanReceive:
        return QMailAccount::MessageSource;
    case CanSend:
        return QMailAccount::MessageSink;
    case Synchronized:
        return QMailAccount::Synchronized;
    case HasPersitentConnection:
        return QMailAccount::HasPersistentConnection;
    }
    Q_UNREACHABLE();
}

Accounts::Filters Accounts::filter() const
{
    return m_filter;
}

void Accounts::setFilter(Accounts::Filters filter)
{
    if (m_filter == filter)
        return;

    m_filter = filter;
    reset();
    emit filterChanged(filter);
}

void Accounts::accountsAdded(const QMailAccountIdList &ids)
{
    QMailAccountKey passKey;
    if (filter() != Enabled)
        // We need to ensure we only show enabled accounts
        // This means we can do filters like CanReceive | Enabled
        passKey = QMailAccountKey::status(maskForFilter(filter()) | QMailAccount::Enabled) & QMailAccountKey::id(ids);
    else
        passKey = QMailAccountKey::status(maskForFilter(filter())) & QMailAccountKey::id(ids);

    QMailAccountIdList results = QMailStore::instance()->queryAccounts(passKey);
    if(results.isEmpty())
        return;

    // TODO: once sortkey is implemented update the model with the sorted result
    // for now we just append the new account
    Q_FOREACH(const QMailAccountId &id, ids) {
        if (!m_idList.contains(id)) {
            Account *account = new Account();
            account->setId(id.toULongLong());
            m_model->append(account);
            m_idList.append(id);
        } else {
            qDebug() << "[Accounts]" << __func__ << "Account with same id already in model";
        }
    }
}

void Accounts::accountsUpdated(const QMailAccountIdList &ids)
{
    QMailAccountKey idKey(QMailAccountKey::id(ids));
    QMailAccountIdList validIds = QMailStore::instance()->queryAccounts(idKey & QMailAccountKey::status(maskForFilter(filter())));
    // We maybe here because a an account has been enabled/disabled or some other filter we use
    // in this model and we now need to remove or add an account to the model

    // remove any that shouldn't be in the model
    Q_FOREACH(const QMailAccountId &id, ids) {

        if (!validIds.contains(id)) {
            int index = m_idList.indexOf(id);
            if (index == -1) {
                continue;
            }
            m_model->remove(index);
            m_idList.removeAt(index);
        }
    }

    // go over the valid ids and either insert any missing or update any existing
    Q_FOREACH(const QMailAccountId &id, validIds) {
        int index = m_idList.indexOf(id);
        if (index == -1) {
            // Not here so create.
            Account *account = new Account();
            account->setId(id.toULongLong());
            m_model->append(account);
            m_idList.append(id);
        } else {
            // just notify somethings changed
            m_model->at(index)->emitAccountChanged();
        }
    }
}

void Accounts::accountsRemoved(const QMailAccountIdList &ids)
{
    Q_FOREACH(const QMailAccountId &id, ids) {
        int index = m_idList.indexOf(id);
        if (index == -1) {
            // This id#s not in this model
            continue;
        }
        // Remove both from model & idlist;
        m_model->remove(index);
        m_idList.removeAt(index);
    }
}

void Accounts::reset()
{
    m_model->clear();
    m_idList.clear();
    init();
}

void Accounts::init()
{
    auto idList = QMailStore::instance()->queryAccounts(QMailAccountKey::status(maskForFilter(filter())) ,QMailAccountSortKey());
    Q_FOREACH(const QMailAccountId &id, idList) {
        int index = m_idList.indexOf(id);
        if (index == -1) {
            // Not here so create.
            Account *account = new Account();
            account->setId(id.toULongLong());
            m_model->append(account);
            m_idList.append(id);
        }
    }
}

