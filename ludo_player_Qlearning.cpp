#include "ludo_player_Qlearning.h"
#include "game.h"
#include <random>


ludo_player_Qlearning::ludo_player_Qlearning():
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0)
{
    for(int i_a = 0; i_a <number_action; ++i_a) {
           for(int i_s = 0; i_s <number_state; ++i_s) {
              Q[i_s][i_a] = (double)(rand()%1000)/1000.0;
        }
    }

    LEARN_RATE= 0.2;
    EXPLORE_RATE= 0.1; //(10% exploration) 
    discount_factor= 0.8; 
    reward = 0;
    previousstate = 0; // state is niets sensen bij begin
    action=0;
    actionn=0;
    file.open ("qtableprint.txt");
}

void ludo_player_Qlearning::gamenumber(int number){
    if(number>25000){ 
        EXPLORE_RATE = 0.1- ((double)number-25000.0)/100000.0;
        std::cout << EXPLORE_RATE << std::endl;
        if(EXPLORE_RATE<0){
            EXPLORE_RATE=0;
        }
    }
}

ludo_player_Qlearning::~ludo_player_Qlearning() { 
    file.close();
}

void ludo_player_Qlearning::readQtable(){

    std::ifstream is ("qtableread.txt", std::ifstream::binary);
    double a, b, c ,d;
    int index = 0;
    while (is >> a >> b >> c >> d)
    {
        Q[index][0]=a;
        Q[index][1]=b;
        Q[index][2]=c;
        Q[index][3]=d;
        std::cout << "Row " << index << ":\t ";
        std::cout << Q[index][0] << "\t ";
        std::cout << Q[index][1] << "\t ";
        std::cout << Q[index][2] << "\t ";
        std::cout << Q[index][3] << "\n";
        index++;
    }
    /*
    std::ifstream is ("qtableread.txt", std::ifstream::binary);
    if (is) {
    // get length of file:
    is.seekg (0, is.end);
    int length = is.tellg();
    is.seekg (0, is.beg);

    char * buffer = new char [length];

    std::cout << "Reading " << length << " characters... ";
    // read data as a block:
    is.read (buffer,length);

    if (is)
      std::cout << "all characters read successfully.";
    else
      std::cout << "error: only " << is.gcount() << " could be read";
    is.close();

    // ...buffer contains the entire file...

    delete[] buffer;
    }
    */

}


void ludo_player_Qlearning::printQtable(){
    for(int i =0 ; i<65536 ; i++){
        file << Q[i][0] << "\t" << Q[i][1] << "\t" << Q[i][2] << "\t" << Q[i][3] << "\n";
    }
    file.close();
}

int ludo_player_Qlearning::getMaxAction(int stateRL,int ranglijst) { // find the largest Q-value for a given state (j), and return action
    float max = -1000;
    float min = 1000;
    int highestactionn;
    int secondhighestactionn;
    int thirdhighestactionn;
    int lowestactionn;

    if(ranglijst==1){
        for(int a=0;a<number_action;++a) { // Find MAX Q of all actions in this state!
            if(Q [stateRL][a] > max) {
                max = Q [stateRL][a];
                highestactionn = a;
            }
        }
        actionn=highestactionn;
    }

    if(ranglijst==2){
        for(int a=0;a<number_action;++a) { // Find MAX Q of all actions in this state!
            if(Q [stateRL][a] > max) {
                max = Q [stateRL][a];
                highestactionn = a;
            }
        }
        max = -1000;
        for(int a=0;a<number_action;++a) { // Find MAX Q of all actions in this state!
            if(Q [stateRL][a] > max) {
                if(highestactionn!=a){
                    max = Q [stateRL][a];
                    secondhighestactionn=a;
                }
            }
        }
        actionn=secondhighestactionn;
    }

    if(ranglijst==3){
        for(int a=0;a<number_action;++a) { // Find MAX Q of all actions in this state!
            if(Q [stateRL][a] > max) {
                max = Q [stateRL][a];
                highestactionn = a;
            }
        }
        max = -1000;
        for(int a=0;a<number_action;++a) { // Find MAX Q of all actions in this state!
            if(Q [stateRL][a] > max) {
                if(highestactionn!=a){
                    max = Q [stateRL][a];
                    secondhighestactionn=a;
                }
            }
        }
        max = -1000;
        for(int a=0;a<number_action;++a) { // Find MAX Q of all actions in this state!
            if(Q [stateRL][a] > max) {
                if(highestactionn!=a && secondhighestactionn!=a){
                    max = Q [stateRL][a];
                    thirdhighestactionn=a;
                }
            }
        }
        actionn=thirdhighestactionn;
    }

    if(ranglijst==4){
        for(int a=0;a<number_action;++a) { // Find MAX Q of all actions in this state!
            if(Q [stateRL][a] < min) {
                min = Q [stateRL][a];
                lowestactionn = a;
            }
        }
        actionn=lowestactionn;
    }

    return actionn;
}
 
