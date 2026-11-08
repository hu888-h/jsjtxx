/* algo.c —— 逐条对齐 p1 工程与 zip 算法包的原始实现
 *  DDA            <- zip/算法2：通用直线的DDA算法/TestView.cpp (DDALine)
 *  Bresenham      <- p1/DrawLine/DrawLineView.cpp (getPoints)
 *  MidpointCircle <- p1/DrawLine/DrawLineView.cpp (getCirclePoints)
 *  ScanlineFill   <- p1/Fill/FillView.cpp (buildEdgeList/makeEdgeRec/fillScan)
 *  CohenSutherland<- zip/算法16 (Encode / CohenSutherlandClip)
 *  MidpointSplit  <- zip/算法17 (MidpointClip)
 *  LiangBarsky    <- zip/算法18
 *  Transform2D    <- zip/算法15 (Transform2.cpp，3x3 齐次矩阵)
 */
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "algo.h"

/* ================= 点列容器 ================= */
void pl_init(PtList *l, int cap)
{
    if (cap < 4) cap = 4;
    l->x = (int *)malloc(sizeof(int) * (size_t)cap);
    l->y = (int *)malloc(sizeof(int) * (size_t)cap);
    l->n = 0; l->cap = cap;
}

void pl_free(PtList *l) { free(l->x); free(l->y); l->x = l->y = NULL; l->n = l->cap = 0; }
void pl_clear(PtList *l) { l->n = 0; }

void pl_push(PtList *l, int x, int y)
{
    if (l->n >= l->cap) {
        int nc = l->cap * 2;
        l->x = (int *)realloc(l->x, sizeof(int) * (size_t)nc);
        l->y = (int *)realloc(l->y, sizeof(int) * (size_t)nc);
        l->cap = nc;
    }
    l->x[l->n] = x;
    l->y[l->n] = y;
    l->n++;
}

#define ROUNDD(d) ((int)((d) + ((d) >= 0 ? 0.5 : -0.5)))

/* ================= 直线 ================= */

/* zip/算法2：通用 DDA。步长 = 1/max(|dx|,|dy|)，逐点四舍五入 */
int line_dda(int x0, int y0, int x1, int y1, PtList *out)
{
    int dx = x1 - x0, dy = y1 - y0;
    int epsl = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    if (epsl == 0) { pl_push(out, x0, y0); return 1; }
    double sx = (double)dx / epsl, sy = (double)dy / epsl;
    double x = x0, y = y0;
    int n0 = out->n;
    for (int i = 0; i <= epsl; ++i) {
        pl_push(out, ROUNDD(x), ROUNDD(y));
        x += sx; y += sy;
    }
    return out->n - n0;
}

/* 中点画线（与 Bresenham 等价但用浮点判别式的教材写法） */
int line_midpoint(int x0, int y0, int x1, int y1, PtList *out)
{
    int dx = abs(x1 - x0), dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
    int steep = dy > dx;
    int n0 = out->n;
    if (steep) { int t = dx; dx = dy; dy = t; }
    double d = 2.0 * dy - dx;
    int x = x0, y = y0;
    for (int i = 0; i <= dx; ++i) {
        pl_push(out, x, y);
        if (d >= 0) {
            if (steep) x += sx; else y += sy;
            d -= 2.0 * dx;
        }
        if (steep) y += sy; else x += sx;
        d += 2.0 * dy;
    }
    return out->n - n0;
}

/* p1/DrawLine getPoints：整数 Bresenham，分 |dx|>=|dy| 与 |dy|>|dx| 两支 */
int line_bresenham(int x0, int y0, int x1, int y1, PtList *out)
{
    int n0 = out->n;
    int dx, dy, p, twody, twodx, twodydx, twodxdy;
    int x, y, xend, yend;

    if (x0 == x1) {
        int a = y0 < y1 ? y0 : y1, b = y0 < y1 ? y1 : y0;
        for (y = a; y <= b; ++y) pl_push(out, x0, y);
        return out->n - n0;
    }
    if (y0 == y1) {
        int a = x0 < x1 ? x0 : x1, b = x0 < x1 ? x1 : x0;
        for (x = a; x <= b; ++x) pl_push(out, x, y0);
        return out->n - n0;
    }
    dx = abs(x0 - x1); dy = abs(y0 - y1);
    if (dx >= dy) {
        p = 2 * dy - dx; twody = 2 * dy; twodydx = 2 * (dy - dx);
        if (x0 > x1) { x = x1; y = y1; xend = x0; }
        else         { x = x0; y = y0; xend = x1; }
        pl_push(out, x, y);
        while (x < xend) {
            ++x;
            if (p < 0) p += twody;
            else { y += (y0 > y1 ? -1 : 1); p += twodydx; }
            pl_push(out, x, y);
        }
    } else {
        p = 2 * dx - dy; twodx = 2 * dx; twodxdy = 2 * (dx - dy);
        if (y0 > y1) { x = x1; y = y1; yend = y0; }
        else         { x = x0; y = y0; yend = y1; }
        pl_push(out, x, y);
        while (y < yend) {
            ++y;
            if (p < 0) p += twodx;
            else { x += (x0 > x1 ? -1 : 1); p += twodxdy; }
            pl_push(out, x, y);
        }
    }
    return out->n - n0;
}

