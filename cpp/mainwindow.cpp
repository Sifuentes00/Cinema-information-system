#include <QHeaderView>
#include <QMainWindow>
#include <QWidget>
#include <QRect>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QStringList>
#include <QDateTime>
#include <QMessageBox>
#include <QMenu>
#include <QInputDialog>
#include <QFile>
#include <QTextStream>
#include <QInputDialog>
#include <QRegularExpression>
#include "filmdialog.h"
#include "film.h"
#include "session.h"
#include "buyticketdialog.h"
#include "place.h"
#include "halldialog.h"
#include "ticket.h"
#include "mainwindow.h"
#include "sessiondialog.h"
#include "viewedticketsdialog.h"
#include "vipplace.h"
#include "sofaplace.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    this->setWindowTitle("Cinema");
    this->resize(1920, 1080);
    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    layout = new QVBoxLayout(centralWidget);

    auto *buttonLayout = new QHBoxLayout();

    auto *addButton = new QPushButton("Добавить фильм", this);
    addButton->setFixedSize(220, 50);
    buttonLayout->addWidget(addButton);

    auto *viewTicketsButton = new QPushButton("Посмотреть купленные билеты", this);
    viewTicketsButton->setFixedSize(220, 50);
    buttonLayout->addWidget(viewTicketsButton);

    auto *searchButton = new QPushButton("Поиск фильма", this);
    searchButton->setFixedSize(220, 50);
    buttonLayout->addWidget(searchButton);

    auto *undoButton = new QPushButton("Отменить последнее действие", this);
    undoButton->setFixedSize(220, 50);
    buttonLayout->addWidget(undoButton);

    auto *viewHistoryButton = new QPushButton("История просмотров", this);
    viewHistoryButton->setFixedSize(220, 50);
    buttonLayout->addWidget(viewHistoryButton);

    auto *editModeButton = new QPushButton("Режим редактирования", this);
    editModeButton->setFixedSize(250, 50);

    auto *exitEditModeButton = new QPushButton("Выйти из режима редактирования", this);
    exitEditModeButton->setFixedSize(250, 50);
    exitEditModeButton->setVisible(false);

    buttonLayout->setAlignment(Qt::AlignCenter);

    layout->addLayout(buttonLayout);
    layout->addWidget(editModeButton, 0, Qt::AlignCenter);
    layout->addWidget(exitEditModeButton, 0, Qt::AlignCenter);

    filmTable = new QTableWidget(this);
    filmTable->setColumnCount(8);
    filmTable->setHorizontalHeaderLabels(QStringList() << "Название" << "Жанр" << "Режиссёр" << "Актёры" << "Сеансы" << "Добавить сеанс" << "Купить билет" << "Удалить фильм");
    filmTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(filmTable);

    readTableFromFile();
    loadTicketsFromFile();
    loadViewedTicketsFromFile();
    updateTickets();

    connect(addButton, &QPushButton::clicked, this, &MainWindow::openAddFilmDialog);
    connect(viewTicketsButton, &QPushButton::clicked, this, &MainWindow::viewBoughtTickets);
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::openSearchMenu);
    connect(undoButton, &QPushButton::clicked, this, &MainWindow::undoLastAction);
    connect(viewHistoryButton, &QPushButton::clicked, this, &MainWindow::viewHistory);
    connect(editModeButton, &QPushButton::clicked, this, [this, editModeButton, exitEditModeButton]() {
        enableEditMode(editModeButton, exitEditModeButton);
    });

    connect(exitEditModeButton, &QPushButton::clicked, this, [this, editModeButton, exitEditModeButton]() {
        disableEditMode(editModeButton, exitEditModeButton);
    });
}

