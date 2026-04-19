#include "JournalController.h"

JournalController::JournalController(QObject* parent)
    : QObject(parent)
{
    m_view = new QWidget(); // пустой виджет-заглушка
}
