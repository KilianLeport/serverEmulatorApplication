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
#ifndef VIRTUALDEVICE_H
#define VIRTUALDEVICE_H

#include <QObject>
#include <QDomElement>

class VirtualDevice
{


public:
    VirtualDevice();
    VirtualDevice(quint8 address, int value = 0 , quint8 state = 0, bool reachable = true, int timeToReach = 0);
    VirtualDevice(const VirtualDevice* device);
    void setAddress(quint8 address);
    void setType(quint8 type);
    void setValue(int value);
    void setState(quint8 state);
    void setReachable(bool reachable);
    void setTimeToReach(int timeToReach);
    quint8 getAddress() const;
    quint8 getType() const;
    QString getTypeString() const;
    int getValue() const;
    QString toString() const;
    quint8 getState() const;
    QString getStateString() const;
    bool getReachable() const;
    int getTimeToReach() const;
    QDomDocument getDomDocument();
    static VirtualDevice parseDomNode(QDomNode domNode);

    //maximum const
    static const int  MAXIMUMADDRESS = 254;
    static const int  MAXIMUMTYPE = 255;
    static const int  MAXIMUMSTATE = 255;
    static const int  MAXIMUMVALUE = 65535;

    static const QStringList typeList;
    static const QStringList stateList;
    static const QStringList HeaderLabelList;

    enum { MaxAnswerTime = 10000, MinAnswerTime = 0 };

    enum State : quint8
    {
        UnknownState = 0,
        On = 1,
        Off = 2,
        Unreachable = 3
    };
    enum OrderCode : quint8
    {
        Read = 0,
        Action = 1
    };
    enum AckCode : quint8
    {
        UnknownAckCode = 1,
        OK = 2,
        InvalidAddress = 3
    };
    enum ActionCode : quint8
    {
        UnknownAction = 0,
        TurnOn = 1,
        TurnOff = 2
    };

    struct AckStructure {
        VirtualDevice::AckCode ackCode;
        quint8 address;
        int value;
        quint8 state;
        void OrderStructure()
        {
            ackCode = VirtualDevice::AckCode::UnknownAckCode;
            address = 255;
            value = 0;
            state = 0;
        }
    };

private:
    quint8 address;
    int value;
    quint8 state;
    bool reachable;
    int timeToReach;
};

#endif // VIRTUALDEVICE_H
