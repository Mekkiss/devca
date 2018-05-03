#include <QLocale>
#include <QColorDialog>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QEvent>
#include <QMouseEvent>
#include <QFont>
#include <QBitmap>
#include <QSettings>
#include <QFile>
#include <QStringList>
#include <QTimer>
#include <QSound>
#include "mainwindow.h"
#include "aboutdialog.h"
#include "sounddialog.h"
#include "offsetdialog.h"
#include "ui_mainwindow.h"
#include "cagraphicsitem.h"
#include "cagraphicsitemcreator.h"

#define MAX_BLASTTEMPLATE_RADIUS 16
#define DEFAULT_RADIUS 4

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  connect(ui->actionE_xit, SIGNAL(triggered()), SLOT(close()), Qt::AutoConnection);
  connect(ui->tab1, SIGNAL(circleDrawn(int,int,QColor,int,QString)), ui->tabWidget, SLOT(passCircle(int,int,QColor,int,QString)), Qt::AutoConnection);
  connect(ui->tab1, SIGNAL(blockDrawn(int,int,QColor,QString)), ui->tabWidget, SLOT(passBlock(int,int,QColor,QString)), Qt::AutoConnection);
  connect(ui->tab1, SIGNAL(crossDrawn(int,int,QColor)), ui->tabWidget, SLOT(passCross(int,int,QColor)), Qt::AutoConnection);
  connect(ui->tab1, SIGNAL(triangleDrawn(int,int,QColor,int)), ui->tabWidget, SLOT(passTriangle(int,int,QColor,int)), Qt::AutoConnection);
  connect(ui->tab1, SIGNAL(itemRemoved(int,int)), ui->tabWidget, SLOT(passRemove(int,int)), Qt::AutoConnection);
  connect(ui->tab1, SIGNAL(itemRemoved(quint16)), ui->tabWidget, SLOT(passRemove(quint16)), Qt::AutoConnection);
  connect(ui->tab1, SIGNAL(dragStarted(int,int)), ui->tabWidget, SLOT(passStartDrag(int,int)), Qt::AutoConnection);
  connect(ui->tab1, SIGNAL(dragFinished(int,int)), ui->tabWidget, SLOT(passFinishDrag(int,int)), Qt::AutoConnection);
  connect(ui->tab1, SIGNAL(lineDrawn(double,double,double,double,QPen)), ui->tabWidget, SLOT(passLine(double,double,double,double,QPen)), Qt::AutoConnection);
  connect(ui->tab1, SIGNAL(caItemDrawn(CAGraphicsItem*)), ui->tabWidget, SLOT(passCAItem(CAGraphicsItem*)), Qt::AutoConnection);
  connect(ui->tab1, SIGNAL(newBackground(CABackground*)), ui->tabWidget, SLOT(passNewBackground(CABackground*)), Qt::AutoConnection);
  connect(ui->tab1, SIGNAL(updateDownload(qint64,qint64)), ui->tabWidget, SLOT(passUpdateDownload(qint64,qint64)), Qt::AutoConnection);
  connect(ui->tab1, SIGNAL(backgroundOffset(int,int)), ui->tabWidget, SLOT(passBackgroundOffset(int,int)), Qt::AutoConnection);
  m_Network = new CANetwork(ui->tabWidget, this);
  connect(m_Network, SIGNAL(changeNetworkLabel(QString)), this, SLOT(changeNetworkLabel(QString)), Qt::AutoConnection);
  connect(m_Network, SIGNAL(clientConnected(bool)), this, SLOT(clientConnected(bool)), Qt::AutoConnection);
  connect(m_Network, SIGNAL(updateProgress(qint64,qint64)), this, SLOT(updateProgress(qint64,qint64)), Qt::AutoConnection);
  m_sSidebar = 0;
  m_sColourbar = 0;
  m_cSize = 1;
  m_tDir = 0;
  m_Tray = new QSystemTrayIcon(QIcon(":/ca.xpm"), this);
  ui->tabWidget->clearCurrentGrid(true);
  ui->tabWidget->drawCurrentGrid();
  QMenu *m = new QMenu(this);
  m->addAction("Show", this, SLOT(show()));
  m->addAction("Hide", this, SLOT(hide()));
  m->addSeparator();
  m->addAction("Exit", this, SLOT(close()));
  m_Tray->setContextMenu(m);
  connect(m_Tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(showFromTray(QSystemTrayIcon::ActivationReason)), Qt::AutoConnection);
  m_netLabel = new QLabel(tr("Not connected"), ui->statusBar);
  m_netLabel->setIndent(4);
  ui->statusBar->addWidget(m_netLabel, 1);
  m_progBar = new QProgressBar(ui->statusBar);
  m_progBar->setFixedHeight(20);
  ui->statusBar->addWidget(m_progBar, 1);
  m_progBar->hide();
  ui->cbarWhite->changeColour(Qt::white);
  ui->cbarLGrey->changeColour(Qt::lightGray);
  ui->cbarLRed->changeColour(Qt::red);
  ui->cbarLGreen->changeColour(Qt::green);
  ui->cbarLBlue->changeColour(Qt::blue);
  ui->cbarLMagenta->changeColour(Qt::magenta);
  ui->cbarLCyan->changeColour(Qt::cyan);
  ui->cbarLYellow->changeColour(Qt::yellow);
  ui->cbarOrange->changeColour(QColor(255, 146, 7));
  ui->cbarBlack->changeColour(Qt::black);
  ui->cbarLGrey_2->changeColour(Qt::darkGray);
  ui->cbarDRed->changeColour(Qt::darkRed);
  ui->cbarDGreen->changeColour(Qt::darkGreen);
  ui->cbarDBlue->changeColour(Qt::darkBlue);
  ui->cbarDMagenta->changeColour(Qt::darkMagenta);
  ui->cbarDCyan->changeColour(Qt::darkCyan);
  ui->cbarDYellow->changeColour(Qt::darkYellow);
  ui->cbarBrown->changeColour(QColor(99, 56, 25));
  QSettings settings("ca.ini", QSettings::IniFormat);
  if (settings.value("connectOnStartup", true).toBool()) QTimer::singleShot(200, this, SLOT(on_actionC_onnect_triggered()));
	ui->tabWidget->setRadius(DEFAULT_RADIUS);
  QList<QAbstractButton*> btn = ui->buttonGroup->buttons();
  for (int x = 0; x < btn.count(); x++) ui->buttonGroup->setId(btn.at(x), x);
  QList<QAbstractButton*> btn2 = ui->buttonGroup_2->buttons();
  for (int x = 0; x < btn2.count(); x++) ui->buttonGroup_2->setId(btn2.at(x), x);
}

