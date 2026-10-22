#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QSet>

class AppLauncher : public QObject {
    Q_OBJECT

public:
    explicit AppLauncher(QObject *parent = nullptr);

    Q_INVOKABLE bool launch(const QString &id, const QString &command, const QStringList &args = QStringList());
    Q_INVOKABLE bool isRunning(const QString &id, const QString &command);
    Q_INVOKABLE QString resolveCommand(const QString &id, const QString &command);
    Q_INVOKABLE bool openSettings();

signals:
    void appLaunched(const QString &id, qint64 pid);
    void appLaunchFailed(const QString &id, const QString &error);
    void openSettingsRequested();

private:
    QString findExecutable(const QStringList &candidates);
    QSet<qint64> m_launchedPids;
};
