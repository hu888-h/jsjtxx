/* fill.c —— 多边形扫描线填充 + 种子填充
 * 对齐 p1/Fill/FillView.cpp：
 *   Edge 结构 (yUpper / xIntersect / dxPerScan)
 *   buildEdgeList() 建 ET 表（按 y 归桶）
 *   makeEdgeRec()   计算斜率倒数与上端 y
 *   insertEdge()    按 xIntersect 升序插入链表
 *   buildActiveList/ fillScan / updateActiveList / resortActiveList
 *   scanFill()      主循环
 * 原实现用全局 window[] 且 malloc(sizeof(edge)) 有 bug（应为 sizeof(Edge)），
 * 这里改为按多边形包围盒动态分配，并把 -1 边界与水平边处理补齐。
 */
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "algo.h"

#define EDGE_MAX 4096

typedef struct Edge {
    int   yUpper;
    double xIntersect, dxPerScan;
    int   used;
    struct Edge *next;
} Edge;

typedef struct {
    Edge *head;      /* 链头哨兵 */
    Edge *tail;
} EdgeList;

/* 按 xIntersect 升序插入（等价 p1 insertEdge） */
static void insert_edge(EdgeList *list, Edge *e)
{
    Edge *q = list->head, *p = q->next;
    while (p) {
        if (e->xIntersect < p->xIntersect) break;
        q = p; p = p->next;
    }
    e->next = q->next;
    q->next = e;
    if (!e->next) list->tail = e;
}

static int y_next(const Vtx *pts, int cnt, int k)
{
    int j = (k + 1 > cnt - 1) ? 0 : k + 1;
    int guard = 0;
    while (pts[k].y == pts[j].y) {
        j = (j + 1 > cnt - 1) ? 0 : j + 1;
        if (++guard > cnt) break;
    }
    return pts[j].y;
}

static void make_edge_rec(Vtx lower, Vtx upper, int yComp, Edge *e)
{
    e->dxPerScan  = (double)(upper.x - lower.x) / (double)(upper.y - lower.y);
    e->xIntersect = (double)lower.x;
    e->yUpper = (upper.y < yComp) ? upper.y - 1 : upper.y;
}

/* 构建边表 ET：每条非水平边只在它的下端 y 处入桶一次（p1 的 buildEdgeList 语义）。
 * 上下端 y 相同时按 yNext / yPrev 规则"缩短"一条边，避免顶点被重复计数（奇偶规则）。 */
static void build_edge_list(const Vtx *pts, int cnt, int ymin, int span, EdgeList *edges)
{
    int yprev = pts[cnt - 2].y;
    Vtx v1 = pts[cnt - 1];
    for (int i = 0; i < cnt; ++i) {
        Vtx v2 = pts[i];
        if (v1.y != v2.y) {
            Edge *e = (Edge *)calloc(1, sizeof(Edge));
            if (v1.y < v2.y) make_edge_rec(v1, v2, y_next(pts, cnt, i), e);
            else             make_edge_rec(v2, v1, yprev, e);
            int b = (v1.y < v2.y ? v1.y : v2.y) - ymin;
            if (b < 0) b = 0;
            if (b < span) insert_edge(&edges[b], e);
            else free(e);
        }
        yprev = v1.y;
        v1 = v2;
    }
}

/* 扫描线填充：按 y 递增维护活动边表 AET，成对取交点画水平跨度。
 * 对应 p1 的 scanFill()：buildActiveList -> fillScan -> updateActiveList -> resortActiveList */
