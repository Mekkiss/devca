#ifndef QGRIDTAB_H
#define QGRIDTAB_H

#include <QTabWidget>
#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsScene>
#include <QPointer>
#include <QMouseEvent>
#include <QTemporaryFile>
#include <QtNetwork>
#include <QToolButton>
#include "qgridview.h"
#include "cagraphicsitem.h"
#include "cagraphicsitemcreator.h"

class QGridScene;
class CABackground;
class CAGrid;

class QGridTab : public QTabWidget
{
  Q_OBJECT
public:
    QGridTab(QWidget *parent = 0);
    ~QGridTab();
    QTabBar *tabBar() const { return QTabWidget::tabBar(); }
    void newGrid(int w = 20, int h = 20, int b = 20, QColor bg = Qt::black, QColor ln = Qt::white, QColor co = Qt::yellow, int l = Qt::DotLine);
    void delGrid(int p = -1);
    void setShape(int s);
    void setColour(QColor b);
    void setCSize(int s);
    void setTDir(int t);
    void setAddChar(QToolButton *on);
    void clearCurrentGrid(bool total = false, bool saveBackground = false, int p = -1);
    void drawCurrentGrid(int p = -1);
    void setColumnCount(int c, int p = -1);
    void setRowCount(int c, int p = -1);
    void setBoxSize(int s, int p = -1);
    void setBackgroundColour(QColor c, int p = -1);
    void setLineColour(QColor c, int p = -1);
    void setLineStyle(int l, int p = -1);
    void setCoordinateColour(QColor c, int p = -1);
    int getCurrentColumnCount();
    int getCurrentRowCount();
    int getCurrentBoxSize();
    QPair<int,int> getCurrentMargins();
    QPair<int,int> getMargins(int p);
    int getShape() { return m_Shape; }
    int getColumnCount(int p);
    int getRowCount(int p);
    int getBoxSize(int p);
    int getRadius(int p);
    void setLThick(int t);
    int getLThick() { return m_lThick; }
    QString getBgFileName(int p);
    QColor getBackgroundColour(int p);
    int getLineStyle(int p);
    QColor getLineColour(int p);
    QColor getCoordinateColour(int p);
    QPointer<QGridScene> getCurrentGridScene(int p = -1);
    QColor getCurrentBackgroundColour();
    QColor getCurrentLineColour();
    QColor getCurrentCoordinateColour();
    QString getCurrentBgFileName();
    void setCurrentBackgroundByURI(QString uri, QString hash = QString("0"), int x = 0, int y = 0);
    void setCurrentBackgroundByFile(QString fileName, int x = 0, int y = 0);
    void setBackgroundByURI(QString uri, int p, QString hash = QString("0"), int x = 0, int y = 0);
    void setBackgroundByFile(QString fileName, int p, int x = 0, int y = 0);
    bool setBackgroundByHash(QString chx, int p, int x = 0, int y = 0);
    void setCurrentBackgroundOffset(int x, int y);
    void setBackgroundOffset(int p, int x, int y);
    void currentResizeToFit();
    void resizeToFit(int p = -1);
    void drawCircle(int p, int x, int y, QColor c, int size, QString letter);
    void drawBlock(int p, int x, int y, QColor c, QString letter);
    void drawCross(int p, int x, int y, QColor c);
    void drawTriangle(int p, int x, int y, QColor c, int facing);
    void drawLine(int p, double x1, double y1, double x2, double y2, QPen pen);
    void drawCAItem(int p, CAGraphicsItem* i);
    void removeItem(int p, int x, int y);
    void removeItem(int p, quint16 check);
    void startDrag(int p, int x, int y);
    void finishDrag(int p, int x, int y);
    void setAllClient(bool client);
    void setRadius(int r);
    void mouseReleaseEvent (QMouseEvent *event);
    void removeTab(int index);
    void saveToFile(QPointer<QFile> file, int p = -1);
    void loadFromFile(QPointer<QFile> file, int width = 0, int height = 0, int cSize = 0, int p = -1);
    CABackground *getCurrentBackground();
    CABackground *getBackground(int p);
    CAGrid *getCurrentGrid();
    CAGrid *getGrid(int p);
    QGraphicsItem *getCurrentBackgroundImage();
    QGraphicsItem *getBackgroundImage(int p);
    QPoint getBackgroundOffset(int p = -1);
    void backgroundOffset(int p, int x, int y);

private:
    int m_Shape;
    int m_cSize;
    int m_tDir;
    QPointer<QToolButton> m_addChar;
    QColor m_Colour;
    int m_MapN;
    int m_lThick;
		int m_radius;
    QTemporaryFile *m_tmpFile;

signals:
    void circleDrawn(int p, int x, int y, QColor c, int size, QString letter);
    void blockDrawn(int p, int x, int y, QColor c, QString letter);
    void crossDrawn(int p, int x, int y, QColor c);
    void triangleDrawn(int p, int x, int y, QColor c, int facing);
    void lineDrawn(int p, double x1, double y1, double x2, double y2, QPen pen);
    void itemRemoved(int p, int x, int y);
    void itemRemoved(int p, quint16 check);
    void dragStarted(int p, int x, int y);
    void dragFinished(int p, int x, int y);
    void caItemDrawn(int p, CAGraphicsItem * i);
    void changeBackground(QString fileName, int p = -1, QTcpSocket *sock = NULL);
    void redrawGrid(int p = -1, int w = 20, int h = 20, int b = 20, QColor bg = Qt::black, QColor ln = Qt::white, QColor co = Qt::yellow, int ls = Qt::DotLine, QTcpSocket *sock = NULL);
    void tabAdded(int w = 20, int h = 20, int b = 20, QColor bg = Qt::black, QColor ln = Qt::white, QColor co = Qt::yellow, QTcpSocket *sock = NULL);
    void tabRemoved(int p);
    void renameTab(QString txt, int p);
    void newBackground(CABackground *b, int p);
    void newBackgroundOffset(int p, int x, int y);
    void updateDownload(qint64 r, qint64 t);

public slots:
    void passCircle(int x, int y, QColor c, int size, QString letter);
    void passBlock(int x, int y, QColor c, QString letter);
    void passCross(int x, int y, QColor c);
    void passTriangle(int x, int y, QColor c, int facing);
    void passLine(double x1, double y1, double x2, double y2, QPen pen);
    void passRemove(int x, int y);
    void passRemove(quint16 check);
    void passStartDrag(int x, int y);
    void passFinishDrag(int x, int y);
    void passCAItem(CAGraphicsItem *i);
    void passNewBackground(CABackground *b);
    void passBackgroundOffset(int x, int y);
    void passUpdateDownload(qint64 r, qint64 t);
};

#endif // QGRIDTAB_H
