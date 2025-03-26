#include "filmdialog.h"

FilmDialog::FilmDialog(QWidget *parent) : QDialog(parent) {
    auto *layout = new QVBoxLayout(this);

    titleInput = new QLineEdit(this);
    titleInput->setPlaceholderText("Название фильма");
    layout->addWidget(titleInput);

    genreInput = new QLineEdit(this);
    genreInput->setPlaceholderText("Жанр");
    layout->addWidget(genreInput);

    directorInput = new QLineEdit(this);
    directorInput->setPlaceholderText("Режиссёр");
    layout->addWidget(directorInput);

    actorInput = new QLineEdit(this);
    actorInput->setPlaceholderText("Актёры");
    layout->addWidget(actorInput);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);
}

Film FilmDialog::film() const {
    return Film(titleInput->text(), genreInput->text(), directorInput->text(), actorInput->text());
}
