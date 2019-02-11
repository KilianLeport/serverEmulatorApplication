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
#ifndef VIRTUALFIELDLISTWIDGET_H
#define VIRTUALFIELDLISTWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QTableView>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>

#include "virtualdeviceitemmodel.h"
#include "virtualdeviceitem.h"
#include "virtualdeviceeditingdialog.h"
#include "customdelegates.h"

class VirtualFieldListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VirtualFieldListWidget(VirtualDeviceItemModel *model, QWidget *parent = nullptr);
    void setMaximumTimeToReach(int maximum);

private:
    void createConnections();
    void headerViewClicked(const int logicalIndex);
    void tableViewClicked(const QItemSelection & /* item */, const QItemSelection & /* item */);
    void addModifyDeviceClicked();
    void deleteDeviceClicked();
    void clearAllClicked();

private:
    VirtualDeviceItemModel* virtualDeviceItemModel;
    QPushButton *addDeviceButton;
    QPushButton *deleteDeviceButton;
    QPushButton *clearAllButton;
    QTableView  *view;

    SpinBoxTimeToReachFactory *spinBoxTimeToReachFactory;
};

#endif // VIRTUALFIELDLISTWIDGET_H
