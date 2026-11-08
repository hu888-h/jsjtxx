/* canvasview.cpp —— 各视图的绘制与鼠标交互（对应 p1 各 CView 子类）
 *
 * 坐标约定：数学坐标 y 向上、原点在绘图区中心；设备坐标 y 向下、原点左上。
 * 等价于 MFC 的 SetMapMode(MM_ANISOTROPIC) + SetViewportExt(w,-h) + SetViewportOrg(w/2,h/2)。
 */
#include "canvasview.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QtMath>
#include <QFontMetrics>

const char *kViewNames[VIEW_COUNT] = {
    "MouseLine（按下-抬起画线）",
    "SingleDraw（两次点击 + 橡皮筋）",
    "坐标对话框定线（取点流程）",
    "DDA 直线（算法2）",
    "Bresenham 直线 / 中点圆",
    "中点圆（8 对称）",
    "扫描线填充（p1/Fill）",
    "种子填充（4 连通行程版）",
    "Cohen-Sutherland 裁剪（算法16）",
    "中点分割裁剪（算法17）",
    "Liang-Barsky 裁剪（算法18）",
};

static const char *kHints[VIEW_COUNT] = {
    "按住左键拖动，抬起完成直线（mousePress/mouseRelease）",
    "第一次点击定起点，移动预览，第二次点击落定；右键清空",
    "点「取起点/取终点」后回视图点选，或在对话框输入坐标",
    "点击绘图区改变半径；算法2 星芒图 0..355° 每 5° 一条",
    "拖动左键画 Bresenham 直线；数字键 1 切到中点圆",
    "点击移动圆心，+/- 改半径",
    "拖动三个顶点；菜单「对话框」可精确输入三角形",
    "点击改变多边形位置，种子随重心重算",
    "拖动两个端点，观察 Cohen-Sutherland 的可见段",
    "拖动两个端点，观察中点分割的二分逼近",
    "拖动两个端点，观察 Liang-Barsky 的参数裁剪",
};

CanvasView::CanvasView(QWidget *parent)
    : QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setMinimumSize(640, 420);
    setAttribute(Qt::WA_OpaquePaintEvent, true);

    pl_init(&m_pts, 4096);
    pl_init(&m_pts2, 2048);
    span_init(&m_spans, 512);

    m_view = VIEW_MOUSELINE;
    resetScene();
}

CanvasView::~CanvasView()
{
    pl_free(&m_pts);
    pl_free(&m_pts2);
    span_free(&m_spans);
}

QString CanvasView::viewName() const { return QString::fromUtf8(kViewNames[m_view]); }
QString CanvasView::viewHint() const { return QString::fromUtf8(kHints[m_view]); }
QString CanvasView::statusText() const { return m_status; }

void CanvasView::resetScene()
{
    m_dragging = false;
    m_pick = 0;
    m_stage = 0;
    m_dragEnd = 0;
    m_hoverVert = -1;
    m_seedDirty = true;

    switch (m_view) {
    case VIEW_MOUSELINE:
        m_a = QPointF(-150, -80); m_b = QPointF(150, 80);
        break;
    case VIEW_SINGLEDRAW:
        m_a = QPointF(-120, 0); m_b = QPointF(120, 0);
        m_segs.clear();
        break;
    case VIEW_DLGLINE:
        m_a = QPointF(-180, -90); m_b = QPointF(180, 90);
        break;
    case VIEW_DDA:
        m_radius = 150.0; m_ddaMode = 0;
        m_a = QPointF(-200, -120); m_b = QPointF(200, 130);
        break;
    case VIEW_BRESENHAM:
        m_a = QPointF(-200, -120); m_b = QPointF(200, 130);
        m_breMode = 0;
        break;
    case VIEW_CIRCLE:
        m_a = QPointF(0, 0); m_b = QPointF(0, 0);
        m_radius = 90.0;
        break;
    case VIEW_SCANFILL:
        m_a = QPointF(-140, -80); m_b = QPointF(150, -60);
        m_tri3 = QPointF(0, 120);
        break;
    case VIEW_SEEDFILL:
        m_a = QPointF(0, 0); m_b = QPointF(0, 0);
        m_radius = 110.0;
        break;
    case VIEW_COHEN:
    case VIEW_MIDCLIP:
    case VIEW_LIANG:
        m_a = QPointF(-330, -170); m_b = QPointF(330, 175);
        break;
    default:
        break;
    }
    m_status.clear();
    update();
}

