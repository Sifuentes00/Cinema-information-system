#ifndef SESSIONDIALOG_H
#define SESSIONDIALOG_H

#include <QDialog>
#include <QDateTimeEdit>
#include <QDialogButtonBox>
#include <QVBoxLayout>

class SessionDialog : public QDialog {
    Q_OBJECT

public:
    explicit SessionDialog(QWidget *parent = nullptr);

    QDateTime dateTime() const;

private:
    QDateTimeEdit *dateTimeEdit;
};

#endif // SESSIONDIALOG_H
