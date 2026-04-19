#pragma once
#ifndef ADDGROUPDIALOG_H
#define ADDGROUPDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QFormLayout>
#include <QHBoxLayout>

class AddGroupDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddGroupDialog(QWidget* parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle("Добавить группу");
        setFixedSize(300, 120);

        // Поля ввода
        editName = new QLineEdit(this);
        spinCourse = new QSpinBox(this);
        spinCourse->setMinimum(1);
        spinCourse->setMaximum(4);
        spinCourse->setValue(1);

        // Кнопки
        btnOk = new QPushButton("OK", this);
        btnCancel = new QPushButton("Отмена", this);

        connect(btnOk, &QPushButton::clicked, this, &AddGroupDialog::accept);
        connect(btnCancel, &QPushButton::clicked, this, &AddGroupDialog::reject);

        // Layout для формы
        QFormLayout* formLayout = new QFormLayout;
        formLayout->addRow("Название группы:", editName);
        formLayout->addRow("Курс:", spinCourse);

        // Layout для кнопок
        QHBoxLayout* btnLayout = new QHBoxLayout;
        btnLayout->addStretch();
        btnLayout->addWidget(btnOk);
        btnLayout->addWidget(btnCancel);

        // Основной layout
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->addLayout(formLayout);
        mainLayout->addLayout(btnLayout);

        setLayout(mainLayout);
    }

    // Методы для получения введённых значений
    QString groupName() const { return editName->text().trimmed(); }
    int groupCourse() const { return spinCourse->value(); }

    int course() const { return groupCourse(); }





private:
    QLineEdit* editName;
    QSpinBox* spinCourse;
    QPushButton* btnOk;
    QPushButton* btnCancel;
};

#endif // ADDGROUPDIALOG_H
