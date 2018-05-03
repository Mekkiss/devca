#include "qgridview.h"

QGridView::QGridView(QGraphicsScene *scene, int s, QColor b, int cSize, int tDir, QToolButton *addChar, int thick, int radius, int l, QWidget *parent) :
    QGraphicsView(scene, parent)
{
  setDefaults(s, b, cSize, tDir, addChar, thick, radius, l);
}

QGridView::QGridView(int s, QColor b, int cSize, int tDir, QToolButton *addChar, int thick, int radius, int l, QWidget *parent) :
    QGraphicsView(parent)
{
  setDefaults(s, b, cSize, tDir, addChar, thick, radius, l);
}

QGridView::~QGridView()
{
  delete m_Background;
  delete m_Grid;
  delete m_Manager;
  if (m_Scene) delete m_Scene;
}

void QGridView::setDefaults(int s, QColor b, int cSize, int tDir, QToolButton *addChar, int thick, int radius, int l)
{
  QSettings settings("ca.ini", QSettings::IniFormat);
  m_tWidth = 0;
  m_tHeight = 0;
  m_Shape = s;
  m_Colour = b;
  m_cSize = cSize;
  m_tDir = tDir;
  m_radius = radius;
  m_lThick = thick;
  bgFileName = QString("");
  m_Background = new CABackground("");
  m_bgColour = settings.value("grid/bgColour", Qt::black).value<QColor>();
  m_lnColour = settings.value("grid/lnColour", Qt::white).value<QColor>();
  m_coColour = settings.value("grid/coColour", Qt::yellow).value<QColor>();
  m_coFont.fromString(settings.value("coFont", QFont("Courier New", 8)).toString());
  m_lStyle = l;
  m_Init = false;
  m_addChar = addChar;
  m_Grid = new CAGrid(20, 20, settings.value("hMargin", 20).toInt(), settings.value("vMargin", 15).toInt(), settings.value("grid/cellSize", 20).toInt(), m_lnColour, m_lStyle, m_coFont, m_coColour);
  setFrameShape(QFrame::NoFrame);
  m_Client = false;
  m_Manager = new QNetworkAccessManager(this);
  connect(m_Manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)), Qt::AutoConnection);
  installEventFilter(this);
}

void QGridView::setShape(int s)
{
  if (m_Scene) m_Scene->setShape(s);
  m_Shape = s;
}

void QGridView::setColour(QColor b)
{
  if (m_Scene) m_Scene->setColour(b);
  m_Colour = b;
}

void QGridView::setCSize(int s)
{
  if (m_Scene) m_Scene->setCSize(s);
  m_cSize = s;
}

void QGridView::setTDir(int t)
{
  if (m_Scene) m_Scene->setTDir(t);
  m_tDir = t;
}

void QGridView::setRadius(int r)
{
  if (m_Scene) m_Scene->setRadius(r);
  m_radius = r;
}

int QGridView::hMargin()
{
  if (m_Scene) return m_Scene->hMargin();
  QSettings settings("ca.ini", QSettings::IniFormat);
  return settings.value("hMargin", 20).toInt();
}

int QGridView::vMargin()
{
  if (m_Scene) return m_Scene->vMargin();
  QSettings settings("ca.ini", QSettings::IniFormat);
  return settings.value("vMargin", 15).toInt();
}

void QGridView::setAddChar(QToolButton *on)
{
  if (m_Scene) m_Scene->setAddChar(on);
  m_addChar = on;
}

void QGridView::setLThick(int t)
{
  if (m_Scene) m_Scene->setLThick(t);
  m_lThick = t;
}

void QGridView::zoom(int direction)
{
    ViewportAnchor v = transformationAnchor();
    setTransformationAnchor(AnchorUnderMouse);
    if (direction > 0)
    {
        scale(1.1, 1.1);
    }
    else
    {
        scale(0.9, 0.9);
    }
    setTransformationAnchor(v);
}


