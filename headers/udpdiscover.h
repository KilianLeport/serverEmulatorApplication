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
#ifndef UDPDISCOVER_H
#define UDPDISCOVER_H

#include <QObject>
#include <QUdpSocket>
#include <QNetworkInterface>
#include <QDataStream>

#include "connectionhandler.h"

class UDPDiscover : public QObject
{
    Q_OBJECT
public:
    explicit UDPDiscover(QObject *parent = nullptr);

private:
    void sendAnswer(QHostAddress &sender);

private slots:
    void readyRead();
    void initializationDiscover();

private:
    QUdpSocket *socket;
    QHostAddress ipAddressComputer;

    static const quint16 PORTUDPSERVER = 10101;
    static const quint16 PORTUDPCLIENT = 10102;
    const char* CODE_CLIENT = "CODECLIENT";
    const char* CODE_SERVER = "CODESERVER";
};


#endif // UDPDISCOVER_H
