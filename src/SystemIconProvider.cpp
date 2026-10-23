#include "SystemIconProvider.h"
#include <QFileInfo>
#include <QPainter>
#include <QDebug>

SystemIconProvider::SystemIconProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
    // Aliases to resolve macOS dock names to Linux desktop system icon theme names
    m_aliases["finder"] = {"org.kde.dolphin", "system-file-manager", "dolphin", "folder"};
    m_aliases["launchpad"] = {"krunner", "org.kde.krunner", "start-here-kde", "view-app-grid"};
    m_aliases["safari"] = {"safari", "internet-web-browser", "firefox", "org.mozilla.firefox", "microsoft-edge", "google-chrome"};
    m_aliases["mail"] = {"kmail", "org.kde.kmail2", "internet-mail", "thunderbird"};
    m_aliases["facetime"] = {"kamoso", "org.kde.kamoso", "camera-web", "cheese"};
    m_aliases["messages"] = {"telegram-desktop", "internet-chat", "messages"};
    m_aliases["maps"] = {"maps", "marble", "org.kde.marble"};
    m_aliases["photos"] = {"photos", "gwenview", "org.kde.gwenview", "image-viewer"};
    m_aliases["contacts"] = {"contacts", "kaddressbook", "org.kde.kaddressbook"};
    m_aliases["settings"] = {"preferences-system", "systemsettings", "org.kde.systemsettings"};
}

QIcon SystemIconProvider::lookupSystemIcon(const QString &rawId) {
    QString id = rawId;

    // Strip qrc:/ or file paths if present
    if (id.startsWith("qrc:/assets/icons/")) {
        id = id.mid(18);
    }
    if (id.endsWith(".svg")) {
        id.chop(4);
    }
    if (id.contains('?')) {
        id = id.left(id.indexOf('?'));
    }

    // 1. Try aliases first if it matches standard dock IDs
    if (m_aliases.contains(id)) {
        for (const QString &cand : m_aliases.value(id)) {
            QIcon ic = QIcon::fromTheme(cand);
            if (!ic.isNull()) {
                return ic;
            }
        }
    }

    // 2. Direct lookup in system theme
    QIcon directIcon = QIcon::fromTheme(id);
    if (!directIcon.isNull()) {
        return directIcon;
    }

    // 3. Try lowercase variant
    QString lowerId = id.toLower();
    if (lowerId != id) {
        QIcon lowerIcon = QIcon::fromTheme(lowerId);
        if (!lowerIcon.isNull()) {
            return lowerIcon;
        }
    }

    // 4. Fallback to local SVG asset
    QString assetPath = QString(":/assets/icons/%1.svg").arg(id);
    if (QFileInfo::exists(assetPath)) {
        return QIcon(assetPath);
    }

    // 5. General generic fallback
    return QIcon::fromTheme("application-x-executable", QIcon(":/assets/icons/finder.svg"));
}

QPixmap SystemIconProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) {
    int w = requestedSize.width() > 0 ? requestedSize.width() : 128;
    int h = requestedSize.height() > 0 ? requestedSize.height() : 128;

    QIcon icon = lookupSystemIcon(id);
    QPixmap pixmap = icon.pixmap(w, h);

    if (pixmap.isNull()) {
        // Create clean fallback icon
        pixmap = QPixmap(w, h);
        pixmap.fill(Qt::transparent);
    }

    if (size) {
        *size = pixmap.size();
    }
    return pixmap;
}
