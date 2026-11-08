/* ui.c —— 菜单栏 / 状态栏 / 对话框（PDF“菜单技术”“对话框技术”章节的 Linux 实现）
 * 有模式对话框 = 嵌套事件循环，关闭前不返回（对应 CDialog::DoModal）
 * 无模式对话框 = dlg_show() 后由主循环照常分发事件（对应 Create + ShowWindow）
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "px.h"



void ui_title(App *a, const char *title, const char *sub)
{
    char buf[256];
    if (sub) snprintf(buf, sizeof(buf), "%s — %s", title, sub);
    else snprintf(buf, sizeof(buf), "%s", title);
    app_set_title(a, buf);
}

/* ========================= 菜单栏 ========================= */

void ui_menubar_init(MenuBar *mb, const Menu *menus, int n)
{
    mb->menus = menus;
    mb->nmenus = n;
    mb->open = -1;
    mb->hot = -1;
    mb->hot_menu = -1;
}

int ui_menubar_active(const MenuBar *mb) { return mb->open >= 0; }

static Rect menu_title_rect(const MenuBar *mb, int i)
{
    Rect r; r.y = 0; r.h = MENUBAR_H; r.x = 8; r.w = 0;
    for (int k = 0; k < i; ++k)
        r.x += tx_width(15, mb->menus[k].title) + 26;
    r.w = tx_width(15, mb->menus[i].title) + 26;
    return r;
}

static int item_w(const MenuBar *mb)
{
    int w = 0;
    if (mb->open < 0) return 0;
    const Menu *m = &mb->menus[mb->open];
    for (int i = 0; i < m->n; ++i) {
        int t = tx_width(14, m->items[i].text) + 56;
        if (t > w) w = t;
    }
    return w < 150 ? 150 : w;
}

static Rect menu_panel_rect(const MenuBar *mb)
{
    Rect r = menu_title_rect(mb, mb->open);
    r.y = MENUBAR_H;
    r.h = mb->menus[mb->open].n * 26 + 8;
    r.w = item_w(mb);
    return r;
}

static Rect item_rect(const MenuBar *mb, int i)
{
    Rect p = menu_panel_rect(mb);
    Rect r; r.x = p.x + 4; r.w = p.w - 8; r.h = 24;
    r.y = p.y + 4 + i * 26;
    return r;
}

void ui_menubar_draw(App *a, MenuBar *mb)
{
    Canvas *c = &a->fb;
    Rect bar = {0, 0, a->w, MENUBAR_H};
    cv_fill_rect(c, bar, PX_RGB(250,250,252));
    cv_hline(c, 0, MENUBAR_H - 1, a->w, CLR_MGRAY);

    for (int i = 0; i < mb->nmenus; ++i) {
        Rect r = menu_title_rect(mb, i);
        if (mb->open == i) cv_fill_rect(c, r, PX_RGB(214,226,246));
        tx_draw_center(c, r, mb->menus[i].title, CLR_BLACK, 15);
    }

    /* 右侧标题 */
    const char *tag = "图像交互 · Linux";
    int tw = tx_width(13, tag);
    tx_draw(c, a->w - tw - 14, 8, tag, CLR_GRAY, 13);

    if (mb->open < 0) return;
    const Menu *m = &mb->menus[mb->open];
    Rect p = menu_panel_rect(mb);
    cv_fill_rect(c, p, PX_RGB(252,252,254));
    cv_rect(c, p, CLR_MGRAY);
    for (int i = 0; i < m->n; ++i) {
        Rect r = item_rect(mb, i);
        if (i == mb->hot && m->items[i].id != -1) cv_fill_rect(c, r, PX_RGB(206,222,246));
        if (m->items[i].id == -1) {   /* id == -1 表示分隔线 */
            cv_hline(c, r.x + 6, r.y + 11, r.w - 12, CLR_MGRAY);
            continue;
        }
        tx_draw(c, r.x + 12, r.y + 4, m->items[i].text, CLR_BLACK, 14);
        {
            char sc[32];
            snprintf(sc, sizeof(sc), "%d", i + 1);
            int w = tx_width(12, sc);
            tx_draw(c, r.x + r.w - w - 10, r.y + 5, sc, CLR_GRAY, 12);
        }
    }
}

int ui_menubar_consumes(const MenuBar *mb, int mx, int my)
{
    if (my < MENUBAR_H) return 1;
    if (mb->open >= 0) {
        Rect p = menu_panel_rect(mb);
        if (rect_contains(p, mx, my)) return 1;
    }
    return 0;
}