void MainWindow::openAddFilmDialog() {
    FilmDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Film film = dialog.film();

        if (film.title().isEmpty() || film.genre().isEmpty() || film.director().isEmpty() || film.actor().isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Все поля должны быть заполнены.");
            return;
        }

        QRegularExpression regex("[0-9]");
        if (regex.match(film.title()).hasMatch() || regex.match(film.genre()).hasMatch() ||
            regex.match(film.director()).hasMatch() || regex.match(film.actor()).hasMatch()) {
            QMessageBox::warning(this, "Ошибка", "Поля не должны содержать числа.");
            return;
        }

        QRegularExpression specialCharRegex("[^a-zA-Zа-яА-ЯёЁ0-9\\s]");
        if (specialCharRegex.match(film.title()).hasMatch() ||
            specialCharRegex.match(film.genre()).hasMatch() ||
            specialCharRegex.match(film.director()).hasMatch() ||
            specialCharRegex.match(film.actor()).hasMatch()) {
            QMessageBox::warning(this, "Ошибка", "Поля не должны содержать специальные символы.");
            return;
        }

        int row = filmTable->rowCount();
        filmTable->insertRow(row);
        filmTable->setRowHeight(row, 50);

        QTableWidgetItem *titleItem = new QTableWidgetItem(film.title());
        titleItem->setFlags(titleItem->flags() & ~Qt::ItemIsEditable);
        filmTable->setItem(row, 0, titleItem);

        QTableWidgetItem *genreItem = new QTableWidgetItem(film.genre());
        genreItem->setFlags(genreItem->flags() & ~Qt::ItemIsEditable);
        filmTable->setItem(row, 1, genreItem);

        QTableWidgetItem *directorItem = new QTableWidgetItem(film.director());
        directorItem->setFlags(directorItem->flags() & ~Qt::ItemIsEditable);
        filmTable->setItem(row, 2, directorItem);

        QTableWidgetItem *actorItem = new QTableWidgetItem(film.actor());
        actorItem->setFlags(actorItem->flags() & ~Qt::ItemIsEditable);
        filmTable->setItem(row, 3, actorItem);

        auto *manageSessionsButton = new QPushButton("Управление сеансами", this);
        filmTable->setCellWidget(row, 5, manageSessionsButton);
        connect(manageSessionsButton, &QPushButton::clicked, this, [this, row]() { openManageSessionsMenu(row); });

        auto *buyTicketButton = new QPushButton("Купить билет", this);
        filmTable->setCellWidget(row, 6, buyTicketButton);
        connect(buyTicketButton, &QPushButton::clicked, this, [this, row]() { buyTicket(row); });

        auto *deleteFilmButton = new QPushButton("Удалить фильм", this);
        filmTable->setCellWidget(row, 7, deleteFilmButton);
        connect(deleteFilmButton, &QPushButton::clicked, this, [this, row]() { deleteFilm(row); });

        Action action;
        action.type = Action::AddFilm;
        action.data = QVariant::fromValue(film);
        action.row = row;
        actionStack.push(action);

        saveTableToFile();
        saveFilmsToFile();
    }
}

void MainWindow::addSession(int row) {
    SessionDialog sessionDialog(this);
    if (sessionDialog.exec() == QDialog::Accepted) {
        QDateTime dateTime = sessionDialog.dateTime();
        Session session(Film(filmTable->item(row, 0)->text(), filmTable->item(row, 1)->text(),
                             filmTable->item(row, 2)->text(), filmTable->item(row, 3)->text()), dateTime);

        QString sessionInfo = dateTime.toString("dd.MM.yyyy hh:mm");
        QTableWidgetItem *sessionItem = filmTable->item(row, 4);
        if (!sessionItem) {
            sessionItem = new QTableWidgetItem(sessionInfo);
            sessionItem->setFlags(sessionItem->flags() & ~Qt::ItemIsEditable);
            filmTable->setItem(row, 4, sessionItem);
        } else {
            QString currentText = sessionItem->text();
            int currentHeight = filmTable->rowHeight(row);
            filmTable->setRowHeight(row, currentHeight + 22);
            currentText += "\n";
            sessionItem->setText(currentText + sessionInfo);
        }

        Action action;
        action.type = Action::AddSession;
        action.data = session.dateTime().toString("dd.MM.yyyy hh:mm");
        action.row = row;
        actionStack.push(action);

        saveTableToFile();
        saveSessionsToFile();
    }
}

void MainWindow::buyTicket(int row) {
    QTableWidgetItem *sessionItem = filmTable->item(row, 4);

    QStringList sessions = sessionItem->text().split('\n', Qt::SkipEmptyParts);
    BuyTicketDialog buyTicketDialog(sessions, this);
    QString selectedSession;

    if (buyTicketDialog.exec() == QDialog::Accepted) {
        selectedSession = buyTicketDialog.selectedSession();
        if (!selectedSession.isEmpty()) {
            QVector<Place> bookedPlaces;
            for (const Ticket &ticket : boughtTickets) {
                if (ticket.session().dateTime().toString("dd.MM.yyyy hh:mm") == selectedSession && ticket.session().film().title() == filmTable->item(row, 0)->text()) {
                    bookedPlaces.append(ticket.place());
                }
            }
            HallDialog hallDialog(10, 15, bookedPlaces, this);
            if (hallDialog.exec() == QDialog::Accepted) {
                Place place = hallDialog.selectedPlace();
                QMessageBox::information(this, "Билет куплен", QString("Вы выбрали место: ряд %1, место %2").arg(place.placeRow() + 1).arg(place.placeColumn() + 1));
                Ticket ticket(Session(Film(filmTable->item(row, 0)->text(), filmTable->item(row, 1)->text(), filmTable->item(row, 2)->text(), filmTable->item(row, 3)->text()), QDateTime::fromString(selectedSession, "dd.MM.yyyy hh:mm")), place);
                boughtTickets.append(ticket);
                Action action;
                action.type = Action::BuyTicket;
                action.data = QVariant::fromValue(ticket);
                action.row = row;
                actionStack.push(action);
            }
        }
    }

    saveTicketsToFile();
}

