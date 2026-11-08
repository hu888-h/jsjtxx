/* view.c —— 各视图的实现，逐条对应 p1 工程与 zip 算法包
 *
 * 坐标系约定：数学坐标 y 向上、原点在绘图区中心；设备坐标 y 向下、原点左上。
 * 与 p1 中 pDC->SetMapMode(MM_ANISOTROPIC) + SetViewportExt(w,-h) + SetViewportOrg(w/2,h/2)
 * 完全等价，只是这里手写换算而不是交给 GDI。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "px.h"
#include "algo.h"
#include "view.h"

/* 每个视图的操作提示（显示在状态栏左侧） */
const char *kViewHint[VIEW_COUNT] = {
    "按住左键拖动，抬起完成直线（WM_LBUTTONDOWN / WM_LBUTTONUP）",
    "第一次点击定起点，移动预览，第二次点击落定；右键清空全部",
    "菜单「对话框 → 坐标输入」精确输入，或先「取起点」再点选",
    "点击绘图区改变半径；算法2 星芒图 0..355° 每 5° 一条",
    "拖动左键画 Bresenham 直线；数字键 1 切到中点圆",
    "点击移动圆心，+/- 改半径；可同时查看中点椭圆",
    "拖动三个顶点；菜单「对话框 → 三角形顶点」精确输入",
    "点击改变多边形位置；种子填充 4 连通",
    "拖动两个端点，观察 Cohen-Sutherland 的可见段",
    "拖动两个端点，观察中点分割的二分逼近结果",
    "拖动两个端点，观察 Liang-Barsky 的参数裁剪结果",
    "菜单「变换」施加平移/旋转/缩放/反射/错切，灰线为原始图形",
    "菜单「对话框」分别体验有模式 DoModal 与无模式 Create+ShowWindow",
};

const char *kViewName[VIEW_COUNT] = {
    "MouseLine（按下-抬起画线）",
    "SingleDraw（两次点击 + 橡皮筋）",
    "坐标对话框定线（DoModal + 取点）",
    "DDA 直线（算法2）",
    "Bresenham 直线 + 中点圆",
    "中点圆 / 中点椭圆",
    "扫描线填充（p1/Fill）",
    "种子填充（4 连通）",
    "Cohen-Sutherland 裁剪（算法16）",
    "中点分割裁剪（算法17）",
    "Liang-Barsky 裁剪（算法18）",
    "二维几何变换 + 五角星（算法15）",
    "有模式 / 无模式对话框（DLGExam）",
};

/* ---------------- 坐标换算 ---------------- */
static inline int dx_of(Rect ar, double x) { return ar.x + ar.w / 2 + (int)lround(x); }
static inline int dy_of(Rect ar, double y) { return ar.y + ar.h / 2 - (int)lround(y); }

static void draw_axes(App *a, Rect ar)
{
    Canvas *c = &a->fb;
    /* 网格 */
    int step = 50;
    for (int x = -ar.w / 2; x <= ar.w / 2; x += step) {
        int dxx = dx_of(ar, (double)x);
        cv_vline(c, dxx, ar.y, ar.h, PX_RGB(238,240,244));
    }
    for (int y = -ar.h / 2; y <= ar.h / 2; y += step) {
        int dyy = dy_of(ar, (double)y);
        cv_hline(c, ar.x, dyy, ar.w, PX_RGB(238,240,244));
    }
    /* X/Y 轴 */
    cv_hline(c, ar.x, dy_of(ar, 0), ar.w, PX_RGB(205,208,214));
    cv_vline(c, dx_of(ar, 0), ar.y, ar.h, PX_RGB(205,208,214));
    /* 刻度数字 */
    char buf[16];
    for (int x = -200; x <= 200; x += 100) {
        if (x == 0) continue;
        snprintf(buf, sizeof(buf), "%d", x);
        tx_draw(c, dx_of(ar, (double)x) - 8, dy_of(ar, 0) + 4, buf, CLR_GRAY, 11);
    }
    for (int y = -200; y <= 200; y += 100) {
        if (y == 0) continue;
        snprintf(buf, sizeof(buf), "%d", y);
        tx_draw(c, dx_of(ar, 0) + 6, dy_of(ar, (double)y) - 14, buf, CLR_GRAY, 11);
    }
    tx_draw(c, dx_of(ar, 0) + 6, dy_of(ar, 0) + 4, "O", CLR_GRAY, 12);
}

