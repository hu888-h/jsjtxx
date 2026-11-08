/* main.c —— 程序入口（对应 MFC 的 CWinApp::InitInstance + 消息泵）
 *
 * 结构对照 p1：
 *   p1/MouseLine                -> VIEW_MOUSELINE
 *   p1/DrawLineView/SingleDraw  -> VIEW_SINGLEDRAW
 *   p1/DrawLineView/SingleDraw2 -> VIEW_DLGLINE（含坐标对话框）
 *   zip/算法2 DDA               -> VIEW_DDA
 *   p1/DrawLine                 -> VIEW_BRESENHAM（Bresenham + 中点圆）
 *   p1/Fill                     -> VIEW_SCANFILL（扫描线填充 + Trigle 对话框）
 *   zip/算法16/17/18            -> VIEW_COHEN / VIEW_MIDCLIP / VIEW_LIANG
 *   zip/算法15                  -> VIEW_TRANSFORM
 *   p1/DLGExam                  -> VIEW_DIALOG（有模式/无模式）
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "px.h"
#include "algo.h"
#include "view.h"

/* ---------------- 命令 ID ---------------- */
#define IDM_VIEW(n)      (1000 + (n))
#define IDM_DLG_MODEL     2001
#define IDM_DLG_NOMODEL   2002
#define IDM_DLG_COORD     2003
#define IDM_DLG_TRI       2004
#define IDM_TF_RESET      3001
#define IDM_TF_TRANSLATE  3002
#define IDM_TF_ROTATE     3003
#define IDM_TF_SCALE      3004
#define IDM_TF_REFLECT_X  3005
#define IDM_TF_REFLECT_Y  3006
#define IDM_TF_REFLECT_O  3007
#define IDM_TF_SHEAR      3008
#define IDM_TF_STAR       3009
#define IDM_HELP_ABOUT    4001
#define IDM_HELP_KEYS     4002

static Scene   g_scene;
static MenuBar g_mb;
static Dialog  g_dlg_nomodel;
static int     g_has_nomodel = 0;
static const char *g_status_left = "";
static char    g_status_right[128];

/* ---------------- 菜单定义 ---------------- */
static MenuItem items_view[] = {
    {"1  MouseLine 按下-抬起画线",      IDM_VIEW(VIEW_MOUSELINE)},
    {"2  SingleDraw 两次点击+橡皮筋",   IDM_VIEW(VIEW_SINGLEDRAW)},
    {"3  坐标对话框定线（DoModal）",    IDM_VIEW(VIEW_DLGLINE)},
    {"4  DDA 直线（算法2）",            IDM_VIEW(VIEW_DDA)},
    {"5  Bresenham 直线 / 中点圆",      IDM_VIEW(VIEW_BRESENHAM)},
    {"6  中点圆 / 中点椭圆",            IDM_VIEW(VIEW_CIRCLE)},
    {"7  扫描线填充（p1/Fill）",        IDM_VIEW(VIEW_SCANFILL)},
    {"8  种子填充（4 连通）",           IDM_VIEW(VIEW_SEEDFILL)},
    {"9  Cohen-Sutherland 裁剪",        IDM_VIEW(VIEW_COHEN)},
    {"10 中点分割裁剪",                 IDM_VIEW(VIEW_MIDCLIP)},
    {"11 Liang-Barsky 裁剪",            IDM_VIEW(VIEW_LIANG)},
    {"12 二维几何变换 + 五角星",        IDM_VIEW(VIEW_TRANSFORM)},
    {"13 有模式 / 无模式对话框",        IDM_VIEW(VIEW_DIALOG)},
};
static MenuItem items_dlg[] = {
    {"有模式对话框（DoModal）",         IDM_DLG_MODEL},
    {"无模式对话框（Create+Show）",     IDM_DLG_NOMODEL},
    {"{-}",                             -1},
    {"坐标输入（定线 / 三角形）",       IDM_DLG_COORD},
    {"三角形顶点（p1/Fill）",           IDM_DLG_TRI},
};
static MenuItem items_tf[] = {
    {"复位（单位矩阵）",                IDM_TF_RESET},
    {"平移 tx=60, ty=40",               IDM_TF_TRANSLATE},
    {"旋转 30°（绕原点）",              IDM_TF_ROTATE},
    {"缩放 1.3（绕原点）",              IDM_TF_SCALE},
    {"关于 X 轴反射",                   IDM_TF_REFLECT_X},
    {"关于 Y 轴反射",                   IDM_TF_REFLECT_Y},
    {"关于原点反射",                    IDM_TF_REFLECT_O},
    {"错切 b=0.4, c=0",                 IDM_TF_SHEAR},
    {"{-}",                             -1},
    {"恢复五角星初值",                  IDM_TF_STAR},
};
static MenuItem items_help[] = {
    {"关于本程序",                      IDM_HELP_ABOUT},
    {"快捷键说明",                      IDM_HELP_KEYS},
};