int ludo_player_Qlearning::make_decision(){

    // state representation
    if(DEBUG){
        std::cout << "making decision here" << std::endl;
    }

    playerstate = 0;
    otherpawnsinhomeposition1=0;
    otherpawnsinhomeposition2=0;
    otherpawnsinhomeposition3=0;


    for(int i = 4; i<8;i++){
        if(pos_start_of_turn[i]==-1){
            otherpawnsinhomeposition1++;
        } 
    }

    for(int i = 8; i<12;i++){
        if(pos_start_of_turn[i]==-1){
            otherpawnsinhomeposition2++;
        } 
    }

    for(int i = 12; i<16;i++){
        if(pos_start_of_turn[i]==-1){
            otherpawnsinhomeposition3++;
        } 
    }
    
    //check if globe is empty
    //NU NOG DIE STERREN IMPLEMENTEREN!! 50.48 -> 52.74 -> 53.38

    for (int i=0 ; i < 4 ; i++){
        pionstates[i]=-1;
        possiblepos = pos_start_of_turn[i]+dice_roll;

        if(DEBUG){
            std::cout << possiblepos << std::endl;
        }

        if((pos_start_of_turn[i]<0 && dice_roll != 6) || pos_start_of_turn[i]==99){
            pionstates[i]=0;
        }
        else{
            if(pos_start_of_turn[i]<0 && dice_roll == 6){
                pionstates[i]=15;
            }
            else{
                if(isGlobe(possiblepos) == true){
                    if(isOccupied(possiblepos) != 0){
                        for(int a=1; a<7;a++){
                            if(isOccupied(pos_start_of_turn[i]-a) != 0 || (otherpawnsinhomeposition1 > 0 && pos_start_of_turn[i]-a==13) || (otherpawnsinhomeposition2 > 0 && pos_start_of_turn[i]-a==26) || (otherpawnsinhomeposition3 > 0 && pos_start_of_turn[i]-a==39)){
                                pionstates[i]=1;
                            }
                        }
                        if(pos_start_of_turn[i]==13 || pos_start_of_turn[i]==26|| pos_start_of_turn[i]==39 ){
                            pionstates[i]=1;
                        }
                        if(isStar(pos_start_of_turn[i])==7){
                            for(int a=7; a<13;a++){
                                if(isOccupied(pos_start_of_turn[i]-a) != 0){
                                    pionstates[i]=1;
                                }
                            }
                        }
                        if(isStar(pos_start_of_turn[i])==6){
                            for(int a=8; a<14;a++){
                                if(isOccupied(pos_start_of_turn[i]-a) != 0){
                                    pionstates[i]=1;
                                }
                            }
                        }
                        if(pionstates[i]==-1){
                            pionstates[i]=2;
                        }
                    }
                    else{
                        for(int a=1; a<7;a++){
                            if(isOccupied(pos_start_of_turn[i]-a) != 0 || (otherpawnsinhomeposition1 > 0 && pos_start_of_turn[i]-a==13) || (otherpawnsinhomeposition2 > 0 && pos_start_of_turn[i]-a==26) || (otherpawnsinhomeposition3 > 0 && pos_start_of_turn[i]-a==39)){
                                pionstates[i]=3;
                            }
                        }
                        if(pos_start_of_turn[i]==13 || pos_start_of_turn[i]==26 || pos_start_of_turn[i]==39){
                            pionstates[i]=3;
                        }
                        if(isStar(pos_start_of_turn[i])==7){
                            for(int a=7; a<13;a++){
                                if(isOccupied(pos_start_of_turn[i]-a) != 0){
                                    pionstates[i]=3;
                                }
                            }
                        }
                        if(isStar(pos_start_of_turn[i])==6){
                            for(int a=8; a<14;a++){
                                if(isOccupied(pos_start_of_turn[i]-a) != 0){
                                    pionstates[i]=3;
                                }
                            }
                        }
                        if(pionstates[i]==-1){
                            pionstates[i]=4;
                        }
                    }
                }
                if(isStar(possiblepos) >0 && pos_start_of_turn[i] != -1){
                    if(isOccupied(possiblepos) >0){
                        for(int a=1; a<7;a++){
                            if(isOccupied(pos_start_of_turn[i]-a) != 0 || (otherpawnsinhomeposition1 > 0 && pos_start_of_turn[i]-a==13) || (otherpawnsinhomeposition2 > 0 && pos_start_of_turn[i]-a==26) || (otherpawnsinhomeposition3 > 0 && pos_start_of_turn[i]-a==39)){
                                pionstates[i]=5;
                            }
                        }
                        if(pos_start_of_turn[i]==13 || pos_start_of_turn[i]==26 || pos_start_of_turn[i]==39){
                            pionstates[i]=5;
                        }
                        if(isStar(pos_start_of_turn[i])==7){
                            for(int a=7; a<13;a++){
                                if(isOccupied(pos_start_of_turn[i]-a) != 0){
                                    pionstates[i]=5;
                                }
                            }
                        }
                        if(isStar(pos_start_of_turn[i])==6){
                            for(int a=8; a<14;a++){
                                if(isOccupied(pos_start_of_turn[i]-a) != 0){
                                    pionstates[i]=5;
                                }
                            }
                        }
                        if(pionstates[i]==-1){
                            pionstates[i]=6;
                        }
                    }
                    else{
                        for(int a=1; a<7;a++){
                            if(isOccupied(pos_start_of_turn[i]-a) != 0 || (otherpawnsinhomeposition1 > 0 && pos_start_of_turn[i]-a==13) || (otherpawnsinhomeposition2 > 0 && pos_start_of_turn[i]-a==26) || (otherpawnsinhomeposition3 > 0 && pos_start_of_turn[i]-a==39)){
                                pionstates[i]=7;
                            }
                        }
                        if(pos_start_of_turn[i]==13 || pos_start_of_turn[i]==26 || pos_start_of_turn[i]==39){
                            pionstates[i]=7;
                        }
                        if(isStar(pos_start_of_turn[i])==7){
                            for(int a=7; a<13;a++){
                                if(isOccupied(pos_start_of_turn[i]-a) != 0){
                                    pionstates[i]=7;
                                }
                            }
                        }
                        if(isStar(pos_start_of_turn[i])==6){
                            for(int a=8; a<14;a++){
                                if(isOccupied(pos_start_of_turn[i]-a) != 0){
                                    pionstates[i]=7;
                                }
                            }
                        }
                        if(pionstates[i]==-1){
                            pionstates[i]=8;
                        }
                    }
                }
                if(possiblepos>50){
                    if(possiblepos==56){
                        pionstates[i]=9;
                    }
                    else{
                        pionstates[i]=10;
                    }
                }
                if(pionstates[i]==-1){
                    if(isOccupied(possiblepos) != 0){
                        for(int a=1; a<7;a++){
                            if(isOccupied(pos_start_of_turn[i]-a) != 0 || (otherpawnsinhomeposition1 > 0 && pos_start_of_turn[i]-a==13) || (otherpawnsinhomeposition2 > 0 && pos_start_of_turn[i]-a==26) || (otherpawnsinhomeposition3 > 0 && pos_start_of_turn[i]-a==39)){
                                pionstates[i]=11;
                            }

                        }
                        if(pos_start_of_turn[i]==13 || pos_start_of_turn[i]==26 || pos_start_of_turn[i]==39){
                            pionstates[i]=11;
                        }
                        if(isStar(pos_start_of_turn[i])==7){
                            for(int a=7; a<13;a++){
                                if(isOccupied(pos_start_of_turn[i]-a) != 0){
                                    pionstates[i]=11;
                                }
                            }
                        }
                        if(isStar(pos_start_of_turn[i])==6){
                            for(int a=8; a<14;a++){
                                if(isOccupied(pos_start_of_turn[i]-a) != 0){
                                    pionstates[i]=11;
                                }
                            }
                        }
                        if(pionstates[i]==-1){
                            pionstates[i]=12;
                        }
                    }
                    else{
                        for(int a=1; a<7;a++){
                            if(isOccupied(pos_start_of_turn[i]-a) != 0 || (otherpawnsinhomeposition1 > 0 && pos_start_of_turn[i]-a==13) || (otherpawnsinhomeposition2 > 0 && pos_start_of_turn[i]-a==26) || (otherpawnsinhomeposition3 > 0 && pos_start_of_turn[i]-a==39)){
                                pionstates[i]=13;
                            }

                        }
                        if(pos_start_of_turn[i]==13 || pos_start_of_turn[i]==26 || pos_start_of_turn[i]==39){
                            pionstates[i]=13;
                        }
                        if(isStar(pos_start_of_turn[i])==7){
                            for(int a=7; a<13;a++){
                                if(isOccupied(pos_start_of_turn[i]-a) != 0){
                                    pionstates[i]=13;
                                }
                            }
                        }
                        if(isStar(pos_start_of_turn[i])==6){
                            for(int a=8; a<14;a++){
                                if(isOccupied(pos_start_of_turn[i]-a) != 0){
                                    pionstates[i]=13;
                                }
                            }
                        }
                        if(pionstates[i]==-1){
                            pionstates[i]=14;
                        }
                    }
                }     
            }
        }
        if(DEBUG){
            std::cout<< pionstates[i]<< std::endl;
        }
        
        if(test){
            if((possiblepos - 8) % 13 == 0){
               std::cout << "De pion die zo staat is : " << i << std::endl;
            }
            std::cout << "pos_start_of_turn:";
                for(int j = 0; j<16; j++){
                    std::cout << " " << pos_start_of_turn[j];
                }
        std::cout << std::endl;
    }
        test=false;
    }
    
    if(DEBUG){
        std::cout << std::endl;
    }
    playerstate = pionstates[0] + pionstates[1]*16 + pionstates[2]*16*16 + pionstates[3]*16*16*16;
    if(DEBUG){
        std::cout<< playerstate << std::endl;
    }

    // REWARDS

    presentpositions = 0;
    presentamountofpionsonstart = 0;
    presentnumberofpionsthatfinished=0;

    //new game means resetting variables

    for(int i=0 ; i < 16 ; i++){
        if(pos_start_of_turn[i]==99){
            presentnumberofpionsthatfinished++;
        }
    }

    if(previousnumberofpionsthatfinished>presentnumberofpionsthatfinished){
        previouspositions=0;
        previousamountofpionsonstart=4;
        previousotherplayerspions_previousturn=0;
        presentotherplayerspions_previousturn=0;
        previousstate=0;
        action=0;
    }

    previousnumberofpionsthatfinished=presentnumberofpionsthatfinished;

    for(int i=0 ; i < 4 ; i++){
        presentpositions = presentpositions + pos_start_of_turn[i];
        if(pos_start_of_turn[i]==-1){
            presentamountofpionsonstart++;
        }
    }

    reward = 0;

    if (presentpositions - previouspositions > 6 && presentpositions - previouspositions < 40){
        reward = 10;
    }

    if (presentpositions - previouspositions > 40){
        reward = 8;
    }

    if (presentamountofpionsonstart - previousamountofpionsonstart < 0){
        reward = 6;
    }

    if (presentamountofpionsonstart - previousamountofpionsonstart > 0){
        reward = -20;
    }

    if (presentotherplayerspions_previousturn - previousotherplayerspions_previousturn < 0){
        reward = 4;
        //std::cout << "------------------------------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
    }

    if(DEBUG){
        std::cout << std::endl;
        std::cout << "present : " << presentotherplayerspions_previousturn << " en previous : " << previousotherplayerspions_previousturn << " verschil " << presentotherplayerspions_previousturn - previousotherplayerspions_previousturn << std::endl;
        std::cout << std::endl;
        std::cout << "reward: " << reward << std::endl;
    }

    previousamountofpionsonstart = presentamountofpionsonstart;
    previouspositions = presentpositions;

    previousotherplayerspions_previousturn = 0;
    for(int i=4 ; i < 16 ; i++){
        if(pos_start_of_turn[i]!=-1){
            previousotherplayerspions_previousturn++;
        }
    }
    if(DEBUG){
        for(u_int i = 0; i < pos_start_of_turn.size(); ++i){
            std::cout << " " << pos_start_of_turn[i];
        }
        std::cout << '\n';
    }


    // Q value
    /*
    std::cout << "playerstate: " << playerstate << std::endl;
    std::cout << "action: " << action << std::endl;
    std::cout << "maxaction: " << maxaction << std::endl;
    std::cout << "Q[previousstate][action]: " << Q[previousstate][action] << std::endl;
    std::cout << "Q[playerstate] [maxaction]: " << Q[playerstate][maxaction] << std::endl;
*/

    maxaction = getMaxAction(playerstate,1);
    Q[previousstate][action] = Q[previousstate][action] + LEARN_RATE * (reward + discount_factor*Q[playerstate][maxaction] - Q[previousstate][action]);
    previousstate = playerstate;
    
    /*
    if(eenmalig){
        for(int i = 0; i < 65536 ; i++){
            std::cout << "row " << i << " : " << Q[i][0] << " "<< Q[i][1] << " "<< Q[i][2] << " "<< Q[i][3] << std::endl;
        }
        eenmalig = 0;
    }
    */
    // Action selection (E-greedy)

    movable_pions.clear();
    pions_on_board_without_winners.clear();

    for (int i=0 ; i < 4 ; i++){
        if(pos_start_of_turn[i]!=-1 && pos_end_of_turn[i] !=99){
            pions_on_board_without_winners.push_back(i);
        }
    }

    for (int i=0 ; i < 4 ; i++){
        if(pos_end_of_turn[i] !=99){
            movable_pions.push_back(i);
        }
    }
    
    if(DEBUG){
        std::cout<< " Here comes the movable pions list:" <<std::endl;
    }


    if(DEBUG){
        for (unsigned i=0; i<pions_on_board_without_winners.size(); i++)
            std::cout << ' ' << movable_pions[i];
        std::cout << '\n';
    }


    //std::cout<< "\n dice: " << dice_roll <<std::endl;

    exploration_activation=(double)(rand()%1000)/1000.0;  // between 0....1
    if(exploration_activation < 1-EXPLORE_RATE){
        //std::cout<< " ---- BEREDENEERD GEKOZEN ---- " <<std::endl;
        possibleaction = getMaxAction(playerstate,1); //getMaxAction(j);
        if(pos_start_of_turn[possibleaction]==-1 && dice_roll != 6 && pions_on_board_without_winners.size()!=0){ 
            possibleaction = getMaxAction(playerstate,2);
        }
        if(pos_start_of_turn[possibleaction]==-1 && dice_roll != 6 && pions_on_board_without_winners.size()!=0){ 
            possibleaction = getMaxAction(playerstate,3);
        }
        if(pos_start_of_turn[possibleaction]==-1 && dice_roll != 6 && pions_on_board_without_winners.size()!=0){ 
            possibleaction = getMaxAction(playerstate,4);
        }
        if(pos_start_of_turn[possibleaction]==99 && dice_roll == 6 && movable_pions.size()!=0){
            possibleaction = getMaxAction(playerstate,2);
        }
        if(pos_start_of_turn[possibleaction]==99 && dice_roll == 6 && movable_pions.size()!=0){
            possibleaction = getMaxAction(playerstate,3);
        }
        if(pos_start_of_turn[possibleaction]==99 && dice_roll == 6 && movable_pions.size()!=0){
            possibleaction = getMaxAction(playerstate,4);
        }
        if(pos_start_of_turn[possibleaction]==99 && pions_on_board_without_winners.size()!=0){
            possibleaction = getMaxAction(playerstate,2);
        }
        if(pos_start_of_turn[possibleaction]==99 && pions_on_board_without_winners.size()!=0){
            possibleaction = getMaxAction(playerstate,3);
        }
        if(pos_start_of_turn[possibleaction]==99 && pions_on_board_without_winners.size()!=0){
            possibleaction = getMaxAction(playerstate,4);
        }
        // de volgende if loops zijn gewoon om foutmelding weg te krijgen (gebeurt als pion op plaats 99 gekozen wordt terwijl er minder dan een 6 wordt gegooid en alle pions locked zijn)
        if(pos_start_of_turn[possibleaction] == 99 && movable_pions.size()!=0){
            possibleaction = 0;
        }
        if(pos_start_of_turn[possibleaction] == 99 && movable_pions.size()!=0){
            possibleaction = 1;
        }
        if(pos_start_of_turn[possibleaction] == 99 && movable_pions.size()!=0){
            possibleaction = 2;
        }
        if(pos_start_of_turn[possibleaction] == 99 && movable_pions.size()!=0){
            possibleaction = 3;
        }
    }

    else {
        possibleaction = (int) (4.0*rand()/(RAND_MAX+1.0));
        //std::cout<< " ---- RANDOM GEKOZEN ---- " <<std::endl;
        //std::cout<< " possibleaction : " << possibleaction <<std::endl;
        if(pos_start_of_turn[possibleaction]==-1 && dice_roll != 6 && pions_on_board_without_winners.size()!=0){ 
            possibleaction = pions_on_board_without_winners[0];
        }
        if(pos_start_of_turn[possibleaction]==99 && dice_roll == 6 && movable_pions.size()!=0){
            possibleaction = movable_pions[0];
        }
        if(pos_start_of_turn[possibleaction]==99 && pions_on_board_without_winners.size()!=0){
            possibleaction = pions_on_board_without_winners[0];
        }
                // de volgende if loops zijn gewoon om foutmelding weg te krijgen (gebeurt als pion op plaats 99 gekozen wordt terwijl er minder dan een 6 wordt gegooid en alle pions locked zijn)
        if(pos_start_of_turn[possibleaction] == 99 && movable_pions.size()!=0){
            possibleaction = 0;
        }
        if(pos_start_of_turn[possibleaction] == 99 && movable_pions.size()!=0){
            possibleaction = 1;
        }
        if(pos_start_of_turn[possibleaction] == 99 && movable_pions.size()!=0){
            possibleaction = 2;
        }
        if(pos_start_of_turn[possibleaction] == 99 && movable_pions.size()!=0){
            possibleaction = 3;
        }
    }

    if(DEBUG){
        std::cout<< " action : " << action<<std::endl;
    }

    action = possibleaction;

    if(pos_start_of_turn[action] == 99){
        std::cout<< " dice roll : " << dice_roll <<std::endl;
        for(u_int i = 0; i < pos_start_of_turn.size(); ++i){
            std::cout << " " << pos_start_of_turn[i];
        }
        std::cout << '\n';
    }

    if(test2){
        std::cout << "De action is : " << action << std::endl;   
    }
    
    return action;
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
    return -1;
    */
}

