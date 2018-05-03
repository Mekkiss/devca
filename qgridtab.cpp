#include "qgridtab.h"
#include <QTabBar>

QGridTab::QGridTab(QWidget *parent) :
    QTabWidget(parent)
{
  m_Shape = 0;
  m_Colour = Qt::white;
  m_MapN = 1;
  m_cSize = 1;
  m_tDir = 0;
  m_lThick = 1;
  m_radius = 4;
  m_tmpFile = new QTemporaryFile();
}

QGridTab::~QGridTab()
{
  delete m_tmpFile;
}

void QGridTab::newGrid(int w, int h, int b, QColor bg, QColor ln, QColor co, int l)
{
  int bSize = b;
  if (count()==1) m_MapN = 1;
  QPointer<QGridView> gv = new QGridView(m_Shape, m_Colour, m_cSize, m_tDir, m_addChar, m_lThick, m_radius, l, this);
  connect(gv, SIGNAL(circleDrawn(int,int,QColor,int,QString)), this, SLOT(passCircle(int,int,QColor,int,QString)), Qt::AutoConnection);
  connect(gv, SIGNAL(blockDrawn(int,int,QColor,QString)), this, SLOT(passBlock(int,int,QColor,QString)), Qt::AutoConnection);
  connect(gv, SIGNAL(crossDrawn(int,int,QColor)), this, SLOT(passCross(int,int,QColor)), Qt::AutoConnection);
  connect(gv, SIGNAL(triangleDrawn(int,int,QColor,int)), this, SLOT(passTriangle(int,int,QColor,int)), Qt::AutoConnection);
  connect(gv, SIGNAL(itemRemoved(int,int)), this, SLOT(passRemove(int,int)), Qt::AutoConnection);
  connect(gv, SIGNAL(itemRemoved(quint16)), this, SLOT(passRemove(quint16)), Qt::AutoConnection);
  connect(gv, SIGNAL(dragStarted(int,int)), this, SLOT(passStartDrag(int,int)), Qt::AutoConnection);
  connect(gv, SIGNAL(dragFinished(int,int)), this, SLOT(passFinishDrag(int,int)), Qt::AutoConnection);
  connect(gv, SIGNAL(lineDrawn(double,double,double,double,QPen)), this, SLOT(passLine(double,double,double,double,QPen)), Qt::AutoConnection);
  connect(gv, SIGNAL(caItemDrawn(CAGraphicsItem*)), this, SLOT(passCAItem(CAGraphicsItem*)));
  connect(gv, SIGNAL(newBackground(CABackground*)), this, SLOT(passNewBackground(CABackground*)), Qt::AutoConnection);
  connect(gv, SIGNAL(backgroundOffset(int,int)), this, SLOT(passBackgroundOffset(int,int)), Qt::AutoConnection);
  connect(gv, SIGNAL(updateDownload(qint64,qint64)), this, SLOT(passUpdateDownload(qint64,qint64)), Qt::AutoConnection);
  int x = addTab(gv, "Map "+QString::number(++m_MapN));
  emit renameTab(tabText(x), x);
  setCurrentIndex(x);
  setColumnCount(w);
  setRowCount(h);
  setBoxSize(bSize);
  setBackgroundColour(bg);
  setLineColour(ln);
  setLineStyle(l);
  setCoordinateColour(co);
  if (!gv->getGridScene()) gv->drawGrid();
}