static Menu menus[] = {
    {"视图",  items_view, (int)(sizeof(items_view) / sizeof(items_view[0]))},
    {"对话框", items_dlg, (int)(sizeof(items_dlg) / sizeof(items_dlg[0]))},
    {"变换",  items_tf,   (int)(sizeof(items_tf) / sizeof(items_tf[0]))},
    {"帮助",  items_help, (int)(sizeof(items_help) / sizeof(items_help[0]))},
};

/* ---------------- 绘制 ---------------- */
static void paint(App *a)
{
    Rect ar = app_draw_area(a);
    Canvas *c = &a->fb;

    /* 绘图区背景 */
    cv_fill_rect(c, ar, CLR_WHITE);
    cv_rect(c, (Rect){ar.x, ar.y, ar.w - 1, ar.h - 1}, PX_RGB(214,216,220));

    scene_draw(a, &g_scene, ar);

    char left[256];
    snprintf(left, sizeof(left), "%s  —  %s", kViewName[g_scene.id], kViewHint[g_scene.id]);
    g_status_left = left;
    int mx = a->mx - ar.x - ar.w / 2;
    int my = -(a->my - ar.y - ar.h / 2);
    snprintf(g_status_right, sizeof(g_status_right),
             "鼠标 (%d, %d)  ·  绘图区 %dx%d", mx, my, ar.w, ar.h);
    ui_statusbar(a, g_status_left, g_status_right);
    ui_menubar_draw(a, &g_mb);

    if (g_dlg_nomodel.visible) dlg_draw(a, &g_dlg_nomodel);
}

/* ---------------- 对话框构造 ---------------- */
static void make_exam_dialog(Dialog *d)
{
    dlg_build(d, "对话框示例（CDlgExam）");
    dlg_add_field(d, "请输入一个整数 m_exam：", g_scene.params[0]);
    dlg_add_okcancel(d);
    d->hint = "确定后数值回写到视图；Esc / 取消 放弃";
}

static void make_coord_dialog(Dialog *d)
{
    if (g_scene.id == VIEW_DLGLINE) {
        dlg_build(d, "输入直线端点（CDiaLineWith2P）");
        dlg_add_field(d, "起点 X：", (int)lround(g_scene.a.x));
        dlg_add_field(d, "起点 Y：", (int)lround(g_scene.a.y));
        dlg_add_field(d, "终点 X：", (int)lround(g_scene.b.x));
        dlg_add_field(d, "终点 Y：", (int)lround(g_scene.b.y));
        dlg_add_button(d, "取起点", 10);
        dlg_add_button(d, "取终点", 11);
        dlg_add_okcancel(d);
        d->hint = "「取起点/取终点」关闭对话框并回到视图点选";
    } else {
        dlg_build(d, "输入三角形顶点（CTrigle）");
        dlg_add_field(d, "X1：", (int)lround(g_scene.a.x));
        dlg_add_field(d, "Y1：", (int)lround(g_scene.a.y));
        dlg_add_field(d, "X2：", (int)lround(g_scene.b.x));
        dlg_add_field(d, "Y2：", (int)lround(g_scene.b.y));
        dlg_add_field(d, "X3：", (int)lround(g_scene.dparams[0]));
        dlg_add_field(d, "Y3：", (int)lround(g_scene.dparams[1]));
        dlg_add_okcancel(d);
        d->hint = "扫描线种子填充的边界顶点";
    }
}


