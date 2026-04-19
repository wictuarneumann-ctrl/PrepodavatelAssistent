#include "GroupsController.h"

GroupsController::GroupsController(QObject* parent)
    : QObject(parent)
{
    m_view = new QWidget(); // пустой виджет-заглушка
}
