#include "GroupWindow.h"
#include "MainWindow.h"
#include "ui_GroupWindow.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>

GroupWindow::GroupWindow(int groupId, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::GroupWindow)
    , m_groupId(groupId)
{
    ui->setupUi(this);

    db = QSqlDatabase::database(); // текущее подключение к БД
    qDebug() << "GroupWindow opened with groupId=" << m_groupId;
    // Настраиваем таблицу студентов
    setupStudentsTable();

    // Загружаем информацию о группе
    loadGroupInfo();

    // Сохраняем изменения названия группы и курса при редактировании
    connect(ui->nameGroup, &QLineEdit::editingFinished, this, &GroupWindow::saveGroupInfo);
    connect(ui->spinCourse, qOverload<int>(&QSpinBox::valueChanged), this, &GroupWindow::saveGroupInfo);

    // Кнопки работы со студентами
    connect(ui->btnAdd, &QPushButton::clicked, this, &GroupWindow::on_btnAdd_clicked);
    connect(ui->btnDel, &QPushButton::clicked, this, &GroupWindow::on_btnDel_clicked);
    connect(ui->btnEdit, &QPushButton::clicked, this, &GroupWindow::on_btnEdit_clicked);
    connect(ui->btnImp, &QPushButton::clicked, this, &GroupWindow::on_btnImp_clicked);
}

GroupWindow::~GroupWindow()
{
    delete ui;
}

void GroupWindow::setupStudentsTable()
{
    // Удаляем старую модель, если есть
    if (studentModel) {
        delete studentModel;
        studentModel = nullptr;
    }

    // Создаём новую модель
    studentModel = new QSqlQueryModel(this);

    // Запрос с СОРТИРОВКОЙ ПО АЛФАВИТУ и правильным номером журнала
    QString queryStr = QString(
        "SELECT "
        "   COALESCE(gs.journal_number, "
        "       (SELECT COUNT(*) FROM group_students gs2 "
        "        WHERE gs2.group_id = gs.group_id "
        "        AND gs2.student_id <= gs.student_id)) AS '№ в журнале', "
        "   s.full_name AS 'ФИО' "
        "FROM students s "
        "JOIN group_students gs ON s.id = gs.student_id "
        "WHERE gs.group_id = %1 "
        "ORDER BY s.full_name COLLATE NOCASE"  // Сортировка по алфавиту
    ).arg(m_groupId);

    qDebug() << "Запрос студентов группы:" << queryStr;

    studentModel->setQuery(queryStr, db);

    // Проверяем ошибки
    if (studentModel->lastError().isValid()) {
        qDebug() << "Ошибка запроса студентов:" << studentModel->lastError().text();
        QMessageBox::warning(this, "Ошибка",
            "Не удалось загрузить студентов:\n" + studentModel->lastError().text());
    }

    // Устанавливаем модель в таблицу
    ui->tableStudent->setModel(studentModel);
    ui->tableStudent->resizeColumnsToContents();

    // Обновляем количество студентов
    updateStudentCount();

    // ПЕРЕСЧИТЫВАЕМ НОМЕРА ПРИ КАЖДОЙ ЗАГРУЗКЕ ТАБЛИЦЫ
    DatabaseManager::recalcJournalNumbers(m_groupId, db);
}




void GroupWindow::refreshStudentsTable()
{
    setupStudentsTable();
}

void GroupWindow::loadGroupInfo()
{
    QSqlQuery q(db);
    q.prepare("SELECT name, course FROM groups WHERE id=?");
    q.addBindValue(m_groupId);

    if (!q.exec() || !q.next()) return;

    ui->nameGroup->setText(q.value(0).toString());
    ui->spinCourse->setValue(q.value(1).toInt());
}

void GroupWindow::saveGroupInfo()
{
    QSqlQuery q(db);
    q.prepare("UPDATE groups SET name=?, course=? WHERE id=?");
    q.addBindValue(ui->nameGroup->text().trimmed());
    q.addBindValue(ui->spinCourse->value());
    q.addBindValue(m_groupId);

    if (!q.exec()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить изменения!");
        return;
    }

    // Обновляем список групп в MainWindow
    if (auto mw = qobject_cast<MainWindow*>(parentWidget()))
        mw->refreshGroups();
}

void GroupWindow::updateStudentCount()
{
    if (!studentModel)
        return;

    ui->countStd->setText(QString::number(studentModel->rowCount()));
}






// Кнопки для работы со студентами пока просто показывают сообщение
void GroupWindow::on_btnAdd_clicked()
{
    QMessageBox::information(this, "Добавить студента", "Добавление пока не реализовано");
    refreshStudentsTable();
}

void GroupWindow::on_btnDel_clicked()
{
    QMessageBox::information(this, "Удалить студента", "Удаление пока не реализовано");
    refreshStudentsTable();
}

void GroupWindow::on_btnEdit_clicked()
{
    QMessageBox::information(this, "Изменить студента", "Изменение пока не реализовано");
    refreshStudentsTable();
}

void GroupWindow::on_btnImp_clicked()
{
    QMessageBox::information(this, "Импорт студентов", "Импорт пока не реализован");
    refreshStudentsTable();
}
