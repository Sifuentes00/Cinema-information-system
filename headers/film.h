#ifndef FILM_H
#define FILM_H

#include <QString>
#include <QMetaType>

class Film {
public:
    Film(): m_title(""), m_genre(""), m_director(""), m_actor("") {}

    Film(const QString &title, const QString &genre, const QString &director, const QString &actor)
        : m_title(title), m_genre(genre), m_director(director), m_actor(actor) {}

    QString title() const { return m_title; }
    QString genre() const { return m_genre; }
    QString director() const { return m_director; }
    QString actor() const { return m_actor; }

    void setTitle(const QString &title) { m_title = title; }
    void setGenre(const QString &genre) { m_genre = genre; }
    void setDirector(const QString &director) { m_director = director; }
    void setActor(const QString &actor) { m_actor = actor; }

    bool operator==(const Film &other) const {
        return (m_title == other.m_title) &&
               (m_genre == other.m_genre) &&
               (m_director == other.m_director) &&
               (m_actor == other.m_actor);
    }

private:
    QString m_title;
    QString m_genre;
    QString m_director;
    QString m_actor;
};

Q_DECLARE_METATYPE(Film)

#endif // FILM_H
