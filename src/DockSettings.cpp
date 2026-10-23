#include "DockSettings.h"
#include <QStandardPaths>
#include <QDir>

DockSettings::DockSettings(QObject *parent)
    : QObject(parent)
{
    load();
}

void DockSettings::recalculateDockHeight() {
    int target = m_iconSize + 2 * m_verticalPadding + 2;
    if (m_dockHeight != target) {
        m_dockHeight = target;
        emit dockHeightChanged(m_dockHeight);
    }
}

void DockSettings::setIconSize(int size) {
    size = qBound(32, size, 140);
    if (m_iconSize != size) {
        m_iconSize = size;
        emit iconSizeChanged(m_iconSize);
        recalculateDockHeight();
        save();
    }
}

void DockSettings::setIconSpacing(int spacing) {
    spacing = qBound(2, spacing, 40);
    if (m_iconSpacing != spacing) {
        m_iconSpacing = spacing;
        emit iconSpacingChanged(m_iconSpacing);
        save();
    }
}

void DockSettings::setVerticalPadding(int padding) {
    padding = qBound(4, padding, 30);
    if (m_verticalPadding != padding) {
        m_verticalPadding = padding;
        emit verticalPaddingChanged(m_verticalPadding);
        recalculateDockHeight();
        save();
    }
}

void DockSettings::setHorizontalPadding(int padding) {
    padding = qBound(6, padding, 50);
    if (m_horizontalPadding != padding) {
        m_horizontalPadding = padding;
        emit horizontalPaddingChanged(m_horizontalPadding);
        save();
    }
}

void DockSettings::setDockHeight(int height) {
    height = qBound(48, height, 200);
    if (m_dockHeight != height) {
        m_dockHeight = height;
        emit dockHeightChanged(m_dockHeight);
        save();
    }
}

void DockSettings::setBottomMargin(int margin) {
    margin = qBound(0, margin, 60);
    if (m_bottomMargin != margin) {
        m_bottomMargin = margin;
        emit bottomMarginChanged(m_bottomMargin);
        save();
    }
}

void DockSettings::setCornerRadius(int radius) {
    radius = qBound(8, radius, 50);
    if (m_cornerRadius != radius) {
        m_cornerRadius = radius;
        emit cornerRadiusChanged(m_cornerRadius);
        save();
    }
}

void DockSettings::setIconCornerRadiusRatio(qreal ratio) {
    ratio = qBound(0.1, ratio, 0.45);
    if (!qFuzzyCompare(m_iconCornerRadiusRatio, ratio)) {
        m_iconCornerRadiusRatio = ratio;
        emit iconCornerRadiusRatioChanged(m_iconCornerRadiusRatio);
        save();
    }
}

void DockSettings::setMagnificationFactor(qreal factor) {
    factor = qBound(1.0, factor, 2.2);
    if (!qFuzzyCompare(m_magnificationFactor, factor)) {
        m_magnificationFactor = factor;
        emit magnificationFactorChanged(m_magnificationFactor);
        save();
    }
}

void DockSettings::setMagnificationRange(qreal range) {
    range = qBound(1.0, range, 4.0);
    if (!qFuzzyCompare(m_magnificationRange, range)) {
        m_magnificationRange = range;
        emit magnificationRangeChanged(m_magnificationRange);
        save();
    }
}

void DockSettings::setBackgroundOpacity(qreal opacity) {
    opacity = qBound(0.05, opacity, 0.95);
    if (!qFuzzyCompare(m_backgroundOpacity, opacity)) {
        m_backgroundOpacity = opacity;
        emit backgroundOpacityChanged(m_backgroundOpacity);
        save();
    }
}

void DockSettings::setBackgroundTint(const QString &tint) {
    if (m_backgroundTint != tint) {
        m_backgroundTint = tint;
        emit backgroundTintChanged(m_backgroundTint);
        save();
    }
}

void DockSettings::setBorderOpacity(qreal opacity) {
    opacity = qBound(0.05, opacity, 1.0);
    if (!qFuzzyCompare(m_borderOpacity, opacity)) {
        m_borderOpacity = opacity;
        emit borderOpacityChanged(m_borderOpacity);
        save();
    }
}

void DockSettings::setAutoHide(bool autoHide) {
    if (m_autoHide != autoHide) {
        m_autoHide = autoHide;
        emit autoHideChanged(m_autoHide);
        save();
    }
}

void DockSettings::setAutoHideDelay(int delay) {
    delay = qBound(100, delay, 3000);
    if (m_autoHideDelay != delay) {
        m_autoHideDelay = delay;
        emit autoHideDelayChanged(m_autoHideDelay);
        save();
    }
}

void DockSettings::setAlwaysOnTop(bool onTop) {
    if (m_alwaysOnTop != onTop) {
        m_alwaysOnTop = onTop;
        emit alwaysOnTopChanged(m_alwaysOnTop);
        save();
    }
}

void DockSettings::setShowRunningIndicators(bool show) {
    if (m_showRunningIndicators != show) {
        m_showRunningIndicators = show;
        emit showRunningIndicatorsChanged(m_showRunningIndicators);
        save();
    }
}

void DockSettings::setEnableMagnification(bool enable) {
    if (m_enableMagnification != enable) {
        m_enableMagnification = enable;
        emit enableMagnificationChanged(m_enableMagnification);
        save();
    }
}

