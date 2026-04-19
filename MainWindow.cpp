#include "mainwindow.h"
#include "ui_MainWindow.h"
#include "GroupWindow.h"
#include "DatabaseManager.h"
#include "AddStudentDialog.h"
#include "AddGroupDialog.h"
#include "DialogAddDiscipline.h"
#include "AppointDisciplineDialog.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardItemModel>
#include <QDebug>
#include <QSqlRelationalTableModel>
#include <QListWidgetItem>
#include <QSqlRecord>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QVector>
#include <QRegularExpression>

// =============================
// MainWindow
// =============================
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowClass())
{
    ui->setupUi(this);
    debugDumpDatabase();

    loadDebtsGroups();
    loadDebtsDisciplines();

    // ==========================
    // Настройка таблицы студентов
    // ==========================
    ui->tableStudents->setSelectionBehavior(QTableView::SelectRows);
    ui->tableStudents->setEditTriggers(QTableView::NoEditTriggers);

    // временная пустая модель для безопасной инициализации header
    QStandardItemModel* tmpModel = new QStandardItemModel(1, 5, this);
    ui->tableStudents->setModel(tmpModel);

    ui->tableStudents->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableStudents->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableStudents->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tableStudents->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->tableStudents->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);

    // ==========================
    // Основная модель
    // ==========================
    model = new QSqlRelationalTableModel(this);
    model->setTable("students");
    model->setRelation(2, QSqlRelation("groups", "id", "name")); // столбец 2 = group_id
    model->relationModel(2)->select(); // загружаем названия групп

    // ==========================
    // Прокси модель для фильтрации
    // ==========================
    proxyModel = new StudentsFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui->tableStudents->setModel(proxyModel);

    // ==========================
    // Header с комбобоксами
    // ==========================
    comboHeader = new HeaderWithCombo(Qt::Horizontal, ui->tableStudents);
    ui->tableStudents->setHorizontalHeader(comboHeader);

    connect(comboHeader, &HeaderWithCombo::groupChanged, this, [=](const QString& text) {
        proxyModel->groupFilter = text;
        proxyModel->refreshFilter();
        });

    connect(comboHeader, &HeaderWithCombo::courseChanged, this, [=](const QString& text) {
        proxyModel->courseFilter = text;
        proxyModel->refreshFilter();
        });

    // ==========================
    // Поиск по ФИО
    // ==========================
    connect(ui->lineSearchFIO, &QLineEdit::textChanged, this, &MainWindow::onSearchFIOChanged);

    // ==========================
    // Кнопки добавления/удаления/импорта/изменения студентов
    // ==========================
    connect(ui->btnAddStd, &QPushButton::clicked, this, &MainWindow::onAddStudentClicked);
    connect(ui->btnDelStd, &QPushButton::clicked, this, &MainWindow::onDeleteStudentClicked);
    connect(ui->btnImpStd, &QPushButton::clicked, this, &MainWindow::onImportStudentsClicked);
    connect(ui->btnEditStd, &QPushButton::clicked, this, &MainWindow::onEditStudentClicked);

    // ==========================
    // Кнопки добавления/удаления/импорта ГРУПП
    // ==========================
    connect(ui->btnAddGroup, &QPushButton::clicked, this, &MainWindow::onAddGroupClicked);


    // ==========================
    // Кнопки добавления/изменения/удаления/назначения ДИСЦИПЛИН
    // ==========================
    connect(ui->btnAppointDisciplines, &QPushButton::clicked, this, &MainWindow::on_btnAppointDiscipline_clicked);

    // ==========================
    // Кнопки перемещения по левому меню и страницы студенты/группы
    // ==========================

    connect(ui->BtnStd, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageStds);
        });

    connect(ui->BtnGroup, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageGroups);
        });

    connect(ui->btnStdGroup, &QPushButton::clicked, this, [this]() {
        ui->LeftMenuWidget->setCurrentWidget(ui->pageStdGroup);
        });

    connect(ui->btnDisciplines, &QPushButton::clicked, this, [this]() {
        ui->LeftMenuWidget->setCurrentWidget(ui->pageDisciplines);
        });

    connect(ui->btnDebts, &QPushButton::clicked, this, [this]() {
        ui->LeftMenuWidget->setCurrentWidget(ui->pageDebts);
        });

    connect(ui->btnAccounting, &QPushButton::clicked, this, [this]() {
        ui->LeftMenuWidget->setCurrentWidget(ui->pageAccounting);
        });

    connect(ui->btnReports, &QPushButton::clicked, this, [this]() {
        ui->LeftMenuWidget->setCurrentWidget(ui->pageReports);
        });

    connect(ui->btnSettings, &QPushButton::clicked, this, [this]() {
        ui->LeftMenuWidget->setCurrentWidget(ui->pageSettings);
        });

 
    connect(ui->btnNoPass, &QPushButton::clicked, this, [=]() { setDebtMark("не сдал"); });
    connect(ui->btnTwo, &QPushButton::clicked, this, [=]() { setDebtMark("2"); });
    connect(ui->btnThree, &QPushButton::clicked, this, [=]() { setDebtMark("3"); });
    connect(ui->btnFour, &QPushButton::clicked, this, [=]() { setDebtMark("4"); });
    connect(ui->btnFive, &QPushButton::clicked, this, [=]() { setDebtMark("5"); });
    connect(ui->btnAllow, &QPushButton::clicked, this, [=]() { setDebtMark("допуск"); });
    connect(ui->btnNoAllow, &QPushButton::clicked, this, [=]() { setDebtMark("не допуск"); });
    connect(ui->btnDel, &QPushButton::clicked, this, [=]() { setDebtMark(""); });

    // ==========================
    // Стартовые страницы
    // ==========================
    ui->LeftMenuWidget->setCurrentWidget(ui->pageStdGroup);
    ui->stackedWidget->setCurrentWidget(ui->pageStds);

    // ==========================
    // Первичная настройка модели
    // ==========================
    updateUI();
}

