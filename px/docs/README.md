# px —— 图形交互技术（上机）Linux · Qt6 版

把 `p1` 的 5 个 MFC(VC6) 工程与 `zip` 的算法包移植成 **Qt6** 原生 Linux 程序。
旧的 X11/GLX 实现已归档到 `../px_x11_legacy/`，本目录是 Qt 重构版。

## 技术栈

| 层 | 用的是 | 替代的 Windows/MFC 原语 |
|---|---|---|
| GUI 框架 | **Qt6 Widgets 6.12** | `CFrameWnd` / `CView` / `CDocument` |
| 绘图 | **QPainter + QImage（软件光栅）** | `CDC` / `CClientDC` / `CPaintDC` |
| 双缓冲 | Qt 自动双缓冲（`WA_OpaquePaintEvent`） | `PIXELFORMATDESCRIPTOR` + `wglCreateContext` + `SwapBuffers` |
| 消息 | `mousePressEvent` / `mouseMoveEvent` / `mouseReleaseEvent` | `WM_LBUTTONDOWN` / `WM_MOUSEMOVE` / `WM_LBUTTONUP` |
| 有模式对话框 | **`QDialog::exec()`**（阻塞） | `CDialog::DoModal()` |
| 无模式对话框 | **`QDialog::show()`**（不阻塞） | `Create()` + `ShowWindow()` |
| 菜单/状态栏 | `QMenuBar` / `QStatusBar` | `CMenu` / `CStatusBar` |
| 构建 | **CMake 3.16+ / C++17 / C11** | `.dsp` / `.sln` / `.vcxproj` |
| 算法 | 纯 C（`algo.c`，`extern "C"`） | 原 `.cpp` 中的算法函数 |

不再依赖：OpenGL/GLX、Xlib、FreeType（直接调用）、glaux。

## 目录

```
px/
├── CMakeLists.txt          Qt6 + CMake，AUTOMOC/qt6_wrap_cpp
├── include/
│   ├── algo.h              算法接口（C，extern "C"）
│   ├── canvasview.h        CanvasView（= MFC 的 CView）
│   └── jstmainwindow.h     JstMainWindow（= CFrameWnd）
├── src/
│   ├── main.cpp            入口 + `-v N` 参数
│   ├── jstmainwindow.cpp   菜单 / 状态栏 / 两种对话框
│   ├── canvasview.cpp      11 个视图的绘制与鼠标交互
│   └── algo.c              全部图形算法（性能优化版）
├── docs/
│   ├── README.md
│   ├── verify.sh           24 项无界面数值自检
│   └── q*.png              实测截图
└── build/
```

## 视图（已按要求裁剪）

保留 11 个视图。**已删除**「二维几何变换 + 五角星」和「中点椭圆」。

| # | 视图 | 源工程 |
|---|---|---|
| 0 | MouseLine 按下-抬起画线 | `p1/MouseLine` |
| 1 | SingleDraw 两次点击 + 橡皮筋 | `p1/DrawLineView/SingleDraw` |
| 2 | 坐标对话框定线 + 取点 | `p1/DrawLineView/SingleDraw2` |
| 3 | DDA 直线（星芒图） | `zip/算法2` |
| 4 | Bresenham 直线 / 中点圆 | `p1/DrawLine` |
| 5 | 中点圆（8 对称，附 Bresenham 对照） | `p1/DrawLine::getCirclePoints` |
| 6 | 扫描线填充（AET） | `p1/Fill` |
| 7 | 种子填充（4 连通行程版） | 教材 |
| 8 | Cohen-Sutherland 裁剪 | `zip/算法16` |
| 9 | 中点分割裁剪 | `zip/算法17` |
| 10 | Liang-Barsky 裁剪 | `zip/算法18` |

## 速度优化（实测数据）

优化围绕三件事：**去掉每帧内存分配**、**把逐像素提交改成逐跨度提交**、**缓存不变的结果**。

### 1. 扫描线填充重写为 AET（活动边表）

