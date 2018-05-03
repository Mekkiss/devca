#include "offsetdialog.h"
#include "ui_offsetdialog.h"

OffsetDialog::OffsetDialog(QWidget *parent, CABackground *b, QPair<int, int> margins, CAGrid *g) :
    QDialog(parent),
    ui(new Ui::OffsetDialog)
{
  ui->setupUi(this);
  m_bg = b->image();
  m_xOffset = b->offset().x();
  m_yOffset = b->offset().y();
  m_cSize = 0;
  m_x = margins.first;
  m_y = margins.second;
  ui->hOffSlider->setValue(m_xOffset);
  ui->hOffBox->setValue(m_xOffset);
  ui->vOffSlider->setValue(m_yOffset);
  ui->vOffBox->setValue(m_yOffset);
  m_Grid = g;
  if (m_Grid)
  {
    ui->cSizeSlider->setValue((m_cSize=g->cellSize()));
    ui->cSizeBox->setValue(m_cSize);
  }
  else
  {
    ui->cSizeSlider->setValue(20);
    ui->cSizeBox->setValue(20);
  }
}

OffsetDialog::~OffsetDialog()
{
    delete ui;
}

void OffsetDialog::on_hOffBox_valueChanged(int arg1)
{
  if (arg1 != m_xOffset)
  {
    ui->hOffSlider->setValue((m_xOffset=arg1));
    if (m_bg) m_bg->setX(m_x+m_xOffset);
  }
}

void OffsetDialog::on_vOffBox_valueChanged(int arg1)
{
  if (arg1 != m_yOffset)
  {
    ui->vOffSlider->setValue((m_yOffset=arg1));
    if (m_bg) m_bg->setY(m_y+m_yOffset);
  }
}

void OffsetDialog::on_hOffSlider_valueChanged(int value)
{
  if (value != m_xOffset)
  {
    ui->hOffBox->setValue((m_xOffset=value));
    if (m_bg) m_bg->setX(m_x+m_xOffset);
  }
}

void OffsetDialog::on_vOffSlider_valueChanged(int value)
{
  if (value != m_yOffset)
  {
    ui->vOffBox->setValue((m_yOffset=value));
    if (m_bg) m_bg->setY(m_y+m_yOffset);
  }
}

void OffsetDialog::on_cSizeSlider_valueChanged(int value)
{
  if (value != m_cSize)
  {
    ui->cSizeBox->setValue((m_cSize=value));
    if (m_Grid) m_Grid->resize(m_cSize);
  }
}

void OffsetDialog::on_cSizeBox_valueChanged(int arg1)
{
  if (arg1 != m_cSize)
  {
    ui->cSizeSlider->setValue((m_cSize=arg1));
    if (m_Grid) m_Grid->resize(m_cSize);
  }
}

