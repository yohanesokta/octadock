#include "TaskManager.h"
#include <QDBusReply>
#include <QDBusArgument>
#include <QDebug>
#include <QtGui/qguiapplication_platform.h>
#include <xcb/xcb.h>

static xcb_connection_t* getXcbConn() {
    auto *x11App = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
    return x11App ? x11App->connection() : nullptr;
}

TaskManager::TaskManager(QObject *parent)
    : QObject(parent)
{
    connect(&m_pollTimer, &QTimer::timeout, this, &TaskManager::refreshWindows);
    m_pollTimer.start(1200);

    QTimer::singleShot(200, this, &TaskManager::refreshWindows);
}

QString TaskManager::extractAppId(const QString &title, const QString &iconName) {
    if (!iconName.isEmpty()) {
        QString clean = iconName;
        if (clean.startsWith("org.kde.")) clean = clean.mid(8);
        if (clean.startsWith("org.mozilla.")) clean = clean.mid(12);
        return clean.toLower();
    }

    QString t = title.toLower();
    if (t.contains("dolphin")) return "dolphin";
    if (t.contains("edge")) return "microsoft-edge";
    if (t.contains("firefox")) return "firefox";
    if (t.contains("chrome")) return "google-chrome";
    if (t.contains("kitty")) return "kitty";
    if (t.contains("konsole")) return "konsole";
    if (t.contains("nopilot") || t.contains("code") || t.contains("visual studio")) return "code";
    if (t.contains("kmail") || t.contains("mail")) return "kmail";
    if (t.contains("gwenview") || t.contains("photos")) return "gwenview";
    if (t.contains("telegram")) return "telegram-desktop";
    if (t.contains("discord")) return "discord";
    if (t.contains("system settings")) return "systemsettings";
    if (t.contains("krunner")) return "krunner";

    // Fallback: first word of title
    QString firstWord = title.split(' ').first().toLower();
    return firstWord.isEmpty() ? "application-default-icon" : firstWord;
}

void TaskManager::queryKWinWindows(QList<WindowTaskInfo> &windows) {
    QDBusMessage msg = QDBusMessage::createMethodCall(
        "org.kde.KWin",
        "/WindowsRunner",
        "org.kde.krunner1",
        "Match"
    );
    msg << QString("");

    QDBusMessage reply = QDBusConnection::sessionBus().call(msg);
    if (reply.type() != QDBusMessage::ReplyMessage || reply.arguments().isEmpty()) {
        return;
    }

    const QDBusArgument arg = reply.arguments().at(0).value<QDBusArgument>();
    arg.beginArray();
    while (!arg.atEnd()) {
        arg.beginStructure();
        QString id, text, icon;
        int category;
        double relevance;
        QVariantMap props;
        arg >> id >> text >> icon >> category >> relevance >> props;
        arg.endStructure();

        // KWin window entries start with "0_"
        if (id.startsWith("0_")) {
            // Ignore OctaDock's own window
            if (text.contains("OctaDock", Qt::CaseInsensitive)) {
                continue;
            }

            WindowTaskInfo info;
            info.windowId = id;
            info.title = text;
            info.iconName = icon;
            info.appId = extractAppId(text, icon);
            info.isActive = false;

            // Avoid duplicates
            bool alreadyAdded = false;
            for (const auto &w : windows) {
                if (w.windowId == info.windowId) {
                    alreadyAdded = true;
                    break;
                }
            }
            if (!alreadyAdded) {
                windows.append(info);
            }
        }
    }
    arg.endArray();
}

void TaskManager::queryX11Windows(QList<WindowTaskInfo> &windows) {
    Q_UNUSED(windows);
    xcb_connection_t *conn = getXcbConn();
    if (!conn) return;

    // Optional X11 fallback query if KWin DBus returns empty
}

void TaskManager::refreshWindows() {
    QList<WindowTaskInfo> windows;
    queryKWinWindows(windows);

    if (windows.isEmpty()) {
        queryX11Windows(windows);
    }

    if (windows != m_cachedWindows) {
        m_cachedWindows = windows;
        emit windowsChanged(m_cachedWindows);
    }
}

void TaskManager::activateWindow(const QString &windowId) {
    if (windowId.isEmpty()) return;

    if (windowId.startsWith("0_")) {
        QDBusMessage msg = QDBusMessage::createMethodCall(
            "org.kde.KWin",
            "/WindowsRunner",
            "org.kde.krunner1",
            "Run"
        );
        msg << windowId << QString("");
        QDBusConnection::sessionBus().call(msg);
    } else {
        // X11 fallback via _NET_ACTIVE_WINDOW
        xcb_connection_t *conn = getXcbConn();
        if (!conn) return;

        bool ok = false;
        uint32_t win = windowId.toUInt(&ok, 0);
        if (!ok || !win) return;

        xcb_intern_atom_cookie_t cookie = xcb_intern_atom(conn, 0, strlen("_NET_ACTIVE_WINDOW"), "_NET_ACTIVE_WINDOW");
        xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(conn, cookie, nullptr);
        if (!reply) return;

        xcb_client_message_event_t event;
        memset(&event, 0, sizeof(event));
        event.response_type = XCB_CLIENT_MESSAGE;
        event.format = 32;
        event.window = win;
        event.type = reply->atom;
        event.data.data32[0] = 2; // source: pager / dock
        event.data.data32[1] = XCB_CURRENT_TIME;
        event.data.data32[2] = 0;

        xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
        if (screen) {
            xcb_send_event(conn,
                           0,
                           screen->root,
                           XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                           reinterpret_cast<const char*>(&event));
            xcb_flush(conn);
        }
        free(reply);
    }
}

void TaskManager::closeWindow(const QString &windowId) {
    if (windowId.isEmpty()) return;

    xcb_connection_t *conn = getXcbConn();
    if (!conn) return;

    bool ok = false;
    uint32_t win = windowId.toUInt(&ok, 0);
    if (!ok || !win) return;

    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(conn, 0, strlen("_NET_CLOSE_WINDOW"), "_NET_CLOSE_WINDOW");
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(conn, cookie, nullptr);
    if (!reply) return;

    xcb_client_message_event_t event;
    memset(&event, 0, sizeof(event));
    event.response_type = XCB_CLIENT_MESSAGE;
    event.format = 32;
    event.window = win;
    event.type = reply->atom;
    event.data.data32[0] = XCB_CURRENT_TIME;
    event.data.data32[1] = 2;

    xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
    if (screen) {
        xcb_send_event(conn,
                       0,
                       screen->root,
                       XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                       reinterpret_cast<const char*>(&event));
        xcb_flush(conn);
    }
    free(reply);
}

void TaskManager::minimizeWindow(const QString &windowId) {
    Q_UNUSED(windowId);
}
