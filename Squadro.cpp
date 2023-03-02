#include "Squadro.h"
#include "ui_Squadro.h"
#include "Cell.h"
#include "Player.h"

#include <QDebug>
#include <QMessageBox>
#include <QSignalMapper>
/*
 Nomes
 * Pedro Henrique Rocha de Castro(20213004828)
 * Diogo Emanuel Antunes Santos(20213002091)

*/
Squadro::Squadro(QWidget *parent)
    : QMainWindow(parent),
        ui(new Ui::Squadro),
        m_player(nullptr) {

    ui->setupUi(this);

    QObject::connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(reset()));
    QObject::connect(ui->actionQuit, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    QObject::connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(showAbout()));


    QSignalMapper* map = new QSignalMapper(this);
    for (int row = 0; row < 7; ++row) {
        for (int col = 0; col < 7; ++col) {
            QString cellName = QString("cell%1%2").arg(row).arg(col);
            Cell* cell = this->findChild<Cell*>(cellName);
            m_board[row][col] = cell;

            if (cell != nullptr) {
                int id = row * 7 + col;
                map->setMapping(cell, id);
                QObject::connect(cell, SIGNAL(clicked(bool)), map, SLOT(map()));
            }
        }
    }
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    QObject::connect(map, SIGNAL(mapped(int)), this, SLOT(play(int)));
#else
    QObject::connect(map, SIGNAL(mappedInt(int)), this, SLOT(play(int)));
#endif
    // When the turn ends, switch the player.
    QObject::connect(this, SIGNAL(turnEnded()), this, SLOT(switchPlayer()));

    // Connect the red player counts.
    Player* red = Player::player(Player::Red);
    QObject::connect(red, SIGNAL(countChanged(int)), this, SLOT(updateStatusBar()));

    // Connect the blue player counts.
    Player* blue = Player::player(Player::Blue);
    QObject::connect(blue, SIGNAL(countChanged(int)), this, SLOT(updateStatusBar()));

    // Reset.
    this->reset();

    // Adjust window.
    this->adjustSize();
    this->setFixedSize(this->size());
}

Squadro::~Squadro() {
    delete ui;
}

void Squadro::play(int id) {
    int row = id / 7;
    int col = id % 7;
    Cell* cell = m_board[row][col];
    Q_ASSERT(cell != nullptr);
    m_board[row][col]->setForce(row,col);
    if (cell->player() == m_player &&  !m_board[row][col]->isParalised()) {
        move(row,col);
        verifyVictory();
        emit turnEnded();
    }
}

void Squadro::switchPlayer() {
    // Switch the player.
    m_player = m_player->other();

    // Finally, update the status bar.
    this->updateStatusBar();
}

void Squadro::reset() {
    // Reset the red player.
    Player* red = Player::player(Player::Red);
    red->reset();

    // Reset the blue player.
    Player* blue = Player::player(Player::Blue);
    blue->reset();

    // Reset board.
    for (int row = 0; row < 7; ++row) {
        for (int col = 0; col < 7; ++col) {
            Cell* cell = m_board[row][col];
            if (cell != nullptr){
                cell->setInverted(false);
                cell->reset();
            }

        }
    }

    for (int i = 1; i < 6; i++) {
        m_board[i][0]->setPlayer(red);
        m_board[i][0]->setLap(1);
        m_board[6][i]->setPlayer(blue);
        m_board[6][i]->setLap(1);
    }

    // Set the starting player.
    m_player = red;

    // Finally, update the status bar.
    this->updateStatusBar();
}

