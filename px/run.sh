#!/bin/bash
# 启动：run.sh [视图编号 0-10]
cd "$(dirname "$0")"
[ -x build/px ] || { cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j; }
# 无桌面环境需要 xcb 平台插件
export QT_QPA_PLATFORM=${QT_QPA_PLATFORM:-xcb}
exec ./build/px ${1:+-v $1}
