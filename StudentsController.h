#ifndef STUDENTSCONTROLLER_H
#define STUDENTSCONTROLLER_H

#include <QObject>
#include <QWidget>
#include <QSqlTableModel>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>

class StudentsController : public QObject
{
    Q_OBJECT

public:
    explicit StudentsController(QObject* parent = nullptr);
    QWidget* view();   // <--- чтобы MainWindow мог показывать виджет

signals:

private slots:
    void addStudent();
    void editStudent();
    void deleteStudent();

private:
    QWidget* m_view;           // Виджет, который будет показываться на экране
    QTableView* m_table;       // Таблица для отображения студентов
    QSqlTableModel* m_model;   // Модель студентов
    QPushButton* addBtn;
    QPushButton* editBtn;
    QPushButton* deleteBtn;
};

#endif // STUDENTSCONTROLLER_H
