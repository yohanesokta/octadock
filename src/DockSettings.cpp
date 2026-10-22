#include "DockSettings.h"
#include <QStandardPaths>
#include <QDir>

DockSettings::DockSettings(QObject *parent)
    : QObject(parent)
{
    load();
}

void DockSettings::setIconSize(int size) {
    size = qBound(32, size, 140);
    if (m_iconSize != size) {
        m_iconSize = size;
        emit iconSizeChanged(m_iconSize);
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
    m_iconSize = s.value("dock/iconSize", 64).toInt();
    m_iconSpacing = s.value("dock/iconSpacing", 14).toInt();
    m_dockHeight = s.value("dock/dockHeight", 92).toInt();
    m_bottomMargin = s.value("dock/bottomMargin", 11).toInt();
    m_cornerRadius = s.value("dock/cornerRadius", 28).toInt();
    m_magnificationFactor = s.value("dock/magnificationFactor", 1.36).toReal();
    m_magnificationRange = s.value("dock/magnificationRange", 2.0).toReal();
    m_backgroundOpacity = s.value("dock/backgroundOpacity", 0.25).toReal();
    m_backgroundTint = s.value("dock/backgroundTint", "#3a526d").toString();
    m_autoHide = s.value("dock/autoHide", false).toBool();
    m_autoHideDelay = s.value("dock/autoHideDelay", 500).toInt();
    m_showRunningIndicators = s.value("dock/showRunningIndicators", true).toBool();
    m_enableMagnification = s.value("dock/enableMagnification", true).toBool();
    m_qpaPlatform = s.value("dock/qpaPlatform", "xcb").toString();
}

void DockSettings::save() {
    QSettings s("OctaDock", "OctaDock");
    s.setValue("dock/iconSize", m_iconSize);
    s.setValue("dock/iconSpacing", m_iconSpacing);
    s.setValue("dock/dockHeight", m_dockHeight);
    s.setValue("dock/bottomMargin", m_bottomMargin);
    s.setValue("dock/cornerRadius", m_cornerRadius);
    s.setValue("dock/magnificationFactor", m_magnificationFactor);
    s.setValue("dock/magnificationRange", m_magnificationRange);
    s.setValue("dock/backgroundOpacity", m_backgroundOpacity);
    s.setValue("dock/backgroundTint", m_backgroundTint);
    s.setValue("dock/autoHide", m_autoHide);
    s.setValue("dock/autoHideDelay", m_autoHideDelay);
    s.setValue("dock/showRunningIndicators", m_showRunningIndicators);
    s.setValue("dock/enableMagnification", m_enableMagnification);
    s.setValue("dock/qpaPlatform", m_qpaPlatform);
}

void DockSettings::resetToDefaults() {
    setIconSize(64);
    setIconSpacing(14);
    setDockHeight(92);
    setBottomMargin(11);
    setCornerRadius(28);
    setMagnificationFactor(1.36);
    setMagnificationRange(2.0);
    setBackgroundOpacity(0.25);
    setBackgroundTint("#3a526d");
    setAutoHide(false);
    setAutoHideDelay(500);
    setShowRunningIndicators(true);
    setEnableMagnification(true);
    setQpaPlatform("xcb");
    save();
}