MainWindow::~MainWindow()
{
  delete m_Network;
  delete ui;
}

void MainWindow::showFromTray(QSystemTrayIcon::ActivationReason r)
{
  switch (r)
  {
    case QSystemTrayIcon::DoubleClick:
      m_Tray->hide();
      show();
      activateWindow();
      raise();
      showNormal();
      break;
    default:
      break;
  }
}

void MainWindow::changeEvent(QEvent *e)
{
  if (e->type() == QEvent::WindowStateChange && isMinimized())
  {
    QSettings settings("ca.ini", QSettings::IniFormat);
    if (settings.value("minimiseToTray", false).toBool())
    {
      e->accept();
      m_Tray->show();
      QTimer::singleShot(200, this, SLOT(hide()));
      return;
    }
  }
  e->ignore();
  QMainWindow::changeEvent(e);
}

void MainWindow::on_action_Load_triggered()
{
  QString fileName = QFileDialog::getOpenFileName(this, "Load Grid", QString(), "CAG files (*.cag)");
  if (fileName.isEmpty() || fileName.isNull()) return;

  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly|QIODevice::Text) || !file.isReadable()) return;

  loadFromFile(&file);
}

void MainWindow::loadFromFile(QPointer<QFile> file, int width, int height, int cSize)
{
  ui->tabWidget->loadFromFile(file, width, height, cSize);
}

void MainWindow::on_action_Save_triggered()
{
  QString fileName = QFileDialog::getSaveFileName(this, "Save Grid", QString(), "CAG files (*.cag)");
  if (fileName.isEmpty() || fileName.isNull()) return;

  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly|QIODevice::Text)) return;

  saveToFile(&file);
}

void MainWindow::saveToFile(QPointer<QFile> file)
{
  ui->tabWidget->saveToFile(file);
}

void MainWindow::on_action_Add_Tab_triggered()
{
  ui->tabWidget->newGrid();
  m_Network->tabAdded();
  m_Network->renameTab(ui->tabWidget->tabText(ui->tabWidget->currentIndex()), ui->tabWidget->currentIndex());
}

void MainWindow::on_action_Remove_Tab_triggered()
{
  m_Network->tabRemoved(ui->tabWidget->currentIndex());
  ui->tabWidget->delGrid(ui->tabWidget->currentIndex());
}

void MainWindow::on_action_Always_On_Top_triggered(bool checked)
{
  if (checked) setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint);
  else setWindowFlags(windowFlags()&~Qt::WindowStaysOnTopHint);
  show();
}

void MainWindow::on_action_Sidebar_triggered()
{
  QBoxLayout *qb = static_cast<QBoxLayout*>(ui->centralWidget->layout());
  int cWidth = width();
  if (m_sSidebar > 0)
  {
    setMinimumWidth(minimumWidth()+m_sSidebar);
    resize(cWidth+m_sSidebar, height());
    qb->addWidget(ui->frame_2);
    ui->frame_2->show();
    m_sSidebar = 0;
  }
  else
  {
    m_sSidebar = ui->frame_2->width();
    qb->removeItem(qb->itemAt(2));
    ui->frame_2->hide();
    setMinimumWidth(minimumWidth()-m_sSidebar);
    resize(cWidth-m_sSidebar, height());
  }
}

void MainWindow::on_action_Colour_Panel_triggered()
{
  QBoxLayout *qb = static_cast<QBoxLayout*>(ui->centralWidget->layout());
  int cWidth = width();
  if (m_sColourbar > 0)
  {
    setMinimumWidth(minimumWidth()+m_sColourbar);
    resize(cWidth+m_sColourbar, height());
    qb->insertWidget(0, ui->frame);
    ui->frame->show();
    m_sColourbar = 0;
  }
  else
  {
    m_sColourbar = ui->frame->width();
    qb->removeItem(qb->itemAt(0));
    ui->frame->hide();
    setMinimumWidth(minimumWidth()-m_sColourbar);
    resize(cWidth-m_sColourbar, height());
  }
}

void MainWindow::on_action_About_triggered()
{
  AboutDialog *d = new AboutDialog(this);
  d->exec();
}

void MainWindow::on_cbarCircle_toggled(bool checked)
{
  if (checked) ui->tabWidget->setShape(0);
}

void MainWindow::on_cbarSquare_toggled(bool checked)
{
  if (checked) ui->tabWidget->setShape(1);
}

void MainWindow::on_cbarCross_toggled(bool checked)
{
  if (checked) ui->tabWidget->setShape(2);
}

void MainWindow::on_cbarTriangle_toggled(bool checked)
{
  if (checked) ui->tabWidget->setShape(3);
}

void MainWindow::on_cbarLine_toggled(bool checked)
{
    if (checked) ui->tabWidget->setShape(5);
}

void MainWindow::on_cbarRadius_toggled(bool checked)
{
    if (checked) ui->tabWidget->setShape(6);
}

void MainWindow::on_cbarCone_toggled(bool checked)
{
    if (checked) ui->tabWidget->setShape(7);
}

void MainWindow::on_cbarMask_toggled(bool checked)
{
    if (checked) ui->tabWidget->setShape(8);
}

void MainWindow::on_cbarWhite_toggled(bool checked)
{
  if (checked) ui->tabWidget->setColour(ui->cbarWhite->getColour());
}

void MainWindow::on_cbarLGrey_toggled(bool checked)
{
  if (checked) ui->tabWidget->setColour(ui->cbarLGrey->getColour());
}

void MainWindow::on_cbarLRed_toggled(bool checked)
{
  if (checked) ui->tabWidget->setColour(ui->cbarLRed->getColour());
}

void MainWindow::on_cbarLGreen_toggled(bool checked)
{
  if (checked) ui->tabWidget->setColour(ui->cbarLGreen->getColour());
}

void MainWindow::on_cbarLBlue_toggled(bool checked)
{
  if (checked) ui->tabWidget->setColour(ui->cbarLBlue->getColour());
}

void MainWindow::on_cbarLMagenta_toggled(bool checked)
{
  if (checked) ui->tabWidget->setColour(ui->cbarLMagenta->getColour());
}

