#include "DisciplinesController.h"

DisciplinesController::DisciplinesController(QObject* parent)
    : QObject(parent)
{
    m_view = new QWidget(); // пустой виджет-заглушка
}