void CanvasView::setView(ViewId id)
{
    m_view = id;
    resetScene();
}

QPointF CanvasView::toMath(const QPointF &dev) const
{
    return QPointF(dev.x() - width() / 2.0, -(dev.y() - height() / 2.0));
}

QPointF CanvasView::toDevice(const QPointF &m) const
{
    return QPointF(width() / 2.0 + m.x(), height() / 2.0 - m.y());
}

void CanvasView::drawMark(QPainter &p, const QPointF &math, QColor c, int r)
{
    QPointF d = toDevice(math);
    p.setPen(QPen(c, 1.6));
    p.setBrush(Qt::NoBrush);
    p.drawEllipse(d, r, r);
    p.drawLine(QPointF(d.x() - r - 2, d.y()), QPointF(d.x() + r + 2, d.y()));
    p.drawLine(QPointF(d.x(), d.y() - r - 2), QPointF(d.x(), d.y() + r + 2));
}

void CanvasView::drawPts(QPainter &p, const PtList &l, QColor c, int thick)
{
    /* 点列是数学坐标：批量转成设备坐标后用 drawPoints 一次提交，
     * 避免逐个 drawPoint 的调用开销（Bresenham 一条线几千点）。 */
    if (l.n <= 0) return;
    QPointF *buf = new QPointF[(size_t)l.n];
    for (int i = 0; i < l.n; ++i)
        buf[i] = QPointF(width() / 2.0 + l.x[i], height() / 2.0 - l.y[i]);
    p.setPen(QPen(c, thick));
    p.drawPoints(buf, l.n);
    delete[] buf;
}

void CanvasView::drawGrid(QPainter &p)
{
    int step = 50;
    p.setPen(QPen(QColor(238, 240, 244), 1));
    for (int x = -width() / 2; x <= width() / 2; x += step)
        p.drawLine(QPointF(width() / 2.0 + x, 0), QPointF(width() / 2.0 + x, height()));
    for (int y = -height() / 2; y <= height() / 2; y += step)
        p.drawLine(QPointF(0, height() / 2.0 - y), QPointF(width(), height() / 2.0 - y));

    p.setPen(QPen(QColor(205, 208, 214), 1));
    p.drawLine(QPointF(0, height() / 2.0), QPointF(width(), height() / 2.0));
    p.drawLine(QPointF(width() / 2.0, 0), QPointF(width() / 2.0, height()));

    p.setPen(QColor(130, 130, 130));
    QFont f = p.font(); f.setPointSize(8); p.setFont(f);
    for (int x = -200; x <= 200; x += 100) {
        if (!x) continue;
        p.drawText(QPointF(width() / 2.0 + x - 12, height() / 2.0 + 16), QString::number(x));
    }
    for (int y = -200; y <= 200; y += 100) {
        if (!y) continue;
        p.drawText(QPointF(width() / 2.0 + 6, height() / 2.0 - y - 6), QString::number(y));
    }
    p.drawText(QPointF(width() / 2.0 + 6, height() / 2.0 + 16), "O");
}

void CanvasView::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);
    p.fillRect(rect(), Qt::white);
    drawGrid(p);

    switch (m_view) {
    case VIEW_MOUSELINE:  drawMouseLine(p); break;
    case VIEW_SINGLEDRAW: drawSingleDraw(p); break;
    case VIEW_DLGLINE:    drawDlgLine(p); break;
    case VIEW_DDA:        drawDDA(p); break;
    case VIEW_BRESENHAM:  drawBresenham(p); break;
    case VIEW_CIRCLE:     drawCircle(p); break;
    case VIEW_SCANFILL:   drawScanFill(p); break;
    case VIEW_SEEDFILL:   drawSeedFill(p); break;
    case VIEW_COHEN:      drawClipView(p, 16); break;
    case VIEW_MIDCLIP:    drawClipView(p, 17); break;
    case VIEW_LIANG:      drawClipView(p, 18); break;
    default: break;
    }

    /* 左上角说明 */
    p.setPen(QColor(70, 70, 70));
    QFont f = p.font(); f.setPointSize(9); p.setFont(f);
    p.drawText(QPointF(12, 22), viewName());
}

