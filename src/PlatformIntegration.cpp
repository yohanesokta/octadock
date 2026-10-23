#include "PlatformIntegration.h"
#include <QGuiApplication>
#include <QDebug>
#include <QProcess>
#include <QtGui/qguiapplication_platform.h>
#include <xcb/xcb.h>
#include <xcb/shape.h>
#include <cmath>
#include <algorithm>

static xcb_connection_t* getNativeXcbConnection() {
    auto *x11App = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
    if (x11App) {
        return x11App->connection();
    }
    return nullptr;
}

static void setXcbAtomProperty(xcb_connection_t *conn, xcb_window_t win, const char *propName, const char *typeAtom, const QVector<uint32_t> &data) {
    if (!conn) return;

    xcb_intern_atom_cookie_t propCookie = xcb_intern_atom(conn, 0, strlen(propName), propName);
    xcb_intern_atom_reply_t *propReply = xcb_intern_atom_reply(conn, propCookie, nullptr);
    if (!propReply) return;

    xcb_atom_t type = XCB_ATOM_CARDINAL;
    if (strcmp(typeAtom, "ATOM") == 0) {
        type = XCB_ATOM_ATOM;
    }

    xcb_change_property(conn,
                        XCB_PROP_MODE_REPLACE,
                        win,
                        propReply->atom,
                        type,
                        32,
                        data.size(),
                        data.constData());

    free(propReply);
}

PlatformIntegration::PlatformIntegration(DockSettings *settings, QObject *parent)
    : QObject(parent)
    , m_settings(settings)
{
    m_currentScreen = QGuiApplication::primaryScreen();
    if (m_currentScreen) {
        connect(m_currentScreen, &QScreen::geometryChanged, this, &PlatformIntegration::onScreenGeometryChanged);
    }
}

int PlatformIntegration::screenWidth() const {
    if (m_currentScreen) {
        return m_currentScreen->geometry().width();
    }
    return 1920;
}

int PlatformIntegration::screenHeight() const {
    if (m_currentScreen) {
        return m_currentScreen->geometry().height();
    }
    return 1080;
}

qreal PlatformIntegration::devicePixelRatio() const {
    if (m_currentScreen) {
        return m_currentScreen->devicePixelRatio();
    }
    return 1.0;
}

bool PlatformIntegration::isWayland() const {
    return QGuiApplication::platformName().contains("wayland", Qt::CaseInsensitive);
}

bool PlatformIntegration::isX11() const {
    return QGuiApplication::platformName().contains("xcb", Qt::CaseInsensitive);
}

bool PlatformIntegration::isKDE() const {
    QString desktop = qEnvironmentVariable("XDG_CURRENT_DESKTOP");
    return desktop.contains("KDE", Qt::CaseInsensitive);
}

void PlatformIntegration::setupWindow(QWindow *window) {
    if (!window) return;

    Qt::WindowFlags flags = Qt::FramelessWindowHint;
    if (m_settings && (m_settings->alwaysOnTop() || m_settings->autoHide())) {
        flags |= Qt::WindowStaysOnTopHint;
    }
    window->setFlags(flags);

    if (isX11()) {
        applyX11Properties(window);
    } else if (isWayland()) {
        applyWaylandProperties(window);
    }

    setWindowBlur(window, true);
    updateStrut(window);
}

void PlatformIntegration::updateWindowFlags(QWindow *window) {
    if (!window) return;

    bool shouldStayOnTop = m_settings && (m_settings->alwaysOnTop() || m_settings->autoHide());
    Qt::WindowFlags flags = Qt::FramelessWindowHint;
    if (shouldStayOnTop) {
        flags |= Qt::WindowStaysOnTopHint;
    }

    window->setFlags(flags);
    if (isX11()) {
        applyX11Properties(window);
    }
    updateStrut(window);
    window->show();
}

void PlatformIntegration::repositionWindow(QWindow *window, int dockWidth, int dockHeight) {
    if (!window || !m_currentScreen) return;

    QRect screenGeo = m_currentScreen->geometry();

    int x = screenGeo.x() + (screenGeo.width() - dockWidth) / 2;
    int y = screenGeo.y() + screenGeo.height() - dockHeight;

    window->setGeometry(x, y, dockWidth, dockHeight);

    // Update blur region using exact dock dimensions to avoid stale geometry artifacts
    setWindowBlur(window, dockWidth, dockHeight, true);

    // Update strut reservations for maximized windows
    updateStrut(window);
}

void PlatformIntegration::setWindowBlur(QWindow *window, bool enable) {
    if (!window) return;
    setWindowBlur(window, window->width(), window->height(), enable);
}

