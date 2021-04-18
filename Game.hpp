#ifndef GAME_HPP_INCLUDED
#define GAME_HPP_INCLUDED

#define CONTROL_SCHEME "ADWFS"
#define CONTROLS_COUNT 5

enum {
    KEY_LEFT,
    KEY_RIGHT,
    KEY_JUMP,
    KEY_FIRE,
    KEY_CROUCH,
};

class Game
{
public:
    int m_scrollDelta = 0;

    bool m_controllerState[CONTROLS_COUNT];
    bool m_lastControllerState[CONTROLS_COUNT];

    void OnRender();
    void OnUpdate();
    void Init();
    void LoadTextures();
};

extern Game* g_game;
Game* GetGame();

#endif // GAME_HPP_INCLUDED
