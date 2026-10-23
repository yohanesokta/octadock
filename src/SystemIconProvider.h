#pragma once

#include <QQuickImageProvider>
#include <QIcon>
#include <QPixmap>
#include <QHash>
#include <QStringList>

class SystemIconProvider : public QQuickImageProvider {
public:
    SystemIconProvider();
    ~SystemIconProvider() override = default;

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    QIcon lookupSystemIcon(const QString &id);
    QHash<QString, QStringList> m_aliases;
};
