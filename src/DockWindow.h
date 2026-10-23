#pragma once

#include <QQuickView>
#include <QPointer>
#include <QTimer>
#include "DockModel.h"
#include "DockSettings.h"
#include "AppLauncher.h"
#include "PlatformIntegration.h"
#include "TaskManager.h"

class DockWindow : public QQuickView {
    Q_OBJECT
    Q_PROPERTY(bool isRevealed READ isRevealed WRITE setRevealed NOTIFY revealedChanged)

public:
    explicit DockWindow(DockSettings *settings,
                        DockModel *model,
                        AppLauncher *launcher,
                        PlatformIntegration *platform,
                        TaskManager *taskManager = nullptr,
                        QWindow *parent = nullptr);
    ~DockWindow() override;

    void initialize();
    bool isRevealed() const { return m_isRevealed; }
    Q_INVOKABLE void setRevealed(bool revealed);

    Q_INVOKABLE void updateDockGeometry(int contentWidth, int contentHeight);
    Q_INVOKABLE void showContextMenu(int index, const QString &name, const QString &itemId, const QString &execCommand, int screenX = -1, int screenY = -1);

signals:
    void revealedChanged(bool revealed);

public slots:
    void onScreenGeometryChanged();
    void onSettingsChanged();
    void checkEdgeTrigger();

private:
    DockSettings *m_settings{nullptr};
    DockModel *m_model{nullptr};
    AppLauncher *m_launcher{nullptr};
    PlatformIntegration *m_platform{nullptr};
    TaskManager *m_taskManager{nullptr};

    bool m_isRevealed{true};
    QTimer m_edgeTriggerTimer;
    QTimer m_hideDelayTimer;
};