void QGridTab::saveToFile(QPointer<QFile> file, int p)
{
  QPointer<QGridScene> scene = QPointer<QGridScene>(getCurrentGridScene(p));
  CAGrid *grid = getCurrentGrid();
  if (!scene) return;

  QTextStream out(file);
  QColor bg = scene->getBackgroundColour();
  QColor ln = grid->lineColour();
  QColor co = grid->coordColour();
  CABackground *cabg = getCurrentBackground();
  QPoint offset = cabg->offset();
  out << "2,"+QString::number(scene->getWidth())+","+QString::number(scene->getHeight())+","+QString::number(scene->getBoxSize())+","+QString::number(bg.red())+","+QString::number(bg.green())+","+QString::number(bg.blue())+","+QString::number(ln.red())+","+QString::number(ln.green())+","+QString::number(ln.blue())+","+QString::number(co.red())+","+QString::number(co.green())+","+QString::number(co.blue())+","+cabg->hash()+","+QString::number(offset.x())+","+QString::number(offset.y())+","+QString::number(grid->type())+"\n";
  if (scene)
  {
    QList<QGraphicsItem*> i = scene->items();
    for (int x = 0; x < i.size(); x++)
    {
      if (i[x]->type() == QGraphicsLineItem::Type || i[x]->type() == QGraphicsSimpleTextItem::Type || i[x]->parentItem() != 0) continue;
      QPoint p(-1, -1);
      int t = 0;
      switch (i[x]->type())
      {
        case QGraphicsEllipseItem::Type:
          if (!t) t = 1;
        case QGraphicsRectItem::Type:
          if (!t) t = 2;
        case QGraphicsPathItem::Type:
          if (!t) t = 3;
          p = scene->mapPoint(i[x]->boundingRect().x(), i[x]->boundingRect().y());
          break;
        case QGraphicsPolygonItem::Type:
          t = 4;
          p = scene->mapPoint(i[x]->pos().x(), i[x]->pos().y());
          break;
        case CALine::Type:
        {
          CALine *it = static_cast<CALine *>(i[x]);
          out << "5,"+QString::number(it->startX(), 'f', 6)+","+QString::number(it->startY(), 'f', 6)+","+QString::number(it->endX(), 'f', 6)+","+QString::number(it->endY(), 'f', 6)+","+QString::number(it->getColor().red())+","+QString::number(it->getColor().green())+","+QString::number(it->getColor().blue())+","+QString::number(it->pen().width())+"\n";
          break;
        }
        case CAGraphicsItem::Type:
      {
          CAGraphicsItem* cai = dynamic_cast<CAGraphicsItem*>(i[x]);
          char *s = cai->transmit();
          out << QString::number(CAGraphicsItem::Type) + "," + s +"\n";
          free(s);
          break;
      }
        case QGraphicsLineItem::Type:
        case QGraphicsSimpleTextItem::Type:
        default:
          break;
      }
      if (t > 0)
      {
        QBrush b = static_cast<QAbstractGraphicsShapeItem*>(i[x])->brush();
        int data = i[x]->data(0).toInt();
        QString letter("");
        QList<QGraphicsItem*> c = i[x]->childItems();
        if (c.size() > 0 && c[0]->type() == QGraphicsTextItem::Type) letter = static_cast<QGraphicsTextItem*>(c[0])->toPlainText();
        out << QString::number(t)+","+QString::number(p.x())+","+QString::number(p.y())+","+QString::number(b.color().red())+","+QString::number(b.color().green())+","+QString::number(b.color().blue())+","+QString::number(data)+((!letter.isEmpty())?","+letter:"")+"\n";
        // type(circle|block|cross|triangle),x-coord,y-coord,red,green,blue,data(circle: size|triangle: facing),letter(circle|block)
      }
    }
  }
}

