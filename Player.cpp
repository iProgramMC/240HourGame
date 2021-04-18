#include "Player.hpp"

float GetElapsedTime();
Player::Player()
{
    m_collSize = { 20, 30 };
    m_pos = { 72, 0 };
    m_vel = { 0, 0 };
    m_onGround = false;
    m_jumpKey = false;
    m_crouching = false;
    m_facesLeft = false;
    m_frameX = 0;
    m_frameY = 0;
    m_action = PlayerAction::Idle;
}

Player::~Player()
{
    //dtor
}

Rect Player::GetCollRect()
{
    return { (int)m_pos.x, (int)m_pos.y, (int)m_collSize.x, (int)m_collSize.y };
}

bool Player::CollideX (CollideInfo info)
{
    int collidable_type = info.m_world->IsTileCollidable (info.m_tile->m_x, info.m_tile->m_y);
    if (collidable_type == 2)
    {
        if (info.m_newY+m_collSize.y-1 >= info.m_tile->m_y * 32 + 16 &&
            info.m_newY+m_collSize.y-1 < (info.m_tile->m_y+1) * 32)
            return 1;
        else return 0;
    }
    else return 1;
}
bool Player::CollideY (CollideInfo info)
{
    int collidable_type = info.m_world->IsTileCollidable (info.m_tile->m_x, info.m_tile->m_y);
    if (collidable_type == 2)
    {
        if (info.m_newY+(info.m_direction?m_collSize.y-1:0) >= info.m_tile->m_y * 32 + 16 &&
            info.m_newY+(info.m_direction?m_collSize.y-1:0) < (info.m_tile->m_y+1) * 32)
            return 1;
        else return 0;
    }
    else return 1;
}

void Player::Move(float moveX, float moveY)
{
    // collision
    float movementUnit;
    std::vector<Tile*> my_vector;
    CollideInfo info;
    info.m_world = GetWorld();
    m_subPixelMemoryX += moveX;
    m_subPixelMemoryY += moveY;
    moveX = floorf(moveX);
    moveY = floorf(moveY);
    m_subPixelMemoryX -= moveX;
    m_subPixelMemoryY -= moveY;

    if (moveX != 0)
    {
        movementUnit = moveX < 0 ? -1 : 1;
        moveX = abs(moveX);

        while (moveX > 0)
        {
            float new_x = m_pos.x + movementUnit;
            // are we colliding with any tiles?
            Rect checkRect = GetCollRect();
            checkRect.x = new_x;
            GetWorld()->ClearVectorAndAddCollidableTilesInRect(checkRect, my_vector);
            if (my_vector.size() != 0)
            {
                info.m_newX = new_x;
                info.m_newY = m_pos.y;
                info.m_direction = new_x > m_pos.x;
                bool shouldBreak = false;
                for (int i = 0; i < my_vector.size(); i++)
                {
                    info.m_tile = my_vector[i];
                    if (CollideX(info))
                    {
                        shouldBreak = true;
                        break;
                    }
                }
                if (shouldBreak) break;
            }
            m_pos.x = new_x;
            moveX -= 1;
        }
    }
    if (moveY != 0)
    {
        movementUnit = moveY < 0 ? -1 : 1;
        moveY = floorf(abs(moveY));

        while (moveY > 0)
        {
            float new_y = m_pos.y + movementUnit;
            // are we colliding with any tiles?
            Rect checkRect = GetCollRect();
            checkRect.y = new_y;
            GetWorld()->ClearVectorAndAddCollidableTilesInRect(checkRect, my_vector);
            if (my_vector.size() != 0)
            {
                info.m_newX = m_pos.x;
                info.m_newY = new_y;
                info.m_direction = new_y > m_pos.y;
                bool shouldBreak = false;
                for (int i = 0; i < my_vector.size(); i++)
                {
                    info.m_tile = my_vector[i];
                    if (CollideY(info))
                    {
                        shouldBreak = true;
                        break;
                    }
                }
                if (shouldBreak)
                {
                    if (movementUnit > 0)
                        m_onGround = true;
                    break;
                }
            }
            m_pos.y = new_y;
            moveY -= 1;
        }
    }
    if (m_pos.x < 0) m_pos.x = 0;
    if (m_pos.y < 0) m_pos.y = 0;
    if (m_pos.x > GetWorld()->m_worldWidth  * 32 - m_collSize.x) m_pos.x = GetWorld()->m_worldWidth  * 32 - m_collSize.x;
    //if (m_pos.y > GetWorld()->m_worldHeight * 32 - m_collSize.y) m_pos.y = GetWorld()->m_worldHeight * 32 - m_collSize.y;
}
//! Shamelessly copied from proton
void SetFloatWithTarget(float *p_out_dest, float r_target, float r_amount)
{
    if (*p_out_dest > r_target)
    {
        //make it less
        *p_out_dest -= r_amount;
        if (*p_out_dest < r_target) *p_out_dest = r_target;

    }
    else
    {
        //make it more
        *p_out_dest += r_amount;
        if (*p_out_dest > r_target) *p_out_dest = r_target;
    }
}

