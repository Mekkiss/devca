#include <QColorDialog>
#include <QFileDialog>
#include <QPushButton>
#include "griddialog.h"
#include "ui_griddialog.h"

GridDialog::GridDialog(CAGrid *grid, QColor bg, CABackground *background, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::GridDialog)
{
    m_ui->setupUi(this);
    // Host settings
    m_Columns = grid->width();
    m_Rows = grid->height();
    m_Boxs = grid->cellSize();
    m_bgColour = bg;
    m_lnColour = grid->lineColour();
    m_coColour = grid->coordColour();
    m_ui->columnEdit->setValue(m_Columns);
    m_ui->rowEdit->setValue(m_Rows);
    m_ui->boxEdit->setValue(m_Boxs);
    if (background != NULL)
    {
      m_Background = new CABackground(background->location(), background->type(), background->hash(), background->image(), background->offset());
      if (m_Background->type() == CABackground::TypeLocal) m_ui->bgImageEdit->setText(m_Background->location());
      else m_ui->bgURIEdit->setText(m_Background->location());
    }
    else m_Background = new CABackground("");
    setBgColourButton(m_bgColour);
    setLnColourButton(m_lnColour);
    setCoColourButton(m_coColour);
    m_ui->lineStyleComboBoxHost->setCurrentIndex(grid->type());
    // Local settings
    QSettings settings("ca.ini", QSettings::IniFormat);
    iBG = (m_bgColourLocal = settings.value("grid/bgColour", Qt::black).value<QColor>());
    iLN = (m_lnColourLocal = settings.value("grid/lnColour", Qt::white).value<QColor>());
    iCO = (m_coColourLocal = settings.value("grid/coColour", Qt::yellow).value<QColor>());
    setBgColourButtonLocal(m_bgColourLocal);
    setLnColourButtonLocal(m_lnColourLocal);
    setCoColourButtonLocal(m_coColourLocal);
    m_ui->lineStyleComboBox->setCurrentIndex((iStyle = settings.value("grid/lineStyle", Qt::DotLine).toInt()));
    m_ui->cellSizeSpinBox->setValue((iCellSize = settings.value("grid/cellSize", 20).toInt()));
    m_ui->overrideHostCheckBox->setChecked((bOverrideHost = settings.value("grid/overrideHost", false).toBool()));
    QString coFont = (iFont = settings.value("coFont", QFont("Courier New", 8)).toString());
    m_ui->coFontComboBox->setCurrentFont(coFont);
    m_ui->horizontalMarginSpinBox->setValue((ihMargin = settings.value("hMargin", 20).toInt()));
    m_ui->verticalMarginSpinBox->setValue((ivMargin = settings.value("vMargin", 15).toInt()));
    QFont f;
    if (!f.fromString(coFont)) f = QFont("Courier New", 8);
    int i = m_ui->coSizeComboBox->findText(QString::number(f.pointSize()));
    if (i > -1) m_ui->coSizeComboBox->setCurrentIndex(i);
    QPushButton *b = m_ui->buttonBox->button(QDialogButtonBox::Apply);
    if (b) b->setDisabled(true);
}

GridDialog::~GridDialog()
{
    delete m_ui;
}

void GridDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void GridDialog::setBgColourButton(QColor c)
{
  QPixmap px = QPixmap(16, 16);
  px.fill(QColor(0, 0, 0, 0));
  QPainter p(&px);
  p.setBrush(QBrush(c, Qt::SolidPattern));
  p.setPen(c);
  p.setBackgroundMode(Qt::OpaqueMode);
  p.setBackground(QBrush(c, Qt::SolidPattern));
  p.drawRect(0, 0, 14, 14);
  QIcon i(px);
  m_ui->bgColourButton->setIcon(i);
}

void GridDialog::setLnColourButton(QColor c)
{
  QPixmap px = QPixmap(16, 16);
  px.fill(QColor(0, 0, 0, 0));
  QPainter p(&px);
  p.setBrush(QBrush(c, Qt::SolidPattern));
  p.setPen(c);
  p.setBackgroundMode(Qt::OpaqueMode);
  p.setBackground(QBrush(c, Qt::SolidPattern));
  p.drawRect(0, 0, 14, 14);
  QIcon i(px);
  m_ui->lnColourButton->setIcon(i);
}

