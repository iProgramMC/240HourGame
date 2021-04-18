#ifndef MAIN_HPP_INCLUDED
#define MAIN_HPP_INCLUDED


extern const uint16_t* g_textures[];
int GetScreenSizeX();
int GetScreenSizeY();

int GetMouseX();
int GetMouseY();
bool IsMouseDown();
int GetTick();
float GetElapsedTime();
void LogMsg(const char* fmt, ...);

#endif // MAIN_HPP_INCLUDED
