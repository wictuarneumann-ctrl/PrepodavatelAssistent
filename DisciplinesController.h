#pragma once
#include <QObject>
#include <QWidget>

class DisciplinesController : public QObject
{
    Q_OBJECT
public:
    explicit DisciplinesController(QObject* parent = nullptr);

    QWidget* view() const { return m_view; }

private:
    QWidget* m_view;
};