void MainWindow::on_cbarLCyan_toggled(bool checked)
{
  if (checked) ui->tabWidget->setColour(ui->cbarLCyan->getColour());
}

void MainWindow::on_cbarLYellow_toggled(bool checked)
{
  if (checked) ui->tabWidget->setColour(ui->cbarLYellow->getColour());
}

void MainWindow::on_cbarOrange_toggled(bool checked)
{
  if (checked) ui->tabWidget->setColour(ui->cbarOrange->getColour());
}

void MainWindow::on_cbarBlack_toggled(bool checked)
{
  if (checked) ui->tabWidget->setColour(ui->cbarBlack->getColour());
}

void MainWindow::on_cbarLGrey_2_toggled(bool checked)
{
  if (checked) ui->tabWidget->setColour(ui->cbarLGrey_2->getColour());
}

void MainWindow::on_cbarDRed_toggled(bool checked)
{
  if (checked) ui->tabWidget->setColour(ui->cbarDRed->getColour());
}

void MainWindow::on_cbarDGreen_toggled(bool checked)
{
  if (checked) ui->tabWidget->setColour(ui->cbarDGreen->getColour());
}

void MainWindow::on_cbarDBlue_toggled(bool checked)
{
  if (checked) ui->tabWidget->setColour(ui->cbarDBlue->getColour());
}

void MainWindow::on_cbarDMagenta_toggled(bool checked)
{
  if (checked) ui->tabWidget->setColour(ui->cbarDMagenta->getColour());
}

void MainWindow::on_cbarDCyan_toggled(bool checked)
{
  if (checked) ui->tabWidget->setColour(ui->cbarDCyan->getColour());
}

void MainWindow::on_cbarDYellow_toggled(bool checked)
{
  if (checked) ui->tabWidget->setColour(ui->cbarDYellow->getColour());
}

void MainWindow::on_cbarBrown_toggled(bool checked)
{
  if (checked) ui->tabWidget->setColour(ui->cbarBrown->getColour());
}

void MainWindow::on_cbarCustom1_toggled(bool checked)
{
  if (checked)
  {
    QColor c = QColorDialog::getColor(ui->cbarCustom1->getColour(), this);
    if (c.isValid())
    {
      ui->cbarCustom1->changeColour(c);
      ui->tabWidget->setColour(c);
    }
  }
}

void MainWindow::on_cbarCustom2_toggled(bool checked)
{
  if (checked)
  {
    QColor c = QColorDialog::getColor(ui->cbarCustom2->getColour(), this);
    if (c.isValid())
    {
      ui->cbarCustom2->changeColour(c);
      ui->tabWidget->setColour(c);
    }
  }
}

void MainWindow::on_sideButton1_released()
{
  QColorDialog *cd = new QColorDialog(this);
  QColor c = cd->getColor(Qt::black, this, "Choose new colour", QColorDialog::ShowAlphaChannel);
  ui->sideButton1->changeColour(c);
}

void MainWindow::on_sideButton2_released()
{
  QColorDialog *cd = new QColorDialog(this);
  QColor c = cd->getColor(Qt::black, this, "Choose new colour", QColorDialog::ShowAlphaChannel);
  ui->sideButton2->changeColour(c);
}

void MainWindow::on_sideButton3_released()
{
  QColorDialog *cd = new QColorDialog(this);
  QColor c = cd->getColor(Qt::black, this, "Choose new colour", QColorDialog::ShowAlphaChannel);
  ui->sideButton3->changeColour(c);
}

void MainWindow::on_sideButton4_released()
{
  QColorDialog *cd = new QColorDialog(this);
  QColor c = cd->getColor(Qt::black, this, "Choose new colour", QColorDialog::ShowAlphaChannel);
  ui->sideButton4->changeColour(c);
}

void MainWindow::on_sideButton5_released()
{
  QColorDialog *cd = new QColorDialog(this);
  QColor c = cd->getColor(Qt::black, this, "Choose new colour", QColorDialog::ShowAlphaChannel);
  ui->sideButton5->changeColour(c);
}

void MainWindow::on_sideButton6_released()
{
  QColorDialog *cd = new QColorDialog(this);
  QColor c = cd->getColor(Qt::black, this, "Choose new colour", QColorDialog::ShowAlphaChannel);
  ui->sideButton6->changeColour(c);
}

void MainWindow::on_sideButton7_released()
{
  QColorDialog *cd = new QColorDialog(this);
  QColor c = cd->getColor(Qt::black, this, "Choose new colour", QColorDialog::ShowAlphaChannel);
  ui->sideButton7->changeColour(c);
}

void MainWindow::on_sideButton8_released()
{
  QColorDialog *cd = new QColorDialog(this);
  QColor c = cd->getColor(Qt::black, this, "Choose new colour", QColorDialog::ShowAlphaChannel);
  ui->sideButton8->changeColour(c);
}

void MainWindow::on_sideButton9_released()
{
  QColorDialog *cd = new QColorDialog(this);
  QColor c = cd->getColor(Qt::black, this, "Choose new colour", QColorDialog::ShowAlphaChannel);
  ui->sideButton9->changeColour(c);
}

void MainWindow::on_sideButton10_released()
{
  QColorDialog *cd = new QColorDialog(this);
  QColor c = cd->getColor(Qt::black, this, "Choose new colour", QColorDialog::ShowAlphaChannel);
  ui->sideButton10->changeColour(c);
}

void MainWindow::on_sideButton11_released()
{
  QColorDialog *cd = new QColorDialog(this);
  QColor c = cd->getColor(Qt::black, this, "Choose new colour", QColorDialog::ShowAlphaChannel);
  ui->sideButton11->changeColour(c);
}

void MainWindow::on_sideButton12_released()
{
  QColorDialog *cd = new QColorDialog(this);
  QColor c = cd->getColor(Qt::black, this, "Choose new colour", QColorDialog::ShowAlphaChannel);
  ui->sideButton12->changeColour(c);
}

void MainWindow::on_sideButton13_released()
{
  QColorDialog *cd = new QColorDialog(this);
  QColor c = cd->getColor(Qt::black, this, "Choose new colour", QColorDialog::ShowAlphaChannel);
  ui->sideButton13->changeColour(c);
}

