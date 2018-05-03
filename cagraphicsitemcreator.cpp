#include "cagraphicsitemcreator.h"
#include "cagraphicsitem.h"
#include "cablasttemplate.h"
#include "caconetemplate.h"
#include "camask.h"

#include <cstdio>
CAGraphicsItemCreator::CAGraphicsItemCreator(QObject *parent) :
    QObject(parent)
{
}

CAGraphicsItem* CAGraphicsItemCreator::create(int subtype, QString *s)
{
  switch (subtype)
  {
    case CABlastTemplate::SubType:
      return CABlastTemplate::parse(s);
      break;
    case CAConeTemplate::SubType:
      return CAConeTemplate::parse(s);
      break;
    case CAMask::SubType:
      return CAMask::parse(s);
      break;
    default:
      return NULL;
  }
}
