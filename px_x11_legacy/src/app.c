/* app.c —— CWinApp / CFrameWnd / 消息循环的 Linux(X11 + GLX) 等价实现
 *
 * PDF 对照表：
 *   WM_LBUTTONDOWN  -> ButtonPress(Button1)
 *   WM_LBUTTONUP    -> ButtonRelease(Button1)
 *   WM_MOUSEMOVE    -> MotionNotify
 *   WM_SIZE         -> ConfigureNotify
 *   PIXELFORMATDESCRIPTOR + ChoosePixelFormat + wglCreateContext
 *                   -> glXChooseVisual + glXCreateContext
 *   SwapBuffers(hDC)-> glTexSubImage2D + glXSwapBuffers（整帧纹理上传）
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "px.h"
#include <X11/cursorfont.h>

App *g_app = NULL;

/* 0xAARRGGBB 在小端内存里是 B,G,R,A —— 正好是 GL 的 BGRA 布局 */
#define PX_TEXFMT 0x80E1u

static long now_ms(void)
{
    struct timeval tv; gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000L + tv.tv_usec / 1000L;
}

App *app_create(const char *title, int w, int h, const char *cursor_name)
{
    App *a = (App *)calloc(1, sizeof(App));
    if (!a) return NULL;
    a->dpy = XOpenDisplay(NULL);
    if (!a->dpy) { fprintf(stderr, "px: 打开 X 显示失败（请检查 DISPLAY）\n"); free(a); return NULL; }
    a->screen = DefaultScreen(a->dpy);
    g_app = a;

    int attrib[] = {
        GLX_RGBA, GLX_DOUBLEBUFFER,
        GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8,
        GLX_DEPTH_SIZE, 0, None
    };
    XVisualInfo *vi = glXChooseVisual(a->dpy, a->screen, attrib);
    if (!vi) { fprintf(stderr, "px: glXChooseVisual 失败（无 GLX 视觉）\n"); XCloseDisplay(a->dpy); free(a); return NULL; }

    Window root = RootWindow(a->dpy, a->screen);
    a->cmap = XCreateColormap(a->dpy, root, vi->visual, AllocNone);

    XSetWindowAttributes swa;
    memset(&swa, 0, sizeof(swa));
    swa.colormap = a->cmap;
    swa.border_pixel = 0;
    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
                     ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
                     StructureNotifyMask | LeaveWindowMask | EnterWindowMask;

    a->w = w; a->h = h;
    a->win = XCreateWindow(a->dpy, root, 60, 40, (unsigned)w, (unsigned)h, 0,
                           vi->depth, InputOutput, vi->visual,
                           CWBorderPixel | CWColormap | CWEventMask, &swa);
    XStoreName(a->dpy, a->win, title);
    XSizeHints sh;
    memset(&sh, 0, sizeof(sh));
    sh.x = 60; sh.y = 40; sh.width = w; sh.height = h;
    sh.flags = USPosition | USSize | PMinSize;
    sh.min_width = 640; sh.min_height = 420;
    XSetWMNormalHints(a->dpy, a->win, &sh);

    if (cursor_name) {
        Cursor cur = XCreateFontCursor(a->dpy, XC_crosshair);
        if (cur) XDefineCursor(a->dpy, a->win, cur);
    }

    a->ctx = glXCreateContext(a->dpy, vi, NULL, True);
    XFree(vi);
    if (!a->ctx) { fprintf(stderr, "px: glXCreateContext 失败\n"); XCloseDisplay(a->dpy); free(a); return NULL; }

    XMapWindow(a->dpy, a->win);
    glXMakeCurrent(a->dpy, a->win, a->ctx);

    cv_init(&a->fb, w, h);
    cv_clear(&a->fb, CLR_WHITE);

    glGenTextures(1, &a->tex);
    glBindTexture(GL_TEXTURE_2D, a->tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, PX_TEXFMT, GL_UNSIGNED_BYTE, a->fb.px);

    a->running = 1;
    a->animating = 0;
    return a;
}

void app_set_title(App *a, const char *title)
{
    if (!a || !a->dpy) return;
    XStoreName(a->dpy, a->win, title);
    XFlush(a->dpy);
}

