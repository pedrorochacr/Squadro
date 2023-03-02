#ifndef SQUADRO_H
#define SQUADRO_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Squadro; }
QT_END_NAMESPACE

class Cell;
class Player;

class Squadro : public QMainWindow {
    Q_OBJECT

public:
    Squadro(QWidget *parent = nullptr);
    virtual ~Squadro();

signals:
    void turnEnded();

private:
    Ui::Squadro *ui;
    Player* m_player;
    Cell* m_board[7][7];

private slots:
    void play(int id);
    void switchPlayer();
    void reset();
    bool verifyWay(int row, int col);
    void move(int row, int col);
    void jump(int row, int col);
    void verifyEnd(int row, int col);
    void eat(int row_E,int col_E,int row, int col, Player* player);
    void showAbout();
    void updateStatusBar();
    void eated(Player* player_E, int row_E, int col_E,int row,int col, int countEnemiesRed, int countEnemiesBlue);
    void verifyVictory();
    void showVictory();
    int step(int row, int col,int force, bool inverted);
};

#endif // SQUADRO_H