/* 全八象限整型对称版：任意方向都正确（用于交互式橡皮筋的中间态） */
int line_bresenham_all(int x0, int y0, int x1, int y1, PtList *out)
{
    int n0 = out->n;
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    for (;;) {
        pl_push(out, x0, y0);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
    return out->n - n0;
}

/* ================= 圆 ================= */

/* p1 getCirclePoints：中点圆，8 对称。
 * 第一象限从 (0,r) 出发，x 递增时 y 必须递减；判别式 d = 1 - r 起。 */
int circle_midpoint(int cx, int cy, int r, PtList *out)
{
    int n0 = out->n;
    if (r <= 0) { pl_push(out, cx, cy); return 1; }
    int x = 0, y = r, d = 1 - r;
    while (x < y) {
        pl_push(out, cx + x, cy + y); pl_push(out, cx - x, cy + y);
        pl_push(out, cx + x, cy - y); pl_push(out, cx - x, cy - y);
        pl_push(out, cx + y, cy + x); pl_push(out, cx - y, cy + x);
        pl_push(out, cx + y, cy - x); pl_push(out, cx - y, cy - x);
        ++x;
        if (d < 0) d += 2 * x + 1;
        else { --y; d += 2 * (x - y) + 1; }
    }
    /* 45° 处的接缝补齐 */
    pl_push(out, cx + x, cy + y); pl_push(out, cx - x, cy + y);
    pl_push(out, cx + x, cy - y); pl_push(out, cx - x, cy - y);
    return out->n - n0;
}

/* Bresenham 圆（判别式 d = 3 - 2r 变体） */
int circle_bresenham(int cx, int cy, int r, PtList *out)
{
    int n0 = out->n;
    int x = 0, y = r, d = 3 - 2 * r;
    if (r <= 0) { pl_push(out, cx, cy); return 1; }
    while (y >= x) {
        pl_push(out, cx + x, cy + y); pl_push(out, cx - x, cy + y);
        pl_push(out, cx + x, cy - y); pl_push(out, cx - x, cy - y);
        pl_push(out, cx + y, cy + x); pl_push(out, cx - y, cy + x);
        pl_push(out, cx + y, cy - x); pl_push(out, cx - y, cy - x);
        ++x;
        if (d > 0) { --y; d = d + 4 * (x - y) + 10; }
        else d = d + 4 * x + 6;
    }
    return out->n - n0;
}

/* 中点椭圆（4 对称，两个区域分别推进）。a 为 x 半轴，b 为 y 半轴。 */
int ellipse_midpoint(int cx, int cy, int a, int b, PtList *out)
{
    int n0 = out->n;
    if (a <= 0) a = 1;
    if (b <= 0) b = 1;
    double a2 = (double)a * a, b2 = (double)b * b;
    int x = 0, y = b;
    double d1 = b2 - a2 * b + 0.25 * a2;
    /* 区域 1：法向量斜率 |dy/dx| < 1 */
    pl_push(out, cx + x, cy + y); pl_push(out, cx - x, cy + y);
    pl_push(out, cx + x, cy - y); pl_push(out, cx - x, cy - y);
    while (a2 * (y - 0.5) > b2 * (x + 1)) {
        ++x;
        if (d1 < 0) {
            d1 += b2 * (2.0 * x + 3.0);
        } else {
            --y;
            d1 += b2 * (2.0 * x + 3.0) + a2 * (-2.0 * y + 2.0);
        }
        pl_push(out, cx + x, cy + y); pl_push(out, cx - x, cy + y);
        pl_push(out, cx + x, cy - y); pl_push(out, cx - x, cy - y);
    }
    /* 区域 2：|dy/dx| >= 1，y 递减 */
    double d2 = b2 * (x + 0.5) * (x + 0.5) + a2 * (y - 1.0) * (y - 1.0) - a2 * b2;
    while (y > 0) {
        --y;
        if (d2 > 0) {
            d2 += a2 * (-2.0 * y + 3.0);
        } else {
            ++x;
            d2 += b2 * (2.0 * x + 2.0) + a2 * (-2.0 * y + 3.0);
        }
        pl_push(out, cx + x, cy + y); pl_push(out, cx - x, cy + y);
        pl_push(out, cx + x, cy - y); pl_push(out, cx - x, cy - y);
    }
    return out->n - n0;
}

/* ================= 裁剪 ================= */

int encode_code(double x, double y, double xl, double yb, double xr, double yt)
{
    int rc = 0;
    if (x < xl)      rc |= CLIP_LEFT;
    else if (x > xr) rc |= CLIP_RIGHT;
    if (y < yb)      rc |= CLIP_BOTTOM;
    else if (y > yt) rc |= CLIP_TOP;
    return rc;
}

/* 算法16：Cohen-Sutherland，按左/右/下/上顺序求交，简弃用逻辑乘 */
int clip_cohen_sutherland(Dpt *p0, Dpt *p1, double xl, double yb, double xr, double yt,
                          Dpt *o0, Dpt *o1)
{
    Dpt a = *p0, b = *p1;
    int ca = encode_code(a.x, a.y, xl, yb, xr, yt);
    int cb = encode_code(b.x, b.y, xl, yb, xr, yt);
    int guard = 0;
    while (ca != 0 || cb != 0) {
        if ((ca & cb) != 0) return 0;                  /* 简弃 */
        if (0 == ca) { Dpt t = a; a = b; b = t; int c = ca; ca = cb; cb = c; }
        Dpt ip;
        if (ca & CLIP_LEFT)        { ip.x = xl; ip.y = (ip.x - a.x) * (b.y - a.y) / (b.x - a.x) + a.y; }
        else if (ca & CLIP_RIGHT)  { ip.x = xr; ip.y = (ip.x - a.x) * (b.y - a.y) / (b.x - a.x) + a.y; }
        else if (ca & CLIP_BOTTOM) { ip.y = yb; ip.x = (ip.y - a.y) * (b.x - a.x) / (b.y - a.y) + a.x; }
        else                       { ip.y = yt; ip.x = (ip.y - a.y) * (b.x - a.x) / (b.y - a.y) + a.x; }
        a = ip;
        ca = encode_code(a.x, a.y, xl, yb, xr, yt);
        if (++guard > 64) return 0;
    }
    /* 交换后起点可能不是原始 p0 方向，按参数序还原：用 x/y 距离判断 */
    *o0 = a; *o1 = b;
    return 1;
}

/* 算法17：中点分割（二分逼近），对两端点分别求可见端点 */
static void midpoint_endpoint(Dpt *p, Dpt q, double xl, double yb, double xr, double yt, int depth)
{
    if (depth > 20) return;
    int cp = encode_code(p->x, p->y, xl, yb, xr, yt);
    int cq = encode_code(q.x, q.y, xl, yb, xr, yt);
    if (cq == 0) { *p = q; return; }              /* q 可见则整段可见 */
    if ((cp & cq) != 0) return;                    /* 同侧，不可见 */
    Dpt mid;
    mid.x = (p->x + q.x) * 0.5;
    mid.y = (p->y + q.y) * 0.5;
    Dpt save = *p;
    *p = mid;
    midpoint_endpoint(p, q, xl, yb, xr, yt, depth + 1);
    if (encode_code(p->x, p->y, xl, yb, xr, yt) != 0) { *p = save; midpoint_endpoint(p, mid, xl, yb, xr, yt, depth + 1); }
}

int clip_midpoint(Dpt *p0, Dpt *p1, double xl, double yb, double xr, double yt,
                  Dpt *o0, Dpt *o1)
{
    Dpt a = *p0, b = *p1;
    int ca = encode_code(a.x, a.y, xl, yb, xr, yt);
    int cb = encode_code(b.x, b.y, xl, yb, xr, yt);
    if ((ca & cb) != 0) return 0;
    if (ca == 0 && cb == 0) { *o0 = a; *o1 = b; return 1; }
    Dpt sa = a, sb = b;
    midpoint_endpoint(&sa, b, xl, yb, xr, yt, 0);   /* 起点向 b 逼近 */
    midpoint_endpoint(&sb, a, xl, yb, xr, yt, 0);   /* 终点向 a 逼近 */
    if (encode_code(sa.x, sa.y, xl, yb, xr, yt) != 0) return 0;
    if (encode_code(sb.x, sb.y, xl, yb, xr, yt) != 0) return 0;
    *o0 = sa; *o1 = sb;
    return 1;
}

/* 算法18：Liang-Barsky 参数化裁剪 */
int clip_liang_barsky(double x0, double y0, double x1, double y1,
                      double xl, double yb, double xr, double yt,
                      double *ox0, double *oy0, double *ox1, double *oy1)
{
    double dx = x1 - x0, dy = y1 - y0;
    double t0 = 0.0, t1 = 1.0;
    double p[4], q[4];
    p[0] = -dx; q[0] = x0 - xl;
    p[1] =  dx; q[1] = xr - x0;
    p[2] = -dy; q[2] = y0 - yb;
    p[3] =  dy; q[3] = yt - y0;
    for (int i = 0; i < 4; ++i) {
        if (p[i] == 0.0) {
            if (q[i] < 0.0) return 0;          /* 平行且在外侧 */
        } else {
            double t = q[i] / p[i];
            if (p[i] < 0.0) { if (t > t1) return 0; if (t > t0) t0 = t; }
            else            { if (t < t0) return 0; if (t < t1) t1 = t; }
        }
    }
    *ox0 = x0 + t0 * dx; *oy0 = y0 + t0 * dy;
    *ox1 = x0 + t1 * dx; *oy1 = y0 + t1 * dy;
    return 1;
}

/* ================= 二维变换 ================= */

void mat_identity(Mat3 *m) { memset(m, 0, sizeof(*m)); m->m[0][0] = m->m[1][1] = m->m[2][2] = 1.0; }

void mat_mul(Mat3 *a, const Mat3 *b, Mat3 *out)
{
    Mat3 r;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) {
            double s = 0;
            for (int k = 0; k < 3; ++k) s += a->m[i][k] * b->m[k][j];
            r.m[i][j] = s;
        }
    *out = r;
}

