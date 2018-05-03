#include "qgridscene.h"
#include "cablasttemplate.h"
#include "camask.h"
#include "caconetemplate.h"
#include "cagraphicsitem.h"

#define MIN(x, y) ((x < y) ? x : y )
#define MAX(x, y) ((x > y) ? x : y )

QGridScene::QGridScene(qreal x, qreal y, qreal width, qreal height, int gWidth, int gHeight, int bSize, int s, QColor b, int cSize, int tDir, QToolButton *addChar, int thick, int ls, bool client, QObject *parent) :
    QGraphicsScene(x, y, width, height, parent)
{
  QSettings settings("ca.ini", QSettings::IniFormat);
  m_gWidth = gWidth;
  m_gHeight = gHeight;
  m_bSize = bSize;
  m_Shape = s;
  m_Colour = b;
  m_cSize = cSize;
  m_tDir = tDir;
  m_hMargin = settings.value("hMargin", 20).toInt();
  m_vMargin = settings.value("vMargin", 15).toInt();
  m_addChar = addChar;
  m_iDrag = NULL;
  m_bgColour = Qt::black;
  m_lnColour = Qt::white;
  m_coColour = Qt::yellow;
  m_lThick = thick;
  m_lStyle = ls;
  m_Client = client;
  // This was causing total CPU use on a processor, so disabled to see if it is needed/until there is a workaround
  //connect(this, SIGNAL(changed(QList<QRectF>)), this, SLOT(update()), Qt::AutoConnection);
}

QGridScene::~QGridScene()
{
}

void QGridScene::handleMask(QPoint p)
{
  static bool first = true;
  static QPoint p1;
  if (first)
  {
    p1 = p;
    first = false;
    return;
  }
  QPoint p2 = p;
  first = true;

  CAMask *c;
  QSize size(abs(p1.x() - p2.x()), abs(p1.y() - p2.y()));
  QPoint topleft(MIN(p1.x(), p2.x()), MIN(p2.y(), p1.y()));
  if( p1.x() == p2.x() || p1.y() == p2.y()) return;
  c = new CAMask(new QRect(topleft, size), m_Colour);

  drawCAItem(c);
}

