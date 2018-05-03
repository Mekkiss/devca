#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifndef WINDOWS
#ifdef WIN32
 #define WINDOWS
#elif WIN64
 #define WINDOWS
#endif
#endif /* WINDOWS */

#include <QtGui/QMainWindow>
#include <QSystemTrayIcon>
#include <QLabel>
#include <QProgressBar>
#include "qgridview.h"
#include "griddialog.h"
#include "settingsdialog.h"
#include "qgridscene.h"
#include "qgridtab.h"
#include "netdialog.h"
#include "connectdialog.h"
#include "cagraphicsitem.h"
#include "network.h"

namespace Ui
{
    class MainWindow;
}

class CANetwork;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    int m_sSidebar;
    int m_sColourbar;
    int m_cSize;
    int m_tDir;
    CANetwork *m_Network;
    QString soundChx;
    QPointer<QLabel> m_netLabel;
    QPointer<QProgressBar> m_progBar;
    QSystemTrayIcon *m_Tray;
    void changeEvent(QEvent *e);
    void loadFromFile(QPointer<QFile> file, int width = 0, int height = 0, int cSize = 0);
    void saveToFile(QPointer<QFile> file);

private slots:
    void ontabWidget_currentChanged(QWidget* );
    void on_actionPlay_Sound_triggered();
    void on_actionBackground_triggered();
    void on_actionClear_All_triggered();
    void on_action_Disconnect_triggered();
    void on_actionNetwork_List_triggered();
    void on_actionT_oolbar_toggled(bool checked);
    void on_actionS_tatus_Bar_toggled(bool checked);
    void on_action_Resync_triggered();
    void on_actionRename_Tab_triggered();
    void on_actionC_onnect_triggered();
    void on_actionPreferences_triggered();
    void on_cbarCustom2_toggled(bool checked);
    void on_cbarCustom1_toggled(bool checked);
    void on_action_Save_triggered();
    void on_actionWidget_Down_triggered();
    void on_actionWidget_Up_triggered();
    void on_action_Resize_Sidebar_triggered();
    void on_action_Save_Sidebar_triggered();
    void on_action_Load_Sidebar_triggered();
    void on_sideSaveButton_released();
    void on_sideLoadButton_released();
    void on_sideResizeButton_released();
    void on_actionGrid_Settings_triggered();
    void on_action_Clear_triggered();
    void on_cbarTriangle_customContextMenuRequested(QPoint pos);
    void on_addcharButton_toggled(bool checked);
    void on_cbarCircle_customContextMenuRequested(QPoint pos);
    void on_sideButton1_released();
    void on_sideButton2_released();
    void on_sideButton3_released();
    void on_sideButton4_released();
    void on_sideButton5_released();
    void on_sideButton6_released();
    void on_sideButton7_released();
    void on_sideButton8_released();
    void on_sideButton9_released();
    void on_sideButton10_released();
    void on_sideButton11_released();
    void on_sideButton12_released();
    void on_sideButton13_released();
    void on_sideButton14_released();
    void on_sideButton15_released();
    void on_sideButton16_released();
    void on_cbarBrown_toggled(bool checked);
    void on_cbarDYellow_toggled(bool checked);
    void on_cbarDCyan_toggled(bool checked);
    void on_cbarDMagenta_toggled(bool checked);
    void on_cbarDBlue_toggled(bool checked);
    void on_cbarDGreen_toggled(bool checked);
    void on_cbarDRed_toggled(bool checked);
    void on_cbarLGrey_2_toggled(bool checked);
    void on_cbarBlack_toggled(bool checked);
    void on_cbarOrange_toggled(bool checked);
    void on_cbarLYellow_toggled(bool checked);
    void on_cbarLCyan_toggled(bool checked);
    void on_cbarLMagenta_toggled(bool checked);
    void on_cbarLBlue_toggled(bool checked);
    void on_cbarLGreen_toggled(bool checked);
    void on_cbarLRed_toggled(bool checked);
    void on_cbarLGrey_toggled(bool checked);
    void on_cbarWhite_toggled(bool checked);
    void on_cbarTriangle_toggled(bool checked);
    void on_cbarLine_toggled(bool checked);
    void on_cbarCross_toggled(bool checked);
    void on_cbarSquare_toggled(bool checked);
    void on_cbarCircle_toggled(bool checked);
    void on_action_About_triggered();
    void on_action_Colour_Panel_triggered();
    void on_action_Sidebar_triggered();
    void on_action_Always_On_Top_triggered(bool checked);
    void on_action_Remove_Tab_triggered();
    void on_action_Add_Tab_triggered();
    void on_action_Load_triggered();
    void setSnap(bool checked);
    void setCSizeMedium(bool checked);
    void setCSizeLarge(bool checked);
    void setCSizeHuge(bool checked);
    void setCSizeGargantuan(bool checked);
    void setCSizeColossal(bool checked);
    void setTDirNorth(bool checked);
    void setTDirNortheast(bool checked);
    void setTDirEast(bool checked);
    void setTDirSoutheast(bool checked);
    void setTDirSouth(bool checked);
    void setTDirSouthwest(bool checked);
    void setTDirWest(bool checked);
    void setTDirNorthwest(bool checked);
    void showFromTray(QSystemTrayIcon::ActivationReason r);
    void on_cbarLine_customContextMenuRequested(QPoint pos);
    void setLThick(bool checked);
    void on_cbarRadius_toggled(bool checked);
    void on_cbarRadius_customContextMenuRequested(const QPoint &pos);
    void on_cbarCone_toggled(bool checked);
    void on_cbarMask_toggled(bool checked);
    void on_cbarCone_customContextMenuRequested(const QPoint &pos);
    void on_actionBackgroundOffset_triggered();
    void on_actionCancel_Download_triggered();
    void changeNetworkLabel(QString txt = QString(""));
    void clientConnected(bool c);
    void updateProgress(qint64 r, qint64 t);
    void on_tabWidget_currentChanged(int index);
};

int epoch();
QString epoch_s();
QString findFileByHash(QString hash, QString dirName);
bool filesIdentical(QString file1, QString file2);
QString fileChecksum(QString fileName);

#endif // MAINWINDOW_H
