#pragma once

#include <QObject>
#include <QString>
#include <QSettings>

class DockSettings : public QObject {
    Q_OBJECT

    Q_PROPERTY(int iconSize READ iconSize WRITE setIconSize NOTIFY iconSizeChanged)
    Q_PROPERTY(int iconSpacing READ iconSpacing WRITE setIconSpacing NOTIFY iconSpacingChanged)
    Q_PROPERTY(int dockHeight READ dockHeight WRITE setDockHeight NOTIFY dockHeightChanged)
    Q_PROPERTY(int bottomMargin READ bottomMargin WRITE setBottomMargin NOTIFY bottomMarginChanged)
    Q_PROPERTY(int cornerRadius READ cornerRadius WRITE setCornerRadius NOTIFY cornerRadiusChanged)
    Q_PROPERTY(qreal magnificationFactor READ magnificationFactor WRITE setMagnificationFactor NOTIFY magnificationFactorChanged)
    Q_PROPERTY(qreal magnificationRange READ magnificationRange WRITE setMagnificationRange NOTIFY magnificationRangeChanged)
    Q_PROPERTY(qreal backgroundOpacity READ backgroundOpacity WRITE setBackgroundOpacity NOTIFY backgroundOpacityChanged)
    Q_PROPERTY(QString backgroundTint READ backgroundTint WRITE setBackgroundTint NOTIFY backgroundTintChanged)
    Q_PROPERTY(bool autoHide READ autoHide WRITE setAutoHide NOTIFY autoHideChanged)
    Q_PROPERTY(int autoHideDelay READ autoHideDelay WRITE setAutoHideDelay NOTIFY autoHideDelayChanged)
    Q_PROPERTY(bool showRunningIndicators READ showRunningIndicators WRITE setShowRunningIndicators NOTIFY showRunningIndicatorsChanged)
    Q_PROPERTY(bool enableMagnification READ enableMagnification WRITE setEnableMagnification NOTIFY enableMagnificationChanged)
    Q_PROPERTY(QString qpaPlatform READ qpaPlatform WRITE setQpaPlatform NOTIFY qpaPlatformChanged)

public:
    explicit DockSettings(QObject *parent = nullptr);

    int iconSize() const { return m_iconSize; }
    void setIconSize(int size);

    int iconSpacing() const { return m_iconSpacing; }
    void setIconSpacing(int spacing);

    int dockHeight() const { return m_dockHeight; }
    void setDockHeight(int height);

    int bottomMargin() const { return m_bottomMargin; }
    void setBottomMargin(int margin);

    int cornerRadius() const { return m_cornerRadius; }
    void setCornerRadius(int radius);

    qreal magnificationFactor() const { return m_magnificationFactor; }
    void setMagnificationFactor(qreal factor);

    qreal magnificationRange() const { return m_magnificationRange; }
    void setMagnificationRange(qreal range);

    qreal backgroundOpacity() const { return m_backgroundOpacity; }
    void setBackgroundOpacity(qreal opacity);

    QString backgroundTint() const { return m_backgroundTint; }
    void setBackgroundTint(const QString &tint);

    bool autoHide() const { return m_autoHide; }
    void setAutoHide(bool autoHide);

    int autoHideDelay() const { return m_autoHideDelay; }
    void setAutoHideDelay(int delay);

    bool showRunningIndicators() const { return m_showRunningIndicators; }
    void setShowRunningIndicators(bool show);

    bool enableMagnification() const { return m_enableMagnification; }
    void setEnableMagnification(bool enable);

    QString qpaPlatform() const { return m_qpaPlatform; }
    void setQpaPlatform(const QString &platform);

    Q_INVOKABLE void load();
    Q_INVOKABLE void save();
    Q_INVOKABLE void resetToDefaults();

signals:
    void iconSizeChanged(int size);
    void iconSpacingChanged(int spacing);
    void dockHeightChanged(int height);
    void bottomMarginChanged(int margin);
    void cornerRadiusChanged(int radius);
    void magnificationFactorChanged(qreal factor);
    void magnificationRangeChanged(qreal range);
    void backgroundOpacityChanged(qreal opacity);
    void backgroundTintChanged(const QString &tint);
    void autoHideChanged(bool autoHide);
    void autoHideDelayChanged(int delay);
    void showRunningIndicatorsChanged(bool show);
    void enableMagnificationChanged(bool enable);
    void qpaPlatformChanged(const QString &platform);

private:
    int m_iconSize{64};
    int m_iconSpacing{14};
    int m_dockHeight{92};
    int m_bottomMargin{11};
    int m_cornerRadius{28};
    qreal m_magnificationFactor{1.36};
    qreal m_magnificationRange{2.0};
    qreal m_backgroundOpacity{0.25};
    QString m_backgroundTint{"#3a526d"};
    bool m_autoHide{false};
    int m_autoHideDelay{500};
    bool m_showRunningIndicators{true};
    bool m_enableMagnification{true};
    QString m_qpaPlatform{"xcb"};
};