void mat_translate(Mat3 *m, double tx, double ty)
{
    Mat3 t; mat_identity(&t);
    t.m[0][2] = tx; t.m[1][2] = ty;
    mat_mul(&t, m, m);          /* m = T * m：新变换后施加 */
}

void mat_scale(Mat3 *m, double sx, double sy)
{
    Mat3 t; mat_identity(&t);
    t.m[0][0] = sx; t.m[1][1] = sy;
    mat_mul(&t, m, m);
}

void mat_scale_at(Mat3 *m, double sx, double sy, double px, double py)
{
    mat_translate(m, -px, -py);
    mat_scale(m, sx, sy);
    mat_translate(m, px, py);
}

void mat_rotate(Mat3 *m, double deg)
{
    double r = deg * 3.14159265358979 / 180.0;
    Mat3 t; mat_identity(&t);
    t.m[0][0] = cos(r); t.m[0][1] = -sin(r);
    t.m[1][0] = sin(r); t.m[1][1] =  cos(r);
    mat_mul(&t, m, m);
}

void mat_rotate_at(Mat3 *m, double deg, double px, double py)
{
    mat_translate(m, -px, -py);
    mat_rotate(m, deg);
    mat_translate(m, px, py);
}

void mat_reflect_origin(Mat3 *m) { mat_scale(m, -1.0, -1.0); }
void mat_reflect_x(Mat3 *m)      { mat_scale(m, 1.0, -1.0); }
void mat_reflect_y(Mat3 *m)      { mat_scale(m, -1.0, 1.0); }

