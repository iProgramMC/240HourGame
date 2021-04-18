#ifndef WORLDRENDERER_HPP
#define WORLDRENDERER_HPP

#include "World.hpp"
#include "Renderer.hpp"
enum {
    TEXTURE_TILES,
    TEXTURE_PLAYER,
};

class WorldCamera
{
public:
    WorldCamera();
    ~WorldCamera();
    void RenderWithCameraAnim (TextureHandle texture, int x, int y, int frameX, int frameY, float scale = 1.0f, float rot = 0.0f, uint32_t color = 0xFFFFFFFF, bool flipX = false, bool flipY = false);
    Vector2 WorldToScreen(Vector2 world);
    Vector2 ScreenToWorld(Vector2 screen);
    Rect GetViewRect();
    void OnUpdate();
    void SetTargetPos(Vector2 myPosition);
//private:
    Vector2 m_pos;
    float m_scale;
    Vector2 m_view;
    Vector2 m_targetPos;
};

class WorldRenderer
{
    public:
        WorldRenderer();
        virtual ~WorldRenderer();
        void SetWorld(World* world);
        void OnRender();
        void OnUpdate();

    protected:

    public:
        World* m_world;
        WorldCamera* m_camera;
};
WorldCamera* GetCamera();
WorldRenderer* GetRenderer();

#endif // WORLDRENDERER_HPP
