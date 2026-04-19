#pragma once
#ifndef DIALOGADDDISCIPLINE_H
#define DIALOGADDDISCIPLINE_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

namespace Ui {
    class DialogAddDiscipline;
}

class DialogAddDiscipline : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddDiscipline(QWidget* parent = nullptr);
    ~DialogAddDiscipline();

    QString getName() const;

private slots:
    void on_btnOK_clicked();
    void on_btnCancel_clicked();

private:
    Ui::DialogAddDiscipline* ui;
};

#endif // DIALOGADDDISCIPLINE_H
