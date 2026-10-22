#include "DockModel.h"
#include <QSettings>
#include <QDebug>

DockModel::DockModel(AppLauncher *launcher, QObject *parent)
    : QAbstractListModel(parent)
    , m_launcher(launcher)
{
    if (m_launcher) {
        connect(m_launcher, &AppLauncher::appLaunched, this, &DockModel::onAppLaunched);
        connect(m_launcher, &AppLauncher::appLaunchFailed, this, &DockModel::onAppLaunchFailed);
    }

    loadFromSettings();

    // Timer to update running states of docked applications
    connect(&m_checkTimer, &QTimer::timeout, this, &DockModel::updateRunningStates);
    m_checkTimer.start(2500);
}

DockModel::~DockModel() {
    qDeleteAll(m_items);
    m_items.clear();
}

int DockModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return m_items.size();
}

QVariant DockModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size()) {
        return QVariant();
    }

    DockItem *item = m_items.at(index.row());
    switch (role) {
    case IdRole:
        return item->id();
    case NameRole:
        return item->name();
    case IconRole:
        return item->icon();
    case ExecRole:
        return item->execCommand();
    case IsRunningRole:
        return item->isRunning();
    case BouncingRole:
        return item->bouncing();
    case ItemRole:
        return QVariant::fromValue(item);
    default:
        return QVariant();
    }
}

bool DockModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size()) {
        return false;
    }

    DockItem *item = m_items.at(index.row());
    bool changed = false;

    switch (role) {
    case IdRole:
        item->setId(value.toString());
        changed = true;
        break;
    case NameRole:
        item->setName(value.toString());
        changed = true;
        break;
    case IconRole:
        item->setIcon(value.toString());
        changed = true;
        break;
    case ExecRole:
        item->setExecCommand(value.toString());
        changed = true;
        break;
    case IsRunningRole:
        if (item->isRunning() != value.toBool()) {
            item->setIsRunning(value.toBool());
            changed = true;
        }
        break;
    case BouncingRole:
        if (item->bouncing() != value.toBool()) {
            item->setBouncing(value.toBool());
            changed = true;
        }
        break;
    }

    if (changed) {
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

QHash<int, QByteArray> DockModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IdRole] = "itemId";
    roles[NameRole] = "name";
    roles[IconRole] = "iconSource";
    roles[ExecRole] = "execCommand";
    roles[IsRunningRole] = "isRunning";
    roles[BouncingRole] = "isBouncing";
    roles[ItemRole] = "dockItem";
    return roles;
}

void DockModel::launch(int index) {
    if (index < 0 || index >= m_items.size()) return;

    DockItem *item = m_items.at(index);
    triggerBounce(index);

    if (m_launcher) {
        m_launcher->launch(item->id(), item->execCommand(), item->execArgs());
    }
}

void DockModel::launchItem(const QString &id) {
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items.at(i)->id() == id) {
            launch(i);
            return;
        }
    }
}

void DockModel::triggerBounce(int index) {
    if (index < 0 || index >= m_items.size()) return;

    DockItem *item = m_items.at(index);
    item->setBouncing(true);
    item->triggerBounce();
    emit bounceTriggered(index);
    emit dataChanged(this->index(index), this->index(index), {BouncingRole});
}

void DockModel::removeItem(int index) {
    if (index < 0 || index >= m_items.size()) return;

    beginRemoveRows(QModelIndex(), index, index);
    DockItem *item = m_items.takeAt(index);
    delete item;
    endRemoveRows();

    emit countChanged(m_items.size());
    saveToSettings();
}

void DockModel::addItem(const QString &id, const QString &name, const QString &icon, const QString &exec) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    DockItem *item = new DockItem(id, name, icon, exec, {}, false, this);
    m_items.append(item);
    endInsertRows();

    emit countChanged(m_items.size());
    saveToSettings();
}

