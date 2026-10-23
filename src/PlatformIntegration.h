#pragma once

#include <QObject>
#include <QRect>
#include <QScreen>
#include <QWindow>
#include "DockSettings.h"

class PlatformIntegration : public QObject {
    Q_OBJECT
    Q_PROPERTY(int screenWidth READ screenWidth NOTIFY screenGeometryChanged)
    Q_PROPERTY(int screenHeight READ screenHeight NOTIFY screenGeometryChanged)
    Q_PROPERTY(qreal devicePixelRatio READ devicePixelRatio NOTIFY screenGeometryChanged)
    Q_PROPERTY(bool isWayland READ isWayland CONSTANT)
    Q_PROPERTY(bool isX11 READ isX11 CONSTANT)
    Q_PROPERTY(bool isKDE READ isKDE CONSTANT)

public:
    explicit PlatformIntegration(DockSettings *settings, QObject *parent = nullptr);
    ~PlatformIntegration() override;

    int screenWidth() const;
    int screenHeight() const;
    qreal devicePixelRatio() const;

    bool isWayland() const;
    bool isX11() const;
    bool isKDE() const;

    Q_INVOKABLE void setupWindow(QWindow *window);
    Q_INVOKABLE void updateWindowFlags(QWindow *window);
    Q_INVOKABLE void repositionWindow(QWindow *window, int dockWidth, int dockHeight);
    Q_INVOKABLE void setWindowBlur(QWindow *window, int dockWidth, int dockHeight, bool enable);
    Q_INVOKABLE void setWindowBlur(QWindow *window, bool enable);
    Q_INVOKABLE void updateStrut(QWindow *window);
    Q_INVOKABLE void setWindowInputTransparent(QWindow *window, bool transparent);

signals:
    void screenGeometryChanged();

private slots:
    void onScreenGeometryChanged(const QRect &geometry);

private:
    void applyX11Properties(QWindow *window);
    void applyWaylandProperties(QWindow *window);

    DockSettings *m_settings{nullptr};
    QScreen *m_currentScreen{nullptr};
    class QProcess *m_strutProcess{nullptr};
};