bool QGridView::eventFilter(QObject *object, QEvent *event)
{
  if (event->type() == 67 && !m_Init && !m_Scene)
  {
    drawGrid();
    m_Init = true;
  }
  return false;
}

void QGridView::drawGrid()
{
  QSettings settings("ca.ini", QSettings::IniFormat);
  m_bgColour = settings.value("grid/bgColour", Qt::black).value<QColor>();
  m_lnColour = settings.value("grid/lnColour", Qt::white).value<QColor>();
  m_coColour = settings.value("grid/coColour", Qt::yellow).value<QColor>();
  m_coFont.fromString(settings.value("coFont", QFont("Courier New", 8)).toString());
  int hMargin = settings.value("hMargin", 20).toInt();
  int vMargin = settings.value("vMargin", 15).toInt();
  m_lStyle = settings.value("grid/lineStyle", Qt::DotLine).toInt();
  bool ar = settings.value("autoResize", true).toBool();
  if (m_Scene) delete m_Scene;
  int newWidth = (m_Grid->width()*m_Grid->cellSize())+m_Grid->width()+(hMargin*2)+1;
  int newHeight = (m_Grid->height()*m_Grid->cellSize())+m_Grid->height()+(vMargin*2)+1;
  m_Scene = new QGridScene(0, 0, newWidth, newHeight, m_Grid->width(), m_Grid->height(), m_Grid->cellSize(), m_Shape, m_Colour, m_cSize, m_tDir, m_addChar, m_lThick, m_lStyle, m_Client);
  connect(m_Scene, SIGNAL(circleDrawn(int,int,QColor,int,QString)), this, SLOT(passCircle(int,int,QColor,int,QString)), Qt::AutoConnection);
  connect(m_Scene, SIGNAL(blockDrawn(int,int,QColor,QString)), this, SLOT(passBlock(int,int,QColor,QString)), Qt::AutoConnection);
  connect(m_Scene, SIGNAL(crossDrawn(int,int,QColor)), this, SLOT(passCross(int,int,QColor)), Qt::AutoConnection);
  connect(m_Scene, SIGNAL(triangleDrawn(int,int,QColor,int)), this, SLOT(passTriangle(int,int,QColor,int)), Qt::AutoConnection);
  connect(m_Scene, SIGNAL(itemRemoved(int,int)), this, SLOT(passRemove(int,int)), Qt::AutoConnection);
  connect(m_Scene, SIGNAL(itemRemoved(quint16)), this, SLOT(passRemove(quint16)), Qt::AutoConnection);
  connect(m_Scene, SIGNAL(dragStarted(int,int)), this, SLOT(passStartDrag(int,int)), Qt::AutoConnection);
  connect(m_Scene, SIGNAL(dragFinished(int,int)), this, SLOT(passFinishDrag(int,int)), Qt::AutoConnection);
  connect(m_Scene, SIGNAL(totalClear()), this, SLOT(totalClear()), Qt::AutoConnection);
  connect(m_Scene, SIGNAL(lineDrawn(CALine*)), this, SLOT(passLine(CALine*)), Qt::AutoConnection);
  connect(m_Scene, SIGNAL(caItemDrawn(CAGraphicsItem*)), this, SLOT(passCAItem(CAGraphicsItem*)), Qt::AutoConnection);
  connect(m_Scene, SIGNAL(zoom(int)), this, SLOT(zoom(int)));
  //if (!m_Client) m_Scene->installEventFilter(m_Scene);
  m_Scene->installEventFilter(m_Scene);
	m_Scene->setRadius(m_radius);
  m_Grid->recalculate(m_Grid->width(), m_Grid->height(), hMargin, vMargin, m_Grid->cellSize(), m_lnColour, m_lStyle);
  m_Grid->draw(m_Scene);
  m_Scene->setBackgroundBrush(m_bgColour);
  setScene(m_Scene);
  if (ar) resizeToFit();
}

