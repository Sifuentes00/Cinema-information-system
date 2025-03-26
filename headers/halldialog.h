#ifndef HALLDIALOG_H
#define HALLDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QVector>
#include <memory>
#include "place.h"
#include "vipplace.h"
#include "sofaplace.h"

class HallDialog : public QDialog {
    Q_OBJECT

public:
    HallDialog(int rows, int columns, const QVector<Place>& bookedPlaces, QWidget *parent = nullptr);
    Place selectedPlace() const;

private slots:
    void selectSeat();

private:
    Place m_selectedPlace; // Поле для хранения объекта любого из классов
    QVector<QVector<QPushButton*>> seatButtons;
};

#endif // HALLDIALOG_H
