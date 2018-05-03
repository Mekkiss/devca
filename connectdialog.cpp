#include "connectdialog.h"
#include "ui_connectdialog.h"

ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ConnectDialog)
{
    m_ui->setupUi(this);
    QSettings settings("ca.ini", QSettings::IniFormat);
    int maxHost = settings.value("maxHosts", 5).toInt();
    for (int x = 0; x < maxHost; x++)
    {
      QString s(settings.value("connect"+QString::number(x), QString()).toString());
      if (!s.isNull() && !s.isEmpty()) m_ui->addressComboBox->insertItem(1, s);
    }
    m_ui->usernameEdit->setText(settings.value("userName", QString()).toString());
    m_ui->showStartupCheckBox->setChecked(settings.value("connectOnStartup", true).toBool());
    m_ui->webCheckBox->setChecked(settings.value("useWebServer", false).toBool());
    m_ui->gmNickEdit->setText(settings.value("gmNick", QString()).toString());
    m_ui->ircNickEdit->setText(settings.value("ircNick", QString()).toString());
    //m_ui->ircServerEdit->setText(settings.value("ircServer", QString()).toString());
    //m_ui->ircChannelEdit->setText(settings.value("ircChannel", QString()).toString());
    if (!m_ui->usernameEdit->text().isEmpty()) m_ui->addressComboBox->setFocus();
}

ConnectDialog::~ConnectDialog()
{
    delete m_ui;
}

void ConnectDialog::changeEvent(QEvent *e)
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

QString ConnectDialog::getAddress()
{
  return m_ui->addressComboBox->currentText();
}

int ConnectDialog::getPort()
{
  return m_ui->portSpinBox->value();
}

QString ConnectDialog::getUserName()
{
  return m_ui->usernameEdit->text();
}

bool ConnectDialog::useWebServer()
{
  return m_ui->webCheckBox->isChecked();
}

bool ConnectDialog::useIRC()
{
    return m_ui->tabWidget->currentIndex() == 1;
   /* return !(m_ui->ircNickEdit->text().isEmpty()
            || m_ui->ircChannelEdit->currentText().isEmpty()
            || m_ui->ircServerEdit->currentText().isEmpty()
            || m_ui->gmNickEdit->text().isEmpty()); */
}

QString ConnectDialog::getIrcNick()
{
    return m_ui->ircNickEdit->text();
}

QString ConnectDialog::getIrcServer()
{
    return m_ui->ircServerEdit->currentText();
}

int ConnectDialog::getIrcPort()
{
    return 6667; //TODO: Replace with smarts
}

QString ConnectDialog::getIrcGM()
{
    return m_ui->gmNickEdit->text();
}

QString ConnectDialog::getIrcChannel()
{
    return m_ui->ircChannelEdit->currentText();
}

void ConnectDialog::on_connectButton_released()
{
  QSettings settings("ca.ini", QSettings::IniFormat);
  if (m_ui->addressComboBox->currentIndex() <= 0 && !m_ui->addressComboBox->currentText().isEmpty())
  {
    bool f = false;
    int maxHost = settings.value("maxHosts", 5).toInt();
    for (int x = 0; x < maxHost; x++)
    {
      QString s(settings.value("connect"+QString::number(x), QString()).toString());
      if ((s.isNull() || s.isEmpty()) && (f=true))
      {
        settings.setValue("connect"+QString::number(x), m_ui->addressComboBox->currentText());
        break;
      }
      else if (!s.compare(m_ui->addressComboBox->currentText()) && (f=true)) break;
    }
    if (!f)
    {
      for (int x = 0; x < maxHost-1; x++)
        settings.setValue("connect"+QString::number(x), settings.value("connect"+QString::number(x+1), QString()).toString());
      settings.setValue("connect"+QString::number(maxHost-1), m_ui->addressComboBox->currentText());
    }
  }
  settings.setValue("connectOnStartup", m_ui->showStartupCheckBox->isChecked());
  settings.setValue("useWebServer", m_ui->webCheckBox->isChecked());
  settings.setValue("gmNick", m_ui->gmNickEdit->text());
  settings.setValue("ircNick", m_ui->ircNickEdit->text());
  if (!m_ui->usernameEdit->text().isEmpty()) settings.setValue("userName", m_ui->usernameEdit->text());
  else settings.remove("userName");
  done(QDialog::Accepted);
}

void ConnectDialog::on_listenButton_released()
{
  QSettings settings("ca.ini", QSettings::IniFormat);
  settings.setValue("connectOnStartup", m_ui->showStartupCheckBox->isChecked());
  settings.setValue("useWebServer", m_ui->webCheckBox->isChecked());
  settings.setValue("gmNick", m_ui->gmNickEdit->text());
  settings.setValue("ircNick", m_ui->ircNickEdit->text());
  if (!m_ui->usernameEdit->text().isEmpty()) settings.setValue("userName", m_ui->usernameEdit->text());
  else settings.remove("userName");
  done(2);
}
