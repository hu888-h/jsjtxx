/* view.h —— 各“视图”（对应 p1 里每个 MFC 工程的 CView 子类）
 */
#ifndef VIEW_H
#define VIEW_H

#include "px.h"
#include "algo.h"

typedef enum {
    VIEW_MOUSELINE = 0,   /* p1/MouseLine：按下-抬起画直线（WM_LBUTTONDOWN/UP） */
    VIEW_SINGLEDRAW,      /* p1/DrawLineView/SingleDraw：两次点击定线 + 橡皮筋 */
    VIEW_DLGLINE,         /* p1/DrawLineView/SingleDraw2：坐标对话框 + 取点 */
    VIEW_DDA,             /* zip/算法2：DDA 直线 */
    VIEW_BRESENHAM,       /* p1/DrawLine：Bresenham 直线 + 中点圆 */
    VIEW_CIRCLE,          /* 中点圆/椭圆 */
    VIEW_SCANFILL,        /* p1/Fill：扫描线填充三角形 */
    VIEW_SEEDFILL,        /* 种子填充 */
    VIEW_COHEN,           /* zip/算法16：Cohen-Sutherland */
    VIEW_MIDCLIP,         /* zip/算法17：中点分割 */
    VIEW_LIANG,           /* zip/算法18：Liang-Barsky */
    VIEW_TRANSFORM,       /* zip/算法15：二维几何变换（五角星） */
    VIEW_DIALOG,          /* p1/DLGExam：有模式 / 无模式对话框 */
    VIEW_COUNT
} ViewId;

#define SEG_MAX 128

typedef struct {
    ViewId  id;
    Dpt     a, b;              /* 数学坐标（y 向上） */
    int     dragging;
    int     stage;             /* 多步交互阶段 */
    int     flag;
    int     radius;
    int     params[8];
    double  dparams[4];
    Mat3    mat;
    Vtx     poly[8];
    int     npoly;

    /* 已落定的线段（对应 p1 linkList<CLine>） */
    struct { double x1, y1, x2, y2; } seg[SEG_MAX];
    int     nseg;
    char    info[256];
} Scene;

void scene_init(Scene *s);
void scene_draw(App *a, Scene *s, Rect area);
void scene_lbutton_down(App *a, Scene *s, int x, int y);
void scene_lbutton_up(App *a, Scene *s, int x, int y);
void scene_motion(App *a, Scene *s, int x, int y);
void scene_key(App *a, Scene *s, int key);
void scene_reset(Scene *s);
void scene_dialog(Scene *s);          /* 打开与该视图关联的对话框 */

extern const char *kViewName[VIEW_COUNT];
extern const char *kViewHint[VIEW_COUNT];

#endif
