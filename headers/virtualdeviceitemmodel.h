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
#ifndef VIRTUALDEVICEITEMMODEL_H
#define VIRTUALDEVICEITEMMODEL_H

#include <QStandardItemModel>
#include <QTimer>
#include <QDebug>

#include "virtualdevice.h"
#include "virtualdeviceitem.h"

class VirtualDeviceItemModel : public QStandardItemModel
{
    Q_OBJECT

public:
    VirtualDeviceItemModel(QObject *parent = nullptr);
    void clear();
    void addModifyVirtualDevice(VirtualDevice vDev);
    void sendRequest(VirtualDevice::OrderCode orderCode, quint8 address = 255, quint8 action = 0);
    void setTimeToReachMaximum(int maximum);
    bool hasAddress(int address);
    QDomDocument getDomDocument();

    static const int ADDRESSCOLUMN = 0;
    static const int STATECOLUMN = 1;
    static const int VALUECOLUMN = 2;
    static const int REACHABLECOLUMN = 3;
    static const int TIMETOANSWERCOLUMN = 4;

signals:
    void answerVirtualDevice(VirtualDevice::AckStructure answerVirtualDevicePending);

private:
    void initialize();

    VirtualDevice::AckStructure answerAckVirtualDevicePending;
    QTimer *timerAnswer;
};

#endif // VIRTUALDEVICEITEMMODEL_H
