#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

echo "=== Packaging OctaDock Portable Bundle ==="

# Discover Qt 6 Installation
QT_DIR="${QT_ROOT:-}"
if [ -z "${QT_DIR}" ]; then
    POSSIBLE_QT_PATHS=(
        "/home/yohanesokta/Qt/6.11.1/gcc_64"
        "/home/yohanesokta/Qt/6.*/gcc_64"
        "/opt/Qt/6.*/gcc_64"
    )
    for p in ${POSSIBLE_QT_PATHS[@]}; do
        if [ -d "${p}" ] && [ -f "${p}/bin/qmake" ]; then
            QT_DIR="${p}"
            break
        fi
    done
fi

if [ -z "${QT_DIR}" ] || [ ! -d "${QT_DIR}" ]; then
    echo "ERROR: Valid Qt 6 installation not found." >&2
    exit 1
fi

echo "Source Qt Directory: ${QT_DIR}"

# 1. Build application if needed
BIN="${PROJECT_ROOT}/build/octadock"
if [ ! -f "${BIN}" ]; then
    echo "Building executable first..."
    "${SCRIPT_DIR}/build.sh"
fi

# 2. Setup AppDir structure
APPDIR="${PROJECT_ROOT}/dist/OctaDock.AppDir"
echo "Target AppDir: ${APPDIR}"
rm -rf "${APPDIR}"
mkdir -p "${APPDIR}/usr/bin"
mkdir -p "${APPDIR}/usr/lib"
mkdir -p "${APPDIR}/usr/plugins/platforms"
mkdir -p "${APPDIR}/usr/plugins/xcbglintegrations"
mkdir -p "${APPDIR}/usr/plugins/wayland-graphics-integration-client"
mkdir -p "${APPDIR}/usr/plugins/wayland-shell-integration"
mkdir -p "${APPDIR}/usr/plugins/wayland-decoration-client"
mkdir -p "${APPDIR}/usr/plugins/imageformats"
mkdir -p "${APPDIR}/usr/plugins/tls"
mkdir -p "${APPDIR}/usr/qml"
mkdir -p "${APPDIR}/usr/share/applications"
mkdir -p "${APPDIR}/usr/share/icons/hicolor/scalable/apps"

# 3. Copy application binary
cp -a "${BIN}" "${APPDIR}/usr/bin/octadock"
STRUT_BIN="${PROJECT_ROOT}/build/octadock-strut"
if [ -f "${STRUT_BIN}" ]; then
    cp -a "${STRUT_BIN}" "${APPDIR}/usr/bin/octadock-strut"
fi

# Bundle gtk-layer-shell library
find /usr/lib/x86_64-linux-gnu -name "libgtk-layer-shell.so*" -exec cp -a {} "${APPDIR}/usr/lib/" \; 2>/dev/null || true

# 4. Copy Qt libraries
echo "Bundling Qt shared libraries..."
QT_LIBS=(
    "libQt6Core.so"
    "libQt6Gui.so"
    "libQt6Quick.so"
    "libQt6Qml.so"
    "libQt6QmlMeta.so"
    "libQt6QmlModels.so"
    "libQt6QmlWorkerScript.so"
    "libQt6Network.so"
    "libQt6OpenGL.so"
    "libQt6DBus.so"
    "libQt6Svg.so"
    "libQt6Widgets.so"
    "libQt6XcbQpa.so"
    "libQt6WaylandClient.so"
    "libQt6WlShellIntegration.so"
    "libicui18n.so"
    "libicuuc.so"
    "libicudata.so"
)

for lib in "${QT_LIBS[@]}"; do
    find "${QT_DIR}/lib" -maxdepth 1 -name "${lib}*" -exec cp -a {} "${APPDIR}/usr/lib/" \;
done

# 5. Copy Qt plugins
echo "Bundling Qt plugins..."
cp -a "${QT_DIR}/plugins/platforms/libqxcb.so" "${APPDIR}/usr/plugins/platforms/" 2>/dev/null || true
cp -a "${QT_DIR}/plugins/platforms/libqwayland.so" "${APPDIR}/usr/plugins/platforms/" 2>/dev/null || true