void QGridTab::loadFromFile(QPointer<QFile> file, int width, int height, int cSize, int p)
{
  QPointer<QGridScene> scene = QPointer<QGridScene>(getCurrentGridScene(p));
  if (!scene) return;

  QSettings settings("ca.ini", QSettings::IniFormat);
  bool ar = settings.value("autoResize", true).toBool();
  QTextStream in(file);
  QStringList s = in.readLine().split(",");
  if (s.size() < 13) return;
  int ver = s[0].toInt();
  if (ver != 1 && ver != 2) return;
  int gWidth = s[1].toInt();
  int gHeight = s[2].toInt();
  int gBoxSize = s[3].toInt();
  if (width) gWidth = width;
  if (height) gHeight = height;
  if (cSize) gBoxSize = cSize;
  scene->setColumnCount(gWidth);
  scene->setRowCount(gWidth);
  scene->setBoxSize(gBoxSize);
  QColor bg = QColor(s[4].toInt(), s[5].toInt(), s[6].toInt());
  QColor ln = QColor(s[7].toInt(), s[8].toInt(), s[9].toInt());
  QColor co = QColor(s[10].toInt(), s[11].toInt(), s[12].toInt());
  QString chx = ((s.size() >= 14)?s[13]:"0");
  QPoint offset = QPoint(((s.size() >= 15)?s[14].toInt():0), ((s.size() >= 16)?s[15].toInt():0));
  int line = ((s.size() >= 17)?s[16].toInt():Qt::DotLine);
  if (gWidth < 1 || gWidth > 52 || gHeight < 1 || gHeight > 52 || gBoxSize < 15 || gBoxSize > 99 || !bg.isValid() || !ln.isValid() || !co.isValid()) return;
  clearCurrentGrid(true);
  setBackgroundColour(bg);
  CAGrid *grid = getGrid(p);
  grid->recalculate(gWidth, gHeight, scene->hMargin(), scene->vMargin(), gBoxSize, ln, line, co);
  grid->draw(scene);
  if (ar) resizeToFit(p);
  emit redrawGrid(currentIndex(), gWidth, gHeight, gBoxSize, bg, ln, co, grid->type());
  if (chx != "0") setBackgroundByHash(chx, p, offset.x(), offset.y());
  if (!getCurrentBgFileName().isEmpty()) emit newBackground(getBackground(p), p);
  while (!in.atEnd())
  {
    int type = 0;
    int cx = -1;
    int cy = -1;
    int red = 0;
    int green = 0;
    int blue = 0;
    int data = 0;
    QString letter("");
    s = in.readLine().split(",");
    if (s.size()<1) continue;
    if (s.size()>=1) type = s[0].toInt();
    if (type == 5 && s.size()>=8)
    {
      int w = 1;
      if (s.size()>=9) w = s[8].toInt();
      scene->drawLine(s[1].toDouble(), s[2].toDouble(), s[3].toDouble(), s[4].toDouble(), QPen(QBrush(QColor(s[5].toInt(), s[6].toInt(), s[7].toInt())), w));
      continue;
    }
    if (s.size()>=2) cx = s[1].toInt();
    if (s.size()>=3) cy = s[2].toInt();
    if (s.size()>=4) red = s[3].toInt();
    if (s.size()>=5) green = s[4].toInt();
    if (s.size()>=6) blue = s[5].toInt();
    if (s.size()>=7) data = s[6].toInt();
    if (s.size()>=8) letter = s[7];
    if (type != CAGraphicsItem::Type && (type < 0 || cx < 0 || cy < 0 || red < 0 || green < 0 || blue < 0)) continue;
    if (type != CAGraphicsItem::Type && (cx >= gWidth || cy >= gHeight)) continue;
    switch (type)
    {
      case 1: // circle
        scene->drawCircle(cx, cy, QColor(red, green, blue), data, letter);
        break;
      case 2: // block
        scene->drawBlock(cx, cy, QColor(red, green, blue), letter);
        break;
      case 3: // cross
        scene->drawCross(cx, cy, QColor(red, green, blue));
        break;
      case 4: // triangle
        scene->drawTriangle(cx, cy, QColor(red, green, blue), data);
        break;
      case CAGraphicsItem::Type:
      {
        CAGraphicsItem *c = CAGraphicsItemCreator::create(cx, &s[2]);
        if (c)
        {
            scene->drawCAItem(c); //Can be null if it failed to be created.
        }
        else
        {
            //TODO: Get angry at user and suggest that they upgrade.
        }
      }
      default:
        break;
    }
  }
}

void QGridTab::removeTab(int index)
{
  if (!m_tmpFile->fileName().isNull() && !m_tmpFile->fileName().isEmpty()) m_tmpFile->remove();
  delete m_tmpFile;
  m_tmpFile = new QTemporaryFile("devca.removedTab.XXXXXX");
  if (m_tmpFile->open()) saveToFile(m_tmpFile, index);
  QTabWidget::removeTab(index);
}

