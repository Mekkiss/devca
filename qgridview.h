#ifndef QGRIDVIEW_H
#define QGRIDVIEW_H

#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsScene>
#include <QGraphicsTextItem>
#include <QFrame>
#include "qgridscene.h"
#include "qgridtab.h"
#include "cagraphicsitem.h"
#include "caline.h"

class QGridScene;
class QGridTab;
class CALine;
class CABackground;
class CAGrid;

class QGridView : public QGraphicsView
{
  Q_OBJECT

private:
    void setDefaults(int s, QColor b, int cSize, int tDir, QToolButton *addChar, int thick, int radius, int l);
    bool eventFilter(QObject *object, QEvent *event);
    bool m_Init;
    QPointer<QGridScene> m_Scene;
    int m_tWidth;
    int m_tHeight;
    int m_Shape;
    int m_cSize;
    int m_tDir;
    int m_lThick;
    QFont m_coFont;
    QPointer<QToolButton> m_addChar;
    QColor m_Colour;
    QColor m_bgColour;
    QColor m_lnColour;
    QColor m_coColour;
    bool m_Client;
    CABackground *m_Background;
    QString bgFileName;
		int m_radius;
    int m_lStyle;
    QPointer<QNetworkAccessManager> m_Manager;
    CAGrid *m_Grid;

public:
    QGridView(QGraphicsScene *scene, int s = 0, QColor b = Qt::white, int cSize = 1, int tDir = 0, QToolButton *addChar = NULL, int thick = 1, int radius = 4, int l = Qt::DotLine, QWidget *parent = 0);
    QGridView(int s = 0, QColor b = Qt::white, int cSize = 1, int tDir = 0, QToolButton *addChar = NULL, int thick = 1, int radius = 4, int l = Qt::DotLine, QWidget *parent = 0);
    ~QGridView();
    void setShape (int s);
    void setColour (QColor b);
    void setCSize(int s);
    void setTDir(int t);
    void setLThick(int t);
    int getLThick() { return m_lThick; }
    void setAddChar(QToolButton *on);
    void clearGrid(bool total = false, bool saveBackground = false);
    void drawGrid();
    void setColumnCount(int c);
    void setRowCount(int c);
    void setBoxSize(int s);
    void setBackgroundColour(QColor c);
    void setLineStyle(int l);
    void setLineColour(QColor c);
    void setCoordinateColour(QColor c);
    int getColumnCount();
    int getRowCount();
    int getBoxSize();
    int getRadius() {return m_radius;}
    QColor getBackgroundColour() {return m_bgColour;}
    int getLineStyle();
    QColor getLineColour();
    QColor getCoordinateColour();
    QPointer<QGridScene> getGridScene();
    void resizeToFit();
    void drawCircle(int x, int y, QColor c, int size, QString letter);
    void drawBlock(int x, int y, QColor c, QString letter);
    void drawCross(int x, int y, QColor c);
    void drawTriangle(int x, int y, QColor c, int facing);
    void drawLine(double x1, double y1, double x2, double y2, QPen pen);
		void drawCAItem(CAGraphicsItem *i);
    void removeItem(int x, int y);
    void removeItem(quint16 check);
    void startDrag(int x, int y);
    void finishDrag(int x, int y);
    void setClient(bool client);
    void setBackgroundByFile(QString fileName, int x = 0, int y = 0);
    void setBackgroundByURI(QString uri, QString hash = QString("0"), int x = 0, int y = 0);
    void setBackgroundOffset(int x, int y);
    QString getBgFileName() {return bgFileName;}
    QGraphicsItem *getBackgroundImage();
    void clearBg();
    QPixmap checkForPixmap(QString fileName);
		void setRadius(int r);
    int hMargin();
    int vMargin();
    CABackground *getBackground() { return m_Background; }
    CAGrid *getGrid() { return m_Grid; }

public slots:
    void passCircle(int x, int y, QColor c, int size, QString letter);
    void passBlock(int x, int y, QColor c, QString letter);
    void passCross(int x, int y, QColor c);
    void passTriangle(int x, int y, QColor c, int facing);
    void passRemove(int x, int y);
    void passRemove(quint16 check);
    void passStartDrag(int x, int y);
    void passFinishDrag(int x, int y);
    void passLine(double x1, double y1, double x2, double y2, QPen pen);
    void passLine(CALine *l);
		void passCAItem(CAGraphicsItem * i);
    void totalClear();
    void zoom(int direction);
private slots:
    void downloadFinished(QNetworkReply *reply);
    void downloadProgress(qint64 r, qint64 t);

signals:
    void circleDrawn(int x, int y, QColor c, int size, QString letter);
    void blockDrawn(int x, int y, QColor c, QString letter);
    void crossDrawn(int x, int y, QColor c);
    void triangleDrawn(int x, int y, QColor c, int facing);
    void itemRemoved(int x, int y);
    void itemRemoved(quint16 check);
    void dragStarted(int x, int y);
    void dragFinished(int x, int y);
    void lineDrawn(double x1, double y1, double x2, double y2, QPen pen);
    void lineDrawn(CALine *l);
		void caItemDrawn(CAGraphicsItem *i);
    void newBackground(CABackground *b);
    void backgroundOffset(int x, int y);
    void updateDownload(qint64 r, qint64 t);
};

