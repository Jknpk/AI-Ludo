#include "ludo_player_ga.h"
#include <random>

ludo_player_ga::ludo_player_ga():
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0),
	q_table{}	// initialize q-table with zeros
{
}

int ludo_player_ga::make_decision(){
    /*
    if(dice_roll == 6){
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]<0){
                return i;
            }
        }
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99){
                return i;
            }
        }
    } else {
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99){
                return i;
            }
        }
        for(int i = 0; i < 4; ++i){ //maybe they are all locked in
            if(pos_start_of_turn[i]<0){
                return i;
            }
        }
    }
    */
    return 1;
}

void ludo_player_ga::start_turn(positions_and_dice relative){
    pos_start_of_turn = relative.pos;
    dice_roll = relative.dice;
	printSummary(relative);
	measureState(relative);
    int decision = make_decision();
    emit select_piece(decision);
}

std::vector<ludo_player_ga::PlayerState> ludo_player_ga::measureState(positions_and_dice relative){

	std::vector<ludo_player_ga::PlayerState> currentState;

	std::vector<int> position = relative.pos;
	int dice = relative.dice;


	for(int i = 0; i < 4; i++){ // For all own players, calculate PlayerState

		// Stuck in home position 
		if(dice != 6 && position[i] < 0) { currentState.push_back(PlayerState::home); continue;} 

		// reach or stay in final position  
		if(position[i]+dice ==57 || position[i]==99) { currentState.push_back(PlayerState::end_position); continue;} 
		
		// house position
		if((position[i]+dice >=52 && position[i]+dice <=56) || position[i]+dice > 57) { currentState.push_back(PlayerState::house); continue;} 
		

		// checking for globes

		// checking for enemy-globe aka suicide
		if((position[i] == -1 && dice == 6 && isOccupiedByEnemy(0, position)) ||
			(isGlobe(position[i]+dice, position) && isOccupiedByEnemy(position[i]+dice, position)))
		{
			currentState.push_back(PlayerState::suicide); 
			continue;
		}
			  
			

		// danger_before_move_but_can_reach_globe
		if((isGlobe(position[i]+dice, position) && !isOccupiedByEnemy(position[i]+dice, position)) && !isPositionSafe(position[i], position)|| // A globe that is not occupied by an enemy
		    isOccupiedByTeammate(position[i]+dice,position) && !isPositionSafe(position[i], position)) // Make it a globe)
		{
			currentState.push_back(PlayerState::danger_before_move_but_can_reach_globe); 
			continue;  
		}

		// reach_globe (no danger)
		if((position[i] == -1 && dice == 6) || // In home position and want to get out with a 6
		   (isGlobe(position[i]+dice, position) && !isOccupiedByEnemy(position[i]+dice, position)) || // A globe that is not occupied by an enemy
		    isOccupiedByTeammate(position[i]+dice,position)) // Make it a globe
		{
			currentState.push_back(PlayerState::reach_globe); 
			continue;  
		}


		// checking for stars

		if(isStar(position[i]+ dice) != 0){
			bool isPositionBeforeMoveSafe = isPositionSafe(position[i], position);
			bool isPositionAfterMoveSafe = isPositionSafe(position[i]+dice, position);

			if(!isPositionBeforeMoveSafe && !isPositionAfterMoveSafe){
				currentState.push_back(PlayerState::danger_before_move_but_can_reach_star_but_danger_after_move); 
				continue;
			}
			if(!isPositionBeforeMoveSafe){
				currentState.push_back(PlayerState::danger_before_move_but_can_reach_star); 
				continue;
			}
			if(!isPositionAfterMoveSafe){
				currentState.push_back(PlayerState::reach_star_but_danger_after_move); 
				continue;
			}
			currentState.push_back(PlayerState::reach_star); 
			continue;
		}


		// checking for hits
		if(isOccupiedByEnemy(position[i]+ dice, position)){
			bool isPositionBeforeMoveSafe = isPositionSafe(position[i], position);
			bool isPositionAfterMoveSafe = isPositionSafe(position[i]+dice, position);

			if(!isPositionBeforeMoveSafe && !isPositionAfterMoveSafe){
				currentState.push_back(PlayerState::danger_before_move_but_can_reach_hit_but_danger_after_move); 
				continue;
			}
			if(!isPositionBeforeMoveSafe){
				currentState.push_back(PlayerState::danger_before_move_but_can_reach_hit); 
				continue;
			}
			if(!isPositionAfterMoveSafe){
				currentState.push_back(PlayerState::reach_hit_but_danger_after_move); 
				continue;
			}
			currentState.push_back(PlayerState::reach_hit); 
			continue;
		}

		// checking for star & hits at once
		if(isStar(position[i]+ dice) != 0 && isOccupiedByEnemy(position[i]+ dice + isStar(position[i]+ dice), position)){
			bool isPositionBeforeMoveSafe = isPositionSafe(position[i], position);
			bool isPositionAfterMoveSafe = isPositionSafe(position[i]+dice, position);

			if(!isPositionBeforeMoveSafe && !isPositionAfterMoveSafe){
				currentState.push_back(PlayerState::danger_before_but_can_reach_star_and_hit_but_danger_after_move); 
				continue;
			}
			if(!isPositionBeforeMoveSafe){
				currentState.push_back(PlayerState::danger_before_move_but_can_reach_star_and_hit); 
				continue;
			}
			if(!isPositionAfterMoveSafe){
				currentState.push_back(PlayerState::reach_star_and_hit_but_danger_after_move); 
				continue;
			}
			currentState.push_back(PlayerState::reach_star_and_hit); 
			continue;
		}


		// executing a normal turn


		bool isPositionBeforeMoveSafe = isPositionSafe(position[i], position);
		bool isPositionAfterMoveSafe = isPositionSafe(position[i]+dice, position);

		if(!isPositionBeforeMoveSafe && !isPositionAfterMoveSafe){
			currentState.push_back(PlayerState::danger_before_move_danger_after_move); 
			continue;
		}
		if(!isPositionBeforeMoveSafe){
			currentState.push_back(PlayerState::danger_before_move); 
			continue;
		}
		if(!isPositionAfterMoveSafe){
			currentState.push_back(PlayerState::danger_after_move); 
			continue;
		}
		currentState.push_back(PlayerState::safe_on_board); 
		continue;


		//std::cout << rules.isStar(dice);

	}


	std::cout << "CurrentState: ";  
	for(unsigned int i = 0; i < currentState.size(); ++i){
    	std::cout << currentState[i] << " " ;    
	}
	std::cout << " End CurrentState" << std::endl;

	return currentState;
}





