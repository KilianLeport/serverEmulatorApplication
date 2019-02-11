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
#ifndef VIRTUALDEVICEEDITINGDIALOG_H
#define VIRTUALDEVICEEDITINGDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QIntValidator>
#include <QPushButton>
#include <QComboBox>

#include "virtualdevice.h"

class VirtualDeviceEditingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VirtualDeviceEditingDialog(VirtualDevice *device, const QString &title, QWidget *parent = nullptr);

private slots:
    void buttonOKClicked();
    void textChanged();

private:
    VirtualDevice *device;
    QLineEdit *lineEditAddress;
    QLineEdit *lineEditValue;
    QComboBox *comboBoxType;
    QComboBox *comboBoxState;
    QCheckBox *checkboxReachable;
    QDialogButtonBox *buttonBox;

};

#endif // VIRTUALDEVICEEDITINGDIALOG_H