void MainWindow::on_sideButton14_released()
{
  QColorDialog *cd = new QColorDialog(this);
  QColor c = cd->getColor(Qt::black, this, "Choose new colour", QColorDialog::ShowAlphaChannel);
  ui->sideButton14->changeColour(c);
}

void MainWindow::on_sideButton15_released()
{
  QColorDialog *cd = new QColorDialog(this);
  QColor c = cd->getColor(Qt::black, this, "Choose new colour", QColorDialog::ShowAlphaChannel);
  ui->sideButton15->changeColour(c);
}

void MainWindow::on_sideButton16_released()
{
  QColorDialog *cd = new QColorDialog(this);
  QColor c = cd->getColor(Qt::black, this, "Choose new colour", QColorDialog::ShowAlphaChannel);
  ui->sideButton16->changeColour(c);
}

void MainWindow::setSnap(bool checked)
{
  if (checked) ui->tabWidget->setShape(5);
  else ui->tabWidget->setShape(4);
}

void MainWindow::setLThick(bool checked)
{
  if (checked) ui->tabWidget->setLThick(3);
  else ui->tabWidget->setLThick(1);
}

void MainWindow::on_cbarLine_customContextMenuRequested(QPoint pos)
{
  if (ui->tabWidget->getShape() != 4 && ui->tabWidget->getShape() != 5) return;
  QMenu m(ui->cbarLine);
  QStringList s;
  s << "Snap to grid" << "Thick";
  for (int x = 0; x < s.size(); x++)
  {
    QAction *a = m.addAction(s[x]);
    a->setCheckable(true);
    switch(x)
    {
      case 0:
        connect(a, SIGNAL(triggered(bool)), this, SLOT(setSnap(bool)), Qt::AutoConnection);
        if (ui->tabWidget->getShape() == 5) a->setChecked(true);
        break;
      case 1:
        connect(a, SIGNAL(triggered(bool)), this, SLOT(setLThick(bool)), Qt::AutoConnection);
        if (ui->tabWidget->getLThick() > 1) a->setChecked(true);
        break;
      default:
        break;
    }
  }
  m.exec(ui->cbarLine->mapToGlobal(pos));
}

void MainWindow::on_cbarCircle_customContextMenuRequested(QPoint pos)
{
  QMenu m(ui->cbarCircle);
  QStringList s;
  s << "Small" << "Medium" << "Large" << "Huge" << "Gargantuan" << "" << "Colossal";
  for (int x = 0; x < s.size(); x++)
  {
    if (QString(s[x]).isEmpty()) continue;
    QAction *a = m.addAction(s[x]);
    a->setCheckable(true);
    if (m_cSize == x) a->setChecked(true);
    switch(x)
    {
      case 0:
        a->setDisabled(true);
        break;
      case 1:
        connect(a, SIGNAL(triggered(bool)), this, SLOT(setCSizeMedium(bool)), Qt::AutoConnection);
        break;
      case 2:
        connect(a, SIGNAL(triggered(bool)), this, SLOT(setCSizeLarge(bool)), Qt::AutoConnection);
        break;
      case 3:
        connect(a, SIGNAL(triggered(bool)), this, SLOT(setCSizeHuge(bool)), Qt::AutoConnection);
        break;
      case 4:
        connect(a, SIGNAL(triggered(bool)), this, SLOT(setCSizeGargantuan(bool)), Qt::AutoConnection);
        break;
      case 6:
        connect(a, SIGNAL(triggered(bool)), this, SLOT(setCSizeColossal(bool)), Qt::AutoConnection);
        break;
      default: break;
    }
  }
  m.exec(ui->cbarCircle->mapToGlobal(pos));
}

void MainWindow::setCSizeMedium(bool checked)
{
  if (checked) ui->tabWidget->setCSize((m_cSize = 1));
}

void MainWindow::setCSizeLarge(bool checked)
{
  if (checked) ui->tabWidget->setCSize((m_cSize = 2));
}

void MainWindow::setCSizeHuge(bool checked)
{
  if (checked) ui->tabWidget->setCSize((m_cSize = 3));
}

void MainWindow::setCSizeGargantuan(bool checked)
{
  if (checked) ui->tabWidget->setCSize((m_cSize = 4));
}

void MainWindow::setCSizeColossal(bool checked)
{
  if (checked) ui->tabWidget->setCSize((m_cSize = 6));
}

void MainWindow::setTDirNorth(bool checked)
{
  if (checked) ui->tabWidget->setTDir((m_tDir = 0));
}

void MainWindow::setTDirNortheast(bool checked)
{
  if (checked) ui->tabWidget->setTDir((m_tDir = 1));
}

void MainWindow::setTDirEast(bool checked)
{
  if (checked) ui->tabWidget->setTDir((m_tDir = 2));
}

void MainWindow::setTDirSoutheast(bool checked)
{
  if (checked) ui->tabWidget->setTDir((m_tDir = 3));
}

void MainWindow::setTDirSouth(bool checked)
{
  if (checked) ui->tabWidget->setTDir((m_tDir = 4));
}

void MainWindow::setTDirSouthwest(bool checked)
{
  if (checked) ui->tabWidget->setTDir((m_tDir = 5));
}

void MainWindow::setTDirWest(bool checked)
{
  if (checked) ui->tabWidget->setTDir((m_tDir = 6));
}

void MainWindow::setTDirNorthwest(bool checked)
{
  if (checked) ui->tabWidget->setTDir((m_tDir = 7));
}

void MainWindow::on_addcharButton_toggled(bool checked)
{
  if (checked) ui->tabWidget->setAddChar(ui->addcharButton);
  else ui->tabWidget->setAddChar(NULL);
}

