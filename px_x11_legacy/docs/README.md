# px —— 图形交互技术（上机）Linux 版

`p1` 里是 5 个 Visual C++ 6.0 / MFC 工程（Windows 专有：`CDC`、`WM_LBUTTONDOWN`、
`PIXELFORMATDESCRIPTOR`、`CDialog::DoModal` 等），`zip` 里是另外 4 套 VS 工程算法。
本目录把这些代码**逐条按原实现移植成 Linux 原生程序**，并补全了原工程的缺漏。

## 与 PDF 的对应关系

| PDF 章节 | Windows 原语 | 本程序实现 |
|---|---|---|
| 屏幕区 / 窗口区 / 客户区 | `CFrameWnd` + `CView` | `App` + `app_draw_area()` |
| GDI 类（CBitmap/CBrush/CFont/CPen） | GDI 对象 | `Color` + `Canvas` + FreeType |
| CDC 类（CClientDC/CWindowDC/CPaintDC） | 设备描述表 | `Canvas`（32 位 ARGB 软件光栅） |
| `WM_LBUTTONDOWN` / `WM_LBUTTONUP` | 消息映射 | `ButtonPress` / `ButtonRelease(Button1)` |
| `WM_MOUSEMOVE` | 消息映射 | `MotionNotify` |
| `WM_SIZE` | 消息映射 | `ConfigureNotify` |
| 有模式对话框 | `CDialog::DoModal` | `dlg_run()` 嵌套事件循环 |
| 无模式对话框 | `Create()` + `ShowWindow()` | `dlg_show()` + 主循环分发 |
| 菜单 / 工具栏 / 状态栏 | `CMenu` / `CToolBar` / `CStatusBar` | `ui_menubar_*` / `ui_statusbar` |
| `PIXELFORMATDESCRIPTOR` + `ChoosePixelFormat` | GDI/OpenGL 桥 | `glXChooseVisual` |
| `wglCreateContext` / `HGLRC` | OpenGL RC | `glXCreateContext` |
| `SwapBuffers(pDC->m_hDC)` | 双缓冲交换 | `app_present()`（整帧纹理 + `glXSwapBuffers`） |
| `glViewport(0,0,w,h)` | 视口 | 同上 |

## 目录

```
px/
├── CMakeLists.txt
├── include/
│   ├── px.h       窗口/画布/菜单/对话框 接口
│   ├── algo.h     图形算法接口（直线、圆、填充、裁剪、变换）
│   └── view.h     视图（对应 p1 各 CView 子类）
├── src/
│   ├── main.c     入口 + 菜单命令 + 事件分发
│   ├── app.c      X11 + GLX 窗口与消息循环
│   ├── canvas.c   CDC 等价的光栅画布
│   ├── text.c     FreeType 文本（中文菜单/对话框）
│   ├── ui.c       菜单栏 / 状态栏 / 对话框
│   ├── algo.c     DDA、Bresenham、中点圆/椭圆、三种裁剪、二维变换
│   ├── fill.c     扫描线填充（ET/AET）+ 种子填充
│   └── view.c     13 个视图的绘制与鼠标交互
└── docs/          说明与实测截图
```

## 算法与源码对照

| 视图 | 源工程 | 原文件 |
|---|---|---|
| 1 MouseLine | `p1/MouseLine` | `MouseLineView.cpp`（`OnLButtonDown`/`OnLButtonUp`） |
| 2 SingleDraw | `p1/DrawLineView/SingleDraw` | `SingleDrawView.cpp`（`SetROP2(R2_NOT)` 橡皮筋 + `linkList<CLine>`） |
| 3 坐标对话框定线 | `p1/DrawLineView/SingleDraw2` | `SingleDrawView.cpp` + `DiaLineWith2P.cpp` |
| 4 DDA | `zip/算法2：通用直线的DDA算法` | `TestView.cpp::DDALine` |
| 5 Bresenham + 中点圆 | `p1/DrawLine` | `DrawLineView.cpp::getPoints` / `getCirclePoints` |
| 6 中点圆 / 椭圆 | 同上 + 教材椭圆算法 | `getCirclePoints` 推广 |
| 7 扫描线填充 | `p1/Fill` | `FillView.cpp::buildEdgeList/makeEdgeRec/fillScan/scanFill` |
| 8 种子填充 | 教材 4 连通泛洪 | — |
| 9 Cohen-Sutherland | `zip/算法16` | `TestView.cpp::Encode/CohenSutherlandClip` |
| 10 中点分割 | `zip/算法17` | `TestView.cpp::MidpointClip` |
| 11 Liang-Barsky | `zip/算法18` | `TestView.cpp` 参数化裁剪 |
| 12 二维变换 + 五角星 | `zip/算法15` | `Transform2.cpp`（3×3 齐次矩阵）+ `Pentagram.cpp` |
| 13 有/无模式对话框 | `p1/DLGExam` | `DLGExamView.cpp::OnModel/OnNomodel` |

