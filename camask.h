#ifndef CAMASK_H
#define CAMASK_H

#include <QGraphicsPathItem>
#include "cagraphicsitem.h"

class CAMask : public CAGraphicsItem, public QGraphicsPathItem
{
     Q_INTERFACES(CAGraphicsItem)
    Q_OBJECT
public:
    explicit CAMask(QObject *parent = 0);
    CAMask(QRect *r, QColor c);
    enum { SubType = UserType + 7 };
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
    QRect *rect() {return m_Rect;}

    static CAMask* parse(QString* s);

private:
    QRect *m_Rect;
    QColor m_Colour;

signals:

public slots:

};

#endif // CAMASK_H