void QGridTab::delGrid(int p)
{
  if (count() > 1)
  {
    if (p < 0)
    {
      int c = count();
      for (int x = 1; x < c; x++)
        removeTab(1);
    }
    else if (p < count()) removeTab(p);
  }
}

void QGridTab::setLThick(int t)
{
  m_lThick = t;
  for (int x = 0; x < count(); x++)
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(x));
    if (v) v->setLThick(t);
  }
}

void QGridTab::setShape(int s)
{
  m_Shape = s;
  for (int x = 0; x < count(); x++)
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(x));
    if (v) v->setShape(s);
  }
}

void QGridTab::setColour(QColor b)
{
  m_Colour = b;
  for (int x = 0; x < count(); x++)
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(x));
    if (v) v->setColour(b);
  }
}

void QGridTab::setCSize(int s)
{
  m_cSize = s;
  for (int x = 0; x < count(); x++)
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(x));
    if (v) v->setCSize(s);
  }
}

void QGridTab::setTDir(int t)
{
  m_tDir = t;
  for (int x = 0; x < count(); x++)
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(x));
    if (v) v->setTDir(t);
  }
}

void QGridTab::setRadius(int r)
{
    m_radius = r;
    for (int x = 0; x < count(); x++)
    {
      QPointer<QGridView> v = static_cast<QGridView*>(widget(x));
      if (v) v->setRadius(r);
    }
}

void QGridTab::setAddChar(QToolButton *on)
{
  m_addChar = on;
  for (int x = 0; x < count(); x++)
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(x));
    if (v) v->setAddChar(on);
  }
}

void QGridTab::clearCurrentGrid(bool total, bool saveBackground, int p)
{
  QPointer<QGridView> v;
  if (p < 0) v = static_cast<QGridView*>(currentWidget());
  else if (p < count()) v = static_cast<QGridView*>(widget(p));
  if (v) v->clearGrid(total, saveBackground);
}

void QGridTab::drawCurrentGrid(int p)
{
  QPointer<QGridView> v;
  if (p < 0) v = static_cast<QGridView*>(currentWidget());
  else if (p < count()) v = static_cast<QGridView*>(widget(p));
  if (v) v->drawGrid();
}

void QGridTab::setBoxSize(int s, int p)
{
  QPointer<QGridView> v;
  if (p < 0) v = static_cast<QGridView*>(currentWidget());
  else if (p < count()) v = static_cast<QGridView*>(widget(p));
  if (v) v->setBoxSize(s);
}

void QGridTab::setColumnCount(int c, int p)
{
  QPointer<QGridView> v;
  if (p < 0) v = static_cast<QGridView*>(currentWidget());
  else if (p < count()) v = static_cast<QGridView*>(widget(p));
  if (v) v->setColumnCount(c);
}

void QGridTab::setRowCount(int c, int p)
{
  QPointer<QGridView> v;
  if (p < 0) v = static_cast<QGridView*>(currentWidget());
  else if (p < count()) v = static_cast<QGridView*>(widget(p));
  if (v) v->setRowCount(c);
}

void QGridTab::setBackgroundColour(QColor c, int p)
{
  QPointer<QGridView> v;
  if (p < 0) v = static_cast<QGridView*>(currentWidget());
  else if (p < count()) v = static_cast<QGridView*>(widget(p));
  if (v) v->setBackgroundColour(c);
}

void QGridTab::setLineStyle(int l, int p)
{
  QPointer<QGridView> v;
  if (p < 0) v = static_cast<QGridView*>(currentWidget());
  else if (p < count()) v = static_cast<QGridView*>(widget(p));
  if (v) v->setLineStyle(l);
}

void QGridTab::setLineColour(QColor c, int p)
{
  QPointer<QGridView> v;
  if (p < 0) v = static_cast<QGridView*>(currentWidget());
  else if (p < count()) v = static_cast<QGridView*>(widget(p));
  if (v) v->setLineColour(c);
}

void QGridTab::setCoordinateColour(QColor c, int p)
{
  QPointer<QGridView> v;
  if (p < 0) v = static_cast<QGridView*>(currentWidget());
  else if (p < count()) v = static_cast<QGridView*>(widget(p));
  if (v) v->setCoordinateColour(c);
}

