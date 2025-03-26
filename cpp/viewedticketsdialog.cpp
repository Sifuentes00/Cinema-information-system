#include "viewedticketsdialog.h"
#include <QFile>


ViewedTicketsDialog::ViewedTicketsDialog(const QVector<Ticket> &viewedTickets, const QString &viewedTicketsPath, QWidget *parent) : viewedTickets(viewedTickets), viewedTicketsPath(viewedTicketsPath), QDialog(parent) {
    this->setWindowTitle("История просмотров");
    this->resize(600, 400);

    listWidget = new QListWidget(this);
    deleteHistoryButton = new QPushButton("Удалить историю", this);
    deleteHistoryButton->setFixedSize(120, 50);

    for (const Ticket &ticket : viewedTickets) {
        QString ticketInfo = QString("Фильм: %1\nВремя: %2\nМесто: %3")
                                 .arg(ticket.session().film().title())
                                 .arg(ticket.session().dateTime().toString("dd.MM.yyyy hh:mm"))
                                 .arg(QString("Ряд %1, Место %2\n").arg(ticket.place().placeRow() + 1).arg(ticket.place().placeColumn() + 1));
        listWidget->addItem(ticketInfo);
    }

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(listWidget);
    layout->addWidget(deleteHistoryButton, 0, Qt::AlignCenter);

    connect(deleteHistoryButton, &QPushButton::clicked, this, &ViewedTicketsDialog::deleteHistory);
}

void ViewedTicketsDialog::deleteHistory() {
    QFile file(viewedTicketsPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл для записи!";
        return;
    }

    viewedTickets.clear();

    file.resize(0);
    file.close();

    listWidget->clear();
    listWidget->addItem("Нет просмотренных билетов.");
}