/* 画点列（数学坐标 -> 设备坐标） */
static void draw_pts(App *a, Rect ar, const PtList *l, Color col, int thick)
{
    Canvas *c = &a->fb;
    for (int i = 0; i < l->n; ++i) {
        int X = dx_of(ar, (double)l->x[i]);
        int Y = dy_of(ar, (double)l->y[i]);
        if (thick > 1) {
            cv_pixel(c, X, Y, col);
            cv_pixel(c, X + 1, Y, col); cv_pixel(c, X, Y + 1, col); cv_pixel(c, X + 1, Y + 1, col);
        } else {
            cv_pixel(c, X, Y, col);
        }
    }
}

static void draw_dpt_line(App *a, Rect ar, Dpt p0, Dpt p1, Color col, int thick)
{
    PtList l; pl_init(&l, 256);
    line_bresenham_all((int)lround(p0.x), (int)lround(p0.y),
                       (int)lround(p1.x), (int)lround(p1.y), &l);
    draw_pts(a, ar, &l, col, thick);
    pl_free(&l);
}

static void draw_clip_window(App *a, Rect ar, double xl, double yb, double xr, double yt)
{
    Canvas *c = &a->fb;
    int x0 = dx_of(ar, xl), y0 = dy_of(ar, yt);
    int w = (int)(xr - xl), h = (int)(yt - yb);
    Rect r = {x0, y0, w, h};
    /* 裁剪窗口内部淡色 */
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x)
            cv_blend_pixel(c, x, y, PX_RGB(120,170,230), 26);
    cv_dashes(c, x0, y0, x0 + w, y0, 6, 4, CLR_BLUE);
    cv_dashes(c, x0, y0 + h, x0 + w, y0 + h, 6, 4, CLR_BLUE);
    cv_dashes(c, x0, y0, x0, y0 + h, 6, 4, CLR_BLUE);
    cv_dashes(c, x0 + w, y0, x0 + w, y0 + h, 6, 4, CLR_BLUE);
}

static void label(App *a, Rect ar, double x, double y, const char *s, Color col)
{
    tx_draw(&a->fb, dx_of(ar, x) + 6, dy_of(ar, y) - 16, s, col, 12);
}

/* ---------------- 场景初始化 ---------------- */
void scene_init(Scene *s)
{
    memset(s, 0, sizeof(*s));
    mat_identity(&s->mat);
    s->a.x = -120; s->a.y = -60;
    s->b.x =  120; s->b.y =  60;
    s->radius = 90;
    s->params[0] = 3;      /* 多边形顶点数 */
    s->params[1] = 0;      /* 阶段/模式 */
    s->dparams[0] = 30.0;  /* 旋转角 */
    s->dparams[1] = 1.2;   /* 缩放 */
}

void scene_reset(Scene *s)
{
    ViewId id = s->id;
    char info[256];
    snprintf(info, sizeof(info), "%s", s->info);
    scene_init(s);
    s->id = id;
    snprintf(s->info, sizeof(s->info), "%s", info);
}

/* ============================================================
 *  1. MouseLine —— p1/MouseLine：OnLButtonDown 记起点，OnLButtonUp 连线
 * ============================================================ */
static void draw_mouse_line(App *a, Scene *s, Rect ar)
{
    Canvas *c = &a->fb;
    if (s->dragging) {
        /* 按下尚未抬起：显示起点标记（对应 OnLButtonDown 里 p1 = point） */
        int X = dx_of(ar, s->a.x), Y = dy_of(ar, s->a.y);
        cv_marker(c, X, Y, 6, CLR_ORANGE);
        draw_dpt_line(a, ar, s->a, s->b, CLR_MGRAY, 1);
        tx_draw(c, X + 8, Y - 8, "起点", CLR_ORANGE, 12);
    }
    if (s->flag > 0)
        draw_dpt_line(a, ar, s->a, s->b, CLR_RED, 2);
    label(a, ar, s->a.x, s->a.y, "P1", CLR_DGRAY);
    label(a, ar, s->b.x, s->b.y, "P2", CLR_DGRAY);
}

/* ============================================================
 *  2. SingleDraw —— p1/DrawLineView/SingleDraw
 *     第一次点击定点，移动时 SetROP2(R2_NOT) 画橡皮筋，第二次点击落定并存链表
 * ============================================================ */
