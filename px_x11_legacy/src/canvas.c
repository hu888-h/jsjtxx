/* canvas.c —— CDC / CClientDC 的软件等价实现
 * 所有图元换算成设备坐标（客户区像素，原点左上，y 向下），与 MFC MM_TEXT 默认映射一致；
 * 数学坐标系（y 向上）由各算法层自行翻转。
 */
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "px.h"

static inline unsigned umul(unsigned a, unsigned b) { return (unsigned)a * (unsigned)b; }

void cv_init(Canvas *c, int w, int h)
{
    if (w < 1) w = 1;
    if (h < 1) h = 1;
    c->w = w; c->h = h;
    c->rop = CV_COPY;
    c->px = (Color *)calloc(umul(w, h), sizeof(Color));
}

void cv_free(Canvas *c)
{
    free(c->px);
    c->px = NULL;
    c->w = c->h = 0;
}

void cv_resize(Canvas *c, int w, int h)
{
    if (w < 1) w = 1;
    if (h < 1) h = 1;
    if (w == c->w && h == c->h) return;
    Color *n = (Color *)calloc(umul(w, h), sizeof(Color));
    int copyw = w < c->w ? w : c->w;
    int copyh = h < c->h ? h : c->h;
    if (c->px) {
        for (int y = 0; y < copyh; ++y)
            memcpy(n + umul(y, w), c->px + umul(y, c->w), umul(copyw, sizeof(Color)));
    }
    free(c->px);
    c->px = n; c->w = w; c->h = h;
}

void cv_set_rop(Canvas *c, int rop) { c->rop = rop; }

void cv_clear(Canvas *c, Color col)
{
    for (int i = 0; i < c->w * c->h; ++i) c->px[i] = col;
}

/* R2_NOT 语义：与已有内容取反（这里是异或，用于橡皮筋预览擦除/重绘） */
void cv_pixel(Canvas *c, int x, int y, Color col)
{
    if ((unsigned)x >= (unsigned)c->w || (unsigned)y >= (unsigned)c->h) return;
    unsigned i = umul(c->w, y) + x;
    if (c->rop == CV_XOR)
        c->px[i] = (c->px[i] ^ 0x00FFFFFFu ^ (col & 0x00FFFFFFu)) | 0xFF000000u;
    else
        c->px[i] = col;
}

static inline void plot(Canvas *c, int x, int y, Color col)
{
    if ((unsigned)x >= (unsigned)c->w || (unsigned)y >= (unsigned)c->h) return;
    unsigned i = umul(c->w, y) + x;
    if (c->rop == CV_XOR)
        c->px[i] = (c->px[i] ^ 0x00FFFFFFu ^ (col & 0x00FFFFFFu)) | 0xFF000000u;
    else
        c->px[i] = col;
}

/* Bresenham，任意斜率 */
void cv_line(Canvas *c, int x0, int y0, int x1, int y1, Color col)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    for (;;) {
        plot(c, x0, y0, col);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void cv_hline(Canvas *c, int x, int y, int len, Color col)
{
    if (len < 0) { x += len; len = -len; }
    for (int i = 0; i < len; ++i) plot(c, x + i, y, col);
}

void cv_vline(Canvas *c, int x, int y, int len, Color col)
{
    if (len < 0) { y += len; len = -len; }
    for (int i = 0; i < len; ++i) plot(c, x, y + i, col);
}

void cv_rect(Canvas *c, Rect r, Color col)
{
    if (r.w <= 0 || r.h <= 0) return;
    cv_hline(c, r.x, r.y, r.w, col);
    cv_hline(c, r.x, r.y + r.h - 1, r.w, col);
    cv_vline(c, r.x, r.y, r.h, col);
    cv_vline(c, r.x + r.w - 1, r.y, r.h, col);
}

void cv_fill_rect(Canvas *c, Rect r, Color col)
{
    if (r.w <= 0 || r.h <= 0) return;
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) plot(c, x, y, col);
}

/* 中点圆（8 对称），与 p1/DrawLine 的 getCirclePoints 同一算法 */
void cv_circle(Canvas *c, int cx, int cy, int r, Color col)
{
    if (r <= 0) { plot(c, cx, cy, col); return; }
    int x = 0, y = r, d = 1 - r;
    while (x < y) {
        plot(c, cx + x, cy + y, col); plot(c, cx - x, cy + y, col);
        plot(c, cx + x, cy - y, col); plot(c, cx - x, cy - y, col);
        plot(c, cx + y, cy + x, col); plot(c, cx - y, cy + x, col);
        plot(c, cx + y, cy - x, col); plot(c, cx - y, cy - x, col);
        ++x;
        if (d < 0) d += 2 * x + 1;
        else { --y; d += 2 * (x - y) + 1; }
    }
    plot(c, cx + x, cy + y, col); plot(c, cx - x, cy + y, col);
    plot(c, cx + x, cy - y, col); plot(c, cx - x, cy - y, col);
}

void cv_fill_circle(Canvas *c, int cx, int cy, int r, Color col)
{
    for (int y = -r; y <= r; ++y) {
        int w = (int)(sqrt((double)(r * r - y * y)));
        cv_hline(c, cx - w, cy + y, 2 * w + 1, col);
    }
}

/* 虚线：用于裁剪窗口边界、辅助线 */
void cv_dashes(Canvas *c, int x0, int y0, int x1, int y1, int on, int off, Color col)
{
    if (on < 1) on = 1;
    if (off < 1) off = 1;
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, state = 1, cnt = 0;
    for (;;) {
        if (state) plot(c, x0, y0, col);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
        if (++cnt >= (state ? on : off)) {
            cnt = 0; state = !state;
        }
    }
}

/* 端点标记：小于 3 像素时中点圆会退化成十字，这里显式画实心圆点 */
void cv_marker(Canvas *c, int x, int y, int r, Color col)
{
    if (r < 3) {
        for (int dy = -1; dy <= 1; ++dy)
            for (int dx = -1; dx <= 1; ++dx) plot(c, x + dx, y + dy, col);
        return;
    }
    int x0 = 0, y0 = r, d = 1 - r;
    while (x0 < y0) {
        plot(c, x + x0, y + y0, col); plot(c, x - x0, y + y0, col);
        plot(c, x + x0, y - y0, col); plot(c, x - x0, y - y0, col);
        plot(c, x + y0, y + x0, col); plot(c, x - y0, y + x0, col);
        plot(c, x + y0, y - x0, col); plot(c, x - y0, y - x0, col);
        ++x0;
        if (d < 0) d += 2 * x0 + 1;
        else { --y0; d += 2 * (x0 - y0) + 1; }
    }
}

void cv_blend_pixel(Canvas *c, int x, int y, Color col, int alpha)
{
    if ((unsigned)x >= (unsigned)c->w || (unsigned)y >= (unsigned)c->h) return;
    if (alpha <= 0) return;
    if (alpha > 255) alpha = 255;
    unsigned i = umul(c->w, y) + x;
    Color d = c->px[i];
    unsigned ia = (unsigned)alpha, sa = 255u - ia;
    unsigned r = (PX_R(col) * ia + PX_R(d) * sa) / 255u;
    unsigned g = (PX_G(col) * ia + PX_G(d) * sa) / 255u;
    unsigned b = (PX_B(col) * ia + PX_B(d) * sa) / 255u;
    c->px[i] = PX_RGB(r, g, b);
}
