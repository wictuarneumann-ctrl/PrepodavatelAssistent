#include <QtWidgets/QApplication>
#include "MainWindow.h"
#include "GroupWindow.h"
#include "DatabaseManager.h"
#include <QFile>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    // Загружаем стили
    QFile styleFile(":/MainWindow/styles/styles.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString style = styleFile.readAll();
        a.setStyleSheet(style);
        styleFile.close();
    }

    // Подключение к БД
    DatabaseManager::instance().open("talium.db");

    MainWindow w;
    w.show();

    return a.exec();
}
