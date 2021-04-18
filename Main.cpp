
#define WINVER 0x0501
#include <Windows.h>
#include <WinUser.h>
#include <WindowsX.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

#include "Sprite.h"
#include "Renderer.hpp"
#include "Game.hpp"

uint8_t g_checker[8 * 8];
//GLuint g_checkerTextureID = 0;

//GLuint g_textureIDs[TextureCount];
//int g_currentID;
bool g_isMinimized = false;

void LogMsg (const char* fmt, ...)
{
    char buffer[4096];
    va_list arg;
    va_start(arg,fmt);
    vsprintf(buffer,fmt,arg);
    printf("%s\n",buffer);
    va_end(arg);
}

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

int g_screenSizeX = 1024;
//int g_screenSizeX = 1280;
int g_screenSizeY = 768;
//int g_screenSizeY = 960;

int GetScreenSizeX()
{
    return g_screenSizeX;
}
int GetScreenSizeY()
{
    return g_screenSizeY;
}

int g_mouseX = 0, g_mouseY = 0;
bool g_isMouseDown = false;

int GetMouseX()
{
    return g_mouseX;
}
int GetMouseY()
{
    return g_mouseY;
}
bool IsMouseDown()
{
    return g_isMouseDown;
}

//! shamelessly stolen from Proton SDK. You'll see more of this I guess
void CenterWindow(HWND hWnd)
{
    RECT r, desk;
    GetWindowRect(hWnd, &r);
    GetWindowRect(GetDesktopWindow(), &desk);

    int wa,ha,wb,hb;

    wa = (r.right - r.left) / 2;
    ha = (r.bottom - r.top) / 2;

    wb = (desk.right - desk.left) / 2;
    hb = (desk.bottom - desk.top) / 2;

    SetWindowPos(hWnd, NULL, wb - wa, hb - ha, r.right - r.left, r.bottom - r.top, 0);
}

int MapToGLCoords(int y)
{
    return g_screenSizeY - y;
}

int g_lastTick = 0, g_thisTick = 0;
int GetTick()
{
    // on windows
    return (int)GetTickCount();
}
int g_lastUpdatedFPSAtTick = 0, g_fps = 0, g_fpsCounting = 0;

void UpdateFPSCounter () {
    if (g_lastUpdatedFPSAtTick == 0) g_lastUpdatedFPSAtTick = GetTick();
    if (GetTick() > g_lastUpdatedFPSAtTick + 1000) {
        g_lastUpdatedFPSAtTick += 1000;
        g_fps = g_fpsCounting;
        g_fpsCounting = 0;
        LogMsg("fps: %d", g_fps);
    }
    g_fpsCounting++;
}

float GetElapsedTime()
{
    float f = (g_thisTick - g_lastTick) / 1000.f;
    return f>.5f?0.f:f;
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    RECT wr = { 0, 0, g_screenSizeX, g_screenSizeY };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);
    int w = wr.right-wr.left;
    int h = wr.bottom-wr.top;

    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "My Game",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          w,
                          h,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);
    CenterWindow(hwnd);

    GetGame()->Init();
    memset (GetGame()->m_controllerState, 0, CONTROLS_COUNT);
    memset (GetGame()->m_lastControllerState, 0, CONTROLS_COUNT);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    /// load content

    //create test checker image
    //for (int i = 0; i < 64; ++i)
    //    g_checker[i] = ((i + (i / 8)) % 2) * 128 + 127;

    InitializeTextureTable();

    GetGame()->LoadTextures();

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */
            g_lastTick = g_thisTick;
            g_thisTick = GetTick();

            // get controller inputs
            const char* v_keys = CONTROL_SCHEME;
            for (int i = 0; i < 5; i++) {
                GetGame()->m_controllerState[i] = GetAsyncKeyState(v_keys[i]) & 0x8000;
            }

            GetGame()->OnUpdate();
            UpdateFPSCounter();
            PrepareGL();
            if (!g_isMinimized)
                GetGame()->OnRender();

            FinishGL();
            SwapBuffers(hDC);
            Sleep (1);
            memcpy (GetGame()->m_lastControllerState, GetGame()->m_controllerState, CONTROLS_COUNT);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    case WM_DESTROY:
        return 0;
    case WM_SIZE:
    {
        int width = LOWORD(lParam);  // Macro to get the low-order word.
        int height = HIWORD(lParam); // Macro to get the high-order word.

        g_screenSizeX = width;
        g_screenSizeY = height;
        glViewport(0, 0, width, height);
        glScissor(0, 0, width, height);
        glOrtho(0, width, height, 0, -1, 1);

        break;
    }

    case WM_MOUSEMOVE:
    {
        g_mouseX = GET_X_LPARAM(lParam);
        g_mouseY = GET_Y_LPARAM(lParam);
        break;
    }
    case WM_LBUTTONDOWN:
    {
        g_mouseX = GET_X_LPARAM(lParam);
        g_mouseY = GET_Y_LPARAM(lParam);

        g_isMouseDown = true;

        break;
    }
    case WM_LBUTTONUP:
    {
        g_isMouseDown = false;
        break;
    }
    /*case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
    {
        / *switch (wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            break;
        }* /
        uint8_t keycode = wParam & 0xFF;
        LogMsg ("Got window event DOWN %#x %c", uMsg, keycode);
        GetGame()->m_keysPressed[keycode] = true;
        GetGame()->m_keysJustPressed[keycode] = true;
        break;
    }
    case WM_SYSKEYUP:
    case WM_KEYUP:
    {
        / *switch (wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            break;
        }* /
        uint8_t keycode = wParam & 0xFF;
        LogMsg ("Got window event UP %#x %c", uMsg, keycode);
        GetGame()->m_keysPressed[keycode] = false;
        GetGame()->m_keysJustPressed[keycode] = false;
        break;
    }*/
    case WM_MOUSEWHEEL:
    {
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        GetGame()->m_scrollDelta += zDelta;
        break;
    }
    break;

    default:
        //LogMsg ("Got window event %#x", uMsg);
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

