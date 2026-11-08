/* canvasview.h —— 绘图视图（对应 p1 里各 MFC 的 CView 子类）
 *
 * 用 Qt 的 QWidget + QPainter 做设备无关光栅绘制，替代 MFC CDC。
 * 鼠标事件直接对应：mousePressEvent -> WM_LBUTTONDOWN
 *                   mouseMoveEvent  -> WM_MOUSEMOVE
 *                   mouseReleaseEvent -> WM_LBUTTONUP
 */
#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include <QWidget>
#include <QPolygon>
#include <QVector>
#include <QPointF>
#include <QImage>
#include <QString>
#include "algo.h"

/* 视图类型 */
enum ViewId {
    VIEW_MOUSELINE = 0,
    VIEW_SINGLEDRAW,
    VIEW_DLGLINE,
    VIEW_DDA,
    VIEW_BRESENHAM,
    VIEW_CIRCLE,
    VIEW_SCANFILL,
    VIEW_SEEDFILL,
    VIEW_COHEN,
    VIEW_MIDCLIP,
    VIEW_LIANG,
    VIEW_COUNT
};

extern const char *kViewNames[VIEW_COUNT];

class CanvasView : public QWidget
{
    Q_OBJECT
public:
    explicit CanvasView(QWidget *parent = nullptr);
    ~CanvasView() override;

    void setView(ViewId id);
    ViewId view() const { return m_view; }

    void resetScene();
    QString viewName() const;
    QString viewHint() const;
    QString statusText() const;

    /* 给主窗口调用的对话框入口 */
    void setLineFromDialog(double sx, double sy, double ex, double ey);
    void setTriangleFromDialog(int x1, int y1, int x2, int y2, int x3, int y3);
    QPointF startPoint() const { return m_a; }
    QPointF endPoint() const { return m_b; }
    double radius() const { return m_radius; }
    void beginPick(int which);      /* 1=取起点 2=取终点 */
    int  pickStage() const { return m_pick; }
    int  triX3() const { return m_tri3.x(); }
    int  triY3() const { return m_tri3.y(); }

signals:
    void statusChanged();
    void sizeChanged();

protected:
    void paintEvent(QPaintEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;

private:
    /* 数学坐标（y 向上，原点在视图中心） <-> 设备坐标 */
    QPointF toMath(const QPointF &dev) const;
    QPointF toDevice(const QPointF &m) const;

    void drawGrid(QPainter &p);
    void drawMouseLine(QPainter &p);
    void drawSingleDraw(QPainter &p);
    void drawDlgLine(QPainter &p);
    void drawDDA(QPainter &p);
    void drawBresenham(QPainter &p);
    void drawCircle(QPainter &p);
    void drawScanFill(QPainter &p);
    void drawSeedFill(QPainter &p);
    void drawClipView(QPainter &p, int method);
    void drawMark(QPainter &p, const QPointF &math, QColor c, int r = 4);
    void drawPts(QPainter &p, const PtList &l, QColor c, int thick = 1);
    void invalidateSeed();

    ViewId   m_view;
    QPointF  m_a, m_b;           /* 数学坐标端点 */
    bool     m_dragging = false;
    int      m_pick = 0;         /* 取点模式 */
    int      m_stage = 0;
    int      m_dragEnd = 0;      /* 1=拖起点 2=拖终点 */

    QVector<QLineF> m_segs;      /* SingleDraw 已落定线段（数学坐标） */

    double   m_radius = 90.0;
    int      m_ddaMode = 0;      /* 0=星芒 1=两点 */
    int      m_breMode = 0;      /* 0=直线 1=圆 */
    QPointF  m_tri3;             /* 三角形第三顶点 */
    int      m_hoverVert = -1;

    /* 种子填充缓冲 */
    QVector<int> m_seedBuf;
    int      m_seedW = 0, m_seedH = 0;
    int      m_seedCount = 0;
    QPolygon m_seedPoly;         /* 设备坐标多边形 */
    QImage   m_seedImage;        /* 缓存好的合成图，几何不变时直接 blit */
    bool     m_seedDirty = true;

    /* 点列复用缓冲，避免每帧分配 */
    PtList   m_pts;
    PtList   m_pts2;
    SpanList m_spans;

    QString  m_status;
};

#endif
