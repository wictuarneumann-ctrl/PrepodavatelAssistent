#include "DatabaseManager.h"
#include <QtSql/QSqlError>
#include <QDebug>
#include <QSqlQuery>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::open(const QString& path) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    if (!m_db.open()) {
        qDebug() << "Ошибка подключения к БД:" << m_db.lastError().text();
        return false;
    }

    qDebug() << "База подключена!";
    return true;
}



void DatabaseManager::recalcAllJournalNumbers(QSqlDatabase db)
{
    QSqlQuery q(db);
    q.exec("SELECT id FROM groups");

    while (q.next()) {
        int groupId = q.value(0).toInt();
        recalcJournalNumbers(groupId, db);
    }

    qDebug() << "Пересчитаны номера для всех групп";
}

void DatabaseManager::recalcJournalNumbers(int groupId, QSqlDatabase db)
{
    QSqlQuery q(db);

    // Получаем студентов группы В АЛФАВИТНОМ ПОРЯДКЕ
    q.prepare(
        "SELECT s.id "
        "FROM students s "
        "JOIN group_students gs ON s.id = gs.student_id "
        "WHERE gs.group_id = ? "
        "ORDER BY s.full_name COLLATE NOCASE"  // COLLATE NOCASE для корректной сортировки
    );
    q.addBindValue(groupId);

    if (!q.exec()) {
        qDebug() << "Ошибка recalcJournalNumbers:" << q.lastError().text();
        return;
    }

    int number = 1;
    while (q.next()) {
        int studentId = q.value(0).toInt();

        QSqlQuery update(db);
        update.prepare(
            "UPDATE group_students "
            "SET journal_number = ? "
            "WHERE group_id = ? AND student_id = ?"
        );
        update.addBindValue(number);
        update.addBindValue(groupId);
        update.addBindValue(studentId);

        if (!update.exec()) {
            qDebug() << "Ошибка обновления номера для studentId" << studentId
                << ":" << update.lastError().text();
        }
        number++;
    }

    qDebug() << "Пересчитаны номера для группы" << groupId << ":" << (number - 1) << "студентов";
}


QSqlDatabase DatabaseManager::db() {
    return m_db;
}
