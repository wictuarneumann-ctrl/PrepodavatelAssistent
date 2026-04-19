#pragma once
#ifndef GROUPWINDOW_H
#define GROUPWINDOW_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQueryModel>

class DatabaseManager;

namespace Ui {
    class GroupWindow;
}

class GroupWindow : public QDialog
{
    Q_OBJECT

public:
    explicit GroupWindow(int groupId, QWidget* parent = nullptr);
    ~GroupWindow();

private slots:
    void loadGroupInfo();
    void saveGroupInfo();
    void updateStudentCount();
    void refreshStudentsTable();

    void on_btnAdd_clicked();
    void on_btnDel_clicked();
    void on_btnEdit_clicked();
    void on_btnImp_clicked();

private:
    Ui::GroupWindow* ui;
    int m_groupId;
    QSqlDatabase db;
    QSqlQueryModel* studentModel;
    void setupStudentsTable();
};

#endif // GROUPWINDOW_H
