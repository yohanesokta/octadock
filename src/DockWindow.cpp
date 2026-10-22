#include "DockWindow.h"
#include <QQmlContext>
#include <QQmlEngine>
#include <QScreen>
#include <QGuiApplication>
#include <QQuickItem>
#include <QMenu>
#include <QAction>
#include <QCursor>
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
        connect(m_settings, &DockSettings::cornerRadiusChanged, this, &DockWindow::onSettingsChanged);
        connect(m_settings, &DockSettings::verticalPaddingChanged, this, &DockWindow::onSettingsChanged);
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

void DockWindow::showContextMenu(int index, const QString &name, const QString &itemId, const QString &execCommand, int screenX, int screenY) {
    Q_UNUSED(itemId);
    Q_UNUSED(execCommand);

    QMenu menu;
    menu.setAttribute(Qt::WA_TranslucentBackground);
    menu.setWindowFlags(menu.windowFlags() | Qt::FramelessWindowHint);
    menu.setStyleSheet(R"(
        QMenu {
            background-color: rgba(28, 32, 40, 245);
            border: 1px solid rgba(255, 255, 255, 45);
            border-radius: 10px;
            padding: 6px;
            color: #ffffff;
            font-size: 13px;
        }
        QMenu::item {
            padding: 6px 22px 6px 12px;
            border-radius: 6px;
            background-color: transparent;
        }
        QMenu::item:selected {
            background-color: #007aff;
            color: #ffffff;
        }
        QMenu::separator {
            height: 1px;
            background-color: rgba(255, 255, 255, 28);
            margin: 4px 6px;
        }
    )");

    if (index >= 0 && !name.isEmpty()) {
        QAction *openAction = menu.addAction(QString("Open %1").arg(name));
        connect(openAction, &QAction::triggered, this, [this, index]() {
            if (m_model) m_model->launch(index);
        });
        menu.addSeparator();
    }

    QAction *prefAction = menu.addAction("Dock Preferences...");
    connect(prefAction, &QAction::triggered, this, [this]() {
        if (m_launcher) emit m_launcher->openSettingsRequested();
    });

    if (index >= 0) {
        QAction *removeAction = menu.addAction("Remove from Dock");
        connect(removeAction, &QAction::triggered, this, [this, index]() {
            if (m_model) m_model->removeItem(index);
        });
    }

    menu.addSeparator();

    QAction *quitAction = menu.addAction("Quit OctaDock");
    connect(quitAction, &QAction::triggered, this, []() {
        qApp->quit();
    });

    QPoint targetPos;
    if (screenX >= 0 && screenY >= 0) {
        targetPos = QPoint(screenX, screenY);
    } else {
        targetPos = QCursor::pos();
    }

    // Measure menu size
    QSize sz = menu.sizeHint();
    int targetX = targetPos.x() - sz.width() / 2;
    int targetY = targetPos.y() - sz.height() - 8;

    // Screen boundary safety clamp
    QScreen *scr = screen();
    if (!scr) scr = QGuiApplication::primaryScreen();
    if (scr) {
        QRect geo = scr->geometry();
        targetX = qBound(geo.left() + 10, targetX, geo.right() - sz.width() - 10);
        if (targetY < geo.top() + 10) {
            targetY = targetPos.y() + 24; // fallback below if somehow near top screen edge
        }
    }

    menu.exec(QPoint(targetX, targetY));
}