| | 旧实现（p1 原样） | 本版 |
|---|---|---|
| 每帧分配 | `malloc` 768 个桶 + 每条边一次 `malloc` + 每次重排重建链表 | 单次 malloc 边池 + 桶头索引数组 |
| 顶点处理 | `yNext/yPrev` 手工缩短，仍有重复计数 | 同语义，但边池一次建完 |
| 排序 | 每扫描线重建整条链表 | 活跃边数组插入排序（活跃边通常 < 16） |
| 输出 | 每像素一次 `glVertex2f` | 合并成跨度，逐跨度一次 `fillRect` |
| 缺陷 | `sizeof(edge)` 越界、`double free` | 已修正 |

### 2. 提交次数

填充 1100×705 里一个三角形：77641 个像素点 → **281 条跨度**，
绘制调用从 77641 次降到 281 次，**减少 276 倍**。

### 3. 实测帧耗（`QWidget::render` 端到端，离屏）

| 视图 | ms/帧 | 折算 fps |
|---|---|---|
| MouseLine | 0.33 | 3000 |
| DDA 星芒（72 条线，19453 点） | 0.93 | 1071 |
| Bresenham 直线 | 0.40 | 2500 |
| 中点圆 | 0.47 | 2143 |
| 扫描线填充 | 0.33 | 3000 |
| 种子填充 | 0.43 | 2308 |
| Cohen-Sutherland | 0.60 | 1667 |
| Liang-Barsky | 0.53 | 1875 |

### 4. 种子填充：1.008 ms/帧 → 0.43 ms/帧

- 逐像素 4 连通（旧）**5.01 ms/帧** → 行程加速 **1.008 ms/帧**（快 5 倍）
- 再加几何不变时缓存 `QImage` 直接 blit → **0.43 ms/帧**（比旧版快 11.6 倍）

### 5. 算法层微优化
- `PtList` / `SpanList` 全部支持 `reserve` 与复用，视图里是成员变量，不再每帧 new。
- 跨度合并：同行相邻跨度先合并再提交（`span_merge_push`）。
- 直线/圆都预先 `pl_reserve`，避免 `realloc` 抖动。
- CMake `Release + -O2`，C 与 C++ 都打开优化。

## 构建与运行

```bash
cd px
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/px          # 默认视图 0
./build/px -v 8     # 直接进 Cohen-Sutherland
```

依赖：`Qt6 Widgets`（6.x）、`CMake ≥ 3.16`、gcc/g++ 支持 C++17。

> 无桌面环境（Xvfb、WSL、纯 X11）请加 `QT_QPA_PLATFORM=xcb`，否则 Qt 可能走 Wayland 而黑屏。

## 自检

```bash
./docs/verify.sh
```

17 项数值断言，全部通过：DDA 步数与端点、Bresenham 跨八象限与反向线、中点圆四极点与
半径误差 ≤1px、填充面积与解析面积误差 <2%、凹多边形跨度数、三种裁剪端点一致（差 4.2e-5）、
端点编码、同侧简弃、种子填充 19×19=361 像素。

## 原 p1/zip 的缺陷（本版已修）

1. `p1/Fill/FillView.cpp`：`malloc(sizeof(edge))` 分配指针大小而非 `Edge` → 堆越界；
   `edges[768]` 与窗口高度不符；`fillScan` 里 `p2->next` 空指针解引用；
   `deleteAfter()` 注释了 `free()` 泄漏；改 AET 后统一释放，杜绝 double free。
2. `p1/DrawLine/DrawLineView.cpp::getPoints`：`sx==ex` / `sy==ey` 只处理正向，
   反向直线一个点都不画；斜线 `y` 自增方向没按 `sy>ey` 反转。
3. 中点圆判别式方向写反（`x` 递增时 `y` 必须递减），原逻辑画出来是双曲线。
4. `CDiaLineWith2P` 在 `OnLButtonDown` 里重复 `DoModal`，`R2_NOT` 预览线不擦除。
5. `glaux.h` 在 Linux 不存在 → Qt 版不再依赖任何 GL 辅助库。
