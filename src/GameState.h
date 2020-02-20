//
// Created by Jonas Hinchely on 20/02/2020.
//

#ifndef GAME_ENGINE_GAMESTATE_H
#define GAME_ENGINE_GAMESTATE_H


class GameState {
public:
    GameState();
    ~GameState();
    Input &input;
    long delta = 0;
    bool quit = false;
    void load();
    void render();
    void update();
};


#endif //GAME_ENGINE_GAMESTATE_H
