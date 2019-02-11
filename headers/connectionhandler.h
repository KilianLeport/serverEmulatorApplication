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
#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H

#include <QObject>
#include <QtXml>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTcpServer>
#include <QTcpSocket>

#include "device.h"

class ConnectionHandler : public QObject
{
    Q_OBJECT

public:
    enum ServerState : int
    {
        Started,
        Unstarted
    };

    explicit ConnectionHandler(QObject *parent = nullptr);
    void sendData(Device::AckCode ackCode, Device::OrderCode orderCode, QHash<int, Device> deviceHash, quint8 address, QTcpSocket *socket);
    bool isServerStarted();
    void sendAlivedMessage(QTcpSocket *socket);

    static const quint16 PORTTCP = 15750;

signals:
    void frameTCPOrder(Device::OrderStructure orderReceived);
    void clientCountChanged(int clientCount);


private slots:
    void tcpReadyRead();
    void newConnection();
    void disconnected();
    void error(QAbstractSocket::SocketError);
    void stateChanged(QAbstractSocket::SocketState socketState);

private:
    void handleFrame(QByteArray &frame, QTcpSocket *socket);

    //Server
    QTcpServer *tcpServer;
    int clientCount;
    QHash<QTcpSocket*, QPair<QSharedPointer<QByteArray>, int>> clientsHash;
    bool serverStarted;
    const QByteArray LIVING = QByteArray("LIVING");
    const QByteArray ALIVE = QByteArray("ALIVE");
};

#endif // CONNECTIONHANDLER_H
