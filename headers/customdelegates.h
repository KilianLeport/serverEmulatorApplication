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
#ifndef CUSTOMDELEGATES_H
#define CUSTOMDELEGATES_H

#include <QWidget>
#include <QStyledItemDelegate>
#include <QSpinBox>
#include <QCheckBox>
#include <QPainter>
#include <QApplication>
#include <QItemEditorFactory>
#include <QDebug>

#include "virtualdevice.h"
#include "device.h"

class SpinBoxValueDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    SpinBoxValueDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class SpinBoxTimeToReachDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    SpinBoxTimeToReachDelegate(QObject *parent = nullptr);

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class SpinBoxTimeToReachFactory : public QItemEditorFactory
{

public:
    SpinBoxTimeToReachFactory(int maximum = 0);
    QWidget *createEditor(int, QWidget *parent) const override;
    void setMaximum(int maximum);

private:
    int maximum;
};

class CheckBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    CheckBoxDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;

    bool editorEvent(QEvent *event,
                     QAbstractItemModel *model,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index) override;
};

class StateDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    StateDelegate(QObject *parent = nullptr);
    QString displayText(const QVariant &value, const QLocale &) const override;
};

class TypeDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    TypeDelegate(QObject *parent = 0);
    QString displayText(const QVariant &value, const QLocale &) const override;
};
#endif // CUSTOMDELEGATES_H
