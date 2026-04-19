#ifndef APPOINTDISCIPLINEDIALOG_H
#define APPOINTDISCIPLINEDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

namespace UI {
    class AppointDisciplineDialog;
}

class AppointDisciplineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AppointDisciplineDialog(QWidget* parent = nullptr);

    // Метод для заполнения комбобоксов данными из базы
    void populateCombos();

    // Виджеты
    QComboBox* comboDiscipline;   // выбор дисциплины
    QComboBox* comboGroup;        // выбор группы
    QComboBox* comboAttestation;  // вид аттестации
    QSpinBox* spinClock;          // количество часов
    QSpinBox* spinLesson;         // количество пар
    QSpinBox* spinPract;          // количество практических работ
    QSpinBox* spinIndep;          // количество самостоятельных работ
    QSpinBox* spinLab;            // количество лабораторных работ

private:
    QPushButton* btnOk;
    QPushButton* btnCancel;
};

#endif // APPOINTDISCIPLINEDIALOG_H
