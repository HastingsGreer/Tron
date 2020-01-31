import sys
import math

from scipy.ndimage.measurements import label


# Auto-generated code below aims at helping you parse
# the standard input according to the problem statement.


# game loop

import numpy as np

board = np.ones((22, 32))
boards = -1 * np.ones((22, 32))
board[0] = 0
board[-1] = 0
board[:, 0] = 0
board[:, -1] = 0


actions = {(-1, 0): "LEFT", (1, 0): "RIGHT", (0, -1): "UP", (0, 1): "DOWN"}
import time

po_actions = list(actions.keys())
import random

target = 0


while True:
    tnow = time.time()
    # n: total number of players (2 to 4).
    # p: your player number (0 to 3).
    n, p = [int(i) for i in input().split()]
    if p == target:
        target = (target + 1) % n
    
    enemx = []
    enemy = []
    for i in range(n):
        # x0: starting X coordinate of lightcycle (or -1)
        # y0: starting Y coordinate of lightcycle (or -1)
        # x1: starting X coordinate of lightcycle (can be the same as X0 if you play before this player)
        # y1: starting Y coordinate of lightcycle (can be the same as Y0 if you play before this player)
        x0, y0, x1, y1 = [int(j) + 1 for j in input().split()]
        
        board[y1, x1] = 0
        board[y0, x0] = 0
        
        boards[y1, x1] = i
        
        if x1 == 0:
           board[boards == i] = 1
           boards[boards == i] = -1
           if i == target:
               target = (target + 1) % n
        elif i != p:
            enemx.append(x1)
            enemy.append(y1)
        
           
        
        if i == p:
            mx, my = x1, y1
        else:
            if i == target:
                tx, ty = x1, y1
    
    #labelled, ncomponents = label(board, np.array([[0, 1, 0], [1, 1, 1], [0, 1, 0]]))
    def score(key):
        pos = key[1] + my, key[0] + mx
        board2 = np.copy(board)
        if board[pos] == 0:
            return -10000000000
        board2[pos] = 0
        
        labelled, ncomponents = label(board2, np.array([[0, 1, 0], [1, 1, 1], [0, 1, 0]]))
        
        enemy_values = [
            labelled[ty + action[1], tx + action[0]] 
            and np.sum(labelled[ty + action[1], tx + action[0]] == labelled)
            for action in po_actions]
        
        for ex, ey in zip(enemx, enemy):
            for action in po_actions:
                board2[ey + action[1], ex + action[0]] = 0
                
        labelled, ncomponents = label(board2, np.array([[0, 1, 0], [1, 1, 1], [0, 1, 0]]))
        
        
        values = [
            labelled[pos[0] + action[1], pos[1] + action[0]] 
            and np.sum(labelled[pos[0] + action[1], pos[1] + action[0]] == labelled)
            for action in po_actions]
        #print(values, file=sys.stderr)
        
        
        return np.max(values) - np.max(enemy_values) - .0001 * ((mx + key[0] - tx)**2 + (my + key[1] - ty)**2)
    def score2(key):
        pos = key[1] + my, key[0] + mx
        if board[pos] == 0:
            return -10000000000
        board2 = np.copy(board)
        
        player_score = 0
        player_positions = [(key[1] + my, key[0] + mx)]
        enemy_positions = zip(enemy, enemx)
        for _ in range(100):
            new_player_positions = []
            for old_p_y, old_p_x in player_positions:
                for dy, dx in po_actions:
                    if board2[old_p_y + dy, old_p_x + dx] == 1:
                        player_score += 1
                        board2[old_p_y + dy, old_p_x + dx] = 0
                        new_player_positions.append((old_p_y + dy, old_p_x + dx))
                    
            player_positions = new_player_positions
            
            new_enemy_positions = []
            for old_p_y, old_p_x in enemy_positions:
                for dy, dx in po_actions:
                    if board2[old_p_y + dy, old_p_x + dx] == 1:
                        
                        board2[old_p_y + dy, old_p_x + dx] = 0
                        new_enemy_positions.append((old_p_y + dy, old_p_x + dx))
                    
            enemy_positions = new_enemy_positions
            #print(board2, file=sys.stderr)
        return player_score
        
        
        
        
   
    if random.random() < .01:
        random.shuffle(po_actions)
    action = max(po_actions, key=score)
    
    
    print(tx, ty, file=sys.stderr)

    print(time.time() - tnow, file=sys.stderr)
    
    print (actions[action])