void QGridView::resizeToFit()
{
  if (m_Scene && ((width()-m_tWidth != m_Scene->width()) || (height()-m_tHeight != m_Scene->height())))
  {
    for (QWidget *w = parentWidget(); w != 0; w = w->parentWidget())
    {
      if (w->objectName() == "MainWindow")
      {
        w->resize((int)(w->width()+(m_Scene->width()-width())), (int)(w->height()+(m_Scene->height()-height())));
        m_tWidth = (int)(width()-m_Scene->width());
        m_tHeight = (int)(height()-m_Scene->height());
        break;
      }
    }
  }
}

void QGridView::clearGrid(bool total, bool saveBackground)
{
  if (m_Scene) m_Scene->clearGrid(total, saveBackground);
}

void QGridView::setBackgroundColour(QColor c)
{
  if (m_Scene) m_Scene->setBackgroundColour(c);
  m_bgColour = c;
}

void QGridView::setLineStyle(int l)
{
  m_Grid->setType(l);
  m_lStyle = l;
}

void QGridView::setLineColour(QColor c)
{
  m_Grid->setColour(c);
  m_lnColour = c;
}

void QGridView::setCoordinateColour(QColor c)
{
  m_Grid->setCoordinateColour(c);
  m_coColour = c;
}

int QGridView::getColumnCount()
{
  return m_Grid->width();
}

int QGridView::getRowCount()
{
  return m_Grid->height();
}

int QGridView::getBoxSize()
{
  return m_Grid->cellSize();
}

int QGridView::getLineStyle()
{
  return m_Grid->type();
}

QColor QGridView::getLineColour()
{
  return m_Grid->lineColour();
}

QColor QGridView::getCoordinateColour()
{
  return m_Grid->coordColour();
}

void QGridView::setColumnCount(int c)
{
  m_Grid->setWidth(c);
  if (m_Scene) m_Scene->setColumnCount(c);
}

void QGridView::setRowCount(int c)
{
  m_Grid->setHeight(c);
  if (m_Scene) m_Scene->setRowCount(c);
}

void QGridView::setBoxSize(int s)
{
  m_Grid->setSize(s);
  if (m_Scene) m_Scene->setBoxSize(s);
}

QPointer<QGridScene> QGridView::getGridScene()
{
  if (m_Scene) return m_Scene;
  return 0;
}

void QGridView::drawCircle(int x, int y, QColor c, int size, QString letter)
{
  if (m_Scene) m_Scene->drawCircle(x, y, c, size, letter);
}

void QGridView::drawBlock(int x, int y, QColor c, QString letter)
{
  if (m_Scene) m_Scene->drawBlock(x, y, c, letter);
}

void QGridView::drawCross(int x, int y, QColor c)
{
  if (m_Scene) m_Scene->drawCross(x, y, c);
}

void QGridView::drawTriangle(int x, int y, QColor c, int facing)
{
  if (m_Scene) m_Scene->drawTriangle(x, y, c, facing);
}

void QGridView::drawLine(double x1, double y1, double x2, double y2, QPen pen)
{
  if (m_Scene) m_Scene->drawLine(x1, y1, x2, y2, pen);
}

void QGridView::drawCAItem(CAGraphicsItem *i)
{
  if (m_Scene) m_Scene->drawCAItem(i);
}

void QGridView::removeItem(int x, int y)
{
  if (m_Scene) m_Scene->undrawItem(x, y);
}

void QGridView::removeItem(quint16 check)
{
  if (m_Scene) m_Scene->undrawItem(check);
}

void QGridView::startDrag(int x, int y)
{
  if (m_Scene) m_Scene->startDrag(x, y);
}

void QGridView::finishDrag(int x, int y)
{
  if (m_Scene) m_Scene->finishDrag(x, y);
}

void QGridView::passCircle(int x, int y, QColor c, int size, QString letter)
{
  emit circleDrawn(x, y, c, size, letter);
}

