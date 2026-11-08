/* px.h —— 《图形交互技术-上机》Linux 版公共接口
 *
 * PDF 章节 -> 本框架等价物：
 *   屏幕区/窗口区/客户区           -> App(win) / app_draw_area()
 *   CDC 类 (CClientDC/CWindowDC)  -> Canvas（软件光栅 + 可选 GL 呈现）
 *   GDI 类 (CPen/CBrush/CFont)     -> Color/线型/FreeType 字体
 *   WM_LBUTTONDOWN/UP/MOUSEMOVE   -> X11 ButtonPress/Release/MotionNotify
 *   对话框 有模式/无模式            -> dlg_run() / dlg_show()+dlg_hide()
 *   菜单/工具栏/状态栏              -> ui_menubar_* / ui_statusbar
 *   PIXELFORMATDESCRIPTOR + HGLRC  -> glXChooseVisual + glXCreateContext
 *   SwapBuffers(pDC->m_hDC)        -> app_present()
 */
#ifndef PX_H
#define PX_H

#include <stdint.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <GL/gl.h>

/* ---------------- 颜色 0xAARRGGBB ---------------- */
typedef uint32_t Color;
#define PX_A(c)          (((c) >> 24) & 0xFFu)
#define PX_R(c)          (((c) >> 16) & 0xFFu)
#define PX_G(c)          (((c) >>  8) & 0xFFu)
#define PX_B(c)          (((c) >>  0) & 0xFFu)
#define PX_RGB(r,g,b)    ((Color)(0xFF000000u | (((Color)(r)) << 16) | (((Color)(g)) << 8) | ((Color)(b))))
#define PX_ARGB(a,r,g,b) ((Color)((((Color)(a)) << 24) | (((Color)(r)) << 16) | (((Color)(g)) << 8) | ((Color)(b)))

#define CLR_BLACK   PX_RGB(0,0,0)
#define CLR_WHITE   PX_RGB(255,255,255)
#define CLR_RED     PX_RGB(214,45,45)
#define CLR_GREEN   PX_RGB(0,150,70)
#define CLR_BLUE    PX_RGB(40,80,200)
#define CLR_YELLOW  PX_RGB(214,190,20)
#define CLR_ORANGE  PX_RGB(232,140,40)
#define CLR_GRAY    PX_RGB(130,130,130)
#define CLR_LGRAY   PX_RGB(232,232,232)
#define CLR_MGRAY   PX_RGB(196,196,196)
#define CLR_DGRAY   PX_RGB(70,70,70)
#define CLR_NAVY    PX_RGB(36,56,120)
#define CLR_PANEL   PX_RGB(246,246,248)

/* ---------------- 几何 ---------------- */
typedef struct { int x, y; } Pt;
typedef struct { int x, y, w, h; } Rect;

static inline int rect_contains(Rect r, int x, int y)
{ return x >= r.x && y >= r.y && x < r.x + r.w && y < r.y + r.h; }

/* ---------------- 光栅画布（CDC 等价） ---------------- */
enum { CV_COPY = 0, CV_XOR = 1 };

typedef struct {
    int    w, h;
    Color *px;    /* 行主序 0xAARRGGBB */
    int    rop;   /* CV_COPY 对应 R2_COPYPEN；CV_XOR 对应 R2_NOT */
} Canvas;

void cv_init(Canvas *c, int w, int h);
void cv_resize(Canvas *c, int w, int h);
void cv_free(Canvas *c);
void cv_set_rop(Canvas *c, int rop);

void cv_clear(Canvas *c, Color col);
void cv_pixel(Canvas *c, int x, int y, Color col);
void cv_line(Canvas *c, int x0, int y0, int x1, int y1, Color col);
void cv_hline(Canvas *c, int x, int y, int len, Color col);
void cv_vline(Canvas *c, int x, int y, int len, Color col);
void cv_rect(Canvas *c, Rect r, Color col);
void cv_fill_rect(Canvas *c, Rect r, Color col);
void cv_circle(Canvas *c, int cx, int cy, int r, Color col);
void cv_fill_circle(Canvas *c, int cx, int cy, int r, Color col);
void cv_dashes(Canvas *c, int x0, int y0, int x1, int y1, int on, int off, Color col);
void cv_blend_pixel(Canvas *c, int x, int y, Color col, int alpha);
void cv_marker(Canvas *c, int x, int y, int r, Color col);   /* 端点标记：r<3 时画实心点 */