void MainWindow::viewBoughtTickets() {
    if (boughtTickets.isEmpty()) {
        QMessageBox::information(this, "Купленные билеты", "Нет купленных билетов");
        return;
    }

    QString ticketInfo;
    for (const Ticket &ticket : boughtTickets) {
        QString filmTitle = ticket.session().film().title();
        QString sessionTime = ticket.session().dateTime().toString("dd.MM.yyyy hh:mm");
        QString placeInfo = QString("Ряд %1, Место %2").arg(ticket.place().placeRow() + 1).arg(ticket.place().placeColumn() + 1);
        ticketInfo += QString("Фильм: %1\nСеанс: %2\nМесто: %3\n\n").arg(filmTitle, sessionTime, placeInfo);
    }

    QMessageBox::information(this, "Купленные билеты", ticketInfo);
}

void MainWindow::deleteFilm(int row) {
    QMessageBox msgBox;
    msgBox.setWindowTitle("Удаление фильма");
    msgBox.setText("Вы уверены, что хотите удалить фильм?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.button(QMessageBox::Yes)->setText("Да");
    msgBox.button(QMessageBox::No)->setText("Нет");

    QMessageBox::StandardButton reply = static_cast<QMessageBox::StandardButton>(msgBox.exec());

    if (reply == QMessageBox::Yes) {
        QString filmTitle = filmTable->item(row, 0)->text();

        Film film(filmTitle,
                  filmTable->item(row, 1)->text(),
                  filmTable->item(row, 2)->text(),
                  filmTable->item(row, 3)->text());
        QVector<Session> relatedSessions;
        QVector<Ticket> relatedTickets;

        if (filmTable->item(row, 4)) {
            QStringList sessionsList = filmTable->item(row, 4)->text().split('\n', Qt::SkipEmptyParts);
            for (const QString &session : sessionsList) {
                QDateTime sessionDateTime = QDateTime::fromString(session, "dd.MM.yyyy hh:mm");
                if (sessionDateTime.isValid()) {
                    relatedSessions.append(Session(film, sessionDateTime));
                }
            }
        }

        for (int i = 0; i < boughtTickets.size(); ++i) {
            if (boughtTickets[i].session().film().title() == filmTitle) {
                relatedTickets.append(boughtTickets[i]);
                boughtTickets.remove(i);
                --i;
            }
        }

        filmTable->removeRow(row);

        for (int i = 0; i < filmTable->rowCount(); ++i) {
            QPushButton *buyTicketButton = qobject_cast<QPushButton*>(filmTable->cellWidget(i, 6));
            if (buyTicketButton) {
                disconnect(buyTicketButton, nullptr, nullptr, nullptr);
                connect(buyTicketButton, &QPushButton::clicked, this, [this, i]() { buyTicket(i); });
            }

            QPushButton *addSessionButton = qobject_cast<QPushButton*>(filmTable->cellWidget(i, 5));
            if (addSessionButton) {
                disconnect(addSessionButton, nullptr, nullptr, nullptr);
                connect(addSessionButton, &QPushButton::clicked, this, [this, i]() { addSession(i); });
            }
        }

        Action action;
        action.type = Action::DeleteFilm;
        action.data = QVariant::fromValue(film);
        action.sessions = relatedSessions;
        action.tickets = relatedTickets;
        action.row = row;
        actionStack.push(action);

        saveTableToFile();
        saveTicketsToFile();
        saveFilmsToFile();
    }
}

void MainWindow::openManageSessionsMenu(int row) {
    QMenu sessionMenu;
    QAction *addSessionAction = sessionMenu.addAction("Добавить сеанс");
    QAction *deleteSessionAction = sessionMenu.addAction("Удалить сеанс");

    connect(addSessionAction, &QAction::triggered, this, [this, row]() { addSession(row); });
    connect(deleteSessionAction, &QAction::triggered, this, [this, row]() { deleteSession(row); });

    sessionMenu.exec(QCursor::pos());
}

void MainWindow::deleteSession(int row) {
    QTableWidgetItem *sessionItem = filmTable->item(row, 4);
    if (!sessionItem) return;

    QStringList sessions = sessionItem->text().split('\n', Qt::SkipEmptyParts);

    bool ok;
    QString selectedSession = QInputDialog::getItem(this, "Удаление сеанса",
                                                    "Выберите сеанс для удаления:", sessions, 0, false, &ok);
    if (ok && !selectedSession.isEmpty()) {
        sessions.removeAll(selectedSession);
        sessionItem->setText(sessions.join('\n'));

        for (int i = 0; i < boughtTickets.size(); ++i) {
            if ((boughtTickets[i].session().film().title() == filmTable->item(row, 0)->text()) && (selectedSession == boughtTickets[i].session().dateTime().toString("dd.MM.yyyy hh:mm"))) {
                boughtTickets.remove(i);
                --i;
            }
        }

        int currentHeight = filmTable->rowHeight(row);
        filmTable->setRowHeight(row, currentHeight - 22);

        Action action;
        action.type = Action::DeleteSession;
        action.data = selectedSession;
        action.row = row;
        actionStack.push(action);

        saveTableToFile();
        saveSessionsToFile();
    }
}

void MainWindow::saveTableToFile() {
    QFile file(saveFilePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка записи", "Не удалось открыть файл для записи.");
        return;
    }

    QTextStream out(&file);

    for (int i = 0; i < filmTable->rowCount(); ++i) {
        QString title = filmTable->item(i, 0)->text();
        QString genre = filmTable->item(i, 1)->text();
        QString director = filmTable->item(i, 2)->text();
        QString actors = filmTable->item(i, 3)->text();

        QTableWidgetItem *sessionsItem = filmTable->item(i, 4);
        QString sessions = sessionsItem ? sessionsItem->text() : "";

        out << "Фильм: " << title << "\n";
        out << "Жанр: " << genre << "\n";
        out << "Режиссёр: " << director << "\n";
        out << "Актёры: " << actors << "\n";
        out << "Сеансы:\n";

        QStringList sessionList = sessions.split('\n', Qt::SkipEmptyParts);
        for (const QString &session : sessionList) {
            out << "  - " << session << "\n";
        }

        out << "-------------------------\n";
    }

    file.close();
}

void MainWindow::readTableFromFile() {
    QFile file(saveFilePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка чтения", "Не удалось открыть файл для чтения.");
        return;
    }

    QTextStream in(&file);

    filmTable->setRowCount(0);

    QString title, genre, director, actors, line;
    QStringList sessions;

    while (!in.atEnd()) {
        line = in.readLine();

        if (line.startsWith("Фильм: ")) {
            title = line.mid(7).trimmed();
        } else if (line.startsWith("Жанр: ")) {
            genre = line.mid(6).trimmed();
        } else if (line.startsWith("Режиссёр: ")) {
            director = line.mid(10).trimmed();
        } else if (line.startsWith("Актёры: ")) {
            actors = line.mid(8).trimmed();
        } else if (line.startsWith("  - ")) {
            sessions.append(line.mid(4).trimmed());
        } else if (line.startsWith("-------------------------")) {

            int row = filmTable->rowCount();

            filmTable->insertRow(row);

            QTableWidgetItem *tittleItem = new QTableWidgetItem(title);
            tittleItem->setFlags(tittleItem->flags() & ~Qt::ItemIsEditable);
            QTableWidgetItem *genreItem = new QTableWidgetItem(genre);
            genreItem->setFlags(genreItem->flags() & ~Qt::ItemIsEditable);
            QTableWidgetItem *directorItem = new QTableWidgetItem(director);
            directorItem->setFlags(directorItem->flags() & ~Qt::ItemIsEditable);
            QTableWidgetItem *actorsItem = new QTableWidgetItem(actors);
            actorsItem->setFlags(actorsItem->flags() & ~Qt::ItemIsEditable);

            filmTable->setItem(row, 0, tittleItem);
            filmTable->setItem(row, 1, genreItem);
            filmTable->setItem(row, 2, directorItem);
            filmTable->setItem(row, 3, actorsItem);

            if (!sessions.isEmpty()) {
                QTableWidgetItem *sessionItem = new QTableWidgetItem(sessions.join("\n"));
                sessionItem->setFlags(sessionItem->flags() & ~Qt::ItemIsEditable);
                filmTable->setItem(row, 4, sessionItem);
            }

            if(sessions.isEmpty()){
                filmTable->setRowHeight(row, 50);
            }else{
            filmTable->setRowHeight(row, 50 + (sessions.size() - 1) * 22);
            }

            auto *manageSessionsButton = new QPushButton("Управление сеансами", this);
            filmTable->setCellWidget(row, 5, manageSessionsButton);
            connect(manageSessionsButton, &QPushButton::clicked, this, [this, row]() { openManageSessionsMenu(row); });

            auto *buyTicketButton = new QPushButton("Купить билет", this);
            filmTable->setCellWidget(row, 6, buyTicketButton);
            connect(buyTicketButton, &QPushButton::clicked, this, [this, row]() { buyTicket(row); });

            auto *deleteFilmButton = new QPushButton("Удалить фильм", this);
            filmTable->setCellWidget(row, 7, deleteFilmButton);
            connect(deleteFilmButton, &QPushButton::clicked, this, [this, row]() { deleteFilm(row); });

            title.clear();
            genre.clear();
            director.clear();
            actors.clear();
            sessions.clear();
        }
    }

    file.close();
}

void MainWindow::openSearchMenu() {
    QMenu searchMenu;

    QAction *searchByTitle = searchMenu.addAction("По названию");
    QAction *searchByGenre = searchMenu.addAction("По жанру");
    QAction *searchByDirector = searchMenu.addAction("По режиссёру");

    connect(searchByTitle, &QAction::triggered, this, [this]() { searchFilm("Название"); });
    connect(searchByGenre, &QAction::triggered, this, [this]() { searchFilm("Жанр"); });
    connect(searchByDirector, &QAction::triggered, this, [this]() { searchFilm("Режиссёр"); });

    searchMenu.exec(QCursor::pos());
}

void MainWindow::searchFilm(const QString &parameter) {
    bool ok;
    QString searchValue = QInputDialog::getText(this, "Поиск фильма",
                                                QString("Введите %1 для поиска:").arg(parameter),
                                                QLineEdit::Normal, "", &ok);

    if(searchValue.isEmpty()){
        QMessageBox::warning(this, "Ошибка", "Поле должно быть заполнены.");
        return;
    }

    QRegularExpression regex("[0-9]");
    if (regex.match(searchValue).hasMatch()) {
        QMessageBox::warning(this, "Ошибка", "Поле не должно содержать числа.");
        return;
    }

    QRegularExpression specialCharRegex("[^a-zA-Zа-яА-ЯёЁ0-9\\s]");
    if (specialCharRegex.match(searchValue).hasMatch()) {
        QMessageBox::warning(this, "Ошибка", "Поле не должно содержать специальные символы.");
        return;
    }

    if (ok) {
        QFile file(saveFilePath);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Ошибка чтения", "Не удалось открыть файл для чтения.");
            return;
        }

        QTextStream in(&file);

        saveFilmsToFile();

        filmTable->setRowCount(0);

        QString title, genre, director, actors, line;
        QStringList sessions;

        while (!in.atEnd()) {
            line = in.readLine();

            if (line.startsWith("Фильм: ")) {
                title = line.mid(7).trimmed();
            } else if (line.startsWith("Жанр: ")) {
                genre = line.mid(6).trimmed();
            } else if (line.startsWith("Режиссёр: ")) {
                director = line.mid(10).trimmed();
            } else if (line.startsWith("Актёры: ")) {
                actors = line.mid(8).trimmed();
            } else if (line.startsWith("  - ")) {
                sessions.append(line.mid(4).trimmed());
            } else if (line.startsWith("-------------------------")) {

                if ((parameter == "Название" && title == searchValue) ||
                    (parameter == "Жанр" && genre == searchValue) ||
                    (parameter == "Режиссёр" && director == searchValue)) {


                    int row = filmTable->rowCount();
                    filmTable->insertRow(row);
                    if(sessions.isEmpty()){
                        filmTable->setRowHeight(row, 50);
                    }else{
                        filmTable->setRowHeight(row, 50 + (sessions.size() - 1) * 22);
                    }

                    QTableWidgetItem *tittleItem = new QTableWidgetItem(title);
                    tittleItem->setFlags(tittleItem->flags() & ~Qt::ItemIsEditable);
                    QTableWidgetItem *genreItem = new QTableWidgetItem(genre);
                    genreItem->setFlags(genreItem->flags() & ~Qt::ItemIsEditable);
                    QTableWidgetItem *directorItem = new QTableWidgetItem(director);
                    directorItem->setFlags(directorItem->flags() & ~Qt::ItemIsEditable);
                    QTableWidgetItem *actorsItem = new QTableWidgetItem(actors);
                    actorsItem->setFlags(actorsItem->flags() & ~Qt::ItemIsEditable);

                    filmTable->setItem(row, 0, tittleItem);
                    filmTable->setItem(row, 1, genreItem);
                    filmTable->setItem(row, 2, directorItem);
                    filmTable->setItem(row, 3, actorsItem);

                    if (!sessions.isEmpty()) {
                        QTableWidgetItem *sessionItem = new QTableWidgetItem(sessions.join("\n"));
                        sessionItem->setFlags(sessionItem->flags() & ~Qt::ItemIsEditable);
                        filmTable->setItem(row, 4, sessionItem);
                    }
                    auto *buyTicketButton = new QPushButton("Купить билет", this);
                    filmTable->setCellWidget(row, 6, buyTicketButton);
                    connect(buyTicketButton, &QPushButton::clicked, this, [this, row]() { buyTicket(row); });
                }

                title.clear();
                genre.clear();
                director.clear();
                actors.clear();
                sessions.clear();
            }
        }

        file.close();

        showCancelSearchButton();
    }
}

