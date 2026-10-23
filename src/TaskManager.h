#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include <QTimer>
#include <QDBusConnection>
#include <QDBusMessage>

struct WindowTaskInfo {
    QString windowId;
    QString title;
    QString appId;
    QString iconName;
    bool isActive{false};

    bool operator==(const WindowTaskInfo &other) const {
        return windowId == other.windowId &&
               title == other.title &&
               appId == other.appId &&
               iconName == other.iconName &&
               isActive == other.isActive;
    }
};

class TaskManager : public QObject {
    Q_OBJECT

public:
    explicit TaskManager(QObject *parent = nullptr);
    ~TaskManager() override = default;

    QList<WindowTaskInfo> openWindows() const { return m_cachedWindows; }

    Q_INVOKABLE void activateWindow(const QString &windowId);
    Q_INVOKABLE void closeWindow(const QString &windowId);
    Q_INVOKABLE void minimizeWindow(const QString &windowId);

signals:
    void windowsChanged(const QList<WindowTaskInfo> &windows);

public slots:
    void refreshWindows();

private:
    void queryKWinWindows(QList<WindowTaskInfo> &windows);
    void queryX11Windows(QList<WindowTaskInfo> &windows);
    QString extractAppId(const QString &title, const QString &iconName);

    QTimer m_pollTimer;
    QList<WindowTaskInfo> m_cachedWindows;
};