int ui_menubar_event(App *a, MenuBar *mb, XEvent *e)
{
    int mx = a->mx, my = a->my;

    /* 按下：只负责开/关菜单与高亮，避免在 ButtonPress 与 ButtonRelease 间重复触发命令 */
    if (e->type == ButtonPress && e->xbutton.button == Button1) {
        if (my < MENUBAR_H) {
            for (int i = 0; i < mb->nmenus; ++i)
                if (rect_contains(menu_title_rect(mb, i), mx, my)) {
                    mb->open = (mb->open == i) ? -1 : i;
                    mb->hot = -1;
                    return 0;
                }
            mb->open = -1;
            return 0;
        }
        if (mb->open >= 0) {
            const Menu *m = &mb->menus[mb->open];
            for (int i = 0; i < m->n; ++i)
                if (rect_contains(item_rect(mb, i), mx, my)) {
                    mb->hot = i;
                    return 0;
                }
            mb->open = -1;
        }
        return 0;
    }

    /* 抬起：真正发布命令 */
    if (e->type == ButtonRelease && e->xbutton.button == Button1) {
        if (mb->open < 0) return 0;
        const Menu *m = &mb->menus[mb->open];
        Rect p = menu_panel_rect(mb);
        if (rect_contains(p, mx, my)) {
            for (int i = 0; i < m->n; ++i)
                if (rect_contains(item_rect(mb, i), mx, my)) {
                    int id = m->items[i].id;
                    mb->open = -1; mb->hot = -1;
                    return (id == -1) ? 0 : id;
                }
            return 0;                 /* 面板内但没点中项：保持打开 */
        }
        /* 面板外抬起：关闭并丢弃 */
        if (my >= MENUBAR_H) { mb->open = -1; mb->hot = -1; }
        return 0;
    }

    if (e->type == MotionNotify) {
        if (mb->open < 0) return 0;
        const Menu *m = &mb->menus[mb->open];
        mb->hot = -1;
        for (int i = 0; i < m->n; ++i)
            if (rect_contains(item_rect(mb, i), mx, my)) mb->hot = i;
        if (my < MENUBAR_H)
            for (int i = 0; i < mb->nmenus; ++i)
                if (rect_contains(menu_title_rect(mb, i), mx, my)) { mb->open = i; mb->hot = -1; }
    }
    return 0;
}

/* ========================= 状态栏 ========================= */

void ui_statusbar(App *a, const char *left, const char *right)
{
    Canvas *c = &a->fb;
    Rect s = {0, a->h - STATUSBAR_H, a->w, STATUSBAR_H};
    cv_fill_rect(c, s, PX_RGB(242,242,246));
    cv_hline(c, 0, s.y, a->w, CLR_MGRAY);
    if (left)  tx_draw(c, 12, s.y + 5, left, CLR_DGRAY, 13);
    if (right) {
        int w = tx_width(13, right);
        tx_draw(c, a->w - w - 12, s.y + 5, right, CLR_DGRAY, 13);
    }
}

/* ========================= 对话框 ========================= */

void dlg_build(Dialog *d, const char *title)
{
    memset(d, 0, sizeof(*d));
    d->title = title;
    d->active = 0;
    d->hover_btn = -1;
    d->press_btn = -1;
    d->result = 0;
}

void dlg_add_field(Dialog *d, const char *label, int value)
{
    if (d->nfields >= DLG_MAX_FIELD) return;
    d->labels[d->nfields] = label;
    d->values[d->nfields] = value;
    snprintf(d->buf[d->nfields], sizeof(d->buf[0]), "%d", value);
    d->nfields++;
}

void dlg_add_button(Dialog *d, const char *text, int code)
{
    if (d->nbtns >= DLG_MAX_BTN) return;
    d->btns[d->nbtns] = text;
    d->btn_code[d->nbtns] = code;
    d->nbtns++;
}

void dlg_add_okcancel(Dialog *d)
{
    dlg_add_button(d, "确定", 1);
    dlg_add_button(d, "取消", 2);
}

int dlg_get(const Dialog *d, int i)
{
    if (i < 0 || i >= d->nfields) return 0;
    return atoi(d->buf[i]);
}

