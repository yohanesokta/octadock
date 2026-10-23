#include "DockModel.h"
#include <QSettings>
#include <QDebug>

DockModel::DockModel(AppLauncher *launcher, TaskManager *taskManager, QObject *parent)
    : QAbstractListModel(parent)
    , m_launcher(launcher)
    , m_taskManager(taskManager)
{
    if (m_launcher) {
        connect(m_launcher, &AppLauncher::appLaunched, this, &DockModel::onAppLaunched);
        connect(m_launcher, &AppLauncher::appLaunchFailed, this, &DockModel::onAppLaunchFailed);
    }

    if (m_taskManager) {
        connect(m_taskManager, &TaskManager::windowsChanged, this, &DockModel::onWindowsChanged);
    }

    loadFromSettings();

    // Timer to update running states of docked applications
    connect(&m_checkTimer, &QTimer::timeout, this, &DockModel::updateRunningStates);
    m_checkTimer.start(2000);
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
    case WindowIdRole:
        return item->windowId();
    case IsPinnedRole:
        return item->isPinned();
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
    case WindowIdRole:
        if (item->windowId() != value.toString()) {
            item->setWindowId(value.toString());
            changed = true;
        }
        break;
    case IsPinnedRole:
        if (item->isPinned() != value.toBool()) {
            item->setIsPinned(value.toBool());
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
    roles[WindowIdRole] = "windowId";
    roles[IsPinnedRole] = "isPinned";
    return roles;
}

void DockModel::launch(int index) {
    if (index < 0 || index >= m_items.size()) return;

    DockItem *item = m_items.at(index);
    triggerBounce(index);

    // If item has a running window, activate it like a real task manager!
    if (item->isRunning() && !item->windowId().isEmpty() && m_taskManager) {
        m_taskManager->activateWindow(item->windowId());
        return;
    }

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
    DockItem *item = new DockItem(id, name, icon, exec, {}, false, true, QString(), this);
    m_items.append(item);
    endInsertRows();

    emit countChanged(m_items.size());
    saveToSettings();
}

static bool isWindowMatch(const DockItem *item, const WindowTaskInfo &win) {
    QString id = item->id().toLower();
    QString exec = item->execCommand().toLower();
    QString winApp = win.appId.toLower();

    if (id == winApp || exec == winApp) return true;
    if ((id == "finder" || id == "dolphin") && (winApp == "dolphin" || winApp == "org.kde.dolphin")) return true;
    if ((id == "terminal" || id == "kitty") && (winApp == "kitty" || winApp == "konsole")) return true;
    if ((id == "code" || id == "nopilot") && (winApp == "code" || winApp == "visual-studio-code" || winApp == "vscode")) return true;
    if (id == "microsoft-edge" && (winApp == "microsoft-edge" || winApp == "msedge")) return true;
    if ((id == "launchpad" || id == "krunner") && winApp == "krunner") return true;
    if ((id == "settings" || id == "systemsettings") && (winApp == "systemsettings" || winApp == "preferences-system")) return true;
    if (id == "safari" && (winApp == "firefox" || winApp == "google-chrome")) return true;

    return false;
}

void DockModel::pinItem(int index) {
    if (index >= 0 && index < m_items.size()) {
        m_items.at(index)->setIsPinned(true);
        emit dataChanged(this->index(index), this->index(index), {IsPinnedRole});
        saveToSettings();
    }
}

void DockModel::unpinItem(int index) {
    if (index < 0 || index >= m_items.size()) return;

    DockItem *item = m_items.at(index);
    if (item->isRunning() && !item->windowId().isEmpty()) {
        item->setIsPinned(false);
        emit dataChanged(this->index(index), this->index(index), {IsPinnedRole});
        saveToSettings();
    } else {
        removeItem(index);
    }
}

void DockModel::onWindowsChanged(const QList<WindowTaskInfo> &windows) {
    // 1. Update running states and active window IDs for pinned items
    for (int i = 0; i < m_items.size(); ++i) {
        DockItem *item = m_items.at(i);
        if (!item->isPinned()) continue;

        bool found = false;
        QString winId;
        for (const auto &win : windows) {
            if (isWindowMatch(item, win)) {
                found = true;
                winId = win.windowId;
                break;
            }
        }

        // Check fallback process polling if no window match
        if (!found && m_launcher) {
            found = m_launcher->isRunning(item->id(), item->execCommand());
        }

        if (item->isRunning() != found || item->windowId() != winId) {
            item->setIsRunning(found);
            item->setWindowId(winId);
            emit dataChanged(index(i), index(i), {IsRunningRole, WindowIdRole});
        }
    }

    // 2. Identify unpinned open windows
    QList<WindowTaskInfo> unpinnedWindows;
    for (const auto &win : windows) {
        bool matchesPinned = false;
        for (int i = 0; i < m_items.size(); ++i) {
            DockItem *item = m_items.at(i);
            if (!item->isPinned()) continue;

            if (isWindowMatch(item, win)) {
                matchesPinned = true;
                break;
            }
        }
        if (!matchesPinned) {
            unpinnedWindows.append(win);
        }
    }

    // 3. Remove unpinned items whose windows were closed
    for (int i = m_items.size() - 1; i >= 0; --i) {
        DockItem *item = m_items.at(i);
        if (!item->isPinned()) {
            bool stillExists = false;
            for (const auto &uw : unpinnedWindows) {
                if (uw.windowId == item->windowId()) {
                    stillExists = true;
                    break;
                }
            }
            if (!stillExists) {
                beginRemoveRows(QModelIndex(), i, i);
                DockItem *removed = m_items.takeAt(i);
                delete removed;
                endRemoveRows();
                emit countChanged(m_items.size());
            }
        }
    }

    // 4. Add dynamic unpinned items for newly opened windows
    for (const auto &uw : unpinnedWindows) {
        bool alreadyInDock = false;
        for (int i = 0; i < m_items.size(); ++i) {
            if (m_items.at(i)->windowId() == uw.windowId) {
                alreadyInDock = true;
                break;
            }
        }
        if (!alreadyInDock) {
            beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
            DockItem *newItem = new DockItem(uw.appId,
                                             uw.title,
                                             uw.iconName.isEmpty() ? uw.appId : uw.iconName,
                                             uw.appId,
                                             {},
                                             true,
                                             false,
                                             uw.windowId,
                                             this);
            m_items.append(newItem);
            endInsertRows();
            emit countChanged(m_items.size());
        }
    }
}

void DockModel::updateRunningStates() {
    if (m_taskManager) {
        m_taskManager->refreshWindows();
        return;
    }

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
    struct DefaultApp {
        const char *id;
        const char *name;
        const char *icon;
        const char *exec;
        bool running;
    };

    const DefaultApp defaults[] = {
        {"finder", "Finder", "org.kde.dolphin", "dolphin", false},
        {"launchpad", "Launchpad", "krunner", "krunner", false},
        {"microsoft-edge", "Microsoft Edge", "microsoft-edge", "microsoft-edge", false},
        {"terminal", "Terminal", "kitty", "kitty", false},
        {"code", "Code", "code", "code", false},
        {"settings", "Settings", "systemsettings", "systemsettings", false}
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
                                    true,
                                    QString(),
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

        m_items.append(new DockItem(id, name, icon, exec, {}, false, true, QString(), this));
    }
    s.endArray();
    endResetModel();
    emit countChanged(m_items.size());

    updateRunningStates();
}

void DockModel::saveToSettings() {
    QSettings s("OctaDock", "OctaDock");
    QList<DockItem*> pinnedItems;
    for (DockItem *it : m_items) {
        if (it->isPinned()) pinnedItems.append(it);
    }

    s.beginWriteArray("items", pinnedItems.size());
    for (int i = 0; i < pinnedItems.size(); ++i) {
        s.setArrayIndex(i);
        DockItem *item = pinnedItems.at(i);
        s.setValue("id", item->id());
        s.setValue("name", item->name());
        s.setValue("icon", item->icon());
        s.setValue("exec", item->execCommand());
    }
    s.endArray();
}