/* ============ 1. MouseLine：按下记起点，抬起连线（p1/MouseLine） ============ */
void CanvasView::drawMouseLine(QPainter &p)
{
    if (m_dragging)
        drawMark(p, m_a, QColor(232, 140, 40), 5);
    if (!m_dragging && m_a != m_b) {
        p.setPen(QPen(QColor(214, 45, 45), 2));
        p.drawLine(toDevice(m_a), toDevice(m_b));
    }
    p.setPen(QColor(70, 70, 70));
    p.drawText(toDevice(m_a) + QPointF(8, -8), "P1");
    p.drawText(toDevice(m_b) + QPointF(8, 16), "P2");
}

/* ============ 2. SingleDraw：两次点击 + 橡皮筋（p1/SingleDraw） ============ */
void CanvasView::drawSingleDraw(QPainter &p)
{
    p.setPen(QPen(QColor(40, 80, 200), 2));
    for (const QLineF &l : m_segs)
        p.drawLine(toDevice(l.p1()), toDevice(l.p2()));

    if (m_dragging) {
        p.setPen(QPen(QColor(232, 140, 40), 1, Qt::DashLine));
        p.drawLine(toDevice(m_a), toDevice(m_b));
        drawMark(p, m_a, QColor(232, 140, 40), 4);
    }
    p.setPen(QColor(70, 70, 70));
    p.drawText(QPointF(12, 40), QString("已保存 %1 条线段").arg(m_segs.size()));
}

/* ============ 3. 坐标对话框定线（p1/SingleDraw2） ============ */
void CanvasView::drawDlgLine(QPainter &p)
{
    p.setPen(QPen(QColor(0, 150, 70), 2));
    p.drawLine(toDevice(m_a), toDevice(m_b));
    drawMark(p, m_a, QColor(214, 45, 45));
    drawMark(p, m_b, QColor(214, 45, 45));
    p.setPen(QColor(70, 70, 70));
    p.drawText(toDevice(m_a) + QPointF(8, -10),
               QString("(%1, %2)").arg((int)m_a.x()).arg((int)m_a.y()));
    p.drawText(toDevice(m_b) + QPointF(8, 14),
               QString("(%1, %2)").arg((int)m_b.x()).arg((int)m_b.y()));
    if (m_pick == 1 || m_pick == 2) {
        p.setPen(QColor(232, 140, 40));
        p.drawText(QPointF(12, 42),
                   m_pick == 1 ? QString::fromUtf8("「取起点」模式：点击绘图区") 
                               : QString::fromUtf8("「取终点」模式：点击绘图区"));
    }
}

/* ============ 4. DDA（zip/算法2） ============ */
void CanvasView::drawDDA(QPainter &p)
{
    if (m_ddaMode == 0) {
        pl_clear(&m_pts);
        int R = (int)m_radius;
        for (int ang = 0; ang < 360; ang += 5) {
            double x = R * qCos(qDegreesToRadians((double)ang));
            double y = R * qSin(qDegreesToRadians((double)ang));
            line_dda(0, 0, (int)qRound(x), (int)qRound(y), &m_pts);
        }
        drawPts(p, m_pts, QColor(40, 80, 200), 1);
        p.setPen(QColor(70, 70, 70));
        p.drawText(QPointF(12, 42), QString("半径 R = %1（点击绘图区改变）").arg(R));
        m_status = QString("半径 R=%1，共 %2 条直线，%3 个像素点").arg(R)
                       .arg(72).arg(m_pts.n);
    } else {
        pl_clear(&m_pts);
        line_dda((int)m_a.x(), (int)m_a.y(), (int)m_b.x(), (int)m_b.y(), &m_pts);
        drawPts(p, m_pts, QColor(40, 80, 200), 2);
        p.setPen(QColor(70, 70, 70));
        p.drawText(QPointF(12, 42),
                   QString("DDA (%1,%2)->(%3,%4)  步数=%5")
                       .arg((int)m_a.x()).arg((int)m_a.y())
                       .arg((int)m_b.x()).arg((int)m_b.y()).arg(m_pts.n));
    }
}