int QGridTab::getColumnCount(int p)
{
  if (p < count())
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) return v->getColumnCount();
  }
  return 20;
}

int QGridTab::getRowCount(int p)
{
  if (p < count())
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) return v->getRowCount();
  }
  return 20;
}

int QGridTab::getRadius(int p)
{
  if (p < count())
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) return v->getRadius();
  }
  return 4;
}

int QGridTab::getBoxSize(int p)
{
  if (p < count())
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) return v->getBoxSize();
  }
  return 20;
}

QString QGridTab::getBgFileName(int p)
{
  if (p < count())
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) return v->getBgFileName();
  }
  return QString("");
}

QColor QGridTab::getBackgroundColour(int p)
{
  if (p < count())
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) return v->getBackgroundColour();
  }
  return Qt::black;
}

int QGridTab::getLineStyle(int p)
{
  QPointer<QGridView> v;
  if (p < 0) v = static_cast<QGridView*>(currentWidget());
  else if (p < count()) v = static_cast<QGridView*>(widget(p));
  if (v) return v->getLineStyle();
  return Qt::DotLine;
}

QColor QGridTab::getLineColour(int p)
{
  if (p < count())
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) return v->getLineColour();
  }
  return Qt::white;
}

QColor QGridTab::getCoordinateColour(int p)
{
  if (p < count())
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) return v->getCoordinateColour();
  }
  return Qt::yellow;
}

int QGridTab::getCurrentColumnCount()
{
  QPointer<QGridView> v = static_cast<QGridView*>(currentWidget());
  return v->getColumnCount();
}

int QGridTab::getCurrentRowCount()
{
  QPointer<QGridView> v = static_cast<QGridView*>(currentWidget());
  return v->getRowCount();
}

int QGridTab::getCurrentBoxSize()
{
  QPointer<QGridView> v = static_cast<QGridView*>(currentWidget());
  return v->getBoxSize();
}

QPair<int,int> QGridTab::getCurrentMargins()
{
  return getMargins(-1);
}

QPair<int,int> QGridTab::getMargins(int p)
{
  QPointer<QGridView> v = NULL;
  if (p < 0) v = static_cast<QGridView*>(currentWidget());
  else if (p < count()) v = static_cast<QGridView*>(widget(p));
  if (v)
  {
    return QPair<int,int>(v->hMargin(), v->vMargin());
  }
  return QPair<int,int>(0, 0);
}

QColor QGridTab::getCurrentBackgroundColour()
{
  QPointer<QGridView> v = static_cast<QGridView*>(currentWidget());
  return v->getBackgroundColour();
}

QColor QGridTab::getCurrentLineColour()
{
  QPointer<QGridView> v = static_cast<QGridView*>(currentWidget());
  return v->getLineColour();
}

QColor QGridTab::getCurrentCoordinateColour()
{
  QPointer<QGridView> v = static_cast<QGridView*>(currentWidget());
  return v->getCoordinateColour();
}

QPoint QGridTab::getBackgroundOffset(int p)
{
  QPointer<QGridView> v = NULL;
  if (p < 0) v = static_cast<QGridView*>(currentWidget());
  else if (p < count()) v = static_cast<QGridView*>(widget(p));
  if (v)
  {
    QGraphicsItem *bg = v->getBackgroundImage();
    if (bg) return QPoint(bg->x()-v->hMargin(), bg->y()-v->vMargin());
  }
  return QPoint(0, 0);
}

void QGridTab::backgroundOffset(int p, int x, int y)
{
  QPointer<QGridView> v = NULL;
  if (p < 0) v = static_cast<QGridView*>(currentWidget());
  else if (p < count()) v = static_cast<QGridView*>(widget(p));
  if (v) v->setBackgroundOffset(x, y);
}

CABackground *QGridTab::getCurrentBackground()
{
  return getBackground(-1);
}

CABackground *QGridTab::getBackground(int p)
{
  if (p < count())
  {
    if (p < 0)
    {
      QPointer<QGridView> v = static_cast<QGridView*>(currentWidget());
      return v->getBackground();
    }
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) return v->getBackground();
  }
  return NULL;
}

