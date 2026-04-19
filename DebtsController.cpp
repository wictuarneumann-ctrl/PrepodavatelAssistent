#include "DebtsController.h"

DebtsController::DebtsController(QObject* parent)
    : QObject(parent)
{
    m_view = new QWidget(); // пустой виджет-заглушка
}