/* ============ 5. Bresenham 直线 / 中点圆（p1/DrawLine） ============ */
void CanvasView::drawBresenham(QPainter &p)
{
    pl_clear(&m_pts);
    if (m_breMode == 0) {
        line_bresenham((int)m_a.x(), (int)m_a.y(), (int)m_b.x(), (int)m_b.y(), &m_pts);
        drawPts(p, m_pts, QColor(214, 45, 45), 2);
        p.setPen(QColor(70, 70, 70));
        p.drawText(QPointF(12, 42),
                   QString("Bresenham (%1,%2)->(%3,%4)  %5 点")
                       .arg((int)m_a.x()).arg((int)m_a.y())
                       .arg((int)m_b.x()).arg((int)m_b.y()).arg(m_pts.n));
    } else {
        circle_midpoint((int)m_a.x(), (int)m_a.y(), (int)m_radius, &m_pts);
        drawPts(p, m_pts, QColor(214, 45, 45), 2);
        p.setPen(QColor(70, 70, 70));
        p.drawText(QPointF(12, 42),
                   QString("中点圆 圆心(%1,%2) r=%3  %4 点")
                       .arg((int)m_a.x()).arg((int)m_a.y())
                       .arg((int)m_radius).arg(m_pts.n));
    }
    m_status = QString("输出 %1 个像素点").arg(m_pts.n);
}

/* ============ 6. 中点圆（8 对称） ============ */
void CanvasView::drawCircle(QPainter &p)
{
    pl_clear(&m_pts);
    circle_midpoint((int)m_a.x(), (int)m_a.y(), (int)m_radius, &m_pts);
    drawPts(p, m_pts, QColor(0, 150, 70), 2);
    /* 同时画 Bresenham 圆做对照（灰色虚线） */
    pl_clear(&m_pts2);
    circle_bresenham((int)m_a.x(), (int)m_a.y(), (int)m_radius, &m_pts2);
    drawPts(p, m_pts2, QColor(190, 190, 196), 1);
    drawMark(p, m_a, QColor(214, 45, 45));
    p.setPen(QColor(70, 70, 70));
    p.drawText(QPointF(12, 42),
               QString("中点圆 r=%1（绿） / Bresenham 圆（灰）  各 %2 点")
                   .arg((int)m_radius).arg(m_pts.n));
    m_status = QString("r=%1，%2 点").arg((int)m_radius).arg(m_pts.n);
}

/* ============ 7. 扫描线填充（p1/Fill，AET 优化版） ============ */
void CanvasView::drawScanFill(QPainter &p)
{
    Vtx v[3];
    v[0].x = (int)m_a.x(); v[0].y = (int)m_a.y();
    v[1].x = (int)m_b.x(); v[1].y = (int)m_b.y();
    v[2].x = (int)m_tri3.x(); v[2].y = (int)m_tri3.y();

    span_clear(&m_spans);
    int ns = fill_scanline_spans(v, 3, -100000, 100000, &m_spans);

    /* 一次 drawRect/逐跨度画填充，不用逐像素 */
    p.setPen(Qt::NoPen);
    for (int i = 0; i < m_spans.n; ++i) {
        QPointF a = toDevice(QPointF(m_spans.s[i].xs, m_spans.s[i].y));
        p.fillRect(QRectF(a.x(), a.y(), m_spans.s[i].xe - m_spans.s[i].xs + 1, 1),
                   QColor(80, 150, 235));
    }

    QPolygonF poly;
    for (int i = 0; i < 3; ++i) poly << toDevice(QPointF(v[i].x, v[i].y));
    p.setPen(QPen(QColor(0, 150, 70), 2));
    p.drawPolygon(poly);
    for (int i = 0; i < 3; ++i) drawMark(p, QPointF(v[i].x, v[i].y), QColor(214, 45, 45), 4);

    p.setPen(QColor(70, 70, 70));
    p.drawText(QPointF(12, 42),
               QString("(%1,%2) (%3,%4) (%5,%6)")
                   .arg(v[0].x).arg(v[0].y).arg(v[1].x).arg(v[1].y).arg(v[2].x).arg(v[2].y));
    m_status = QString("扫描线跨度 %1 条（AET 算法）").arg(ns);
}

