#ifndef FILMDIALOG_H
#define FILMDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include "film.h"

class FilmDialog : public QDialog {
    Q_OBJECT

public:
    explicit FilmDialog(QWidget *parent = nullptr);

    Film film() const;

private:
    QLineEdit *titleInput;
    QLineEdit *genreInput;
    QLineEdit *directorInput;
    QLineEdit *actorInput;
};

#endif // FILMDIALOG_H
