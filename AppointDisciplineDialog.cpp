#include "AppointDisciplineDialog.h"
#include <QFormLayout>

AppointDisciplineDialog::AppointDisciplineDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Назначение дисциплины группе");
    setModal(true);

    //
     // Создаём виджеты
     //
    comboDiscipline = new QComboBox(this);
    comboGroup = new QComboBox(this);
    comboAttestation = new QComboBox(this);

    spinClock = new QSpinBox(this);
    spinLesson = new QSpinBox(this);
    spinPract = new QSpinBox(this);
    spinIndep = new QSpinBox(this);
    spinLab = new QSpinBox(this);

    // Ограничения 
    spinClock->setRange(0, 1000);
    spinLesson->setRange(0, 500);
    spinPract->setRange(0, 300);
    spinIndep->setRange(0, 300);
    spinLab->setRange(0, 300);

    //
    // Кнопки
    //
    btnOk = new QPushButton("Сохранить", this);
    btnCancel = new QPushButton("Отмена", this);

    connect(btnOk, &QPushButton::clicked, this, &AppointDisciplineDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &AppointDisciplineDialog::reject);

    //
    // Layout формы
    //
    QFormLayout* form = new QFormLayout();
    form->addRow("Группа:", comboGroup);
    form->addRow("Дисциплина:", comboDiscipline);
    form->addRow("Аттестация:", comboAttestation);
    form->addRow("Часы всего:", spinClock);
    form->addRow("Пар:", spinLesson);
    form->addRow("Практические:", spinPract);
    form->addRow("Лабораторные:", spinLab);
    form->addRow("Самостоятельные:", spinIndep);

    QHBoxLayout* buttons = new QHBoxLayout();
    buttons->addWidget(btnOk);
    buttons->addWidget(btnCancel);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(form);
    mainLayout->addLayout(buttons);

    populateCombos();
}

//
// Загружаем дисциплины и группы из БД
//
void AppointDisciplineDialog::populateCombos()
{
    comboDiscipline->clear();
    comboGroup->clear();

    //
    // Дисциплины
    //
    QSqlQuery qd("SELECT id, name FROM disciplines ORDER BY name");
    while (qd.next()) {
        comboDiscipline->addItem(qd.value(1).toString(), qd.value(0).toInt());
    }

    //
    // Группы
    //
    QSqlQuery qg("SELECT id, name FROM groups ORDER BY name");
    while (qg.next()) {
        comboGroup->addItem(qg.value(1).toString(), qg.value(0).toInt());
    }

    // Аттестация (просто примеры)
    comboAttestation->clear();
    comboAttestation->addItem("Дифференцированный зачёт");
    comboAttestation->addItem("Экзамен");
}



