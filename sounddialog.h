#ifndef SOUNDDIALOG_H
#define SOUNDDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class SoundDialog;
}

class SoundDialog : public QDialog
{
  Q_OBJECT
public:
    SoundDialog(QWidget *parent = 0);
    void addSoundList(QString fileName, QString chx);
    void addmp3SoundList(QString fileName, QString chx);
    int numSounds();
    int numMP3s();
    void disableMP3List();
    void disableSoundList();
    void disablePlayButton();
    void disableSendButton();
    void disableMP3PlayButton();
    void disableMP3SendButton();
    void disableMP3StopButton();

private:
    Ui::SoundDialog *m_ui;
    void playSound(QString chx);
    void playMP3(QString chx);

private slots:
    void on_mp3StopButton_released();
    void on_mp3SendButton_released();
    void on_mp3PlayButton_released();
    void on_sendButton_released();
    void on_playButton_released();

signals:
    void sendSound(QString chx);
    void stopSound();
};
QString fileChecksum(QString fileName);
#endif // SOUNDDIALOG_H
