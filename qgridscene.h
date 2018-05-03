#ifndef QGRIDSCENE_H
#define QGRIDSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPolygonF>
#include <QPointF>
#include <QPainterPath>
#include <QPainter>
#include <QInputDialog>
#include <QTextDocument>
#include <QTextOption>
#include <QGraphicsPolygonItem>
#include <QToolButton>
#include <QSettings>
#include "caline.h"
#include "cagraphicsitem.h"

class CALine;

class QGridScene : public QGraphicsScene
{
  Q_OBJECT

public:
    QGridScene(qreal x, qreal y, qreal width, qreal height, int gWidth = 0, int gHeight = 0, int bSize = 0, int s = 0, QColor b = Qt::white, int cSize = 1, int tDir = 0, QToolButton *addChar = false, int thick = 1, int ls = Qt::DotLine, bool client = FALSE, QObject *parent = 0);
    ~QGridScene();
    void setShape(int s) {m_Shape = s;}
    void setColour(QColor b) {m_Colour = b;}
    void setCSize(int s) {m_cSize = s;}
    void setAddChar(QToolButton *on) {m_addChar = on;}
    void setTDir(int t) {m_tDir = t;}
    void clearGrid(bool total = false, bool saveBackground = false);
    void setBackgroundColour(QColor c);
    void setColumnCount(int c) {m_gWidth = c;}
    void setRowCount(int c) {m_gHeight = c;}
    void setBoxSize(int s) {m_bSize = s;}
    int getWidth() {return m_gWidth;}
    int getHeight() {return m_gHeight;}
    int getBoxSize() {return m_bSize;}
    int getRadius() {return m_radius;}
    QColor getBackgroundColour() {return m_bgColour;}
    QPoint mapPoint(qreal cx, qreal cy);
    void drawCircle(int x, int y, QColor c, int size = 1, QString letter = QString());
    void drawBlock(int x, int y, QColor c, QString letter = QString());
    void drawCross(int x, int y, QColor c);
    void drawTriangle(int x, int y, QColor c, int facing = 0);
		void drawCAItem(CAGraphicsItem *i);
    void undrawItem(int x, int y);
    void undrawItem(quint16 check);
    void startDrag(int x, int y);
    void finishDrag(int x, int y);
    void drawLine(double x1, double y1, double x2, double y2, QPen pen);
    void setLThick(int s) { m_lThick = s; }
    QPoint coord2pixel(double x, double y);
    void setRadius(int r);
    int hMargin() {return m_hMargin;}
    int vMargin() {return m_vMargin;}
    void resizeScene(int width, int height, int size);
    void setClient(bool client);
		
private:
    bool eventFilter(QObject *watched, QEvent *event);
    int m_gWidth;
    int m_gHeight;
    int m_bSize;
    int m_Shape;
    int m_cSize;
    int m_tDir;
    int m_hMargin;
    int m_vMargin;
    int m_lThick;
    int m_lStyle;
    bool m_Client;
    QToolButton *m_addChar;
    QGraphicsItem *m_iDrag;
    QColor m_Colour;
    QColor m_bgColour;
    QColor m_lnColour;
    QColor m_coColour;
		int m_radius;
    QPointF dmapPoint(qreal cx, qreal cy);
    QPoint mapCorner(qreal cx, qreal cy, int prefer = 0);
		void handleMask(QPoint p);

signals:
    void circleDrawn(int x, int y, QColor c, int size, QString letter);
    void blockDrawn(int x, int y, QColor c, QString letter);
    void crossDrawn(int x, int y, QColor c);
    void triangleDrawn(int x, int y, QColor c, int facing);
    void lineDrawn(double x1, double x2, double y1, double y2, QPen pen);
    void lineDrawn(CALine *l);
    void itemRemoved(int x, int y);
    void itemRemoved(quint16 check);
    void dragStarted(int x, int y);
    void dragFinished(int x, int y);
		void caItemDrawn(CAGraphicsItem * i);
    void totalClear();
    void zoom(int dir);
};

#endif // QGRIDSCENE_H
