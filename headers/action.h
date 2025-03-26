#ifndef ACTION_H
#define ACTION_H

#include <QVariant>
#include <QVector>
#include "Film.h"
#include "Session.h"
#include "Ticket.h"

class Action {
public:
    enum ActionType { AddFilm, DeleteFilm, AddSession, DeleteSession, BuyTicket } type;
    QVariant data;
    QVector<Session> sessions;
    QVector<Ticket> tickets;
    int row;
};

#endif // ACTION_H