void MainWindow::showCancelSearchButton() {
    QPushButton *cancelSearchButton = new QPushButton("Отменить поиск", this);
    layout->addWidget(cancelSearchButton,  0, Qt::AlignCenter);
    cancelSearchButton->setFixedSize(220, 50);

    connect(cancelSearchButton, &QPushButton::clicked, this, [this, cancelSearchButton]() {
        readTableFromFile();
        cancelSearchButton->deleteLater();
    });
}

void MainWindow::saveFilmsToFile() {
    QFile file(filmsPath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка записи", "Не удалось открыть файл для записи фильмов.");
        return;
    }

    QTextStream out(&file);

    for (int i = 0; i < filmTable->rowCount(); ++i) {
        QString title = filmTable->item(i, 0)->text();
        QString genre = filmTable->item(i, 1)->text();
        QString director = filmTable->item(i, 2)->text();
        QString actors = filmTable->item(i, 3)->text();

        out << "Фильм: " << title << "\n";
        out << "Жанр: " << genre << "\n";
        out << "Режиссёр: " << director << "\n";
        out << "Актёры: " << actors << "\n";
        out << "-------------------------\n";
    }

    file.close();
}

void MainWindow::saveSessionsToFile() {
    QFile file(sessionsPath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка записи", "Не удалось открыть файл для записи сеансов.");
        return;
    }

    QTextStream out(&file);

    for (int i = 0; i < filmTable->rowCount(); ++i) {
        QString title = filmTable->item(i, 0)->text();
        QTableWidgetItem *sessionsItem = filmTable->item(i, 4);
        QString sessions = sessionsItem ? sessionsItem->text() : "";

        out << "Фильм: " << title << "\n";
        out << "Сеансы:\n";

        QStringList sessionList = sessions.split('\n', Qt::SkipEmptyParts);
        for (const QString &session : sessionList) {
            out << "  - " << session << "\n";
        }

        out << "-------------------------\n";
    }

    file.close();
}