// ==========================
// Универсальное обновление модели студентов
// ==========================
void MainWindow::refreshModel()
{
    // Пересчитаем номера
    DatabaseManager::recalcAllJournalNumbers(QSqlDatabase::database());

    // Создаём новый запрос для таблицы студентов
    QSqlQuery query(QSqlDatabase::database());
    query.prepare(
        "SELECT gs.journal_number AS journal_number, "
        "s.full_name AS full_name, "
        "g.name AS group_name, "
        "g.course AS course "
        "FROM students s "
        "JOIN group_students gs ON s.id = gs.student_id "
        "JOIN groups g ON s.group_id = g.id "
        "ORDER BY g.name, s.full_name"
    );
    query.exec();

    // Создаём QSqlQueryModel
    QSqlQueryModel* m = new QSqlQueryModel(this);
    m->setQuery(QSqlQuery(
        "SELECT gs.journal_number AS journal_number, "
        "s.full_name AS full_name, "
        "g.name AS group_name, "
        "g.course AS course "
        "FROM students s "
        "JOIN group_students gs ON s.id = gs.student_id "
        "JOIN groups g ON s.group_id = g.id "
        "ORDER BY g.name, s.full_name",
        QSqlDatabase::database()
    ));

    // Подключаем к QTableView
    ui->tableStudents->setModel(m);

    // Настраиваем колонки
    m->setHeaderData(0, Qt::Horizontal, "№ в журнале");
    m->setHeaderData(1, Qt::Horizontal, "ФИО");
    m->setHeaderData(2, Qt::Horizontal, "Группа");
    m->setHeaderData(3, Qt::Horizontal, "Курс");

    ui->tableStudents->resizeColumnsToContents();

    // Обновляем Header Combo
    updateHeaderCombos();

    // Если у тебя есть прокси-модель для фильтров
    proxyModel->setSourceModel(m);
    proxyModel->invalidate();
}


// ==========================
// Обновление комбобоксов Header
// ==========================
void MainWindow::updateHeaderCombos()
{
    if (!comboHeader) return;

    comboHeader->comboGroup->blockSignals(true);
    comboHeader->comboGroup->clear();
    comboHeader->comboGroup->addItem("Все группы");

    QSqlQuery qGroups("SELECT name FROM groups ORDER BY name");
    while (qGroups.next())
        comboHeader->comboGroup->addItem(qGroups.value(0).toString());

    comboHeader->comboGroup->blockSignals(false);

    comboHeader->comboCourse->blockSignals(true);
    comboHeader->comboCourse->clear();
    comboHeader->comboCourse->addItem("Все курсы");

    QSqlQuery qCourses("SELECT DISTINCT course FROM groups ORDER BY course");
    while (qCourses.next())
        comboHeader->comboCourse->addItem(qCourses.value(0).toString());

    comboHeader->comboCourse->blockSignals(false);
}