void MainWindow::on_cbarTriangle_customContextMenuRequested(QPoint pos)
{
  QMenu m(ui->cbarTriangle);
  QStringList s;
  s << "North" << "Northeast" << "East" << "Southeast" << "South" << "Southwest" << "West" << "Northwest";
  for (int x = 0; x < s.size(); x++)
  {
    QAction *a = m.addAction(s[x]);
    a->setCheckable(true);
    if (m_tDir == x) a->setChecked(true);
    switch(x)
    {
      case 0:
        connect(a, SIGNAL(triggered(bool)), this, SLOT(setTDirNorth(bool)), Qt::AutoConnection);
        break;
      case 1:
        connect(a, SIGNAL(triggered(bool)), this, SLOT(setTDirNortheast(bool)), Qt::AutoConnection);
        break;
      case 2:
        connect(a, SIGNAL(triggered(bool)), this, SLOT(setTDirEast(bool)), Qt::AutoConnection);
        break;
      case 3:
        connect(a, SIGNAL(triggered(bool)), this, SLOT(setTDirSoutheast(bool)), Qt::AutoConnection);
        break;
      case 4:
        connect(a, SIGNAL(triggered(bool)), this, SLOT(setTDirSouth(bool)), Qt::AutoConnection);
        break;
      case 5:
        connect(a, SIGNAL(triggered(bool)), this, SLOT(setTDirSouthwest(bool)), Qt::AutoConnection);
        break;
      case 6:
        connect(a, SIGNAL(triggered(bool)), this, SLOT(setTDirWest(bool)), Qt::AutoConnection);
        break;
      case 7:
        connect(a, SIGNAL(triggered(bool)), this, SLOT(setTDirNorthwest(bool)), Qt::AutoConnection);
        break;
      default: break;
    }
  }
  m.exec(ui->cbarTriangle->mapToGlobal(pos));
}

void MainWindow::on_cbarRadius_customContextMenuRequested(const QPoint &pos)
{
    (void)pos;
    QPointer<QGridScene> scene = QPointer<QGridScene>(ui->tabWidget->getCurrentGridScene());
    QInputDialog t(ui->cbarRadius);
    bool ok;
    int i = t.getInt(ui->cbarRadius, tr("Select radius"), tr("radius"), scene->getRadius(), 1, MAX_BLASTTEMPLATE_RADIUS, 1, &ok);
    if (ok) ui->tabWidget->setRadius(i);
}

void MainWindow::on_action_Clear_triggered()
{
  ui->tabWidget->clearCurrentGrid();
  m_Network->clearGrid(ui->tabWidget->currentIndex());
}

void MainWindow::on_actionGrid_Settings_triggered()
{
  QGridTab *t = ui->tabWidget;
  CAGrid *grid = t->getCurrentGrid();
  GridDialog *gd = new GridDialog(grid, t->getCurrentBackgroundColour(), t->getCurrentBackground(), this);
  if (gd->exec() == QDialog::Accepted)
  {
    // Local settings
    QSettings settings("ca.ini", QSettings::IniFormat);
    bool ar = settings.value("autoResize", true).toBool();
    QColor bgLocal = gd->getBgColourLocal();
    QColor lnLocal = gd->getLnColourLocal();
    QColor coLocal = gd->getCoColourLocal();
    QFont f = gd->getFont();
    if (bgLocal.isValid()) settings.setValue("grid/bgColour", bgLocal);
    if (lnLocal.isValid()) settings.setValue("grid/lnColour", lnLocal);
    if (coLocal.isValid()) settings.setValue("grid/coColour", coLocal);
    settings.setValue("grid/lineStyle", gd->getLineStyle());
    settings.setValue("grid/cellSize", gd->getCellSize());
    settings.setValue("grid/overrideHost", gd->getOverrideHost());
    settings.setValue("hMargin", gd->getHMargin());
    settings.setValue("vMargin", gd->getVMargin());
    settings.setValue("coFont", f.toString());
    if (gd->changed() && (QMessageBox::warning(this, "Grid Preferences Changed", "Changed grid preferences will not update unless the grid is redrawn. Redraw grid?", QMessageBox::Yes, QMessageBox::No))==QMessageBox::Yes)
    {
      if (!m_Network->client())
      {
        QTemporaryFile file("devca.XXXXXX");
        if (!file.open()) return;
        saveToFile(&file);
        t->drawCurrentGrid();
        file.seek(0);
        loadFromFile(&file);
      }
      m_Network->resync();
    }
    // Host grid settings
    CABackground *b = gd->getBackground();
    int newColumn = gd->getColumns();
    int newRow = gd->getRows();
    int newSize = gd->getBoxSize();
    QColor bg = gd->getBgColour();
    QColor ln = gd->getLnColour();
    QColor co = gd->getCoColour();
    int l = gd->getLineStyleHost();
    //NOTUSED QPoint o = t->getBackgroundOffset();
    t->setCurrentBackgroundByFile("");
    if (!gd->getBackgroundFile().isEmpty()) t->setCurrentBackgroundByFile(gd->getBackgroundFile(), b->offset().x(), b->offset().y());
    else if (!gd->getBackgroundURI().isEmpty())
    {
      if (gd->getBackgroundURI().compare(b->location(), Qt::CaseInsensitive) == 0 && !gd->getURIRefresh()) t->setCurrentBackgroundByURI(b->location(), b->hash(), b->offset().x(), b->offset().y());
      else t->setCurrentBackgroundByURI(gd->getBackgroundURI(), "0", b->offset().x(), b->offset().y());
    }
    if ((newSize != grid->cellSize() || newColumn != grid->width() || newRow != grid->height()) && !m_Network->client())
    {
      t->setBackgroundColour(bg);
      grid->setCoordinateColour(co);
      grid->setType(l);
      grid->setColour(ln);
      QGridScene *scene = t->getCurrentGridScene();
      QTemporaryFile file("devca.XXXXXX");
      if (!file.open()) return;
      saveToFile(&file);
      t->clearCurrentGrid(true, true);
      grid->resize(newSize);
      grid->recalculate(newColumn, newRow, grid->hMargin(), grid->vMargin(), newSize, ln, l);
      m_Network->redrawGrid(t->currentIndex(), grid->width(), grid->height(), grid->cellSize(), bg, ln, co, l);
      if (scene)
      {
        scene->resizeScene(newColumn, newRow, newSize);
        grid->draw(t->getCurrentGridScene());
        if (ar) ui->tabWidget->currentResizeToFit();
      }
      file.seek(0);
      loadFromFile(&file, newColumn, newRow, newSize);
    }
    else
    {
      t->setBackgroundColour(bg);
      grid->setCoordinateColour(co);
      grid->setType(l);
      grid->setColour(ln);
      m_Network->changeBackgroundColour(bg);
      m_Network->changeCoordinateColour(co);
      m_Network->changeLineStyle(l);
      m_Network->changeLineColour(ln);
    }
  }
}

