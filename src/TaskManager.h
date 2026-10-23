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
    Q_CLASSINFO("D-Bus Interface", "org.octadock.TaskManager")

public:
    explicit TaskManager(QObject *parent = nullptr);
    ~TaskManager() override;

    QList<WindowTaskInfo> openWindows() const { return m_cachedWindows; }

    Q_INVOKABLE void activateWindow(const QString &windowId);
    Q_INVOKABLE void closeWindow(const QString &windowId);
    Q_INVOKABLE void minimizeWindow(const QString &windowId);

signals:
    void windowsChanged(const QList<WindowTaskInfo> &windows);

public slots:
    void refreshWindows();
    Q_SCRIPTABLE void updateWindowsJson(const QString &json);

private:
    void setupKWinTrackingScript();
    void unloadKWinTrackingScript();
    void queryKWinWindows(QList<WindowTaskInfo> &windows);
    void queryX11Windows(QList<WindowTaskInfo> &windows);
    QString extractAppId(const QString &title, const QString &iconName);

    QTimer m_pollTimer;
    QList<WindowTaskInfo> m_cachedWindows;
    int m_kwinScriptId{-1};
    QString m_kwinScriptName;
};