// ==========================
// Добавление студента
// ==========================
void MainWindow::onAddStudentClicked()
{
    AddStudentDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;

    QString fio = dlg.fio().trimmed();
    QString groupName = dlg.group().trimmed();
    int course = dlg.course();

    if (fio.isEmpty() || groupName.isEmpty() || course <= 0) {
        QMessageBox::warning(this, "Ошибка", "Все поля должны быть заполнены корректно!");
        return;
    }

    // Получаем или создаём группу
    int groupId = 0;
    QSqlQuery q;
    q.prepare("SELECT id FROM groups WHERE name = ?");
    q.addBindValue(groupName);
    if (q.exec() && q.next())
        groupId = q.value(0).toInt();
    else {
        QSqlQuery qInsert;
        qInsert.prepare("INSERT INTO groups (name, course) VALUES (?, ?)");
        qInsert.addBindValue(groupName);
        qInsert.addBindValue(course);
        if (!qInsert.exec()) {
            QMessageBox::warning(this, "Ошибка", "Не удалось добавить группу:\n" + qInsert.lastError().text());
            return;
        }
        groupId = qInsert.lastInsertId().toInt();
    }

    // Добавляем студента
    QSqlQuery qInsertStudent;
    qInsertStudent.prepare("INSERT INTO students (full_name, group_id, course) VALUES (?, ?, ?)");
    qInsertStudent.addBindValue(fio);
    qInsertStudent.addBindValue(groupId);
    qInsertStudent.addBindValue(course);

    if (!qInsertStudent.exec()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось добавить студента:\n" + qInsertStudent.lastError().text());
        return;
    }

    int studentId = qInsertStudent.lastInsertId().toInt();

    // Добавляем запись в group_students
    QSqlQuery qInsertJournal;
    qInsertJournal.prepare("INSERT INTO group_students (group_id, student_id) VALUES (?, ?)");
    qInsertJournal.addBindValue(groupId);
    qInsertJournal.addBindValue(studentId);
    qInsertJournal.exec();

    // Пересчёт номеров журнала для этой группы
    DatabaseManager::recalcJournalNumbers(groupId, QSqlDatabase::database());




    // Обновляем интерфейс
    updateUI();

    QMessageBox::information(this, "Успех", QString("Студент \"%1\" успешно добавлен!").arg(fio));
}


// ==========================
// Удаление студента
// ==========================
void MainWindow::onDeleteStudentClicked()
{
    QModelIndex index = ui->tableStudents->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "Удаление студента", "Выберите студента для удаления!");
        return;
    }

    QModelIndex sourceIndex = proxyModel->mapToSource(index);
    int row = sourceIndex.row();

    int studentId = model->data(model->index(row, 0)).toInt();
    int groupId = model->data(model->index(row, 2)).toInt();
    QString fio = model->data(model->index(row, 1)).toString();

    auto reply = QMessageBox::question(
        this,
        "Подтверждение удаления",
        QString("Вы действительно хотите удалить студента \"%1\"?").arg(fio),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply != QMessageBox::Yes) return;

    // Удаляем студента из students
    model->removeRow(row);
    model->submitAll();

    // Удаляем запись из group_students
    QSqlQuery qDelete;
    qDelete.prepare("DELETE FROM group_students WHERE student_id=?");
    qDelete.addBindValue(studentId);
    qDelete.exec();

    // Пересчёт номеров журнала
    DatabaseManager::recalcJournalNumbers(groupId, QSqlDatabase::database());

    // Обновляем интерфейс
    updateUI();

    QMessageBox::information(this, "Удалено", QString("Студент \"%1\" успешно удалён!").arg(fio));
}


// ==========================
// Изменение студента
// ==========================

void AddStudentDialog::setData(const QString& fio, const QString& group, int course)
{
    lineFIO->setText(fio);
    comboGroup->setCurrentText(group);
    comboCourse->setCurrentText(QString::number(course));

}

