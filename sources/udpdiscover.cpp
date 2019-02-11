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
#include "udpdiscover.h"

UDPDiscover::UDPDiscover(QObject *parent) : QObject(parent){
}

void UDPDiscover::initializationDiscover(){
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost)){
            ipAddressComputer = address;
        }
    }
    if(!ipAddressComputer.isNull()){
        socket = new QUdpSocket(this);
        socket->bind(ipAddressComputer, PORTUDPSERVER);
        connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
        qInfo() << "Waiting for request on port " << PORTUDPSERVER << " at IP: " << ipAddressComputer.toString() << "...";
    }else{
        qCritical() << "No IP found! " ;
    }
}

void UDPDiscover::sendAnswer(QHostAddress &sender){
    QByteArray byteArrayAnswer;
    QDataStream stream(&byteArrayAnswer, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_10);
    QByteArray code(CODE_SERVER);
    stream  << code << ipAddressComputer.toIPv4Address() << ConnectionHandler::PORTTCP;
    socket->writeDatagram(byteArrayAnswer, sender, PORTUDPCLIENT);
    qDebug() << "SendAnswer byteArrayAnswer:   " << byteArrayAnswer;
}

void UDPDiscover::readyRead(){
    QByteArray datagram;
    datagram.resize(static_cast<int>(socket->pendingDatagramSize()));
    QHostAddress sender;
    quint16 senderPort;

    socket->readDatagram(datagram.data(), datagram.size(),&sender, &senderPort);

    qInfo() << "Message received from: " << sender.toString() <<  " port: " << senderPort;
    qInfo() << "Message: " << datagram;

    QByteArray code(CODE_CLIENT);
    if(datagram == code){
        sendAnswer(sender);
    }
}
