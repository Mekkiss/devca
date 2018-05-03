#include "camask.h"
#include <QBuffer>
#include "qgridscene.h"
#include <QMessageBox>

CAMask::CAMask(QObject *parent)
{
  m_Colour = Qt::white;
}

CAMask::CAMask(QRect *r, QColor c)
{
  m_Rect = new QRect(r->topLeft(), r->size());
  m_Colour = c;
}

char * CAMask::transmit()
{
  //char* s;
  char *s = (char*) malloc(50 * sizeof(char));
  int x1, y1, x2, y2;
  m_Rect->getCoords(&x1, &y1, &x2, &y2);
  sprintf(s, "%i,%i %i %i %i %i %i %i", subtype(), x1, y1, x2, y2, m_Colour.red(), m_Colour.green(), m_Colour.blue());
  //asprintf(&s, "%i,%i %i %i %i", subtype(), x1, y1, x2, y2);
  return s;
}
bool CAMask::disallowStacking() const
{
    return true;
}
bool CAMask::disallowCirclePlacement()
{
    return false;
}

void CAMask::addMargins(int h, int v)
{
  m_Rect->adjust(h, v, h, v);
}

void CAMask::attachToScene(void* scene)
{
    QGridScene* s = (QGridScene*)scene;
    setPen(QPen(m_Colour));
    QPainterPath p = QPainterPath(QPointF(m_Rect->topLeft().x()+s->hMargin(), m_Rect->topLeft().y()+s->vMargin()));
    p.lineTo(QPointF(m_Rect->topRight().x()+s->hMargin(), m_Rect->topRight().y()+s->vMargin()));
    p.lineTo(QPointF(m_Rect->bottomRight().x()+s->hMargin(), m_Rect->bottomRight().y()+s->vMargin()));
    p.lineTo(QPointF(m_Rect->bottomLeft().x()+s->hMargin(), m_Rect->bottomLeft().y()+s->vMargin()));
    p.closeSubpath();
    setPath(p);
    setBrush(QBrush(m_Colour));
    setZValue(6);
    if (!s->items().contains(this)) s->addItem(this);
}

CAMask* CAMask::parse(QString* s)
{
  QString str = *s;
  int x1, y1, x2, y2, r, g, b;
  str.remove(QRegExp("^.*,"));
  QStringList l = str.split(" ");
  x1 = l.first().toInt();
  l.removeFirst();
  y1= l.first().toInt();
  l.removeFirst();
  x2 = l.first().toInt();
  l.removeFirst();
  y2 = l.first().toInt();
  l.removeFirst();
  r = l.first().toInt();
  l.removeFirst();
  g = l.first().toInt();
  l.removeFirst();
  b = l.first().toInt();
  QRect *rect = new QRect(QPoint(x1, y1), QPoint(x2, y2));
  CAMask *m = new CAMask(rect, QColor(r, g, b));
  return m;
}