bool QGridScene::eventFilter(QObject *watched, QEvent *event)
{
  static QPointF lnStartP = QPointF(-1, -1);
  static QPointF mskStartP = QPointF(-1, -1);
  static CALine *lnDrag = NULL;
  static CAMask *mskDrag = NULL;
  if (event->type() == QEvent::GraphicsSceneWheel)  //allow zooming by clients
  {
      QGraphicsSceneWheelEvent*e = static_cast<QGraphicsSceneWheelEvent*>(event);
      if( e->delta() > 0)
      {
          emit(zoom(1));
      } else
      {
          emit (zoom(-1));
      }
      e->accept();
  }

  if (!m_Client && (event->type() == QEvent::GraphicsSceneMouseMove && mskStartP != QPointF(-1,-1)))
  {
    QGraphicsSceneMouseEvent *e = static_cast<QGraphicsSceneMouseEvent*>(event);
    QPoint coords = mapPoint(e->scenePos().x(), e->scenePos().y());
    //QPointF coordsf = dmapPoint(e->scenePos().x(), e->scenePos().y());
    QPen pen = QPen(m_Colour);
    pen.setWidth(m_lThick);
    QPoint start = coord2pixel(mskStartP.x(), mskStartP.y());
    QPoint end = coord2pixel(coords.x(), coords.y());
    if (end.x() < 0 || end.y() < 0) return false;
    if (mskDrag != NULL)
    {
      removeItem(mskDrag);
      delete mskDrag;
    }
    QRect r = QRect(QPoint(start.x()+1, start.y()+1), QPoint(end.x()+m_bSize, end.y()+m_bSize));
    if (r.width() >= 0) r.setWidth(MAX(m_bSize, r.width()));
    else
    {
      r.setX(r.x()+m_bSize-1);
      r.setWidth(MIN(m_bSize*-1, r.width()));
      r.setWidth(r.width()-m_bSize+1);
    }
    if (r.height() >= 0) r.setHeight(MAX(m_bSize, r.height()));
    else
    {
      r.setY(r.y()+m_bSize-1);
      r.setHeight(MIN(m_bSize*-1, r.height()));
      r.setHeight(r.height()-m_bSize+1);
    }
    r.adjust(m_hMargin*-1, m_vMargin*-1, m_hMargin*-1, m_vMargin*-1);
    mskDrag = new CAMask(&r, m_Colour);
    drawCAItem(mskDrag);
    return false;
  }
  if (!m_Client && (event->type() == QEvent::GraphicsSceneMouseRelease && mskStartP != QPointF(-1, -1)))
  {
    emit caItemDrawn(mskDrag);
    mskStartP = QPointF(-1,-1);
    return false;
  }
  if (!m_Client && (event->type() == QEvent::GraphicsSceneMouseMove && lnStartP != QPointF(-1,-1)))
  {
    QGraphicsSceneMouseEvent *e = static_cast<QGraphicsSceneMouseEvent*>(event);
    QPoint coords = mapCorner(e->scenePos().x(), e->scenePos().y());
    QPointF coordsf = dmapPoint(e->scenePos().x(), e->scenePos().y());
    QPen pen = QPen(m_Colour);
    pen.setWidth(m_lThick);
    QPoint start = coord2pixel(lnStartP.x(), lnStartP.y());
    QPoint end = coord2pixel(coordsf.x(), coordsf.y());
    if (m_Shape == 5) end = coord2pixel(coords.x(), coords.y());
    if (end.x() < 0 || end.y() < 0) return false;
    if (lnDrag != NULL) removeItem(lnDrag);
		delete lnDrag;
    lnDrag = new CALine(start, end, pen, start.x()-m_hMargin, start.y()-m_vMargin, end.x()-m_hMargin, end.y()-m_vMargin);
    addItem(lnDrag);
    return false;
  }
  if (!m_Client && (event->type() == QEvent::GraphicsSceneMouseRelease && lnStartP != QPointF(-1, -1)))
  {
    emit lineDrawn(lnDrag);
    lnStartP = QPointF(-1,-1);
    return false;
  }
  if (!m_Client && (event->type() == QEvent::GraphicsSceneMouseMove && (m_Shape == 4 || m_Shape == 5)))
  {
    QGraphicsSceneMouseEvent *e = static_cast<QGraphicsSceneMouseEvent*>(event);
    if (e->buttons() == Qt::RightButton)
    {
      QList<QGraphicsItem *>i = items(e->scenePos().x()-1, e->scenePos().y()-1, 2, 2, Qt::IntersectsItemShape);
      for (int x = 0; x < i.size(); x++)
        if (i[x]->type() == CALine::Type)
        {
          CALine *c = static_cast<CALine*>(i[x]);
          undrawItem(c->check());
        }
      return false;
    }
  }
  if (!m_Client && (event->type() == QEvent::GraphicsSceneMouseRelease && m_iDrag))
  {
    QGraphicsSceneMouseEvent *e = static_cast<QGraphicsSceneMouseEvent*>(event);
    if (e->button() == Qt::LeftButton)
    {
      QPoint coords = mapPoint(e->scenePos().x(), e->scenePos().y());
      QList<QGraphicsItem *>ie = items(coords.x()*m_bSize+coords.x()+m_hMargin+1, coords.y()*m_bSize+coords.y()+m_vMargin+1, ((m_iDrag)?m_iDrag->boundingRect().width():(m_bSize*m_cSize)+m_cSize-1), ((m_iDrag)?m_iDrag->boundingRect().height():(m_bSize*m_cSize)+m_cSize-1), Qt::IntersectsItemShape);
      if (ie.size() < 1 || !ie.contains(m_iDrag))
        finishDrag(coords.x(), coords.y());
      return false;
    }
  }
  if (event->type() == QEvent::GraphicsSceneMousePress)
  {
    QGraphicsSceneMouseEvent *e = static_cast<QGraphicsSceneMouseEvent*>(event);
    QPoint coords = mapPoint(e->buttonDownScenePos(e->button()).x(), e->buttonDownScenePos(e->button()).y());
    QPointF coordsf = dmapPoint(e->buttonDownScenePos(e->button()).x(), e->buttonDownScenePos(e->button()).y());
    QList<QGraphicsItem *>i = items(coords.x()*m_bSize+coords.x()+m_hMargin+1, coords.y()*m_bSize+coords.y()+m_vMargin+1, m_bSize-1, m_bSize-1, Qt::IntersectsItemShape);
    QList<QGraphicsItem *>ie =items(coords.x()*m_bSize+coords.x()+m_hMargin+1, coords.y()*m_bSize+coords.y()+m_vMargin+1, ((m_iDrag)?m_iDrag->boundingRect().width():(m_bSize*m_cSize)+m_cSize-1), ((m_iDrag)?m_iDrag->boundingRect().height():(m_bSize*m_cSize)+m_cSize-1), Qt::IntersectsItemShape);
    if (coords.x() >= 0 && coords.y() >= 0)
    {
      if ((i.size() > 0 && i[0]->type()) && ((m_Shape != 6 && m_Shape != 7 && m_Shape != 8) || e->button() == Qt::RightButton))
      {
        QGraphicsItem *si = i[0];
        if (i[0]->parentItem() != 0) si = i[0]->parentItem(); // Ignore text - choose parent item
        if (e->button() == Qt::RightButton)
        {
          if (m_Client && si->type() != CAGraphicsItem::Type) return false;
          else if (m_Client)
          {
            CAGraphicsItem *gi = dynamic_cast<CAGraphicsItem*>(si);
            if (gi->subtype() == CAConeTemplate::SubType || gi->subtype() == CABlastTemplate::SubType) undrawItem(coords.x(), coords.y());
            return false;
          }
          undrawItem(coords.x(), coords.y()); // Remove item on a right click
        }
        else if (!m_Client && e->button() == Qt::LeftButton && si != m_iDrag && si->type() == QGraphicsEllipseItem::Type) startDrag(coords.x(), coords.y()); // Begin dragging circle
        else if (!m_Client && si == m_iDrag) finishDrag(coords.x(), coords.y()); // Cancel drag if you click on the item again
        if (!m_Client && si->type() != CALine::Type && (!m_iDrag || si->type() != QGraphicsRectItem::Type) && si->zValue() >= 0)
        {
          if (si->type() == CAGraphicsItem::Type)
          {
            CAGraphicsItem *gi = dynamic_cast<CAGraphicsItem*>(si);
            if (gi->disallowStacking()) return false;
          }
          else return false;
        }
      }
      if (e->button() == Qt::LeftButton)
      {
        if (!m_Client && m_iDrag)
        {
          finishDrag(coords.x(), coords.y());
          return false;
        }
        switch (m_Shape)
        {
          case 0: // Circle
          {
            if (m_Client) break;
            bool f = false;
            for (int x = 0; x < ie.size(); x++)
            {
              if (ie[x]->type() != QGraphicsLineItem::Type && ie[x]->type() != CALine::Type && ie[x]->type() != QGraphicsPixmapItem::Type && ie[x]->zValue() >= 0)
              {
                  if (ie[x]->type() == CAGraphicsItem::Type && !((CAGraphicsItem* )ie[x])->disallowCirclePlacement())
                  {
                  }
                  else
                  {
                    f = true;
                  }
              }
              if (ie[x]->type() == QGraphicsSimpleTextItem::Type && ie[x]->zValue() < 0) f = true;
						}
            if (!f)
            {
              QString text = QString();
              if (m_addChar)
              {
                text = QInputDialog::getText(m_addChar, tr("Input character"), tr("Character:"), QLineEdit::Normal, QString());
                m_addChar->setChecked(false);
              }
              drawCircle(coords.x(), coords.y(), m_Colour, m_cSize, text);
            }
            break;
          }
          case 1: // Square
          {
            if (m_Client) break;
            QString text = QString();
            if (m_addChar)
            {
              text = QInputDialog::getText(m_addChar, tr("Input character"), tr("Character:"), QLineEdit::Normal, QString());
              m_addChar->setChecked(false);
            }
            drawBlock(coords.x(), coords.y(), m_Colour, text);
            break;
          }
          case 2: // Cross
          {
            if (m_Client) break;
            drawCross(coords.x(), coords.y(), m_Colour);
            break;
          }
          case 3: // Triangle
          {
            if (m_Client) break;
            drawTriangle(coords.x(), coords.y(), m_Colour, m_tDir);
            break;
          }
          case 4: //line
          {
            if (m_Client) break;
            lnStartP = coordsf;
            QPen pen = QPen(m_Colour);
            QPoint start = coord2pixel(lnStartP.x(), lnStartP.y());
            QPoint end = coord2pixel(lnStartP.x(), lnStartP.y());
            lnDrag = new CALine(start, end, pen, start.x()-m_hMargin, start.y()-m_vMargin, end.x()-m_hMargin, end.y()-m_vMargin);
            addItem(lnDrag);
            break;
          }
          case 5: //line with snap-to-grid
          {
            if (m_Client) break;
            lnStartP = mapCorner(e->buttonDownScenePos(e->button()).x(), e->buttonDownScenePos(e->button()).y());
            QPen pen = QPen(m_Colour);
            QPoint start = coord2pixel(lnStartP.x(), lnStartP.y());
            QPoint end = coord2pixel(lnStartP.x(), lnStartP.y());
            lnDrag = new CALine(start, end, pen, start.x()-m_hMargin, start.y()-m_vMargin, end.x()-m_hMargin, end.y()-m_vMargin);
            addItem(lnDrag);
            break;
          }
          case 6: //blast tempate
          {
            QPen *pen = new QPen(m_Colour);
            QPoint p = mapCorner(e->buttonDownScenePos(e->button()).x(), e->buttonDownScenePos(e->button()).y());
            CABlastTemplate * c = new CABlastTemplate(&p, pen, m_radius);
            drawCAItem(c);
            if (!m_Client) emit caItemDrawn(c);
            break;
          }
					case 7: //blast cone
					{
            QPen *pen = new QPen(m_Colour);
            int pref = 0;
            if (m_radius <= 3)
            {
              switch ((CAConeTemplate::direction)m_tDir)
              {
                case CAConeTemplate::N:  pref = 4; break;
                case CAConeTemplate::NE: pref = 3; break;
                case CAConeTemplate::E:  pref = 3; break;
                case CAConeTemplate::SE: pref = 1; break;
                case CAConeTemplate::S:  pref = 1; break;
                case CAConeTemplate::SW: pref = 2; break;
                case CAConeTemplate::W:  pref = 2; break;
                case CAConeTemplate::NW: pref = 4; break;
                default: break;
              }
            }
            QPoint p = mapCorner(e->buttonDownScenePos(e->button()).x(), e->buttonDownScenePos(e->button()).y(), pref);
            if (m_tDir == 0 && p.y() <= 0) break; // north
            else if (m_tDir == 1 && (p.y() <= 0 || p.x() >= getWidth())) break; // northeast
            else if (m_tDir == 2 && p.x() >= getWidth()) break; // east
            else if (m_tDir == 3 && (p.x() >= getWidth() || p.y() >= getHeight())) break; // southeast
            else if (m_tDir == 4 && p.y() >= getHeight()) break; // south
            else if (m_tDir == 5 && (p.x() <= 0 || p.y() >= getHeight())) break; // southwest
            else if (m_tDir == 6 && p.x() <= 0) break; // west
            else if (m_tDir == 7 && (p.x() <= 0 || p.y() <= 0)) break; // northwest
            CAConeTemplate *c = new CAConeTemplate(&p, pen, m_radius, (CAConeTemplate::direction)m_tDir);
            drawCAItem(c);
            if (!m_Client) emit caItemDrawn(c);
            break;
					}
          case 8: //mask
          {
            if (m_Client) break;
            mskStartP = coords;
            QPoint start = coord2pixel(mskStartP.x(), mskStartP.y());
            mskDrag = new CAMask(new QRect(start.x()+1-m_hMargin, start.y()+1-m_vMargin, m_bSize, m_bSize), m_Colour);
            drawCAItem(mskDrag);
            break;
					}
          default: break;
        }
      }
    }
  }
  return false;
}

