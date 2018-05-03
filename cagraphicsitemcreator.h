#ifndef CAGRAPHICSITEMCREATOR_H
#define CAGRAPHICSITEMCREATOR_H

#include <QObject>
#include "cagraphicsitem.h"
class CAGraphicsItemCreator : public QObject
{
    Q_OBJECT
public:
    explicit CAGraphicsItemCreator(QObject *parent = 0);
    static CAGraphicsItem* create(int subtype, QString *s);
signals:

public slots:

};

#endif // CAGRAPHICSITEMCREATOR_H
