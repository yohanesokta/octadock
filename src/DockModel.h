#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QTimer>
#include "DockItem.h"
#include "AppLauncher.h"

class DockModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum DockRoles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        IconRole,
        ExecRole,
        IsRunningRole,
        BouncingRole,
        ItemRole
    };
    Q_ENUM(DockRoles)

    explicit DockModel(AppLauncher *launcher, QObject *parent = nullptr);
    ~DockModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const { return m_items.size(); }

    Q_INVOKABLE void launch(int index);
    Q_INVOKABLE void launchItem(const QString &id);
    Q_INVOKABLE void removeItem(int index);
    Q_INVOKABLE void addItem(const QString &id, const QString &name, const QString &icon, const QString &exec);
    Q_INVOKABLE void triggerBounce(int index);
    Q_INVOKABLE void updateRunningStates();
    Q_INVOKABLE void setItemRunning(const QString &id, bool running);

signals:
    void countChanged(int count);
    void bounceTriggered(int index);

private slots:
    void onAppLaunched(const QString &id, qint64 pid);
    void onAppLaunchFailed(const QString &id, const QString &error);

private:
    void initDefaultItems();
    void loadFromSettings();
    void saveToSettings();

    QList<DockItem*> m_items;
    AppLauncher *m_launcher{nullptr};
    QTimer m_checkTimer;
};
