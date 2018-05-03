#ifndef CALINE_H
#define CALINE_H

#include <QLineF>
#include <QPen>
#include <QColor>
#include <QGraphicsLineItem>
#include <cstdio>

class CALine : public QGraphicsLineItem
{

public:
  CALine();
  CALine(const QPointF & p1, const QPointF & p2, const QPen &pen, double x1, double y1, double x2, double y2);
  double startX() { return m_startX; }
  double startY() { return m_startY; }
  double endX() { return m_endX; }
  double endY() { return m_endY; }
  QColor getColor() { return m_color; }
  quint16 check() { return m_check; }
  //void attachToScene(void* scene);
  enum { Type = UserType + 1 };

  int type() const
  {
    // Enable the use of qgraphicsitem_cast with this item.
    return Type;
  }

private:
  double m_startX;
  double m_startY;
  double m_endX;
  double m_endY;
  quint16 m_check;
  QColor m_color;
  quint16 lineChecksum();
};

#endif // CALINE_H