CAGrid *QGridTab::getCurrentGrid()
{
  return getGrid(-1);
}

CAGrid *QGridTab::getGrid(int p)
{
  if (p < count())
  {
    if (p < 0)
    {
      QPointer<QGridView> v = static_cast<QGridView*>(currentWidget());
      return v->getGrid();
    }
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) return v->getGrid();
  }
  return NULL;
}

QGraphicsItem *QGridTab::getCurrentBackgroundImage()
{
  return getBackgroundImage(-1);
}

QGraphicsItem *QGridTab::getBackgroundImage(int p)
{
  if (p < count())
  {
    if (p < 0)
    {
      QPointer<QGridView> v = static_cast<QGridView*>(currentWidget());
      return v->getBackgroundImage();
    }
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) return v->getBackgroundImage();
  }
  return NULL;
}

QPointer<QGridScene> QGridTab::getCurrentGridScene(int p)
{
  if (p < count())
  {
    if (p < 0)
    {
      QPointer<QGridView> v = static_cast<QGridView*>(currentWidget());
      return v->getGridScene();
    }
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) return v->getGridScene();
  }
  return NULL;
}

void QGridTab::currentResizeToFit()
{
  resizeToFit(-1);
}

void QGridTab::resizeToFit(int p)
{
  if (p < count())
  {
    if (p < 0)
    {
      QPointer<QGridView> v = static_cast<QGridView*>(currentWidget());
      return v->resizeToFit();
    }
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) return v->resizeToFit();
  }
}

void QGridTab::drawCircle(int p, int x, int y, QColor c, int size, QString letter)
{
  if (p < count())
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) v->drawCircle(x, y, c, size, letter);
  }
}

void QGridTab::drawBlock(int p, int x, int y, QColor c, QString letter)
{
  if (p < count())
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) v->drawBlock(x, y, c, letter);
  }
}

void QGridTab::drawCross(int p, int x, int y, QColor c)
{
  if (p < count())
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) v->drawCross(x, y, c);
  }
}

void QGridTab::drawTriangle(int p, int x, int y, QColor c, int facing)
{
  if (p < count())
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) v->drawTriangle(x, y, c, facing);
  }
}

void QGridTab::drawLine(int p, double x1, double y1, double x2, double y2, QPen pen)
{
  if (p < count())
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) v->drawLine(x1, y1, x2, y2, pen);
  }
}

void QGridTab::drawCAItem(int p, CAGraphicsItem* i)
{
    if ( p < count())
    {
        QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
        if (v) v->drawCAItem(i);
    }
}

void QGridTab::removeItem(int p, int x, int y)
{
  if (p < count())
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) v->removeItem(x, y);
  }
}

void QGridTab::removeItem(int p, quint16 check)
{
  if (p < count())
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) v->removeItem(check);
  }
}

void QGridTab::startDrag(int p, int x, int y)
{
  if (p < count())
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) v->startDrag(x, y);
  }
}

void QGridTab::finishDrag(int p, int x, int y)
{
  if (p < count())
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(p));
    if (v) v->finishDrag(x, y);
  }
}

QString QGridTab::getCurrentBgFileName()
{
  QPointer<QGridView> v = static_cast<QGridView*>(currentWidget());
  if (v) return v->getBgFileName();
  return QString("");
}

void QGridTab::setCurrentBackgroundByFile(QString fileName, int x, int y)
{
  setBackgroundByFile(fileName, -1, x, y);
}

void QGridTab::setCurrentBackgroundByURI(QString uri, QString hash, int x, int y)
{
  setBackgroundByURI(uri, -1, hash, x, y);
}

void QGridTab::setBackgroundByURI(QString uri, int p, QString hash, int x, int y)
{
  QPointer<QGridView> v;
  if (p < 0) v = static_cast<QGridView*>(currentWidget());
  else if (p < count()) v = static_cast<QGridView*>(widget(p));
  if (v) v->setBackgroundByURI(uri, hash, x, y);
}

