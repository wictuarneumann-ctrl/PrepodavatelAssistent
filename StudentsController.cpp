#include "StudentsController.h"
#include <QSqlRecord>
#include <QMessageBox>

StudentsController::StudentsController(QObject* parent)
    : QObject(parent)
{
    // Создание модели
    m_model = new QSqlTableModel(this);
    m_model->setTable("students");
    m_model->select();

    // Основной виджет
    m_view = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_view);

    // Кнопки
    addBtn = new QPushButton("Добавить студента");
    editBtn = new QPushButton("Изменить");
    deleteBtn = new QPushButton("Удалить");

    // Таблица
    m_table = new QTableView();
    m_table->setModel(m_model);

    // Добавление в интерфейс
    layout->addWidget(addBtn);
    layout->addWidget(editBtn);
    layout->addWidget(deleteBtn);
    layout->addWidget(m_table);

    // Подключение сигналов
    connect(addBtn, &QPushButton::clicked, this, &StudentsController::addStudent);
    connect(editBtn, &QPushButton::clicked, this, &StudentsController::editStudent);
    connect(deleteBtn, &QPushButton::clicked, this, &StudentsController::deleteStudent);
}

QWidget* StudentsController::view()
{
    return m_view;
}

void StudentsController::addStudent()
{
    // TODO: Окно AddStudentDialog
    QMessageBox::information(m_view, "Добавление", "Тут будет добавление студента");
}

void StudentsController::editStudent()
{
    // TODO: Окно EditStudentDialog
    QMessageBox::information(m_view, "Изменение", "Тут будет изменение студента");
}

void StudentsController::deleteStudent()
{
    QModelIndex index = m_table->currentIndex();
    if (!index.isValid()) return;

    m_model->removeRow(index.row());
    m_model->submitAll();
    m_model->select();
}
