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
#include "virtualdeviceeditingdialog.h"

VirtualDeviceEditingDialog::VirtualDeviceEditingDialog(VirtualDevice *device, const QString &title, QWidget *parent ) : QDialog(parent){
    this->device = device;
    QString address = QString::number(device->getAddress());
    if(device->getAddress() == 255)
        address = "";

    lineEditAddress = new QLineEdit(address, this);
    lineEditValue = new QLineEdit(QString::number(device->getValue()), this);
    comboBoxState = new QComboBox(this);
    comboBoxState->addItems(VirtualDevice::stateList);
    if(device->getState() < VirtualDevice::stateList.size())
        comboBoxState->setCurrentIndex(device->getState());
    checkboxReachable = new QCheckBox(tr("Reachable"), this);
    checkboxReachable->setChecked(device->getReachable());

    lineEditAddress->setValidator(new QIntValidator(0, VirtualDevice::MAXIMUMADDRESS, this));
    lineEditValue->setValidator(new QIntValidator(0, VirtualDevice::MAXIMUMVALUE, this));
    QObject::connect(lineEditAddress, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged()));
    QObject::connect(lineEditValue, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged()));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(buttonOKClicked()));
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    this->textChanged(); //Actualize OK button state

    QGroupBox *formGroupBox = new QGroupBox(tr("Device edition"), this);

    QFormLayout *formLayout = new QFormLayout(this);
    formLayout->addRow(tr("&Address:"), lineEditAddress);
    formLayout->addRow(tr("&State:"), comboBoxState);
    formLayout->addRow(tr("&Value:"), lineEditValue);
    formLayout->addRow(tr("&Reachable:"), checkboxReachable);
    formLayout->setRowWrapPolicy(QFormLayout::WrapLongRows);
    formGroupBox->setLayout(formLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(formGroupBox);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(title);
    setMinimumSize(350,150);
}
void VirtualDeviceEditingDialog::textChanged(){
    if(!lineEditAddress->text().isEmpty() && !lineEditValue->text().isEmpty())
        buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    else
        buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}
void VirtualDeviceEditingDialog::buttonOKClicked(){
    device->setAddress(static_cast<quint8>(lineEditAddress->text().toInt()));
    device->setValue(lineEditValue->text().toInt());
    device->setState(static_cast<quint8>(comboBoxState->currentIndex()));
    device->setReachable(checkboxReachable->isChecked());
    QDialog::accept();
}

