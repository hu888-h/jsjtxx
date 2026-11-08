/* algo.c —— 算法层：逐条对齐 p1 工程与 zip 算法包
 * 性能要点：所有点列/跨度容器都支持预留与复用，避免每帧 malloc/free。
 */
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "algo.h"

#define ROUNDD(d) ((int)((d) + ((d) >= 0 ? 0.5 : -0.5)))

static inline void *xrealloc(void *p, size_t n)
{
    void *q = realloc(p, n);
    if (!q) abort();
    return q;
}

/* ================= 点列容器 ================= */
void pl_init(PtList *l, int cap)
{
    if (cap < 8) cap = 8;
    l->x = (int *)xrealloc(NULL, sizeof(int) * (size_t)cap);
    l->y = (int *)xrealloc(NULL, sizeof(int) * (size_t)cap);
    l->n = 0;
    l->cap = cap;
}

void pl_free(PtList *l)
{
    free(l->x); free(l->y);
    l->x = l->y = NULL;
    l->n = l->cap = 0;
}

void pl_clear(PtList *l) { l->n = 0; }

void pl_reserve(PtList *l, int cap)
{
    if (cap <= l->cap) return;
    int nc = l->cap ? l->cap : 8;
    while (nc < cap) nc <<= 1;
    l->x = (int *)xrealloc(l->x, sizeof(int) * (size_t)nc);
    l->y = (int *)xrealloc(l->y, sizeof(int) * (size_t)nc);
    l->cap = nc;
}

void pl_push(PtList *l, int x, int y)
{
    if (l->n >= l->cap) pl_reserve(l, l->cap * 2);
    l->x[l->n] = x;
    l->y[l->n] = y;
    l->n++;
}

/* ================= 跨度容器 ================= */
void span_init(SpanList *l, int cap)
{
    if (cap < 16) cap = 16;
    l->s = (Span *)xrealloc(NULL, sizeof(Span) * (size_t)cap);
    l->n = 0;
    l->cap = cap;
}

void span_free(SpanList *l)
{
    free(l->s);
    l->s = NULL;
    l->n = l->cap = 0;
}

void span_clear(SpanList *l) { l->n = 0; }

void span_push(SpanList *l, int y, int xs, int xe)
{
    if (xs > xe) { int t = xs; xs = xe; xe = t; }
    if (l->n >= l->cap) {
        int nc = l->cap ? l->cap * 2 : 16;
        l->s = (Span *)xrealloc(l->s, sizeof(Span) * (size_t)nc);
        l->cap = nc;
    }
    l->s[l->n].y = y;
    l->s[l->n].xs = xs;
    l->s[l->n].xe = xe;
    l->n++;
}

/* ================= 直线 ================= */

/* zip/算法2：通用 DDA。步长 1/max(|dx|,|dy|)，逐点四舍五入 */
int line_dda(int x0, int y0, int x1, int y1, PtList *out)
{
    int dx = x1 - x0, dy = y1 - y0;
    int adx = dx < 0 ? -dx : dx, ady = dy < 0 ? -dy : dy;
    int epsl = adx > ady ? adx : ady;
    if (epsl == 0) { pl_push(out, x0, y0); return 1; }
    double sx = (double)dx / (double)epsl, sy = (double)dy / (double)epsl;
    double x = (double)x0, y = (double)y0;
    int n0 = out->n;
    pl_reserve(out, out->n + epsl + 1);
    for (int i = 0; i <= epsl; ++i) {
        pl_push(out, ROUNDD(x), ROUNDD(y));
        x += sx; y += sy;
    }
    return out->n - n0;
}

