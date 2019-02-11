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
#include "virtualdevice.h"

const QStringList VirtualDevice::stateList = QStringList() << QObject::tr("Unknown")
                                                    << QObject::tr("On")
                                                    << QObject::tr("Off")
                                                    << QObject::tr("Unreachable");
const QStringList VirtualDevice::HeaderLabelList = QStringList() << QObject::tr("Address")
                                                                 << QObject::tr("State")
                                                                 << QObject::tr("Value")
                                                                 << QObject::tr("Reachable")
                                                                 << QObject::tr("Time To Reach");

VirtualDevice::VirtualDevice(){
    address = 255;
    value = 0;
    state = 0;
    reachable = true;
    timeToReach = MinAnswerTime;
}
VirtualDevice::VirtualDevice(quint8 address, int value, quint8 state, bool reachable, int timeToReach){
    this->address = address;
    this->value = value;
    this->state = state;
    this->reachable = reachable;
    this->timeToReach= timeToReach;
}
VirtualDevice::VirtualDevice(const VirtualDevice *device){
    address = device->getAddress();
    value = device->getValue();
    state = device->getState();
    reachable = device->getReachable();
    timeToReach = device->getTimeToReach();
}
void VirtualDevice::setAddress(quint8 address){
    this->address = address;
}
void VirtualDevice::setValue(int value){
    this->value = value;
}
void VirtualDevice::setState(quint8 state){
    this->state = state;
}
void VirtualDevice::setReachable(bool reachable){
    this->reachable = reachable;
}
void VirtualDevice::setTimeToReach(int timeToReach){
    this->timeToReach = timeToReach;
}
quint8 VirtualDevice::getAddress() const{
    return address;
}
int VirtualDevice::getValue() const{
    return value;
}
quint8 VirtualDevice::getState() const{
    return state;
}
QString VirtualDevice::getStateString() const{
    return stateList.value(state, QString::number(state));
}
bool VirtualDevice::getReachable() const{
    return reachable;
}
int VirtualDevice::getTimeToReach() const{
    return timeToReach;
}
QString VirtualDevice::toString() const{
    QString str = "";
    str + "address  " + QString::number(address)
        + "value  " + QString::number(value);
    return str;
}
QDomDocument VirtualDevice::getDomDocument(){
    QDomDocument doc;
    QDomElement deviceElement = doc.createElement("virtualDevice");
    QDomElement addressElement = doc.createElement("address");
    QDomElement valueElement = doc.createElement("value");
    QDomElement reachableElement = doc.createElement("reachable");
    QDomElement stateElement = doc.createElement("state");
    QDomElement timeToReachElement = doc.createElement("timeToReach");

    addressElement.appendChild(doc.createTextNode(QString::number(this->getAddress())));
    valueElement.appendChild(doc.createTextNode(QString::number(this->getValue())));
    reachableElement.appendChild(doc.createTextNode(QString::number(this->getReachable())));
    timeToReachElement.appendChild(doc.createTextNode(QString::number(this->getTimeToReach())));
    stateElement.appendChild(doc.createTextNode(QString::number(this->getState())));

    deviceElement.appendChild(addressElement);
    deviceElement.appendChild(valueElement);
    deviceElement.appendChild(reachableElement);
    deviceElement.appendChild(timeToReachElement);
    deviceElement.appendChild(stateElement);
    doc.appendChild(deviceElement);

    return doc;
}
VirtualDevice VirtualDevice::parseDomNode(QDomNode domNode){
    VirtualDevice dev;
    QDomElement address = domNode.firstChildElement("address");
    QDomElement value = domNode.firstChildElement("value");
    QDomElement reachable = domNode.firstChildElement("reachable");
    QDomElement state = domNode.firstChildElement("state");
    QDomElement timeToReach = domNode.firstChildElement("timeToReach");
    if (!(address.isNull() || value.isNull() || reachable.isNull() || state.isNull()|| timeToReach.isNull())){
        dev = new VirtualDevice(static_cast<quint8>(address.text().toInt()),
                                value.text().toInt(),
                                static_cast<quint8>(state.text().toInt()),
                                static_cast<bool>(reachable.text().toInt()),
                                timeToReach.text().toInt());
    }
   return dev;
}
