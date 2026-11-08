/* algo.h —— 图形学算法库（对应 p1 各 MFC 工程 + zip 算法包）
 * 所有算法输出整数设备坐标点列，坐标系变换由调用方负责（与 p1 的 getPoints() 一致）。
 */
#ifndef ALGO_H
#define ALGO_H

#include <stddef.h>
#include <stdint.h>

/* 点列容器：算法只追加，不关心容量上限（由 px 端保证 n 足够） */
typedef struct {
    int *x, *y;
    int  n, cap;
} PtList;

void pl_init(PtList *l, int cap);
void pl_free(PtList *l);
void pl_clear(PtList *l);
void pl_push(PtList *l, int x, int y);

/* ---- 直线 ---- */
int  line_dda(int x0,int y0,int x1,int y1, PtList *out);          /* 算法2 通用DDA */
int  line_bresenham(int x0,int y0,int x1,int y1, PtList *out);    /* p1/DrawLine getPoints（八象限整数版）*/
int  line_bresenham_all(int x0,int y0,int x1,int y1, PtList *out);/* 全八象限（整型对称实现）*/
int  line_midpoint(int x0,int y0,int x1,int y1, PtList *out);     /* 中点画线 */

/* ---- 圆 / 椭圆 ---- */
int  circle_midpoint(int cx,int cy,int r, PtList *out);           /* p1/DrawLine getCirclePoints */
int  circle_bresenham(int cx,int cy,int r, PtList *out);
int  ellipse_midpoint(int cx,int cy,int a,int b, PtList *out);

/* ---- 多边形填充 ---- */
typedef struct { int x, y; } Vtx;
int  fill_scanline(const Vtx *poly, int n, int y0, int y1, PtList *out);  /* 扫描线种子/ET 算法 */
int  fill_scanline_flat(int *buf, int w, int h, const Vtx *poly, int n); /* 直接写像素缓冲 */
int  fill_seed(int *buf, int w, int h, int x, int y, int border, int value); /* 种子填充 */

/* ---- 裁剪 ---- */
typedef struct { double x, y; } Dpt;
enum { CLIP_LEFT=1, CLIP_RIGHT=2, CLIP_BOTTOM=4, CLIP_TOP=8 };

int  clip_cohen_sutherland(Dpt *p0, Dpt *p1,
                           double xl, double yb, double xr, double yt,
                           Dpt *o0, Dpt *o1);                    /* 算法16 */
int  clip_midpoint(Dpt *p0, Dpt *p1,
                   double xl, double yb, double xr, double yt,
                   Dpt *o0, Dpt *o1);                            /* 算法17 中点分割 */
int  clip_liang_barsky(double x0,double y0,double x1,double y1,
                       double xl,double yb,double xr,double yt,
                       double *ox0,double *oy0,double *ox1,double *oy1); /* 算法18 */
int  encode_code(double x, double y, double xl, double yb, double xr, double yt);

/* ---- 二维几何变换（算法15） ---- */
typedef struct { double m[3][3]; } Mat3;
void mat_identity(Mat3 *m);
void mat_mul(Mat3 *a, const Mat3 *b, Mat3 *out);
void mat_translate(Mat3 *m, double tx, double ty);
void mat_scale(Mat3 *m, double sx, double sy);
void mat_scale_at(Mat3 *m, double sx, double sy, double px, double py);
void mat_rotate(Mat3 *m, double deg);
void mat_rotate_at(Mat3 *m, double deg, double px, double py);
void mat_reflect_origin(Mat3 *m);
void mat_reflect_x(Mat3 *m);
void mat_reflect_y(Mat3 *m);
void mat_shear(Mat3 *m, double b, double c);
void mat_apply(const Mat3 *m, double x, double y, double *ox, double *oy);

/* ---- 常用多边形 ---- */
int  poly_star(double cx, double cy, double r, int spikes, double inner_ratio,
               double rot_deg, PtList *out);      /* 算法15 五角星 */

#endif
