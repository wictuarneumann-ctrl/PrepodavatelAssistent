#pragma once

#include <QMainWindow>
#include <QSqlTableModel>
#include <QTableView>
#include <QSortFilterProxyModel>
#include <QComboBox>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QtSql/QSqlQuery>
#include <QSqlRelationalTableModel>
#include <QListWidgetItem>
#include <QStandardItemModel>
#include <QStandardItem>

namespace Ui {
    class MainWindowClass;
}

// =============================
// StudentsFilterProxyModel (встроенный)
// =============================
class StudentsFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit StudentsFilterProxyModel(QObject* parent = nullptr)
        : QSortFilterProxyModel(parent) {
    }

    QString groupFilter;
    QString courseFilter;

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override
    {
        QModelIndex indexName = sourceModel()->index(source_row, 1, source_parent);
        QModelIndex indexGroup = sourceModel()->index(source_row, 2, source_parent);
        QModelIndex indexCourse = sourceModel()->index(source_row, 3, source_parent);

        bool match = true;
        QRegularExpression regExp = filterRegularExpression();
        if (!regExp.pattern().isEmpty())
            match &= sourceModel()->data(indexName).toString().contains(regExp);

        if (!groupFilter.isEmpty() && groupFilter != "Все группы")
            match &= sourceModel()->data(indexGroup).toString() == groupFilter;

        if (!courseFilter.isEmpty() && courseFilter != "Все курсы")
            match &= sourceModel()->data(indexCourse).toString() == courseFilter;

        return match;
    }

public slots:
    void refreshFilter() { invalidateFilter(); }
};

// =============================
// HeaderWithCombo (встроенный)
// =============================
class HeaderWithCombo : public QHeaderView
{
    Q_OBJECT
public:
    explicit HeaderWithCombo(Qt::Orientation orientation, QWidget* parent = nullptr)
        : QHeaderView(orientation, parent)
    {
        comboGroup = new QComboBox(this);
        comboCourse = new QComboBox(this);

        comboGroup->setEditable(false);
        comboCourse->setEditable(false);

        connect(comboGroup, &QComboBox::currentTextChanged, this, &HeaderWithCombo::groupChanged);
        connect(comboCourse, &QComboBox::currentTextChanged, this, &HeaderWithCombo::courseChanged);

        connect(this, &QHeaderView::sectionResized, this, &HeaderWithCombo::updateComboPositions);
        connect(this, &QHeaderView::sectionMoved, this, &HeaderWithCombo::updateComboPositions);
    }

    QComboBox* comboGroup;
    QComboBox* comboCourse;

signals:
    void groupChanged(const QString& text);
    void courseChanged(const QString& text);

protected:
    void resizeEvent(QResizeEvent* event) override
    {
        QHeaderView::resizeEvent(event);
        updateComboPositions();
    }

private:



    void updateComboPositions()
    {
        if (!comboGroup || !comboCourse) return;
        comboGroup->setGeometry(sectionViewportPosition(2), 0, sectionSize(2), height());
        comboCourse->setGeometry(sectionViewportPosition(3), 0, sectionSize(3), height());
    }
};

// =============================
// MainWindow
// =============================
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    void refreshGroups();
    ~MainWindow();

private slots:
    void onAddStudentClicked();
    void onDeleteStudentClicked();
    void onSearchFIOChanged(const QString& text);
    void onImportStudentsClicked();
    void onEditStudentClicked();
    void updateHeaderCombos();

    void onAddGroupClicked();
    
    void refreshModel();
    void updateUI();
    void on_listGroups_itemDoubleClicked(QListWidgetItem* item);
    void on_btnEditGroup_clicked();
    int getGroupIdByName(const QString& groupName);

    void debugDumpDatabase();
    void printTable(const QString& tableName);
    QString getGroupNameById(int groupId);


    void refreshDisciplinesList();
    void refreshDisciplinesTable(int disciplineId =-1);
    void on_btnAddDiscipline_clicked();
    void on_btnAddDisciplines_clicked();
    void on_btnAppointDiscipline_clicked();







    void on_comboGroup_currentIndexChanged(int);
    void on_comboDiscipline_currentIndexChanged(int);

    void refreshDebtsTable();
    void loadExistingDebts(QStandardItemModel* model);
    int resolveDebtColumn(QString type, int num);
    void decodeColumn(int col, QString& type, int& num);
    void setDebtMark(QString mark);
    void colorDebtCell(QStandardItem* item, QString mark);
    void loadDebtsGroups();
    void loadDebtsDisciplines();



private:
    Ui::MainWindowClass* ui;

    QSqlRelationalTableModel* model;
    StudentsFilterProxyModel* proxyModel;
    HeaderWithCombo* comboHeader;


    // debts screen
    int currentGroupId = -1;
    int currentDisciplineId = -1;
    int currentGroupDisciplineId = -1;

    int practiceCount = 0;
    int selfworkCount = 0;
    int labCount = 0;

    QVector<int> debtStudentIds;


};