void MainWindow::on_sideResizeButton_released()
{
  int size = ui->sideEdit1->width();
  bool ok;
  int newSize = QInputDialog::getInt(this, tr("Sidebar Size"), tr("Input new size:"), size, 100, 650, 30, &ok);
  if (ok && size != newSize)
  {
    int cWidth = width();
    int fWidth = ui->frame_2->width();
    ui->sideEdit1->setFixedWidth(newSize);
    ui->sideEdit2->setFixedWidth(newSize);
    ui->sideEdit3->setFixedWidth(newSize);
    ui->sideEdit4->setFixedWidth(newSize);
    ui->sideEdit5->setFixedWidth(newSize);
    ui->sideEdit6->setFixedWidth(newSize);
    ui->sideEdit7->setFixedWidth(newSize);
    ui->sideEdit8->setFixedWidth(newSize);
    ui->sideEdit9->setFixedWidth(newSize);
    ui->sideEdit10->setFixedWidth(newSize);
    ui->sideEdit11->setFixedWidth(newSize);
    ui->sideEdit12->setFixedWidth(newSize);
    ui->sideEdit13->setFixedWidth(newSize);
    ui->sideEdit14->setFixedWidth(newSize);
    ui->sideEdit15->setFixedWidth(newSize);
    ui->sideEdit16->setFixedWidth(newSize);
    ui->frame_2->setMinimumWidth(fWidth+(newSize-size));
    ui->frame_2->setMaximumWidth(fWidth+(newSize-size));
    setMinimumWidth(minimumWidth()+(newSize-size));
    resize(cWidth+(newSize-size), height());
    ui->sideLayout->setColumnMinimumWidth(0, newSize);
  }
}

void MainWindow::on_sideLoadButton_released()
{
  QString fileName = QFileDialog::getOpenFileName(this, "Load Sidebar", QString(), "CAS files (*.cas)");
  if (fileName.isEmpty() || fileName.isNull()) return;

  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) return;

  QTextStream in(&file);
  for (int x = 0; x < ui->sideLayout->rowCount() && !in.atEnd(); x++)
  {
    QString line = in.readLine();
    QLayoutItem *i =  ui->sideLayout->itemAtPosition(x, 0);
    if (i)
    {
      QLineEdit *e = static_cast<QLineEdit*>(i->widget());
      if (e) e->setText(line);
    }
    if (!in.atEnd())
    {
      QString line2 = in.readLine();
      QStringList rgb = line2.split(",");
      int r = ((rgb.size()>=1)?rgb[0].toInt():0);
      int g = ((rgb.size()>=2)?rgb[1].toInt():0);
      int b = ((rgb.size()>=3)?rgb[2].toInt():0);
      int a = ((rgb.size()>=4)?rgb[3].toInt():255);
      QColor c(r, g, b, a);
      QLayoutItem *i2 = ui->sideLayout->itemAtPosition(x, 1);
      if (i2 != 0)
      {
        QSideButton *b = static_cast<QSideButton*>(i2->widget());
        if (b) b->changeColour(c);
      }
    }
  }
}

void MainWindow::on_sideSaveButton_released()
{
  QString fileName = QFileDialog::getSaveFileName(this, "Save Sidebar", QString(), "CAS files (*.cas)");
  if (fileName.isEmpty() || fileName.isNull()) return;

  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly|QIODevice::Text)) return;

  QTextStream out(&file);
  for (int x = 0; x < ui->sideLayout->rowCount(); x++)
  {
    QLayoutItem *i =  ui->sideLayout->itemAtPosition(x, 0);
    QLayoutItem *i2 = ui->sideLayout->itemAtPosition(x, 1);
    if (i)
    {
      QLineEdit *e = static_cast<QLineEdit*>(i->widget());
      out << e->text() << "\n";
      if (i2)
      {
        QSideButton *b = static_cast<QSideButton*>(i2->widget());
        QColor c = b->getColour();
        out << QString::number(c.red()) << "," << QString::number(c.green()) << "," << QString::number(c.blue()) << "," << QString::number(c.alpha()) << "\n";
      }
    }
  }
}

void MainWindow::on_action_Load_Sidebar_triggered()
{
  on_sideLoadButton_released();
}

void MainWindow::on_action_Save_Sidebar_triggered()
{
  on_sideSaveButton_released();
}

void MainWindow::on_action_Resize_Sidebar_triggered()
{
  on_sideResizeButton_released();
}

void MainWindow::on_actionWidget_Up_triggered()
{
  int index = 0;
  for (int x = 0; x < ui->sideLayout->rowCount(); x++)
  {
    QLayoutItem *i;
    if ((i = ui->sideLayout->itemAtPosition(x, 0)))
    {
      QLineEdit *e = static_cast<QLineEdit*>(i->widget());
      if (e->hasFocus()) index = x;
    }
  }
  if (index > 0)
  {
    QList<QLayoutItem *>i, i2;
    i << ui->sideLayout->itemAtPosition(index, 0) << ui->sideLayout->itemAtPosition(index, 1);
    i2 << ui->sideLayout->itemAtPosition((index-1), 0) << ui->sideLayout->itemAtPosition((index-1), 1);
    if (i[0] && i[1] && i2[0] && i2[1])
    {
      for (int x = 0; x < i.size() && x < i2.size(); x++)
      {
        QRect dg = i2[x]->geometry();
        QRect sg = i[x]->geometry();
        i[x]->setGeometry(dg);
        i2[x]->setGeometry(sg);
        ui->frame_2->setTabOrder(i[x]->widget(), i2[x]->widget());
        ui->sideLayout->removeItem(i[x]);
        ui->sideLayout->removeItem(i2[x]);
        ui->sideLayout->addItem(i[x], (index-1), x);
        ui->sideLayout->addItem(i2[x], index, x);
      }
    }
  }
}

void MainWindow::on_actionWidget_Down_triggered()
{
  int index = ui->sideLayout->rowCount();
  for (int x = 0; x < ui->sideLayout->rowCount(); x++)
  {
    QLayoutItem *i;
    if ((i = ui->sideLayout->itemAtPosition(x, 0)))
    {
      QLineEdit *e = static_cast<QLineEdit*>(i->widget());
      if (e->hasFocus()) index = x;
    }
  }
  if (index < (ui->sideLayout->rowCount()-1))
  {
    QList<QLayoutItem *>i, i2;
    i << ui->sideLayout->itemAtPosition(index, 0) << ui->sideLayout->itemAtPosition(index, 1);
    i2 << ui->sideLayout->itemAtPosition((index+1), 0) << ui->sideLayout->itemAtPosition((index+1), 1);
    if (i[0] && i[1] && i2[0] && i2[1])
    {
      for (int x = 0; x < i.size() && x < i2.size(); x++)
      {
        QRect dg = i2[x]->geometry();
        QRect sg = i[x]->geometry();
        i[x]->setGeometry(dg);
        i2[x]->setGeometry(sg);
        ui->frame_2->setTabOrder(i2[x]->widget(), i[x]->widget());
        ui->sideLayout->removeItem(i[x]);
        ui->sideLayout->removeItem(i2[x]);
        ui->sideLayout->addItem(i[x], (index+1), x);
        ui->sideLayout->addItem(i2[x], index, x);
      }
    }
  }
}