/* ============ 8. 种子填充（4 连通行程版） ============ */
void CanvasView::invalidateSeed() { m_seedDirty = true; }

void CanvasView::drawSeedFill(QPainter &p)
{
    int w = width(), h = height();
    if (!w || !h) return;
    if (m_seedDirty || m_seedW != w || m_seedH != h) {
        m_seedW = w; m_seedH = h;
        m_seedBuf.assign((size_t)w * (size_t)h, 0);

        /* 五边形边界写入设备坐标缓冲 */
        int n = 5;
        m_seedPoly.clear();
        for (int i = 0; i < n; ++i) {
            double ang = i * 2.0 * M_PI / n + 0.4;
            QPointF m(m_a.x() + m_radius * qCos(ang), m_a.y() + m_radius * qSin(ang));
            m_seedPoly << toDevice(m).toPoint();
        }
        /* 边界光栅化：用 QPainter 画到 1bit 缓冲代价高，这里用 Bresenham 直写 */
        for (int i = 0; i < n; ++i) {
            QPoint p0 = m_seedPoly[i], p1 = m_seedPoly[(i + 1) % n];
            int dx = qAbs(p1.x() - p0.x()), sx = p0.x() < p1.x() ? 1 : -1;
            int dy = -qAbs(p1.y() - p0.y()), sy = p0.y() < p1.y() ? 1 : -1;
            int err = dx + dy, X = p0.x(), Y = p0.y();
            for (;;) {
                if ((unsigned)X < (unsigned)w && (unsigned)Y < (unsigned)h)
                    m_seedBuf[(size_t)Y * w + X] = 2;
                if (X == p1.x() && Y == p1.y()) break;
                int e2 = 2 * err;
                if (e2 >= dy) { err += dy; X += sx; }
                if (e2 <= dx) { err += dx; Y += sy; }
            }
        }

        /* 重心作为种子 */
        double gx = 0, gy = 0;
        for (int i = 0; i < n; ++i) { gx += m_seedPoly[i].x(); gy += m_seedPoly[i].y(); }
        int sx = (int)(gx / n), sy = (int)(gy / n);
        if (sx >= 0 && sy >= 0 && sx < w && sy < h && m_seedBuf[(size_t)sy * w + sx] == 2) { sx += 1; sy += 1; }
        m_seedCount = fill_seed(m_seedBuf.data(), w, h, sx, sy, 2, 1);
        m_seedDirty = false;

        /* 把 int 缓冲一次性转成 QImage（只在几何变化时做） */
        m_seedImage = QImage(w, h, QImage::Format_RGB32);
        m_seedImage.fill(Qt::white);
        for (int y = 0; y < h; ++y) {
            const int *row = m_seedBuf.constData() + (size_t)y * w;
            QRgb *dst = reinterpret_cast<QRgb *>(m_seedImage.scanLine(y));
            int x = 0;
            while (x < w) {
                if (row[x] == 1)      { dst[x] = qRgb(90, 160, 240); ++x; }
                else if (row[x] == 2) { dst[x] = qRgb(0, 150, 70);   ++x; }
                else ++x;
            }
        }
    }

    /* 缓存好的 QImage 直接 blit：比重建 QRect 快一个数量级 */
    if (!m_seedImage.isNull() && m_seedImage.size() == size())
        p.drawImage(0, 0, m_seedImage);

    p.setPen(QColor(70, 70, 70));
    p.drawText(QPointF(12, 42),
               QString("4 连通种子填充：填充 %1 像素（行程加速）").arg(m_seedCount));
    m_status = QString("种子填充 %1 像素").arg(m_seedCount);
}

