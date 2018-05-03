#ifndef CACONETEMPLATE_H
#define CACONETEMPLATE_H

#include <QGraphicsItemGroup>
#include <QGraphicsItem>
#include "cagraphicsitem.h"

class CAConeTemplate :  public CAGraphicsItem, public QGraphicsPathItem
{
    Q_INTERFACES(CAGraphicsItem)
    Q_OBJECT
public:
    enum direction { N, NE, E, SE, S, SW, W, NW };
    explicit CAConeTemplate(QObject *parent = 0);
    CAConeTemplate(QPoint *c, QPen *p, int s, direction d);
    enum { SubType = UserType + 6 };

    int subtype() const
    {
      // Enable the use of qgraphicsitem_cast with this item.
      return SubType;
    }

    int Type() const
    {
      // Enable the use of qgraphicsitem_cast with this item.
      return CAGraphicsItem::Type;
    }
    int type() const
    {
      // Enable the use of qgraphicsitem_cast with this item.
        return Type();
    }

    char* transmit();
    void attachToScene(void* scene);
    bool disallowStacking() const;
    bool disallowCirclePlacement();
    void addMargins(int h, int v);
public:
    static CAConeTemplate* parse(QString* s);
signals:

public slots:
private:
    QPoint *centre;
    QPen *pen;
    int size;
    direction d;
};

#endif // CACONETEMPLATE_H