void ludo_player_Qlearning::start_turn(positions_and_dice relative){
    pos_start_of_turn = relative.pos;
    dice_roll = relative.dice;
    int decision = make_decision();
    emit select_piece(decision);
}

void ludo_player_Qlearning::post_game_analysis(std::vector<int> relative_pos){
    presentotherplayerspions_previousturn=0;

    pos_end_of_turn = relative_pos;

    for(int i = 4; i<16; i++){
        if(pos_end_of_turn[i]!=-1){
            presentotherplayerspions_previousturn++;
        }
    }

    
    if(DEBUG){
        for(u_int i = 0; i < pos_start_of_turn.size(); ++i){
            std::cout << " " << pos_end_of_turn[i];
        }
        std::cout << " da was de pos end of turn" << '\n';
    }

    if(test2){
        std::cout << "pos_end_of_turn:";
            for(int i = 0; i<16; i++){
                std::cout << " " << pos_end_of_turn[i];
            }
            test2=false;
        std::cout << std::endl;
    }
    
    bool game_complete = true;
    for(int i = 0; i < 4; ++i){
        if(pos_end_of_turn[i] < 99){
            game_complete = false;
        }
    }
    emit turn_complete(game_complete);
}

int ludo_player_Qlearning::isStar(int index){
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

bool ludo_player_Qlearning::isGlobe(int index){
    test=false;
    if(index < 52){     //check only the indexes on the board, not in the home streak
        if(index % 13 == 0 || (index - 8) % 13 == 0 || isOccupied(index) > 1){  //if more people of the same team stand on the same spot it counts as globe
            if((index - 8) % 13 == 0){
                //test=true;
                //test2=true;
                //std::cout<< "-----------------" << std::endl;
            }
            return true;
        }
    }
    return false;
}

int ludo_player_Qlearning::isOccupiedtotal(int index){ //returns number of people
    int number_of_people = 0;
    if(index != 99){
        for(int j = 0; j<16; j++){
            if(pos_start_of_turn[j] == index){
                ++number_of_people;
            }
        }
    }
    return number_of_people;
}

int ludo_player_Qlearning::isOccupiedmyown(int index){ //returns number of people
    int number_of_people = 0;
    if(index != 99){
        for(int j = 0; j<4; j++){
            if(pos_start_of_turn[j] == index){
                ++number_of_people;
            }
        }
    }
    return number_of_people;
}

int ludo_player_Qlearning::isOccupied(int index){ //returns number of people
    int number_of_people = 0;
    if(index != 99){
        for(int j = 4; j<16; j++){
            if(pos_start_of_turn[j] == index){
                ++number_of_people;
            }
        }
    }
    return number_of_people;
}