void QGridView::passBlock(int x, int y, QColor c, QString letter)
{
  emit blockDrawn(x, y, c, letter);
}

void QGridView::passCross(int x, int y, QColor c)
{
  emit crossDrawn(x, y, c);
}

void QGridView::passTriangle(int x, int y, QColor c, int facing)
{
  emit triangleDrawn(x, y, c, facing);
}

void QGridView::passLine(double x1, double y1, double x2, double y2, QPen pen)
{
  emit lineDrawn(x1, y1, x2, y2, pen);
}

void QGridView::passLine(CALine *l)
{
  emit lineDrawn(l->startX(), l->startY(), l->endX(), l->endY(), l->pen());
}

void QGridView::passCAItem(CAGraphicsItem * i)
{
    emit caItemDrawn(i);
}

void QGridView::passRemove(int x, int y)
{
  emit itemRemoved(x, y);
}

void QGridView::passRemove(quint16 check)
{
  emit itemRemoved(check);
}

void QGridView::passFinishDrag(int x, int y)
{
  emit dragFinished(x, y);
}

void QGridView::passStartDrag(int x, int y)
{
  emit dragStarted(x, y);
}

void QGridView::setClient(bool client)
{
  m_Client = client;
  if (m_Scene) m_Scene->setClient(client);
}

void QGridView::setBackgroundByURI(QString uri, QString hash, int x, int y)
{
  if (uri.isNull() || uri.isEmpty())
  {
    clearBg();
    return;
  }
  QString fileName = findFileByHash(hash, "images/");
  if (hash.compare(tr("0")) == 0 || fileName.isEmpty())
  {
    QNetworkReply *reply = m_Manager->get(QNetworkRequest(QUrl(uri)));
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)), Qt::AutoConnection);
    m_Background = new CABackground(uri, CABackground::TypeRemote, 0, NULL, QPoint(x, y));
  }
  else
  {
    if (QPixmap(fileName).isNull()) return;
    QPixmap p = checkForPixmap(fileName);
    p = p.copy(QRect(0, 0, qMin(p.width(), (int)m_Scene->width()), qMin(p.height(), (int)m_Scene->height())));
    if (m_Scene)
    {
      clearBg();
      QGraphicsPixmapItem *i = m_Scene->addPixmap(p);
      i->setPos(m_Scene->hMargin()+x, m_Scene->vMargin()+y);
      i->setZValue(-2);
      m_Background = new CABackground(uri, CABackground::TypeRemote, hash, i, QPoint(x, y));
      emit newBackground(m_Background);
    }
    return;
  }
}

void QGridView::setBackgroundOffset(int x, int y)
{
  m_Background->setOffset(QPoint(x, y));
  if (m_Background->image()) m_Background->image()->setPos(m_Background->offset().x()+m_Scene->hMargin(), m_Background->offset().y()+m_Scene->vMargin());
  emit backgroundOffset(x, y);
}

void QGridView::setBackgroundByFile(QString fileName, int x, int y)
{
  if (fileName.isNull() || fileName.isEmpty())
  {
    bgFileName.clear();
    clearBg();
    return;
  }
  if (QPixmap(fileName).isNull()) return;
  QPixmap p = checkForPixmap(fileName);
  p = p.copy(QRect(0, 0, qMin(p.width(), (int)m_Scene->width()), qMin(p.height(), (int)m_Scene->height())));
  if (m_Scene)
  {
    clearBg();
    QGraphicsPixmapItem *i = m_Scene->addPixmap(p);
    i->setPos(m_Scene->hMargin()+x, m_Scene->vMargin()+y);
    i->setZValue(-2);
    m_Background = new CABackground(fileName, CABackground::TypeLocal, fileChecksum(fileName), i, QPoint(x, y));
    emit newBackground(m_Background);
  }
}

