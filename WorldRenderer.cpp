#include "WorldRenderer.hpp"


WorldCamera::WorldCamera()
{
    m_pos.x = 0;
    m_pos.y = 0;
    m_scale = 2;
    m_view.x = GetScreenSizeX() / m_scale;
    m_view.y = GetScreenSizeY() / m_scale;
    m_targetPos.x = -222;
}
void WorldCamera::OnUpdate()
{
    m_view.x = GetScreenSizeX() / m_scale;
    m_view.y = GetScreenSizeY() / m_scale;

    // set the target position
    if (m_targetPos.x != -222) {
        m_pos = {
            m_targetPos.x - m_view.x / 2,
            m_targetPos.y - m_view.y / 2,
        };
        m_targetPos.x = -222;
    }
    if (m_pos.x < 0) m_pos.x = 0;
    if (m_pos.y < 0) m_pos.y = 0;
    if (m_pos.x > GetWorld()->m_worldWidth *32-m_view.x) m_pos.x = GetWorld()->m_worldWidth *32-m_view.x;
    if (m_pos.y > GetWorld()->m_worldHeight*32-m_view.y) m_pos.y = GetWorld()->m_worldHeight*32-m_view.y;
}
void WorldCamera::SetTargetPos(Vector2 myPosition) {
    m_targetPos = myPosition;
}
WorldCamera::~WorldCamera()
{
}
Vector2 WorldCamera::WorldToScreen(Vector2 world)
{
    float x = world.x, y = world.y;
    x -= m_pos.x, y -= m_pos.y;
    x *= m_scale, y *= m_scale;
    return { x, y };
}
Vector2 WorldCamera::ScreenToWorld(Vector2 screen)
{
    float x = screen.x, y = screen.y;
    x /= m_scale, y /= m_scale;
    x += m_pos.x, y += m_pos.y;
    return { x, y };
}
Rect WorldCamera::GetViewRect()
{
    return { (int)m_pos.x, (int)m_pos.y, (int)m_view.x, (int)m_view.y };
}

void WorldCamera::RenderWithCameraAnim(TextureHandle tex, int x, int y, int frameX, int frameY, float scale, float rot, uint32_t color, bool flipX, bool flipY)
{
    Vector2 screen = WorldToScreen ({(float)x, (float)y});
    scale *= m_scale;
    int the_width, the_height;

    if (GetFrameWidth(tex))
        the_width = GetFrameWidth(tex);
    else
        the_height = GetTextureWidth(tex);

    if (GetFrameHeight(tex))
        the_width = GetFrameHeight(tex);
    else
        the_height = GetTextureHeight(tex);

    the_width = (int)ceilf((float)the_width * scale);
    the_height = (int)ceilf((float)the_height * scale);

    DrawTexture(tex, (int)screen.x, (int)screen.y, the_width, the_height, frameX, frameY, color, rot, flipX, flipY);
}


WorldRenderer::WorldRenderer()
{
    m_camera = new WorldCamera();
    m_world = nullptr;
}

WorldRenderer::~WorldRenderer()
{
    delete m_camera;
    m_world = nullptr;
}

void WorldRenderer::SetWorld (World* world)
{
    m_world = world;
}

void WorldRenderer::OnUpdate()
{
    if (!m_world) return;
    m_camera->OnUpdate();
}
void WorldRenderer::OnRender()
{
    if (!m_world) return;
    Rect view = m_camera->GetViewRect();
    std::vector<Tile*> drawn_tiles;
    m_world->AddTilesInRect(view, drawn_tiles);

    for (int i = 0; i < drawn_tiles.size(); i++) {
        if (drawn_tiles[i]->m_ID != 0) {
            int vis = m_world->GetVisual(drawn_tiles[i]->m_x, drawn_tiles[i]->m_y);
            if (vis != -1) {
                if (drawn_tiles[i]->m_ID == 4) {
                    m_camera->RenderWithCameraAnim(TEXTURE_TILES, drawn_tiles[i]->m_x * 32, drawn_tiles[i]->m_y * 32 + 16, GET_VISUAL_X(vis), GET_VISUAL_Y(vis), .5f);
                    m_camera->RenderWithCameraAnim(TEXTURE_TILES, drawn_tiles[i]->m_x * 32 + 16, drawn_tiles[i]->m_y * 32 + 16, GET_VISUAL_X(vis), GET_VISUAL_Y(vis), .5f);
                }
                else
                    m_camera->RenderWithCameraAnim(TEXTURE_TILES, drawn_tiles[i]->m_x * 32, drawn_tiles[i]->m_y * 32, GET_VISUAL_X(vis), GET_VISUAL_Y(vis));
            }
        }
    }
}


WorldRenderer* g_renderer = nullptr;
WorldRenderer* GetRenderer() {
    if (!g_renderer) {
        g_renderer = new WorldRenderer();
    }
    return g_renderer;
}

WorldCamera* GetCamera() {
    return GetRenderer()->m_camera;
}
