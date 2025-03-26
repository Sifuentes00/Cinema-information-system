#ifndef PLACE_H
#define PLACE_H

class Place
{
public:
    Place() : m_placeRow(), m_placeColumn() {}

    Place(int placeRow, int placeColumn)
        : m_placeRow(placeRow), m_placeColumn(placeColumn) {}

    int placeRow() const { return m_placeRow; }
    int placeColumn() const { return m_placeColumn; }

    void setPlaceRow(int placeRow) { m_placeRow = placeRow; }
    void setPlaceColumn(int placeColumn) { m_placeColumn = placeColumn; }

    bool operator==(const Place &other) const {
        return m_placeRow == other.m_placeRow && m_placeColumn == other.m_placeColumn;
    }

    // Перегрузка оператора присваивания
    Place& operator=(const Place &other) {
        if (this != &other) { // Проверка на самоприсваивание
            m_placeRow = other.m_placeRow;
            m_placeColumn = other.m_placeColumn;
        }
        return *this; // Возвращаем ссылку на текущий объект
    }

private:
    int m_placeRow;
    int m_placeColumn;
};

#endif // PLACE_H
