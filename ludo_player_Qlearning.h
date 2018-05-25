#ifndef LUDO_PLAYER_QLEARNING_H
#define LUDO_PLAYER_QLEARNING_H
#include <QObject>
#include <iostream>
#include "positions_and_dice.h"
#include <fstream>
#include <stdlib.h>

#define  number_state   83251
#define  number_action  4

class ludo_player_Qlearning : public QObject {
    Q_OBJECT
private:
    std::vector<int> pos_start_of_turn;
    std::vector<int> pos_end_of_turn ;
    std::vector<int> movable_pions ;
    std::vector<int> pions_on_board_without_winners ;
    int dice_roll;
    int make_decision();

    std::random_device rd;
    std::mt19937 gen;
    int getMaxAction(int stateRL, int ranglijst); // find the largest Q-value for a given state (j), and return action

    int isStar(int index);
    bool isGlobe(int index);
    int isOccupied(int index); //see if it is occupied and return the piece number
    int isOccupiedtotal(int index); //see if it is occupied and return the piece number
    int isOccupiedmyown(int index); //see if it is occupied and return the piece number

    int pionstates[4];
    int DEBUG = FALSE;
    int eenmalig = 1;



public:

    ~ludo_player_Qlearning(); 
    void printQtable();
    void readQtable();
    void gamenumber(int number);
    int isOccupiedthemselves(int index);

    int possiblepos;
    std::ofstream file;
    std::ifstream is;

    bool test = false;
    bool test2= false;

    int previouspositions=0;
    int otherpawnsinhomeposition1;
    int otherpawnsinhomeposition2;
    int otherpawnsinhomeposition3;
    int presentpositions=0;
    int previousamountofpionsonstart=4;
    int presentamountofpionsonstart=4;
    int previousotherplayerspions_previousturn=0;
    int presentotherplayerspions_previousturn=0;
    int previousnumberofpionsthatfinished=0;
    int presentnumberofpionsthatfinished=0;
    int amountofpawnsinhomestreet_previously=0;
    int amountofpawnsinhomestreet_now=0;
    int actionn;
    // Dit is hier alles voor de Qlearning
    double LEARN_RATE;
    double EXPLORE_RATE;
    double discount_factor;
    int maxaction=0;
    int action;
    int possibleaction;
    int reward;
    double e[2];
    double exploration_activation;
    double Q [number_state][number_action];
    long previousstate; //dit is de state bij het begin van de game
    long playerstate;

    ludo_player_Qlearning();
signals:
    void select_piece(int);
    void turn_complete(bool);
public slots:
    void start_turn(positions_and_dice relative);
    void post_game_analysis(std::vector<int> relative_pos);
};

#endif // LUDO_PLAYER_QLEARNING_H
