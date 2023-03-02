#ifndef CELL_H
#define CELL_H

#include <QPushButton>

class Player;

class Cell : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(Player* player READ player WRITE setPlayer NOTIFY playerChanged)
    Q_PROPERTY(bool inverted READ isInverted WRITE setInverted NOTIFY invertedChanged)
    Q_PROPERTY(int lapCounter READ lapCounter NOTIFY lapCounterChanged)
    Q_PROPERTY(int paralised READ isParalised NOTIFY paralisedChanged)
   // Q_PROPERTY(int force READ force WRITE setForce NOTIFY forceChanged)

public:
    explicit Cell(QWidget *parent = nullptr);
    virtual ~Cell();

    bool isInverted() const { return m_inverted; }
    bool isParalised() const{ return m_paralised;}
    void setInverted(bool inverted);
    void setParalised(bool paralised);
    int lapCounter() const {return m_lapCounter;}
    Player* player() const { return m_player; }
    void setPlayer(Player* player);
    void setForce(int row, int col);
    int force() const { return m_force; }

public slots:
    void reset();
     void incrementLap();
     void setLap(int lap);
signals:
    void playerChanged(Player* player);
    void invertedChanged(bool inverted);
    void lapCounterChanged(int lapCounter);
    void paralisedChanged(bool paralised);


private:
    Player* m_player;
    bool m_inverted;
    int m_force;
    int m_lapCounter;
    bool m_paralised;
private slots:
    void updateCell();


};

#endif // CELL_H
