#include "Renderer.hpp"
#include "Main.hpp"

#define MaxTextureCount 256 //! Increase if necessary
#define PI 3.1415926

GLuint g_textureIDs[MaxTextureCount];

Texture g_loadedTextures[MaxTextureCount];

int g_texturesLoadedCurrently;

TextureHandle LoadRGBA5551Texture (const uint16_t* data, int width, int height, bool smoothing, int frameW, int frameH)
{
    //! note: width&height must be powers of 2
    TextureHandle handle = g_texturesLoadedCurrently++;
    //! load the texture itself
    glBindTexture (GL_TEXTURE_2D, g_textureIDs[handle]);
    g_loadedTextures[handle].m_textureID = g_textureIDs[handle];
    g_loadedTextures[handle].m_frameWidth = frameW ? frameW : width;
    g_loadedTextures[handle].m_frameHeight= frameH ? frameH : height;
    g_loadedTextures[handle].m_smoothing = smoothing;
    g_loadedTextures[handle].m_textureWidth = width;
    g_loadedTextures[handle].m_textureHeight= height;
    g_loadedTextures[handle].m_textureBuffer = data;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, smoothing ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, smoothing ? GL_LINEAR : GL_NEAREST);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB5_A1, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, data);
    glBindTexture (GL_TEXTURE_2D, 0);

    return handle;
}
int GetTextureWidth(TextureHandle handle) {
    return g_loadedTextures[handle].m_textureWidth;
}
int GetTextureHeight(TextureHandle handle) {
    return g_loadedTextures[handle].m_textureHeight;
}
int GetFrameWidth(TextureHandle handle) {
    return g_loadedTextures[handle].m_frameWidth;
}
int GetFrameHeight(TextureHandle handle) {
    return g_loadedTextures[handle].m_frameHeight;
}

void InitializeTextureTable ()
{
    glGenTextures (MaxTextureCount, &g_textureIDs[0]);
}

void PrepareGL()
{
    glClearColor(0.1f, 0.4f, 0.6f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //match projection to window resolution (could be in reshape callback)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, GetScreenSizeX(), GetScreenSizeY(), 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glLoadIdentity();
    glDisable(GL_LIGHTING);

    glColor3f(1,1,1);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );
}

void FinishGL ()
{
    glDisable( GL_BLEND );
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void DrawLLTexture (TextureHandle handle, int x, int y, int w, int h, int tW, int tH, uint32_t color, int sX, int sY, int sW, int sH, float rot, bool flipX, bool flipY)
{
    int x2 = x + w;
    int y2 = y + w;

    //! Setup texture coords
    float u1 = sX, v1 = sY, u2 = sX + (sW == -1 ? tW : sW), v2 = sY + (sH == -1 ? tH : sH);

    //! Setup vertex coords
    int vx[4], vy[4];
    vx[0]=x;
    vy[0]=y;
    vx[1]=x;
    vy[1]=y2;
    vx[2]=x2;
    vy[2]=y2;
    vx[3]=x2;
    vy[3]=y;

    //! Append the rotation, I suppose
    if (rot != 0.0f)
    {
        float ang[4];
        for (float i = 0, j = PI/2; i < 4; i += 1, j += PI/2)
            ang[(int)i] = rot + j;

        //! Done setting up angles, now rotate
        int whalf = w/2, hhalf = h/2;
        int cx = x+whalf, cy = y+hhalf;
        for (int i = 0; i < 4; i++)
            vx[i] = whalf * cosf (ang[i]) + cx, vy[i] = hhalf * sinf (ang[i]) + cy;
    }

    if (flipX) std::swap(u1, u2);
    if (flipY) std::swap(v1, v2);

    //! Setup texture UV coords
    float tx[4],ty[4];
    tx[0]=u1/tW,ty[0]=v1/tH;
    tx[1]=u1/tW,ty[1]=v2/tH;
    tx[2]=u2/tW,ty[2]=v2/tH;
    tx[3]=u2/tW,ty[3]=v1/tH;

    //! Setup color
    Color col;
    col.rgba = color;
    glColor4ub ((GLubyte)col.r, (GLubyte)col.g, (GLubyte)col.b, (GLubyte)col.a);
    //! Load texture
    glBindTexture(GL_TEXTURE_2D, g_loadedTextures[handle].m_textureID);
    glEnable (GL_TEXTURE_2D);
    //! Actually draw the quad itself
    glBegin(GL_QUADS);
    //! Each of the vertices
    for (int i=0; i<4; i++)
    {
        glTexCoord2f(tx[i],ty[i]);
        glVertex2i (vx[i], vy[i]);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void DrawTexture (TextureHandle handle, int x, int y, int w, int h, int frameX, int frameY, uint32_t color, float rot, bool flipX, bool flipY)
{
    if (w == -1) w = g_loadedTextures[handle].m_textureWidth;
    if (h == -1) h = g_loadedTextures[handle].m_textureHeight;

    int framepX = frameX * g_loadedTextures[handle].m_frameWidth;
    int framepY = frameY * g_loadedTextures[handle].m_frameHeight;

    DrawLLTexture (handle, x, y,
                   w, h,
                   g_loadedTextures[handle].m_textureWidth,
                   g_loadedTextures[handle].m_textureHeight,
                   color,
                   framepX,
                   framepY,
                   g_loadedTextures[handle].m_frameWidth,
                   g_loadedTextures[handle].m_frameHeight,
                   rot, flipX, flipY);
}

void DrawQuad (int x, int y, int w, int h, Color* colors)
{
    int x2 = x + w;
    int y2 = y + w;
    float vx[4], vy[4];
    vx[0]=x;
    vy[0]=y;
    vx[1]=x;
    vy[1]=y2;
    vx[2]=x2;
    vy[2]=y2;
    vx[3]=x2;
    vy[3]=y;
    glBegin(GL_QUADS);
    for (int i=0; i<4; i++)
    {
        glColor3ub((GLubyte)colors[i].r,(GLubyte)colors[i].g,(GLubyte)colors[i].b);
        glVertex2f (vx[i], vy[i]);
    }
    glEnd();
}
