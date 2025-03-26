#ifndef TICKET_H
#define TICKET_H

#include "session.h"
#include "place.h"
#include <QMetaType>

class Ticket
{
public:
    Ticket(): m_session(), m_place() {}

    Ticket(const Session &session, const Place &place)
        : m_session(session), m_place(place) {}

    Ticket(const Ticket &other)
        : m_session(other.m_session), m_place(other.m_place) {}

    Session session() const { return m_session; }
    Place place() const { return m_place; }

    void setSession(const Session &session) { m_session = session; }
    void setPlace(const Place &place) { m_place = place; }

    Ticket &operator=(Ticket &&other) noexcept {
        if (this != &other) {
            m_session = std::move(other.m_session);
            m_place = std::move(other.m_place);
        }
        return *this;
    }

    Ticket &operator=(const Ticket &other) {
        if (this != &other) {
            m_session = other.m_session;
            m_place = other.m_place;
        }
        return *this;
    }

    bool operator==(const Ticket &other) const {
        return (m_session == other.m_session) && (m_place == other.m_place);
    }

private:
    Session m_session;
    Place m_place;
};

Q_DECLARE_METATYPE(Ticket)

#endif // TICKET_H
