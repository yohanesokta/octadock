#include "AppLauncher.h"
#include <QStandardPaths>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QHash>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>

AppLauncher::AppLauncher(QObject *parent)
    : QObject(parent)
{
}

QString AppLauncher::findExecutable(const QStringList &candidates) {
    for (const QString &cand : candidates) {
        if (cand.startsWith("http://") || cand.startsWith("https://") || cand.startsWith("mailto:")) {
            return cand;
        }
        QString resolved = QStandardPaths::findExecutable(cand);
        if (!resolved.isEmpty()) {
            return resolved;
        }
        if (QFileInfo::exists(cand) && QFileInfo(cand).isExecutable()) {
            return cand;
        }
    }
    return QString();
}

QString AppLauncher::resolveCommand(const QString &id, const QString &command) {
    if (!command.isEmpty()) {
        QString found = findExecutable({command});
        if (!found.isEmpty()) {
            return found;
        }
    }

    if (id == QLatin1String("finder")) {
        return findExecutable({"dolphin", "nautilus", "thunar", "pcmanfm-qt", "pcmanfm", "xdg-open"});
    } else if (id == QLatin1String("launchpad")) {
        return findExecutable({"krunner", "fuzzel", "rofi", "wofi", "dmenu_run"});
    } else if (id == QLatin1String("safari")) {
        return findExecutable({"firefox", "google-chrome-stable", "chromium", "brave-browser", "epiphany", "x-www-browser"});
    } else if (id == QLatin1String("mail")) {
        return findExecutable({"kmail", "thunderbird", "geary", "evolution", "betterbird"});
    } else if (id == QLatin1String("facetime")) {
        return findExecutable({"kamoso", "cheese", "snapshot", "cameractrls"});
    } else if (id == QLatin1String("messages")) {
        return findExecutable({"telegram-desktop", "discord", "signal-desktop", "element-desktop", "vesktop"});
    } else if (id == QLatin1String("maps")) {
        return findExecutable({"marble", "gnome-maps", "https://maps.google.com"});
    } else if (id == QLatin1String("photos")) {
        return findExecutable({"gwenview", "eog", "loupe", "shotwell", "ristretto", "viewnior"});
    } else if (id == QLatin1String("contacts")) {
        return findExecutable({"kaddressbook", "gnome-contacts", "https://contacts.google.com"});
    } else if (id == QLatin1String("settings")) {
        return findExecutable({"systemsettings", "gnome-control-center", "xfce4-settings-manager"});
    }

    return command;
}

bool AppLauncher::launch(const QString &id, const QString &command, const QStringList &args) {
    static QHash<QString, qint64> s_lastLaunchTime;
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    QString key = id + QLatin1Char(':') + command;
    if (s_lastLaunchTime.contains(key) && (now - s_lastLaunchTime.value(key)) < 800) {
        return true; // Ignore duplicate/double launch within 800ms
    }
    s_lastLaunchTime[key] = now;

    if (id == QLatin1String("settings") && command.isEmpty()) {
        emit openSettingsRequested();
        return true;
    }

    QString resolved = resolveCommand(id, command);
    if (resolved.isEmpty()) {
        if (id == QLatin1String("finder")) {
            resolved = "xdg-open";
            return QProcess::startDetached(resolved, {QDir::homePath()});
        }
        if (id == QLatin1String("photos")) {
            resolved = "xdg-open";
            QString picDir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
            return QProcess::startDetached(resolved, {picDir.isEmpty() ? QDir::homePath() : picDir});
        }
        if (id == QLatin1String("safari")) {
            return QDesktopServices::openUrl(QUrl("https://www.google.com"));
        }
        if (id == QLatin1String("maps")) {
            return QDesktopServices::openUrl(QUrl("https://maps.google.com"));
        }
        if (id == QLatin1String("contacts")) {
            return QDesktopServices::openUrl(QUrl("https://contacts.google.com"));
        }
        if (id == QLatin1String("facetime")) {
            return QDesktopServices::openUrl(QUrl("https://meet.google.com"));
        }
        if (id == QLatin1String("messages")) {
            return QDesktopServices::openUrl(QUrl("https://messages.google.com"));
        }
        if (id == QLatin1String("mail")) {
            return QDesktopServices::openUrl(QUrl("mailto:"));
        }

        qWarning() << "Could not resolve executable for dock item:" << id << command;
        emit appLaunchFailed(id, QString("No executable found for %1").arg(id));
        return false;
    }

    if (resolved.startsWith("http://") || resolved.startsWith("https://") || resolved.startsWith("mailto:")) {
        bool ok = QDesktopServices::openUrl(QUrl(resolved));
        if (ok) emit appLaunched(id, 0);
        return ok;
    }

    qint64 pid = 0;
    QStringList finalArgs = args;
    if (id == QLatin1String("launchpad") && resolved.contains("rofi")) {
        if (finalArgs.isEmpty()) finalArgs << "-show" << "drun";
    }

    bool started = QProcess::startDetached(resolved, finalArgs, QString(), &pid);
    if (started) {
        if (pid > 0) {
            m_launchedPids.insert(pid);
        }
        emit appLaunched(id, pid);
        return true;
    } else {
        qWarning() << "Failed to start process:" << resolved << finalArgs;
        emit appLaunchFailed(id, QString("Failed to start %1").arg(resolved));
        return false;
    }
}

bool AppLauncher::isRunning(const QString &id, const QString &command) {
    if (id == QLatin1String("finder")) {
        // Desktop manager is typically always running
        return true;
    }

    QString bin = command;
    if (bin.isEmpty()) {
        bin = resolveCommand(id, QString());
    }
    if (bin.isEmpty()) {
        return false;
    }

    QFileInfo info(bin);
    QString baseName = info.baseName();
    if (baseName.isEmpty()) return false;

    // Check /proc for running process names
    QDir procDir("/proc");
    const QStringList pids = procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &pidStr : pids) {
        bool ok = false;
        pidStr.toInt(&ok);
        if (!ok) continue;

        QFile commFile(QString("/proc/%1/comm").arg(pidStr));
        if (commFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString comm = QString::fromUtf8(commFile.readAll()).trimmed();
            if (comm == baseName || comm.startsWith(baseName.left(14))) {
                return true;
            }
        }
    }

    return false;
}

bool AppLauncher::openSettings() {
    emit openSettingsRequested();
    return true;
}
