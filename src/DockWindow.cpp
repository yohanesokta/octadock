#include "DockWindow.h"
#include <QQmlContext>
#include <QQmlEngine>
#include <QScreen>
#include <QGuiApplication>
#include <QQuickItem>
#include <QDebug>

DockWindow::DockWindow(DockSettings *settings,
                       DockModel *model,
                       AppLauncher *launcher,
                       PlatformIntegration *platform,
                       QWindow *parent)
    : QQuickView(parent)
    , m_settings(settings)
    , m_model(model)
    , m_launcher(launcher)
    , m_platform(platform)
{
    setTitle("OctaDock");
    setColor(QColor(Qt::transparent));

    if (m_platform) {
        m_platform->setupWindow(this);
        connect(m_platform, &PlatformIntegration::screenGeometryChanged, this, &DockWindow::onScreenGeometryChanged);
    }

    if (m_settings) {
        connect(m_settings, &DockSettings::dockHeightChanged, this, &DockWindow::onSettingsChanged);
        connect(m_settings, &DockSettings::bottomMarginChanged, this, &DockWindow::onSettingsChanged);
        connect(m_settings, &DockSettings::iconSizeChanged, this, &DockWindow::onSettingsChanged);
        connect(m_settings, &DockSettings::iconSpacingChanged, this, &DockWindow::onSettingsChanged);
    }
}

DockWindow::~DockWindow() {
}

void DockWindow::initialize() {
    QQmlContext *ctx = rootContext();
    ctx->setContextProperty("dockSettings", m_settings);
    ctx->setContextProperty("dockModel", m_model);
    ctx->setContextProperty("appLauncher", m_launcher);
    ctx->setContextProperty("platformIntegration", m_platform);
    ctx->setContextProperty("dockWindow", this);

    setResizeMode(QQuickView::SizeViewToRootObject);
    setSource(QUrl("qrc:/qml/Main.qml"));

    onScreenGeometryChanged();
}

void DockWindow::updateDockGeometry(int contentWidth, int contentHeight) {
    if (m_platform) {
        m_platform->repositionWindow(this, contentWidth, contentHeight);
    }
}

void DockWindow::onScreenGeometryChanged() {
    if (rootObject() && rootObject()->width() > 0 && rootObject()->height() > 0) {
        updateDockGeometry(static_cast<int>(rootObject()->width()), static_cast<int>(rootObject()->height()));
    } else if (width() > 0 && height() > 0) {
        updateDockGeometry(width(), height());
    }
}

void DockWindow::onSettingsChanged() {
    if (rootObject() && rootObject()->width() > 0 && rootObject()->height() > 0) {
        updateDockGeometry(static_cast<int>(rootObject()->width()), static_cast<int>(rootObject()->height()));
    } else if (width() > 0 && height() > 0) {
        updateDockGeometry(width(), height());
    }
}
