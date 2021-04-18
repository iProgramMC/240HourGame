#include <stdio.h>
#include "Game.hpp"
#include "Renderer.hpp"
#include "Player.hpp"
#include "Main.hpp"
#include "WorldRenderer.hpp"
#define TextureCount 2

int g_currentID;
int g_texturebr = 0;
Game* g_game;
float g_theta = 0.f;


Game* GetGame()
{
    if (!g_game)
    {
        g_game = new Game();
    }
    return g_game;
}

void Game::OnRender()
{
    GetRenderer()->OnRender();
    GetPlayer()->Render();
    DrawTexture(g_currentID, GetMouseX()-32, GetMouseY()-32, 64, 64, g_texturebr%16, g_texturebr/16, 0xffffffff, g_theta);//, colors[g_isMouseDown]);
}

void Game::OnUpdate()
{
    if (m_scrollDelta)
    {
        g_currentID = 0;
        int k = m_scrollDelta / 120; // one notch
        g_texturebr += k;
        while (g_texturebr < 0) g_texturebr += 256;
        while (g_texturebr > 256) g_texturebr -= 256;

        m_scrollDelta = 0;
    }
    g_theta += GetElapsedTime();
    float scale = GetScreenSizeY() / (32.f*C_WORLDHEIGHT);
    GetCamera()->m_scale = scale;
    GetPlayer()->Update();
    GetRenderer()->OnUpdate();
}

void Game::LoadTextures()
{
    for(int i=0; i<TextureCount; i++)
    {
        LoadRGBA5551Texture(g_textures[i], 512/(i+1),512/(i+1),false, 32, 32);
    }
}

void Game::Init()
{
    GetRenderer()->m_world = GetWorld();
    GetPlayer();
}
