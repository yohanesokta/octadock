#pragma once

#include <QObject>
#include <QString>
#include <QSettings>

class DockSettings : public QObject {
    Q_OBJECT

    Q_PROPERTY(int iconSize READ iconSize WRITE setIconSize NOTIFY iconSizeChanged)
    Q_PROPERTY(int iconSpacing READ iconSpacing WRITE setIconSpacing NOTIFY iconSpacingChanged)
    Q_PROPERTY(int verticalPadding READ verticalPadding WRITE setVerticalPadding NOTIFY verticalPaddingChanged)
    Q_PROPERTY(int horizontalPadding READ horizontalPadding WRITE setHorizontalPadding NOTIFY horizontalPaddingChanged)
    Q_PROPERTY(int dockHeight READ dockHeight WRITE setDockHeight NOTIFY dockHeightChanged)
    Q_PROPERTY(int bottomMargin READ bottomMargin WRITE setBottomMargin NOTIFY bottomMarginChanged)
    Q_PROPERTY(int cornerRadius READ cornerRadius WRITE setCornerRadius NOTIFY cornerRadiusChanged)
    Q_PROPERTY(qreal iconCornerRadiusRatio READ iconCornerRadiusRatio WRITE setIconCornerRadiusRatio NOTIFY iconCornerRadiusRatioChanged)
    Q_PROPERTY(qreal magnificationFactor READ magnificationFactor WRITE setMagnificationFactor NOTIFY magnificationFactorChanged)
    Q_PROPERTY(qreal magnificationRange READ magnificationRange WRITE setMagnificationRange NOTIFY magnificationRangeChanged)
    Q_PROPERTY(qreal backgroundOpacity READ backgroundOpacity WRITE setBackgroundOpacity NOTIFY backgroundOpacityChanged)
    Q_PROPERTY(QString backgroundTint READ backgroundTint WRITE setBackgroundTint NOTIFY backgroundTintChanged)
    Q_PROPERTY(qreal borderOpacity READ borderOpacity WRITE setBorderOpacity NOTIFY borderOpacityChanged)
    Q_PROPERTY(bool autoHide READ autoHide WRITE setAutoHide NOTIFY autoHideChanged)
    Q_PROPERTY(int autoHideDelay READ autoHideDelay WRITE setAutoHideDelay NOTIFY autoHideDelayChanged)
    Q_PROPERTY(bool alwaysOnTop READ alwaysOnTop WRITE setAlwaysOnTop NOTIFY alwaysOnTopChanged)
    Q_PROPERTY(bool showRunningIndicators READ showRunningIndicators WRITE setShowRunningIndicators NOTIFY showRunningIndicatorsChanged)
    Q_PROPERTY(bool enableMagnification READ enableMagnification WRITE setEnableMagnification NOTIFY enableMagnificationChanged)
    Q_PROPERTY(QString qpaPlatform READ qpaPlatform WRITE setQpaPlatform NOTIFY qpaPlatformChanged)

public:
    explicit DockSettings(QObject *parent = nullptr);

    int iconSize() const { return m_iconSize; }
    void setIconSize(int size);

    int iconSpacing() const { return m_iconSpacing; }
    void setIconSpacing(int spacing);

    int verticalPadding() const { return m_verticalPadding; }
    void setVerticalPadding(int padding);

    int horizontalPadding() const { return m_horizontalPadding; }
    void setHorizontalPadding(int padding);

    int dockHeight() const { return m_dockHeight; }
    void setDockHeight(int height);

    int bottomMargin() const { return m_bottomMargin; }
    void setBottomMargin(int margin);

    int cornerRadius() const { return m_cornerRadius; }
    void setCornerRadius(int radius);

    qreal iconCornerRadiusRatio() const { return m_iconCornerRadiusRatio; }
    void setIconCornerRadiusRatio(qreal ratio);

    qreal magnificationFactor() const { return m_magnificationFactor; }
    void setMagnificationFactor(qreal factor);

    qreal magnificationRange() const { return m_magnificationRange; }
    void setMagnificationRange(qreal range);

    qreal backgroundOpacity() const { return m_backgroundOpacity; }
    void setBackgroundOpacity(qreal opacity);

    QString backgroundTint() const { return m_backgroundTint; }
    void setBackgroundTint(const QString &tint);

    qreal borderOpacity() const { return m_borderOpacity; }
    void setBorderOpacity(qreal opacity);

    bool autoHide() const { return m_autoHide; }
    void setAutoHide(bool autoHide);

    int autoHideDelay() const { return m_autoHideDelay; }
    void setAutoHideDelay(int delay);

    bool alwaysOnTop() const { return m_alwaysOnTop; }
    void setAlwaysOnTop(bool onTop);

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
    void verticalPaddingChanged(int padding);
    void horizontalPaddingChanged(int padding);
    void dockHeightChanged(int height);
    void bottomMarginChanged(int margin);
    void cornerRadiusChanged(int radius);
    void iconCornerRadiusRatioChanged(qreal ratio);
    void magnificationFactorChanged(qreal factor);
    void magnificationRangeChanged(qreal range);
    void backgroundOpacityChanged(qreal opacity);
    void backgroundTintChanged(const QString &tint);
    void borderOpacityChanged(qreal opacity);
    void autoHideChanged(bool autoHide);
    void autoHideDelayChanged(int delay);
    void alwaysOnTopChanged(bool onTop);
    void showRunningIndicatorsChanged(bool show);
    void enableMagnificationChanged(bool enable);
    void qpaPlatformChanged(const QString &platform);

private:
    void recalculateDockHeight();

    int m_iconSize{58};
    int m_iconSpacing{18};
    int m_verticalPadding{12};
    int m_horizontalPadding{22};
    int m_dockHeight{84};
    int m_bottomMargin{6};
    int m_cornerRadius{24};
    qreal m_iconCornerRadiusRatio{0.225};
    qreal m_magnificationFactor{1.35};
    qreal m_magnificationRange{2.0};
    qreal m_backgroundOpacity{0.28};
    QString m_backgroundTint{"#dbe5f0"};
    qreal m_borderOpacity{0.32};
    bool m_autoHide{false};
    int m_autoHideDelay{500};
    bool m_alwaysOnTop{false};
    bool m_showRunningIndicators{true};
    bool m_enableMagnification{true};
    QString m_qpaPlatform{"xcb"};
};
