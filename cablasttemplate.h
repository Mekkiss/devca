#ifndef CABLASTTEMPLATE_H
#define CABLASTTEMPLATE_H

#include <QGraphicsPathItem>
#include <QGraphicsItem>
#include "cagraphicsitem.h"
#include <QPainterPath>

class CABlastTemplate : public CAGraphicsItem, public QGraphicsPathItem
{
        Q_INTERFACES(CAGraphicsItem)
    Q_OBJECT
public:
    explicit CABlastTemplate(QObject *parent = 0);
    CABlastTemplate(QPoint *c, QPen *p, int s);
    enum { SubType = UserType + 5 };
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
    QPainterPath getPath(void* scene);

public:
    static CABlastTemplate* parse(QString* s);

private:
    QPoint *centre;
    QPen *pen;
    int size;
signals:

public slots:

};

#endif // CABLASTTEMPLATE_H