static void dlg_layout(Dialog *d, App *a)
{
    int field_h = 34, pad = 18;
    int label_w = 0;
    for (int i = 0; i < d->nfields; ++i) {
        int w = tx_width(14, d->labels[i]);
        if (w > label_w) label_w = w;
    }
    int boxw = label_w + 210;
    if (boxw < 300) boxw = 300;
    int boxh = 74 + d->nfields * field_h + (d->hint || d->note ? 26 : 0) + 46;
    d->box.w = boxw;
    d->box.h = boxh;
    d->box.x = (a->w - boxw) / 2;
    d->box.y = MENUBAR_H + (a->h - MENUBAR_H - STATUSBAR_H - boxh) / 2;
    if (d->box.y < MENUBAR_H + 10) d->box.y = MENUBAR_H + 10;

    for (int i = 0; i < d->nfields; ++i) {
        Rect r;
        r.x = d->box.x + pad + label_w + 12;
        r.y = d->box.y + 62 + i * field_h;
        r.w = boxw - (label_w + 12) - pad * 2 - 10;
        r.h = 26;
        d->fld_box[i] = r;
    }

    int bw = 86, gap = 12;
    int total = d->nbtns * bw + (d->nbtns - 1) * gap;
    int bx = d->box.x + (boxw - total) / 2;
    int by = d->box.y + boxh - 46;
    for (int i = 0; i < d->nbtns; ++i) {
        Rect r; r.x = bx + i * (bw + gap); r.y = by; r.w = bw; r.h = 30;
        d->btn_box[i] = r;
    }
}

void dlg_draw(App *a, Dialog *d)
{
    if (!d->visible) return;
    if (d->box.w == 0) dlg_layout(d, a);
    Canvas *c = &a->fb;

    /* 遮罩（有模式对话框置灰底层，对应 CDialog 的模态语义） */
    for (int y = MENUBAR_H; y < a->h - STATUSBAR_H; ++y)
        for (int x = 0; x < a->w; ++x)
            cv_blend_pixel(c, x, y, PX_RGB(60,60,60), 90);

    Rect b = d->box;
    /* 阴影 + 面板 */
    Rect sh = {b.x + 4, b.y + 5, b.w, b.h};
    cv_fill_rect(c, sh, PX_RGB(150,150,155));
    cv_fill_rect(c, b, CLR_PANEL);
    cv_rect(c, b, PX_RGB(120,130,150));

    Rect cap = {b.x, b.y, b.w, 34};
    cv_fill_rect(c, cap, PX_RGB(70,96,150));
    tx_draw(c, b.x + 14, b.y + 6, d->title, CLR_WHITE, 15);

    for (int i = 0; i < d->nfields; ++i) {
        int ly = d->fld_box[i].y + 5;
        tx_draw(c, b.x + 18, ly, d->labels[i], CLR_DGRAY, 14);
        Rect f = d->fld_box[i];
        cv_fill_rect(c, f, CLR_WHITE);
        cv_rect(c, f, (i == d->active) ? PX_RGB(60,110,200) : PX_RGB(160,160,170));
        if (i == d->active) cv_rect(c, (Rect){f.x - 1, f.y - 1, f.w + 2, f.h + 2}, PX_RGB(120,170,240));
        tx_draw(c, f.x + 7, f.y + 5, d->buf[i], CLR_BLACK, 14);
        if (i == d->active) {
            int cw = tx_width(14, d->buf[i]);
            cv_vline(c, f.x + 8 + cw, f.y + 4, f.h - 8, PX_RGB(40,40,40));
        }
    }

    if (d->hint) tx_draw(c, b.x + 18, b.y + b.h - 74, d->hint, CLR_GRAY, 12);
    if (d->note) tx_draw(c, b.x + 18, b.y + b.h - 74, d->note, PX_RGB(190,90,30), 12);

    for (int i = 0; i < d->nbtns; ++i) {
        Rect r = d->btn_box[i];
        int prim = (d->btn_code[i] == 1);
        Color bg = prim ? PX_RGB(66,120,210) : PX_RGB(238,238,242);
        if (d->hover_btn == i) bg = prim ? PX_RGB(84,140,230) : PX_RGB(226,230,238);
        if (d->press_btn == i) bg = prim ? PX_RGB(48,96,180)  : PX_RGB(206,212,224);
        cv_fill_rect(c, r, bg);
        cv_rect(c, r, prim ? PX_RGB(40,84,160) : PX_RGB(150,150,160));
        tx_draw_center(c, r, d->btns[i], prim ? CLR_WHITE : CLR_BLACK, 14);
    }
}

static int field_at(Dialog *d, int mx, int my)
{
    for (int i = 0; i < d->nfields; ++i)
        if (rect_contains(d->fld_box[i], mx, my)) return i;
    return -1;
}

static int btn_at(Dialog *d, int mx, int my)
{
    for (int i = 0; i < d->nbtns; ++i)
        if (rect_contains(d->btn_box[i], mx, my)) return i;
    return -1;
}

static void field_append(Dialog *d, int ch)
{
    if (d->active < 0) return;
    char *s = d->buf[d->active];
    int n = (int)strlen(s);
    if (n < 10) {
        if (n == 1 && s[0] == '0') { s[0] = (char)ch; s[1] = 0; return; }
        s[n] = (char)ch; s[n + 1] = 0;
    }
}