void DockModel::updateRunningStates() {
    if (!m_launcher) return;

    for (int i = 0; i < m_items.size(); ++i) {
        DockItem *item = m_items.at(i);
        bool running = m_launcher->isRunning(item->id(), item->execCommand());
        if (item->isRunning() != running) {
            item->setIsRunning(running);
            emit dataChanged(index(i), index(i), {IsRunningRole});
        }
    }
}

void DockModel::setItemRunning(const QString &id, bool running) {
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items.at(i)->id() == id) {
            m_items.at(i)->setIsRunning(running);
            emit dataChanged(index(i), index(i), {IsRunningRole});
            return;
        }
    }
}

void DockModel::onAppLaunched(const QString &id, qint64 pid) {
    Q_UNUSED(pid);
    setItemRunning(id, true);
}

void DockModel::onAppLaunchFailed(const QString &id, const QString &error) {
    qWarning() << "App launch failed for" << id << ":" << error;
}

void DockModel::initDefaultItems() {
    // Reference application layout matching GEMINI.md:
    // 1. Finder
    // 2. Launchpad
    // 3. Safari
    // 4. Mail
    // 5. FaceTime
    // 6. Messages
    // 7. Maps
    // 8. Photos
    // 9. Contacts
    struct DefaultApp {
        const char *id;
        const char *name;
        const char *icon;
        const char *exec;
        bool running;
    };

    const DefaultApp defaults[] = {
        {"finder", "Finder", "qrc:/assets/icons/finder.svg", "dolphin", true},
        {"launchpad", "Launchpad", "qrc:/assets/icons/launchpad.svg", "krunner", false},
        {"safari", "Safari", "qrc:/assets/icons/safari.svg", "firefox", false},
        {"mail", "Mail", "qrc:/assets/icons/mail.svg", "kmail", false},
        {"facetime", "FaceTime", "qrc:/assets/icons/facetime.svg", "kamoso", false},
        {"messages", "Messages", "qrc:/assets/icons/messages.svg", "telegram-desktop", false},
        {"maps", "Maps", "qrc:/assets/icons/maps.svg", "marble", false},
        {"photos", "Photos", "qrc:/assets/icons/photos.svg", "gwenview", false},
        {"contacts", "Contacts", "qrc:/assets/icons/contacts.svg", "kaddressbook", false}
    };

    beginResetModel();
    qDeleteAll(m_items);
    m_items.clear();

    for (const auto &d : defaults) {
        m_items.append(new DockItem(QLatin1String(d.id),
                                    QLatin1String(d.name),
                                    QLatin1String(d.icon),
                                    QLatin1String(d.exec),
                                    {},
                                    d.running,
                                    this));
    }
    endResetModel();
    emit countChanged(m_items.size());
}

void DockModel::loadFromSettings() {
    QSettings s("OctaDock", "OctaDock");
    int size = s.beginReadArray("items");
    if (size == 0) {
        s.endArray();
        initDefaultItems();
        updateRunningStates();
        return;
    }

    beginResetModel();
    qDeleteAll(m_items);
    m_items.clear();

    for (int i = 0; i < size; ++i) {
        s.setArrayIndex(i);
        QString id = s.value("id").toString();
        QString name = s.value("name").toString();
        QString icon = s.value("icon").toString();
        QString exec = s.value("exec").toString();
        bool isRunning = s.value("isRunning", false).toBool();

        m_items.append(new DockItem(id, name, icon, exec, {}, isRunning, this));
    }
    s.endArray();
    endResetModel();
    emit countChanged(m_items.size());

    updateRunningStates();
}

void DockModel::saveToSettings() {
    QSettings s("OctaDock", "OctaDock");
    s.beginWriteArray("items", m_items.size());
    for (int i = 0; i < m_items.size(); ++i) {
        s.setArrayIndex(i);
        DockItem *item = m_items.at(i);
        s.setValue("id", item->id());
        s.setValue("name", item->name());
        s.setValue("icon", item->icon());
        s.setValue("exec", item->execCommand());
        s.setValue("isRunning", item->isRunning());
    }
    s.endArray();
}
