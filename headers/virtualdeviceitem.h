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
#ifndef VIRTUALDEVICEITEM_H
#define VIRTUALDEVICEITEM_H

#include <QStandardItem>

#include "virtualdevice.h"

class VirtualDeviceItem : public QStandardItem
{
public:
    VirtualDeviceItem(VirtualDevice vDev);
    VirtualDevice getVirtualDevice();
    QStandardItem *getValueItem() const  { return valueItem; }
    QStandardItem *getStateItem() const  { return stateItem; }
    QStandardItem *getReachableItem() const  { return reachableItem; }
    QStandardItem *getTimeToReachItem() const  { return timeToReachItem; }

private:
    QStandardItem *valueItem;
    QStandardItem *stateItem;
    QStandardItem *reachableItem;
    QStandardItem *timeToReachItem;
};

#endif // VIRTUALDEVICEITEM_H