void Squadro::move(int row, int col) {
    m_board[row][col]->reset();

    if(verifyWay(row,col)){ //true tem um objeto no caminho que pode ser comido
        return;
    }
    else{
       jump(row,col);
    }

}
void Squadro::jump(int row, int col){
    Cell* cell=m_board[row][col];
    bool invert = m_board[row][col]->isInverted();
    int steps = step(row,col,cell->force(), cell->isInverted());
    if(cell->isInverted()){
        steps = -steps;
    }
    if(m_player->type() == Player::Red){
        m_board[row][col+steps]->setInverted(invert);
        m_board[row][col+steps]->setPlayer(m_player);
        verifyEnd(row, col+steps);

    }
    else{
        m_board[row-steps][col]->setInverted(invert);
        m_board[row-steps][col]->setPlayer(m_player);
        verifyEnd(row-steps, col);
    }
}
bool Squadro::verifyWay(int row, int col){
        Cell* cell=m_board[row][col];
        int steps = step(row,col,cell->force(), cell->isInverted());
        int multiplier=1;
        int aux;
        if(cell->isInverted()){//se tiver invertido, vai ser multiplicado por -1 para os jogadores andarem no grid
            multiplier = -1;
        }
        if(m_player->type() == Player::Red){
            for(int i=1;i<=steps;i++){
                aux = i*multiplier;
                if(m_board[row][col+aux]->player()!= nullptr){
                       Cell* cell = m_board[row][col+aux];
                       Player* player = cell->player();
                       eat(row, col+aux,row,col, player);

                       return true;
                }
            }

        }
        else{
            for(int i=1;i<=steps;i++){
                aux = i*multiplier;
                if(m_board[row-aux][col]->player()!= nullptr){
                        Cell* cell = m_board[row-aux][col];
                        Player* player = cell->player();
                        eat(row-aux,col,row,col, player);

                        return true;
                }
            }
        }
        return false;

}
void Squadro::eat(int row_E, int col_E,int row, int col,Player* player){
    int countEnemiesRed=0;
    int countEnemiesBlue=0;
    bool invert = m_board[row][col]->isInverted();
    Cell* cell=m_board[row][col];
    if(m_player->type() == Player::Red){
        int i=col_E;
        while(m_board[row_E][i]->player()!=nullptr){
            countEnemiesBlue++;
            if(cell->isInverted()){
                i--;
            }
            else{
                i++;
            }
        }

    }
    else{ //jogador azul
        int i=row_E;
        while(m_board[i][col_E]->player()!=nullptr){
            countEnemiesRed++;
            if(cell->isInverted()){
                i++;
            }
            else{
                i--;
            }
        }
    }
    if(cell->isInverted()){
        countEnemiesBlue = -countEnemiesBlue;
        countEnemiesRed= -countEnemiesRed;
    }
    int end = m_player->end(cell->isInverted(),m_player);
    if(m_player->type() == Player::Red){
       if(col_E+countEnemiesBlue>6){
            m_board[row][end]->setPlayer(m_player);
            verifyEnd(row, end);
       }
       else{
           m_board[row][col_E+countEnemiesBlue]->setInverted(invert);
           m_board[row][col_E+countEnemiesBlue]->setPlayer(m_player);
           verifyEnd(row, col_E+countEnemiesBlue);
       }

    }
   else{
        if(row_E-countEnemiesRed<0){
            m_board[end][col]->setPlayer(m_player);
            verifyEnd(end, col);

        }
        else{
           m_board[row_E-countEnemiesRed][col]->setInverted(invert);
           m_board[row_E-countEnemiesRed][col]->setPlayer(m_player);
           verifyEnd(row_E-countEnemiesRed, col);
        }

    }
     eated(player, row_E,col_E, row,col,countEnemiesRed, countEnemiesBlue);
}
void Squadro::eated(Player* player_E, int row_E, int col_E,int row,int col, int countEnemiesRed, int countEnemiesBlue){
    int start;
    Cell* cell;

    if(m_board[row][col]->isInverted()){
        countEnemiesBlue = -countEnemiesBlue;
        countEnemiesRed= -countEnemiesRed;
    }
    if(player_E->type() == Player::Red){ // se o jogador a ser comido for vermelho
        if(m_board[row][col]->isInverted()){
            for(int i=row_E;i<countEnemiesRed+row_E; i++){
                cell = m_board[i][col];
                start = cell->player()->initial(cell->isInverted(), player_E);
                cell->reset();
                m_board[i][start]->setPlayer(player_E);
            }
        }
       else{
            for(int i=row_E;i>row_E-countEnemiesRed;i--){
                cell = m_board[i][col];
                start = cell->player()->initial(cell->isInverted(), player_E);
                cell->reset();
                m_board[i][start]->setPlayer(player_E);
            }
        }

    }
    else{
          if(m_board[row][col]->isInverted()){
              for(int i=col_E;i>col_E-countEnemiesBlue;i--){
                  cell = m_board[row][i];
                  start = cell->player()->initial(cell->isInverted(), player_E);
                  cell->reset();
                  m_board[start][i]->setPlayer(player_E);
              }
          }
          else{
              for(int i=col_E;i<countEnemiesBlue+col_E;i++){
                  cell = m_board[row][i];
                  start = cell->player()->initial(cell->isInverted(), player_E);
                  cell->reset();
                  m_board[start][i]->setPlayer(player_E);
              }
          }

    }



}
void Squadro::verifyEnd(int row, int col) {
    int end = m_player->end(m_board[row][col]->isInverted(), m_player);
    Cell* cell = m_board[row][col];
    if(m_player->type() == Player::Red){
        if(col == end){
            if(end==0){
                cell->setParalised(true);
                m_player->incrementCount();
            }
            m_board[row][col]->setInverted(!m_board[row][col]->isInverted());
        }
    }
   else{
        if(row == end){
            if(end==6){
                 cell->setParalised(true);
                m_player->incrementCount();
            }
            m_board[row][col]->setInverted(!m_board[row][col]->isInverted());
        }
    }

}
int Squadro::step(int row, int col, int force, bool inverted){
    if(m_player->type() == Player::Red){
       if(inverted){
          if(col+force<0){
              return col;
          }
          else{
              return force;
          }
       }
       else{
           if(col+force>6){
               return (6-col);
           }
           else{
               return force;
           }
       }

    }
   else{
        if(inverted){
           if(row+force>6){
               return 6-row;
           }
           else{
               return force;
           }
        }
        else{
            if(row-force<0){
                return row;
            }
            else{
                return force;
            }
        }
    }

}
void Squadro::verifyVictory(){
    if(m_player->count()==4){
        showVictory();
        this->reset();
    }

}
void Squadro::showVictory() {
    QMessageBox::information(this, tr("Vencedor"),
        tr("Parabéns, o %1 venceu").arg(m_player->name()));
}
void Squadro::showAbout() {
    QMessageBox::information(this, tr("About"),
        tr("Squadro\n\nDiogo Emanuel Antunes Santos - diogodeas1@gmail.com\n Pedro Henrique Rocha de Castro - rochapedro436@gmail.com"));
}

void Squadro::updateStatusBar() {
    ui->statusbar->showMessage(tr("Vez do %1 (%2 de 4)").arg(m_player->name()).arg(m_player->count()));
}
