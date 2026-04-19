#pragma once
#include <QObject>
#include <QWidget>

class ReportsController : public QObject
{
    Q_OBJECT
public:
    explicit ReportsController(QObject* parent = nullptr);

    QWidget* view() const { return m_view; }

private:
    QWidget* m_view;
};
