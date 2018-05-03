#ifndef CAGRAPHICSITEM_H
#define CAGRAPHICSITEM_H

#include <QGraphicsItem>

class CAGraphicsItem :  public QObject
{
    Q_OBJECT

public:

     enum { Type = QGraphicsItem::UserType + 3 };
     int type() const
     {
       // Enable the use of qgraphicsitem_cast with this item.
       return Type;
     }
     virtual int subtype() const = 0; // Identifier
     virtual char* transmit() = 0;    //send the item over the wire (serialize)
                                      //memory must be freed with free()
     virtual void attachToScene(void* scene) = 0; //draw the item
     virtual bool disallowStacking() const = 0; // Should items be able to stack?
     virtual bool disallowCirclePlacement() = 0; //Should circles be placeable?
     virtual void addMargins(int h, int v) = 0; // Add margins
public:
     //virtual static CAGraphicsItem* parse(const char* s);
};

Q_DECLARE_INTERFACE(CAGraphicsItem, "CAGraphicsItem/1.0")
#endif // CAGRAPHICSITEM_H
