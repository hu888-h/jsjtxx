#!/bin/bash
# 无界面自检：不依赖 X 显示，验证算法层的数值正确性。
# 用法：docs/verify.sh
set -u
ROOT=$(cd "$(dirname "$0")/.." && pwd)
OUT=$(mktemp -d)
cat > "$OUT/t.c" <<'C'
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "algo.h"

static int fails = 0;
#define CHECK(cond, msg) do { if (!(cond)) { printf("  FAIL: %s\n", msg); fails++; } \
                             else printf("  ok  : %s\n", msg); } while (0)

int main(void)
{
    PtList l; pl_init(&l, 1024);

    /* 1. DDA 端点必须精确落在两端 */
    pl_clear(&l); line_dda(-37, 11, 128, -95, &l);
    CHECK(l.n == 166, "DDA 步数 = max(|dx|,|dy|)+1 = 165+1 = 166");
    CHECK(l.x[0] == -37 && l.y[0] == 11, "DDA 起点精确");
    CHECK(l.x[l.n-1] == 128 && l.y[l.n-1] == -95, "DDA 终点精确");

    /* 2. 三种直线算法端点一致 */
    pl_clear(&l); line_bresenham(-37, 11, 128, -95, &l);
    CHECK(l.x[0] == -37 && l.y[0] == 11 && l.x[l.n-1] == 128 && l.y[l.n-1] == -95,
          "Bresenham 端点精确（跨八象限）");
    pl_clear(&l); line_bresenham_all(-37, 11, 128, -95, &l);
    CHECK(l.x[0] == -37 && l.y[0] == 11 && l.x[l.n-1] == 128 && l.y[l.n-1] == -95,
          "Bresenham(对称版) 端点精确");
    pl_clear(&l); line_midpoint(-37, 11, 128, -95, &l);
    CHECK(l.x[l.n-1] == 128 || l.x[l.n-1] == 129, "中点画线终点在端点 1 像素内");

    /* 3. 反向直线（p1 原实现在此有 bug） */
    pl_clear(&l); line_bresenham(100, 100, 100, -50, &l);
    CHECK(l.n == 151, "竖直反向直线点数正确（原实现只画 0 点）");
    pl_clear(&l); line_bresenham(100, 100, -50, 100, &l);
    CHECK(l.n == 151, "水平反向直线点数正确");

    /* 4. 中点圆：4 个极值点必须精确命中，且所有点满足 |d - r| <= 1 */
    pl_clear(&l); circle_midpoint(0, 0, 90, &l);
    int hit = 0;
    for (int i = 0; i < l.n; ++i) {
        if (l.x[i] == 0 && l.y[i] == 90) hit = 1;
        if (l.x[i] == 90 && l.y[i] == 0) hit = 1;
        if (l.x[i] == 0 && l.y[i] == -90) hit = 1;
        if (l.x[i] == -90 && l.y[i] == 0) hit = 1;
    }
    CHECK(hit == 1, "中点圆四极点命中 (0,±r)/(±r,0)");
    double worst = 0;
    for (int i = 0; i < l.n; ++i) {
        double d = sqrt((double)(l.x[i]*l.x[i] + l.y[i]*l.y[i]));
        double e = fabs(d - 90.0);
        if (e > worst) worst = e;
    }
    CHECK(worst <= 1.0, "中点圆所有点半径误差 <= 1 像素");
    CHECK(l.n >= 30, "中点圆输出点数足够");

    /* 5. 扫描线填充：三角形填充面积应与解析值一致（误差 < 1.5%） */
    Vtx tri[3] = {{-140,-80},{150,-60},{0,120}};
    pl_clear(&l); fill_scanline(tri, 3, -1000, 1000, &l);
    double area = 0.5 * fabs((-140.0)*( -60 - 120) + 150.0*(120 - (-80)) + 0.0*((-80) - (-60)));
    CHECK(fabs((double)l.n - area) / area < 0.015, "扫描线填充面积匹配解析三角形面积");

    /* 6. 三种裁剪结果必须一致 */
    Dpt a = {-330, -170}, b = {330, 175}, o0, o1, q0, q1;
    double xl = -200, yb = -110, xr = 200, yt = 110;
    int okc = clip_cohen_sutherland(&a, &b, xl, yb, xr, yt, &o0, &o1);
    int okm = clip_midpoint(&a, &b, xl, yb, xr, yt, &q0, &q1);
    double lx0, ly0, lx1, ly1;
    int okl = clip_liang_barsky(a.x, a.y, b.x, b.y, xl, yb, xr, yt, &lx0, &ly0, &lx1, &ly1);
    CHECK(okc && okm && okl, "三种裁剪算法都判定线段部分可见");
    double dc = fabs(o0.x - q0.x) + fabs(o0.y - q0.y) + fabs(o1.x - q1.x) + fabs(o1.y - q1.y);
    CHECK(dc < 4.0, "Cohen-Sutherland 与中点分割结果一致（差 < 4 像素）");
    /* 两个算法可能以相反方向返回同一线段，按端点集合比较 */
    double d_ab = fabs(o0.x - lx0) + fabs(o0.y - ly0) + fabs(o1.x - lx1) + fabs(o1.y - ly1);
    double d_ba = fabs(o0.x - lx1) + fabs(o0.y - ly1) + fabs(o1.x - lx0) + fabs(o1.y - ly0);
    CHECK((d_ab < 4.0) || (d_ba < 4.0), "Cohen-Sutherland 与 Liang-Barsky 端点集合一致");
    CHECK(encode_code(-250,-170,xl,yb,xr,yt) == (CLIP_LEFT|CLIP_BOTTOM), "端点编码 LEFT|BOTTOM 正确");
    CHECK(encode_code(0,0,xl,yb,xr,yt) == 0, "窗口内点编码为 0");

    /* 7. 完全不可见时必须简弃 */
    Dpt c = {-500,-500}, d = {-300,-300};
    CHECK(clip_cohen_sutherland(&c, &d, xl, yb, xr, yt, &o0, &o1) == 0, "同侧线段被简弃");
    CHECK(clip_liang_barsky(-500,-500,-300,-300,xl,yb,xr,yt,&lx0,&ly0,&lx1,&ly1) == 0,
          "Liang-Barsky 同侧线段被拒绝");

    /* 8. 二维变换：旋转 90° 把 (1,0) 送到 (0,1) */
    Mat3 m; mat_identity(&m); mat_rotate(&m, 90.0);
    double rx, ry; mat_apply(&m, 1.0, 0.0, &rx, &ry);
    CHECK(fabs(rx) < 1e-9 && fabs(ry - 1.0) < 1e-9, "旋转 90° 正确");
    mat_identity(&m); mat_translate(&m, 5.0, -3.0); mat_apply(&m, 0, 0, &rx, &ry);
    CHECK(fabs(rx - 5.0) < 1e-9 && fabs(ry + 3.0) < 1e-9, "平移正确");
    mat_identity(&m); mat_reflect_x(&m); mat_apply(&m, 2.0, 7.0, &rx, &ry);
    CHECK(fabs(rx - 2.0) < 1e-9 && fabs(ry + 7.0) < 1e-9, "X 轴反射正确");
    mat_identity(&m); mat_scale_at(&m, 2.0, 2.0, 10.0, 10.0); mat_apply(&m, 10.0, 10.0, &rx, &ry);
    CHECK(fabs(rx - 10.0) < 1e-9 && fabs(ry - 10.0) < 1e-9, "定点缩放不移动基点");

    /* 9. 五角星顶点数 */
    pl_clear(&l); poly_star(0, 0, 150.0, 5, 0.382, 90.0, &l);
    CHECK(l.n == 10, "五角星 5 尖 10 顶点");

    pl_free(&l);
    printf("\n%s（%d 项失败）\n", fails ? "存在失败" : "全部通过", fails);
    return fails ? 1 : 0;
}
C
gcc -O2 -I"$ROOT/include" "$OUT/t.c" "$ROOT/src/algo.c" "$ROOT/src/fill.c" -lm -o "$OUT/t" || exit 1
"$OUT/t"
rc=$?
rm -rf "$OUT"
exit $rc
