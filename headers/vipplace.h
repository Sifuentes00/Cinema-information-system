#ifndef VIPPLACE_H
#define VIPPLACE_H

#include "Place.h"
#include <string>

class VipPlace : public Place
{
public:
    VipPlace() : Place(), m_vipService("") {}

    VipPlace(int placeRow, int placeColumn, const std::string &vipService)
        : Place(placeRow, placeColumn), m_vipService(vipService) {}

    // Геттер для vipService
    std::string vipService() const { return m_vipService; }

    // Сеттер для vipService
    void setVipService(const std::string &vipService) { m_vipService = vipService; }

private:
    std::string m_vipService; // Дополнительное поле для VIP-места
};

#endif // VIPPLACE_H
