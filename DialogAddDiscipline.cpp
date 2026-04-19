#include "DialogAddDiscipline.h"
#include "ui_DialogAddDiscipline.h"


DialogAddDiscipline::DialogAddDiscipline(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::DialogAddDiscipline)
{
    ui->setupUi(this);
}

DialogAddDiscipline::~DialogAddDiscipline()
{
    delete ui;
}


QString DialogAddDiscipline::getName() const {
    return ui->lineDisciplineName->text().trimmed();
}
void DialogAddDiscipline::on_btnOK_clicked()
{
    accept();
}

void DialogAddDiscipline::on_btnCancel_clicked()
{
    reject();
}