QPixmap QGridView::checkForPixmap(QString fileName)
{
  QDir dir("images/");
  dir.setFilter(QDir::Files|QDir::Hidden|QDir::NoSymLinks);
  dir.setSorting(QDir::Size|QDir::Reversed);
  if (dir.exists())
  {
    QString chksm = fileChecksum(fileName);
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
      QFileInfo fileInfo = list.at(i);
      if (chksm.compare(fileChecksum(fileInfo.filePath())) == 0)
      {
        bgFileName = fileInfo.absoluteFilePath();
        return QPixmap(bgFileName);
      }
    }
  }
  else QDir().mkdir("images/");
  QString newFileName(tr("images/%1%2").arg(epoch_s()).arg(((!QFileInfo(fileName).suffix().isEmpty())?"."+QFileInfo(fileName).suffix():"")));
  if (QFile::copy(fileName, newFileName))
  {
    QFileInfo fi(newFileName);
    bgFileName = fi.absoluteFilePath();
    return QPixmap(bgFileName);
  }
  return QPixmap(fileName);
}

void QGridView::totalClear()
{
  bgFileName.clear();
  clearBg();
}

void QGridView::clearBg()
{
  if (m_Background && m_Scene)
  {
    QGraphicsPixmapItem *i;
    if((i = m_Background->image()) != NULL && i->scene() && i->scene() == m_Scene) m_Scene->removeItem(i);
    delete m_Background;
    m_Background = new CABackground("");
  }
}

QGraphicsItem *QGridView::getBackgroundImage()
{
  if (m_Background) return m_Background->image();
  return NULL;
}

CABackground::CABackground(QString location, int type, QString hash, QGraphicsPixmapItem *img, QPoint o)
{
  m_Location = QString(location);
  m_Type = type;
  m_Hash = QString(hash);
  m_Image = img;
  m_Offset = QPoint(o.x(), o.y());
}

CABackground::~CABackground()
{
}

void CABackground::setHash(QString hash)
{
  m_Hash = QString(hash);
}

void CABackground::setImage(QGraphicsPixmapItem *i)
{
  m_Image = i;
}

void CABackground::setOffset(QPoint o)
{
  m_Offset = QPoint(o.x(), o.y());
}

void QGridView::downloadProgress(qint64 r, qint64 t)
{
  emit updateDownload(r, t);
}