if [ -d "${QT_DIR}/plugins/xcbglintegrations" ]; then
    cp -a "${QT_DIR}/plugins/xcbglintegrations"/* "${APPDIR}/usr/plugins/xcbglintegrations/" 2>/dev/null || true
fi

if [ -d "${QT_DIR}/plugins/wayland-graphics-integration-client" ]; then
    cp -a "${QT_DIR}/plugins/wayland-graphics-integration-client"/* "${APPDIR}/usr/plugins/wayland-graphics-integration-client/" 2>/dev/null || true
fi

if [ -d "${QT_DIR}/plugins/wayland-shell-integration" ]; then
    cp -a "${QT_DIR}/plugins/wayland-shell-integration"/* "${APPDIR}/usr/plugins/wayland-shell-integration/" 2>/dev/null || true
fi

if [ -d "${QT_DIR}/plugins/wayland-decoration-client" ]; then
    cp -a "${QT_DIR}/plugins/wayland-decoration-client"/* "${APPDIR}/usr/plugins/wayland-decoration-client/" 2>/dev/null || true
fi

if [ -d "${QT_DIR}/plugins/imageformats" ]; then
    cp -a "${QT_DIR}/plugins/imageformats"/* "${APPDIR}/usr/plugins/imageformats/" 2>/dev/null || true
fi

if [ -d "${QT_DIR}/plugins/tls" ]; then
    cp -a "${QT_DIR}/plugins/tls"/* "${APPDIR}/usr/plugins/tls/" 2>/dev/null || true
fi

# 6. Copy Qt Quick/QML modules
echo "Bundling required QML modules..."
QML_MODULES=(
    "QtQuick"
    "QtQml"
    "Qt"
)

for mod in "${QML_MODULES[@]}"; do
    if [ -d "${QT_DIR}/qml/${mod}" ]; then
        cp -a "${QT_DIR}/qml/${mod}" "${APPDIR}/usr/qml/"
    fi
done

# 7. Create AppRun script
echo "Creating AppRun script..."
cat << 'EOF' > "${APPDIR}/AppRun"
#!/bin/sh
SELF=$(readlink -f "$0")
APPDIR=$(dirname "$SELF")

export PATH="${APPDIR}/usr/bin:${PATH}"
export LD_LIBRARY_PATH="${APPDIR}/usr/lib:${LD_LIBRARY_PATH:-}"
export QT_PLUGIN_PATH="${APPDIR}/usr/plugins"
export QML2_IMPORT_PATH="${APPDIR}/usr/qml"
export QT_QPA_PLATFORM_PLUGIN_PATH="${APPDIR}/usr/plugins/platforms"

# If QT_QPA_PLATFORM is unset and running on Wayland with XWayland available,
# default to xcb for flawless bottom-centered placement and dock window rules.
if [ -z "${QT_QPA_PLATFORM}" ] && [ "${XDG_SESSION_TYPE}" = "wayland" ] && [ -n "${DISPLAY}" ]; then
    export QT_QPA_PLATFORM="xcb"
fi

exec "${APPDIR}/usr/bin/octadock" "$@"
EOF
chmod +x "${APPDIR}/AppRun"

# 8. Create Desktop entry
cat << 'EOF' > "${APPDIR}/OctaDock.desktop"
[Desktop Entry]
Type=Application
Name=OctaDock
Comment=macOS-like Dock for Linux Desktop
Exec=octadock
Icon=octadock
Categories=Utility;DesktopSettings;
Terminal=false
StartupNotify=true
EOF
cp "${APPDIR}/OctaDock.desktop" "${APPDIR}/usr/share/applications/OctaDock.desktop"

# 9. Copy Application Icon
cp "${PROJECT_ROOT}/assets/icons/octadock.svg" "${APPDIR}/octadock.svg"
cp "${PROJECT_ROOT}/assets/icons/octadock.svg" "${APPDIR}/.DirIcon"
cp "${PROJECT_ROOT}/assets/icons/octadock.svg" "${APPDIR}/usr/share/icons/hicolor/scalable/apps/octadock.svg"

# 10. Copy License and Notice
if [ -f "${PROJECT_ROOT}/LICENSE" ]; then
    cp "${PROJECT_ROOT}/LICENSE" "${APPDIR}/LICENSE"
fi

echo "AppDir successfully assembled at: ${APPDIR}"

# 11. Optionally create AppImage if appimagetool is available
if command -v appimagetool &>/dev/null; then
    echo "Creating AppImage using appimagetool..."
    APPIMAGE_OUT="${PROJECT_ROOT}/dist/OctaDock-x86_64.AppImage"
    rm -f "${APPIMAGE_OUT}"
    ARCH=x86_64 appimagetool "${APPDIR}" "${APPIMAGE_OUT}"
    if [ -f "${APPIMAGE_OUT}" ]; then
        chmod +x "${APPIMAGE_OUT}"
        echo "=== AppImage Succeeded! ==="
        echo "Created: ${APPIMAGE_OUT}"
    fi
else
    echo "Notice: appimagetool not found. AppDir is fully functional and ready to run via ${APPDIR}/AppRun"
fi

echo "=== Packaging Complete ==="
