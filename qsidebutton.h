#ifndef QSIDEBUTTON_H
#define QSIDEBUTTON_H

#include <QToolButton>
#include <QPainter>

class QSideButton : public QToolButton
{
public:
    QSideButton(QWidget *parent = 0);
    void changeColour(QColor c);
    QColor getColour() {return m_Colour;}

private:
    QColor m_Colour;
};

class QColourButton : public QToolButton
{
public:
    QColourButton(QWidget *parent = 0);
    void changeColour(QColor c);
    QColor getColour() {return m_Colour;}

private:
    QColor m_Colour;
};

#endif // QSIDEBUTTON_H
