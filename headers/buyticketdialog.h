#ifndef BUYTICKETDIALOG_H
#define BUYTICKETDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPushButton>
#include <QLabel>

class BuyTicketDialog : public QDialog {
    Q_OBJECT

public:
    BuyTicketDialog(const QStringList &sessions, QWidget *parent = nullptr);

    QString selectedSession() const;

private:
    QButtonGroup *buttonGroup;
    QString selectedSessionText;
};

#endif // BUYTICKETDIALOG_H