/* 每个视图的初值（对应 p1 各工程构造函数里的成员初始化） */
static void view_defaults(Scene *s, ViewId id)
{
    scene_init(s);
    s->id = id;
    switch (id) {
    case VIEW_MOUSELINE:
        s->a.x = -150; s->a.y = -80; s->b.x = 150; s->b.y = 80; s->flag = 0; break;
    case VIEW_SINGLEDRAW:
        s->a.x = -120; s->a.y = 0; s->b.x = 120; s->b.y = 0; s->nseg = 0; break;
    case VIEW_DLGLINE:
        s->a.x = -180; s->a.y = -90; s->b.x = 180; s->b.y = 90; s->flag = 1; break;
    case VIEW_DDA:
        s->params[0] = 150; s->params[1] = 0; break;
    case VIEW_BRESENHAM:
        s->a.x = -200; s->a.y = -120; s->b.x = 200; s->b.y = 130; s->params[1] = 0; break;
    case VIEW_CIRCLE:
        s->a.x = -120; s->a.y = 0; s->b.x = 130; s->b.y = 20; s->radius = 90; break;
    case VIEW_SCANFILL:
        s->a.x = -140; s->a.y = -80; s->b.x = 150; s->b.y = -60;
        s->dparams[0] = 0; s->dparams[1] = 120; break;
    case VIEW_SEEDFILL:
        s->a.x = 0; s->a.y = 0; s->radius = 110; s->flag = 0; break;
    case VIEW_COHEN:
    case VIEW_MIDCLIP:
    case VIEW_LIANG:
        s->a.x = -330; s->a.y = -170; s->b.x = 330; s->b.y = 175; break;
    case VIEW_TRANSFORM:
        mat_identity(&s->mat); break;
    case VIEW_DIALOG:
        s->params[0] = 42; break;
    default: break;
    }
}

/* ---------------- 命令处理 ---------------- */
static void apply_command(App *a, int id)
{
    if (id >= 1000 && id < 2000) {
        view_defaults(&g_scene, (ViewId)(id - 1000));
        return;
    }

    switch (id) {
    case IDM_DLG_MODEL: {
        Dialog d;
        make_exam_dialog(&d);
        int r = dlg_run(a, &d, &g_mb, NULL);
        if (r == 1) g_scene.params[0] = dlg_get(&d, 0);
        break; }
    case IDM_DLG_NOMODEL:
        make_exam_dialog(&g_dlg_nomodel);
        dlg_show(a, &g_dlg_nomodel);
        g_has_nomodel = 1;
        break;
    case IDM_DLG_COORD: {
        Dialog d;
        make_coord_dialog(&d);
        int r = dlg_run(a, &d, &g_mb, NULL);
        if (r == 10) { g_scene.stage = 1; dlg_hide(&d); }
        else if (r == 11) { g_scene.stage = 2; dlg_hide(&d); }
        else if (r == 1) {
            if (g_scene.id == VIEW_DLGLINE) {
                g_scene.a.x = dlg_get(&d, 0); g_scene.a.y = dlg_get(&d, 1);
                g_scene.b.x = dlg_get(&d, 2); g_scene.b.y = dlg_get(&d, 3);
                g_scene.flag = 1;
            } else if (g_scene.id == VIEW_SCANFILL) {
                g_scene.a.x = dlg_get(&d, 0); g_scene.a.y = dlg_get(&d, 1);
                g_scene.b.x = dlg_get(&d, 2); g_scene.b.y = dlg_get(&d, 3);
                g_scene.dparams[0] = dlg_get(&d, 4);
                g_scene.dparams[1] = dlg_get(&d, 5);
            }
        }
        break; }
    case IDM_DLG_TRI: {
        if (g_scene.id != VIEW_SCANFILL) g_scene.id = VIEW_SCANFILL;
        Dialog d; make_coord_dialog(&d);
        if (dlg_run(a, &d, &g_mb, NULL) == 1) {
            g_scene.a.x = dlg_get(&d, 0); g_scene.a.y = dlg_get(&d, 1);
            g_scene.b.x = dlg_get(&d, 2); g_scene.b.y = dlg_get(&d, 3);
            g_scene.dparams[0] = dlg_get(&d, 4);
            g_scene.dparams[1] = dlg_get(&d, 5);
        }
        break; }
    case IDM_TF_RESET:    mat_identity(&g_scene.mat); snprintf(g_scene.info, sizeof(g_scene.info), "已复位为单位矩阵"); break;
    case IDM_TF_TRANSLATE: mat_translate(&g_scene.mat, 60.0, 40.0); snprintf(g_scene.info, sizeof(g_scene.info), "追加：平移 tx=60 ty=40"); break;
    case IDM_TF_ROTATE:   mat_rotate(&g_scene.mat, 30.0); snprintf(g_scene.info, sizeof(g_scene.info), "追加：绕原点旋转 30°"); break;
    case IDM_TF_SCALE:    mat_scale(&g_scene.mat, 1.3, 1.3); snprintf(g_scene.info, sizeof(g_scene.info), "追加：缩放 1.3x"); break;
    case IDM_TF_REFLECT_X: mat_reflect_x(&g_scene.mat); snprintf(g_scene.info, sizeof(g_scene.info), "追加：关于 X 轴反射"); break;
    case IDM_TF_REFLECT_Y: mat_reflect_y(&g_scene.mat); snprintf(g_scene.info, sizeof(g_scene.info), "追加：关于 Y 轴反射"); break;
    case IDM_TF_REFLECT_O: mat_reflect_origin(&g_scene.mat); snprintf(g_scene.info, sizeof(g_scene.info), "追加：关于原点反射"); break;
    case IDM_TF_SHEAR:    mat_shear(&g_scene.mat, 0.4, 0.0); snprintf(g_scene.info, sizeof(g_scene.info), "追加：错切 b=0.4"); break;
    case IDM_TF_STAR:     mat_identity(&g_scene.mat); g_scene.info[0] = 0; break;
    case IDM_HELP_ABOUT: {
        Dialog d;
        dlg_build(&d, "关于");
        d.note = "图形交互技术-上机 Linux 版 · X11 + GLX + FreeType";
        dlg_add_button(&d, "确定", 1);
        dlg_run(a, &d, &g_mb, NULL);
        break; }
    case IDM_HELP_KEYS: {
        Dialog d;
        dlg_build(&d, "快捷键");
        d.note = "+/- 改半径  1/2 改顶点数  r 复位  Esc 退出";
        dlg_add_button(&d, "确定", 1);
        dlg_run(a, &d, &g_mb, NULL);
        break; }
    default: break;
    }
}