void MainWindow::onEditStudentClicked()
{
    QModelIndex index = ui->tableStudents->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "Редактирование", "Выберите студента!");
        return;
    }

    QModelIndex sourceIndex = proxyModel->mapToSource(index);
    int row = sourceIndex.row();

    int studentId = model->data(model->index(row, 0)).toInt();
    QString oldFio = model->data(model->index(row, 1)).toString();
    int oldGroupId = model->data(model->index(row, 2)).toInt();
    int oldCourse = model->data(model->index(row, 3)).toInt();

    QString oldGroupName = getGroupNameById(oldGroupId); // нужна функция ниже

    AddStudentDialog dlg(this);
    dlg.setData(oldFio, oldGroupName, oldCourse);

    if (dlg.exec() != QDialog::Accepted) return;

    QString newFio = dlg.fio().trimmed();
    QString newGroupName = dlg.group().trimmed();
    int newCourse = dlg.course();

    // Обновляем students
    QSqlQuery qUpdate;
    qUpdate.prepare("UPDATE students SET full_name=?, course=? WHERE id=?");
    qUpdate.addBindValue(newFio);
    qUpdate.addBindValue(newCourse);
    qUpdate.addBindValue(studentId);
    qUpdate.exec();

    // Получаем или создаём новую группу
    int newGroupId = 0;
    QSqlQuery q;
    q.prepare("SELECT id FROM groups WHERE name=?");
    q.addBindValue(newGroupName);
    if (q.exec() && q.next())
        newGroupId = q.value(0).toInt();
    else {
        QSqlQuery qInsert;
        qInsert.prepare("INSERT INTO groups (name, course) VALUES (?, ?)");
        qInsert.addBindValue(newGroupName);
        qInsert.addBindValue(newCourse);
        qInsert.exec();
        newGroupId = qInsert.lastInsertId().toInt();
    }

    // Обновляем group_students
    QSqlQuery qUpdateGS;
    qUpdateGS.prepare("UPDATE group_students SET group_id=? WHERE student_id=?");
    qUpdateGS.addBindValue(newGroupId);
    qUpdateGS.addBindValue(studentId);
    qUpdateGS.exec();

    // Пересчёт номеров для старой и новой группы
    if (oldGroupId != newGroupId)
        DatabaseManager::recalcJournalNumbers(oldGroupId, QSqlDatabase::database());

    DatabaseManager::recalcJournalNumbers(newGroupId, QSqlDatabase::database());

    // Обновляем интерфейс
    updateUI();

    QMessageBox::information(this, "Редактирование", QString("Данные студента \"%1\" обновлены!").arg(newFio));
}

QString MainWindow::getGroupNameById(int groupId)
{
    QSqlQuery q;
    q.prepare("SELECT name FROM groups WHERE id=?");
    q.addBindValue(groupId);
    if (q.exec() && q.next())
        return q.value(0).toString();
    return "";
}

// ==========================
// Поиск по ФИО
// ==========================
void MainWindow::onSearchFIOChanged(const QString& text)
{
    QRegularExpression regExp(text, QRegularExpression::CaseInsensitiveOption);
    proxyModel->setFilterRegularExpression(regExp);
}