static void draw_single_draw(App *a, Scene *s, Rect ar)
{
    Canvas *c = &a->fb;
    /* 已保存的线段（p1 用 linkList<CLine> 存文档，这里用定长数组） */
    for (int i = 0; i < s->nseg; ++i) {
        Dpt p = { s->seg[i].x1, s->seg[i].y1 };
        Dpt q = { s->seg[i].x2, s->seg[i].y2 };
        draw_dpt_line(a, ar, p, q, CLR_BLUE, 2);
    }
    /* 正在拖拽的橡皮筋（对应 SetROP2(R2_NOT) 的预览线） */
    if (s->dragging) {
        draw_dpt_line(a, ar, s->a, s->b, PX_RGB(230,140,60), 1);
        int X = dx_of(ar, s->a.x), Y = dy_of(ar, s->a.y);
        cv_marker(c, X, Y, 6, CLR_ORANGE);
    }
    char buf[64];
    snprintf(buf, sizeof(buf), "已保存 %d 条线段", s->nseg);
    tx_draw(c, ar.x + 12, ar.y + 10, buf, CLR_DGRAY, 13);
    tx_draw(c, ar.x + 12, ar.y + 28, "第一次点击定起点，移动预览，第二次点击落定；右键清空", CLR_GRAY, 12);
}

/* ============================================================
 *  3. DLGLINE —— p1/DrawLineView/SingleDraw2
 *     坐标对话框（DoModal）+ “取起点/取终点”按钮回到视图取点
 * ============================================================ */
static void draw_dlg_line(App *a, Scene *s, Rect ar)
{
    Canvas *c = &a->fb;
    if (s->flag > 0)
        draw_dpt_line(a, ar, s->a, s->b, CLR_GREEN, 2);
    int X1 = dx_of(ar, s->a.x), Y1 = dy_of(ar, s->a.y);
    int X2 = dx_of(ar, s->b.x), Y2 = dy_of(ar, s->b.y);
    cv_marker(c, X1, Y1, 6, CLR_RED);
    cv_marker(c, X2, Y2, 6, CLR_RED);
    char buf[64];
    snprintf(buf, sizeof(buf), "(%d, %d)", (int)lround(s->a.x), (int)lround(s->a.y));
    tx_draw(c, X1 + 8, Y1 - 18, buf, CLR_DGRAY, 12);
    snprintf(buf, sizeof(buf), "(%d, %d)", (int)lround(s->b.x), (int)lround(s->b.y));
    tx_draw(c, X2 + 8, Y2 + 4, buf, CLR_DGRAY, 12);
    if (s->stage == 1) {
        tx_draw(c, ar.x + 12, ar.y + 10, "「取起点」模式：请点击绘图区选择起点",
                CLR_ORANGE, 13);
    } else if (s->stage == 2) {
        tx_draw(c, ar.x + 12, ar.y + 10, "「取终点」模式：请点击绘图区选择终点",
                CLR_ORANGE, 13);
    }
}

/* ============================================================
 *  4. DDA —— zip/算法2
 *     原实验是 0..355 度每 5 度画一条半径 300 的直线（星芒），这里保留该形态，
 *     并允许点击改变半径。
 * ============================================================ */
static void draw_dda(App *a, Scene *s, Rect ar)
{
    if (s->params[1] == 0) {
        PtList l; pl_init(&l, 4096);
        int R = s->params[0] > 0 ? s->params[0] : 150;
        for (int ang = 0; ang < 360; ang += 5) {
            double x = R * cos(ang * 3.14159265358979 / 180.0);
            double y = R * sin(ang * 3.14159265358979 / 180.0);
            line_dda(0, 0, (int)lround(x), (int)lround(y), &l);
        }
        draw_pts(a, ar, &l, CLR_BLUE, 1);
        pl_free(&l);
        
        char buf[64];
        snprintf(buf, sizeof(buf), "半径 R = %d（点击绘图区改变）", R);
        tx_draw(&a->fb, ar.x + 12, ar.y + 10, buf, CLR_DGRAY, 13);
    } else {
        PtList l; pl_init(&l, 512);
        int n = line_dda((int)s->a.x, (int)s->a.y, (int)s->b.x, (int)s->b.y, &l);
        draw_pts(a, ar, &l, CLR_BLUE, 2);
        pl_free(&l);
        char buf[96];
        snprintf(buf, sizeof(buf), "DDA: (%d,%d)->(%d,%d)  步数=%d",
                 (int)s->a.x, (int)s->a.y, (int)s->b.x, (int)s->b.y, n);
        tx_draw(&a->fb, ar.x + 12, ar.y + 10, buf, CLR_DGRAY, 13);
    }
}

