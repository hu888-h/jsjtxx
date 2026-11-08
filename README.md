# jsjtxx — 图形交互技术上机（MFC → Qt6 移植）

图形学/图形交互技术课程的上机作业集合。原始工程是 VC6 + MFC（`p1/`），
已移植到 Linux 原生 Qt6（`px/`），更早的实现保留在 `px_x11_legacy/`。

## 三条线

| 目录 | 说明 | 技术栈 |
|---|---|---|
| `p1/` | 原始 MFC 工程，5 个独立作业 | VC6 / MFC / CDC |
| `px/` | **主实现**，Qt6 重构版 | Qt6 Widgets + QPainter + CMake |
| `px_x11_legacy/` | 早期 X11/GLX 实现，已归档 | Xlib + GLX + FreeType |

## 移植对照（MFC → Qt6）

| 层 | MFC 原语 | Qt6 替代 |
|---|---|---|
| 主窗口 | `CFrameWnd` | `QMainWindow` |
| 视口 | `CView` / `CDC` | `CanvasView` / `QPainter` |
| 双缓冲 | `PIXELFORMATDESCRIPTOR` + `wglCreateContext` + `SwapBuffers` | Qt 自动双缓冲（`WA_OpaquePaintEvent`） |
| 鼠标消息 | `WM_LBUTTONDOWN` / `WM_MOUSEMOVE` | `mousePressEvent` / `mouseMoveEvent` |
| 有模式对话框 | `CDialog::DoModal()` | `QDialog::exec()` |
| 无模式对话框 | `Create()` + `ShowWindow()` | `QDialog::show()` |
| 菜单/状态栏 | `CMenu` / `CStatusBar` | `QMenuBar` / `QStatusBar` |
| 构建 | `.dsp` / `.sln` | CMake 3.16+ |

## 五个作业（`p1/`）

| 目录 | 内容 |
|---|---|
| `DrawLine/` | 直线算法：DDA、中点 Bresenham、圆/椭圆生成 |
| `MouseLine/` | 鼠标交互绘线，实时预览 |
| `DrawLineView/` | 双视图联动（`SingleDraw`, `SingleDraw2`） |
| `DLGExam/` | 对话框交互与参数输入 |
| `Fill/` | 多边形填充（扫描线/种子）与裁剪算法 |

算法核心都收敛在 `px/src/algo.c`（纯 C，`extern "C"` 导出），
GUI 只是外壳 —— 想单测算法可以绕开 Qt 直接编译这个文件。

## 构建

```bash
# Qt6 版（主实现）
cd px
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/px

# 早期 X11 版（需要 libX11 / GL / freetype）
cd px_x11_legacy
./run.sh
```

依赖：`qt6-base`、`cmake`、`gcc`。Qt6 版无 OpenGL 依赖，纯软件光栅。

## MFC 工程说明

`p1/` 里的 `.dsp` / `.clw`（ClassWizard 文件）是 VC6 时代的工程元数据，
保留了但**不能**在现代 MSVC 直接打开；需要的话用 VS 2019+ 的工程转换向导。
各项目下的 `Debug/`、`Release/`、`.pch`、`.ncb`、`.aps` 等构建产物已排除，
源码（`.cpp` / `.h` / `.rc` / `.dsp`）完整。

## 许可

课业代码，MIT。`px/docs/` 里的示意图为自绘。
