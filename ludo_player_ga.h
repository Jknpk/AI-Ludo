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
	double q_table[22][22][22][22][4];	// define q_table as a 5 dimensional array, first 4 numbers equals to number of states
	enum PlayerState{home /* done */, safe_on_board, house /* done */, end_position /* done */,
					 reach_globe /* done */ , reach_star /* done */, reach_hit /* done */, reach_star_and_hit /* done */,  
					 
					 suicide, 														// done

					 danger_before_move, 
					 danger_after_move,
					 danger_before_move_danger_after_move,

					 danger_before_move_but_can_reach_globe, 						// done

					 danger_before_move_but_can_reach_star,							// done
					 danger_before_move_but_can_reach_star_but_danger_after_move,	// done
					 reach_star_but_danger_after_move,								// done

					 danger_before_move_but_can_reach_hit,							// done
					 danger_before_move_but_can_reach_hit_but_danger_after_move,	// done
					 reach_hit_but_danger_after_move,								// done

					 danger_before_move_but_can_reach_star_and_hit,					// done
					 danger_before_but_can_reach_star_and_hit_but_danger_after_move,// done
					 reach_star_and_hit_but_danger_after_move                       // done

					 //what about reaching an enemy globe after star-travel? let's add it to suicide
					 //what about reaching a friendly globe after star-travel? Not considered right now..
	};
	int make_decision(std::vector<PlayerState> current);
	std::vector<PlayerState> measureState(positions_and_dice relative); // returns the indizes of the state we are in
	double calculateReward(std::vector<PlayerState> a, std::vector<PlayerState> b); 	// comparing states
	void updateQTable(double reward, std::vector<PlayerState> old, std::vector<PlayerState> current);
	std::vector<PlayerState> oldState;
	
	void updateRewardForNextIteration(std::vector<PlayerState> currentState, int decision);  

	void printSummary(positions_and_dice relative);
	int isStar(int index);
	int getIndexOfPreviousStar(int currentStarIndex);
	int isOccupied(int index, std::vector<int> position);
	bool isOccupiedByEnemy(int index, std::vector<int> position);
	bool isOccupiedByTeammate(int index, std::vector<int> position);
	bool isGlobe(int index, std::vector<int> position);
	bool areEnemysBehind(int index, std::vector<int> position);
	bool isPositionSafe(int isThisPositionSafe, std::vector<int> position);


	int oldDecision = 0;
	double rewardForNextIteration = 0;
public:
    ludo_player_ga();
    bool useTrainedQTable = false;
signals:
    void select_piece(int);
    void turn_complete(bool);
public slots:
    void start_turn(positions_and_dice relative);
    void post_game_analysis(std::vector<int> relative_pos);
};

#endif // LUDO_PLAYER_GA_H