QPointF QGridScene::dmapPoint(qreal cx, qreal cy)
{
  double x = (int)cx-m_hMargin;
  double y = (int)cy-m_vMargin;
  if (x >= 0 && y >= 0 && (x/=(m_bSize+1)) < m_gWidth && (y/=(m_bSize+1)) < m_gHeight) return QPointF(x, y);
  else return QPointF(-1, -1);
}

void QGridScene::drawLine(double x1, double y1, double x2, double y2, QPen pen)
{
  addItem(new CALine(QPointF(x1+m_hMargin, y1+m_vMargin), QPointF(x2+m_hMargin, y2+m_vMargin), pen, x1, y1, x2, y2));
  emit lineDrawn(x1, y1, x2, y2, pen);
}

QPoint QGridScene::coord2pixel(double x, double y)
{
  return QPoint(x*m_bSize+x+m_hMargin, y*m_bSize+y+m_vMargin);
}

void QGridScene::setRadius(int r)
{
  m_radius = r;
}

void QGridScene::resizeScene(int width, int height, int size)
{
  m_gWidth = width;
  m_gHeight = height;
  m_bSize = size;
  setSceneRect(0, 0, (width*(size+1))+(m_hMargin*2), (height*(size+1))+(m_vMargin*2));
}

void QGridScene::setClient(bool client)
{
  m_Client = client;
}