/* ============ 9/10/11. 三种裁剪 ============ */
void CanvasView::drawClipView(QPainter &p, int method)
{
    const double xl = -200, yb = -110, xr = 200, yt = 110;
    QPointF tl = toDevice(QPointF(xl, yt));
    QPointF br = toDevice(QPointF(xr, yb));
    QRectF win(tl, br);
    p.fillRect(win, QColor(120, 170, 230, 26));
    p.setPen(QPen(QColor(40, 80, 200), 1, Qt::DashLine));
    p.setBrush(Qt::NoBrush);
    p.drawRect(win);

    /* 原线段（灰） */
    p.setPen(QPen(QColor(160, 160, 165), 1));
    p.drawLine(toDevice(m_a), toDevice(m_b));
    drawMark(p, m_a, QColor(150, 150, 150), 4);
    drawMark(p, m_b, QColor(150, 150, 150), 4);

    Dpt p0 = { m_a.x(), m_a.y() }, p1 = { m_b.x(), m_b.y() };
    Dpt o0 = {0,0}, o1 = {0,0};
    int ok = 0;
    QString name;
    if (method == 16) {
        ok = clip_cohen_sutherland(&p0, &p1, xl, yb, xr, yt, &o0, &o1);
        name = "Cohen-Sutherland";
    } else if (method == 17) {
        ok = clip_midpoint(&p0, &p1, xl, yb, xr, yt, &o0, &o1);
        name = QString::fromUtf8("中点分割");
    } else {
        double ax, ay, bx, by;
        ok = clip_liang_barsky(p0.x, p0.y, p1.x, p1.y, xl, yb, xr, yt, &ax, &ay, &bx, &by);
        if (ok) { o0.x = ax; o0.y = ay; o1.x = bx; o1.y = by; }
        name = "Liang-Barsky";
    }

    p.setPen(QColor(70, 70, 70));
    if (ok) {
        p.setPen(QPen(QColor(214, 45, 45), 2));
        p.drawLine(toDevice(QPointF(o0.x, o0.y)), toDevice(QPointF(o1.x, o1.y)));
        drawMark(p, QPointF(o0.x, o0.y), QColor(232, 140, 40), 5);
        drawMark(p, QPointF(o1.x, o1.y), QColor(232, 140, 40), 5);
        p.setPen(QColor(70, 70, 70));
        p.drawText(QPointF(12, 42),
                   QString("%1 可见段 (%2,%3) -> (%4,%5)")
                       .arg(name)
                       .arg(o0.x, 0, 'f', 1).arg(o0.y, 0, 'f', 1)
                       .arg(o1.x, 0, 'f', 1).arg(o1.y, 0, 'f', 1));
        m_status = QString("可见段长度 %1").arg(
            qSqrt(qPow(o1.x - o0.x, 2) + qPow(o1.y - o0.y, 2)), 0, 'f', 1);
    } else {
        p.drawText(QPointF(12, 42), QString("%1 整段不可见（简弃）").arg(name));
        m_status = "整段不可见";
    }
}

/* ============ 鼠标 / 键盘（消息映射） ============ */
void CanvasView::mousePressEvent(QMouseEvent *e)
{
    QPointF m = toMath(e->position());

    if (e->button() == Qt::RightButton) {
        resetScene();
        emit statusChanged();
        return;
    }
    if (e->button() != Qt::LeftButton) return;

    switch (m_view) {
    case VIEW_MOUSELINE:
        m_a = m; m_b = m;
        m_dragging = true;
        break;

    case VIEW_SINGLEDRAW:
        if (m_dragging) {
            m_b = m;
            m_dragging = false;
            m_segs.append(QLineF(m_a, m_b));
        } else {
            m_a = m; m_b = m;
            m_dragging = true;
        }
        break;

    case VIEW_DLGLINE:
        if (m_pick == 1) { m_a = m; m_pick = 0; }
        else if (m_pick == 2) { m_b = m; m_pick = 0; }
        break;

    case VIEW_DDA:
        if (m_ddaMode == 0) {
            m_radius = qMax(10.0, qSqrt(m.x() * m.x() + m.y() * m.y()));
        } else {
            m_a = m; m_dragging = true;
        }
        break;

    case VIEW_BRESENHAM:
        m_a = m;
        if (m_breMode == 0) m_dragging = true;
        break;

    case VIEW_CIRCLE:
        m_a = m;
        break;

    case VIEW_SCANFILL: {
        /* 抓最近的顶点 */
        QPointF verts[3] = { m_a, m_b, m_tri3 };
        int best = -1; double bestD = 900;
        for (int i = 0; i < 3; ++i) {
            double d = QLineF(verts[i], m).length();
            if (d < bestD) { bestD = d; best = i; }
        }
        if (best >= 0) {
            m_hoverVert = best;
            m_dragEnd = best + 1;
        } else {
            /* 整体平移 */
            QPointF delta = m - (m_a + m_b + m_tri3) / 3.0;
            m_a += delta; m_b += delta; m_tri3 += delta;
        }
        break; }

    case VIEW_SEEDFILL:
        m_a = m;
        invalidateSeed();
        break;

    case VIEW_COHEN:
    case VIEW_MIDCLIP:
    case VIEW_LIANG: {
        double da = QLineF(m_a, m).length();
        double db = QLineF(m_b, m).length();
        if (da <= db) { m_a = m; m_dragEnd = 1; }
        else          { m_b = m; m_dragEnd = 2; }
        m_dragging = true;
        break; }

    default:
        break;
    }
    update();
    emit statusChanged();
}

