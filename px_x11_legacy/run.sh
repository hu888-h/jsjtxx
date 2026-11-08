#!/bin/bash
# 便捷启动：run.sh [视图编号 0-12]
cd "$(dirname "$0")"
[ -x build/px ] || { cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j; }
exec ./build/px ${1:+-v $1}
