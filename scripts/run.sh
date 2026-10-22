#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

# Discover Qt 6 Installation
QT_DIR="${QT_ROOT:-/home/yohanesokta/Qt/6.11.1/gcc_64}"

MODE="${1:-dev}"

case "${MODE}" in
    --appimage|appimage)
        APPIMAGE="${PROJECT_ROOT}/dist/OctaDock-x86_64.AppImage"
        if [ ! -f "${APPIMAGE}" ]; then
            echo "AppImage not found at ${APPIMAGE}. Run scripts/package.sh first." >&2
            exit 1
        fi
        echo "Running OctaDock AppImage..."
        exec "${APPIMAGE}" "$@"
        ;;

    --appdir|appdir)
        APPDIR="${PROJECT_ROOT}/dist/OctaDock.AppDir"
        if [ ! -f "${APPDIR}/AppRun" ]; then
            echo "AppDir not found at ${APPDIR}. Run scripts/package.sh first." >&2
            exit 1
        fi
        echo "Running OctaDock from AppDir..."
        exec "${APPDIR}/AppRun" "$@"
        ;;

    *)
        BIN="${PROJECT_ROOT}/build/octadock"
        if [ ! -f "${BIN}" ]; then
            echo "Executable not found at ${BIN}. Building first..."
            "${SCRIPT_DIR}/build.sh"
        fi

        # Setup runtime environment for development build
        if [ -d "${QT_DIR}" ]; then
            export LD_LIBRARY_PATH="${QT_DIR}/lib:${LD_LIBRARY_PATH:-}"
            export QT_PLUGIN_PATH="${QT_DIR}/plugins:${QT_PLUGIN_PATH:-}"
            export QML2_IMPORT_PATH="${QT_DIR}/qml:${QML2_IMPORT_PATH:-}"
        fi

        # On Wayland, default to xcb (via XWayland) if unset, providing reliable bottom dock placement
        if [ -z "${QT_QPA_PLATFORM:-}" ] && [ "${XDG_SESSION_TYPE:-}" = "wayland" ] && [ -n "${DISPLAY:-}" ]; then
            export QT_QPA_PLATFORM="xcb"
        fi

        echo "Running development build: ${BIN}"
        exec "${BIN}" "$@"
        ;;
esac
