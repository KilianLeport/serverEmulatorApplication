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
#include "connectionhandler.h"

ConnectionHandler::ConnectionHandler(QObject *parent) : QObject(parent){
    tcpServer = new QTcpServer(this);
    serverStarted = false;
    clientCount = 0;
    if (!tcpServer->listen(QHostAddress::Any, PORTTCP)){
        qCritical() << "The server didn't start: " << tcpServer->errorString();
    }else
    {
        qInfo() << "The server started on: " << QString::number(tcpServer->serverPort());
        serverStarted = true;
        connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
    }
}
bool ConnectionHandler::isServerStarted(){
    return serverStarted;
}
void ConnectionHandler::sendData(Device::AckCode ackCode, Device::OrderCode orderCode, QHash<int, Device> deviceHash, quint8 address, QTcpSocket *socket){
    QDomDocument doc;
    QDomElement ackElement = doc.createElement("ack");
    QDomElement ackCodeElement = doc.createElement("ackCode");
    QDomElement deviceElement = doc.createElement("device");
    QDomElement addressElement = doc.createElement("address");
    QDomElement orderCodeElement = doc.createElement("orderCode");
    ackCodeElement.appendChild(doc.createTextNode(QString::number(ackCode)));

    doc.appendChild(ackElement);
    ackElement.appendChild(ackCodeElement);

    QHash<int, Device>::iterator i;

    switch(ackCode){
    case Device::AckCode::OKread:
        for (i = deviceHash.begin(); i != deviceHash.end(); ++i){
            QDomDocument deviceDoc = i.value().getDomDocument();
            ackElement.appendChild(deviceDoc);
        }
        break;
    case Device::AckCode::OK:
    case Device::AckCode::InvalidAddress:
    case Device::AckCode::UnknownAddress:
    case Device::AckCode::InvalidName:
    case Device::AckCode::UnreachedDevice:
    case Device::AckCode::InvalidFrameFromClient:
    case Device::AckCode::InvalidFrameFromServer:
        ackElement.appendChild(deviceElement);
        addressElement.appendChild(doc.createTextNode(QString::number(address)));
        orderCodeElement.appendChild(doc.createTextNode(QString::number(orderCode)));
        deviceElement.appendChild(addressElement);
        deviceElement.appendChild(orderCodeElement);
        break;
    default:
        break;
    }
    QByteArray data = doc.toByteArray();
    int size = data.size();
    QByteArray sizeArray;
    QDataStream streamSize(&sizeArray, QIODevice::WriteOnly);
    streamSize.setVersion(QDataStream::Qt_5_10);
    streamSize << size;

    QByteArray block;
    QDataStream stream(&block, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_10);
    stream.writeRawData(sizeArray, sizeof(int));
    stream.writeRawData(data, size);

    if(ackCode == Device::AckCode::OK){
        QHash<QTcpSocket*, QPair<QSharedPointer<QByteArray>, int>>::iterator i;
        for (i = clientsHash.begin(); i != clientsHash.end(); ++i){
            QTcpSocket *socket = i.key();
            if(socket->state() == QAbstractSocket::ConnectedState){
                qInfo() << "sendData() Send to client " << socket->peerAddress().toString() << " ackCode :" << QString::number(ackCode);
                socket->write(block);
            }
        }
    }
    else{
        if(clientsHash.contains(socket)){
            if(socket->state() == QAbstractSocket::ConnectedState){
                qInfo() << "sendData() Send to client " << socket->peerAddress().toString() << " ackCode :" << QString::number(ackCode);
                socket->write(block);
            }
        }
    }
}
void ConnectionHandler::sendAlivedMessage(QTcpSocket *socket){
    QByteArray sizeArray;
    QDataStream streamSize(&sizeArray, QIODevice::WriteOnly);
    streamSize.setVersion(QDataStream::Qt_5_10);
    streamSize << ALIVE.size();

    QByteArray block;
    QDataStream stream(&block, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_10);
    stream.writeRawData(sizeArray, sizeof(int));
    stream.writeRawData(ALIVE, ALIVE.size());

    if(clientsHash.contains(socket)){
        if(socket->state() == QAbstractSocket::ConnectedState){
            socket->write(block);
        }
    }
}
void ConnectionHandler::handleFrame(QByteArray &frame, QTcpSocket *socket){
    Device::OrderStructure orderToSend;
    orderToSend.socket = socket;

    QDomDocument doc;
    doc.setContent(frame);
    QDomElement docElem = doc.documentElement();
    QDomElement orderCodeElement = docElem.firstChildElement("orderCode");
    QDomElement addressElement = docElem.firstChildElement("address");
    QDomElement nameElement = docElem.firstChildElement("name");
    QDomElement typeElement = docElem.firstChildElement("type");
    QDomElement actionElement = docElem.firstChildElement("action");
    QDomElement coordinateXElement = docElem.firstChildElement("coordinateX");
    QDomElement coordinateYElement = docElem.firstChildElement("coordinateY");

    if(docElem.nodeName().compare("order") == 0
            && !orderCodeElement.isNull()
            && !addressElement.isNull()){
        quint8 orderCode = static_cast<quint8>(orderCodeElement.text().toInt());
        quint8 address = static_cast<quint8>(addressElement.text().toInt());
        orderToSend.address = address;
        orderToSend.orderCode = (Device::OrderCode) orderCode;
        switch(orderCode){
        case Device::OrderCode::AddModify:
            if(!nameElement.isNull() && !typeElement.isNull() && !coordinateXElement.isNull() && !coordinateYElement.isNull()){
                orderToSend.name = nameElement.text();
                orderToSend.type = static_cast<quint8>(typeElement.text().toInt());
                orderToSend.coordinateX = coordinateXElement.text().toInt();
                orderToSend.coordinateY = coordinateYElement.text().toInt();
                qInfo() << "handleFrame() Client " << socket->peerAddress().toString() << " order AddModify address :" << QString::number(address);
                emit frameTCPOrder(orderToSend);
            }else{
                qCritical() << "Frame invalid from client";
                emit frameTCPOrder(orderToSend);
            }
            break;
        case Device::OrderCode::Delete:
            qInfo() << "handleFrame() Client " << socket->peerAddress().toString() << " order Delete address :" << QString::number(address);
            emit frameTCPOrder(orderToSend);
            break;
        case Device::OrderCode::Read:
            qInfo() << "handleFrame() Client " << socket->peerAddress().toString() << " order Read address :" << QString::number(address);
            emit frameTCPOrder(orderToSend);
            break;
        case Device::OrderCode::Action:
            if(!actionElement.isNull()){
                int action = actionElement.text().toInt();
                orderToSend.action = static_cast<quint8>(action);
                qInfo() << "handleFrame() Client " << socket->peerAddress().toString() << " order Action address :" << QString::number(address);
                emit frameTCPOrder(orderToSend);
            }else{
                qCritical() << "Frame invalid from client";
                orderToSend.orderCode = Device::OrderCode::UnknownCode;
                emit frameTCPOrder(orderToSend);
            }
            break;
        default:
            break;
        }
    }else{
        qCritical() << "Frame invalid from client";
        orderToSend.orderCode = Device::OrderCode::UnknownCode;
        emit frameTCPOrder(orderToSend);
    }
}
//******************************private slots***********************************
void ConnectionHandler::tcpReadyRead(){
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    QPair<QSharedPointer<QByteArray>, int> attributSocket = clientsHash.value(socket);
    QSharedPointer<QByteArray> buffer = attributSocket.first;
    int size = attributSocket.second;
    while (socket->bytesAvailable() > 0)
    {
        buffer.get()->append(socket->readAll());
        while(buffer.get()->size() >= 4){
            if (size == 0 && buffer.get()->size() >= 4) //if size of data has received completely, store it on our global variable
            {
                QByteArray source = buffer.get()->mid(0, 4);
                QDataStream data(&source, QIODevice::ReadOnly);
                data >> size;
                attributSocket.second = size;
                clientsHash.insert(socket, attributSocket);
                buffer.get()->remove(0, 4);
            }
            if (size > 0 && buffer.get()->size() >= size) // If data has received completely
            {
                QByteArray data = buffer.get()->mid(0, size);
                buffer.get()->remove(0, size);
                /*if(data.contains(LIVING) && size == LIVING.size())
                    sendAlivedMessage(socket);
                else
                    handleFrame(data, socket);*/
                if(!data.contains(LIVING))
                    handleFrame(data, socket);
                size = 0;
                attributSocket.second = size;
                clientsHash.insert(socket, attributSocket);
            }
        }
    }
}
void ConnectionHandler::newConnection(){
    while (tcpServer->hasPendingConnections())
    {
        QTcpSocket *newClient = tcpServer->nextPendingConnection();
        connect(newClient, SIGNAL(readyRead()), this, SLOT(tcpReadyRead()));
        connect(newClient, SIGNAL(disconnected()), this, SLOT(disconnected()));

        connect(newClient, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &ConnectionHandler::error);
        connect(newClient, QOverload<QAbstractSocket::SocketState>::of(&QAbstractSocket::stateChanged), this, &ConnectionHandler::stateChanged);

        QSharedPointer<QByteArray> buffer(new QByteArray());
        int size = 0;
        QPair<QSharedPointer<QByteArray>, int> pair;
        pair.first = buffer;
        pair.second = size;

        clientsHash.insert(newClient, pair);
        clientCount++;
        qInfo() << "newConnection() Client " << newClient->peerAddress().toString() << " connected";
        emit clientCountChanged(clientCount);
    }
}
void ConnectionHandler::disconnected(){
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    if(clientsHash.contains(socket)){
        clientsHash.remove(socket);
    }
    clientCount--;
    emit clientCountChanged(clientCount);
    qInfo() << "disconnected() Client " << socket->peerAddress() << " disconnected";
    socket->deleteLater();
}
void ConnectionHandler::error(QAbstractSocket::SocketError /*socketError*/){
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    qCritical() << "error() Client " << socket->peerAddress().toString() << " error: " << socket->errorString();
}

void ConnectionHandler::stateChanged(QAbstractSocket::SocketState socketState){   
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    qInfo() << "stateChanged() Client " <<  socket->peerAddress().toString() << " state changed: " << socketState;
}
