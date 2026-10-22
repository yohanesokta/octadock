#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

echo "=== Building OctaDock ==="
echo "Project Root: ${PROJECT_ROOT}"

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
    echo "ERROR: Valid Qt 6 installation not found. Set QT_ROOT environment variable." >&2
    exit 1
fi

echo "Using Qt: ${QT_DIR}"
export PATH="${QT_DIR}/bin:${PATH}"

# Check CMake and Ninja
CMAKE_BIN="$(which cmake || true)"
if [ -z "${CMAKE_BIN}" ]; then
    echo "ERROR: cmake is not installed." >&2
    exit 1
fi

GENERATOR="Unix Makefiles"
if command -v ninja &>/dev/null; then
    GENERATOR="Ninja"
fi
echo "Using Generator: ${GENERATOR}"

BUILD_DIR="${PROJECT_ROOT}/build"
mkdir -p "${BUILD_DIR}"

echo "Configuring with CMake..."
cmake -S "${PROJECT_ROOT}" -B "${BUILD_DIR}" \
    -G "${GENERATOR}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="${QT_DIR}"

echo "Compiling..."
cmake --build "${BUILD_DIR}" -- -j"$(nproc)"

if [ -f "${BUILD_DIR}/octadock" ]; then
    echo "=== Build Succeeded! ==="
    echo "Binary: ${BUILD_DIR}/octadock"
else
    echo "ERROR: Build failed to produce octadock executable." >&2
    exit 1
fi