void GridDialog::setCoColourButton(QColor c)
{
  QPixmap px = QPixmap(16, 16);
  px.fill(QColor(0, 0, 0, 0));
  QPainter p(&px);
  p.setBrush(QBrush(c, Qt::SolidPattern));
  p.setPen(c);
  p.setBackgroundMode(Qt::OpaqueMode);
  p.setBackground(QBrush(c, Qt::SolidPattern));
  p.drawRect(0, 0, 14, 14);
  QIcon i(px);
  m_ui->coColourButton->setIcon(i);
}

void GridDialog::on_bgColourButton_released()
{
  QColorDialog *d = new QColorDialog(m_bgColour, this);
  QColor c = d->getColor(m_bgColour, this);
  if (c.isValid()) setBgColourButton((m_bgColour=c));
}

void GridDialog::on_lnColourButton_released()
{
  QColorDialog *d = new QColorDialog(m_lnColour, this);
  QColor c = d->getColor(m_lnColour, this);
  if (c.isValid()) setLnColourButton((m_lnColour=c));
}

void GridDialog::on_coColourButton_released()
{
  QColorDialog *d = new QColorDialog(m_coColour, this);
  QColor c = d->getColor(m_coColour, this);
  if (c.isValid()) setCoColourButton((m_coColour=c));
}

int GridDialog::getColumns()
{
  int c = m_ui->columnEdit->value();
  if (c < 1 || c > 52) return m_Columns;
  return (m_Columns=c);
}

int GridDialog::getRows()
{
  int c = m_ui->rowEdit->value();
  if (c < 1 || c > 52) return m_Rows;
  return (m_Rows=c);
}

int GridDialog::getBoxSize()
{
  int c = m_ui->boxEdit->value();
  if (c < 15 || c > 99) return m_Boxs;
  return (m_Boxs=c);
}

QString GridDialog::getBackgroundFile()
{
  return m_ui->bgImageEdit->text();
}

QString GridDialog::getBackgroundURI()
{
  return m_ui->bgURIEdit->text();
}

void GridDialog::on_bgImageBrowseButton_released()
{
  m_ui->bgURIEdit->clear();
  QString fileName = QFileDialog::getOpenFileName(this, "Choose Background Image", QString(), "Images (*.png *.gif *.xpm *.bmp *.jpg *.jpeg *.pbm *.pgm *.ppm *.tiff *.xbm)");
  if (fileName.isNull() || fileName.isEmpty()) return;
  m_ui->bgImageEdit->setText(fileName);
}

void GridDialog::on_bgImageEdit_textEdited(const QString &arg1)
{
  m_ui->bgURIEdit->clear();
}

void GridDialog::setBgColourButtonLocal(QColor c)
{
  QPixmap px = QPixmap(16, 16);
  px.fill(QColor(0, 0, 0, 0));
  QPainter p(&px);
  p.setBrush(QBrush(c, Qt::SolidPattern));
  p.setPen(c);
  p.setBackgroundMode(Qt::OpaqueMode);
  p.setBackground(QBrush(c, Qt::SolidPattern));
  p.drawRect(0, 0, 14, 14);
  QIcon i(px);
  m_ui->bgColourButtonLocal->setIcon(i);
}

void GridDialog::setLnColourButtonLocal(QColor c)
{
  QPixmap px = QPixmap(16, 16);
  px.fill(QColor(0, 0, 0, 0));
  QPainter p(&px);
  p.setBrush(QBrush(c, Qt::SolidPattern));
  p.setPen(c);
  p.setBackgroundMode(Qt::OpaqueMode);
  p.setBackground(QBrush(c, Qt::SolidPattern));
  p.drawRect(0, 0, 14, 14);
  QIcon i(px);
  m_ui->lnColourButtonLocal->setIcon(i);
}

