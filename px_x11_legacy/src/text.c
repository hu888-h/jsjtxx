/* text.c —— FreeType 文本绘制（用于菜单/对话框/状态栏，替代 CDC::TextOut）
 * 支持 UTF-8，字体默认 Noto Sans CJK（覆盖简体中文）。
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "px.h"
#include <ft2build.h>
#include FT_FREETYPE_H

static FT_Library g_lib;
static FT_Face    g_face;
static int        g_ok;
static int        g_size = 0;

static const char *kFallback[] = {
    "/usr/share/fonts/noto-cjk/NotoSansCJK-Regular.ttc",
    "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc",
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
    NULL
};

int tx_init(const char *fontpath, int face_index)
{
    if (g_ok) return 1;
    if (FT_Init_FreeType(&g_lib)) return 0;
    if (fontpath && FT_New_Face(g_lib, fontpath, face_index, &g_face) == 0) {
        g_ok = 1; return 1;
    }
    for (int i = 0; kFallback[i]; ++i) {
        if (FT_New_Face(g_lib, kFallback[i], 0, &g_face) == 0) { g_ok = 1; return 1; }
    }
    return 0;
}

static int ensure_size(int size)
{
    if (!g_ok) return 0;
    if (size == g_size) return 1;
    if (FT_Set_Pixel_Sizes(g_face, 0, (unsigned)size)) return 0;
    g_size = size;
    return 1;
}

int tx_height(int size) { return size + 4; }

int tx_width(int size, const char *utf8)
{
    if (!utf8) return 0;
    if (!ensure_size(size)) return (int)strlen(utf8) * (size / 2 + 1);
    int w = 0;
    const char *p = utf8;
    while (*p) {
        unsigned cp; int n = 0;
        unsigned char c = (unsigned char)*p;
        if (c < 0x80) { cp = c; n = 1; }
        else if ((c >> 5) == 0x6) { cp = c & 0x1F; n = 2; }
        else if ((c >> 4) == 0xE) { cp = c & 0x0F; n = 3; }
        else if ((c >> 3) == 0x1E) { cp = c & 0x07; n = 4; }
        else { cp = c; n = 1; }
        for (int i = 1; i < n && p[i]; ++i) cp = (cp << 6) | (p[i] & 0x3F);
        p += n;
        if (FT_Load_Char(g_face, cp, FT_LOAD_RENDER)) { w += size / 2; continue; }
        w += g_face->glyph->advance.x >> 6;
    }
    return w;
}

void tx_draw(Canvas *c, int x, int y, const char *utf8, Color col, int size)
{
    if (!utf8) return;
    if (!ensure_size(size)) return;
    int baseline = y + (int)(g_face->size->metrics.ascender >> 6);
    const char *p = utf8;
    while (*p) {
        unsigned cp; int n = 1;
        unsigned char ch = (unsigned char)*p;
        if (ch < 0x80) { cp = ch; n = 1; }
        else if ((ch >> 5) == 0x6) { cp = ch & 0x1F; n = 2; }
        else if ((ch >> 4) == 0xE) { cp = ch & 0x0F; n = 3; }
        else if ((ch >> 3) == 0x1E) { cp = ch & 0x07; n = 4; }
        else { cp = ch; n = 1; }
        for (int i = 1; i < n && p[i]; ++i) cp = (cp << 6) | (p[i] & 0x3F);
        p += n;
        if (FT_Load_Char(g_face, cp, FT_LOAD_RENDER)) continue;
        FT_GlyphSlot g = g_face->glyph;
        int gx = x + g->bitmap_left;
        int gy = baseline - g->bitmap_top;
        for (int row = 0; row < (int)g->bitmap.rows; ++row) {
            for (int cc = 0; cc < (int)g->bitmap.width; ++cc) {
                unsigned char a = g->bitmap.buffer[row * g->bitmap.pitch + cc];
                if (a) cv_blend_pixel(c, gx + cc, gy + row, col, a);
            }
        }
        x += g->advance.x >> 6;
    }
}

void tx_draw_center(Canvas *c, Rect box, const char *utf8, Color col, int size)
{
    int w = tx_width(size, utf8);
    int h = tx_height(size);
    tx_draw(c, box.x + (box.w - w) / 2, box.y + (box.h - h) / 2, utf8, col, size);
}
