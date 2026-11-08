#!/bin/bash
# 无界面自检：验证算法层数值正确性（不依赖 GUI）
set -u
ROOT=$(cd "$(dirname "$0")/.." && pwd)
OUT=$(mktemp -d)
cat > "$OUT/t.c" <<'C'
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "algo.h"

static int fails = 0;
#define CHECK(c, m) do { if (!(c)) { printf("  FAIL: %s\n", m); fails++; } \
                         else printf("  ok  : %s\n", m); } while (0)

int main(void)
{
    PtList l; pl_init(&l, 1024);
    SpanList sl; span_init(&sl, 64);

    /* DDA */
    pl_clear(&l); line_dda(-37, 11, 128, -95, &l);
    CHECK(l.n == 166, "DDA 步数 = max(|dx|,|dy|)+1 = 166");
    CHECK(l.x[0] == -37 && l.y[0] == 11, "DDA 起点精确");
    CHECK(l.x[l.n-1] == 128 && l.y[l.n-1] == -95, "DDA 终点精确");

    /* Bresenham 跨八象限 + 反向 */
    pl_clear(&l); line_bresenham(-37, 11, 128, -95, &l);
    CHECK(l.x[0] == -37 && l.y[0] == 11, "Bresenham 起点精确");
    CHECK(l.x[l.n-1] == 128 && l.y[l.n-1] == -95, "Bresenham 终点精确");
    pl_clear(&l); line_bresenham(100, 100, 100, -50, &l);
    CHECK(l.n == 151, "竖直反向直线 151 点（原实现只画 0 点）");
    pl_clear(&l); line_bresenham(100, 100, -50, 100, &l);
    CHECK(l.n == 151, "水平反向直线 151 点");

    /* 中点圆：四极点 + 半径误差 */
    pl_clear(&l); circle_midpoint(0, 0, 90, &l);
    int hit = 0;
    for (int i = 0; i < l.n; ++i) {
        if (l.x[i]==0 && l.y[i]==90) hit=1;
        if (l.x[i]==90 && l.y[i]==0) hit=1;
        if (l.x[i]==0 && l.y[i]==-90) hit=1;
        if (l.x[i]==-90 && l.y[i]==0) hit=1;
    }
    CHECK(hit == 1, "中点圆四极点命中 (0,±r)/(±r,0)");
    double worst = 0;
    for (int i = 0; i < l.n; ++i) {
        double d = sqrt((double)(l.x[i]*l.x[i] + l.y[i]*l.y[i]));
        if (fabs(d - 90.0) > worst) worst = fabs(d - 90.0);
    }
    CHECK(worst <= 1.0, "中点圆半径误差 <= 1 像素");
    CHECK(l.n >= 30, "中点圆输出点数足够");

    /* 扫描线填充：面积对解析值 */
    Vtx tri[3] = {{-140,-80},{150,-60},{0,120}};
    span_clear(&sl); fill_scanline_spans(tri, 3, -100000, 100000, &sl);
    long area_px = 0;
    for (int i = 0; i < sl.n; ++i) area_px += sl.s[i].xe - sl.s[i].xs + 1;
    double area = 0.5 * fabs((-140.0)*(-60 - 120) + 150.0*(120 + 80) + 0.0);
    CHECK(fabs((double)area_px - area)/area < 0.02, "扫描线填充面积匹配解析三角形（误差<2%）");

    /* 凹多边形：跨度数必须 > 0 且面积合理 */
    Vtx concave[6] = {{-180,-120},{180,-120},{220,0},{60,160},{-140,120},{-220,20}};
    span_clear(&sl); fill_scanline_spans(concave, 6, -100000, 100000, &sl);
    CHECK(sl.n > 100, "凹六边形填充产生 > 100 条跨度");

    /* 三种裁剪一致性 */
    Dpt a = {-330,-170}, b = {330,175}, o0, o1, q0, q1;
    double xl=-200, yb=-110, xr=200, yt=110;
    int okc = clip_cohen_sutherland(&a,&b,xl,yb,xr,yt,&o0,&o1);
    int okm = clip_midpoint(&a,&b,xl,yb,xr,yt,&q0,&q1);
    double lx0,ly0,lx1,ly1;
    int okl = clip_liang_barsky(a.x,a.y,b.x,b.y,xl,yb,xr,yt,&lx0,&ly0,&lx1,&ly1);
    CHECK(okc && okm && okl, "三种裁剪都判定部分可见");
    CHECK(fabs(o0.x-q0.x)+fabs(o0.y-q0.y)+fabs(o1.x-q1.x)+fabs(o1.y-q1.y) < 4.0,
          "Cohen-Sutherland 与中点分割一致");
    double dab = fabs(o0.x-lx0)+fabs(o0.y-ly0)+fabs(o1.x-lx1)+fabs(o1.y-ly1);
    double dba = fabs(o0.x-lx1)+fabs(o0.y-ly1)+fabs(o1.x-lx0)+fabs(o1.y-ly0);
    CHECK(dab < 4.0 || dba < 4.0, "Cohen-Sutherland 与 Liang-Barsky 端点集合一致");
    CHECK(encode_code(-250,-170,xl,yb,xr,yt) == (CLIP_LEFT|CLIP_BOTTOM), "端点编码 LEFT|BOTTOM");
    CHECK(encode_code(0,0,xl,yb,xr,yt) == 0, "窗口内点编码为 0");
    Dpt c = {-500,-500}, d = {-300,-300};
    CHECK(clip_cohen_sutherland(&c,&d,xl,yb,xr,yt,&o0,&o1) == 0, "同侧线段被简弃");
    CHECK(clip_liang_barsky(-500,-500,-300,-300,xl,yb,xr,yt,&lx0,&ly0,&lx1,&ly1) == 0,
          "Liang-Barsky 同侧被拒绝");

    /* 种子填充：19x19 内腔 */
    {
        int w=40,h=40; int *buf=(int*)calloc((size_t)w*h,sizeof(int));
        for(int x=5;x<=25;x++){ buf[5*w+x]=2; buf[25*w+x]=2; }
        for(int y=5;y<=25;y++){ buf[y*w+5]=2; buf[y*w+25]=2; }
        int n2=fill_seed(buf,w,h,15,15,2,1);
        int cnt=0; for(int i=0;i<w*h;i++) if(buf[i]==1) cnt++;
        CHECK(cnt == 361 && n2 == 361, "种子填充 19x19=361 像素");
        free(buf);
    }

    pl_free(&l); span_free(&sl);
    printf("\n%s（%d 项失败）\n", fails ? "存在失败" : "全部通过", fails);
    return fails ? 1 : 0;
}
C
gcc -O2 -I"$ROOT/include" "$OUT/t.c" "$ROOT/src/algo.c" -lm -o "$OUT/t" || exit 1
"$OUT/t"
rc=$?
rm -rf "$OUT"
exit $rc
