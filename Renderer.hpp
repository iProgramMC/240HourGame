#ifndef RENDERER_HPP_INCLUDED
#define RENDERER_HPP_INCLUDED

#include <stdint.h>
#include <math.h>
#include <algorithm>
#include <GL/gl.h>
#include <GL/glext.h>
struct Color
{
    union
    {
        uint32_t rgba;
        uint8_t as_bytes[4];
        struct
        {
            uint8_t a,b,g,r;
        };
    };
};
struct Point
{
    int x, y;
};
int GetScreenSizeX();
int GetScreenSizeY();
struct Texture
{
    GLuint m_textureID;
    int m_textureWidth;
    int m_textureHeight;
    //! format is RGBA5551
    const uint16_t* m_textureBuffer;
    bool m_smoothing;
    int m_frameWidth, m_frameHeight;
};

typedef int TextureHandle; //! index in this array

int GetTextureWidth(TextureHandle handle);
int GetTextureHeight(TextureHandle handle);
int GetFrameWidth(TextureHandle handle);
int GetFrameHeight(TextureHandle handle);

TextureHandle LoadRGBA5551Texture (const uint16_t* data, int width, int height, bool smoothing = false, int frameW = 0, int frameH = 0);
void InitializeTextureTable ();
void PrepareGL();
void FinishGL();
void DrawLLTexture (TextureHandle handle, int x, int y, int w, int h, int tW, int tH, uint32_t color = 0xFFFFFFFF, int sX = 0, int sY = 0, int sW = -1, int sH = -1, float rot = 0.0f, bool flipX = false, bool flipY = false);
void DrawTexture (TextureHandle handle, int x, int y, int w = -1, int h = -1, int frameX = 0, int frameY = 0, uint32_t color = 0xFFFFFFFF, float rot = 0.0f, bool flipX = false, bool flipY = false);

#endif // RENDERER_HPP_INCLUDED
