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
#include "mainwindow.h"

LogViewerWidget * MainWindow::logViewerWidgetTab = nullptr;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent){
    connectionHandler = new ConnectionHandler(this);
    connect(connectionHandler, &ConnectionHandler::frameTCPOrder, this, &MainWindow::orderReceived);
    connect(connectionHandler, &ConnectionHandler::clientCountChanged, this,&MainWindow::clientCountChanged);
    stateMachine = OrderToSend;
    orderReceivedFlag = false;
    ackFromVirtualFlag = false;
    timerFinishedBeforeAck = false;
    timerAnswer = new QTimer(this);
    timerAnswer->setSingleShot(true);
    connect(timerAnswer, &QTimer::timeout, this, &MainWindow::timerOut);

    createActions();
    createMenus();
    createCentralZone();
    createToolBar();
    createStatusBar();

    //properties of the main window
    setMinimumSize(750, 750);
    setWindowFlags(Qt::Window);
    setWindowIcon(QIcon(":/icons/server.svg"));
    setWindowTitle(tr("Serveur"));

    readSettings();

    updateVirtualField();
}
void MainWindow::createActions(){
    //QActions
    actionExit = new QAction(tr("&Quit"), this);
    actionExit->setShortcut(QKeySequence("Ctrl+Q"));
    actionExit->setIcon(QIcon(":/icons/quit.svg"));
    connect(actionExit, &QAction::triggered, this, &MainWindow::close);
    actionSave = new QAction(tr("&Save"), this);
    actionSave->setShortcut(QKeySequence("Ctrl+S"));
    actionSave->setIcon(QIcon(":/icons/save.svg"));
    connect(actionSave, &QAction::triggered, this, &MainWindow::save);
    actionSaveAs = new QAction(tr("Save &As..."), this);
    actionSaveAs->setShortcut(QKeySequence("Ctrl+Shift+S"));
    actionSaveAs->setIcon(QIcon(":/icons/save-as.svg"));
    connect(actionSaveAs, &QAction::triggered, this, &MainWindow::saveAs);
    actionOpen = new QAction(tr("&Open virtual field"), this);
    actionOpen->setShortcut(QKeySequence("Ctrl+O"));
    actionOpen->setIcon(QIcon(":/icons/open.svg"));
    connect(actionOpen, &QAction::triggered, this, &MainWindow::open);
    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFile[i] = new QAction(this);
        recentFile[i]->setVisible(true);
        connect(recentFile[i], &QAction::triggered, this, &MainWindow::openRecentFile);
    }
    actionShowLogTab = new QAction(tr("Show &log"), this);
    actionShowLogTab->setCheckable(true);
    actionShowLogTab->setChecked(true);
    actionShowLogTab->setShortcut(QKeySequence("Ctrl+L"));
    connect(actionShowLogTab, &QAction::triggered, this, &MainWindow::showHideTab);
    //connect(actionShowLogTab, &QAction::triggered, [=] { showHideTab(2, actionShowLogTab->isChecked()); });
    actionShowVirtualFieldTab = new QAction(tr("Show &VirtualField"), this);
    actionShowVirtualFieldTab->setCheckable(true);
    actionShowVirtualFieldTab->setChecked(true);
    actionShowVirtualFieldTab->setShortcut(QKeySequence("Ctrl+V"));
    connect(actionShowVirtualFieldTab, &QAction::triggered, this, &MainWindow::showHideTab);
    actionShowDeviceListTab = new QAction(tr("Show &Device Table Server"), this);
    actionShowDeviceListTab->setCheckable(true);
    actionShowDeviceListTab->setChecked(true);
    actionShowDeviceListTab->setShortcut(QKeySequence("Ctrl+D"));
    connect(actionShowDeviceListTab, &QAction::triggered, this, &MainWindow::showHideTab);
}
void MainWindow::createMenus(){
    //QMenus
    menuFile = menuBar()->addMenu(tr("&File"));
    //menuEdit = menuBar()->addMenu(tr("&Edit"));
    menuView = menuBar()->addMenu(tr("&View"));

    menuFile->addAction(actionSave);
    menuFile->addAction(actionSaveAs);
    menuFile->addAction(actionOpen);
    menuFile->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        menuFile->addAction(recentFile[i]);
    separatorRecentFile = menuFile->addSeparator();
    menuFile->addAction(actionExit);

    menuView->addAction(actionShowVirtualFieldTab);
    menuView->addAction(actionShowDeviceListTab);
    menuView->addAction(actionShowLogTab);

    updateRecentFileActions();
}
void MainWindow::createCentralZone(){
    centralZoneWidget = new QTabWidget(this);
    logViewerWidgetTab = new LogViewerWidget(this);

    deviceItemModel = new DeviceItemModel(this);
    deviceListWidgetTab = new DeviceListWidget(deviceItemModel, this);

    virtualDeviceItemModel = new VirtualDeviceItemModel(this);
    virtualFieldListWidgetTab = new VirtualFieldListWidget(virtualDeviceItemModel, this);
    connect(virtualDeviceItemModel, &VirtualDeviceItemModel::answerVirtualDevice,
            this, &MainWindow::ackFromVirtualDeviceItemModel);

    addTab(virtualFieldListWidgetTab, actionShowVirtualFieldTab, tr("Liste Device Virtual Field"));
    addTab(deviceListWidgetTab, actionShowDeviceListTab, tr("Liste Device Server"));
    addTab(logViewerWidgetTab, actionShowLogTab, tr("log"));

    connect(centralZoneWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    centralZoneWidget->setMovable(true);
    centralZoneWidget->setTabsClosable(true);

    setCentralWidget(centralZoneWidget);
}
void MainWindow::createToolBar(){
    numberOfConnection = new QLabel(tr("no client connected"), this);
    answerTimeMaximumLabel = new QLabel(tr("Maximum answer time:"), this);
    answerTimeMaximumSpinBox = new QSpinBox(this);
    answerTimeMaximumSpinBox->setRange(VirtualDevice::MinAnswerTime, VirtualDevice::MaxAnswerTime);
    answerTimeMaximumSpinBox->setSingleStep(100);
    answerTimeMaximumSpinBox->setValue(2000);
    answerTimeMaximumSpinBox->setSuffix("ms");
    connect(answerTimeMaximumSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            virtualDeviceItemModel, &VirtualDeviceItemModel::setTimeToReachMaximum);
    connect(answerTimeMaximumSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            virtualFieldListWidgetTab, &VirtualFieldListWidget::setMaximumTimeToReach);

    toolBar = addToolBar(tr("&File"));
    toolBar->addAction(actionExit);
    toolBar->addAction(actionSave);
    toolBar->addAction(actionSaveAs);
    toolBar->addAction(actionOpen);
    toolBar->addSeparator();
    toolBar->addWidget(numberOfConnection);
    toolBar->addSeparator();
    toolBar->addWidget(answerTimeMaximumLabel);
    toolBar->addWidget(answerTimeMaximumSpinBox);
}
void MainWindow::createStatusBar(){
    if (connectionHandler->isServerStarted())
        statusBar()->showMessage(tr("The server started"));
    else
        statusBar()->showMessage(tr("The server didn't start") );
}
void MainWindow::addTab(QWidget *widget, QAction *action, const QString &label){
    tabQWidgetQActionHash.insert(widget, action);
    tabQActionQWidgetHash.insert(action, widget);
    tabQWidgetQStringHash.insert(widget, label);
    centralZoneWidget->addTab(widget, label);
}
bool MainWindow::save(){
    if(curFile.isEmpty()){
        return saveAs();
    }else{
        return saveFile(curFile);
    }
}
bool MainWindow::saveAs(){
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Virtual Field"), "",
                                                    tr("Virtual Field (*.vfi);;All Files (*)"));
    if(fileName.isEmpty())
        return false;
    else{
        return saveFile(fileName);
    }
}
void MainWindow::openRecentFile(){
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        loadFile(action->data().toString());
}
void MainWindow::open(){
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Virtual Field"), "",
                                                    tr("Virtual Field (*.vfi);;All Files (*)"));
    if(!fileName.isEmpty())
        loadFile(fileName);
}
void MainWindow::loadFile(const QString &fileName){
    QVector<VirtualDevice> virtualDeviceVector;
    QVector<Device> deviceVector;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_10);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QByteArray dataVirtualDevices, dataDevices;
    int answerTime;
    in >> dataVirtualDevices >> dataDevices >> answerTime;

    file.flush();
    file.close();
    QApplication::restoreOverrideCursor();

    QDomDocument docVirtualDevices, docDevices;
    docVirtualDevices.setContent(dataVirtualDevices);
    docDevices.setContent(dataDevices);

    QDomElement docElemVirtualDevices = docVirtualDevices.documentElement();
    QDomElement docElemDevices = docDevices.documentElement();
    bool fileValid = true;
    if(docElemVirtualDevices.nodeName().compare("virtualDevices") == 0
            && docElemDevices.nodeName().compare("devices") == 0
            && answerTime >= VirtualDevice::MinAnswerTime && answerTime <= VirtualDevice::MaxAnswerTime){
        QDomNodeList virtualDevice = docElemVirtualDevices.elementsByTagName("virtualDevice");
        QDomNodeList device = docElemDevices.elementsByTagName("device");
        if(!virtualDevice.isEmpty()) {
            for (int i = 0; i < virtualDevice.size(); i++) {
                VirtualDevice dev = VirtualDevice::parseDomNode(virtualDevice.item(i));
                if(dev.getAddress() >= 255)
                    fileValid = false;
                else
                    virtualDeviceVector.append(dev);
            }
        }
        if(!device.isEmpty()) {
            for (int i = 0; i < device.size(); i++) {
                Device dev = Device::parseDomNode(device.item(i));
                if(dev.getAddress() >= 255)
                    fileValid = false;
                else
                    deviceVector.append(dev);
            }
        }
    }else{
        fileValid = false;
    }

    if(!fileValid){
        QMessageBox::information(this, tr("Unable to open file"),
                                 tr("The file is corrupted."));
        virtualDeviceVector.clear();
        deviceVector.clear();

        statusBar()->showMessage(tr("File load failed"), 2000);
    }else{
        int tab = centralZoneWidget->indexOf(virtualFieldListWidgetTab);
        if(tab != -1)
            centralZoneWidget->setCurrentIndex(tab);
        int result = QMessageBox::Yes;
        if(virtualDeviceItemModel->rowCount() > 0){

            result = QMessageBox::question(this, tr("Confirmation"), tr("Merge devices?"),
                                           QMessageBox::Yes | QMessageBox::No);
            if (result == QMessageBox::Yes){
                bool yesToAll = false;
                bool noToAll = false;
                for (int i = 0; i < virtualDeviceVector.size(); ++i) {
                    bool deviceExisted = virtualDeviceItemModel->hasAddress(virtualDeviceVector.at(i).getAddress());
                    if(deviceExisted && !yesToAll && !noToAll){ //if device exists and not choice already made
                        int result = QMessageBox::question(this, tr("Confirmation"),
                                                           tr("Replace this virtual device: ") + QString::number(virtualDeviceVector.at(i).getAddress()) + "?",
                                                           QMessageBox::Yes |  QMessageBox::YesToAll |
                                                           QMessageBox::No | QMessageBox::NoToAll);
                        switch(result){
                        case QMessageBox::Yes:
                            virtualDeviceItemModel->addModifyVirtualDevice(virtualDeviceVector.at(i));
                            break;
                        case QMessageBox::YesToAll:
                            virtualDeviceItemModel->addModifyVirtualDevice(virtualDeviceVector.at(i));
                            yesToAll = true;
                            break;
                        case QMessageBox::No:
                            break;
                        case QMessageBox::NoToAll:
                            noToAll = true;
                            break;
                        default:
                            break;
                        }
                    }else if(deviceExisted && yesToAll){ //if device exists and yesToAll choosen
                        virtualDeviceItemModel->addModifyVirtualDevice(virtualDeviceVector.at(i));
                    }else if(!deviceExisted){//if device doesn't exist
                        virtualDeviceItemModel->addModifyVirtualDevice(virtualDeviceVector.at(i));
                    }
                }
                tab = centralZoneWidget->indexOf(deviceListWidgetTab);
                if(tab != -1)
                    centralZoneWidget->setCurrentIndex(tab);
                yesToAll = false;
                noToAll = false;
                for (int i = 0; i < deviceVector.size(); ++i) {
                    bool deviceExisted = deviceItemModel->hasAddress(deviceVector.at(i).getAddress());
                    if(deviceExisted && !yesToAll && !noToAll){ //if device exists and not choice already made
                        int result = QMessageBox::question(this, tr("Confirmation"),
                                                           tr("Replace this device: ") + QString::number(deviceVector.at(i).getAddress()) + "?",
                                                           QMessageBox::Yes |  QMessageBox::YesToAll |
                                                           QMessageBox::No | QMessageBox::NoToAll);
                        switch(result){
                        case QMessageBox::Yes:
                            deviceItemModel->addModifyDevice(deviceVector.at(i));
                            break;
                        case QMessageBox::YesToAll:
                            deviceItemModel->addModifyDevice(deviceVector.at(i));
                            yesToAll = true;
                            break;
                        case QMessageBox::No:
                            break;
                        case QMessageBox::NoToAll:
                            noToAll = true;
                            break;
                        default:
                            break;
                        }
                    }else if(deviceExisted && yesToAll){ //if device exists and yesToAll choosen
                        deviceItemModel->addModifyDevice(deviceVector.at(i));
                    }else if(!deviceExisted){//if device doesn't exist
                        deviceItemModel->addModifyDevice(deviceVector.at(i));
                    }
                }
                answerTimeMaximumSpinBox->setValue(answerTime);
            }else if(result == QMessageBox::No){
                result = QMessageBox::question(this, tr("Confirmation"), tr("Delete existing items?"),
                                               QMessageBox::Yes | QMessageBox::No);
                if (result == QMessageBox::Yes){
                    virtualDeviceItemModel->clear();
                    for (int i = 0; i < virtualDeviceVector.size(); ++i) {
                        virtualDeviceItemModel->addModifyVirtualDevice(virtualDeviceVector.at(i));
                    }
                    deviceItemModel->clear();
                    for (int i = 0; i < deviceVector.size(); ++i) {
                        deviceItemModel->addModifyDevice(deviceVector.at(i));
                    }
                    answerTimeMaximumSpinBox->setValue(answerTime);
                }else{
                    return;
                }
            }
        }else{
            virtualDeviceItemModel->clear();
            for (int i = 0; i < virtualDeviceVector.size(); ++i) {
                virtualDeviceItemModel->addModifyVirtualDevice(virtualDeviceVector.at(i));
            }
            deviceItemModel->clear();
            for (int i = 0; i < deviceVector.size(); ++i) {
                deviceItemModel->addModifyDevice(deviceVector.at(i));
            }
            answerTimeMaximumSpinBox->setValue(answerTime);
        }
        updateVirtualField();

        setCurrentFile(fileName);
        statusBar()->showMessage(tr("File loaded"), 2000);
    }
}
bool MainWindow::saveFile(const QString &fileName){
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Recent Files"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    QByteArray data = virtualDeviceItemModel->getDomDocument().toByteArray();
    QByteArray data1 = deviceItemModel->getDomDocument().toByteArray();
    int answerTimeMaximum = answerTimeMaximumSpinBox->value();
    QDataStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out.setVersion(QDataStream::Qt_5_10);
    out << data << data1 << answerTimeMaximum;
    file.flush();
    file.close();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}
