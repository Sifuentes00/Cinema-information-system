#include "sessiondialog.h"

SessionDialog::SessionDialog(QWidget *parent) : QDialog(parent) {
    auto *layout = new QVBoxLayout(this);

    dateTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime(), this);
    dateTimeEdit->setCalendarPopup(true);
    layout->addWidget(dateTimeEdit);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);
}

QDateTime SessionDialog::dateTime() const {
    return dateTimeEdit->dateTime();
}
