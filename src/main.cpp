#include <QApplication>
#include <QQuickWindow>
#include <QDir>
#include <QIcon>
#include <QSettings>
#include <QStandardPaths>
#include <QTimer>
#include <QDebug>
#include "DockSettings.h"
#include "AppLauncher.h"
#include "TaskManager.h"
#include "DockModel.h"
#include "PlatformIntegration.h"
#include "DockWindow.h"

static void setupIconTheme() {
    // Configure standard icon search paths so QIcon can find user and system themes
    QStringList searchPaths = QIcon::themeSearchPaths();
    QString home = QDir::homePath();
    QStringList extraPaths = {
        home + "/.local/share/icons",
        home + "/.icons",
        "/usr/local/share/icons",
        "/usr/share/icons",
        "/var/lib/flatpak/exports/share/icons",
        home + "/.local/share/flatpak/exports/share/icons"
    };
    for (const QString &p : extraPaths) {
        if (QDir(p).exists() && !searchPaths.contains(p)) {
            searchPaths.prepend(p);
        }
    }
    QIcon::setThemeSearchPaths(searchPaths);

    // Detect KDE icon theme from kdeglobals
    QString themeName = "WhiteSur-dark";
    QString kdeGlobalsPath = home + "/.config/kdeglobals";
    if (QFile::exists(kdeGlobalsPath)) {
        QSettings kdeSettings(kdeGlobalsPath, QSettings::IniFormat);
        QString t = kdeSettings.value("Icons/Theme").toString();
        if (!t.isEmpty()) {
            themeName = t;
        }
    }

    if (QIcon::themeName().isEmpty()) {
        QIcon::setThemeName(themeName);
    }
}

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

    setupIconTheme();

    DockSettings settings;
    AppLauncher launcher;
    TaskManager taskManager;
    DockModel model(&launcher, &taskManager);
    PlatformIntegration platform(&settings);

    DockWindow window(&settings, &model, &launcher, &platform, &taskManager);
    window.initialize();
    window.show();

    if (app.arguments().contains("--preferences") || app.arguments().contains("--settings")) {
        QTimer::singleShot(400, &launcher, &AppLauncher::openSettingsRequested);
    }

    return app.exec();
}
