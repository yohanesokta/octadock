#include "PlatformIntegration.h"
#include <QGuiApplication>
#include <QDebug>
#include <QProcess>

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

    // Window flags for dock behavior
    Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool;
    window->setFlags(flags);

    // Make window bypass taskbar and window switcher where supported
    window->setProperty("_NET_WM_WINDOW_TYPE", "_NET_WM_WINDOW_TYPE_DOCK");
    window->setProperty("_NET_WM_STATE", "_NET_WM_STATE_SKIP_TASKBAR");

    if (isX11()) {
        applyX11Properties(window);
    } else if (isWayland()) {
        applyWaylandProperties(window);
    }

    setWindowBlur(window, true);
}

void PlatformIntegration::repositionWindow(QWindow *window, int dockWidth, int dockHeight) {
    if (!window || !m_currentScreen) return;

    QRect screenGeo = m_currentScreen->geometry();
    int margin = m_settings ? m_settings->bottomMargin() : 11;

    int x = screenGeo.x() + (screenGeo.width() - dockWidth) / 2;
    int y = screenGeo.y() + screenGeo.height() - dockHeight - margin;

    window->setGeometry(x, y, dockWidth, dockHeight);
}

void PlatformIntegration::setWindowBlur(QWindow *window, bool enable) {
    if (!window) return;

    // Request KWin blur behind region
    if (isKDE()) {
        window->setProperty("_KDE_NET_WM_BLUR_BEHIND_REGION", enable ? 1 : 0);
    }
}

void PlatformIntegration::applyX11Properties(QWindow *window) {
    Q_UNUSED(window);
}

void PlatformIntegration::applyWaylandProperties(QWindow *window) {
    Q_UNUSED(window);
}

void PlatformIntegration::onScreenGeometryChanged(const QRect &geometry) {
    Q_UNUSED(geometry);
    emit screenGeometryChanged();
}