## 原工程的缺陷与修正

1. `p1/Fill/FillView.cpp`：
   - `malloc(sizeof(edge))` 分配的是**指针大小**而非 `sizeof(Edge)`，必然堆越界 → 改为 `sizeof(Edge)`。
   - `Edge *edges[WINDOW_HEIGHT]` 固定 768 行，与窗口实际高度不符 → 改为按多边形包围盒动态分配。
   - `fillScan` 里 `p1 = p2->next` 在 `p2 == NULL` 时解引用 → 成对遍历改为先判空。
   - `deleteAfter()` 注释掉了 `free(p)`，泄漏 → 改为过顶边即释放，且 ET 桶节点所有权转移后在 AET 统一释放（避免 double free）。
   - `memset(window, 0, ...)` 后再 `scanFill`，但 `OnDraw` 里逐点画 1024×768 个点，坐标未做 y 翻转 → 改为设备坐标 + 视口居中映射。
2. `p1/DrawLine/DrawLineView.cpp`：
   - `getPoints` 在 `sx==ex` 与 `sy==ey` 分支只处理 `sy<=ey`，反向直线会一个点都不画 → 补齐两端比较。
   - 斜线分支只在 `dx>=dy` 时把 p 累加 `twody`，`y` 的自增方向没有按 `sy>ey` 反转 → 增加 `y0>y1` 时的递减。
3. `p1/DrawLineView/SingleDraw2/SingleDrawView.cpp`：`dlg.DoModal()` 在 `OnLButtonDown` 内被重复调用
   且 `R2_NOT` 预览线没在第二点处擦除，会残留 → 预览线改为每帧重绘。
4. `p1/Fill/Trigle.cpp`：`CTrigle` 没有 `OnOK` 覆写，`DoModal()==IDOK` 分支依赖默认值 → 补 DDX 校验。
5. 所有工程：`#include <gl\glaux.h>` 在 Linux 不存在（`glaux` 是 Windows 专用辅助库）→
   本版不依赖 glaux，投影/视口全用 `glOrtho` + `glViewport` 自行设置。

## 构建

```bash
cd px
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/px              # 默认 1100x760
./build/px -v 9         # 直接以某个视图启动
```

依赖：`x11`、`gl`(GLX)、`freetype2`、`m`。字体优先 `/usr/share/fonts/noto-cjk/NotoSansCJK-Regular.ttc`。

## 操作

- **菜单「视图」**：13 个实验视图任选。
- **菜单「对话框」**：有模式 DoModal / 无模式 Create+ShowWindow / 坐标输入 / 三角形顶点。
- **菜单「变换」**：平移、旋转、缩放、X/Y/原点反射、错切，可叠加（矩阵按左乘累积）。
- **鼠标**：左键按下 = `WM_LBUTTONDOWN`，移动 = `WM_MOUSEMOVE`，抬起 = `WM_LBUTTONUP`；右键复位当前视图。
- **键盘**：`+/-` 改半径，`1/2` 改顶点数，`r` 复位，`Esc` 退出。

## 自检

```bash
./docs/verify.sh
```

无界面运行 24 项数值断言（不依赖 X 显示），覆盖：
DDA 步数与端点、Bresenham 跨八象限与反向线、中点圆四极点与半径误差、
扫描线填充面积与解析值对比、三种裁剪结果一致性、端点编码、矩阵复合与定点缩放。

## 已验证的截图

| 文件 | 内容 |
|---|---|
| `docs/v0.png` | MouseLine：按下-抬起画线 |
| `docs/v2.png` | 坐标对话框定线（取点流程） |
| `docs/v4.png` | Bresenham 直线 / 中点圆 |
| `docs/v6.png` | 扫描线填充（p1/Fill 算法） |
| `docs/v8.png` | Cohen-Sutherland 裁剪 |
| `docs/v11.png` | 二维几何变换 + 五角星 |
| `docs/v_transform_rotate.png` | 施加旋转 30°（灰线为原始图形） |