void QGridView::downloadFinished(QNetworkReply *reply)
{
  if (reply->isFinished() && reply->error() == QNetworkReply::NoError)
  {
    QByteArray o = reply->readAll();
    QString oHash = QString(QCryptographicHash::hash(o, QCryptographicHash::Sha1).toHex().data());
    QString fileName = findFileByHash(oHash, "images/");
    if (!fileName.isEmpty())
    {
      if (QPixmap(fileName).isNull()) return;
      QPixmap p = checkForPixmap(fileName);
      p = p.copy(QRect(0, 0, qMin(p.width(), (int)m_Scene->width()), qMin(p.height(), (int)m_Scene->height())));
      if (m_Scene)
      {
        QGraphicsPixmapItem *i = m_Scene->addPixmap(p);
        i->setPos(m_Scene->hMargin(), m_Scene->vMargin());
        i->setZValue(-2);
        m_Background->setHash(oHash);
        m_Background->setImage(i);
        emit newBackground(m_Background);
      }
      return;
    }
    else
    {
      QString suffix("");
      QList<QNetworkReply::RawHeaderPair> headers = reply->rawHeaderPairs();
      for (int x = 0; x < headers.count(); x++)
      {
        if (QString(headers.at(x).first).compare(tr("Content-Type"), Qt::CaseInsensitive) == 0)
        {
          if (QString(headers.at(x).second).compare(tr("image/jpeg"), Qt::CaseInsensitive) == 0) suffix = tr("jpg");
          if (QString(headers.at(x).second).compare(tr("image/png"), Qt::CaseInsensitive) == 0) suffix = tr("png");
          if (QString(headers.at(x).second).compare(tr("image/gif"), Qt::CaseInsensitive) == 0) suffix = tr("gif");
          if (QString(headers.at(x).second).compare(tr("image/pjpeg"), Qt::CaseInsensitive) == 0) suffix = tr("jpg");
          if (QString(headers.at(x).second).compare(tr("image/svg+xml"), Qt::CaseInsensitive) == 0) suffix = tr("svg");
          if (QString(headers.at(x).second).compare(tr("image/tiff"), Qt::CaseInsensitive) == 0) suffix = tr("tiff");
          if (QString(headers.at(x).second).compare(tr("image/bmp"), Qt::CaseInsensitive) == 0) suffix = tr("bmp");
        }
      }
      if (!suffix.isEmpty())
      {
        QDir dir("images/");
        if (!dir.exists()) QDir().mkdir("images/");
        QString newFileName(tr("images/%1.%2").arg(epoch_s(), suffix));
        QFile *file = new QFile(newFileName);
        if (file->open(QIODevice::WriteOnly)) file->write(o);
        if (file->isOpen()) file->close();
        QPixmap p = checkForPixmap(newFileName);
        p = p.copy(QRect(0, 0, qMin(p.width(), (int)m_Scene->width()), qMin(p.height(), (int)m_Scene->height())));
        if (m_Scene)
        {
          QGraphicsPixmapItem *i = m_Scene->addPixmap(p);
          i->setPos(m_Scene->hMargin(), m_Scene->vMargin());
          i->setZValue(-2);
          m_Background->setHash(fileChecksum(newFileName));
          m_Background->setImage(i);
          emit newBackground(m_Background);
        }
      }
      else clearBg();
    }
  }
}

CAGrid::CAGrid(int c, int r, int hMargin, int vMargin, int s, QColor colour, int type, QFont cf, QColor cc)
{
  m_Columns = qMax(1, c);
  m_Rows = qMax(1, r);
  m_hMargin = hMargin;
  m_vMargin = vMargin;
  m_Size = qMax((int)CAGrid::Min_Size, qMin((int)CAGrid::Max_Size, s));
  m_Colour = colour;
  m_Type = type;
  m_coFont = cf;
  m_coColour = cc;
  calculate();
}

CAGrid::~CAGrid()
{
  for (int x = m_Lines.count(); x > 0; x--)
    delete m_Lines.takeLast();
}

