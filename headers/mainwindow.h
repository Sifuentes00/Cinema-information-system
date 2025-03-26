#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QTableWidgetItem>
#include <QStack>
#include <QRegularExpression>
#include "filmdialog.h"
#include "session.h"
#include "ticket.h"
#include "action.h"
#include "viewedticketsdialog.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

    friend class ViewedTicketsDialog;

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void openAddFilmDialog();
    void addSession(int row);
    void buyTicket(int row);
    void viewBoughtTickets();
    void deleteFilm(int row);
    void openManageSessionsMenu(int row);
    void deleteSession(int row);
    void openSearchMenu();
    void showCancelSearchButton();
    void searchFilm(const QString &parameter);
    void updateTickets();
    void viewHistory();
    void undoLastAction();
    void enableEditMode(QPushButton *editModeButton, QPushButton *exitEditModeButton);
    void disableEditMode(QPushButton *editModeButton, QPushButton *exitEditModeButton);
    void saveTableToFile();
    void readTableFromFile();
    void saveFilmsToFile();
    void saveSessionsToFile();
    void saveTicketsToFile();
    void loadTicketsFromFile();
    void saveViewedTicketsToFile();
    void loadViewedTicketsFromFile();

private:
    QVBoxLayout *layout;
    QTableWidget *filmTable;
    QVector<Ticket> boughtTickets;
    QVector<Ticket> viewedTickets;
    QStack<Action> actionStack;
    QString saveFilePath = "table.txt";
    QString filmsPath = "films.txt";
    QString sessionsPath = "sessions.txt";
    QString ticketsPath = "tickets.txt";
    QString viewedTicketsPath = "viewedTickets.txt";
};

#endif // MAINWINDOW_H