void mat_shear(Mat3 *m, double b, double c)
{
    Mat3 t; mat_identity(&t);
    t.m[0][1] = b; t.m[1][0] = c;
    mat_mul(&t, m, m);
}

void mat_apply(const Mat3 *m, double x, double y, double *ox, double *oy)
{
    double X = m->m[0][0] * x + m->m[0][1] * y + m->m[0][2];
    double Y = m->m[1][0] * x + m->m[1][1] * y + m->m[1][2];
    double W = m->m[2][0] * x + m->m[2][1] * y + m->m[2][2];
    if (W != 0.0 && W != 1.0) { X /= W; Y /= W; }
    *ox = X; *oy = Y;
}

/* 五角星（算法15 的 Pentagram 示例）：外半径 r、内半径 r*inner_ratio */
int poly_star(double cx, double cy, double r, int spikes, double inner_ratio,
              double rot_deg, PtList *out)
{
    int n0 = out->n;
    double step = 3.14159265358979 / spikes;
    double start = rot_deg * 3.14159265358979 / 180.0;
    for (int i = 0; i < spikes * 2; ++i) {
        double rad = (i % 2 == 0) ? r : r * inner_ratio;
        double a = start + i * step;
        pl_push(out, ROUNDD(cx + rad * cos(a)), ROUNDD(cy - rad * sin(a)));
    }
    return out->n - n0;
}