/* ---------------- 文本（FreeType，支持中文） ---------------- */
int  tx_init(const char *fontpath, int face_index);
int  tx_height(int size);
int  tx_width(int size, const char *utf8);
void tx_draw(Canvas *c, int x, int y, const char *utf8, Color col, int size);
void tx_draw_center(Canvas *c, Rect box, const char *utf8, Color col, int size);

/* ---------------- 窗口 / 消息循环（CWinApp + CFrameWnd 等价） ---------------- */
typedef struct App App;
typedef void (*AppPaint)(App *a);
typedef void (*AppEvent)(App *a, XEvent *e);

struct App {
    Display    *dpy;
    int         screen;
    Window      win;
    GLXContext  ctx;
    Colormap    cmap;
    int         w, h;
    Canvas      fb;
    unsigned    tex;
    int         tex_w, tex_h;
    int         running;
    int         animating;      /* 1 = 每 16ms 触发一次动事件 */
    int         dirty;          /* 事件处理中置 1，主循环据此重绘 */
    int         mx, my;         /* 客户区坐标 */
    unsigned    buttons;
    int         shift, ctrl, key;
    void       *user;
    AppPaint    paint;          /* 供嵌套对话框循环重绘整帧 */
    AppEvent    ev;
};

extern App *g_app;

App  *app_create(const char *title, int w, int h, const char *cursor_name);
void  app_run(App *a, AppPaint paint, AppEvent ev);
void  app_present(App *a);
void  app_destroy(App *a);
void  app_set_title(App *a, const char *title);
void  app_invalidate(App *a);
void  app_repaint(App *a);
Rect  app_draw_area(App *a);

/* ---------------- 菜单 / 状态栏 ---------------- */
#define MENUBAR_H   30
#define STATUSBAR_H 28

typedef struct { const char *text; int id; } MenuItem;
typedef struct { const char *title; const MenuItem *items; int n; } Menu;

typedef struct {
    const Menu *menus;
    int         nmenus;
    int         open;       /* -1 关闭 */
    int         hot;        /* 当前高亮项 */
    int         hot_menu;
} MenuBar;

void ui_menubar_init(MenuBar *mb, const Menu *menus, int n);
void ui_menubar_draw(App *a, MenuBar *mb);
int  ui_menubar_event(App *a, MenuBar *mb, XEvent *e);   /* 返回命令 id，0=无 */
int  ui_menubar_active(const MenuBar *mb);
int  ui_menubar_consumes(const MenuBar *mb, int mx, int my);

void ui_statusbar(App *a, const char *left, const char *right);
void ui_title(App *a, const char *title, const char *sub);

/* ---------------- 对话框（CDialog 等价） ---------------- */
#define DLG_MAX_FIELD 8
#define DLG_MAX_BTN   5

typedef struct {
    const char *title;
    const char *labels[DLG_MAX_FIELD];
    int         values[DLG_MAX_FIELD];
    char        buf[DLG_MAX_FIELD][24];
    int         nfields;

    const char *btns[DLG_MAX_BTN];
    int         btn_code[DLG_MAX_BTN];
    int         nbtns;

    Rect        box;
    Rect        fld_box[DLG_MAX_FIELD];
    Rect        btn_box[DLG_MAX_BTN];
    int         active;       /* 当前编辑框下标，-1 表示无 */
    int         hover_btn, press_btn;
    int         visible;
    int         result;       /* 1=OK 2=Cancel 10+=自定义 */
    const char *hint;
    const char *note;
} Dialog;

void dlg_build(Dialog *d, const char *title);
void dlg_add_field(Dialog *d, const char *label, int value);
void dlg_add_button(Dialog *d, const char *text, int code);
void dlg_add_okcancel(Dialog *d);
int  dlg_run(App *a, Dialog *d, MenuBar *mb, const char *status);
void dlg_show(App *a, Dialog *d);
void dlg_hide(Dialog *d);
int  dlg_event(App *a, Dialog *d, XEvent *e, MenuBar *mb, const char *status);
void dlg_draw(App *a, Dialog *d);
int  dlg_get(const Dialog *d, int i);

int  msgbox(App *a, const char *title, const char *text, MenuBar *mb);

#endif