static void field_backspace(Dialog *d)
{
    if (d->active < 0) return;
    char *s = d->buf[d->active];
    int n = (int)strlen(s);
    if (n > 0) s[n - 1] = 0;
}

/* 返回 1 表示事件已被对话框消费 */
static int dialog_input(App *a, Dialog *d, XEvent *e)
{
    int mx = a->mx, my = a->my;

    if (e->type == MotionNotify) {
        d->hover_btn = btn_at(d, mx, my);
        return 1;
    }

    if (e->type == ButtonPress && e->xbutton.button == Button1) {
        int b = btn_at(d, mx, my);
        if (b >= 0) { d->press_btn = b; return 1; }
        int f = field_at(d, mx, my);
        if (f >= 0) { d->active = f; return 1; }
        d->active = -1;
        return 1;
    }

    if (e->type == ButtonRelease && e->xbutton.button == Button1) {
        int b = btn_at(d, mx, my);
        int pb = d->press_btn;
        d->press_btn = -1;
        if (pb >= 0 && b == pb) { d->result = d->btn_code[b]; return 2; } /* 2 = 关闭 */
        return 1;
    }

    if (e->type == KeyPress) {
        KeySym ks = XLookupKeysym(&e->xkey, 0);
        if (ks == XK_Return || ks == XK_KP_Enter) { d->result = 1; return 2; }
        if (ks == XK_Escape) { d->result = 2; return 2; }
        if (ks == XK_BackSpace) { field_backspace(d); return 1; }
        if (ks == XK_Tab) {
            if (d->nfields) d->active = (d->active + 1) % d->nfields;
            return 1;
        }
        if (ks >= 0x20 && ks < 0x7F && !(e->xkey.state & ControlMask)) {
            if (d->active >= 0) {
                if (ks >= '0' && ks <= '9') field_append(d, (int)ks);
                else if (ks == '-') {
                    char *s = d->buf[d->active];
                    if (s[0] == '0' && s[1] == 0) { s[0] = '-'; s[1] = 0; }
                }
            }
            return 1;
        }
        return 1;
    }
    return 0;
}

void dlg_show(App *a, Dialog *d)
{
    dlg_layout(d, a);
    d->visible = 1;
    d->result = 0;
}

void dlg_hide(Dialog *d) { d->visible = 0; }

static void full_paint(App *a, Dialog *d)
{
    if (a->paint) { cv_clear(&a->fb, CLR_WHITE); a->paint(a); }
    if (d) dlg_draw(a, d);
}

int dlg_run(App *a, Dialog *d, MenuBar *mb, const char *status)
{
    dlg_show(a, d);
    full_paint(a, d);
    app_present(a);

    while (a->running) {
        XEvent e;
        while (XPending(a->dpy)) {
            XNextEvent(a->dpy, &e);
            if (e.type == ConfigureNotify) {
                a->w = e.xconfigure.width; a->h = e.xconfigure.height;
                cv_resize(&a->fb, a->w, a->h);
                dlg_layout(d, a);
                full_paint(a, d); app_present(a);
                continue;
            }
            if (e.type == Expose) { full_paint(a, d); app_present(a); continue; }
            if (e.type == ClientMessage) { a->running = 0; break; }
            a->mx = (e.type == MotionNotify) ? e.xmotion.x : a->mx;
            a->my = (e.type == MotionNotify) ? e.xmotion.y : a->my;
            if (e.type == ButtonPress || e.type == ButtonRelease) { a->mx = e.xbutton.x; a->my = e.xbutton.y; }
            if (e.type == KeyPress) { a->key = (int)XLookupKeysym(&e.xkey, 0); }

            int r = dialog_input(a, d, &e);
            if (r == 2) { dlg_hide(d); return d->result; }
            if (r == 1) { full_paint(a, d); app_present(a); }
        }
        if (!XPending(a->dpy)) {
            struct timespec ts = {0, 2000000};
            nanosleep(&ts, NULL);
        }
    }
    dlg_hide(d);
    return 0;
}

int dlg_event(App *a, Dialog *d, XEvent *e, MenuBar *mb, const char *status)
{
    if (!d->visible) return 0;
    int r = dialog_input(a, d, e);
    if (r == 2) { dlg_hide(d); return d->result; }
    return 0;
}

/* 对应 AfxMessageBox */
int msgbox(App *a, const char *title, const char *text, MenuBar *mb)
{
    Dialog d;
    dlg_build(&d, title);
    d.note = text;
    dlg_add_button(&d, "确定", 1);
    return dlg_run(a, &d, mb, NULL);
}
