#include "ludo_player_ga.h"
#include <random>
#include <algorithm>

ludo_player_ga::ludo_player_ga():
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    rd(),
    gen(rd()),
    dice_roll(0),
	q_table{}	// initialize q-table with zeros
{
	for(int i = 0; i < 4; i++) oldState.push_back(PlayerState::home); // Initialize oldState with home-positions
}

int ludo_player_ga::make_decision(std::vector<Actions> possible_actions, std::vector<PlayerState> current){
    if(useTrainedQTable){
    	/*
    	double qMax = q_table[current[0]][current[1]][current[2]][current[3]][0];
    	int indexOfQMax = 0;
    	for(int i = 1; i < 9; i++){
			if(q_table[current[0]][current[1]][current[2]][current[3]][i] > qMax){
			 qMax = q_table[current[0]][current[1]][current[2]][current[3]][i];
			 indexOfQMax = i;
			}
		}
		*/

		std::vector<double> x; 
		for(int i = 0; i < 9; i++){
    		x.push_back(q_table[current[0]][current[1]][current[2]][current[3]][i]);
    	}

	    //std::vector<int> y;

	    std::vector<int> y(x.size());
	    std::size_t n(0);
	    std::generate(std::begin(y), std::end(y), [&]{ return n++; });

	    std::sort(  std::begin(y), std::end(y), [&](double i1, double i2) { return x[i1] > x[i2]; } );

	    for (auto v : y)
	        std::cout << v << ' ';

	    for(int i = 0; i < 9; i++){
	    	Actions a = Actions(y[i]);

	    	for(int j = 0; j < 4; j++){
				if(possible_actions[j]==a){

					return j;
				}
			}
	    }


		// = 

		/*
		Actions a = Actions(indexOfQMax);

		for(int i = 0; i < 4; i++){
			if(possible_actions[i]==a){
				return i;
			}
		}
		std::cout << "Can't find it :/" << std::endl;
		return 0;
		*/

    }
    // else do random action selection


    int badmove_counter = 0;
    for(int i = 0; i < 4; i++){
    	if(possible_actions[i] == Actions::nothing_to_do){
    		badmove_counter++;
    	}
    }

    if(badmove_counter == 4){
    	return 0;
    	std::cout << "Yoooooo" << std::endl;
    }

    else{
    	int select;
    	while(true){
    		std::uniform_int_distribution<> piece(0, possible_actions.size()-1);
    		select = piece(gen);
    		if(possible_actions[select] != Actions::nothing_to_do){
    			return select;
    		}

    	}
    }

    /*
    std::vector<int> valid_moves;
    if(dice_roll == 6){
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]<0){
                valid_moves.push_back(i);
            }
        }
    }
    for(int i = 0; i < 4; ++i){
        if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99){
            valid_moves.push_back(i);
        }
    }
    if(valid_moves.size()==0){
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i] != 99){
                valid_moves.push_back(i);
            }
        }
    }
    std::uniform_int_distribution<> piece(0, valid_moves.size()-1);
    int select = piece(gen);
    */
    return -1;
   
}

void ludo_player_ga::start_turn(positions_and_dice relative){
    pos_start_of_turn = relative.pos;
    dice_roll = relative.dice;

	//printSummary(relative);
	std::vector<ludo_player_ga::PlayerState> currentState = measureState(relative);
	std::vector<ludo_player_ga::Actions> possible_actions = measureActions(relative, currentState);

	double reward = calculateReward(oldState, currentState);
	updateQTable(reward, oldState, currentState);
	
    int decision = make_decision(possible_actions, currentState); // random if in learning mode
    updateRewardForNextIteration(possible_actions[decision]);

    oldAction = possible_actions[decision];
    oldState = currentState;
    emit select_piece(decision);
}

std::vector<ludo_player_ga::PlayerState> ludo_player_ga::measureState(positions_and_dice relative){

	std::vector<ludo_player_ga::PlayerState> currentState;

	std::vector<int> position = relative.pos;
	//int dice = relative.dice;



	for(int i = 0; i < 4; i++){ // For all own players, calculate PlayerState
		// home position
		if(position[i] == -1) { currentState.push_back(PlayerState::home); continue; } 

		// house position
		if(position[i] >=52 && position[i] <=56) { currentState.push_back(PlayerState::house); continue; }

		// end_position
		if(position[i] == 99) { currentState.push_back(PlayerState::end_position); continue; }

		// on_globe
		if(isGlobe(position[i], position) || (isOccupied(position[i], position) > 1)) { currentState.push_back(PlayerState::on_globe); continue; }

		// on_board

		currentState.push_back(PlayerState::on_board); 



/*
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

*/
	}
/*

	std::cout << "CurrentState: ";  
	for(unsigned int i = 0; i < currentState.size(); ++i){
    	std::cout << currentState[i] << " " ;    
	}
	std::cout << " End CurrentState" << std::endl;
*/
	return currentState;
}