void ludo_player_ga::printSummary(positions_and_dice relative){
	std::cout << "Start Turn: ";    
	for(unsigned int i = 0; i < relative.pos.size(); ++i){
    	std::cout << relative.pos[i] << " " ;    
	}
	std::cout << " End Turn" << std::endl;
	//std::cout << "Q-table value: " << q_table[0][0][0][0][0] << std::endl;
}


void ludo_player_ga::post_game_analysis(std::vector<int> relative_pos){
    pos_end_of_turn = relative_pos;
    bool game_complete = true;
    for(int i = 0; i < 4; ++i){
        if(pos_end_of_turn[i] < 99){
            game_complete = false;
        }
    }
    emit turn_complete(game_complete);
}






int ludo_player_ga::isOccupied(int index, std::vector<int> position){ //returns number of people standing on a specific field
    int number_of_people = 0;

    for(int i = 0; i < position.size(); i++){
    	if(position[i] == index) number_of_people++;
    }


    return number_of_people;
}


bool ludo_player_ga::isGlobe(int index, std::vector<int> position){
    if(index < 52){     //check only the indexes on the board, not in the home streak
        if(index % 13 == 0 || (index - 8) % 13 == 0 || isOccupied(index, position) > 1){  //if more people of the same team stand on the same spot it counts as globe
            return true;
        }
    }
    return false;
}




// returns true if at least one enemy stands on the index-position
bool ludo_player_ga::isOccupiedByEnemy(int index, std::vector<int> position){
	for(int i = 4; i < position.size(); i++){
		if(position[i] == index) return true;
	}
	return false;
}

// returns true if at least one teammate stands on the index-position
bool ludo_player_ga::isOccupiedByTeammate(int index, std::vector<int> position){
	for(int i = 0; i < 4; i++){
		if(position[i] == index) return true;
	}
	return false;	
}




// stolen helper functions
int ludo_player_ga::isStar(int index){
    if(index == 5  ||
       index == 18 ||
       index == 31 ||
       index == 44){
        return 6;
    } else if(index == 11 ||
              index == 24 ||
              index == 37 ||
              index == 50){
        return 7;
    }
    return 0;
}



int ludo_player_ga::getIndexOfPreviousStar(int currentStarIndex){
	int stars[8] = {5, 11, 18, 24, 31, 37, 44, 50};
	for(int i = 0; i < 8; i++){
		if(stars[i] == currentStarIndex){
			if(i==0){return stars[7];}
			return stars[i-1];
		}
	} 
	return -1;

}



// Checks the 6 previous positions behind the watched index-position
bool ludo_player_ga::areEnemysBehind(int index, std::vector<int> position){
	for(int i = 4; i < position.size(); i++){
		for(int j = 1; j <=6; j++){
			int danger = position[i]+j;
			if(danger>=51){danger=danger-51;}	// It's a circle!
			if(index == danger){
				return true;
			}
		}
	}
	return false;
}


// Check if enemys could hit the current Position during the next round. Also checks possible fast-travellers using stars
bool ludo_player_ga::isPositionSafe(int isThisPositionSafe, std::vector<int> position){
	// when do we have danger?
		
		// If others are 1-6 behind
	if(areEnemysBehind(isThisPositionSafe, position)) return false;


	// If we are on a star and someone could reach the star behind us
	if(isStar(isThisPositionSafe)){
		int indexOfPreviousStar= getIndexOfPreviousStar(isThisPositionSafe);
		if(areEnemysBehind(indexOfPreviousStar, position)) return false;
	}	
	return true;
}

