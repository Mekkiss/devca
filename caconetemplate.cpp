#include "caconetemplate.h"
#include <cstdio>
#include "qgridscene.h"
#include "cablasttemplate.h"

CAConeTemplate::CAConeTemplate(QObject *parent)
{
}

CAConeTemplate::CAConeTemplate(QPoint *c, QPen *p, int s, direction d)
{
  centre = new QPoint(c->x(), c->y());
  pen = new QPen(p->brush(), p->width(), p->style(), p->capStyle(), p->joinStyle());
  size = s ;
  this->d = d;
}

char* CAConeTemplate::transmit()
{
    char *s = (char*) malloc(50 * sizeof(char));

    sprintf(s, "%i,%i %i %i %i %i %i %i", CAConeTemplate::subtype(), centre->x(), centre->y(),
             pen->color().red(), pen->color().green(), pen->color().blue(), size, d);

    return s;
}

bool CAConeTemplate::disallowStacking() const
{
    return false;
}
bool CAConeTemplate::disallowCirclePlacement()
{
    return false;
}

void CAConeTemplate::addMargins(int h, int v)
{
  //m_Rect->adjust(h, v, h, v);
}

CAConeTemplate* CAConeTemplate::parse(QString *s)
{
    int c1, c2;
    int r,g,b;
    int si; int d;
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
    d = l.first().toInt();
    l.removeFirst();
    QPoint *c = new QPoint(c1, c2);
    QPen *p = new QPen(QColor(r, g, b));
    CAConeTemplate* cbt = new CAConeTemplate(c, p, si, (direction)d);
    return cbt;
}

#define CALINE_START_END endpxl = s->coord2pixel(end.x(), end.y()); \
    p.lineTo(QPointF(endpxl))

void CAConeTemplate::attachToScene(void* scene)
{
    QGridScene* s = (QGridScene*)scene;
    QPoint cursor = *centre;
    QPoint end = QPoint(0,0);
    QPoint endpxl;
    bool last_x = true;
    bool last_2 = false;
    cursor.rx() += size;
    cursor.ry()--;
    cursor.rx() = 0; cursor.ry() = size;

    QPainterPath p = QPainterPath(QPointF(s->coord2pixel(0, 0)));

    p.setFillRule(Qt::OddEvenFill);
    if (d % 2 == 1) // NE, SE, SW, NW
    {

        end = cursor;
        CALINE_START_END;
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
        setRotation((qreal)(45 * (d-3)));
    }
    // N, E, S, W
    else if (size < 3) // Don't bother with 1 and 2 square cones: they shouldn't come up...
    {
        return;
    }
    else if ( size < 4) // special handling for 3 square cones: they don't follow standards...
    {
        end.rx()++;
        CALINE_START_END;
        end.ry()++;
        CALINE_START_END;
        end.rx()++;
        CALINE_START_END;
        end.ry()+=2;
        CALINE_START_END;
        end.rx()-=3;
        CALINE_START_END;
        end.ry()-=2;
        CALINE_START_END;
        end.rx()++;
        CALINE_START_END;
        setRotation((qreal)(45 * (d+4)));
    }
    else
    {
        p = QPainterPath(QPointF(s->coord2pixel(size, size)));
        end = QPoint(size, size);
        while (end.x() > 0)
        {
            end.rx()--;
            CALINE_START_END;
            end.ry()--;
            CALINE_START_END;
        }
        while (end.x() < size)
        {
            end.ry()--;
            CALINE_START_END;
            end.rx()++;
            CALINE_START_END;
        }
        CABlastTemplate c(centre, pen, size);
        p = p.intersected(c.getPath(scene));
        setRotation((qreal)(45 * (d+6)));
    }
    p.closeSubpath();
    QPointF offset(s->coord2pixel(centre->x(), centre->y()));
    p.translate(offset);
    p.translate(-(s->coord2pixel(0,0)));

    setTransformOriginPoint(QPointF(s->coord2pixel(centre->x(), centre->y())));
    setPath(p);
    setPen(*pen);
    QBrush b = pen->brush();
    QColor col = b.color();
    col.setAlpha(40);
    b.setColor(col);
    setBrush(b);
    setZValue(5);
    s->addItem(this);
}