class CABackground
{
public:
  CABackground(QString location, int type = CABackground::TypeLocal, QString hash = "0", QGraphicsPixmapItem *img = NULL, QPoint o = QPoint(0,0));
  ~CABackground();
  enum { TypeLocal = 1, TypeRemote = 2 };
  QString location() { return m_Location; }
  QString hash() { return m_Hash; }
  int type() { return m_Type; }
  QGraphicsPixmapItem* image() { return m_Image; }
  QPoint offset() { return m_Offset; }
  void setHash(QString hash);
  void setImage(QGraphicsPixmapItem *i);
  void setOffset(QPoint o);

private:
  QString m_Location;
  int m_Type;
  QString m_Hash;
  QGraphicsPixmapItem *m_Image;
  QPoint m_Offset;
};

class CAGrid
{
public:
  CAGrid(int c, int r, int hMargin, int vMargin, int s = 20, QColor colour = Qt::white, int type = Qt::DotLine, QFont cf = QFont("Courier New", 8), QColor cc = Qt::yellow);
  ~CAGrid();
  void calculate();
  void recalculate(int c, int r, int hMargin, int vMargin, int s, QColor colour = Qt::white, int type = Qt::DotLine, QColor cc = Qt::yellow, QFont cf = QFont("Courier New", 8));
  void draw(QGridScene *scene);
  void setType(int type);
  void setColour(QColor c);
  void setCoordinateColour(QColor c);
  void setWidth(int c) { m_Columns = c; }
  void setHeight(int r) { m_Rows = r; }
  void setSize(int s) { m_Size = s; }
  void modifyLines();
  void modifyCoords();
  int cellSize() { return m_Size; }
  int width() { return m_Columns; }
  int height() { return m_Rows; }
  int type() { return m_Type; }
  QColor lineColour() { return m_Colour; }
  QColor coordColour() { return m_coColour; }
  void resize(int size = 0);
  int hMargin() { return m_hMargin; }
  int vMargin() { return m_vMargin; }

private:
  enum { Min_Size = 15, Max_Size = 99 };
  int m_Columns;
  int m_Rows;
  int m_hMargin;
  int m_vMargin;
  int m_Size;
  QColor m_Colour;
  QColor m_coColour;
  QFont m_coFont;
  int m_Type;
  QList<QGraphicsLineItem*> m_Lines;
  QList<QGraphicsSimpleTextItem*> m_TopCoords;
  QList<QGraphicsSimpleTextItem*> m_BottomCoords;
  QList<QGraphicsSimpleTextItem*> m_LeftCoords;
  QList<QGraphicsSimpleTextItem*> m_RightCoords;
};

int epoch();
QString epoch_s();
QString findFileByHash(QString hash, QString dirName);
bool filesIdentical(QString file1, QString file2);
QString fileChecksum(QString fileName);

#endif // QGRIDVIEW_H
