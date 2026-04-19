#pragma once
#include <QObject>
#include <QWidget>

class GroupsController : public QObject
{
    Q_OBJECT
public:
    explicit GroupsController(QObject* parent = nullptr);

    QWidget* view() const { return m_view; }

private:
    QWidget* m_view;
};