void MainWindow::saveTicketsToFile() {
    QFile file(ticketsPath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка записи", "Не удалось открыть файл для записи билетов.");
        return;
    }

    QTextStream out(&file);

    for (const Ticket &ticket : boughtTickets) {
        QString filmTitle = ticket.session().film().title();
        QString sessionTime = ticket.session().dateTime().toString("dd.MM.yyyy hh:mm");
        int row = ticket.place().placeRow() + 1;
        int column = ticket.place().placeColumn() + 1;

        out << "Фильм: " << filmTitle << "\n";
        out << "Сеанс: " << sessionTime << "\n";
        out << "Место: Ряд " << row << ", Место " << column << "\n";
        out << "-------------------------\n";
    }

    file.close();
}

void MainWindow::loadTicketsFromFile() {
    QFile file(ticketsPath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка чтения", "Не удалось открыть файл для загрузки билетов.");
        return;
    }

    QTextStream in(&file);

    QString filmTitle, sessionTime, line;
    int placeRow = -1, placeColumn = -1;

    while (!in.atEnd()) {
        line = in.readLine();

        if (line.startsWith("Фильм: ")) {
            filmTitle = line.mid(7).trimmed();
        } else if (line.startsWith("Сеанс: ")) {
            sessionTime = line.mid(7).trimmed();
        } else if (line.startsWith("Место: Ряд ")) {
            QStringList placeParts = line.mid(11).split(", Место ");
            if (placeParts.size() == 2) {
                placeRow = placeParts[0].toInt() - 1;
                placeColumn = placeParts[1].toInt() - 1;
            }
        } else if (line.startsWith("-------------------------")) {

            if (!filmTitle.isEmpty() && !sessionTime.isEmpty() && placeRow >= 0 && placeColumn >= 0) {
                Film film(filmTitle, "", "", "");
                QDateTime dateTime = QDateTime::fromString(sessionTime, "dd.MM.yyyy hh:mm");
                Session session(film, dateTime);
                Place place(placeRow, placeColumn);

                boughtTickets.append(Ticket(session, place));
            }

            filmTitle.clear();
            sessionTime.clear();
            placeRow = -1;
            placeColumn = -1;
        }
    }

    file.close();
}