std::vector<ludo_player_ga::Actions> ludo_player_ga::measureActions(positions_and_dice relative, std::vector<ludo_player_ga::PlayerState> currentState){

	std::vector<ludo_player_ga::Actions> possible_actions;

	std::vector<int> position = relative.pos;

	int dice = relative.dice;


	for(int i = 0; i < 4; i++){
		if(currentState[i] == PlayerState::home){
			if(dice == 6){
				possible_actions.push_back(Actions::step_out_of_home);
				continue;
			}else{
				possible_actions.push_back(Actions::nothing_to_do);
				continue;
			}
		}

		if(currentState[i] == PlayerState::end_position){
			possible_actions.push_back(Actions::nothing_to_do);
			continue;
		}

		if(currentState[i] == PlayerState::house){
			if(position[i] + dice == 57){
				possible_actions.push_back(Actions::go_to_end_position);
				continue;
			} else{
				possible_actions.push_back(Actions::move_normal);	// Improvable, 
				continue;
			}
		}

		if(currentState[i] == PlayerState::on_globe || currentState[i] == PlayerState::on_board){
			//, go_to_end_position, go_into_house,kill, move_to_globe, move_to_star, move_to_star_and_kill, suicide, move_normal
			
			if(position[i] + dice > 51){ // go into house
				if(position[i] + dice == 57){
					possible_actions.push_back(Actions::go_to_end_position);
					continue;	
				}else{
					possible_actions.push_back(Actions::go_into_house);
					continue;	
				}				
			}
			if(isOccupiedByEnemy(position[i]+dice, position)){
				if(isOccupied(position[i] + dice, position) > 1 || isGlobe(position[i] + dice, position)){
					possible_actions.push_back(Actions::suicide);
					continue;
				}else{
					possible_actions.push_back(Actions::kill);
					continue;
				}
			}	

			if(isStar(position[i]+dice) > 0){
				if(isOccupiedByEnemy(isStar(position[i]+dice) + dice + position[i], position)){
					if(isOccupied(position[i] + dice, position) > 1 || isGlobe(position[i] + dice, position)){
						possible_actions.push_back(Actions::suicide);
						continue;
					}else{
						possible_actions.push_back(Actions::move_to_star_and_kill);
						continue;
					}
				}else{
					possible_actions.push_back(Actions::move_to_star);
					continue;
				}
			}

			if(isGlobe(position[i]+dice, position) || isOccupiedByTeammate(position[i]+dice, position)){
				possible_actions.push_back(Actions::move_to_globe);
				continue;
			}

			possible_actions.push_back(Actions::move_normal);
		}
	}
/*
	std::cout << "possible_actions: ";  
	for(unsigned int i = 0; i < possible_actions.size(); ++i){
    	std::cout << possible_actions[i] << " " ;    
	}
	std::cout << " end possible_actions" << std::endl;
	*/
	return possible_actions;
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

    if(game_complete){
    	positions_and_dice relative;
    	relative.pos = relative_pos;
    	relative.dice = 0;
    	std::vector<ludo_player_ga::PlayerState> currentState = measureState(relative);
    	double reward = calculateReward(oldState, currentState);
		updateQTable(reward, oldState, currentState);

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
        if(index % 13 == 0 || (index - 8) % 13 == 0){  // Checks just the globe-positions and home-positions
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




double ludo_player_ga::calculateReward(std::vector<ludo_player_ga::PlayerState> old, std::vector<ludo_player_ga::PlayerState> current){
	double reward = rewardForNextIteration;
	
	int endPositions;
	for(int i = 0; i < 4; i++){
		if(old[i] == PlayerState::on_board && current[i] == PlayerState::home){
			// Someone killed our player
			reward -= 5;
		}
		if(current[i] == PlayerState::end_position) endPositions++;
	}

	if(endPositions == 4){
		reward += 100;
		std::cout << "WOOOON" << std::endl;
		//exit(0);
	}



	return reward;
}


void ludo_player_ga::updateQTable(double reward, std::vector<ludo_player_ga::PlayerState> old, std::vector<ludo_player_ga::PlayerState> current){

	double alpha = 0.5;
	double gamma = 0.9;
	double oldQValue = q_table[old[0]][old[1]][old[2]][old[3]][oldAction];
	double qMax = q_table[current[0]][current[1]][current[2]][current[3]][0];
	for(int i = 1; i < 9; i++){
		if(q_table[current[0]][current[1]][current[2]][current[3]][i] > qMax) qMax = q_table[current[0]][current[1]][current[2]][current[3]][i];

	}

	double delta_q_old = alpha * (reward + gamma * qMax - oldQValue);

	q_table[old[0]][old[1]][old[2]][old[3]][oldAction] = oldQValue + delta_q_old;
	std::cout << q_table[old[0]][old[1]][old[2]][old[3]][oldAction] << std::endl;

}


void ludo_player_ga::updateRewardForNextIteration(Actions action){
	switch(action){
		case Actions::kill:
			rewardForNextIteration = 2;
			break;
		case Actions::step_out_of_home:
			rewardForNextIteration = 2;
			break;
		case Actions::go_into_house:
			rewardForNextIteration = 2;
			break;
		case Actions::go_to_end_position:
			rewardForNextIteration = 5;
			break;
		case Actions::move_to_star_and_kill:
			rewardForNextIteration = 3;
			break;
		case Actions::move_to_star:
			rewardForNextIteration = 1;
			break;
		case Actions::suicide:
			rewardForNextIteration = -10;
			break;
		case Actions::move_to_globe:
			rewardForNextIteration = 1;
			break;
		default:
			rewardForNextIteration = 0; 
	}		
}