// ==========================
// Импорт студентов (CSV)
// ==========================
void MainWindow::onImportStudentsClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Импорт CSV", "", "CSV (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не открыть файл: " + file.errorString());
        return;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    if (content.startsWith(QChar(0xFEFF))) content.remove(0, 1);

    QStringList allLines = content.split('\n', Qt::SkipEmptyParts);
    if (allLines.isEmpty()) {
        QMessageBox::information(this, "Импорт", "Файл пустой.");
        return;
    }

    model->database().transaction();

    int maxId = 0;
    QSqlQuery maxQuery("SELECT MAX(id) FROM students");
    if (maxQuery.next() && !maxQuery.value(0).isNull())
        maxId = maxQuery.value(0).toInt();

    int addedCount = 0;
    for (QString line : allLines) {
        line = line.trimmed();
        if (line.isEmpty()) continue;

        QStringList parts = line.contains(',') ? line.split(',') : line.split(';');
        if (parts.size() < 3) continue;

        QString full_name = parts[0].trimmed().remove('"');
        QString group_name = parts[1].trimmed().remove('"');
        int course = parts[2].trimmed().remove('"').toInt();
        if (course <= 0) course = 1;

        // Проверка на дубли
        QSqlQuery checkQuery;
        checkQuery.prepare("SELECT COUNT(*) FROM students WHERE full_name = ? AND group_name = ?");
        checkQuery.addBindValue(full_name);
        checkQuery.addBindValue(group_name);
        bool exists = false;
        if (checkQuery.exec() && checkQuery.next())
            exists = (checkQuery.value(0).toInt() > 0);
        if (exists) continue;

        maxId++;
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO students (id, full_name, group_name, course) VALUES (?, ?, ?, ?)");
        insertQuery.addBindValue(maxId);
        insertQuery.addBindValue(full_name);
        insertQuery.addBindValue(group_name);
        insertQuery.addBindValue(course);

        if (insertQuery.exec()) addedCount++;
        else maxId--;
    }

    if (addedCount > 0 && model->database().commit()) {
        updateUI();
        QMessageBox::information(this, "Импорт завершён",
            QString("Успешно добавлено студентов: %1").arg(addedCount));
    }
    else {
        model->database().rollback();
        QMessageBox::information(this, "Импорт", "Новых студентов не добавлено.");
    }

    if (addedCount > 0 && model->database().commit()) {
        // Пересчитать номера во всех группах
        DatabaseManager::recalcAllJournalNumbers(QSqlDatabase::database());

        updateUI();
        QMessageBox::information(this, "Импорт завершён",
            QString("Успешно добавлено студентов: %1").arg(addedCount));
    }
}

// ==========================
// Добавление группы
// ==========================
void MainWindow::onAddGroupClicked()
{
    AddGroupDialog dlg(this);

    if (dlg.exec() != QDialog::Accepted)
        return;

    QString groupName = dlg.groupName().trimmed();
    int course = dlg.course();

    if (groupName.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Название группы не может быть пустым!");
        return;
    }

    // Проверяем, нет ли уже такой группы
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM groups WHERE name = ?");
    checkQuery.addBindValue(groupName);

    bool exists = false;
    if (checkQuery.exec() && checkQuery.next())
        exists = (checkQuery.value(0).toInt() > 0);

    if (exists) {
        QMessageBox::information(this, "Добавление группы",
            "Такая группа уже существует!");
        return;
    }

    // Вставляем новую группу
    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT INTO groups (name, course) VALUES (?, ?)");
    insertQuery.addBindValue(groupName);
    insertQuery.addBindValue(course);

    if (!insertQuery.exec()) {
        QMessageBox::warning(this, "Ошибка",
            "Не удалось добавить группу:\n" + insertQuery.lastError().text());
        return;
    }

    QMessageBox::information(this, "Группа добавлена",
        QString("Группа \"%1\" успешно добавлена!").arg(groupName));

    updateUI();
}

// ==========================
// Обновление списка групп
// ==========================
void MainWindow::refreshGroups()
{
    // Header Combo
    if (comboHeader && comboHeader->comboGroup) {
        comboHeader->comboGroup->blockSignals(true);
        comboHeader->comboGroup->clear();
        comboHeader->comboGroup->addItem("Все группы");

        QSqlQuery query("SELECT name FROM groups ORDER BY name");
        while (query.next())
            comboHeader->comboGroup->addItem(query.value(0).toString());

        comboHeader->comboGroup->blockSignals(false);
    }

    // QListWidget
    if (ui->listGroups) {
        ui->listGroups->clear();

        QSqlQuery query("SELECT name FROM groups ORDER BY name");
        while (query.next())
            ui->listGroups->addItem(query.value(0).toString());
    }
}


void MainWindow::on_listGroups_itemDoubleClicked(QListWidgetItem* item)
{
    QString groupName = item->text();

    int groupId = getGroupIdByName(item->text());
    if (groupId < 0) return;  // группа не найдена

    GroupWindow* gw = new GroupWindow(groupId, this);
    gw->exec();
}

void MainWindow::on_btnEditGroup_clicked()
{
    QListWidgetItem* item = ui->listGroups->currentItem();
    if (!item) return;

    int groupId = getGroupIdByName(item->text());
    if (groupId < 0) return;

    GroupWindow* gw = new GroupWindow(groupId, this);
    gw->exec();
}