void PlatformIntegration::setWindowBlur(QWindow *window, int dockWidth, int dockHeight, bool enable) {
    if (!window) return;

    WId winId = window->winId();
    if (!winId) return;

    xcb_connection_t *conn = getNativeXcbConnection();
    if (!conn) return;

    if (enable) {
        int glassHeight = m_settings ? m_settings->dockHeight() : 84;
        int cornerRadius = m_settings ? m_settings->cornerRadius() : 24;
        int margin = m_settings ? m_settings->bottomMargin() : 6;
        int winW = dockWidth > 0 ? dockWidth : window->width();
        int winH = dockHeight > 0 ? dockHeight : window->height();
        int yOffset = winH - glassHeight - margin;
        if (yOffset < 0) yOffset = 0;
        if (cornerRadius > glassHeight / 2) cornerRadius = glassHeight / 2;

        QVector<uint32_t> rectData;

        // Central horizontal body of the dock glass
        int bodyY = yOffset + cornerRadius;
        int bodyH = glassHeight - 2 * cornerRadius;
        if (bodyH > 0 && winW > 0) {
            rectData << 0 << static_cast<uint32_t>(bodyY) << static_cast<uint32_t>(winW) << static_cast<uint32_t>(bodyH);
        }

        // Top and bottom rounded slices to form a perfect squircle/capsule blur matching dockGlass
        for (int dy = 0; dy < cornerRadius; ++dy) {
            double dyFromCenter = cornerRadius - dy;
            int dx = static_cast<int>(std::round(cornerRadius - std::sqrt(std::max(0.0, static_cast<double>(cornerRadius * cornerRadius) - dyFromCenter * dyFromCenter))));
            uint32_t sliceW = winW > 2 * dx ? static_cast<uint32_t>(winW - 2 * dx) : 0;
            if (sliceW > 0) {
                // Top rounded slice (starts at yOffset + dy)
                rectData << static_cast<uint32_t>(dx) << static_cast<uint32_t>(yOffset + dy) << sliceW << 1;
                // Bottom rounded slice (ends at yOffset + glassHeight - 1 - dy)
                rectData << static_cast<uint32_t>(dx) << static_cast<uint32_t>(yOffset + glassHeight - 1 - dy) << sliceW << 1;
            }
        }

        if (rectData.isEmpty()) {
            rectData << 0 << static_cast<uint32_t>(yOffset) << static_cast<uint32_t>(winW) << static_cast<uint32_t>(glassHeight);
        }

        setXcbAtomProperty(conn, winId, "_KDE_NET_WM_BLUR_BEHIND_REGION", "CARDINAL", rectData);

        // Explicitly DELETE _KDE_NET_WM_BACKGROUND_CONTRAST_REGION to prevent dark shadow glitch
        xcb_intern_atom_cookie_t contrastCookie = xcb_intern_atom(conn, 0, strlen("_KDE_NET_WM_BACKGROUND_CONTRAST_REGION"), "_KDE_NET_WM_BACKGROUND_CONTRAST_REGION");
        xcb_intern_atom_reply_t *contrastReply = xcb_intern_atom_reply(conn, contrastCookie, nullptr);
        if (contrastReply) {
            xcb_delete_property(conn, winId, contrastReply->atom);
            free(contrastReply);
        }
    } else {
        xcb_intern_atom_cookie_t cookie = xcb_intern_atom(conn, 0, strlen("_KDE_NET_WM_BLUR_BEHIND_REGION"), "_KDE_NET_WM_BLUR_BEHIND_REGION");
        xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(conn, cookie, nullptr);
        if (reply) {
            xcb_delete_property(conn, winId, reply->atom);
            free(reply);
        }

        xcb_intern_atom_cookie_t contrastCookie = xcb_intern_atom(conn, 0, strlen("_KDE_NET_WM_BACKGROUND_CONTRAST_REGION"), "_KDE_NET_WM_BACKGROUND_CONTRAST_REGION");
        xcb_intern_atom_reply_t *contrastReply = xcb_intern_atom_reply(conn, contrastCookie, nullptr);
        if (contrastReply) {
            xcb_delete_property(conn, winId, contrastReply->atom);
            free(contrastReply);
        }
    }

    xcb_flush(conn);
}