void MainWindow::ontabWidget_currentChanged(QWidget* )
{
  QSettings settings("ca.ini", QSettings::IniFormat);
  bool ar = settings.value("autoResize", true).toBool();
  if (ar) ui->tabWidget->currentResizeToFit();
}

void MainWindow::on_actionPreferences_triggered()
{
  SettingsDialog *sd = new SettingsDialog(this);
  if (sd->exec() == QDialog::Accepted)
  {
    QSettings settings("ca.ini", QSettings::IniFormat);
    settings.setValue("autoResize", sd->getAutoResize());
    settings.setValue("overrideStyle", sd->getOverrideStyle());
    settings.setValue("minimiseToTray", sd->getMinimiseToTray());
    settings.setValue("maxHosts", sd->getMaxHosts());
    settings.setValue("disableSound", sd->getDisableSound());
    settings.setValue("connectOnStartup", sd->getConnectStartup());
    settings.setValue("webBackup", sd->getWebBackup());
    settings.setValue("serverScript", sd->getServerScript());
  }
}

void MainWindow::on_actionRename_Tab_triggered()
{
  QString txt = QInputDialog::getText(this, tr("Rename Tab"), tr("Input new name:"));
  if (!txt.isNull() && !txt.isEmpty())
  {
    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), txt.replace(',','.'));
    m_Network->renameTab(txt.replace(',','.'), ui->tabWidget->currentIndex());
  }
}

void MainWindow::on_action_Resync_triggered()
{
  m_Network->resync();
}

void MainWindow::on_actionS_tatus_Bar_toggled(bool checked)
{
  int cHeight = height();
  int h = ui->statusBar->height();
  if (checked && !ui->statusBar->isVisible())
  {
    setMinimumHeight(minimumHeight()+h);
    resize(width(), cHeight+h);
    ui->statusBar->show();
  }
  else if (ui->statusBar->isVisible())
  {
    ui->statusBar->hide();
    setMinimumHeight(minimumHeight()-h);
    resize(width(), cHeight-h);
  }
}

void MainWindow::on_actionT_oolbar_toggled(bool checked)
{
  int cHeight = height();
  int h = ui->mainToolBar->height();
  if (checked && !ui->mainToolBar->isVisible())
  {
    setMinimumHeight(minimumHeight()+h);
    resize(width(), cHeight+h);
    ui->mainToolBar->show();
  }
  else if (ui->mainToolBar->isVisible())
  {
    ui->mainToolBar->hide();
    setMinimumHeight(minimumHeight()-h);
    resize(width(), cHeight-h);
  }
}

void MainWindow::on_actionNetwork_List_triggered()
{
  if (m_Network->numClients() == 0) QMessageBox::information(this, "Network List", "There are no connected clients.");
  else
  {
    NetDialog *nd = new NetDialog(m_Network->clientNames(), m_Network->clientList(), this);
    connect(nd, SIGNAL(disconnectUser(int)), m_Network, SLOT(disconnectUser(int)), Qt::AutoConnection);
    connect(m_Network, SIGNAL(clientChange(QList<QString>,QList<CASock*>)), nd, SLOT(updateList(QList<QString>,QList<CASock*>)), Qt::AutoConnection);
    nd->exec();
  }
}

void MainWindow::on_action_Disconnect_triggered()
{
  m_Network->networkDisconnect();
}

void MainWindow::on_actionClear_All_triggered()
{
  ui->tabWidget->clearCurrentGrid(true);
  m_Network->clearGrid(ui->tabWidget->currentIndex(), true);
}

void MainWindow::on_actionBackground_triggered()
{
  QString fileName = QFileDialog::getOpenFileName(this, "Choose Background Image", QString(), "Images (*.png *.gif *.xpm *.bmp *.jpg *.jpeg *.pbm *.pgm *.ppm *.tiff *.xbm)");
  if (fileName.isNull() || fileName.isEmpty()) return;
  ui->tabWidget->setCurrentBackgroundByFile(fileName);
}

void MainWindow::on_actionPlay_Sound_triggered()
{
  SoundDialog *sDlg = new SoundDialog(this);
  QDir dir("sound/");

  dir.setFilter(QDir::Files|QDir::Hidden|QDir::NoSymLinks);
  dir.setSorting(QDir::Name);
  dir.setNameFilters(QStringList(tr("*.wav")));
  if (dir.exists())
  {
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
      QFileInfo fileInfo = list.at(i);
      sDlg->addSoundList(tr("%1 (%2 kB)").arg(fileInfo.fileName(), QString::number(fileInfo.size()/1024)), fileChecksum(fileInfo.filePath()));
    }
  }
  if (sDlg->numSounds() <= 0)
  {
    sDlg->disableSoundList();
    sDlg->disablePlayButton();
    sDlg->disableSendButton();
  }
  dir.setNameFilters(QStringList(tr("*.mp3")));
  if (dir.exists())
  {
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
      QFileInfo fileInfo = list.at(i);
      sDlg->addmp3SoundList(tr("%1 (%2 kB)").arg(fileInfo.fileName(), QString::number(fileInfo.size()/1024)), fileChecksum(fileInfo.filePath()));
    }
  }
  if (sDlg->numMP3s() <= 0)
  {
    sDlg->disableMP3List();
    sDlg->disableMP3PlayButton();
    sDlg->disableMP3SendButton();
    sDlg->disableMP3StopButton();
  }
  connect(sDlg, SIGNAL(sendSound(QString)), m_Network, SLOT(soundToClients(QString)), Qt::AutoConnection);
  sDlg->show();
  sDlg->raise();
  sDlg->activateWindow();
}