QPoint QGridScene::mapPoint(qreal cx, qreal cy)
{
  int x = (int)cx-m_hMargin;
  int y = (int)cy-m_vMargin;
  if (x >= 0 && y >= 0 && (x/=(m_bSize+1)) < m_gWidth && (y/=(m_bSize+1)) < m_gHeight) return QPoint(x, y);
  else return QPoint(-1, -1);
}

QPoint QGridScene::mapCorner(qreal cx, qreal cy, int prefer)
{
  int x = (int)cx-m_hMargin;
  int y = (int)cy-m_vMargin;
  if (x >= 0 && y >= 0 && (x/=(m_bSize+1)) < m_gWidth && (y/=(m_bSize+1)) < m_gHeight)
  {
    switch(prefer)
    {
      case 1: // top-left
        return QPoint(x, y);
      case 2: // top-right
        return QPoint(++x, y);
        break;
      case 3: // bottom-left
        return QPoint(x, ++y);
        break;
      case 4: // bottom-right
        return QPoint(++x, ++y);
        break;
      default:
      {
        if (((int)cx-m_hMargin)%(m_bSize+1) > m_bSize/2) x++;
        if (((int)cy-m_vMargin)%(m_bSize+1) > m_bSize/2) y++;
        return QPoint(x, y);
      }
    }
  }
  return QPoint(-1, -1);
}

