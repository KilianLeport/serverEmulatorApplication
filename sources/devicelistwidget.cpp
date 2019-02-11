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
#include "devicelistwidget.h"

DeviceListWidget::DeviceListWidget(DeviceItemModel *model, QWidget *parent) : QWidget(parent){
    this->deviceItemModel = model;
    view = new QTableView(this) ;
    view->setModel(deviceItemModel);
    view->verticalHeader()->setVisible(false);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);

    view->setItemDelegateForColumn(DeviceItemModel::TYPECOLUMN, new TypeDelegate(this));
    view->setItemDelegateForColumn(DeviceItemModel::STATECOLUMN, new StateDelegate(this));

    connect(view->horizontalHeader(), &QHeaderView::sectionClicked, this, &DeviceListWidget::headerViewClicked);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(view);
    this->setLayout(layout);
}
void DeviceListWidget::headerViewClicked(const int logicalIndex){
    deviceItemModel->sort(logicalIndex);
}