int MainWindow::getGroupIdByName(const QString& groupName)
{
    QSqlDatabase db = QSqlDatabase::database();  // используем текущее подключение
    QSqlQuery query(db);  // используем открытое подключение к БД
    query.prepare("SELECT id FROM groups WHERE name = ?");
    query.addBindValue(groupName);

    if (query.exec() && query.next())
        return query.value(0).toInt();  // возвращаем id группы
    else
        return -1;  // группа не найдена
}


void MainWindow::on_btnAppointDiscipline_clicked()
{
    // Создаем диалог
    AppointDisciplineDialog dialog(this);

    // Заполняем комбобоксы данными из БД
    dialog.populateCombos();

    // Показываем окно модально
    if (dialog.exec() != QDialog::Accepted)
        return;

    // Получаем данные, введённые пользователем
    int disciplineId = dialog.comboDiscipline->currentData().toInt();
    int groupId = dialog.comboGroup->currentData().toInt();
    int hoursTotal = dialog.spinClock->value();
    int lessonsCount = dialog.spinLesson->value();
    int practiceCount = dialog.spinPract->value();
    int labCount = dialog.spinLab->value();
    int selfworkCount = dialog.spinIndep->value();
    QString attestation = dialog.comboAttestation->currentText();

    // Сохраняем в БД
    QSqlQuery query;
    query.prepare("INSERT INTO group_disciplines "
        "(group_id, discipline_id, hours_total, attestation_type, practice_count, selfwork_count, lessons_count, lab_count) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(groupId);
    query.addBindValue(disciplineId);
    query.addBindValue(hoursTotal);
    query.addBindValue(attestation);
    query.addBindValue(practiceCount);
    query.addBindValue(selfworkCount);
    query.addBindValue(lessonsCount);
    query.addBindValue(labCount);

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка БД", query.lastError().text());
        return;
    }

    // Обновляем таблицу на главном окне
    refreshDisciplinesTable(-1);
}

void MainWindow::refreshDisciplinesTable(int disciplineId)
{
    auto* m = new QSqlQueryModel(this);

    QString query =
        "SELECT gd.id, d.name, "
        "g.name, "
        "gd.hours_total, "
        "gd.lessons_count, "
        "gd.practice_count, "
        "gd.selfwork_count, "
        "gd.lab_count, "
        "gd.attestation_type "
        
        "FROM group_disciplines gd "
        "JOIN disciplines d ON gd.discipline_id = d.id "
        "JOIN groups g ON gd.group_id = g.id ";

    if (disciplineId > 0)
        query += QString("WHERE gd.discipline_id = %1 ").arg(disciplineId);

    query += "ORDER BY g.name";

    m->setQuery(QSqlQuery(query));

    m->setHeaderData(0, Qt::Horizontal, "ID");
    m->setHeaderData(1, Qt::Horizontal, "Название дисциплины");
    m->setHeaderData(2, Qt::Horizontal, "Группа");
    m->setHeaderData(3, Qt::Horizontal, "Часы");
    m->setHeaderData(4, Qt::Horizontal, "Пары");
    m->setHeaderData(5, Qt::Horizontal, "Кол-во ПР");
    m->setHeaderData(6, Qt::Horizontal, "Кол-во СР");
    m->setHeaderData(7, Qt::Horizontal, "Кол-во ЛР");
    m->setHeaderData(8, Qt::Horizontal, "Вид аттестации");
    

    ui->tableDisciplines->setModel(m);
    ui->tableDisciplines->hideColumn(0);
    ui->tableDisciplines->resizeColumnsToContents();
    ui->tableDisciplines->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableDisciplines->setSelectionMode(QAbstractItemView::SingleSelection);
}



// ==========================
// Централизованное обновление интерфейса
// ==========================
void MainWindow::updateUI()
{
    refreshModel();
    refreshGroups();
    refreshDisciplinesList();
    refreshDisciplinesTable(-1);
}









void MainWindow::refreshDisciplinesList()
{
    ui->listDisciplines->clear();

    QSqlQuery query("SELECT id, name FROM disciplines ORDER BY name");

    while (query.next()) {
        QString name = query.value(1).toString();
        int id = query.value(0).toInt();

        QListWidgetItem* item = new QListWidgetItem(name);
        item->setData(Qt::UserRole, id);
        ui->listDisciplines->addItem(item);
    }
}