/* ============================================================
 *  5. BRESENHAM —— p1/DrawLine：getPoints + getCirclePoints
 * ============================================================ */
static void draw_bresenham(App *a, Scene *s, Rect ar)
{
    if (s->params[1] == 0) {
        PtList l; pl_init(&l, 512);
        line_bresenham((int)s->a.x, (int)s->a.y, (int)s->b.x, (int)s->b.y, &l);
        draw_pts(a, ar, &l, CLR_RED, 2);
        char buf[96];
        snprintf(buf, sizeof(buf), "Bresenham 直线 (%d,%d)->(%d,%d)",
                 (int)s->a.x, (int)s->a.y, (int)s->b.x, (int)s->b.y);
        tx_draw(&a->fb, ar.x + 12, ar.y + 10, buf, CLR_DGRAY, 13);
        pl_free(&l);
    } else {
        PtList l; pl_init(&l, 2048);
        circle_midpoint((int)s->a.x, (int)s->a.y, s->radius, &l);
        draw_pts(a, ar, &l, CLR_RED, 2);
        char buf[96];
        snprintf(buf, sizeof(buf), "中点圆 圆心(%d,%d) r=%d",
                 (int)s->a.x, (int)s->a.y, s->radius);
        tx_draw(&a->fb, ar.x + 12, ar.y + 10, buf, CLR_DGRAY, 13);
        pl_free(&l);
    }
}

/* ============================================================
 *  6. CIRCLE —— 中点圆 / 中点椭圆对比
 * ============================================================ */
static void draw_circle(App *a, Scene *s, Rect ar)
{
    PtList l; pl_init(&l, 4096);
    circle_midpoint((int)s->a.x, (int)s->a.y, s->radius, &l);
    draw_pts(a, ar, &l, CLR_GREEN, 2);
    pl_clear(&l);
    ellipse_midpoint((int)s->b.x, (int)s->b.y, s->radius, s->radius / 2, &l);
    draw_pts(a, ar, &l, CLR_ORANGE, 2);
    pl_free(&l);
    char buf[128];
    snprintf(buf, sizeof(buf), "中点圆 r=%d（绿） / 中点椭圆 a=%d,b=%d（橙）",
             s->radius, s->radius, s->radius / 2);
    tx_draw(&a->fb, ar.x + 12, ar.y + 10, buf, CLR_DGRAY, 13);
}

/* ============================================================
 *  7. SCANFILL —— p1/Fill：扫描线填充三角形（顶点可由对话框 / 点击修改）
 * ============================================================ */
static Vtx *tri_pts(Scene *s)
{
    static Vtx v[3];
    v[0].x = (int)s->a.x; v[0].y = (int)s->a.y;
    v[1].x = (int)s->b.x; v[1].y = (int)s->b.y;
    v[2].x = (int)(s->dparams[0]); v[2].y = (int)(s->dparams[1]);
    return v;
}

static void draw_scanfill(App *a, Scene *s, Rect ar)
{
    Canvas *c = &a->fb;
    Vtx *v = tri_pts(s);
    /* 用 p1 的边表算法生成所有填充点到点列，再逐点画 */
    PtList fill; pl_init(&fill, 8192);
    fill_scanline(v, 3, -100000, 100000, &fill);
    for (int i = 0; i < fill.n; ++i) {
        int X = dx_of(ar, (double)fill.x[i]);
        int Y = dy_of(ar, (double)fill.y[i]);
        cv_pixel(c, X, Y, PX_RGB(80,150,235));
    }
    pl_free(&fill);

    /* 三角边 */
    for (int i = 0; i < 3; ++i) {
        int j = (i + 1) % 3;
        Dpt p = { (double)v[i].x, (double)v[i].y };
        Dpt q = { (double)v[j].x, (double)v[j].y };
        draw_dpt_line(a, ar, p, q, CLR_GREEN, 2);
    }
    for (int i = 0; i < 3; ++i) {
        int X = dx_of(ar, (double)v[i].x), Y = dy_of(ar, (double)v[i].y);
        cv_marker(c, X, Y, 6, CLR_RED);
    }
    char buf[128];
    snprintf(buf, sizeof(buf), "扫描线填充：(%d,%d) (%d,%d) (%d,%d)",
             v[0].x, v[0].y, v[1].x, v[1].y, v[2].x, v[2].y);
    tx_draw(c, ar.x + 12, ar.y + 10, buf, CLR_DGRAY, 13);
    tx_draw(c, ar.x + 12, ar.y + 28, "点击顶点可拖动；对话框可精确输入", CLR_GRAY, 12);
}

