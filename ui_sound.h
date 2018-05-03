/********************************************************************************
** Form generated from reading ui file 'sound.ui'
**
** Created: Tue 13. Jul 18:00:49 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_SOUND_H
#define UI_SOUND_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QToolButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SoundDialog
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QComboBox *soundList;
    QToolButton *playButton;
    QToolButton *sendButton;

    void setupUi(QDialog *SoundDialog)
    {
        if (SoundDialog->objectName().isEmpty())
            SoundDialog->setObjectName(QString::fromUtf8("SoundDialog"));
        SoundDialog->setWindowModality(Qt::NonModal);
        SoundDialog->resize(311, 135);
        buttonBox = new QDialogButtonBox(SoundDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(220, 100, 81, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Close);
        horizontalLayoutWidget = new QWidget(SoundDialog);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(10, 10, 291, 80));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        soundList = new QComboBox(horizontalLayoutWidget);
        soundList->setObjectName(QString::fromUtf8("soundList"));

        horizontalLayout->addWidget(soundList);

        playButton = new QToolButton(horizontalLayoutWidget);
        playButton->setObjectName(QString::fromUtf8("playButton"));

        horizontalLayout->addWidget(playButton);

        sendButton = new QToolButton(horizontalLayoutWidget);
        sendButton->setObjectName(QString::fromUtf8("sendButton"));

        horizontalLayout->addWidget(sendButton);


        retranslateUi(SoundDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SoundDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SoundDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(SoundDialog);
    } // setupUi

    void retranslateUi(QDialog *SoundDialog)
    {
        SoundDialog->setWindowTitle(QApplication::translate("SoundDialog", "Play Sound", 0, QApplication::UnicodeUTF8));
        playButton->setText(QApplication::translate("SoundDialog", "...", 0, QApplication::UnicodeUTF8));
        sendButton->setText(QApplication::translate("SoundDialog", "...", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(SoundDialog);
    } // retranslateUi

};

namespace Ui {
    class SoundDialog: public Ui_SoundDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SOUND_H