void MainWindow::on_cbarCone_customContextMenuRequested(const QPoint &pos)
{
  (void)pos;
  QStringList items;
  items << tr("North") << tr("Northeast") << tr("East") << tr("Southeast") << tr("South") << tr("Southwest") << tr("West") << tr("Northwest");

  bool ok;
  QString item = QInputDialog::getItem(this, tr("Select direction"), tr("direction"), items, m_tDir, false, &ok);
  if (ok && !item.isEmpty() && items.indexOf(item) >= 0)
  {
    ui->tabWidget->setTDir(((m_tDir = items.indexOf(item))));
    QPointer<QGridScene> scene = QPointer<QGridScene>(ui->tabWidget->getCurrentGridScene());
    QInputDialog t(ui->cbarRadius);
    int i = t.getInt(ui->cbarRadius, tr("Select radius"), tr("radius"), scene->getRadius(), 1, MAX_BLASTTEMPLATE_RADIUS, 1, &ok);
    if (ok) ui->tabWidget->setRadius(i);
  }
}

void MainWindow::on_actionBackgroundOffset_triggered()
{
  QSettings settings("ca.ini", QSettings::IniFormat);
  bool ar = settings.value("autoResize", true).toBool();
  QTemporaryFile file("devca.XXXXXX");
  if (!file.open()) return;
  saveToFile(&file);
  ui->tabWidget->clearCurrentGrid(true, true);
  CABackground *b = ui->tabWidget->getCurrentBackground();
  CAGrid *grid = ui->tabWidget->getCurrentGrid();
  OffsetDialog *d = new OffsetDialog(this, b, ui->tabWidget->getCurrentMargins(), grid);
  if (d->exec() == QDialog::Accepted)
  {
    file.seek(0);
    loadFromFile(&file, grid->width(), grid->height(), d->cellSize());
    ui->tabWidget->setCurrentBackgroundOffset(d->xOffset(), d->yOffset());
    if (ar) ui->tabWidget->currentResizeToFit();
  }
  else
  {
    file.seek(0);
    loadFromFile(&file);
  }
  if (file.open()) file.close();
}

void MainWindow::on_actionCancel_Download_triggered()
{
  m_Network->cancelDownload();
}

void MainWindow::changeNetworkLabel(QString txt)
{
  m_netLabel->setText(txt);
}

void MainWindow::on_actionC_onnect_triggered()
{
  ConnectDialog *cd = new ConnectDialog(this);
  cd->exec();
  //m_Network->processConnectDialog(cd->getUserName(), cd->getAddress(), (quint16)cd->getPort(), r, cd->useWebServer());
  m_Network->processConnectDialog(cd); //Cleaner.
}

void MainWindow::clientConnected(bool c)
{
  if (c)
  {
    on_cbarRadius_toggled(true);
    ui->buttonGroup_2->checkedButton()->setChecked(false);
    ui->cbarRadius->setChecked(true);
    ui->cbarCircle->setEnabled(false);
    ui->cbarCross->setEnabled(false);
    ui->cbarSquare->setEnabled(false);
    ui->cbarTriangle->setEnabled(false);
    ui->cbarLine->setEnabled(false);
    ui->addcharButton->setEnabled(false);
    ui->action_Add_Tab->setEnabled(false);
    ui->action_Remove_Tab->setEnabled(false);
    ui->action_Load->setEnabled(false);
    ui->action_Clear->setEnabled(false);
    ui->cbarMask->setEnabled(false);
    ui->actionCancel_Download->setText(tr("Cancel Download"));
    if (ui->frame->isVisible()) this->on_action_Colour_Panel_triggered();
    for (int x = 1; x < ui->tabWidget->count(); x++)
      ui->tabWidget->removeTab(x);
    ui->tabWidget->clearCurrentGrid(true, false);
    ui->tabWidget->setAllClient(true);
  }
  else
  {
    ui->cbarCircle->setEnabled(true);
    ui->cbarCross->setEnabled(true);
    ui->cbarOrange->setEnabled(true);
    ui->cbarSquare->setEnabled(true);
    ui->cbarTriangle->setEnabled(true);
    ui->cbarLine->setEnabled(true);
    ui->addcharButton->setEnabled(true);
    ui->action_Add_Tab->setEnabled(true);
    ui->action_Remove_Tab->setEnabled(true);
    ui->action_Load->setEnabled(true);
    ui->action_Clear->setEnabled(true);
    ui->cbarMask->setEnabled(true);
    ui->actionCancel_Download->setText(tr("Cancel Upload"));
    ui->tabWidget->setAllClient(false);
  }
}

void MainWindow::updateProgress(qint64 r, qint64 t)
{
  if (t == 0 || r == t)
  {
    m_progBar->reset();
    m_progBar->hide();
    return;
  }
  else if (r == 0 || !m_progBar->isVisible())
  {
    m_progBar->reset();
    m_progBar->setRange(0, t);
    m_progBar->show();
  }
  m_progBar->setValue(r);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
  QSettings settings("ca.ini", QSettings::IniFormat);
  if (settings.value("autoResize", true).toBool()) ui->tabWidget->resizeToFit(index);
}

// ORPHAN FUNCTIONS
int epoch()
{
  QDateTime epoch(QDate(1970, 1, 1), QTime(0, 0));
  return epoch.secsTo(QDateTime::currentDateTime());
}

QString epoch_s()
{
  return QString::number(epoch());
}

QString findFileByHash(QString hash, QString dirName)
{
  QDir dir(dirName);
  dir.setFilter(QDir::Files|QDir::Hidden|QDir::NoSymLinks);
  dir.setSorting(QDir::Size|QDir::Reversed);
  if (dir.exists())
  {
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
      QFileInfo fileInfo = list.at(i);
      if (hash.compare(fileChecksum(fileInfo.filePath())) == 0) return fileInfo.filePath();
    }
  }
  return QString("");
}

bool filesIdentical(QString file1, QString file2)
{
  if (fileChecksum(file1).compare(fileChecksum(file2)) == 0) return true;
  return false;
}

QString fileChecksum(QString fileName)
{
  if (fileName.isNull() || fileName.isEmpty()) return "0"; //Empty files has SHA of 0
  QByteArray checkData;
  QFile checkFile(fileName);
  checkFile.open(QIODevice::ReadOnly);
  checkData = checkFile.readAll();
  checkFile.close();
  QString o = QString(QCryptographicHash::hash(checkData, QCryptographicHash::Sha1).toHex().data());
  return o;
}