void MainWindow::setCurrentFile(const QString &fileName){
    curFile = fileName;
    setWindowFilePath(curFile);

    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("recentFileList", files);

    updateRecentFileActions();
}
void MainWindow::closeTab(int tab){
    tabQWidgetQActionHash.value(centralZoneWidget->widget(tab))->setChecked(false);
    centralZoneWidget->removeTab(tab);
}
void MainWindow::showHideTab(bool checked){
    QAction *action = qobject_cast<QAction *>(sender());
    QWidget *widget = tabQActionQWidgetHash.value(action);
    if(checked){
        centralZoneWidget->addTab(widget, tabQWidgetQStringHash.value(widget));
    }else{
        centralZoneWidget->removeTab(centralZoneWidget->indexOf(widget));
    }
}
void MainWindow::clientCountChanged(int clientCount){
    if(clientCount == 0)
        numberOfConnection->setText(tr("no client connected"));
    else
        numberOfConnection->setText(tr("number of clients: ") + QString::number(clientCount));
}
void MainWindow::timerOut(){
    qDebug() << "timerOut()" ;
    timerFinishedBeforeAck = true;
    updateStateMachine();
}
void MainWindow::updateStateMachine(){
    QHash<int, Device> deviceHash;
    switch(stateMachine) {
    case OrderToSend:
        if(queueOrderPending.size() != 0){
            while(queueOrderPending.size() > 0 && !orderReceivedFlag){
                currentOrderPending = queueOrderPending.dequeue();
                if(currentOrderPending.socket->state() == QAbstractSocket::ConnectedState
                        && currentOrderPending.socket->isValid()){
                    orderReceivedFlag = true;
                }
            }
        }else
            orderReceivedFlag = false; // useless
        if(orderReceivedFlag){
            qDebug() << "orderReceivedFlag";
            orderReceivedFlag = false;
            quint8 addressOrder = currentOrderPending.address;
            if(currentOrderPending.orderCode == Device::OrderCode::AddModify){
                if(currentOrderPending.name.size() <= Device::MAXIMUMSIZENAMEDEVICE){
                    virtualDeviceItemModel->sendRequest(VirtualDevice::OrderCode::Read, addressOrder);
                    timerAnswer->start(answerTimeMaximumSpinBox->value());
                    stateMachine = AckReceived;
                }else{
                    connectionHandler->sendData(Device::AckCode::InvalidName, currentOrderPending.orderCode, deviceHash, addressOrder, currentOrderPending.socket);
                }
            }else{
                if(deviceItemModel->hasAddress(addressOrder)){
                    DeviceItem *item;
                    switch(currentOrderPending.orderCode) {
                    case Device::OrderCode::UnknownCode:
                        connectionHandler->sendData(Device::AckCode::InvalidFrameFromClient, currentOrderPending.orderCode, deviceHash, addressOrder, currentOrderPending.socket);
                        stateMachine = OrderToSend;
                        break;
                    case Device::OrderCode::Read:
                        virtualDeviceItemModel->sendRequest(VirtualDevice::OrderCode::Read, addressOrder);
                        timerAnswer->start(answerTimeMaximumSpinBox->value());
                        stateMachine = AckReceived;
                        break;
                    case Device::OrderCode::Action:
                        virtualDeviceItemModel->sendRequest(VirtualDevice::OrderCode::Action, addressOrder, currentOrderPending.action);
                        timerAnswer->start(answerTimeMaximumSpinBox->value());
                        stateMachine = AckReceived;
                        break;
                    case Device::OrderCode::Delete:
                        item = deviceItemModel->getDeviceItemFromAddress(addressOrder);
                        deviceItemModel->removeRow(item->row());

                        connectionHandler->sendData(Device::AckCode::OK, currentOrderPending.orderCode, deviceHash, addressOrder, currentOrderPending.socket);
                        stateMachine = OrderToSend;
                        break;
                    default:
                        break;
                    }
                }else{
                    connectionHandler->sendData(Device::AckCode::UnknownAddress, currentOrderPending.orderCode, deviceHash, addressOrder, currentOrderPending.socket);
                    stateMachine = OrderToSend;
                }
            }
            updateStateMachine();
        }
        break;
    case AckReceived:
        if(ackFromVirtualFlag){
            qDebug() << "ackFromVirtualFlag";
            ackFromVirtualFlag = false;
            quint8 addressOrder = currentOrderPending.address;
            if(addressOrder == currentAckPending.address){
                if(currentAckPending.ackCode == VirtualDevice::AckCode::InvalidAddress){
                    connectionHandler->sendData(Device::AckCode::InvalidAddress, currentOrderPending.orderCode, deviceHash, addressOrder, currentOrderPending.socket);
                }else if(currentAckPending.ackCode == VirtualDevice::AckCode::OK){
                    Device dev;
                    DeviceItem *item;
                    switch(currentOrderPending.orderCode){
                    case Device::OrderCode::AddModify:
                        dev = Device(currentOrderPending.address, currentOrderPending.type, currentOrderPending.name,
                                     currentAckPending.value, currentAckPending.state, currentOrderPending.coordinateX, currentOrderPending.coordinateY);
                        deviceItemModel->addModifyDevice(dev);
                        item = deviceItemModel->getDeviceItemFromAddress(addressOrder);
                        deviceHash.insert(currentOrderPending.address, item->getDevice());
                        connectionHandler->sendData(Device::AckCode::OK, currentOrderPending.orderCode, deviceHash, addressOrder, currentOrderPending.socket);
                        break;
                    case Device::OrderCode::Read:
                        if(deviceItemModel->hasAddress(addressOrder)){
                            item = deviceItemModel->getDeviceItemFromAddress(addressOrder);
                            dev = item->getDevice();
                            dev.setValue(currentAckPending.value);
                            dev.setState(currentAckPending.state);
                            deviceItemModel->addModifyDevice(dev);
                            deviceHash.insert(currentOrderPending.address, item->getDevice());
                            connectionHandler->sendData(Device::AckCode::OKread, currentOrderPending.orderCode, deviceHash, addressOrder, currentOrderPending.socket);
                        }
                        break;
                    case Device::OrderCode::Action:
                        if(deviceItemModel->hasAddress(addressOrder)){
                            item = deviceItemModel->getDeviceItemFromAddress(addressOrder);
                            dev = item->getDevice();
                            dev.setValue(currentAckPending.value);
                            dev.setState(currentAckPending.state);
                            deviceItemModel->addModifyDevice(dev);
                            deviceHash.insert(currentOrderPending.address, item->getDevice());
                            connectionHandler->sendData(Device::AckCode::OK, currentOrderPending.orderCode, deviceHash, addressOrder, currentOrderPending.socket);
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
            stateMachine = OrderToSend;
            updateStateMachine();
        }
        if(timerFinishedBeforeAck){
            qDebug() << "timerFinishedBeforeAck";
            timerFinishedBeforeAck = false;

            quint8 addressOrder = currentOrderPending.address;
            if(deviceItemModel->hasAddress(addressOrder)){
                DeviceItem *item = deviceItemModel->getDeviceItemFromAddress(addressOrder);
                Device dev = item->getDevice();
                dev.setState(Device::StateCode::Unreachable);
                deviceItemModel->addModifyDevice(dev);
            }
            if((currentOrderPending.orderCode == Device::OrderCode::Read) && deviceItemModel->hasAddress(addressOrder)){
                DeviceItem *item = deviceItemModel->getDeviceItemFromAddress(addressOrder);
                deviceHash.insert(currentOrderPending.address, item->getDevice());
                connectionHandler->sendData(Device::AckCode::OKread, currentOrderPending.orderCode, deviceHash, addressOrder, currentOrderPending.socket);
            }else
                connectionHandler->sendData(Device::AckCode::UnreachedDevice, currentOrderPending.orderCode, deviceHash, currentOrderPending.address, currentOrderPending.socket);
            stateMachine = OrderToSend;
            updateStateMachine();
        }
        break;
    }
    orderReceivedFlag = false;
    ackFromVirtualFlag = false;
    timerFinishedBeforeAck = false;
}
void MainWindow::updateVirtualField(){
    answerTimeMaximumSpinBox->valueChanged(answerTimeMaximumSpinBox->value());
}
void MainWindow::updateRecentFileActions(){
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), static_cast<int>(MaxRecentFiles));

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
        recentFile[i]->setText(text);
        recentFile[i]->setData(files[i]);
        recentFile[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFile[j]->setVisible(false);

    separatorRecentFile->setVisible(numRecentFiles > 0);
}
void MainWindow::orderReceived(Device::OrderStructure orderReceived){
    qDebug() << "orderReceived()";
    if(orderReceived.orderCode == Device::OrderCode::Read){
        if(orderReceived.address == 255 && deviceItemModel->rowCount() != 0){
            for (int row = 0; row < deviceItemModel->rowCount(); ++row) {
                Device::OrderStructure order = orderReceived;
                order.address = static_cast<quint8>(deviceItemModel->item(row, DeviceItemModel::ADDRESSCOLUMN)->data(Qt::EditRole).toInt());
                queueOrderPending.enqueue(order);
            }
        }else{
            queueOrderPending.enqueue(orderReceived);
        }
    }else{
        queueOrderPending.enqueue(orderReceived);
    }
    updateStateMachine();
}
void MainWindow::ackFromVirtualDeviceItemModel(VirtualDevice::AckStructure ackReceived){
    qDebug() << "ackFromVirtualDeviceItemModel()";
    timerAnswer->stop();
    currentAckPending = ackReceived;
    ackFromVirtualFlag = true;
    updateStateMachine();
}
void MainWindow::readSettings(){
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    const QString file = settings.value("lastFile", QString()).toString();
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    }
    if(!file.isEmpty()){
        loadFile(file);
    }
}
void MainWindow::writeSettings(){
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("lastFile", curFile);
}
void MainWindow::closeEvent(QCloseEvent *event){
    writeSettings();
    QMainWindow::closeEvent(event);
}