/* ============================================================
 *  8. SEEDFILL —— 种子填充（4 连通）
 *    p1 里 window[] 是 1024x768 全局缓冲，这里按绘图区尺寸动态建。
 * ============================================================ */
static int *g_seedbuf = NULL;
static int  g_seedw = 0, g_seedh = 0;

static void ensure_seedbuf(int w, int h)
{
    if (g_seedw == w && g_seedh == h && g_seedbuf) return;
    free(g_seedbuf);
    g_seedbuf = (int *)calloc((size_t)w * (size_t)h, sizeof(int));
    g_seedw = w; g_seedh = h;
}

static void draw_seedfill(App *a, Scene *s, Rect ar)
{
    Canvas *c = &a->fb;
    ensure_seedbuf(ar.w, ar.h);
    /* 用一个不规则闭合多边形作为边界 */
    Vtx poly[6];
    int n = 5;
    for (int i = 0; i < n; ++i) {
        double ang = i * 2.0 * 3.14159265358979 / n + 0.4;
        poly[i].x = (int)lround(s->a.x + s->radius * cos(ang));
        poly[i].y = (int)lround(s->a.y + s->radius * sin(ang));
    }
    /* 边界写入缓冲（设备坐标） */
    memset(g_seedbuf, 0, (size_t)ar.w * (size_t)ar.h * sizeof(int));
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        int x0 = dx_of(ar, (double)poly[i].x) - ar.x;
        int y0 = dy_of(ar, (double)poly[i].y) - ar.y;
        int x1 = dx_of(ar, (double)poly[j].x) - ar.x;
        int y1 = dy_of(ar, (double)poly[j].y) - ar.y;
        int dx = abs(x1 - x0), sxs = x0 < x1 ? 1 : -1;
        int dy = -abs(y1 - y0), sys = y0 < y1 ? 1 : -1;
        int err = dx + dy;
        for (;;) {
            if ((unsigned)x0 < (unsigned)ar.w && (unsigned)y0 < (unsigned)ar.h)
                g_seedbuf[y0 * ar.w + x0] = 2;
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sxs; }
            if (e2 <= dx) { err += dx; y0 += sys; }
        }
    }
    /* 种子点取多边形重心（用顶点平均值，对凹多边形也稳定） */
    double gx = 0, gy = 0;
    for (int i = 0; i < n; ++i) { gx += poly[i].x; gy += poly[i].y; }
    int sx = dx_of(ar, gx / n) - ar.x;
    int sy = dy_of(ar, gy / n) - ar.y;
    /* 种子若落在边界上（例如重心恰在边界像素），向内偏移一格 */
    if ((unsigned)sx < (unsigned)ar.w && (unsigned)sy < (unsigned)ar.h &&
        g_seedbuf[sy * ar.w + sx] == 2) {
        sx += 1; sy += 1;
    }
    int filled = fill_seed(g_seedbuf, ar.w, ar.h, sx, sy, 2, 1);
    for (int y = 0; y < ar.h; ++y)
        for (int x = 0; x < ar.w; ++x) {
            if (g_seedbuf[y * ar.w + x] == 1) cv_pixel(c, ar.x + x, ar.y + y, PX_RGB(90,160,240));
            else if (g_seedbuf[y * ar.w + x] == 2) cv_pixel(c, ar.x + x, ar.y + y, CLR_GREEN);
        }
    char buf[128];
    snprintf(buf, sizeof(buf), "种子填充（4 连通）：%d 边多边形，种子 (%d,%d)，填充 %d 像素",
             n, sx, sy, filled);
    tx_draw(c, ar.x + 12, ar.y + 10, buf, CLR_DGRAY, 13);
    tx_draw(c, ar.x + 12, ar.y + 28, "点击绘图区移动多边形，种子随重心自动重算", CLR_GRAY, 12);
}

