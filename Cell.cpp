#include "Cell.h"
#include "Player.h"

Cell::Cell(QWidget *parent)
    : QPushButton(parent),
        m_player(nullptr),
        m_inverted(false) {
    QObject::connect(this, SIGNAL(playerChanged(Player*)), this, SLOT(updateCell()));
    QObject::connect(this, SIGNAL(invertedChanged(bool)), this, SLOT(updateCell()));

    this->updateCell();
}

Cell::~Cell() {
}

void Cell::setPlayer(Player* player) {
    if (m_player != player) {
        m_player = player;
        emit playerChanged(player);
    }
}
void Cell::setLap(int lap){
    m_lapCounter = lap;
    emit lapCounterChanged(m_lapCounter);
}
void Cell::setInverted(bool inverted) {
    if (m_inverted != inverted) {
        m_inverted = inverted;
        emit invertedChanged(inverted);
    }
}
void Cell::setParalised(bool paralised) {
    if (m_paralised != paralised) {
        m_paralised = paralised;
        emit paralisedChanged(paralised);
    }
}
void Cell::setForce(int row, int col){
    if(m_player->type() == Player::Red){
        if(row == 1 || row ==5){
            m_force =1;
            if(m_inverted){
                m_force =3;
            }
        }
        else if(row ==2 || row ==4){
            m_force =3;
            if(m_inverted){
                m_force =1;
            }
        }
        else if(row ==3){
           m_force =2;
        }

    }
    else{
            if(col == 1 || col ==5){
                m_force = 1;
                if(m_inverted){
                    m_force =3;
                }
            }
            else if(col ==2 || col ==4){
                m_force =3;
                if(m_inverted){
                    m_force =1;
                }
            }
            else if(col ==3){
                m_force = 2;

            }
    }


}
void Cell::reset() {
    m_player = nullptr;
    m_paralised = false;
    this->updateCell();
}

void Cell::updateCell() {
    QPixmap img;

    if (m_player != nullptr) {
        img = m_player->pixmap();
        if (m_inverted)
            img = img.transformed(QTransform().rotate(180));
    }

    this->setIcon(img);
}
void Cell::incrementLap() {
    m_lapCounter++;
    emit lapCounterChanged(m_lapCounter);
}
