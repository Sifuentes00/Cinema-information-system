#include "halldialog.h"

HallDialog::HallDialog(int rows, int columns, const QVector<Place>& bookedPlaces, QWidget *parent)
    : QDialog(parent), m_selectedPlace(-1, -1) {

    auto *layout = new QGridLayout(this);
    seatButtons.resize(rows);

    for (int row = 0; row < rows; ++row) {
        seatButtons[row].resize(columns);
        for (int col = 0; col < columns; ++col) {
            auto *button = new QPushButton(QString("%1-%2").arg(row + 1).arg(col + 1), this);
            button->setCheckable(true);

            if (bookedPlaces.contains(Place(row, col))) {
                button->setStyleSheet("background-color: red;");
                button->setEnabled(false);
            }else if (row >= 3 && row <= 6 && col >= 6 && col <= 9) { // Ряды 4-7, места 7-10
                button->setStyleSheet("background-color: gold;"); // Цвет для VIP-мест
                button->setProperty("placeType", QVariant::fromValue(VipPlace(row, col, "VIP Service")));
                // Если место является диваном
            } else if (row == 9) { // 15-й ряд
                button->setStyleSheet("background-color: lightblue;"); // Цвет для диванов
                button->setProperty("placeType", QVariant::fromValue(SofaPlace(row, col, "VIP Service", "Blue")));
            }

            connect(button, &QPushButton::clicked, this, &HallDialog::selectSeat);
            layout->addWidget(button, row, col);
            seatButtons[row][col] = button;
        }
    }

    auto *buyButton = new QPushButton("Купить билет", this);
    connect(buyButton, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(buyButton, rows, 0, 1, columns);

    setLayout(layout);
}

void HallDialog::selectSeat() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        for (auto &row : seatButtons) {
            for (auto &seatButton : row) {
                seatButton->setChecked(false);
            }
        }
        button->setChecked(true);
        QStringList parts = button->text().split('-');
        m_selectedPlace = Place(parts[0].toInt() - 1, parts[1].toInt() - 1);
    }
}

Place HallDialog::selectedPlace() const {
    return m_selectedPlace;
}