/* ============================================================
 *  9/10/11. 三种裁剪 —— 算法16/17/18
 * ============================================================ */
static void clip_setup(Scene *s, double *xl, double *yb, double *xr, double *yt)
{
    *xl = -200; *yb = -110; *xr = 200; *yt = 110;
    (void)s;
}

static void draw_clip_view(App *a, Scene *s, Rect ar, int method)
{
    Canvas *c = &a->fb;
    double xl, yb, xr, yt;
    clip_setup(s, &xl, &yb, &xr, &yt);
    draw_clip_window(a, ar, xl, yb, xr, yt);

    /* 原始线段：灰色虚线（窗口外部分） */
    draw_dpt_line(a, ar, s->a, s->b, PX_RGB(150,150,155), 1);
    /* 端点 */
    int X0 = dx_of(ar, s->a.x), Y0 = dy_of(ar, s->a.y);
    int X1 = dx_of(ar, s->b.x), Y1 = dy_of(ar, s->b.y);
    cv_marker(c, X0, Y0, 5, CLR_GRAY);
    cv_marker(c, X1, Y1, 5, CLR_GRAY);

    Dpt o0 = {0,0}, o1 = {0,0};
    int ok = 0;
    const char *name = "";
    if (method == 16) {
        Dpt p0 = s->a, p1 = s->b;
        ok = clip_cohen_sutherland(&p0, &p1, xl, yb, xr, yt, &o0, &o1);
        name = "Cohen-Sutherland";
    } else if (method == 17) {
        Dpt p0 = s->a, p1 = s->b;
        ok = clip_midpoint(&p0, &p1, xl, yb, xr, yt, &o0, &o1);
        name = "中点分割";
    } else {
        ok = clip_liang_barsky(s->a.x, s->a.y, s->b.x, s->b.y, xl, yb, xr, yt,
                               &o0.x, &o0.y, &o1.x, &o1.y);
        name = "Liang-Barsky";
    }

    if (ok) {
        draw_dpt_line(a, ar, o0, o1, CLR_RED, 2);
        int A = dx_of(ar, o0.x), B = dy_of(ar, o0.y);
        int C = dx_of(ar, o1.x), D = dy_of(ar, o1.y);
        cv_marker(c, A, B, 7, CLR_ORANGE);
        cv_marker(c, C, D, 7, CLR_ORANGE);
        char buf[160];
        snprintf(buf, sizeof(buf), "%s：可见段 (%.1f,%.1f) -> (%.1f,%.1f)",
                 name, o0.x, o0.y, o1.x, o1.y);
        tx_draw(c, ar.x + 12, ar.y + 10, buf, CLR_DGRAY, 13);
    } else {
        char buf[160];
        snprintf(buf, sizeof(buf), "%s：整条线段不可见（简弃）", name);
        tx_draw(c, ar.x + 12, ar.y + 10, buf, CLR_RED, 13);
    }
    tx_draw(c, ar.x + 12, ar.y + 28, "拖动两端点改变线段，窗口固定 400x220", CLR_GRAY, 12);
}

/* ============================================================
 *  12. TRANSFORM —— zip/算法15：二维几何变换 + 五角星
 * ============================================================ */
static void draw_transform(App *a, Scene *s, Rect ar)
{
    Canvas *c = &a->fb;
    PtList star; pl_init(&star, 64);
    poly_star(0.0, 0.0, 150.0, 5, 0.382, 90.0, &star);

    /* 原始（灰） */
    for (int i = 0; i < star.n; ++i) {
        int j = (i + 1) % star.n;
        Dpt p = { (double)star.x[i], (double)star.y[i] };
        Dpt q = { (double)star.x[j], (double)star.y[j] };
        draw_dpt_line(a, ar, p, q, PX_RGB(185,185,190), 1);
    }
    /* 变换后（蓝） */
    for (int i = 0; i < star.n; ++i) {
        int j = (i + 1) % star.n;
        double ax, ay, bx, by;
        mat_apply(&s->mat, (double)star.x[i], (double)star.y[i], &ax, &ay);
        mat_apply(&s->mat, (double)star.x[j], (double)star.y[j], &bx, &by);
        Dpt p = { ax, ay }, q = { bx, by };
        draw_dpt_line(a, ar, p, q, CLR_BLUE, 2);
    }
    pl_free(&star);

    /* 显示当前矩阵前三项（齐次矩阵的线性部分 + 平移量） */
    char buf[200];
    snprintf(buf, sizeof(buf),
             "矩阵 [%.2f %.2f %.2f ; %.2f %.2f %.2f]",
             s->mat.m[0][0], s->mat.m[0][1], s->mat.m[0][2],
             s->mat.m[1][0], s->mat.m[1][1], s->mat.m[1][2]);
    tx_draw(c, ar.x + 12, ar.y + 10, buf, CLR_DGRAY, 13);
    if (s->info[0])
        tx_draw(c, ar.x + 12, ar.y + 28, s->info, CLR_BLUE, 12);
    else
        tx_draw(c, ar.x + 12, ar.y + 28, "菜单「变换」施加平移/旋转/缩放/反射/错切", CLR_GRAY, 12);
}

