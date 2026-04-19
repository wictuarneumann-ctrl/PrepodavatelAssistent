#pragma once
#include <QtSql/QSqlDatabase>

class DatabaseManager {
public:
    static DatabaseManager& instance();

    bool open(const QString& path = "talium.db");
    QSqlDatabase db();

    // Делаем методы публичными и статическими
    static void recalcJournalNumbers(int groupId, QSqlDatabase db);
    static void recalcAllJournalNumbers(QSqlDatabase db);
private:
    DatabaseManager() = default;
    QSqlDatabase m_db;
};