void MainWindow::saveViewedTicketsToFile() {
    QFile file(viewedTicketsPath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка записи", "Не удалось открыть файл для записи билетов.");
        return;
    }

    QTextStream out(&file);

    for (const Ticket &ticket : viewedTickets) {
        QString filmTitle = ticket.session().film().title();
        QString sessionTime = ticket.session().dateTime().toString("dd.MM.yyyy hh:mm");
        int row = ticket.place().placeRow() + 1;
        int column = ticket.place().placeColumn() + 1;

        out << "Фильм: " << filmTitle << "\n";
        out << "Сеанс: " << sessionTime << "\n";
        out << "Место: Ряд " << row << ", Место " << column << "\n";
        out << "-------------------------\n";
    }

    file.close();
}

void MainWindow::loadViewedTicketsFromFile() {
    QFile file(viewedTicketsPath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка чтения", "Не удалось открыть файл для загрузки билетов.");
        return;
    }

    QTextStream in(&file);

    QString filmTitle, sessionTime, line;
    int placeRow = -1, placeColumn = -1;

    while (!in.atEnd()) {
        line = in.readLine();

        if (line.startsWith("Фильм: ")) {
            filmTitle = line.mid(7).trimmed();
        } else if (line.startsWith("Сеанс: ")) {
            sessionTime = line.mid(7).trimmed();
        } else if (line.startsWith("Место: Ряд ")) {
            QStringList placeParts = line.mid(11).split(", Место ");
            if (placeParts.size() == 2) {
                placeRow = placeParts[0].toInt() - 1;
                placeColumn = placeParts[1].toInt() - 1;
            }
        } else if (line.startsWith("-------------------------")) {
            if (!filmTitle.isEmpty() && !sessionTime.isEmpty() && placeRow >= 0 && placeColumn >= 0) {
                Film film(filmTitle, "", "", "");
                QDateTime dateTime = QDateTime::fromString(sessionTime, "dd.MM.yyyy hh:mm");
                Session session(film, dateTime);
                Place place(placeRow, placeColumn);

                viewedTickets.append(Ticket(session, place));
            }

            filmTitle.clear();
            sessionTime.clear();
            placeRow = -1;
            placeColumn = -1;
        }
    }

    file.close();
}

