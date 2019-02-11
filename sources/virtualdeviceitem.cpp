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
#include "virtualdeviceitem.h"

VirtualDeviceItem::VirtualDeviceItem(VirtualDevice vDev)
{
    this->setFlags(flags() & ~Qt::ItemIsEditable);
    this->setData(QVariant(vDev.getAddress()), Qt::EditRole);
    this->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);

    stateItem = new QStandardItem();
    stateItem->setFlags(stateItem->flags() & ~Qt::ItemIsEditable);
    stateItem->setData(QVariant(vDev.getState()), Qt::EditRole);
    stateItem->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);

    valueItem = new QStandardItem();
    valueItem->setData(QVariant(vDev.getValue()), Qt::EditRole);
    valueItem->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);

    reachableItem = new QStandardItem();
    reachableItem->setData(QVariant(vDev.getReachable()), Qt::EditRole);
    reachableItem->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);

    timeToReachItem = new QStandardItem();
    timeToReachItem->setData(QVariant(vDev.getTimeToReach()), Qt::EditRole);
    timeToReachItem->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
}

VirtualDevice VirtualDeviceItem::getVirtualDevice(){
    VirtualDevice vDev = VirtualDevice(static_cast<quint8>(this->data(Qt::EditRole).toInt()),
                                       valueItem->data(Qt::EditRole).toInt(),
                                       static_cast<quint8>(stateItem->data(Qt::EditRole).toInt()),
                                       reachableItem->data(Qt::EditRole).toBool(),
                                       timeToReachItem->data(Qt::EditRole).toInt());
    return vDev;
}
