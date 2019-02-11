/**
 * Monitoring Program
 * Copyright (C) 2018 Kilian Leport
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QTabWidget>
#include <QToolBar>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>

#include "udpdiscover.h"
#include "logviewerwidget.h"
#include "connectionhandler.h"
#include "virtualdevice.h"
#include "virtualdeviceitemmodel.h"
#include "virtualfieldlistwidget.h"
#include "device.h"
#include "devicelistwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    static LogViewerWidget *logViewerWidgetTab;

private:
    void createActions();
    void createMenus();
    void createCentralZone();
    void createToolBar();
    void createStatusBar();
    void addTab(QWidget *widget, QAction *action, const QString &label);

    bool save();
    bool saveAs();
    void openRecentFile();
    void open();

    bool saveFile(const QString &fileName);    
    void loadFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);

    void closeTab(int tab);
    void showHideTab(bool checked);
    void clientCountChanged(int clientCount);
    void timerOut();
    void updateStateMachine();
    void updateVirtualField();
    void updateRecentFileActions();
    void orderReceived(Device::OrderStructure orderReceived);
    void ackFromVirtualDeviceItemModel(VirtualDevice::AckStructure ackReceived);
    void readSettings();
    void writeSettings();
    void closeEvent(QCloseEvent *event) override;

private:
    //QActions
    QAction *actionExit;
    QAction *actionSave;
    QAction *actionSaveAs;
    QAction *actionOpen;
    QAction *separatorRecentFile;
    QAction *actionShowLogTab;
    QAction *actionShowVirtualFieldTab;
    QAction *actionShowDeviceListTab;
    enum { MaxRecentFiles = 3 };
    QAction *recentFile[MaxRecentFiles];
    //Menu Bar
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuView;
    //Central Zone
    QTabWidget *centralZoneWidget;
    VirtualFieldListWidget *virtualFieldListWidgetTab;
    VirtualFieldListWidget *virtualFieldListWidgetTab2;
    DeviceListWidget *deviceListWidgetTab;
    QHash<QWidget*, QAction*> tabQWidgetQActionHash;
    QHash<QAction*, QWidget*> tabQActionQWidgetHash;
    QHash<QWidget*, QString> tabQWidgetQStringHash;
    //Tool Bar
    QToolBar *toolBar;
    QLabel *numberOfConnection;
    QLabel *answerTimeMaximumLabel;
    //enum { MaxAnswerTime = 10000, MinAnswerTime = 500 };
    QSpinBox *answerTimeMaximumSpinBox;

    //Device
    DeviceItemModel *deviceItemModel;

    //network
    ConnectionHandler *connectionHandler;

    //virtual field
    VirtualDeviceItemModel *virtualDeviceItemModel;

    //Communication between device and client
    QQueue<Device::OrderStructure> queueOrderPending;
    Device::OrderStructure currentOrderPending;
    VirtualDevice::AckStructure currentAckPending;
    QTimer *timerAnswer;

    //StateMachine
    enum State
    {
        OrderToSend,
        AckReceived
    };
    State stateMachine;
    bool orderReceivedFlag;
    bool ackFromVirtualFlag;
    bool timerFinishedBeforeAck;

    //handle file
    QString curFile;
};
#endif // MAINWINDOW_H