void MainWindow::undoLastAction() {
    if (actionStack.isEmpty()) {
        QMessageBox::information(this, "Отмена действия", "Нет действий для отмены.");
        return;
    }

    Action lastAction = actionStack.pop();

    switch (lastAction.type) {
    case Action::AddFilm: {
        deleteFilm(lastAction.row);
        break;
    }
    case Action::DeleteFilm: {
        Film film = lastAction.data.value<Film>();
        int row = lastAction.row;

        filmTable->insertRow(row);
        filmTable->setRowHeight(row, 50);
        filmTable->setItem(row, 0, new QTableWidgetItem(film.title()));
        filmTable->setItem(row, 1, new QTableWidgetItem(film.genre()));
        filmTable->setItem(row, 2, new QTableWidgetItem(film.director()));
        filmTable->setItem(row, 3, new QTableWidgetItem(film.actor()));

        QStringList sessionsList;
        for (const Session &session : lastAction.sessions) {
            sessionsList.append(session.dateTime().toString("dd.MM.yyyy hh:mm"));
        }
        filmTable->setItem(row, 4, new QTableWidgetItem(sessionsList.join('\n')));

        for (const Ticket &ticket : lastAction.tickets) {
            boughtTickets.append(ticket);
        }

        auto *manageSessionsButton = new QPushButton("Управление сеансами", this);
        filmTable->setCellWidget(row, 5, manageSessionsButton);
        connect(manageSessionsButton, &QPushButton::clicked, this, [this, row]() { openManageSessionsMenu(row); });

        auto *buyTicketButton = new QPushButton("Купить билет", this);
        filmTable->setCellWidget(row, 6, buyTicketButton);
        connect(buyTicketButton, &QPushButton::clicked, this, [this, row]() { buyTicket(row); });

        auto *deleteFilmButton = new QPushButton("Удалить фильм", this);
        filmTable->setCellWidget(row, 7, deleteFilmButton);
        connect(deleteFilmButton, &QPushButton::clicked, this, [this, row]() { deleteFilm(row); });

        break;
    }
    case Action::AddSession: {
        int row = lastAction.row;
        QString sessionInfo = lastAction.data.toString();
        QTableWidgetItem *sessionItem = filmTable->item(row, 4);
        if (sessionItem) {
            QStringList sessions = sessionItem->text().split('\n', Qt::SkipEmptyParts);
            sessions.removeAll(sessionInfo);
            sessionItem->setText(sessions.join('\n'));
        }
        break;
    }
    case Action::DeleteSession: {
        int row = lastAction.row;
        QString sessionInfo = lastAction.data.toString();
        QTableWidgetItem *sessionItem = filmTable->item(row, 4);
        if (sessionItem) {
            QString currentText = sessionItem->text();
            currentText += "\n" + sessionInfo;
            sessionItem->setText(currentText);
        }
        break;
    }
    case Action::BuyTicket: {
        Ticket ticket = lastAction.data.value<Ticket>();
        boughtTickets.removeAll(ticket);
        break;
    }
}

    saveFilmsToFile();
    saveSessionsToFile();
    saveTicketsToFile();
    saveTableToFile();
}

