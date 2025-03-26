#ifndef SESSION_H
#define SESSION_H

#include <QDateTime>
#include <QMetaType>
#include "film.h"

class Session {
public:
    Session(): m_film(), m_dateTime() {}
    Session(const Film &film, const QDateTime &dateTime)
        : m_film(film), m_dateTime(dateTime) {}

    Film film() const { return m_film; }
    QDateTime dateTime() const { return m_dateTime; }

    void setFilm(const Film &film) { m_film = film; }
    void setDateTime(const QDateTime &dateTime) { m_dateTime = dateTime; }

    bool operator==(const Session &other) const {
        return (m_film == other.m_film) && (m_dateTime == other.m_dateTime);
    }

private:
    Film m_film;
    QDateTime m_dateTime;
};

Q_DECLARE_METATYPE(Session)

#endif // SESSION_H
