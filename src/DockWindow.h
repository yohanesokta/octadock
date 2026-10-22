#pragma once

#include <QQuickView>
#include <QPointer>
#include "DockModel.h"
#include "DockSettings.h"
#include "AppLauncher.h"
#include "PlatformIntegration.h"

class DockWindow : public QQuickView {
    Q_OBJECT

public:
    explicit DockWindow(DockSettings *settings,
                        DockModel *model,
                        AppLauncher *launcher,
                        PlatformIntegration *platform,
                        QWindow *parent = nullptr);
    ~DockWindow() override;

    void initialize();
    Q_INVOKABLE void updateDockGeometry(int contentWidth, int contentHeight);

public slots:
    void onScreenGeometryChanged();
    void onSettingsChanged();

private:
    DockSettings *m_settings{nullptr};
    DockModel *m_model{nullptr};
    AppLauncher *m_launcher{nullptr};
    PlatformIntegration *m_platform{nullptr};
};