void CAGrid::calculate()
{
  for (int x = m_Lines.count(); x > 0; x--)
    delete m_Lines.takeLast();
  for (int x = m_TopCoords.count(); x > 0; x--)
    delete m_TopCoords.takeLast();
  for (int x = m_BottomCoords.count(); x > 0; x--)
    delete m_BottomCoords.takeLast();
  for (int x = m_RightCoords.count(); x > 0; x--)
    delete m_RightCoords.takeLast();
  for (int x = m_LeftCoords.count(); x > 0; x--)
    delete m_LeftCoords.takeLast();
  for (int x = 0; x <= m_Columns; x++)
    m_Lines.append(new QGraphicsLineItem(x*(m_Size+1), 0, x*(m_Size+1), m_Rows*(m_Size+1)));
  for (int x = 0; x <= m_Rows; x++)
    m_Lines.append(new QGraphicsLineItem(0, x*(m_Size+1), m_Columns*(m_Size+1), x*(m_Size+1)));
  for (int x = 0; x < m_Lines.count(); x++)
  {
    QGraphicsLineItem *l = m_Lines.at(x);
    l->setPos(l->x()+m_hMargin, l->y()+m_vMargin);
    l->setPen(QPen(m_Colour, 0, (Qt::PenStyle)(m_Type)));
    l->setZValue(-1);
  }
  QStringList a;
  a << "A" << "B" << "C" << "D" << "E" << "F" << "G" << "H" << "I" << "J" << "K" << "L" << "M" << "N" << "O" << "P" << "Q" << "R" << "S" << "T" << "U" << "V" << "W" << "X" << "Y" << "Z" << "!A" << "!B" << "!C" << "!D" << "!E" << "!F" << "!G" << "!H" << "!I" << "!J" << "!K" << "!L" << "!M" << "!N" << "!O" << "!P" << "!Q" << "!R" << "!S" << "!T" << "!U" << "!V" << "!W" << "!X" << "!Y" << "!Z";
  for (int x = 0; x < m_Columns; x++)
  {
    QGraphicsSimpleTextItem *t = new QGraphicsSimpleTextItem(a[x]);
    t->setFont(m_coFont);
    t->setBrush(m_coColour);
    t->setZValue(-1);
    t->setPos(m_hMargin+(x*(m_Size+1)+((m_Size-t->boundingRect().width())/2)), m_vMargin-t->boundingRect().height());
    m_TopCoords.append(t);
  }
  for (int x = 0; x < m_Columns; x++)
  {
    QGraphicsSimpleTextItem *t = new QGraphicsSimpleTextItem(a[x]);
    t->setFont(m_coFont);
    t->setBrush(m_coColour);
    t->setZValue(-1);
    t->setPos(m_hMargin+(x*(m_Size+1)+((m_Size-t->boundingRect().width())/2)), (m_Rows*(m_Size+1))+m_vMargin+1);
    m_BottomCoords.append(t);
  }
  for (int x = 0; x < m_Rows; x++)
  {
    QGraphicsSimpleTextItem *t = new QGraphicsSimpleTextItem(QString::number(x+1));
    t->setFont(m_coFont);
    t->setBrush(m_coColour);
    t->setZValue(-1);
    t->setPos((m_Columns*(m_Size+1))+m_hMargin+3, m_vMargin+(x*(m_Size+1)+((m_Size-t->boundingRect().height())/2)));
    m_RightCoords.append(t);
  }
  for (int x = 0; x < m_Rows; x++)
  {
    QGraphicsSimpleTextItem *t = new QGraphicsSimpleTextItem(QString::number(x+1));
    t->setFont(m_coFont);
    t->setBrush(m_coColour);
    t->setZValue(-1);
    t->setPos(m_hMargin-t->boundingRect().width()-3, m_vMargin+(x*(m_Size+1)+((m_Size-t->boundingRect().height())/2)));
    m_LeftCoords.append(t);
  }
  while (!a.isEmpty())
  {
    a.first().clear();
    a.removeFirst();
  }
}

void CAGrid::recalculate(int c, int r, int hMargin, int vMargin, int s, QColor colour, int type, QColor cc, QFont cf)
{
  for (int x = m_Lines.count(); x > 0; x--)
    delete m_Lines.takeLast();
  for (int x = m_TopCoords.count(); x > 0; x--)
    delete m_TopCoords.takeLast();
  for (int x = m_BottomCoords.count(); x > 0; x--)
    delete m_BottomCoords.takeLast();
  for (int x = m_RightCoords.count(); x > 0; x--)
    delete m_RightCoords.takeLast();
  for (int x = m_LeftCoords.count(); x > 0; x--)
    delete m_LeftCoords.takeLast();
  m_Columns = qMax(1, c);
  m_Rows = qMax(1, r);
  m_hMargin = hMargin;
  m_vMargin = vMargin;
  m_Size = qMax((int)CAGrid::Min_Size, qMin((int)CAGrid::Max_Size, s));
  m_Colour = colour;
  m_Type = type;
  m_coColour = cc;
  m_coFont = cf;
  calculate();
}