void PlatformIntegration::updateStrut(QWindow *window) {
    if (!window) return;
    WId winId = window->winId();
    if (!winId) return;

    xcb_connection_t *conn = getNativeXcbConnection();
    if (!conn) return;

    bool isAutoHide = m_settings && m_settings->autoHide();
    if (!isAutoHide) {
        int glassHeight = m_settings ? m_settings->dockHeight() : 71;
        int margin = m_settings ? m_settings->bottomMargin() : 5;
        uint32_t reservedBottom = static_cast<uint32_t>(glassHeight + margin + 6);
        int scrW = m_currentScreen ? m_currentScreen->geometry().width() : 1920;

        // _NET_WM_STRUT: [left, right, top, bottom]
        QVector<uint32_t> strutData = {0, 0, 0, reservedBottom};
        setXcbAtomProperty(conn, winId, "_NET_WM_STRUT", "CARDINAL", strutData);

        // _NET_WM_STRUT_PARTIAL: 12 cardinals
        // [left, right, top, bottom, left_start_y, left_end_y, right_start_y, right_end_y, top_start_x, top_end_x, bottom_start_x, bottom_end_x]
        QVector<uint32_t> partialData = {
            0, 0, 0, reservedBottom,
            0, 0, 0, 0,
            0, 0, 0, static_cast<uint32_t>(scrW > 0 ? scrW - 1 : 1919)
        };
        setXcbAtomProperty(conn, winId, "_NET_WM_STRUT_PARTIAL", "CARDINAL", partialData);
    } else {
        // Delete strut hints so maximized windows can expand full screen
        const char *strutAtoms[] = {"_NET_WM_STRUT", "_NET_WM_STRUT_PARTIAL"};
        for (const char *atomName : strutAtoms) {
            xcb_intern_atom_cookie_t c = xcb_intern_atom(conn, 0, strlen(atomName), atomName);
            xcb_intern_atom_reply_t *r = xcb_intern_atom_reply(conn, c, nullptr);
            if (r) {
                xcb_delete_property(conn, winId, r->atom);
                free(r);
            }
        }
    }
    xcb_flush(conn);
}

void PlatformIntegration::setWindowInputTransparent(QWindow *window, bool transparent) {
    if (!window) return;
    WId winId = window->winId();
    if (!winId) return;

    xcb_connection_t *conn = getNativeXcbConnection();
    if (!conn) return;

    if (transparent) {
        xcb_shape_rectangles(conn, XCB_SHAPE_SO_SET, XCB_SHAPE_SK_INPUT, XCB_CLIP_ORDERING_UNSORTED, winId, 0, 0, 0, nullptr);
    } else {
        xcb_shape_mask(conn, XCB_SHAPE_SO_SET, XCB_SHAPE_SK_INPUT, winId, 0, 0, XCB_NONE);
    }
    xcb_flush(conn);
}

void PlatformIntegration::applyX11Properties(QWindow *window) {
    if (!window) return;
    WId winId = window->winId();
    if (!winId) return;

    xcb_connection_t *conn = getNativeXcbConnection();
    if (!conn) return;

    // Set _NET_WM_WINDOW_TYPE to _NET_WM_WINDOW_TYPE_DOCK
    xcb_intern_atom_cookie_t dockCookie = xcb_intern_atom(conn, 0, strlen("_NET_WM_WINDOW_TYPE_DOCK"), "_NET_WM_WINDOW_TYPE_DOCK");
    xcb_intern_atom_reply_t *dockReply = xcb_intern_atom_reply(conn, dockCookie, nullptr);
    if (dockReply) {
        setXcbAtomProperty(conn, winId, "_NET_WM_WINDOW_TYPE", "ATOM", {dockReply->atom});
        free(dockReply);
    }

    // Set _NET_WM_STATE: always skip taskbar/pager
    QVector<uint32_t> stateAtoms;
    const char *mandatoryStates[] = {
        "_NET_WM_STATE_SKIP_TASKBAR",
        "_NET_WM_STATE_SKIP_PAGER"
    };
    for (const char *name : mandatoryStates) {
        xcb_intern_atom_cookie_t c = xcb_intern_atom(conn, 0, strlen(name), name);
        xcb_intern_atom_reply_t *r = xcb_intern_atom_reply(conn, c, nullptr);
        if (r) {
            stateAtoms.append(r->atom);
            free(r);
        }
    }

    // Window layering:
    // If alwaysOnTop is set -> _NET_WM_STATE_ABOVE (dock sits on top of all windows)
    // If alwaysOnTop is false -> _NET_WM_STATE_BELOW (normal windows sit above the panel)
    const char *layerAtomName = (m_settings && m_settings->alwaysOnTop())
                                ? "_NET_WM_STATE_ABOVE"
                                : "_NET_WM_STATE_BELOW";
    xcb_intern_atom_cookie_t layerCookie = xcb_intern_atom(conn, 0, strlen(layerAtomName), layerAtomName);
    xcb_intern_atom_reply_t *layerReply = xcb_intern_atom_reply(conn, layerCookie, nullptr);
    if (layerReply) {
        stateAtoms.append(layerReply->atom);
        free(layerReply);
    }

    if (!stateAtoms.isEmpty()) {
        setXcbAtomProperty(conn, winId, "_NET_WM_STATE", "ATOM", stateAtoms);
    }

    xcb_flush(conn);
}

void PlatformIntegration::applyWaylandProperties(QWindow *window) {
    Q_UNUSED(window);
}

void PlatformIntegration::onScreenGeometryChanged(const QRect &geometry) {
    Q_UNUSED(geometry);
    emit screenGeometryChanged();
}
