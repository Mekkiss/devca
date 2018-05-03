#include <QColorDialog>
#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SettingsDialog)
{
    QSettings settings("ca.ini", QSettings::IniFormat);
    m_ui->setupUi(this);
    m_ui->automaticallyResizeCheckBox->setChecked(settings.value("autoResize", true).toBool());
    m_ui->overrideWindowStyleCheckBox->setChecked(settings.value("overrideStyle", true).toBool());
    m_ui->minimiseToTrayCheckBox->setChecked(settings.value("minimiseToTray", false).toBool());
    m_ui->maxHostsSpinBox->setValue(settings.value("maxHosts", 5).toInt());
    m_ui->disableSound->setChecked(settings.value("disableSound", false).toBool());
    m_ui->connectStartup->setChecked(settings.value("connectOnStartup", true).toBool());
    m_ui->webBackup->setChecked(settings.value("webBackup", true).toBool());
    m_ui->serverScript->setText(settings.value("serverScript", "http://www.pfrpg.com.au/devca.php5").toString());
}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
}

void SettingsDialog::changeEvent(QEvent *e)
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

bool SettingsDialog::getAutoResize()
{
  return m_ui->automaticallyResizeCheckBox->isChecked();
}

bool SettingsDialog::getOverrideStyle()
{
  return m_ui->overrideWindowStyleCheckBox->isChecked();
}

bool SettingsDialog::getMinimiseToTray()
{
  return m_ui->minimiseToTrayCheckBox->isChecked();
}

bool SettingsDialog::getConnectStartup()
{
  return m_ui->connectStartup->isChecked();
}

bool SettingsDialog::getDisableSound()
{
  return m_ui->disableSound->isChecked();
}

int SettingsDialog::getMaxHosts()
{
  return m_ui->maxHostsSpinBox->value();
}

bool SettingsDialog::getWebBackup()
{
  return m_ui->webBackup->isChecked();
}

QString SettingsDialog::getServerScript()
{
  QString w = m_ui->serverScript->text();
  if (w.isNull() || w.isEmpty()) w = QString("http://www.pfrpg.com.au/devca.php5");
  return w;
}

void SettingsDialog::on_buttonBox_clicked(QAbstractButton* button)
{
  if (m_ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole)
  {
    QSettings settings("ca.ini", QSettings::IniFormat);
    settings.setValue("autoResize", getAutoResize());
    settings.setValue("overrideStyle", getOverrideStyle());
    settings.setValue("minimiseToTray", getMinimiseToTray());
    settings.setValue("maxHosts", getMaxHosts());
    settings.setValue("disableSound", getDisableSound());
    settings.setValue("webBackup", getWebBackup());
    settings.setValue("serverScript", getServerScript());
  }
}