void CAGrid::draw(QGridScene *scene)
{
  if (scene)
  {
    for (int x = 0; x < m_Lines.count(); x++)
      scene->addItem(m_Lines.at(x));
    for (int x = 0; x < m_TopCoords.count(); x++)
      scene->addItem(m_TopCoords.at(x));
    for (int x = 0; x < m_BottomCoords.count(); x++)
      scene->addItem(m_BottomCoords.at(x));
    for (int x = 0; x < m_RightCoords.count(); x++)
      scene->addItem(m_RightCoords.at(x));
    for (int x = 0; x < m_LeftCoords.count(); x++)
      scene->addItem(m_LeftCoords.at(x));
    //QRectF r = scene->sceneRect();
    scene->setSceneRect(0, 0, (m_Columns*m_Size)+m_Columns+(m_hMargin*2)+1, (m_Rows*m_Size)+m_Rows+(m_vMargin*2)+1);
    //QRectF r2 = scene->sceneRect();
    scene->setColumnCount(m_Columns);
    scene->setRowCount(m_Rows);
  }
}

void CAGrid::setType(int type)
{
  m_Type = type;
  modifyLines();
}

void CAGrid::setColour(QColor c)
{
  if (c.isValid())
  {
    m_Colour = c;
    modifyLines();
  }
}

void CAGrid::setCoordinateColour(QColor c)
{
  if (c.isValid())
  {
    m_coColour = c;
    modifyCoords();
  }
}

void CAGrid::modifyLines()
{
  for (int x = 0; x < m_Lines.count(); x++)
    m_Lines.at(x)->setPen(QPen(m_Colour, 0, (Qt::PenStyle)(m_Type)));
}

void CAGrid::modifyCoords()
{
  for (int x = 0; x < m_TopCoords.count(); x++)
    m_TopCoords.at(x)->setBrush(m_coColour);
  for (int x = 0; x < m_BottomCoords.count(); x++)
    m_BottomCoords.at(x)->setBrush(m_coColour);
  for (int x = 0; x < m_RightCoords.count(); x++)
    m_RightCoords.at(x)->setBrush(m_coColour);
  for (int x = 0; x < m_LeftCoords.count(); x++)
    m_LeftCoords.at(x)->setBrush(m_coColour);
}

void CAGrid::resize(int size)
{
  if (size) m_Size = size;
  int c = 0;
  int r = 0;
  for (int x = 0; x < m_Lines.count(); x++)
  {
    QGraphicsLineItem *l = m_Lines.at(x);
    QLineF line(l->line());
    if (line.angle() == 0)
    {
      line.setPoints(QPointF(0, c*(m_Size+1)), QPointF(m_Columns*(m_Size+1), c*(m_Size+1)));
      c++;
    }
    else
    {
      line.setPoints(QPointF(r*(m_Size+1), 0), QPointF(r*(m_Size+1), m_Rows*(m_Size+1)));
      r++;
    }
    l->setLine(line);
  }
  for (int x = 0; x < m_Columns; x++)
  {
    QGraphicsSimpleTextItem *t = m_TopCoords.at(x);
    t->setPos(m_hMargin+(x*(m_Size+1)+((m_Size-t->boundingRect().width())/2)), m_vMargin-t->boundingRect().height());
  }
  for (int x = 0; x < m_Columns; x++)
  {
    QGraphicsSimpleTextItem *t = m_BottomCoords.at(x);
    t->setPos(m_hMargin+(x*(m_Size+1)+((m_Size-t->boundingRect().width())/2)), (m_Rows*(m_Size+1))+m_vMargin+1);
  }
  for (int x = 0; x < m_Rows; x++)
  {
    QGraphicsSimpleTextItem *t = m_RightCoords.at(x);
    t->setPos((m_Columns*(m_Size+1))+m_hMargin+3, m_vMargin+(x*(m_Size+1)+((m_Size-t->boundingRect().height())/2)));
  }
  for (int x = 0; x < m_Rows; x++)
  {
    QGraphicsSimpleTextItem *t = m_LeftCoords.at(x);
    t->setPos(m_hMargin-t->boundingRect().width()-3, m_vMargin+(x*(m_Size+1)+((m_Size-t->boundingRect().height())/2)));
  }
}
