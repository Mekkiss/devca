#include "qsidebutton.h"

QSideButton::QSideButton(QWidget *parent) :
    QToolButton(parent)
{
  QPixmap px = QPixmap(16, 16);
  px.fill(QColor(0, 0, 0, 0));
  QPainter p(&px);
  p.setBrush(Qt::SolidPattern);
  p.setPen(Qt::SolidPattern);
  p.setBackgroundMode(Qt::OpaqueMode);
  p.setBackground((m_Colour=Qt::black));
  p.drawEllipse(0, 0, 15, 15);
  QIcon i(px);
  setIcon(i);
}

void QSideButton::changeColour(QColor c)
{
  QPixmap px = QPixmap(16, 16);
  px.fill(QColor(0, 0, 0, 0));
  QPainter p(&px);
  p.setBrush(QBrush((m_Colour=c), Qt::SolidPattern));
  p.setPen(c);
  p.setBackgroundMode(Qt::OpaqueMode);
  p.setBackground(QBrush(c, Qt::SolidPattern));
  p.drawEllipse(0, 0, 15, 15);
  QIcon i(px);
  setIcon(i);
}

QColourButton::QColourButton(QWidget *parent) :
    QToolButton(parent)
{
  m_Colour = Qt::white;
}

void QColourButton::changeColour(QColor c)
{
  QPixmap px = QPixmap(16, 16);
  px.fill(QColor(0, 0, 0, 0));
  QPainter p(&px);
  p.setBrush(QBrush((m_Colour=c), Qt::SolidPattern));
  p.setPen(c);
  p.setBackgroundMode(Qt::OpaqueMode);
  p.setBackground(QBrush(c, Qt::SolidPattern));
  p.drawRect(0, 0, 16, 16);
  QIcon i(px);
  setIcon(i);
}
