#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QAbstractButton>
#include <QSettings>
#include <QtGui/QDialog>
#include <QFontDialog>

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    bool getAutoResize();
    bool getOverrideStyle();
    bool getMinimiseToTray();
    bool getDisableSound();
    bool getConnectStartup();
    int getMaxHosts();
    bool getWebBackup();
    QString getServerScript();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SettingsDialog *m_ui;

private slots:
    void on_buttonBox_clicked(QAbstractButton* button);
};

#endif // SETTINGSDIALOG_H
