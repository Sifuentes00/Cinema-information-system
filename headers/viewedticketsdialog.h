#ifndef VIEWEDTICKETSDIALOG_H
#define VIEWEDTICKETSDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include "mainwindow.h"
#include "ticket.h"

class ViewedTicketsDialog : public QDialog {
    Q_OBJECT

public:
    explicit ViewedTicketsDialog(const QVector<Ticket>& tickets, const QString& path, QWidget *parent = nullptr);

private slots:
    void deleteHistory();

private:
    QVector<Ticket> viewedTickets;
    QString viewedTicketsPath;
    QListWidget *listWidget;
    QPushButton *deleteHistoryButton;
};

#endif // VIEWEDTICKETSDIALOG_H