void GridDialog::setCoColourButtonLocal(QColor c)
{
  QPixmap px = QPixmap(16, 16);
  px.fill(QColor(0, 0, 0, 0));
  QPainter p(&px);
  p.setBrush(QBrush(c, Qt::SolidPattern));
  p.setPen(c);
  p.setBackgroundMode(Qt::OpaqueMode);
  p.setBackground(QBrush(c, Qt::SolidPattern));
  p.drawRect(0, 0, 14, 14);
  QIcon i(px);
  m_ui->coColourButtonLocal->setIcon(i);
}

void GridDialog::on_bgColourButtonLocal_released()
{
  QColorDialog *d = new QColorDialog(m_bgColourLocal, this);
  QColor c = d->getColor(m_bgColourLocal, this);
  if (c.isValid()) setBgColourButtonLocal((m_bgColourLocal=c));
}

void GridDialog::on_lnColourButtonLocal_released()
{
  QColorDialog *d = new QColorDialog(m_lnColourLocal, this);
  QColor c = d->getColor(m_lnColourLocal, this);
  if (c.isValid()) setLnColourButtonLocal((m_lnColourLocal=c));
}

void GridDialog::on_coColourButtonLocal_released()
{
  QColorDialog *d = new QColorDialog(m_coColourLocal, this);
  QColor c = d->getColor(m_coColourLocal, this);
  if (c.isValid()) setCoColourButtonLocal((m_coColourLocal=c));
}

bool GridDialog::getOverrideHost()
{
  return m_ui->overrideHostCheckBox->isChecked();
}

bool GridDialog::getURIRefresh()
{
  return m_ui->bgURIRefresh->isChecked();
}

int GridDialog::getLineStyleHost()
{
  return m_ui->lineStyleComboBoxHost->currentIndex();
}

int GridDialog::getLineStyle()
{
  return m_ui->lineStyleComboBox->currentIndex();
}

int GridDialog::getCellSize()
{
  return m_ui->cellSizeSpinBox->value();
}

int GridDialog::getHMargin()
{
  return m_ui->horizontalMarginSpinBox->value();
}

int GridDialog::getVMargin()
{
  return m_ui->verticalMarginSpinBox->value();
}

QFont GridDialog::getFont()
{
  QFont f(m_ui->coFontComboBox->currentFont().family(), m_ui->coSizeComboBox->currentText().toInt());
  return f;
}

bool GridDialog::changed()
{
  if (iBG != getBgColourLocal()) return true;
  if (iLN != getLnColourLocal()) return true;
  if (iCO != getCoColourLocal()) return true;
  if (iFont != getFont().toString()) return true;
  if (iStyle != getLineStyle()) return true;
  if (bOverrideHost != getOverrideHost()) return true;
  if (iCellSize != getCellSize()) return true;
  if (ihMargin != getHMargin()) return true;
  if (ivMargin != getVMargin()) return true;
  return false;
}

void GridDialog::on_buttonBox_clicked(QAbstractButton* button)
{
  if (m_ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole)
  {
    QSettings settings("ca.ini", QSettings::IniFormat);
    QColor bg = getBgColourLocal();
    QColor ln = getLnColourLocal();
    QColor co = getCoColourLocal();
    QFont f = getFont();
    if (bg.isValid()) settings.setValue("grid/bgColour", bg);
    if (ln.isValid()) settings.setValue("grid/lnColour", ln);
    if (co.isValid()) settings.setValue("grid/coColour", co);
    settings.setValue("grid/lineStyle", getLineStyle());
    settings.setValue("grid/cellSize", getCellSize());
    settings.setValue("grid/overrideHost", getOverrideHost());
    settings.setValue("hMargin", getHMargin());
    settings.setValue("vMargin", getVMargin());
    settings.setValue("coFont", f.toString());
  }
}

void GridDialog::on_tabWidget_currentChanged(int index)
{
  QPushButton *b = m_ui->buttonBox->button(QDialogButtonBox::Apply);
  if (b)
  {
    if (index == 1) b->setDisabled(false);
    else b->setDisabled(true);
  }
}

void GridDialog::on_bgURIEdit_textEdited(const QString &arg1)
{
  m_ui->bgImageEdit->clear();
}
