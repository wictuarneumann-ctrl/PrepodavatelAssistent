#pragma once
#include <QObject>
#include <QWidget>

class JournalController : public QObject
{
    Q_OBJECT
public:
    explicit JournalController(QObject* parent = nullptr);

    QWidget* view() const { return m_view; }

private:
    QWidget* m_view;
};
