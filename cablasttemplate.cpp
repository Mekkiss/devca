#include "cablasttemplate.h"
#include "qgridscene.h"
#include <QPainterPath>
#include <QGraphicsPathItem>

CABlastTemplate::CABlastTemplate(QObject *parent)
{        
}
CABlastTemplate::CABlastTemplate(QPoint *c, QPen *p, int s)
{
  centre = new QPoint(c->x(), c->y());
  pen = new QPen(p->brush(), p->width(), p->style(), p->capStyle(), p->joinStyle());
  size = s ;
}

char* CABlastTemplate::transmit()
{
  char *s = (char*) malloc(50 * sizeof(char));

  sprintf(s, "%i,%i %i %i %i %i %i", CABlastTemplate::subtype(), centre->x(), centre->y(),
           pen->color().red(), pen->color().green(), pen->color().blue(), size);

  return s;
}

bool CABlastTemplate::disallowStacking() const
{
    return false;
}
bool CABlastTemplate::disallowCirclePlacement()
{
    return false;
}

void CABlastTemplate::addMargins(int h, int v)
{
  //m_Rect->adjust(h, v, h, v);
}

CABlastTemplate* CABlastTemplate::parse(QString *s)
{
    int c1, c2;
    int r,g,b;
    int si;
    QString str = *s;
    str.remove(QRegExp("^.*,"));
    QStringList l = str.split(" ");
    c1 = l.first().toInt();
    l.removeFirst();
    c2 = l.first().toInt();
    l.removeFirst();
    r = l.first().toInt();
    l.removeFirst();
    g = l.first().toInt();
    l.removeFirst();
    b = l.first().toInt();
    l.removeFirst();
    si = l.first().toInt();
    l.removeFirst();
    QPoint *c = new QPoint(c1, c2);
    QPen *p = new QPen(QColor(r, g, b));
    CABlastTemplate* cbt = new CABlastTemplate(c, p, si);
    return cbt;
}

/* This #define was used when we were using QGraphicsItemGroup
#define CALINE_START_END startpxl = s->coord2pixel(start.x(), start.y()); \
                       endpxl = s->coord2pixel(end.x(), end.y()); \
                       l = new CALine(QPointF(startpxl), QPointF(endpxl), *pen, \
                            (double)start.x(), (double)start.y(), (double)end.x(), (double)end.y()); \
                       addToGroup(l) */
#define CALINE_START_END endpxl = s->coord2pixel(end.x(), end.y()); \
    p.lineTo(QPointF(endpxl))

QPainterPath CABlastTemplate::getPath(void *scene)
{
    QGridScene* s = (QGridScene*)scene;
    QPoint cursor = *centre;
    QPoint end = cursor;
    QPoint endpxl;
    bool last_x = true;
    bool last_2 = false;
    cursor.rx() += size;
    cursor.ry()--;
    cursor.rx() = 0; cursor.ry() = size;
    end = cursor;
    QPainterPath p = QPainterPath(QPointF(s->coord2pixel(cursor.x(), cursor.y())));
    p.setFillRule(Qt::OddEvenFill);

    end.rx()++;
    CALINE_START_END;
    while(end.x() < end.y())
    {
        if (last_x)
        {
            end.ry()--;
        }
        else
        {
            end.rx()++;
            if (end.x() < end.y())
            {
                end.rx()++;
                last_2 = true;
            }
            else last_2 = false;
        }
        last_x ^= true;
        CALINE_START_END;
    }
    while(end.y() > 0)
    {
        if (last_x)
        {
            end.ry()--;
            if (end.y() > 0 && last_2)
            {
                end.ry()--;
                last_2 = true;
            }
            else last_2 ^= true;
        }
        else
        {
            end.rx()++;
        }
        CALINE_START_END;
        last_x ^= true;
    }
    end.ry()--;
    CALINE_START_END;
    end.rx()--;
    CALINE_START_END;
    last_x = true;
    while(-end.x() < end.y())
    {
        if (last_x)
        {
            end.ry()--;
            if (-end.x() < end.y())
            {
                end.ry()--;
                last_2 = true;
            }
            else last_2 = false;
        }
        else
        {
            end.rx()--;
        }
        last_x ^= true;
        CALINE_START_END;
    }
    while(end.x() > 0)
    {
        if (last_x)
        {
            end.ry()--;
        }
        else
        {
            end.rx()--;
            if (end.x() > 0 && last_2)
            {
                end.rx()--;
                last_2 = true;
            }
            else last_2 ^= true;
        }
        CALINE_START_END;
        last_x ^= true;
    }
    end.rx()--;
    CALINE_START_END;
    end.ry()++;
    CALINE_START_END;
    last_x = false;
    last_2 = true;
    while(end.y() < end.x())
    {
        if (last_x)
        {
            end.ry()++;
        }
        else
        {
            end.rx()--;
            if (end.y() < end.x() && last_2)
            {
                end.rx()--;
                last_2 = true;
            }
            else last_2 = false;
        }
        CALINE_START_END;
        last_x ^= true;
    }
    while(end.y() < 0)
    {
        if (last_x)
        {
            end.ry()++;
            if (end.y() < 0 && last_2)
            {
                end.ry()++;
                last_2 = true;
            }
            else last_2 ^= true;
        }
        else
        {
            end.rx()--;
        }
        last_x ^= true;
        CALINE_START_END;
    }
    end.ry()++;
    CALINE_START_END;
    end.rx()++;
    CALINE_START_END;
    last_x = true;
    while(-end.x() > end.y())
    {
        if (last_x)
        {
            end.ry()++;
            if (-end.x() > end.y())
            {
                end.ry()++;
                last_2 = true;
            }
            else last_2 = false;
        }
        else
        {
            end.rx()++;
        }
        last_x ^= true;
        CALINE_START_END;
    }
    while(end.x() < 0)
    {
        if (last_x)
        {
            end.ry()++;
        }
        else
        {
            end.rx()++;
            if (end.x() < 0 && last_2)
            {
                end.rx()++;
                last_2 = true;
            }
            else last_2 ^= true;
        }
        CALINE_START_END;
        last_x ^= true;
    }

    p.closeSubpath();

    return p;
}

void CABlastTemplate::attachToScene(void* scene)
{
  QGridScene* s = (QGridScene*)scene;
  QPainterPath p = getPath(scene);
  QPointF offset(s->coord2pixel(centre->x(), centre->y()));
  p.translate(offset);
  p.translate(-(s->coord2pixel(0,0)));
  setPath(p);
  setPen(*pen);
  QBrush b = pen->brush();
  QColor col = b.color();
  col.setAlpha(40);
  b.setColor(col);
  setBrush(b);
  if (!s->items().contains(this)) s->addItem(this);
}
