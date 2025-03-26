#include "buyticketdialog.h"

BuyTicketDialog::BuyTicketDialog(const QStringList &sessions, QWidget *parent)
    : QDialog(parent), buttonGroup(new QButtonGroup(this)) {

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Выберите сеанс:", this));

    for (const QString &session : sessions) {
        auto *radioButton = new QRadioButton(session, this);
        buttonGroup->addButton(radioButton);
        layout->addWidget(radioButton);
    }

    auto *okButton = new QPushButton("ОК", this);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(okButton);

    setLayout(layout);
}

QString BuyTicketDialog::selectedSession() const {
    QAbstractButton *checkedButton = buttonGroup->checkedButton();
    return checkedButton ? checkedButton->text() : QString();
}