/* ---------------- 事件分发 ---------------- */
static void on_event(App *a, XEvent *e)
{
    if (e->type == 0) return;

    /* 1. 无模式对话框：仅在其面板矩形内拦截事件，其余照常传递给主窗口 */
    if (g_dlg_nomodel.visible) {
        Rect nb = g_dlg_nomodel.box;
        int inside = rect_contains(nb, a->mx, a->my);
        int ev_is_btn = (e->type == ButtonPress || e->type == ButtonRelease);
        if (inside || e->type == KeyPress) {
            int r = dlg_event(a, &g_dlg_nomodel, e, &g_mb, NULL);
            if (r == 1) g_scene.params[0] = dlg_get(&g_dlg_nomodel, 0);
            else if (r == 2) dlg_hide(&g_dlg_nomodel);
            if (inside || ev_is_btn) return;
            if (r) return;
        }
        /* 点面板外部：无模式对话框保持显示，但事件继续交给主窗口 */
    }

    /* 2. 菜单：先分发，再判断是否被菜单区域消费 */
    int id = ui_menubar_event(a, &g_mb, e);
    if (id) { apply_command(a, id); return; }
    if (e->type == ButtonPress || e->type == ButtonRelease || e->type == MotionNotify) {
        if (ui_menubar_consumes(&g_mb, a->mx, a->my)) return;
        if (ui_menubar_active(&g_mb)) return;
    }

    /* 3. 绘图区 */
    Rect ar = app_draw_area(a);
    if (!rect_contains(ar, a->mx, a->my)) {
        if (a->my >= a->h - STATUSBAR_H) return;
        return;
    }

    switch (e->type) {
    case ButtonPress:
        if (e->xbutton.button == Button1) {
            if (g_scene.stage != 0 && g_scene.id == VIEW_DLGLINE) {
                scene_lbutton_down(a, &g_scene, a->mx, a->my);
                break;
            }
            scene_lbutton_down(a, &g_scene, a->mx, a->my);
        } else if (e->xbutton.button == Button3) {
            scene_reset(&g_scene);
        }
        break;
    case ButtonRelease:
        if (e->xbutton.button == Button1)
            scene_lbutton_up(a, &g_scene, a->mx, a->my);
        break;
    case MotionNotify:
        scene_motion(a, &g_scene, a->mx, a->my);
        break;
    case KeyPress:
        scene_key(a, &g_scene, a->key);
        break;
    default:
        break;
    }
}

/* ---------------- main ---------------- */
int main(int argc, char **argv)
{
    int w = 1100, h = 760;
    int start_view = 0;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-w") && i + 1 < argc) w = atoi(argv[++i]);
        else if (!strcmp(argv[i], "-h") && i + 1 < argc) h = atoi(argv[++i]);
        else if (!strcmp(argv[i], "-v") && i + 1 < argc) {
            int v = atoi(argv[++i]);
            if (v >= 0 && v < VIEW_COUNT) start_view = v;
        }
    }

    tx_init("/usr/share/fonts/noto-cjk/NotoSansCJK-Regular.ttc", 0);

    App *a = app_create("图形交互技术-上机 · Linux 版", w, h, "cross");
    if (!a) return 1;

    view_defaults(&g_scene, (ViewId)start_view);
    ui_menubar_init(&g_mb, menus, (int)(sizeof(menus) / sizeof(menus[0])));

    app_run(a, paint, on_event);
    app_destroy(a);
    return 0;
}