void QGridScene::drawCircle(int x, int y, QColor c, int size, QString letter)
{
  QGraphicsEllipseItem *el = addEllipse(x*m_bSize+x+m_hMargin+2, y*m_bSize+y+m_vMargin+1, (m_bSize*size)+size-3, (m_bSize*size)+size-2, QPen(Qt::NoPen), c);
  el->setData(0, size);
  el->setZValue(3);
  if (!letter.isNull() && !letter.isEmpty())
  {
    QTextOption o(Qt::AlignHCenter|Qt::AlignVCenter);
    o.setWrapMode(QTextOption::NoWrap);
    QGraphicsTextItem *t = addText(QString(letter.at(0)), QFont("Arial", (int)(m_bSize*0.6)*size, QFont::Bold));
    t->document()->setDocumentMargin(0);
    t->document()->setDefaultTextOption(o);
    t->document()->setTextWidth((m_bSize*size)+size);
    t->setDefaultTextColor(Qt::black);
    t->setZValue(4);
    t->setPos(x*m_bSize+x+m_hMargin+1+((size%2==0)?1:0), y*m_bSize+y+m_vMargin+(size*2));
    t->setTextWidth((m_bSize*size)+size-1);
    t->setParentItem(el);
  }
  emit circleDrawn(x, y, c, size, letter);
}

void QGridScene::drawBlock(int x, int y, QColor c, QString letter)
{
  QGraphicsRectItem *re = addRect(x*m_bSize+x+m_hMargin+1, y*m_bSize+y+m_vMargin+1, m_bSize, m_bSize, QPen(Qt::NoPen), c);
  re->setData(0, 0);
  if (!letter.isNull() && !letter.isEmpty())
  {
    QTextOption o(Qt::AlignHCenter|Qt::AlignVCenter);
    o.setWrapMode(QTextOption::NoWrap);
    QGraphicsTextItem *t = addText(QString(letter.at(0)), QFont("Arial", (int)(m_bSize*0.6), QFont::Bold));
    t->document()->setDocumentMargin(0);
    t->document()->setDefaultTextOption(o);
    t->document()->setTextWidth(m_bSize+1);
    t->setDefaultTextColor(Qt::black);
    t->setZValue(1);
    t->setPos(x*m_bSize+x+m_hMargin+1, y*m_bSize+y+m_vMargin+1);
    t->setTextWidth(m_bSize);
    t->setParentItem(re);
  }
  emit blockDrawn(x, y, c, letter);
}