void MainWindow::updateTickets() {
    for (int i = 0; i < boughtTickets.size(); ++i) {
        if(boughtTickets[i].session().dateTime() < QDateTime::currentDateTime()){
            viewedTickets.append(boughtTickets[i]);
            boughtTickets.remove(i);
            --i;
        }
    }

    for (int j = 0; j < filmTable->rowCount(); ++j) {

        QString sessionsString = filmTable->item(j, 4)->text();

        if (sessionsString.isEmpty()) {
            continue;
        }

        QStringList sessionsList = sessionsString.split('\n', Qt::SkipEmptyParts);

        QStringList updatedSessionsList;

        for (const QString &session : sessionsList) {
            QDateTime sessionDateTime = QDateTime::fromString(session, "dd.MM.yyyy hh:mm");

            if (sessionDateTime >= QDateTime::currentDateTime()) {
                updatedSessionsList.append(session);
            }
        }

        filmTable->item(j, 4)->setText(updatedSessionsList.join('\n'));
    }

    saveTicketsToFile();
    saveViewedTicketsToFile();
    saveTableToFile();
    saveSessionsToFile();
}


void MainWindow::viewHistory() {
    ViewedTicketsDialog *dialog = new ViewedTicketsDialog(viewedTickets, viewedTicketsPath, this);
    dialog->exec();

    QFile file(viewedTicketsPath);
    if (file.exists() && file.size() == 0) {
        viewedTickets.clear();
    }

    saveViewedTicketsToFile();
}

void MainWindow::enableEditMode(QPushButton *editModeButton, QPushButton *exitEditModeButton) {
    for (int row = 0; row < filmTable->rowCount(); ++row) {
        for (int column = 0; column < filmTable->columnCount(); ++column) {
            QTableWidgetItem *item = filmTable->item(row, column);
            if (item) {
                item->setFlags(item->flags() | Qt::ItemIsEditable);
            }
        }
    }

    exitEditModeButton->setVisible(true);

    editModeButton->setVisible(false);
}

void MainWindow::disableEditMode(QPushButton *editModeButton, QPushButton *exitEditModeButton) {
    for (int row = 0; row < filmTable->rowCount(); ++row) {
        for (int column = 0; column < filmTable->columnCount(); ++column) {
            QTableWidgetItem *item = filmTable->item(row, column);
            if (item) {
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            }
        }
    }

    exitEditModeButton->setVisible(false);

    editModeButton->setVisible(true);

    saveTableToFile();
    saveFilmsToFile();
    saveSessionsToFile();
}