/* 中点画线：与 Bresenham 等价，浮点判别式的教材写法 */
int line_midpoint(int x0, int y0, int x1, int y1, PtList *out)
{
    int dx = x1 - x0, dy = y1 - y0;
    int adx = dx < 0 ? -dx : dx, ady = dy < 0 ? -dy : dy;
    int sx = dx >= 0 ? 1 : -1, sy = dy >= 0 ? 1 : -1;
    int steep = ady > adx;
    int n0 = out->n;
    if (steep) { int t = adx; adx = ady; ady = t; }
    double d = 2.0 * ady - adx;
    int x = x0, y = y0;
    pl_reserve(out, out->n + adx + 1);
    for (int i = 0; i <= adx; ++i) {
        pl_push(out, x, y);
        if (d >= 0) {
            if (steep) x += sx; else y += sy;
            d -= 2.0 * adx;
        }
        if (steep) y += sy; else x += sx;
        d += 2.0 * ady;
    }
    return out->n - n0;
}

/* p1 getPoints：整数 Bresenham。原实现只处理正向，这里补齐反向 */
int line_bresenham(int x0, int y0, int x1, int y1, PtList *out)
{
    int n0 = out->n;
    int dx, dy, p, twody, twodx, twodydx, twodxdy;
    int x, y, xend, yend, ystep, xstep;

    if (x0 == x1) {
        int a = y0 < y1 ? y0 : y1, b = y0 < y1 ? y1 : y0;
        pl_reserve(out, out->n + (b - a) + 1);
        for (y = a; y <= b; ++y) pl_push(out, x0, y);
        return out->n - n0;
    }
    if (y0 == y1) {
        int a = x0 < x1 ? x0 : x1, b = x0 < x1 ? x1 : x0;
        pl_reserve(out, out->n + (b - a) + 1);
        for (x = a; x <= b; ++x) pl_push(out, x, y0);
        return out->n - n0;
    }

    dx = x0 - x1; dy = y0 - y1;
    dx = dx < 0 ? -dx : dx;
    dy = dy < 0 ? -dy : dy;

    if (dx >= dy) {
        p = 2 * dy - dx; twody = 2 * dy; twodydx = 2 * (dy - dx);
        ystep = (y0 < y1) ? 1 : -1;
        if (x0 > x1) { x = x1; y = y1; xend = x0; }
        else         { x = x0; y = y0; xend = x1; }
        pl_reserve(out, out->n + dx + 1);
        pl_push(out, x, y);
        while (x < xend) {
            ++x;
            if (p < 0) p += twody;
            else { y += ystep; p += twodydx; }
            pl_push(out, x, y);
        }
    } else {
        p = 2 * dx - dy; twodx = 2 * dx; twodxdy = 2 * (dx - dy);
        xstep = (x0 < x1) ? 1 : -1;
        if (y0 > y1) { x = x1; y = y1; yend = y0; }
        else         { x = x0; y = y0; yend = y1; }
        pl_reserve(out, out->n + dy + 1);
        pl_push(out, x, y);
        while (y < yend) {
            ++y;
            if (p < 0) p += twodx;
            else { x += xstep; p += twodxdy; }
            pl_push(out, x, y);
        }
    }
    return out->n - n0;
}

/* ================= 圆 ================= */

/* p1 getCirclePoints：中点圆，8 对称。x 递增时 y 递减 */
int circle_midpoint(int cx, int cy, int r, PtList *out)
{
    int n0 = out->n;
    if (r <= 0) { pl_push(out, cx, cy); return 1; }
    pl_reserve(out, out->n + 8 * (r / 2 + 2));
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
    pl_push(out, cx + x, cy + y); pl_push(out, cx - x, cy + y);
    pl_push(out, cx + x, cy - y); pl_push(out, cx - x, cy - y);
    return out->n - n0;
}

