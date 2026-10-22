#include <QApplication>
#include <QQuickWindow>
#include <QDir>
#include <QStandardPaths>
#include <QTimer>
#include <QDebug>
#include "DockSettings.h"
#include "AppLauncher.h"
#include "DockModel.h"
#include "PlatformIntegration.h"
#include "DockWindow.h"

int main(int argc, char *argv[]) {
    // Enable alpha buffer for translucency
    QQuickWindow::setDefaultAlphaBuffer(true);

    // If QT_QPA_PLATFORM is not specified and running on Wayland,
    // default to xcb (via XWayland) for precise dock positioning and panel rules,
    // while honoring any explicit user choice.
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
        const QString sessionType = qEnvironmentVariable("XDG_SESSION_TYPE");
        if (sessionType.contains("wayland", Qt::CaseInsensitive)) {
            // Check if DISPLAY (XWayland) is available
            if (!qEnvironmentVariableIsEmpty("DISPLAY")) {
                qputenv("QT_QPA_PLATFORM", "xcb");
            }
        }
    }

    QApplication app(argc, argv);
    app.setApplicationName("OctaDock");
    app.setOrganizationName("OctaDock");
    app.setApplicationVersion("1.0.0");

    DockSettings settings;
    AppLauncher launcher;
    DockModel model(&launcher);
    PlatformIntegration platform(&settings);

    DockWindow window(&settings, &model, &launcher, &platform);
    window.initialize();
    window.show();

    if (app.arguments().contains("--preferences") || app.arguments().contains("--settings")) {
        QTimer::singleShot(400, &launcher, &AppLauncher::openSettingsRequested);
    }

    return app.exec();
}