void QGridScene::drawCross(int x, int y, QColor c)
{
  QPainterPath line1, line2, path;
  QPolygonF poly, polx;
  line1.moveTo(x*m_bSize+x+m_hMargin+2, y*m_bSize+y+m_vMargin+2);
  line1.lineTo(x*m_bSize+x+m_hMargin+3, y*m_bSize+y+m_vMargin+1);
  line1.lineTo(x*m_bSize+x+m_hMargin+m_bSize, y*m_bSize+y+m_vMargin+m_bSize-1);
  line1.lineTo(x*m_bSize+x+m_hMargin+m_bSize, y*m_bSize+y+m_vMargin+m_bSize);
  line1.closeSubpath();
  line2.moveTo(x*m_bSize+x+m_hMargin+2, y*m_bSize+y+m_vMargin+m_bSize-1);
  line2.lineTo(x*m_bSize+x+m_hMargin+3, y*m_bSize+y+m_vMargin+m_bSize);
  line2.lineTo(x*m_bSize+x+m_hMargin+m_bSize, y*m_bSize+y+m_vMargin+2);
  line2.lineTo(x*m_bSize+x+m_hMargin+m_bSize, y*m_bSize+y+m_vMargin+1);
  line2.closeSubpath();
  line1.setFillRule(Qt::WindingFill);
  line2.setFillRule(Qt::WindingFill);
  poly = line1.toFillPolygon();
  polx = line2.toFillPolygon();
  path.setFillRule(Qt::WindingFill);
  path = line1.united(line2);
  QGraphicsPathItem *p = addPath(path, QPen(Qt::NoPen), c);
  p->setData(0, 0);
  emit crossDrawn(x, y, c);
}

void QGridScene::drawTriangle(int x, int y, QColor c, int facing)
{
  QRectF r(QPointF(x*m_bSize+x+m_hMargin+1, y*m_bSize+y+m_vMargin+1), QSizeF(m_bSize, m_bSize));
  QGraphicsPolygonItem *t;
  QPolygonF poly;
  if (facing==0) poly << QPointF(0, m_bSize-1) << QPointF(m_bSize-1, m_bSize-1) << QPointF((m_bSize/2), 0) << QPointF((m_bSize/2)-1, 0);
  if (facing==1) poly << QPointF(0, (m_bSize/2)-2) << QPointF(m_bSize-1, 0) << QPointF((m_bSize/2)+1, m_bSize-1);
  if (facing==2) poly << QPointF(0, 0) << QPointF(0, m_bSize-1) << QPointF(m_bSize-1, (m_bSize/2)) << QPointF(m_bSize-1, (m_bSize/2)-1);
  if (facing==3) poly << QPointF((m_bSize/2)+1, 0) << QPointF(m_bSize-1, m_bSize-1) << QPointF(0, (m_bSize/2)+1);
  if (facing==4) poly << QPointF(0, 0) << QPointF(m_bSize-1, 0) << QPointF((m_bSize/2), m_bSize-1) << QPointF((m_bSize/2)-1, m_bSize-1);
  if (facing==5) poly << QPointF((m_bSize/2)-2, 0) << QPointF(m_bSize-1, (m_bSize/2)+1) << QPointF(0, m_bSize-1);
  if (facing==6) poly << QPointF(0, (m_bSize/2)) << QPointF(0, (m_bSize/2)-1) << QPointF(m_bSize-1, 0) << QPointF(m_bSize-1, m_bSize-1);
  if (facing==7) poly << QPointF(m_bSize-1, (m_bSize/2)-2) << QPointF((m_bSize/2)-2, m_bSize-1) << QPointF(0, 0);
  t = new QGraphicsPolygonItem(poly);
  t->setBrush(c);
  t->setPen(QPen(QBrush(c), 0, Qt::SolidLine, Qt::SquareCap,  Qt::BevelJoin));
  t->setPos(x*m_bSize+x+m_hMargin+1, y*m_bSize+y+m_vMargin+1);
  t->setData(0, facing);
  addItem(t);
  emit triangleDrawn(x, y, c, facing);
}

void QGridScene::drawCAItem(CAGraphicsItem *i)
{
  i->attachToScene(this);
}

void QGridScene::undrawItem(int x, int y)
{
  QList<QGraphicsItem *>i = items(x*m_bSize+x+m_hMargin+1, y*m_bSize+y+m_vMargin+1, m_bSize-1, m_bSize-1, Qt::IntersectsItemShape);
  if (x >= 0 && y >= 0 && i.size() > 0)
  {
    QGraphicsItem *si = i.at(0);
    if (si->type() == QGraphicsPixmapItem::Type || si->zValue() < 0) return;
    if (i.at(0)->parentItem() != 0) si = i.at(0)->parentItem();
    if (si == m_iDrag) m_iDrag = NULL;
    emit itemRemoved(x, y);
    removeItem(si);
  }
}