/* Bresenham 圆（d = 3 - 2r 变体） */
int circle_bresenham(int cx, int cy, int r, PtList *out)
{
    int n0 = out->n;
    if (r <= 0) { pl_push(out, cx, cy); return 1; }
    pl_reserve(out, out->n + 8 * (r / 2 + 2));
    int x = 0, y = r, d = 3 - 2 * r;
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

/* ================= 扫描线填充（AET，重写优化版） =================
 * p1 原实现：每帧 malloc 768 个桶 + 每条边 malloc + 成对遍历链表 + 排序重建，
 * 且存在 sizeof(edge) 越界、double free、顶点重复计数等问题。
 *
 * 本版：单次 malloc 一个边池 + 桶头索引数组，边按 y 入桶后直接接管到 AET，
 * 全程 O(n + 扫描线数 * 活跃边数)，不做链表重排（用插入排序替换为稳定归并），
 * 每帧只调用一次，输出合并后的跨度 (SpanList)，由绘制端一次画完。
 */
typedef struct Edge {
    int    yUpper;
    double xIntersect, dxPerScan;
    int    next;        /* 池内下标链，-1 结尾 */
    int    active_next;
} Edge;

static void span_merge_push(SpanList *out, int y, int xs, int xe)
{
    if (xs > xe) { int t = xs; xs = xe; xe = t; }
    /* 与上一个跨度同行且相/相接则合并，减少画线调用次数 */
    if (out->n > 0) {
        Span *p = &out->s[out->n - 1];
        if (p->y == y && xs <= p->xe + 1) {
            if (xe > p->xe) p->xe = xe;
            return;
        }
    }
    span_push(out, y, xs, xe);
}

static int y_next(const Vtx *pts, int cnt, int k)
{
    int j = (k + 1 > cnt - 1) ? 0 : k + 1;
    for (int guard = 0; guard <= cnt; ++guard) {
        if (pts[k].y != pts[j].y) break;
        j = (j + 1 > cnt - 1) ? 0 : j + 1;
    }
    return pts[j].y;
}

/* 主入口：返回生成的跨度数 */
int fill_scanline_spans(const Vtx *poly, int n, int ylo, int yhi, SpanList *out)
{
    if (n < 3) return 0;

    int ymin = poly[0].y, ymax = poly[0].y;
    for (int i = 1; i < n; ++i) {
        if (poly[i].y < ymin) ymin = poly[i].y;
        if (poly[i].y > ymax) ymax = poly[i].y;
    }
    if (ylo > ymin) ymin = ylo;
    if (yhi < ymax) ymax = yhi;
    if (ymax < ymin) return 0;

    int span = ymax - ymin + 1;
    int n0 = out->n;

    int *bucket = (int *)xrealloc(NULL, sizeof(int) * (size_t)span);
    for (int i = 0; i < span; ++i) bucket[i] = -1;

    Edge *pool = (Edge *)xrealloc(NULL, sizeof(Edge) * (size_t)(n + 1));
    int ecount = 0;

    /* 建边：每条非水平边在其下端 y 入桶（p1 的 buildEdgeList 语义） */
    int yprev = poly[n - 2].y;
    Vtx v1 = poly[n - 1];
    for (int i = 0; i < n; ++i) {
        Vtx v2 = poly[i];
        if (v1.y != v2.y) {
            Vtx lower, upper;
            if (v1.y < v2.y) { lower = v1; upper = v2; }
            else             { lower = v2; upper = v1; }
            Edge *e = &pool[ecount];
            e->dxPerScan = (double)(upper.x - lower.x) / (double)(upper.y - lower.y);
            e->xIntersect = (double)lower.x;
            int yComp = (v1.y < v2.y) ? y_next(poly, n, i) : yprev;
            e->yUpper = (upper.y < yComp) ? upper.y - 1 : upper.y;
            e->next = -1;
            e->active_next = -1;
            int b = lower.y - ymin;
            if (b < 0) b = 0;
            if (b < span) { e->next = bucket[b]; bucket[b] = ecount; ecount++; }
        }
        yprev = v1.y;
        v1 = v2;
    }

    /* AET：小数组 + 插入排序，活跃边数通常 < 16，比链表更快 */
    int   actCap = ecount > 0 ? ecount : 1;
    int  *act = (int *)xrealloc(NULL, sizeof(int) * (size_t)actCap);
    int   an = 0;

    for (int scan = ymin; scan <= ymax; ++scan) {
        /* 入桶边并入 AET */
        for (int k = bucket[scan - ymin]; k != -1; k = pool[k].next) {
            if (an < actCap) act[an++] = k;
        }
        if (an < 2) {
            /* 仍需更新/淘汰现存边 */
            int w = 0;
            for (int i = 0; i < an; ++i) {
                Edge *e = &pool[act[i]];
                if (scan >= e->yUpper) continue;
                e->xIntersect += e->dxPerScan;
                act[w++] = act[i];
            }
            an = w;
            continue;
        }
        /* 按 xIntersect 插入排序（an 很小） */
        for (int i = 1; i < an; ++i) {
            int key = act[i];
            double kx = pool[key].xIntersect;
            int j = i - 1;
            while (j >= 0 && pool[act[j]].xIntersect > kx) { act[j + 1] = act[j]; --j; }
            act[j + 1] = key;
        }
        /* 成对填跨度 */
        for (int i = 0; i + 1 < an; i += 2) {
            int xa = (int)ceil(pool[act[i]].xIntersect);
            int xb = (int)floor(pool[act[i + 1]].xIntersect);
            if (xa <= xb) span_merge_push(out, scan, xa, xb);
        }
        /* 更新 / 淘汰 */
        int w = 0;
        for (int i = 0; i < an; ++i) {
            Edge *e = &pool[act[i]];
            if (scan >= e->yUpper) continue;
            e->xIntersect += e->dxPerScan;
            act[w++] = act[i];
        }
        an = w;
    }

    free(act);
    free(pool);
    free(bucket);
    return out->n - n0;
}

/* 直接写像素缓冲（1 = 填充），逻辑与上面一致但输出到 int 缓冲 */
int fill_scanline_flat(int *buf, int w, int h, const Vtx *poly, int n)
{
    SpanList sl;
    span_init(&sl, 64);
    int cnt = fill_scanline_spans(poly, n, 0, h - 1, &sl);
    for (int i = 0; i < sl.n; ++i) {
        int y = sl.s[i].y;
        int xs = sl.s[i].xs, xe = sl.s[i].xe;
        if (y < 0 || y >= h) continue;
        if (xs < 0) xs = 0;
        if (xe > w - 1) xe = w - 1;
        int *row = buf + (size_t)y * (size_t)w;
        for (int x = xs; x <= xe; ++x) row[x] = 1;
        cnt += xe - xs + 1;
    }
    span_free(&sl);
    return cnt;
}

/* 种子填充（4 连通，显式栈 + 行程加速） */
int fill_seed(int *buf, int w, int h, int x, int y, int border, int value)
{
    if (x < 0 || y < 0 || x >= w || y >= h) return 0;
    if (buf[(size_t)y * w + x] == border || buf[(size_t)y * w + x] == value) return 0;

    int cap = 1024, top = 0, cnt = 0;
    int *st = (int *)xrealloc(NULL, sizeof(int) * (size_t)(cap * 2));
    st[top * 2] = x; st[top * 2 + 1] = y; top++;

    while (top) {
        --top;
        int cx = st[top * 2], cy = st[top * 2 + 1];
        if (cx < 0 || cy < 0 || cx >= w || cy >= h) continue;
        int idx = cy * w + cx;
        if (buf[idx] == border || buf[idx] == value) continue;

        /* 向左右扩展成行程，减少栈操作 */
        int xs = cx, xe = cx;
        while (xs - 1 >= 0 && buf[cy * w + xs - 1] != border && buf[cy * w + xs - 1] != value) --xs;
        while (xe + 1 < w && buf[cy * w + xe + 1] != border && buf[cy * w + xe + 1] != value) ++xe;
        for (int i = xs; i <= xe; ++i) buf[cy * w + i] = value;
        cnt += xe - xs + 1;

        /* 上下两行按行程入栈 */
        for (int d = -1; d <= 1; d += 2) {
            int ny = cy + d;
            if (ny < 0 || ny >= h) continue;
            int i = xs;
            while (i <= xe) {
                if (buf[ny * w + i] != border && buf[ny * w + i] != value) {
                    if (top + 4 >= cap) {
                        cap *= 2;
                        st = (int *)xrealloc(st, sizeof(int) * (size_t)(cap * 2));
                    }
                    st[top * 2] = i; st[top * 2 + 1] = ny; top++;
                    while (i <= xe && buf[ny * w + i] != border && buf[ny * w + i] != value) ++i;
                } else ++i;
            }
        }
    }
    free(st);
    return cnt;
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

/* 算法16：Cohen-Sutherland，左/右/下/上顺序求交 */
int clip_cohen_sutherland(Dpt *p0, Dpt *p1, double xl, double yb, double xr, double yt,
                          Dpt *o0, Dpt *o1)
{
    Dpt a = *p0, b = *p1;
    int ca = encode_code(a.x, a.y, xl, yb, xr, yt);
    int cb = encode_code(b.x, b.y, xl, yb, xr, yt);
    int swap = 0;
    for (int guard = 0; guard < 64; ++guard) {
        if ((ca | cb) == 0) {                 /* 全部可见 */
            if (swap) { *o0 = b; *o1 = a; } else { *o0 = a; *o1 = b; }
            return 1;
        }
        if ((ca & cb) != 0) return 0;         /* 简弃 */
        if (ca == 0) {                        /* 保证 a 在窗外 */
            Dpt t = a; a = b; b = t;
            int c = ca; ca = cb; cb = c;
            swap ^= 1;
        }
        Dpt ip;
        if (ca & CLIP_LEFT)        { ip.x = xl; ip.y = (ip.x - a.x) * (b.y - a.y) / (b.x - a.x) + a.y; }
        else if (ca & CLIP_RIGHT)  { ip.x = xr; ip.y = (ip.x - a.x) * (b.y - a.y) / (b.x - a.x) + a.y; }
        else if (ca & CLIP_BOTTOM) { ip.y = yb; ip.x = (ip.y - a.y) * (b.x - a.x) / (b.y - a.y) + a.x; }
        else                       { ip.y = yt; ip.x = (ip.y - a.y) * (b.x - a.x) / (b.y - a.y) + a.x; }
        a = ip;
        ca = encode_code(a.x, a.y, xl, yb, xr, yt);
    }
    return 0;
}

/* 算法17：中点分割（二分逼近），分别把两端点推向可见侧 */
static int mid_probe(Dpt *p, Dpt q, double xl, double yb, double xr, double yt, int depth)
{
    if (depth > 24) return encode_code(p->x, p->y, xl, yb, xr, yt) == 0;
    int cq = encode_code(q.x, q.y, xl, yb, xr, yt);
    if (cq == 0) { *p = q; return 1; }                 /* q 可见 → 整段可见 */
    int cp = encode_code(p->x, p->y, xl, yb, xr, yt);
    if ((cp & cq) != 0) return 0;                      /* 同侧，不可见 */

    Dpt mid;
    mid.x = (p->x + q.x) * 0.5;
    mid.y = (p->y + q.y) * 0.5;
    Dpt save = *p;
    *p = mid;
    if (mid_probe(p, q, xl, yb, xr, yt, depth + 1)) return 1;
    *p = save;
    return mid_probe(p, mid, xl, yb, xr, yt, depth + 1);
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
    if (ca != 0 && !mid_probe(&sa, b, xl, yb, xr, yt, 0)) return 0;
    if (cb != 0 && !mid_probe(&sb, a, xl, yb, xr, yt, 0)) return 0;

    /* 统一端点方向：使 o0 靠近原始 p0，便于三种算法直接对比 */
    double d_ab = (sa.x - a.x) * (sa.x - a.x) + (sa.y - a.y) * (sa.y - a.y);
    double d_ba = (sb.x - a.x) * (sb.x - a.x) + (sb.y - a.y) * (sb.y - a.y);
    if (d_ab <= d_ba) { *o0 = sa; *o1 = sb; }
    else              { *o0 = sb; *o1 = sa; }
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
    p[0] = -dx;  q[0] = x0 - xl;
    p[1] =  dx;  q[1] = xr - x0;
    p[2] = -dy;  q[2] = y0 - yb;
    p[3] =  dy;  q[3] = yt - y0;
    for (int i = 0; i < 4; ++i) {
        if (p[i] == 0.0) {
            if (q[i] < 0.0) return 0;      /* 平行且在外侧 */
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
