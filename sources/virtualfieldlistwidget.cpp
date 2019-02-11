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
#include "virtualfieldlistwidget.h"

VirtualFieldListWidget::VirtualFieldListWidget(VirtualDeviceItemModel *model, QWidget *parent) : QWidget(parent){
    virtualDeviceItemModel = model;

    view = new QTableView(this) ;
    view->setModel(virtualDeviceItemModel);
    view->verticalHeader()->setVisible(false);
    /* QString styleSheet = "::section {" // "QHeaderView::section {"
                          "background-color: lightblue;"
                          "color: black; }";
     view->horizontalHeader()->setStyleSheet(styleSheet);*/

    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setItemDelegateForColumn(VirtualDeviceItemModel::VALUECOLUMN, new SpinBoxValueDelegate(this));
    view->setItemDelegateForColumn(VirtualDeviceItemModel::REACHABLECOLUMN, new CheckBoxDelegate(this));

    SpinBoxTimeToReachDelegate *spinBoxTimDelegate = new SpinBoxTimeToReachDelegate(this);
    spinBoxTimeToReachFactory = new SpinBoxTimeToReachFactory();
    spinBoxTimDelegate->setItemEditorFactory(spinBoxTimeToReachFactory);
    view->setItemDelegateForColumn(VirtualDeviceItemModel::TIMETOANSWERCOLUMN, spinBoxTimDelegate);
    view->setItemDelegateForColumn(VirtualDeviceItemModel::STATECOLUMN, new StateDelegate(this));

    addDeviceButton = new QPushButton(tr("Add device"), this);
    deleteDeviceButton = new QPushButton(tr("Delete device"), this);
    clearAllButton = new QPushButton(tr("Clear all"), this);
    deleteDeviceButton->setEnabled(false);

    QHBoxLayout *layoutButton = new QHBoxLayout();
    layoutButton->addWidget(addDeviceButton);
    layoutButton->addWidget(deleteDeviceButton);
    layoutButton->addWidget(clearAllButton);
    layoutButton->addStretch();

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(view);
    layout->addLayout(layoutButton);
    this->setLayout(layout);

    createConnections();
}
void VirtualFieldListWidget::setMaximumTimeToReach(int maximum){
    spinBoxTimeToReachFactory->setMaximum(maximum);
}
void VirtualFieldListWidget::createConnections(){
    connect(view->selectionModel(), &QItemSelectionModel::selectionChanged, this, &VirtualFieldListWidget::tableViewClicked);
    connect(view->horizontalHeader(), &QHeaderView::sectionClicked, this, &VirtualFieldListWidget::headerViewClicked);
    connect(addDeviceButton, &QPushButton::clicked, this, &VirtualFieldListWidget::addModifyDeviceClicked);
    connect(deleteDeviceButton, &QPushButton::clicked, this, &VirtualFieldListWidget::deleteDeviceClicked);
    connect(clearAllButton, &QPushButton::clicked, this, &VirtualFieldListWidget::clearAllClicked);
}
void VirtualFieldListWidget::tableViewClicked(const QItemSelection & /* unused */, const QItemSelection & /* unused */){
    if(view->selectionModel()->hasSelection()){
        deleteDeviceButton->setEnabled(true);
        addDeviceButton->setText(tr("Modify device"));
    }
    else{
        deleteDeviceButton->setEnabled(false);
        addDeviceButton->setText(tr("Add device"));
    }
}
void VirtualFieldListWidget::headerViewClicked(const int logicalIndex){
    virtualDeviceItemModel->sort(logicalIndex);
}
void VirtualFieldListWidget::addModifyDeviceClicked(){
    QItemSelectionModel *selectedModel = view->selectionModel();
    if(selectedModel->hasSelection()){
        VirtualDeviceItem *item = static_cast<VirtualDeviceItem*>(virtualDeviceItemModel
                ->itemFromIndex(selectedModel->currentIndex().siblingAtColumn(VirtualDeviceItemModel::ADDRESSCOLUMN)));
        int address = item->data(Qt::EditRole).toInt();
        if(virtualDeviceItemModel->hasAddress(address)){
            VirtualDevice dev = item->getVirtualDevice();
            VirtualDeviceEditingDialog deviceDialogEditing(&dev, tr("Modify device"), this);
            int result = deviceDialogEditing.exec();
            if(result == QDialog::Accepted){
                if(dev.getAddress() != address && virtualDeviceItemModel->hasAddress(dev.getAddress())){
                    QMessageBox::warning(this, tr("Error"), tr("Device already exists!"));
                }else if(dev.getAddress() != address){
                    virtualDeviceItemModel->removeRow(item->row());
                    virtualDeviceItemModel->addModifyVirtualDevice(dev);
                }else{
                    virtualDeviceItemModel->addModifyVirtualDevice(dev);
                }
            }
        }
    }else{//add new device
        VirtualDevice dev;
        VirtualDeviceEditingDialog deviceDialogEditing(&dev, tr("Add new device"), this);
        int result = deviceDialogEditing.exec();
        if(result == QDialog::Accepted){
            if(virtualDeviceItemModel->hasAddress(dev.getAddress())){
                QMessageBox::warning(this, tr("Error"), tr("Device already exists!"));
            }else{
                virtualDeviceItemModel->addModifyVirtualDevice(dev);
            }
        }
    }
}
void VirtualFieldListWidget::deleteDeviceClicked(){
    QItemSelectionModel *selectedModel = view->selectionModel();
    if(selectedModel->hasSelection()){
        VirtualDeviceItem *item = static_cast<VirtualDeviceItem*>(virtualDeviceItemModel
                ->itemFromIndex(selectedModel->currentIndex().siblingAtColumn(VirtualDeviceItemModel::ADDRESSCOLUMN)));
        int address = item->data(Qt::EditRole).toInt();
        QString question = tr("Are you sure you want to delete item: ");
        int result = QMessageBox::question(this, tr("Confirmation"), question + QString::number(address) + " ?",
                              QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::Yes){
            virtualDeviceItemModel->removeRow(item->row());
        }
    }
}
void VirtualFieldListWidget::clearAllClicked(){
    int result = QMessageBox::question(this, tr("Confirmation"), tr("Are you sure you want to delete everything ?"),
                          QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes){
        virtualDeviceItemModel->clear();
    }
}
