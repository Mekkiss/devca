#include <QSound>
#include <QDir>
#ifdef WINDOWS
#include <windows.h>
#endif /* WIN32 */
#include "sounddialog.h"
#include "ui_sounddialog.h"

SoundDialog::SoundDialog(QWidget *parent) : QDialog(parent),
    m_ui(new Ui::SoundDialog)
{
  m_ui->setupUi(this);
  m_ui->stopButton->setDisabled(true);
}

void SoundDialog::addSoundList(QString fileName, QString chx)
{
  m_ui->soundList->addItem(fileName, QVariant().fromValue(chx));
}

void SoundDialog::addmp3SoundList(QString fileName, QString chx)
{
  m_ui->mp3SoundList->addItem(fileName, QVariant().fromValue(chx));
}

void SoundDialog::on_playButton_released()
{
  playSound(m_ui->soundList->itemData(m_ui->soundList->currentIndex()).toString());
}

void SoundDialog::on_sendButton_released()
{
  playSound(m_ui->soundList->itemData(m_ui->soundList->currentIndex()).toString());
  emit sendSound(m_ui->soundList->itemData(m_ui->soundList->currentIndex()).toString());
}

void SoundDialog::playSound(QString chx)
{
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
      if (fileChecksum(fileInfo.filePath()).compare(chx) == 0)
      {
#ifdef WINDOWS
        QSound s(fileInfo.filePath(), this);
        s.play();
#endif
        break;
      }
    }
  }
}

void SoundDialog::playMP3(QString chx)
{
  QDir dir("sound/");

  dir.setFilter(QDir::Files|QDir::Hidden|QDir::NoSymLinks);
  dir.setSorting(QDir::Name);
  dir.setNameFilters(QStringList(tr("*.mp3")));
  if (dir.exists())
  {
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
      QFileInfo fileInfo = list.at(i);
      if (fileChecksum(fileInfo.filePath()).compare(chx) == 0)
      {
#ifdef WINDOWS
        wchar_t *wch = NULL;
        QString("open "+fileInfo.filePath()+" type mpegvideo alias file").toWCharArray(wch);
        mciSendString(wch, NULL, 0,0);
        mciSendStringA("play file", NULL, 0, 0);
#endif /*WIN32 */
        break;
      }
    }
  }
}

void SoundDialog::on_mp3PlayButton_released()
{
  playMP3(m_ui->mp3SoundList->itemData(m_ui->mp3SoundList->currentIndex()).toString());
}

void SoundDialog::on_mp3SendButton_released()
{
  playMP3(m_ui->mp3SoundList->itemData(m_ui->mp3SoundList->currentIndex()).toString());
  emit sendSound(m_ui->mp3SoundList->itemData(m_ui->mp3SoundList->currentIndex()).toString());
}

void SoundDialog::on_mp3StopButton_released()
{
#ifdef WINDOWS
  mciSendStringA("close file", NULL, 0,0);
#endif /* WIN32 */
  emit stopSound();
}

int SoundDialog::numSounds()
{
  return m_ui->soundList->count();
}

int SoundDialog::numMP3s()
{
  return m_ui->mp3SoundList->count();
}

void SoundDialog::disableMP3List()
{
  m_ui->mp3SoundList->setDisabled(true);
}

void SoundDialog::disableSoundList()
{
  m_ui->soundList->setDisabled(true);
}

void SoundDialog::disablePlayButton()
{
  m_ui->playButton->setDisabled(true);
}

void SoundDialog::disableSendButton()
{
  m_ui->sendButton->setDisabled(true);
}

void SoundDialog::disableMP3PlayButton()
{
  m_ui->mp3PlayButton->setDisabled(true);
}

void SoundDialog::disableMP3SendButton()
{
  m_ui->mp3SendButton->setDisabled(true);
}

void SoundDialog::disableMP3StopButton()
{
  m_ui->mp3StopButton->setDisabled(true);
}