/* ============================================================
 *  13. DIALOG —— p1/DLGExam：有模式 vs 无模式
 * ============================================================ */
static void draw_dialog_view(App *a, Scene *s, Rect ar)
{
    Canvas *c = &a->fb;
    tx_draw(c, ar.x + 12, ar.y + 10, "对话框技术：同一个对话框控件，两种显示方式", CLR_DGRAY, 13);
    tx_draw(c, ar.x + 12, ar.y + 34, "菜单「对话框 → 有模式」：DoModal，关闭前无法操作主窗口", CLR_GRAY, 12);
    tx_draw(c, ar.x + 12, ar.y + 56, "菜单「对话框 → 无模式」：Create + ShowWindow，主窗口仍可交互", CLR_GRAY, 12);

    char buf[160];
    snprintf(buf, sizeof(buf), "最近一次输入值: %d", s->params[0]);
    Rect box = { ar.x + 12, ar.y + 92, 320, 44 };
    cv_fill_rect(c, box, PX_RGB(236,242,252));
    cv_rect(c, box, PX_RGB(150,170,200));
    tx_draw(c, box.x + 12, box.y + 8, "m_exam = ", CLR_DGRAY, 14);
    snprintf(buf, sizeof(buf), "%d", s->params[0]);
    tx_draw(c, box.x + 90, box.y + 8, buf, CLR_BLUE, 18);
}

/* ---------------- 统一绘制入口 ---------------- */
void scene_draw(App *a, Scene *s, Rect ar)
{
    draw_axes(a, ar);
    switch (s->id) {
    case VIEW_MOUSELINE:  draw_mouse_line(a, s, ar); break;
    case VIEW_SINGLEDRAW: draw_single_draw(a, s, ar); break;
    case VIEW_DLGLINE:    draw_dlg_line(a, s, ar); break;
    case VIEW_DDA:        draw_dda(a, s, ar); break;
    case VIEW_BRESENHAM:  draw_bresenham(a, s, ar); break;
    case VIEW_CIRCLE:     draw_circle(a, s, ar); break;
    case VIEW_SCANFILL:   draw_scanfill(a, s, ar); break;
    case VIEW_SEEDFILL:   draw_seedfill(a, s, ar); break;
    case VIEW_COHEN:      draw_clip_view(a, s, ar, 16); break;
    case VIEW_MIDCLIP:    draw_clip_view(a, s, ar, 17); break;
    case VIEW_LIANG:      draw_clip_view(a, s, ar, 18); break;
    case VIEW_TRANSFORM:  draw_transform(a, s, ar); break;
    case VIEW_DIALOG:     draw_dialog_view(a, s, ar); break;
    default: break;
    }
}