void Player::OnDie()
{
    LogMsg("Player died");
    // player dies
    m_respawnTimer = 3.f;
    m_action = PlayerAction::Dead;
}
void Player::OnRespawn()
{
    LogMsg("Player respawned");
    m_pos.x = 32 * m_spawnPointX;
    m_pos.y = 32 * m_spawnPointY;
    m_action = PlayerAction::Idle;
}

void Player::Update()
{
    m_onGround = false;
    float moveX = m_vel.x * GetElapsedTime();
    float moveY = m_vel.y * GetElapsedTime();
    Move(moveX, moveY);

    if (m_pos.y >= GetWorld()->m_worldHeight * 32)
    {
        if (m_action != PlayerAction::Dead)
            OnDie();
    }

    // gravity
    if (m_onGround)
    {
        if (m_vel.y > 0) m_vel.y = 0;
    }
    else
    {
        float gravity_pull;
        if (m_vel.y < 0)
        {
            if (m_jumpKey) gravity_pull = 1000.0f;
            /*******/ else gravity_pull = 5250.0f;
        }
        else gravity_pull = 1000.0f;

        // apply gravity
        m_vel.y += gravity_pull * GetElapsedTime();
        if (m_vel.y > 400) m_vel.y = 400;
    }

    m_crouching = GetGame()->m_controllerState[KEY_CROUCH];
    // acceleration
    bool left, right;
    left = GetGame()->m_controllerState[KEY_LEFT];
    right= GetGame()->m_controllerState[KEY_RIGHT];
    if (m_crouching || m_action == PlayerAction::Dead)
        left = right = false;
    float accel = 0;
    if (left)
    {
        m_facesLeft = true;
        if (right)  accel = 0;
        else        accel = -1200.f;
    }
    else if (right)
    {
        m_facesLeft = false;
        if (left)   accel = 0;
        else        accel = +1200.f;
    }
    else accel = 0;

    if (accel != 0)
    {
        if (accel > 0)
        {
            m_vel.x += accel * GetElapsedTime();
            if (m_vel.x > 250.0f) m_vel.x = 250.0f;
        }
        else
        {
            m_vel.x += accel * GetElapsedTime();
            if (m_vel.x <-250.0f) m_vel.x =-250.0f;
        }
    }
    else
    {
        SetFloatWithTarget (&m_vel.x, 0.0f, GetElapsedTime() * 1200.f);
    }

    if (m_onGround)
    {
        if (GetGame()->m_controllerState[KEY_JUMP] && !m_jumpKey)
        {
            // jump?
            m_vel.y = -500.0f;
            m_jumpKey = true;
            m_onGround = false;
            StartJump();
        }
    }
    if (!GetGame()->m_controllerState[KEY_JUMP])
        m_jumpKey = false;

    GetCamera()->SetTargetPos( { m_pos.x + m_collSize.x / 2, m_pos.y + m_collSize.y / 2 });

    switch (m_action)
    {
    case PlayerAction::Idle:
        UpdateIdle();
        break;
    case PlayerAction::Walk:
        UpdateWalk();
        break;
    case PlayerAction::Jump:
        UpdateJump();
        break;
    case PlayerAction::Dead:
        UpdateDead();
        break;
    }
    if (m_crouching)
        UpdateCrouching();
}
void Player::Render()
{
    GetCamera()->RenderWithCameraAnim((TextureHandle)TEXTURE_PLAYER, (int)m_pos.x - 4, (int)m_pos.y - 2, m_frameX, m_frameY, 1.0f, 0.0f, 0xFFFFFFFF, m_facesLeft);
}
void Player::UpdateCrouching()
{
    m_frameX = 0;
    m_frameY = 1;
}
void Player::UpdateIdle()
{
    if (m_vel.x != 0)
    {
        StartWalk();
        return;
    }
    m_frameX = 0;
    m_frameY = 0;
}
void Player::StartJump()
{
    m_action = PlayerAction::Jump;
}
void Player::UpdateDead()
{
    m_respawnTimer -= GetElapsedTime();
    if (m_respawnTimer <= 0)
    {
        OnRespawn();
    }
}
void Player::UpdateJump()
{
    if (m_onGround)
        m_action = PlayerAction::Idle;

    m_frameX = 1;
    m_frameY = 0;
}
void Player::StartWalk()
{
    m_walkTimer = 0;
    m_action = PlayerAction::Walk;
}
#define WALK_CYCLE_TIME .25f //in seconds
#define WALK_CYCLE_FRAMES 3
void Player::UpdateWalk()
{
    if (m_vel.x == 0)
    {
        m_action = PlayerAction::Idle;
        return;
    }
    float wt = m_walkTimer;
    m_walkTimer += GetElapsedTime();
    while (m_walkTimer > WALK_CYCLE_TIME) m_walkTimer -= WALK_CYCLE_TIME;

    m_frameX = 5 + (int)((m_walkTimer / WALK_CYCLE_TIME) * WALK_CYCLE_FRAMES);
    m_frameY = 0;
}

Player* g_player;
Player* GetPlayer()
{
    if (!g_player) g_player = new Player();
    return g_player;
}
