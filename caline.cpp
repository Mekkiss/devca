#include "caline.h"

CALine::CALine()
{

}

CALine::CALine(const QPointF & p1, const QPointF & p2, const QPen &pen, double x1, double y1, double x2, double y2)
{

  setLine(QLine(p1.toPoint(), p2.toPoint()));
  m_startX = x1;
  m_startY = y1;
  m_endX = x2;
  m_endY = y2;
  m_color = pen.color();
  setPen(pen);
  m_check = lineChecksum();
}

quint16 CALine::lineChecksum()
{
  char check[255];
  sprintf(check, "x1%.6fy1%.6fx2%.6fy2%.6f", m_startX, m_startY, m_endX, m_endY);
  return qChecksum(check, strlen(check));
}

/*void CALine::attachToScene(void* scene)
{
  QGridScene* s = (QGridScene*)scene;
  setLine(QLine(x1+s->hMargin(), y1+s->vMargin(), x2+s->hMargin(), y2+s->vMargin()));
  if (!s->items().contains(this)) s->addItem(this);
}*/
