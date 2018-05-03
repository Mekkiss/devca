#ifndef GRIDDIALOG_H
#define GRIDDIALOG_H

#include <QtGui/QDialog>
#include <QAbstractButton>
#include "qgridview.h"

namespace Ui {
    class GridDialog;
}

class CABackground;

class GridDialog : public QDialog {
    Q_OBJECT
public:
    GridDialog(CAGrid *grid, QColor bg = Qt::black, CABackground *background = NULL, QWidget *parent = 0);
    ~GridDialog();
    void setBgColourButton(QColor c);
    void setLnColourButton(QColor c);
    void setCoColourButton(QColor c);
    QColor getBgColour() {return m_bgColour;}
    QColor getLnColour() {return m_lnColour;}
    QColor getCoColour() {return m_coColour;}
    int getColumns();
    int getRows();
    int getBoxSize();
    QString getBackgroundFile();
    QString getBackgroundURI();
    CABackground *getBackground() {return m_Background;}
    void setBgColourButtonLocal(QColor c);
    void setLnColourButtonLocal(QColor c);
    void setCoColourButtonLocal(QColor c);
    QColor getBgColourLocal() {return m_bgColourLocal;}
    QColor getLnColourLocal() {return m_lnColourLocal;}
    QColor getCoColourLocal() {return m_coColourLocal;}
    bool getOverrideHost();
    bool getURIRefresh();
    int getLineStyle();
    int getLineStyleHost();
    int getCellSize();
    int getHMargin();
    int getVMargin();
    QFont getFont();
    bool changed();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::GridDialog *m_ui;
    int m_Columns;
    int m_Rows;
    int m_Boxs;
    QColor m_bgColour;
    QColor m_lnColour;
    QColor m_coColour;
    QColor m_bgColourLocal;
    QColor m_lnColourLocal;
    QColor m_coColourLocal;
    QString m_fontFamily;
    CABackground *m_Background;
    int m_fontSize;
    QColor iBG;
    QColor iLN;
    QColor iCO;
    QString iFont;
    int iStyle;
    int ihMargin;
    int ivMargin;
    int iCellSize;
    bool bOverrideHost;

private slots:
    void on_bgImageBrowseButton_released();
    void on_coColourButton_released();
    void on_lnColourButton_released();
    void on_bgColourButton_released();
    void on_buttonBox_clicked(QAbstractButton* button);
    void on_coColourButtonLocal_released();
    void on_lnColourButtonLocal_released();
    void on_bgColourButtonLocal_released();
    void on_tabWidget_currentChanged(int index);
    void on_bgURIEdit_textEdited(const QString &arg1);
    void on_bgImageEdit_textEdited(const QString &arg1);
};

#endif // GRIDDIALOG_H
