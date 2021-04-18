#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "WorldRenderer.hpp"
#include "Game.hpp"
#include "Main.hpp"

enum PlayerAction {
    Idle,
    Walk,
    Jump,
    Dead,
};

struct CollideInfo {
    World* m_world;
    Tile* m_tile;
    int m_newX, m_newY;
    bool m_direction;//0: goes up, 1: goes down
};

class Player
{
    public:
        Player();
        virtual ~Player();
        void Render();
        void Update();
        void Move(float moveX, float moveY);
        Rect GetCollRect();

        void UpdateIdle();
        void UpdateWalk();
        void UpdateJump();
        void UpdateDead();
        void UpdateCrouching();
        void StartWalk();
        void StartJump();
        void OnDie();
        void OnRespawn();

        bool CollideX (CollideInfo info);
        bool CollideY (CollideInfo info);


    protected:

    private:
        Vector2 m_pos;
        Vector2 m_vel;
        Vector2 m_collSize;
        bool m_onGround;
        bool m_jumpKey;
        bool m_crouching;
        bool m_facesLeft;
        int m_frameX, m_frameY;
        float m_walkTimer;
        PlayerAction m_action;
        float m_respawnTimer = 0;
        int m_spawnPointX, m_spawnPointY;
        float m_subPixelMemoryX = 0, m_subPixelMemoryY = 0;
};

Player* GetPlayer();

#endif // PLAYER_HPP