void app_invalidate(App *a) { (void)a; }

Rect app_draw_area(App *a)
{
    Rect r;
    r.x = 0; r.y = MENUBAR_H;
    r.w = a->w;
    r.h = a->h - MENUBAR_H - STATUSBAR_H;
    if (r.h < 1) r.h = 1;
    return r;
}

void app_present(App *a)
{
    glViewport(0, 0, a->w, a->h);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glOrtho(0, a->w, a->h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glDisable(GL_DEPTH_TEST);

    glBindTexture(GL_TEXTURE_2D, a->tex);
    if (a->tex_w != a->w || a->tex_h != a->h) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, a->w, a->h, 0, PX_TEXFMT, GL_UNSIGNED_BYTE, a->fb.px);
        a->tex_w = a->w; a->tex_h = a->h;
    } else {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, a->w, a->h, PX_TEXFMT, GL_UNSIGNED_BYTE, a->fb.px);
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glClearColor(0.92f, 0.92f, 0.92f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glColor4f(1, 1, 1, 1);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2i(0, 0);
        glTexCoord2f(1, 0); glVertex2i(a->w, 0);
        glTexCoord2f(1, 1); glVertex2i(a->w, a->h);
        glTexCoord2f(0, 1); glVertex2i(0, a->h);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glXSwapBuffers(a->dpy, a->win);
}

void app_destroy(App *a)
{
    if (!a) return;
    if (a->dpy) {
        if (a->ctx) { glXMakeCurrent(a->dpy, None, NULL); glXDestroyContext(a->dpy, a->ctx); }
        if (a->win) XDestroyWindow(a->dpy, a->win);
        XCloseDisplay(a->dpy);
    }
    cv_free(&a->fb);
    if (g_app == a) g_app = NULL;
    free(a);
}

static void refresh(App *a, AppPaint paint)
{
    if (paint) { cv_clear(&a->fb, CLR_WHITE); paint(a); }
    app_present(a);
}

void app_run(App *a, AppPaint paint, AppEvent ev)
{
    XEvent e;
    long next_tick = 0;
    refresh(a, paint);

    while (a->running) {
        while (XPending(a->dpy)) {
            XNextEvent(a->dpy, &e);
            switch (e.type) {
            case ConfigureNotify:
                if (e.xconfigure.width != a->w || e.xconfigure.height != a->h) {
                    a->w = e.xconfigure.width;
                    a->h = e.xconfigure.height;
                    cv_resize(&a->fb, a->w, a->h);
                    refresh(a, paint);
                }
                break;
            case MotionNotify:
                a->mx = e.xmotion.x; a->my = e.xmotion.y;
                if (ev) ev(a, &e);
                refresh(a, paint);
                break;
            case ButtonPress:
                a->mx = e.xbutton.x; a->my = e.xbutton.y;
                a->buttons |= (1u << e.xbutton.button);
                if (ev) ev(a, &e);
                refresh(a, paint);
                break;
            case ButtonRelease:
                a->mx = e.xbutton.x; a->my = e.xbutton.y;
                a->buttons &= ~(1u << e.xbutton.button);
                if (ev) ev(a, &e);
                refresh(a, paint);
                break;
            case KeyPress: {
                KeySym ks = XLookupKeysym(&e.xkey, 0);
                a->shift = (e.xkey.state & ShiftMask) != 0;
                a->ctrl  = (e.xkey.state & ControlMask) != 0;
                a->key   = (int)ks;
                if (ev) ev(a, &e);
                refresh(a, paint);
                break; }
            case Expose:
                if (e.xexpose.count == 0) refresh(a, paint);
                break;
            case ClientMessage:
                if ((Atom)e.xclient.data.l[0] == XInternAtom(a->dpy, "WM_DELETE_WINDOW", False))
                    a->running = 0;
                break;
            default:
                break;
            }
        }

        if (a->animating) {
            long t = now_ms();
            if (t >= next_tick) {
                next_tick = t + 16;
                if (ev) { XEvent d; memset(&d, 0, sizeof(d)); d.type = 0; ev(a, &d); }
                refresh(a, paint);
            }
        }
        if (!XPending(a->dpy)) usleep(800);
    }
}