void CanvasView::mouseMoveEvent(QMouseEvent *e)
{
    QPointF m = toMath(e->position());

    switch (m_view) {
    case VIEW_SINGLEDRAW:
        if (m_dragging) { m_b = m; update(); }
        break;
    case VIEW_DDA:
    case VIEW_BRESENHAM:
        if (m_dragging) { m_b = m; update(); }
        break;
    case VIEW_SCANFILL:
        if (m_dragging && m_dragEnd > 0) {
            if (m_dragEnd == 1) m_a = m;
            else if (m_dragEnd == 2) m_b = m;
            else m_tri3 = m;
            update();
        }
        break;
    case VIEW_COHEN:
    case VIEW_MIDCLIP:
    case VIEW_LIANG:
        if (m_dragging) {
            if (m_dragEnd == 1) m_a = m; else if (m_dragEnd == 2) m_b = m;
            update();
        }
        break;
    default:
        break;
    }
}

void CanvasView::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton) return;
    QPointF m = toMath(e->position());

    switch (m_view) {
    case VIEW_MOUSELINE:
        if (m_dragging) { m_b = m; m_dragging = false; }
        break;
    case VIEW_DDA:
    case VIEW_BRESENHAM:
        if (m_dragging) { m_b = m; m_dragging = false; }
        break;
    case VIEW_SCANFILL:
        m_dragging = false; m_dragEnd = 0; m_hoverVert = -1;
        break;
    case VIEW_COHEN:
    case VIEW_MIDCLIP:
    case VIEW_LIANG:
        m_dragging = false; m_dragEnd = 0;
        break;
    default:
        break;
    }
    update();
    emit statusChanged();
}

void CanvasView::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Plus:
    case Qt::Key_Equal:
        m_radius += 10; invalidateSeed(); update(); break;
    case Qt::Key_Minus:
        m_radius = qMax(5.0, m_radius - 10); invalidateSeed(); update(); break;
    case Qt::Key_1:
        if (m_view == VIEW_BRESENHAM) m_breMode = 1 - m_breMode;
        else if (m_view == VIEW_DDA) m_ddaMode = 1 - m_ddaMode;
        update(); break;
    case Qt::Key_R:
        resetScene(); emit statusChanged(); return;
    default:
        QWidget::keyPressEvent(e);
        return;
    }
    emit statusChanged();
}

void CanvasView::resizeEvent(QResizeEvent *e)
{
    invalidateSeed();
    QWidget::resizeEvent(e);
    emit sizeChanged();
}

void CanvasView::setLineFromDialog(double sx, double sy, double ex, double ey)
{
    m_a = QPointF(sx, sy);
    m_b = QPointF(ex, ey);
    invalidateSeed();
    update();
}

void CanvasView::setTriangleFromDialog(int x1, int y1, int x2, int y2, int x3, int y3)
{
    m_a = QPointF(x1, y1);
    m_b = QPointF(x2, y2);
    m_tri3 = QPointF(x3, y3);
    invalidateSeed();
    update();
}

void CanvasView::beginPick(int which)
{
    m_pick = which;
    update();
}
