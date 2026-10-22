#include "DockItem.h"

DockItem::DockItem(QObject *parent)
    : QObject(parent)
{
}

DockItem::DockItem(const QString &id,
                   const QString &name,
                   const QString &icon,
                   const QString &execCommand,
                   const QStringList &execArgs,
                   bool isRunning,
                   QObject *parent)
    : QObject(parent)
    , m_id(id)
    , m_name(name)
    , m_icon(icon)
    , m_execCommand(execCommand)
    , m_execArgs(execArgs)
    , m_isRunning(isRunning)
{
}

void DockItem::setId(const QString &id) {
    if (m_id != id) {
        m_id = id;
        emit idChanged(m_id);
    }
}

void DockItem::setName(const QString &name) {
    if (m_name != name) {
        m_name = name;
        emit nameChanged(m_name);
    }
}

void DockItem::setIcon(const QString &icon) {
    if (m_icon != icon) {
        m_icon = icon;
        emit iconChanged(m_icon);
    }
}

void DockItem::setExecCommand(const QString &execCommand) {
    if (m_execCommand != execCommand) {
        m_execCommand = execCommand;
        emit execCommandChanged(m_execCommand);
    }
}

void DockItem::setExecArgs(const QStringList &execArgs) {
    if (m_execArgs != execArgs) {
        m_execArgs = execArgs;
        emit execArgsChanged(m_execArgs);
    }
}

void DockItem::setIsRunning(bool isRunning) {
    if (m_isRunning != isRunning) {
        m_isRunning = isRunning;
        emit isRunningChanged(m_isRunning);
    }
}

void DockItem::setBouncing(bool bouncing) {
    if (m_bouncing != bouncing) {
        m_bouncing = bouncing;
        emit bouncingChanged(m_bouncing);
    }
}

void DockItem::triggerBounce() {
    emit bounceRequested();
}
