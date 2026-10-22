#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

class DockItem : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(QString execCommand READ execCommand WRITE setExecCommand NOTIFY execCommandChanged)
    Q_PROPERTY(QStringList execArgs READ execArgs WRITE setExecArgs NOTIFY execArgsChanged)
    Q_PROPERTY(bool isRunning READ isRunning WRITE setIsRunning NOTIFY isRunningChanged)
    Q_PROPERTY(bool bouncing READ bouncing WRITE setBouncing NOTIFY bouncingChanged)

public:
    explicit DockItem(QObject *parent = nullptr);
    DockItem(const QString &id,
             const QString &name,
             const QString &icon,
             const QString &execCommand,
             const QStringList &execArgs = QStringList(),
             bool isRunning = false,
             QObject *parent = nullptr);

    QString id() const { return m_id; }
    void setId(const QString &id);

    QString name() const { return m_name; }
    void setName(const QString &name);

    QString icon() const { return m_icon; }
    void setIcon(const QString &icon);

    QString execCommand() const { return m_execCommand; }
    void setExecCommand(const QString &execCommand);

    QStringList execArgs() const { return m_execArgs; }
    void setExecArgs(const QStringList &execArgs);

    bool isRunning() const { return m_isRunning; }
    void setIsRunning(bool isRunning);

    bool bouncing() const { return m_bouncing; }
    void setBouncing(bool bouncing);

public slots:
    void triggerBounce();

signals:
    void idChanged(const QString &id);
    void nameChanged(const QString &name);
    void iconChanged(const QString &icon);
    void execCommandChanged(const QString &execCommand);
    void execArgsChanged(const QStringList &execArgs);
    void isRunningChanged(bool isRunning);
    void bouncingChanged(bool bouncing);
    void bounceRequested();

private:
    QString m_id;
    QString m_name;
    QString m_icon;
    QString m_execCommand;
    QStringList m_execArgs;
    bool m_isRunning{false};
    bool m_bouncing{false};
};
