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
#include "virtualdeviceitemmodel.h"

VirtualDeviceItemModel::VirtualDeviceItemModel(QObject *parent): QStandardItemModel(parent){
    timerAnswer = new QTimer(this);
    timerAnswer->setSingleShot(true);
    connect(timerAnswer, &QTimer::timeout, [this] (){ answerVirtualDevice(answerAckVirtualDevicePending);});
    initialize();
}
void VirtualDeviceItemModel::initialize(){
    QStringList headerLabels = VirtualDevice::HeaderLabelList;
    this->setHorizontalHeaderLabels(headerLabels);
}
void VirtualDeviceItemModel::clear(){
    QStandardItemModel::clear();
    initialize();
}
void VirtualDeviceItemModel::addModifyVirtualDevice(VirtualDevice vDev){
    if(!hasAddress(vDev.getAddress())){
        VirtualDeviceItem *deviceItem = new VirtualDeviceItem(vDev);
        this->appendRow(QList<QStandardItem*>() << deviceItem
                        << deviceItem->getStateItem()
                        << deviceItem->getValueItem()
                        << deviceItem->getReachableItem()
                        << deviceItem->getTimeToReachItem());
    }else{
        VirtualDeviceItem *item = static_cast<VirtualDeviceItem*>(findItems(QString::number(vDev.getAddress())).constFirst());
        item->getStateItem()->setData(QVariant(vDev.getState()), Qt::EditRole);
        item->getValueItem()->setData(QVariant(vDev.getValue()), Qt::EditRole);
        item->getReachableItem()->setData(QVariant(vDev.getReachable()), Qt::EditRole);
        item->getTimeToReachItem()->setData(QVariant(vDev.getTimeToReach()), Qt::EditRole);
    }
    sort(ADDRESSCOLUMN);
}
void VirtualDeviceItemModel::sendRequest(VirtualDevice::OrderCode orderCode, quint8 address, quint8 action){
    if(hasAddress(address)){
        VirtualDeviceItem *item = static_cast<VirtualDeviceItem*>(findItems(QString::number(address)).constFirst());
        VirtualDevice vdev = item->getVirtualDevice();
        switch(orderCode){
        case VirtualDevice::OrderCode::Read:
            if(vdev.getReachable()){
                answerAckVirtualDevicePending.ackCode = VirtualDevice::AckCode::OK;
                answerAckVirtualDevicePending.address = address;
                answerAckVirtualDevicePending.state = vdev.getState();
                answerAckVirtualDevicePending.value = vdev.getValue();
                timerAnswer->start(vdev.getTimeToReach());
            }
            break;
        case VirtualDevice::OrderCode::Action:
            if(vdev.getReachable()){
                if(action == VirtualDevice::ActionCode::TurnOn){
                    item->getStateItem()->setData(QVariant(VirtualDevice::State::On), Qt::EditRole);
                    answerAckVirtualDevicePending.state = VirtualDevice::State::On;
                } else if(action == VirtualDevice::ActionCode::TurnOff){
                    item->getStateItem()->setData(QVariant(VirtualDevice::State::Off), Qt::EditRole);
                    answerAckVirtualDevicePending.state = VirtualDevice::State::Off;
                }
                //TODO handle device that can't do action and send appropriate ack
                answerAckVirtualDevicePending.ackCode = VirtualDevice::AckCode::OK;
                answerAckVirtualDevicePending.address = address;
                answerAckVirtualDevicePending.value = vdev.getValue();
                timerAnswer->start(vdev.getTimeToReach());
            }
            break;
        }
    }
}
void VirtualDeviceItemModel::setTimeToReachMaximum(int maximum){
    for (int row = 0; row < rowCount(); ++row) {
        VirtualDeviceItem *item = static_cast<VirtualDeviceItem*>(this->item(row, ADDRESSCOLUMN));
        if(item->getTimeToReachItem()->data(Qt::EditRole).toInt() > maximum)
            item->getTimeToReachItem()->setData(QVariant(maximum), Qt::EditRole);
    }
}
bool VirtualDeviceItemModel::hasAddress(int address){
    QList<QStandardItem*> list = findItems(QString::number(address), Qt::MatchExactly, ADDRESSCOLUMN);
    return !list.isEmpty();
}
QDomDocument VirtualDeviceItemModel::getDomDocument(){
    QDomDocument doc;
    QDomElement devicesElement = doc.createElement("virtualDevices");
    doc.appendChild(devicesElement);
    for (int row = 0; row < rowCount(); ++row) {
        VirtualDeviceItem *item = static_cast<VirtualDeviceItem*>(this->item(row, ADDRESSCOLUMN));
        devicesElement.appendChild(item->getVirtualDevice().getDomDocument());
    }
    return doc;
}