void MainWindow::on_btnAddDiscipline_clicked()
{
    DialogAddDiscipline dlg(this);

    if (dlg.exec() == QDialog::Accepted) {
        QString name = dlg.getName();

        if (name.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Введите название дисциплины");
            return;
        }

        QSqlQuery query;
        query.prepare("INSERT INTO discipline (name) VALUES (:name)");
        query.bindValue(":name", name);

        if (!query.exec()) {
            QMessageBox::critical(this, "Ошибка БД", query.lastError().text());
            return;
        }

        refreshDisciplinesList();
    }
}


void MainWindow::on_btnAddDisciplines_clicked()
{
    DialogAddDiscipline dlg(this);

    if (dlg.exec() == QDialog::Accepted) {
        QString name = dlg.getName();

        if (name.isEmpty()) {
            QMessageBox::warning(this, "Ошибка",
                "Название дисциплины не может быть пустым!");
            return;
        }

        QSqlQuery q;
        q.prepare("INSERT INTO disciplines (name) VALUES (:name)");
        q.bindValue(":name", name);
        q.exec();

        refreshDisciplinesList();
    }
}




void MainWindow::loadDebtsGroups()
{
    ui->comboGroup->clear();

    QSqlQuery q("SELECT id, name FROM groups ORDER BY name");

    while (q.next())
        ui->comboGroup->addItem(q.value(1).toString(),
            q.value(0).toInt());
}

void MainWindow::loadDebtsDisciplines()
{
    ui->comboDiscipline->clear();

    QSqlQuery q("SELECT DISTINCT d.id, d.name "
        "FROM disciplines d "
        "JOIN group_disciplines gd "
        "ON gd.discipline_id = d.id");

    while (q.next())
        ui->comboDiscipline->addItem(q.value(1).toString(),
            q.value(0).toInt());
}

