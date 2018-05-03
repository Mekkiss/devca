/********************************************************************************
** Form generated from reading UI file 'caoffsetdialog.ui'
**
** Created: Fri 23. Dec 22:52:21 2011
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CAOFFSETDIALOG_H
#define UI_CAOFFSETDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_CAOffsetDialog
{
public:
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *CAOffsetDialog)
    {
        if (CAOffsetDialog->objectName().isEmpty())
            CAOffsetDialog->setObjectName(QString::fromUtf8("CAOffsetDialog"));
        CAOffsetDialog->resize(400, 300);
        buttonBox = new QDialogButtonBox(CAOffsetDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        retranslateUi(CAOffsetDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), CAOffsetDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), CAOffsetDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(CAOffsetDialog);
    } // setupUi

    void retranslateUi(QDialog *CAOffsetDialog)
    {
        CAOffsetDialog->setWindowTitle(QApplication::translate("CAOffsetDialog", "Dialog", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CAOffsetDialog: public Ui_CAOffsetDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CAOFFSETDIALOG_H