void QGridScene::undrawItem(quint16 check)
{
  QList<QGraphicsItem *>i = items(0, 0, width(), height());
  for (int x = 0; x < i.size(); x++)
  {
    if (i[x]->type() == QGraphicsPixmapItem::Type || i[x]->zValue() < 0) continue;
    if (i[x]->type() == CALine::Type)
    {
      CALine *c = static_cast<CALine*>(i[x]);
      if (c->check() == check)
      {
        emit itemRemoved(check);
        removeItem(c);
        return;
      }
    }
  }
}

void QGridScene::startDrag(int x, int y)
{
  QList<QGraphicsItem *>i = items(x*m_bSize+x+m_hMargin+1, y*m_bSize+y+m_vMargin+1, m_bSize-1, m_bSize-1, Qt::IntersectsItemShape);
  if (x >= 0 && y >= 0 && i.size() > 0)
  {
    QGraphicsItem *si = i[0];
    if (i[0]->parentItem() != 0) si = i[0]->parentItem();
    if (m_iDrag) m_iDrag->setOpacity(1.0);
    m_iDrag = si;
    si->setOpacity(0.5);
    emit dragStarted(x, y);
  }
}

void QGridScene::finishDrag(int x, int y)
{
  if (m_iDrag && m_iDrag->type() == QGraphicsEllipseItem::Type)
  {
    QList<QGraphicsItem *>ie = items(x*m_bSize+x+m_hMargin+1, y*m_bSize+y+m_vMargin+1, ((m_iDrag)?m_iDrag->boundingRect().width()+1:(m_bSize*m_cSize)+m_cSize-1), ((m_iDrag)?m_iDrag->boundingRect().height():(m_bSize*m_cSize)+m_cSize-1), Qt::IntersectsItemShape);
    QGraphicsEllipseItem *it = static_cast<QGraphicsEllipseItem*>(m_iDrag);
    bool f = false;
    if (x >= 0 && y >= 0)
    {
      for (int i = 0; i < ie.size(); i++)
        if (ie[i]->type() != QGraphicsLineItem::Type && ie[i]->type() != CALine::Type && ie[i]->type() != QGraphicsRectItem::Type && ie[i]->type() != QGraphicsPixmapItem::Type && ie[i]->type() != QGraphicsTextItem::Type && ie[i] != m_iDrag)
        {
            if (ie[i]->type() == CAGraphicsItem::Type)
            {
                if (((CAGraphicsItem* )ie[i])->disallowCirclePlacement())
                {
                    f = true;
                }
            }
            else
            {
              f = true;
            }
        }
			if (!f)
      {
        QList<QGraphicsItem *>ci = it->childItems();
        it->setRect(x*m_bSize+x+m_hMargin+2, y*m_bSize+y+m_vMargin+1, it->rect().width(), it->rect().height());
        it->setZValue(3);
        if (ci.size() > 0)
        {
          int dragSize = it->data(0).toInt();
          ci[0]->setPos(x*m_bSize+x+m_hMargin+1, y*m_bSize+y+m_vMargin+1+dragSize);
          ci[0]->setZValue(4);
        }
      }
    }
    m_iDrag->setOpacity(1.0);
    update(0, 0, width(), height());
    m_iDrag = NULL;
    emit dragFinished(x, y);
  }
}

void QGridScene::clearGrid(bool total, bool saveBackground)
{
  QList<QGraphicsItem*>i = items();
  for (int x = 0; x < i.size(); x++)
  {
    if (!total && ((i[x]->type() == QGraphicsEllipseItem::Type && !i[x]->children().isEmpty()))) continue;
    if (saveBackground && i[x]->type() == QGraphicsPixmapItem::Type) continue;
    else if (i[x]->type() != QGraphicsLineItem::Type && i[x]->type() != QGraphicsSimpleTextItem::Type && i[x]->parentItem() == 0) removeItem(i[x]);
  }
  if (total && !saveBackground) emit totalClear();
}

void QGridScene::setBackgroundColour(QColor c)
{
  if (c.isValid())
  {
    setBackgroundBrush(QBrush((m_bgColour=c), Qt::SolidPattern));
    update(0, 0, width(), height());
  }
}