void MainWindow::on_comboGroup_currentIndexChanged(int)
{
    currentGroupId =
        ui->comboGroup->currentData().toInt();

    loadDebtsDisciplines();
    refreshDebtsTable();
}


    void MainWindow::refreshDebtsTable()
    {
        if (currentGroupId < 0 || currentDisciplineId < 0)
            return;

        debtStudentIds.clear();

        auto* model = new QStandardItemModel(this);

        QStringList headers;
        headers << "ФИО";

        for (int i = 1;i <= practiceCount;i++)
            headers << "ПР " + QString::number(i);

        for (int i = 1;i <= selfworkCount;i++)
            headers << "СР " + QString::number(i);

        for (int i = 1;i <= labCount;i++)
            headers << "ЛР " + QString::number(i);

        headers << "Допуск" << "Экзамен";

        model->setHorizontalHeaderLabels(headers);

        // студенты
        QSqlQuery qs;
        qs.prepare(
            "SELECT s.id, s.full_name "
            "FROM students s "
            "WHERE s.group_id=? "
            "ORDER BY s.full_name");

        qs.addBindValue(currentGroupId);
        qs.exec();

        int row = 0;

        while (qs.next()) {

            int sid = qs.value(0).toInt();
            debtStudentIds << sid;

            model->insertRow(row);
            model->setItem(row, 0,
                new QStandardItem(qs.value(1).toString()));

            row++;
        }

        ui->tableDebts->setModel(model);
        ui->tableDebts->resizeColumnsToContents();

        loadExistingDebts(model);
    }


    void MainWindow::loadExistingDebts(QStandardItemModel* model)
    {
        QSqlQuery q;
        q.prepare("SELECT student_id, work_type, work_number, mark "
            "FROM debts WHERE group_discipline_id=?");

        q.addBindValue(currentGroupDisciplineId);
        q.exec();

        while (q.next()) {

            int sid = q.value(0).toInt();
            QString type = q.value(1).toString();
            int num = q.value(2).toInt();
            QString mark = q.value(3).toString();

            int row = debtStudentIds.indexOf(sid);
            if (row < 0) continue;

            int col = resolveDebtColumn(type, num);

            auto* item =
                new QStandardItem(mark);

            colorDebtCell(item, mark);

            model->setItem(row, col, item);
        }
    }

    int MainWindow::resolveDebtColumn(QString type, int num)
    {
        int base = 1;

        if (type == "practice")
            return base + num - 1;

        base += practiceCount;

        if (type == "selfwork")
            return base + num - 1;

        base += selfworkCount;

        if (type == "lab")
            return base + num - 1;

        base += labCount;

        if (type == "allow") return base;
        if (type == "exam") return base + 1;

        return -1;
    }

    void MainWindow::setDebtMark(QString mark)
    {
        auto idx = ui->tableDebts->currentIndex();
        if (!idx.isValid()) return;

        int row = idx.row();
        int col = idx.column();

        int sid = debtStudentIds[row];

        QString type;
        int num;

        decodeColumn(col, type, num);

        QSqlQuery q;

        if (mark.isEmpty()) {
            q.prepare("DELETE FROM debts "
                "WHERE student_id=? "
                "AND group_discipline_id=? "
                "AND work_type=? "
                "AND work_number IS ?");
        }
        else {
            q.prepare("INSERT OR REPLACE INTO debts "
                "(student_id,group_discipline_id,"
                "work_type,work_number,mark)"
                "VALUES(?,?,?,?,?)");
        }

        q.addBindValue(sid);
        q.addBindValue(currentGroupDisciplineId);
        q.addBindValue(type);
        q.addBindValue(num);
        q.addBindValue(mark);

        q.exec();

        refreshDebtsTable();
    }


    void MainWindow::on_comboDiscipline_currentIndexChanged(int)
    {
        currentDisciplineId =
            ui->comboDiscipline->currentData().toInt();
        // получаем group_discipline
        QSqlQuery q;
        q.prepare(
            "SELECT id, practice_count, selfwork_count, lab_count "
            "FROM group_disciplines "
            "WHERE group_id=? AND discipline_id=?");
        q.addBindValue(currentGroupId);
        q.addBindValue(currentDisciplineId);
        if (!q.exec() || !q.next())
            return;
        currentGroupDisciplineId = q.value(0).toInt();
        practiceCount = q.value(1).toInt();
        selfworkCount = q.value(2).toInt();
        labCount = q.value(3).toInt();
        refreshDebtsTable();
    }

    void MainWindow::decodeColumn(int col, QString& type, int& num)
    {
        if (col == 0) {
            type = "";
            num = 0;
            return;
        }
        int base = 1;
        if (col < base + practiceCount) {
            type = "practice";
            num = col - base + 1;
            return;
        }
        base += practiceCount;
        if (col < base + selfworkCount) {
            type = "selfwork";
            num = col - base + 1;
            return;
        }
        base += selfworkCount;
        if (col < base + labCount) {
            type = "lab";
            num = col - base + 1;
            return;
        }
        base += labCount;
        if (col == base) {
            type = "allow";
            num = 0;
            return;
        }
        if (col == base + 1) {
            type = "exam";
            num = 0;
            return;
        }
    }

    void MainWindow::colorDebtCell(QStandardItem* item,
        QString mark)
    {
        if (mark == "2" ||
            mark == "не сдал" ||
            mark == "nopass") {
            item->setBackground(Qt::red);
        }
        else if (!mark.isEmpty()) {
            item->setBackground(Qt::green);
        }
    }


void MainWindow::debugDumpDatabase()
{
    printTable("students");
    printTable("groups");
    printTable("group_students");
    printTable("disciplines");
    printTable("group_disciplines");
    printTable("debts");
    printTable("lessons");
}

void MainWindow::printTable(const QString& tableName)
{
    qDebug() << "\n=== TABLE:" << tableName << "===";

    QSqlQuery q(QSqlDatabase::database());
    q.prepare("SELECT * FROM " + tableName);

    if (!q.exec())
    {
        qDebug() << "Query failed:" << q.lastError();
        return;
    }

    while (q.next())
    {
        QString row;
        for (int i = 0; i < q.record().count(); ++i)
        {
            row += QString("%1=%2  ")
                .arg(q.record().fieldName(i))
                .arg(q.value(i).toString());
        }
        qDebug() << row;
    }
}



// ==========================
// Деструктор
// ==========================
MainWindow::~MainWindow()
{
    delete ui;
}