void DockSettings::setQpaPlatform(const QString &platform) {
    if (m_qpaPlatform != platform) {
        m_qpaPlatform = platform;
        emit qpaPlatformChanged(m_qpaPlatform);
        save();
    }
}

void DockSettings::load() {
    QSettings s("OctaDock", "OctaDock");
    m_iconSize = s.value("dock/iconSize", 58).toInt();
    m_iconSpacing = s.value("dock/iconSpacing", 18).toInt();
    m_verticalPadding = s.value("dock/verticalPadding", 12).toInt();
    m_horizontalPadding = s.value("dock/horizontalPadding", 22).toInt();
    m_dockHeight = s.value("dock/dockHeight", 84).toInt();
    m_bottomMargin = s.value("dock/bottomMargin", 6).toInt();
    m_cornerRadius = s.value("dock/cornerRadius", 24).toInt();
    m_iconCornerRadiusRatio = s.value("dock/iconCornerRadiusRatio", 0.225).toReal();
    m_magnificationFactor = s.value("dock/magnificationFactor", 1.35).toReal();
    m_magnificationRange = s.value("dock/magnificationRange", 2.0).toReal();
    m_backgroundOpacity = s.value("dock/backgroundOpacity", 0.28).toReal();
    m_backgroundTint = s.value("dock/backgroundTint", "#dbe5f0").toString();
    m_borderOpacity = s.value("dock/borderOpacity", 0.32).toReal();
    m_autoHide = s.value("dock/autoHide", false).toBool();
    m_autoHideDelay = s.value("dock/autoHideDelay", 500).toInt();
    m_alwaysOnTop = s.value("dock/alwaysOnTop", false).toBool();
    m_showRunningIndicators = s.value("dock/showRunningIndicators", true).toBool();
    m_enableMagnification = s.value("dock/enableMagnification", true).toBool();
    m_qpaPlatform = s.value("dock/qpaPlatform", "xcb").toString();
}

void DockSettings::save() {
    QSettings s("OctaDock", "OctaDock");
    s.setValue("dock/iconSize", m_iconSize);
    s.setValue("dock/iconSpacing", m_iconSpacing);
    s.setValue("dock/verticalPadding", m_verticalPadding);
    s.setValue("dock/horizontalPadding", m_horizontalPadding);
    s.setValue("dock/dockHeight", m_dockHeight);
    s.setValue("dock/bottomMargin", m_bottomMargin);
    s.setValue("dock/cornerRadius", m_cornerRadius);
    s.setValue("dock/iconCornerRadiusRatio", m_iconCornerRadiusRatio);
    s.setValue("dock/magnificationFactor", m_magnificationFactor);
    s.setValue("dock/magnificationRange", m_magnificationRange);
    s.setValue("dock/backgroundOpacity", m_backgroundOpacity);
    s.setValue("dock/backgroundTint", m_backgroundTint);
    s.setValue("dock/borderOpacity", m_borderOpacity);
    s.setValue("dock/autoHide", m_autoHide);
    s.setValue("dock/autoHideDelay", m_autoHideDelay);
    s.setValue("dock/alwaysOnTop", m_alwaysOnTop);
    s.setValue("dock/showRunningIndicators", m_showRunningIndicators);
    s.setValue("dock/enableMagnification", m_enableMagnification);
    s.setValue("dock/qpaPlatform", m_qpaPlatform);
}

void DockSettings::resetToDefaults() {
    m_iconSize = 58;
    m_iconSpacing = 18;
    m_verticalPadding = 12;
    m_horizontalPadding = 22;
    m_dockHeight = 84;
    m_bottomMargin = 6;
    m_cornerRadius = 24;
    m_iconCornerRadiusRatio = 0.225;
    m_magnificationFactor = 1.35;
    m_magnificationRange = 2.0;
    m_backgroundOpacity = 0.28;
    m_backgroundTint = "#dbe5f0";
    m_borderOpacity = 0.32;
    m_autoHide = false;
    m_autoHideDelay = 500;
    m_alwaysOnTop = false;
    m_showRunningIndicators = true;
    m_enableMagnification = true;
    m_qpaPlatform = "xcb";

    emit iconSizeChanged(m_iconSize);
    emit iconSpacingChanged(m_iconSpacing);
    emit verticalPaddingChanged(m_verticalPadding);
    emit horizontalPaddingChanged(m_horizontalPadding);
    emit dockHeightChanged(m_dockHeight);
    emit bottomMarginChanged(m_bottomMargin);
    emit cornerRadiusChanged(m_cornerRadius);
    emit iconCornerRadiusRatioChanged(m_iconCornerRadiusRatio);
    emit magnificationFactorChanged(m_magnificationFactor);
    emit magnificationRangeChanged(m_magnificationRange);
    emit backgroundOpacityChanged(m_backgroundOpacity);
    emit backgroundTintChanged(m_backgroundTint);
    emit borderOpacityChanged(m_borderOpacity);
    emit autoHideChanged(m_autoHide);
    emit autoHideDelayChanged(m_autoHideDelay);
    emit showRunningIndicatorsChanged(m_showRunningIndicators);
    emit enableMagnificationChanged(m_enableMagnification);
    emit qpaPlatformChanged(m_qpaPlatform);

    save();
}
