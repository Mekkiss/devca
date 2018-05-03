#ifndef OFFSETDIALOG_H
#define OFFSETDIALOG_H

#include <QDialog>
#include <QGraphicsItem>
#include "qgridview.h"

namespace Ui {
    class OffsetDialog;
}

class CABackground;
class CAGrid;

class OffsetDialog : public QDialog
{
    Q_OBJECT

public:
  explicit OffsetDialog(QWidget *parent = 0, CABackground *b = new CABackground(""), QPair<int,int> margins = QPair<int,int>(0,0), CAGrid *g = NULL);
  ~OffsetDialog();
  QGraphicsItem *bg() { return m_bg; }
  int bgX() const { return m_x; }
  int bgY() const { return m_y; }
  int xOffset() const { return m_xOffset; }
  int yOffset() const { return m_yOffset; }
  int cellSize() { return m_cSize; }

private slots:
  void on_hOffSlider_valueChanged(int value);
  void on_vOffSlider_valueChanged(int value);
  void on_hOffBox_valueChanged(int arg1);
  void on_vOffBox_valueChanged(int arg1);
  void on_cSizeSlider_valueChanged(int value);
  void on_cSizeBox_valueChanged(int arg1);

private:
  Ui::OffsetDialog *ui;
  QGraphicsItem *m_bg;
  int m_x;
  int m_y;
  int m_cSize;
  int m_xOffset;
  int m_yOffset;
  CAGrid *m_Grid;
};

#endif // OFFSETDIALOG_H
