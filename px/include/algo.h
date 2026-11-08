/* algo.h —— 图形学算法库（Qt 版，纯算法，不依赖 GUI）
 *
 * 对应 p1 各 MFC 工程 + zip 算法包：
 *   DDA            <- zip/算法2：通用直线的DDA算法
 *   Bresenham      <- p1/DrawLine/DrawLineView.cpp::getPoints
 *   MidpointCircle <- p1/DrawLine/DrawLineView.cpp::getCirclePoints
 *   ScanlineFill   <- p1/Fill/FillView.cpp（重写为 AET，去掉每帧重建）
 *   CohenSutherland<- zip/算法16
 *   MidpointSplit  <- zip/算法17
 *   LiangBarsky    <- zip/算法18
 */
#ifndef ALGO_H
#define ALGO_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 点列容器：算法输出设备坐标点列（y 向下），由调用方负责坐标系换算 */
typedef struct {
    int *x, *y;
    int  n, cap;
} PtList;

void pl_init(PtList *l, int cap);
void pl_free(PtList *l);
void pl_clear(PtList *l);
void pl_push(PtList *l, int x, int y);
void pl_reserve(PtList *l, int cap);

/* ---- 直线 ---- */
int line_dda(int x0, int y0, int x1, int y1, PtList *out);
int line_bresenham(int x0, int y0, int x1, int y1, PtList *out);
int line_midpoint(int x0, int y0, int x1, int y1, PtList *out);

/* ---- 圆 ---- */
int circle_midpoint(int cx, int cy, int r, PtList *out);
int circle_bresenham(int cx, int cy, int r, PtList *out);

/* ---- 多边形填充 ----
 * fill_span_rows：一次算完所有扫描线跨度，输出到 spans（每行 start/len），
 * 相比逐点输出可减少 5~8 倍的点列写入量。 */
typedef struct { int y, xs, xe; } Span;
typedef struct {
    Span *s;
    int   n, cap;
} SpanList;

void span_init(SpanList *l, int cap);
void span_free(SpanList *l);
void span_clear(SpanList *l);
void span_push(SpanList *l, int y, int xs, int xe);

typedef struct { int x, y; } Vtx;

int  fill_scanline_spans(const Vtx *poly, int n, int ylo, int yhi, SpanList *out);
int  fill_scanline_flat(int *buf, int w, int h, const Vtx *poly, int n);
int  fill_seed(int *buf, int w, int h, int x, int y, int border, int value);

/* ---- 裁剪 ---- */
typedef struct { double x, y; } Dpt;
enum { CLIP_LEFT = 1, CLIP_RIGHT = 2, CLIP_BOTTOM = 4, CLIP_TOP = 8 };

int encode_code(double x, double y, double xl, double yb, double xr, double yt);
int clip_cohen_sutherland(Dpt *p0, Dpt *p1, double xl, double yb, double xr, double yt,
                          Dpt *o0, Dpt *o1);
int clip_midpoint(Dpt *p0, Dpt *p1, double xl, double yb, double xr, double yt,
                  Dpt *o0, Dpt *o1);
int clip_liang_barsky(double x0, double y0, double x1, double y1,
                      double xl, double yb, double xr, double yt,
                      double *ox0, double *oy0, double *ox1, double *oy1);

#ifdef __cplusplus
}
#endif

#endif
