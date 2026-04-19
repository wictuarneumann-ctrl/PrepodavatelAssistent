#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QtSql/QSqlQuery>

class AddStudentDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddStudentDialog(QWidget* parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle("Добавить студента");

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        QFormLayout* formLayout = new QFormLayout();

        // ФИО — обычное поле
        lineFIO = new QLineEdit(this);
        formLayout->addRow("ФИО:", lineFIO);

        // Группа — комбобокс с возможностью ввода нового значения
        comboGroup = new QComboBox(this);
        comboGroup->setEditable(true);
        comboGroup->addItem(""); // пустое значение по умолчанию
        QSqlQuery qGroups("SELECT name FROM groups ORDER BY name");
        while (qGroups.next())
            comboGroup->addItem(qGroups.value(0).toString());
        formLayout->addRow("Группа:", comboGroup);

        // Курс — комбобокс с возможностью ввода нового значения
        comboCourse = new QComboBox(this);
        comboCourse->setEditable(true);
        comboCourse->addItem(""); // пустое значение по умолчанию
        QSqlQuery qCourses("SELECT DISTINCT course FROM students");
        while (qCourses.next())
            comboCourse->addItem(qCourses.value(0).toString());
        formLayout->addRow("Курс:", comboCourse);

        mainLayout->addLayout(formLayout);

        // Кнопки
        QPushButton* btnOk = new QPushButton("Добавить", this);
        QPushButton* btnCancel = new QPushButton("Отмена", this);

        QHBoxLayout* btnLayout = new QHBoxLayout();
        btnLayout->addStretch();
        btnLayout->addWidget(btnOk);
        btnLayout->addWidget(btnCancel);
        mainLayout->addLayout(btnLayout);

        connect(btnOk, &QPushButton::clicked, this, &AddStudentDialog::accept);
        connect(btnCancel, &QPushButton::clicked, this, &AddStudentDialog::reject);
    }
    void setData(const QString& fio, const QString& group, int course);
    // Получение значений
    QString fio() const { return lineFIO->text().trimmed(); }
    QString group() const { return comboGroup->currentText().trimmed(); }

    // Курс теперь безопасно возвращает int, значение по умолчанию = 1
    int course() const
    {
        bool ok;
        int c = comboCourse->currentText().trimmed().toInt(&ok);
        return (ok && c > 0) ? c : 1;
    }


    

private:
    QLineEdit* lineFIO;
    QComboBox* comboGroup;
    QComboBox* comboCourse;
};
