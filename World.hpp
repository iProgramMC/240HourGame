#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>

#define VISUAL_XY_TO_INDEX(x, y) 16 * y + x
#define GET_VISUAL_X(index) index % 16
#define GET_VISUAL_Y(index) index / 16

typedef struct rect {
    int x, y, w, h;
} Rect;
typedef struct vec2 {
    float x, y;
} Vector2;
#define C_WORLDWIDTH 200
#define C_WORLDHEIGHT 12

extern const char* g_defaultWorld;
int CharToTileID(char tileID);

struct Tile {
    int m_ID;
    int m_x, m_y;
};

class World
{
    public:
        World();
        virtual ~World();

        int GetVisual(int x, int y);
        void AddTilesInRect(Rect, std::vector<Tile*>&);
        void ClearVectorAndAddCollidableTilesInRect(Rect, std::vector<Tile*>&);
        Tile* GetTile(int x, int y);
        int IsTileCollidable (int x, int y);

    //protected:
        Tile* m_tiles;
        int m_worldWidth;
        int m_worldHeight;
};

extern World* g_world;
World* GetWorld();

#endif // WORLD_HPP
