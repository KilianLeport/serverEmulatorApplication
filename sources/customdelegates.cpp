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
#include "customdelegates.h"

//***************SpinBoxValueDelegate********************************
SpinBoxValueDelegate::SpinBoxValueDelegate(QObject *parent)
    : QStyledItemDelegate(parent){
}
QWidget *SpinBoxValueDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &/* index */) const{
    QSpinBox *editor = new QSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(0);
    editor->setMaximum(VirtualDevice::MAXIMUMVALUE);

    return editor;
}
void SpinBoxValueDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const{
    int value = index.model()->data(index, Qt::EditRole).toInt();

    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->setValue(value);
}
void SpinBoxValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const{
    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->interpretText();
    int value = spinBox->value();

    model->setData(index, value, Qt::EditRole);
}
void SpinBoxValueDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const{
    editor->setGeometry(option.rect);
}

//***************SpinBoxTimeToReachDelegate********************************
SpinBoxTimeToReachDelegate::SpinBoxTimeToReachDelegate(QObject *parent)
    : QStyledItemDelegate(parent){
}
void SpinBoxTimeToReachDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const{
    int value = index.model()->data(index, Qt::EditRole).toInt();

    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->setValue(value);
}
void SpinBoxTimeToReachDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const{
    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->interpretText();
    int value = spinBox->value();

    model->setData(index, value, Qt::EditRole);
}
void SpinBoxTimeToReachDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const{
    editor->setGeometry(option.rect);
}

//***************SpinBoxTimeToReachFactory********************************
SpinBoxTimeToReachFactory::SpinBoxTimeToReachFactory(int maximum){
    this->maximum = maximum;
}
QWidget *SpinBoxTimeToReachFactory::createEditor(int /*userType*/, QWidget *parent) const{  
    QSpinBox *editor = new QSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(VirtualDevice::MinAnswerTime);
    editor->setMaximum(maximum);
    editor->setSingleStep(100);
    editor->setSuffix("ms");
    return editor;
}
void SpinBoxTimeToReachFactory::setMaximum(int maximum){
    this->maximum = maximum;
}

//***************CheckBoxDelegate********************************
CheckBoxDelegate::CheckBoxDelegate(QObject *parent)
    : QStyledItemDelegate(parent){
}
QWidget *CheckBoxDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &/* index */) const{
    QCheckBox *checkBox = new QCheckBox(parent);

    return checkBox;
}

void CheckBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const{
    bool value = index.model()->data(index, Qt::EditRole).toBool();

    QCheckBox *checkBox = static_cast<QCheckBox*>(editor);
    checkBox->setChecked(value);
}

void CheckBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const{
    QCheckBox *checkBox = static_cast<QCheckBox*>(editor);
    bool value = checkBox->isChecked();

    model->setData(index, value, Qt::EditRole);
}
void CheckBoxDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option, const QModelIndex &index) const{
   painter->save();

   QStyleOptionViewItem opt = option;
   const QWidget *widget = option.widget;
   opt.text = "";

   //option
   QStyle *style = widget ? widget->style() : QApplication::style();

   //handle selection color
   style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

   //retrieve data
   bool data = index.model()->data(index, Qt::EditRole).toBool();

   //create CheckBox style
   QStyleOptionButton checkboxstyle;
   QRect checkbox_rect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator, &checkboxstyle);

   //center
   checkboxstyle.rect = option.rect;
   checkboxstyle.rect.setLeft(option.rect.x() +
                              option.rect.width()/2 - checkbox_rect.width()/2);
   //checked or not checked
   if(data)
       checkboxstyle.state = QStyle::State_On|QStyle::State_Enabled;
   else
       checkboxstyle.state = QStyle::State_Off|QStyle::State_Enabled;

   QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkboxstyle, painter);

    painter->restore();
}
void CheckBoxDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const{
    QStyleOptionButton checkboxstyle;
    QRect checkbox_rect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator, &checkboxstyle);

    //center
    checkboxstyle.rect = option.rect;
    checkboxstyle.rect.setLeft(option.rect.x() +
                               option.rect.width()/2 - checkbox_rect.width()/2);

    editor->setGeometry(checkboxstyle.rect);
}

bool CheckBoxDelegate::editorEvent(QEvent *event,
                            QAbstractItemModel *model,
                            const QStyleOptionViewItem &option,
                            const QModelIndex &index){
    if (event->type() == QEvent::MouseButtonDblClick)
        return true;
    if (event->type() == QEvent::MouseButtonRelease)
    {
        bool value = index.data(Qt::EditRole).toBool();
        // invert checkbox state
        model->setData(index, !value, (Qt::EditRole));
        return true;
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
//***************StateDelegate********************************
StateDelegate::StateDelegate(QObject *parent)
    : QStyledItemDelegate(parent){
}
QString StateDelegate::displayText(const QVariant &value, const QLocale &/*locale*/) const{
    return VirtualDevice::stateList.value(value.toInt(), QString::number(value.toInt()));
}
//***************TypeDelegate********************************
TypeDelegate::TypeDelegate(QObject *parent)
    : QStyledItemDelegate(parent){
}
QString TypeDelegate::displayText(const QVariant &value, const QLocale &/*locale*/) const{
    return Device::typeList.value(value.toInt(), QString::number(value.toInt()));
}
