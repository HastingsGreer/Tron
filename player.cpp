#pragma GCC optimize 2

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <bitset>
#include <bit>
#include <chrono> 
using namespace std::chrono; 
using namespace std;

 
struct game {
    unsigned int board[22];
    int N;
    int P;
    int X[4];
    int Y[4];
};

const int actions[8] = {-1, 0, 1, 0, 0, -1, 0, 1};

int x0, y0;

game newgame(){
    game res;
    for(int i = 0; i < 22; i++){
        res.board[i] = 0x80000001;
    }
    res.board[0] = 0xFFFFFFFF;
    res.board[21] = 0xFFFFFFFF;
    for(int i = 0; i < 4; i++){
        res.X[i] = -1;
        res.Y[i] = -1;
    }
    return res;
}

void print(game g){
    for(int i = 0; i < 22; i++){
        std::cerr << std::bitset<32>(g.board[i])  << std::endl;
    }
    for(int i = 0; i < 4; i++){
        std::cerr << g.X[i]<< " " << g.Y[i]<< std::endl;
    }
}

inline int actionx(int x, int action) {
    return x + actions[action * 2];
}
inline int actiony(int y, int action) {
    return y + actions[action * 2 + 1];
}

int num_evals = 0;
double score(game b, int player_idx, int action){
    num_evals += 1;
    
    if (!b.X[player_idx]) {
        return -1000000;
    }
    int player_x = actionx(b.X[player_idx], action);
    int player_y = actiony(b.Y[player_idx], action);
    
    if (b.board[player_y] & (1 << player_x)) {
        return -1000000;
    }
    
    int player_score = 0;
    unsigned int player_positions[22] = {0};
    unsigned int new_positions[22] = {0};
    unsigned int enemy_positions[22] = {0};
    
    for(int i = 0; i < 4; i++){
        if ( i == player_idx){
            b.board[player_y] |= 1 << player_x;
            player_positions[player_y] |= 1 << player_x;
        } else {
            enemy_positions[b.Y[i]] |= 1 << b.X[i];
        }
    }
    for(int count = 0; count < 15; count++){
        
        memset(new_positions, 0, sizeof(new_positions));
        for(int y = 1; y < 21; y++){
            unsigned int row = 0;
            row = enemy_positions[y - 1];
            row |= enemy_positions[y + 1];
            row |= enemy_positions[y] << 1;
            row |= enemy_positions[y] >> 1;
            row &= ~b.board[y];
            new_positions[y] = row;
        }
        memcpy(enemy_positions, new_positions, sizeof(new_positions));
        for(int y = 1; y < 21; y++){
            b.board[y] |= enemy_positions[y];
        }
        int old_score = player_score;
        memset(new_positions, 0, sizeof(new_positions));
        for(int y = 1; y < 21; y++){
            unsigned int row = 0;
            row = player_positions[y - 1];
            row |= player_positions[y + 1];
            row |= player_positions[y] << 1;
            row |= player_positions[y] >> 1;
            row &= ~b.board[y];
            new_positions[y] = row;
            
        }
        memcpy(player_positions, new_positions, sizeof(new_positions));
        for(int y = 1; y < 21; y++){
            b.board[y] |= player_positions[y];
            player_score += __builtin_popcount(player_positions[y]);
        }
        if(player_score == old_score){
            break;
        }
    }  
    return player_score;
}

inline int choose(game b, int player_idx){
    double best_score = -1110000000;
    int best_action = 0;
    for(int i = 0; i < 4; i++){
        double new_score = score(b, player_idx, i);
        if (new_score > best_score){
            best_score = new_score;
            best_action = i;
        }
    }
    return best_action;
}

inline bool valid(game input, int player_index, int action){
    return !(
        input.board[
            actiony(input.Y[player_index], action)
        ] 
        & 
        1 << actionx(input.X[player_index], action)
    );
}

void act(game& input, int player_index, int action){
    if (input.X[player_index] == 0){
        return;
    }
    if (valid(input, player_index, action)){
        input.X[player_index] = actionx(input.X[player_index], action);
        input.Y[player_index] = actiony(input.Y[player_index], action);
        input.board[input.Y[player_index]] |= 1 << input.X[player_index];
    } else {
        input.X[player_index] = 0;
        input.Y[player_index] = 0;
    }
}

double score_recursive(game &b, int player_idx, int recursions){
    if (recursions < 1){

        double best_score = 0;
        for(int i = 0; i < 4; i++){
          double new_score = score(b, player_idx, i);
          if (new_score > best_score){
            best_score = new_score;
          }
        }
        return best_score;
    }
    double best_score = -100000;
    
    int valid_options = 0;
    for(int canidate_action = 0; canidate_action < 4; canidate_action++){
        valid_options += valid(b, player_idx, canidate_action);
    }
    for(int canidate_action = 0; canidate_action < 4; canidate_action++){
        
        if(!valid(b, player_idx, canidate_action)){
            continue;
        }
        game next = b;
        act(next, player_idx, canidate_action);

        for(int enemy_idx = (player_idx + 1) % b.N; 
            enemy_idx != player_idx;
            enemy_idx = (enemy_idx + 1) % b.N) 
        { 

            act(next, enemy_idx, choose(next, enemy_idx));
        }

        double new_score = 1 + score_recursive(next, player_idx, recursions / valid_options - 1);
        best_score = max(best_score, new_score);
    }
    return best_score;
}

std::string x[4] = {"LEFT", "RIGHT", "UP", "DOWN"};

int choose_recursive(game b, int player_idx){
    double best_score = -1110000;
    int best_action = 0;
    for(int i = 0; i < 4; i++){
        if(!valid(b, player_idx, i)){
            continue;
        }
        
        game test = b;
        act(test, player_idx, i);
        for(int enemy_idx = (player_idx + 1) % b.N; 
            enemy_idx != player_idx;
            enemy_idx = (enemy_idx + 1) % b.N) 
        { 
            act(test, enemy_idx, choose(test, enemy_idx));
        }
        
        double new_score = score_recursive(test, player_idx, 40);
        //std::cerr << x[i] << ":" << new_score << std::endl;
        if (new_score > best_score){
            best_score = new_score;
            best_action = i;
        }
    }
    std::cerr << best_score << std::endl;
    return best_action;
}
        
int main()
{  
    game thegame = newgame();
    print(thegame);

    while (1) {

        cin >>thegame.N >> thegame.P; cin.ignore();
        for (int i = 0; i < thegame.N; i++) {
            
            cin >> x0 >> y0 >> thegame.X[i] >> thegame.Y[i] ; cin.ignore();
            thegame.X[i] += 1;
            thegame.Y[i] += 1;
            
            thegame.board[thegame.Y[i]] |= 1 << thegame.X[i];
            thegame.board[y0 + 1] |= 1 << (x0 + 1);
        }
        num_evals = 0;
        auto start = high_resolution_clock::now(); 
        int action = choose_recursive(thegame, thegame.P);
        auto stop = high_resolution_clock::now();
        
        auto duration = duration_cast<microseconds>(stop - start);
        
        
        std::cerr << "time" << duration.count() << std::endl;
        std::cerr << "eval calls: " << num_evals << std::endl;
        
        
        cout << x[action] << endl; // A single line with UP, DOWN, LEFT or RIGHT
    }
}