void QGridTab::setBackgroundByFile(QString fileName, int p, int x, int y)
{
  QPointer<QGridView> v;
  if (p < 0) v = static_cast<QGridView*>(currentWidget());
  else if (p < count()) v = static_cast<QGridView*>(widget(p));
  if (v) v->setBackgroundByFile(fileName, x, y);
}

bool QGridTab::setBackgroundByHash(QString chx, int p, int x, int y)
{
  QString fileName = findFileByHash(chx, "images/");
  if (!fileName.isEmpty())
  {
    setBackgroundByFile(fileName, p, x, y);
    return true;
  }
  return false;
}

void QGridTab::setCurrentBackgroundOffset(int x, int y)
{
  setBackgroundOffset(-1, x, y);
}

void QGridTab::setBackgroundOffset(int p, int x, int y)
{
  QPointer<QGridView> v;
  if (p < 0) v = static_cast<QGridView*>(currentWidget());
  else if (p < count()) v = static_cast<QGridView*>(widget(p));
  if (v) v->setBackgroundOffset(x, y);
}

void QGridTab::passCircle(int x, int y, QColor c, int size, QString letter)
{
  emit circleDrawn(currentIndex(), x, y, c, size, letter);
}

void QGridTab::passBlock(int x, int y, QColor c, QString letter)
{
  emit blockDrawn(currentIndex(), x, y, c, letter);
}

void QGridTab::passCross(int x, int y, QColor c)
{
  emit crossDrawn(currentIndex(), x, y, c);
}

void QGridTab::passTriangle(int x, int y, QColor c, int facing)
{
  emit triangleDrawn(currentIndex(), x, y, c, facing);
}

void QGridTab::passLine(double x1, double y1, double x2, double y2, QPen pen)
{
  emit lineDrawn(currentIndex(), x1, y1, x2, y2, pen);
}

void QGridTab::passCAItem(CAGraphicsItem *i)
{
    //i->transmit();
  emit caItemDrawn(currentIndex(), i);
}

void QGridTab::passNewBackground(CABackground *b)
{
  emit newBackground(b, currentIndex());
}

void QGridTab::passBackgroundOffset(int x, int y)
{
  emit newBackgroundOffset(currentIndex(), x, y);
}

void QGridTab::passUpdateDownload(qint64 r, qint64 t)
{
  emit updateDownload(r, t);
}

void QGridTab::passRemove(int x, int y)
{
  emit itemRemoved(currentIndex(), x, y);
}

void QGridTab::passRemove(quint16 check)
{
  emit itemRemoved(currentIndex(), check);
}

void QGridTab::passStartDrag(int x, int y)
{
  emit dragStarted(currentIndex(), x, y);
}

void QGridTab::passFinishDrag(int x, int y)
{
  emit dragFinished(currentIndex(), x, y);
}

void QGridTab::setAllClient(bool client)
{
  for (int x = 0; x < count(); x++)
  {
    QPointer<QGridView> v = static_cast<QGridView*>(widget(x));
    if (v) v->setClient(client);
  }
}

void QGridTab::mouseReleaseEvent (QMouseEvent *event)
{
  int tab = tabBar()->tabAt(event->pos());
  if (tab >= 0)
  {
    if (event->button() == Qt::MiddleButton) delGrid(tab);
    else if (event->button() == Qt::RightButton)
    {
      QString txt = QInputDialog::getText(this, tr("Rename Tab"), tr("Input new name:"));
      if (!txt.isNull() && !txt.isEmpty())
      {
        setTabText(tab, txt);
        emit renameTab(tabText(tab), tab);
      }
    }
  }
  else if (event->button() == Qt::RightButton)
  {
    if (m_tmpFile && !m_tmpFile->fileName().isNull() && !m_tmpFile->fileName().isEmpty() && QFile::exists(m_tmpFile->fileName()))
    {
      newGrid();
      emit tabAdded();
      m_tmpFile->seek(0);
      loadFromFile(m_tmpFile);
      m_tmpFile->remove();
      delete m_tmpFile;
      m_tmpFile = new QTemporaryFile();
    }
  }
}