/* ---------------- 鼠标交互 ---------------- */
void scene_lbutton_down(App *a, Scene *s, int x, int y)
{
    Rect ar = app_draw_area(a);
    double mx = (double)(x - ar.x - ar.w / 2);
    double my = (double)(-(y - ar.y - ar.h / 2));
    switch (s->id) {
    case VIEW_MOUSELINE:
        s->a.x = mx; s->a.y = my;
        s->b = s->a;
        s->dragging = 1;
        break;
    case VIEW_SINGLEDRAW:
        if (s->dragging) {
            s->b.x = mx; s->b.y = my; s->dragging = 0; s->flag = 1;
            if (s->nseg < SEG_MAX) {
                s->seg[s->nseg].x1 = s->a.x; s->seg[s->nseg].y1 = s->a.y;
                s->seg[s->nseg].x2 = s->b.x; s->seg[s->nseg].y2 = s->b.y;
                s->nseg++;
            }
        } else { s->a.x = mx; s->a.y = my; s->b = s->a; s->dragging = 1; }
        break;
    case VIEW_DLGLINE:
        if (s->stage == 1) { s->a.x = mx; s->a.y = my; s->stage = 0; s->flag = 1; }
        else if (s->stage == 2) { s->b.x = mx; s->b.y = my; s->stage = 0; s->flag = 1; }
        break;
    case VIEW_DDA:
        if (s->params[1] == 0) {
            s->params[0] = (int)lround(sqrt(mx * mx + my * my));
            if (s->params[0] < 10) s->params[0] = 10;
        } else { s->a.x = mx; s->a.y = my; s->dragging = 1; }
        break;
    case VIEW_BRESENHAM:
        s->a.x = mx; s->a.y = my;
        if (s->params[1] == 0) { s->dragging = 1; }
        break;
    case VIEW_CIRCLE:
        if (s->params[1] == 0) { s->a.x = mx; s->a.y = my; s->radius = 90; }
        else {
            s->radius = (int)lround(sqrt((s->a.x - mx) * (s->a.x - mx) +
                                         (s->a.y - my) * (s->a.y - my)));
            if (s->radius < 5) s->radius = 5;
        }
        break;
    case VIEW_SCANFILL:
        s->a.x = mx; s->a.y = my;
        break;
    case VIEW_SEEDFILL:
        s->a.x = mx; s->a.y = my;
        s->flag = 0;
        break;
    case VIEW_COHEN:
    case VIEW_MIDCLIP:
    case VIEW_LIANG: {
        double da = (s->a.x - mx) * (s->a.x - mx) + (s->a.y - my) * (s->a.y - my);
        double db = (s->b.x - mx) * (s->b.x - mx) + (s->b.y - my) * (s->b.y - my);
        s->stage = (da < db) ? 1 : 2;
        if (s->stage == 1) { s->a.x = mx; s->a.y = my; } else { s->b.x = mx; s->b.y = my; }
        s->dragging = 1;
        break; }
    case VIEW_TRANSFORM:
        s->a.x = mx; s->a.y = my;
        break;
    default:
        break;
    }
}

void scene_lbutton_up(App *a, Scene *s, int x, int y)
{
    Rect ar = app_draw_area(a);
    double mx = (double)(x - ar.x - ar.w / 2);
    double my = (double)(-(y - ar.y - ar.h / 2));
    switch (s->id) {
    case VIEW_MOUSELINE:
        if (s->dragging) { s->b.x = mx; s->b.y = my; s->dragging = 0; s->flag = 1; }
        break;
    case VIEW_DDA:
    case VIEW_BRESENHAM:
        if (s->dragging) { s->b.x = mx; s->b.y = my; s->dragging = 0; }
        break;
    case VIEW_COHEN:
    case VIEW_MIDCLIP:
    case VIEW_LIANG:
        s->dragging = 0;
        break;
    default:
        break;
    }
}

void scene_motion(App *a, Scene *s, int x, int y)
{
    Rect ar = app_draw_area(a);
    double mx = (double)(x - ar.x - ar.w / 2);
    double my = (double)(-(y - ar.y - ar.h / 2));
    switch (s->id) {
    case VIEW_SINGLEDRAW:
        if (s->dragging) { s->b.x = mx; s->b.y = my; }
        break;
    case VIEW_DDA:
    case VIEW_BRESENHAM:
        if (s->dragging) { s->b.x = mx; s->b.y = my; }
        break;
    case VIEW_COHEN:
    case VIEW_MIDCLIP:
    case VIEW_LIANG:
        if (s->dragging) {
            if (s->stage == 1) { s->a.x = mx; s->a.y = my; }
            else if (s->stage == 2) { s->b.x = mx; s->b.y = my; }
        }
        break;
    default:
        break;
    }
}

void scene_key(App *a, Scene *s, int key)
{
    switch (key) {
    case XK_plus: case XK_equal:
        s->radius += 10; break;
    case XK_minus:
        s->radius -= 10; if (s->radius < 5) s->radius = 5; break;
    case XK_1: s->params[0]++; break;
    case XK_2: s->params[0]--; if (s->params[0] < 3) s->params[0] = 3; break;
    case XK_r: scene_reset(s); break;
    default: break;
    }
}
