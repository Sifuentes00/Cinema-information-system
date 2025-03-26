#ifndef SOFAPLACE_H
#define SOFAPLACE_H

#include "VipPlace.h"

// Наследуем от класса VipPlace
class SofaPlace : public VipPlace
{
public:
    // Конструктор по умолчанию
    SofaPlace() : VipPlace(), m_sofaColor("") {}

    // Конструктор с параметрами
    SofaPlace(int placeRow, int placeColumn, const std::string &vipService, const std::string &sofaColor)
        : VipPlace(placeRow, placeColumn, vipService), m_sofaColor(sofaColor) {}

    // Геттер для sofaColor
    std::string sofaColor() const { return m_sofaColor; }

    // Сеттер для sofaColor
    void setSofaColor(const std::string &sofaColor) { m_sofaColor = sofaColor; }

private:
    std::string m_sofaColor; // Дополнительное поле для дивана
};

#endif // SOFAPLACE_H
