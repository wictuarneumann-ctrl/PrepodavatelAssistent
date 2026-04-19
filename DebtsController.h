#pragma once
#include <QObject>
#include <QWidget>

class DebtsController : public QObject
{
    Q_OBJECT
public:
    explicit DebtsController(QObject* parent = nullptr);

    QWidget* view() const { return m_view; }

private:
    QWidget* m_view;
};