int fill_scanline(const Vtx *poly, int n, int y0, int y1, PtList *out)
{
    if (n < 3) return 0;
    int ymin = poly[0].y, ymax = poly[0].y;
    for (int i = 1; i < n; ++i) {
        if (poly[i].y < ymin) ymin = poly[i].y;
        if (poly[i].y > ymax) ymax = poly[i].y;
    }
    if (y0 > ymin) ymin = y0;
    if (y1 < ymax) ymax = y1;
    if (ymax < ymin) return 0;

    int span = ymax - ymin + 1;
    EdgeList *et = (EdgeList *)calloc((size_t)span, sizeof(EdgeList));
    for (int i = 0; i < span; ++i)
        et[i].head = (Edge *)calloc(1, sizeof(Edge));
    build_edge_list(poly, n, ymin, span, et);

    EdgeList active;
    active.head = (Edge *)calloc(1, sizeof(Edge));

    int n0 = out->n;
    for (int scan = ymin; scan <= ymax; ++scan) {
        /* 1. 新边并入 AET，并清空该桶（节点所有权转移给 AET，避免二次释放） */
        Edge *p = et[scan - ymin].head->next;
        et[scan - ymin].head->next = NULL;
        while (p) {
            Edge *q = p->next;
            insert_edge(&active, p);
            p = q;
        }
        if (!active.head->next) continue;

        /* 2. 成对填跨度 */
        Edge *a = active.head->next;
        while (a && a->next) {
            Edge *b = a->next;
            int xa = (int)ceil(a->xIntersect);
            int xb = (int)floor(b->xIntersect);
            for (int x = xa; x <= xb; ++x) pl_push(out, x, scan);
            a = b->next;
        }

        /* 3. 更新 AET：过顶的删除（释放），其余累加斜率 */
        Edge *q = active.head, *cur = active.head->next;
        while (cur) {
            if (scan >= cur->yUpper) {
                q->next = cur->next;
                free(cur);
                cur = q->next;
            } else {
                cur->xIntersect += cur->dxPerScan;
                q = cur;
                cur = cur->next;
            }
        }

        /* 4. 按 x 重排（下次成对匹配才有意义） */
        Edge *list = active.head->next;
        active.head->next = NULL;
        while (list) {
            Edge *nx = list->next;
            insert_edge(&active, list);
            list = nx;
        }
    }

    /* 清理：AET 里剩余的边 + 空桶哨兵 */
    Edge *e = active.head->next;
    while (e) { Edge *nx = e->next; free(e); e = nx; }
    free(active.head);
    for (int i = 0; i < span; ++i) free(et[i].head);
    free(et);
    return out->n - n0;
}

/* 直接写像素缓冲（1 = 填充色），供 OpenGL 逐点上传，等价 p1 的全局 window[] */
int fill_scanline_flat(int *buf, int w, int h, const Vtx *poly, int n)
{
    if (n < 3) return 0;
    int cnt = 0;
    int ymin = poly[0].y, ymax = poly[0].y;
    for (int i = 1; i < n; ++i) {
        if (poly[i].y < ymin) ymin = poly[i].y;
        if (poly[i].y > ymax) ymax = poly[i].y;
    }
    if (ymin < 0) ymin = 0;
    if (ymax > h - 1) ymax = h - 1;

    double *xs = (double *)malloc(sizeof(double) * (size_t)n);
    if (!xs) return 0;
    for (int y = ymin; y <= ymax; ++y) {
        /* 求该扫描线与各边的交点（半开区间规则，避免顶点重复计数） */
        int nx = 0;
        for (int i = 0, j = n - 1; i < n; j = i++) {
            int y1 = poly[j].y, y2 = poly[i].y;
            if (y1 == y2) continue;
            if (y >= y1 && y < y2) {
                double t = (double)(y - y1) / (double)(y2 - y1);
                xs[nx++] = poly[j].x + t * (poly[i].x - poly[j].x);
            } else if (y >= y2 && y < y1) {
                double t = (double)(y - y2) / (double)(y1 - y2);
                xs[nx++] = poly[i].x + t * (poly[j].x - poly[i].x);
            }
        }
        /* 插入排序 */
        for (int i = 1; i < nx; ++i) {
            double k = xs[i]; int j = i - 1;
            while (j >= 0 && xs[j] > k) { xs[j + 1] = xs[j]; --j; }
            xs[j + 1] = k;
        }
        for (int i = 0; i + 1 < nx; i += 2) {
            int xa = (int)ceil(xs[i]);
            int xb = (int)floor(xs[i + 1]);
            if (xa < 0) xa = 0;
            if (xb > w - 1) xb = w - 1;
            for (int x = xa; x <= xb; ++x) {
                buf[y * w + x] = 1;
                cnt++;
            }
        }
    }
    free(xs);
    return cnt;
}

/* 种子填充（4 连通，边界泛洪），对应 p1 之外但同属上机常见算法 */
int fill_seed(int *buf, int w, int h, int x, int y, int border, int value)
{
    typedef struct { int x, y; } P;
    if (x < 0 || y < 0 || x >= w || y >= h) return 0;
    if (buf[y * w + x] == border || buf[y * w + x] == value) return 0;
    int cap = 1024, top = 0, cnt = 0;
    P *st = (P *)malloc(sizeof(P) * (size_t)cap);
    st[top].x = x; st[top].y = y; top++;
    while (top) {
        P c = st[--top];
        if (c.x < 0 || c.y < 0 || c.x >= w || c.y >= h) continue;
        if (buf[c.y * w + c.x] == border || buf[c.y * w + c.x] == value) continue;
        buf[c.y * w + c.x] = value;
        cnt++;
        if (top + 4 >= cap) { cap *= 2; st = (P *)realloc(st, sizeof(P) * (size_t)cap); }
        st[top].x = c.x + 1; st[top].y = c.y; top++;
        st[top].x = c.x - 1; st[top].y = c.y; top++;
        st[top].x = c.x; st[top].y = c.y + 1; top++;
        st[top].x = c.x; st[top].y = c.y - 1; top++;
    }
    free(st);
    return cnt;
}
