#ifndef LUDO_PLAYER_GA_H
#define LUDO_PLAYER_GA_H
#include <QObject>
#include <iostream>
#include "positions_and_dice.h"

class ludo_player_ga : public QObject {
    Q_OBJECT
private:
    std::vector<int> pos_start_of_turn;
    std::vector<int> pos_end_of_turn;
	std::random_device rd;
    std::mt19937 gen;
    int dice_roll;
	double q_table[5][5][5][5][10];	// define q_table as a 5 dimensional array, first 4 numbers equals to number of states
	enum PlayerState{
		home, house, end_position,on_globe, on_board
	};

	enum Actions{step_out_of_home, go_into_house, go_to_end_position, kill, move_to_globe, move_to_star, move_to_star_and_kill, suicide, move_normal, nothing_to_do};


	int make_decision(std::vector<ludo_player_ga::Actions> possible_actions, std::vector<ludo_player_ga::PlayerState> current);
	std::vector<PlayerState> measureState(positions_and_dice relative); // returns the indizes of the state we are in
	std::vector<Actions> measureActions(positions_and_dice relative, std::vector<ludo_player_ga::PlayerState> currentState); // returns the possible actions we can do. 
	double calculateReward(std::vector<PlayerState> a, std::vector<PlayerState> b); 	// comparing states
	void updateQTable(double reward, std::vector<PlayerState> old, std::vector<PlayerState> current, std::vector<Actions> possible_actions);
	std::vector<PlayerState> oldState;
	
	void updateRewardForNextIteration(Actions action);  

	void printSummary(positions_and_dice relative);
	int isStar(int index);
	int getIndexOfPreviousStar(int currentStarIndex);
	int isOccupied(int index, std::vector<int> position);
	bool isOccupiedByEnemy(int index, std::vector<int> position);
	bool isOccupiedByTeammate(int index, std::vector<int> position);
	bool isGlobe(int index, std::vector<int> position);
	bool areEnemysBehind(int index, std::vector<int> position);
	bool isPositionSafe(int isThisPositionSafe, std::vector<int> position);


	Actions oldAction = Actions::move_normal;
	double rewardForNextIteration = 0;
public:
    ludo_player_ga();
    bool useTrainedQTable = false;
    void printQTable();
signals:
    void select_piece(int);
    void turn_complete(bool);
public slots:
    void start_turn(positions_and_dice relative);
    void post_game_analysis(std::vector<int> relative_pos);
};

#endif // LUDO_PLAYER_GA_H
