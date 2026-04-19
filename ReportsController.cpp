#include "ReportsController.h"

ReportsController::ReportsController(QObject* parent)
    : QObject(parent)
{
    m_view = new QWidget(); // пустой виджет-заглушка
}
