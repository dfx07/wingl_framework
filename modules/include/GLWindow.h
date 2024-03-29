﻿////////////////////////////////////////////////////////////////////////////////////
// File: GLWindow.cpp   -- Create and handle event window                           
// Copyright (C) 2020-2021 Thuong.NV   Created : 31/08/2021                         
// For conditions of distribution and use, see copyright notice in readme.txt       
////////////////////////////////////////////////////////////////////////////////////

#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <iostream>

#include <stdlib.h>
#include <string.h>
#include <unordered_map>

#include <map>
#include <stack>


#include <chrono>

#include <windows.h>
#include "GLWinDef.h"
#include "GLCom.h"
#include "GLWinControl.h"
#include "GLWinFont.h"

#include <GL/glew.h>
#include <GL/wglew.h>

#pragma comment (lib,"Gdiplus.lib")
#pragma comment (lib,"opengl32.lib")


#define addAtribute(attribs, name, value)\
{\
    assert((size_t) attribCount < attribsize);\
    attribs[attribCount++] = name;  \
    attribs[attribCount++] = value; \
}

#define addAtributeEnd(attribs)\
{\
    assert((size_t) attribCount < attribsize); \
    attribs[attribCount++] = 0;  \
}

//Không sử dụng Graphics vì nó bị đỏ khi Bug khó chịu @^@
typedef Gdiplus::Graphics                   WndGDIplus;


class GLWindow;


enum class FontWeight
{
    Normal,
    Bold  ,
    Thin  ,
    Light ,
};

struct WndProp
{
    bool                m_bFullScreen;
    bool                m_bGDIplus;
    bool                m_bOpenGL;       // State OpenGL
    int                 m_iAntialiasing; // Antialiasing level = 8


    DWORD               m_dwExStyle;     // Window Extended Style
    DWORD               m_dwStyle;       // Window Style


    WndProp()
    {
        m_bFullScreen   = false;
        m_bGDIplus      = false;
        m_bOpenGL       = false;
        m_iAntialiasing = -1;
    }

    WndProp(const char* title, int xpos, int ypos, int width = 640, int height= 480)
    {
        m_bFullScreen   = false;
        m_bGDIplus      = false;
        m_bOpenGL       = true;
        m_iAntialiasing = -1;
    }
};

struct WndStatus
{
    std::string         m_title;
    int                 m_x;
    int                 m_y;
    int                 m_width;
    int                 m_height;
    int                 m_RelState;

    WndStatus()
    {
        m_title     = "None";
        m_x         = 0;
        m_y         = 0;
        m_width     = 640;
        m_height    = 480;
        m_RelState  = 0;    // Trạng thái mặc định
    }
};

//==================================================================================
// Class Window : Thông tin và ngữ cảnh của một handle                              
//==================================================================================
class Window
{
private:
    std::stack<WndProp>    m_PropStack;
    std::stack<WndStatus>  m_StatusStack;
    GLWindow*              m_pGLWin;

    vector<Control*>       m_controls;

private:
    HWND                   m_hWnd;
    WndProp                m_pProp;
    WndRender              m_pRender;
    GDIplusToken           m_gdiToken;
    std::map<int, bool>    m_mouse;
    std::map<int, bool>    m_keyboard;


    short                  m_zDeltaScroll;

private:
    // Window property
    std::string            m_title;
    int                    m_x;
    int                    m_y;
    int                    m_width;     // The drawable width
    int                    m_height;    // The drawable height

    // State information
    bool                   m_bShow;

    // System information : fps -frametime
    FPSCounter             m_fpscounter;
    Timer                  m_update_timer;

    // Text render 2D
    GLWinFontRender        m_text_render;
    const char*            m_fontNameTextRender;
    unsigned int           m_fontSizeTextRender;
    bool                   m_bSysInfo;


    // Font information
    const char*            m_fontName;
    unsigned int           m_fontSize;
    FontWeight             m_fontWeight;
private:
    void(*m_funOnDraw)         (Window* win) = NULL;
    void(*m_funOnCreated)      (Window* win) = NULL;
    void(*m_funOnDestroy)      (Window* win) = NULL;
    void(*m_funOnPaint)        (Window* win) = NULL;
    void(*m_funOnMouse)        (Window* win) = NULL;
    void(*m_funOnMouseRealt)   (Window* win) = NULL;
    void(*m_funOnMouseMove)    (Window* win) = NULL;
    void(*m_funOnKeyboard)     (Window* win) = NULL;
    void(*m_funOnProcess)      (Window* win) = NULL;
    void(*m_funOnResize)       (Window* win) = NULL;
    void(*m_funOnMouseScroll)  (Window* win) = NULL;

public:
    HWND GetHwnd()                                              { return m_hWnd; }
    HDC  GetHDC()                                               { return m_pRender.m_hDc;}
    // Get set don't care
    void SetOnDrawfunc(void(*funOnDraw)(Window*))               { m_funOnDraw = funOnDraw; }
    void SetOnCreatedfunc(void(*funOnCreate)(Window*))          { m_funOnCreated = funOnCreate; }
    void SetOnDestroyfunc(void(*funOnDestroy)(Window*))         { m_funOnDestroy = funOnDestroy; }
    void SetOnPaintfunc(void(*funOnPaint)(Window*))             { m_funOnPaint = funOnPaint; }
    void SetOnMouseButtonfunc(void(*funOnMouse)(Window*))       { m_funOnMouse = funOnMouse; }
    void SetOnMouseButtonRealtfunc(void(*funOnMouse)(Window*))  { m_funOnMouseRealt = funOnMouse; }
    void SetOnMouseMovefunc(void(*funOnMouseMove)(Window*))     { m_funOnMouseMove = funOnMouseMove; }
    void SetOnKeyboardfunc(void(*funOnKeyboard)(Window*))       { m_funOnKeyboard = funOnKeyboard; }
    void SetProcessfunc(void(*funProcess)(Window*))             { m_funOnProcess = funProcess; }
    void SetOnResizefunc(void(*funOnResize)(Window*))           { m_funOnResize = funOnResize; }
    void SetOnMouseScrollfunc(void(*funOnScroll)(Window*))      { m_funOnMouseScroll = funOnScroll; }

    //==================================================================================
    //⮟⮟ Triển khai hàm   - not important                                              
    //==================================================================================
    virtual void OnCreated()
    {
        if (m_funOnCreated)
        {
            this->m_funOnCreated(this);
        }
    }
    virtual void OnCommand(int type)
    {

    }

    virtual void OnKeyBoard()
    {
        if (m_funOnKeyboard)
        {
            this->m_funOnKeyboard(this);
        }
    }
    virtual void OnMouseButton()
    {
        if (m_funOnMouse)
        {
            this->m_funOnMouse(this);
        }
    }
    virtual void OnMouseMove()
    {
        if (m_funOnMouseMove)
        {
            this->m_funOnMouseMove(this);
        }
    }
    virtual void OnProcess()
    {
        if (m_funOnMouseRealt)
        {
            this->m_funOnMouseRealt(this);
        }

        if (m_funOnProcess)
        {
            this->m_funOnProcess(this);
        }
    }
    virtual void OnResize()
    {
        if (m_funOnResize)
        {
            this->m_funOnResize(this);
        }
    }
    virtual void OnMouseScroll()
    {
        if (m_funOnMouseScroll)
        {
            this->m_funOnMouseScroll(this);
        }
    }
    virtual void SwapBuffer()
    {
        SwapBuffers(m_pRender.m_hDc);
    }

    virtual void OnPaint()
    {
        if (m_funOnPaint)
        {
            this->m_funOnPaint(this);
        }
    }
    //==================================================================================
    //⮝⮝ Triển khai hàm  - not important                                               
    //==================================================================================

    virtual void OnDraw()
    {
        //Xử lý mặc định và hiển thị Fps - Cập nhật title sau từng giây
        //if (m_update_timer.GetTimeSecond() >= 1.0f)
        //{
        //    m_update_timer.Reset();

        //    string title = this->m_title;
        //    title.append(" - " + std::to_string(this->GetWidth()) + "x"
        //        + std::to_string(this->GetHeight()));
        //    title.append(", FPS: " + std::to_string(m_fpscounter.FPS()));
        //    title.append(" - " + std::string((char*)glGetString(GL_RENDERER)));

        //    this->UpdateTitle(title);
        //}

        // Thực hiện vẽ custom người dùng
        if (m_funOnDraw)
        {
            //glViewport(0, 0, m_width, m_height);
            this->m_funOnDraw(this);
        }

        // Hiển thị thông tin fps - frametime và thông tin hệ thống nếu cần
        if (m_bSysInfo == true)
        {
            char strsys[80];
            sprintf(strsys, "fps: %d - frametime : %f ms", GetFPS(), GetFrameTime());
            m_text_render.Use();
            m_text_render.Write(10, 20, strsys);

            m_text_render.DontUse();
        }
    }

private:

    void SetMouseButtonStatus(int btn, bool status)
    {
        m_mouse[btn] = status;
    }
    void SetKeyboardStatus(int key, bool status)
    {
        m_keyboard[key] = status;
    }
//======================================================================================
//⮟⮟ Triển khai window win32                                                           
//======================================================================================

private:
    static bool CALLBACK SetChildFont(HWND hwnd, LPARAM font)
    {
        HFONT hFont = (HFONT)SendMessage(hwnd, WM_GETFONT, NULL, NULL);
        //HFONT defhFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        if (!hFont)
        {
            SendMessage(hwnd, WM_SETFONT, font, true);
        }
        return TRUE;
    }
    //==================================================================================
    // Cập nhật thông font cho window                                                   
    //==================================================================================
    void UpdateFont()
    {
        if (!m_hWnd || !m_fontName) return;

        int iFontWeight = FW_NORMAL;
        switch (m_fontWeight)
        {
            case FontWeight::Bold : iFontWeight = FW_BOLD ; break;
            case FontWeight::Thin : iFontWeight = FW_THIN ; break;
            case FontWeight::Light: iFontWeight = FW_LIGHT; break;
            default:
                break;
        }
        HFONT hFont = NULL;
        hFont = CreateFont( m_fontSize, 0, 0, 0, iFontWeight, 0,
                           0, 0, 0, 0, 0, 0, 0, m_fontName);

        SendMessage(m_hWnd, WM_SETFONT, (WPARAM)hFont, FALSE);

        EnumChildWindows(m_hWnd, (WNDENUMPROC)SetChildFont, (LPARAM)hFont);
    }

public:
    void SetFont(const char* fontName, const unsigned int fontSize, const FontWeight fontWeight = FontWeight::Normal)
    {
        m_fontName   = fontName;
        m_fontSize   = fontSize;
        m_fontWeight = fontWeight;

    }

//======================================================================================
//⮟⮟ Triển khai chính khởi tạo và xử lý control                                        
//======================================================================================
private:
    //==================================================================================
    // Khởi tạo toàn bộ control đã được thêm                                            
    //==================================================================================
    virtual void OnInitControl()
    {
        // IDS bắt đầu của control nó sẽ tăng khi vào hàm onInitControl của các control
        UINT IDS = 1000; 
        for (int i = 0; i < m_controls.size(); i++)
        {
            m_controls[i]->SetParent(m_hWnd);
            m_controls[i]->OnInitControl(IDS);
        }
    }

    //==================================================================================
    // Xóa và hủy tòa bộ control có trong window                                        
    //==================================================================================
    virtual void DestroyControl()
    {
        for (int i = 0; i < m_controls.size(); i++)
        {
            m_controls[i]->OnDestroy();
            delete m_controls[i];
        }
    }

    //==================================================================================
    // Xử lý sự kiện khi một Control được kích hoạt sự kiện                             
    //==================================================================================
    void OnCommandControl(HWND hwndCtrl, WORD ID, WORD Event)
    {
        Control * control = GetControlFormID(ID);
        if (!control) return;
        control->Event(this, ID, Event);
    }

    void OnDrawControl(HWND hwndCtrl, WORD ID)
    {
        Control * control = GetControlFormID(ID);
        if (!control) return;
        control->Draw();
    }

    //==================================================================================
    // Lấy control từ ID của nó                                                         
    //==================================================================================
    Control* GetControlFormID(WORD ID)
    {
        for (int i = 0; i < m_controls.size(); i++)
        {
            INT CtrlID = m_controls[i]->GetID();

            if (CtrlID == ID || m_controls[i]->ContainID(ID))
            {
                return m_controls[i];
            }
        }
        return NULL;
    }

//======================================================================================
//⮟⮟ Triển khai chính tạo và xử lý ngữ cảnh window  - important                        
//======================================================================================
private:

    //==================================================================================
    // Lấy mảng thuộc tính thiết lập để set style window                                
    //==================================================================================
    void GetFixelFormatAttribute(int *attribs, int attribsize)
    {
        int attribCount = 0;

        addAtribute(attribs, WGL_DRAW_TO_WINDOW_ARB, GL_TRUE);
        addAtribute(attribs, WGL_SUPPORT_OPENGL_ARB, GL_TRUE);
        addAtribute(attribs, WGL_DOUBLE_BUFFER_ARB, GL_TRUE);
        addAtribute(attribs, WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB);
        addAtribute(attribs, WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB);
        addAtribute(attribs, WGL_COLOR_BITS_ARB, 32);
        addAtribute(attribs, WGL_DEPTH_BITS_ARB, 24);
        addAtribute(attribs, WGL_STENCIL_BITS_ARB, 8);

        if (m_pProp.m_iAntialiasing > 0)
        {
            addAtribute(attribs, WGL_SAMPLE_BUFFERS_ARB, GL_TRUE);          // Enable multisampling
            addAtribute(attribs, WGL_SAMPLES_ARB, m_pProp.m_iAntialiasing); // Number of samples
        }
        addAtributeEnd(attribs);
    }

    //==================================================================================
    // Khởi tạo ngữ cảnh OpenGL (OpenGL context)                                        
    // bOpenGLex : sử dụng OpenGL mở rộng                                               
    //==================================================================================
    bool CreateOpenGLContext(bool bOpenGLEx = true)
    {
        assert(m_hWnd);
        HDC   hDC = GetDC(m_hWnd);
        HGLRC hglrc = NULL;

        int iPixelFormat; unsigned int num_formats = 0;

        // Get pixel format attributes through "modern" extension
        if (bOpenGLEx)
        {
            int pixelAttribs[47];
            GetFixelFormatAttribute(pixelAttribs, sizeof(pixelAttribs) / sizeof(pixelAttribs[0]));

            wglChoosePixelFormatARB(hDC, pixelAttribs, 0, 1, &iPixelFormat, &num_formats);

            PIXELFORMATDESCRIPTOR pfd;
            DescribePixelFormat(hDC, iPixelFormat, sizeof(pfd), &pfd);
            SetPixelFormat(hDC, iPixelFormat, &pfd);

            // Specify that we want to create an OpenGL x.x core profile context
            int gl_attribs[] = {
                WGL_CONTEXT_MAJOR_VERSION_ARB, 1,
                WGL_CONTEXT_MINOR_VERSION_ARB, 5,
                WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                0,
            };
            hglrc = wglCreateContextAttribsARB(hDC, 0, gl_attribs);
        }
        // Get pixel format attributes through legacy PFDs
        else
        {
            PIXELFORMATDESCRIPTOR pfd = {
                sizeof(PIXELFORMATDESCRIPTOR),
                1,
                PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
                PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
                32,                   // Colordepth of the framebuffer.
                0, 0, 0, 0, 0, 0,
                0,
                0,
                0,
                0, 0, 0, 0,
                24,                   // Number of bits for the depthbuffer
                8,                    // Number of bits for the stencilbuffer
                0,                    // Number of Aux buffers in the framebuffer.
                PFD_MAIN_PLANE,
                0,
                0, 0, 0
            };
            iPixelFormat = ChoosePixelFormat(hDC, &pfd);
            SetPixelFormat(hDC, iPixelFormat, &pfd);

            hglrc = wglCreateContext(hDC);
        }

        if (wglMakeCurrent(hDC, hglrc))
        {
            m_pRender.m_hDc = hDC;
            m_pRender.m_hGLRC = hglrc;
            m_pProp.m_bOpenGL = true;
            return true;
        }
        else
        {
            bug_msg("Abort", "Failed create OpenGL rendering context.");
            DeleteOpenGLContext();
            m_pProp.m_bOpenGL = false;
            return false;
        }
    }

    //==================================================================================
    // Xóa ngữ cảnh OpenGL trên window                                                  
    //==================================================================================
    void DeleteOpenGLContext()
    {
        ReleaseDC(m_hWnd, m_pRender.m_hDc); // release device context
        wglDeleteContext(m_pRender.m_hGLRC);// delete the rendering context
        m_pRender.m_hDc = NULL;
        m_pRender.m_hGLRC = NULL;
    }

    //==================================================================================
    // Khởi tạo window sử dụng GDI plus                                                 
    //==================================================================================
    void CreateGDIplus()
    {
        if (m_pProp.m_bGDIplus)
        {
            GdiplusStartupInput gdiplusStartupInput;
            ULONG_PTR           gdiplusToken;

            Gdiplus::Status status = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
            if (status == Gdiplus::Status::Ok)
            {
                m_gdiToken.m_id = gdiplusToken;
                m_gdiToken.m_prop = gdiplusStartupInput;
            }
            else
            {
                m_gdiToken.m_id = NULL;
            }
        }
    }

    //==================================================================================
    // Xóa ngữ cảnh window sử dụng GDI plus                                             
    //==================================================================================
    void DeleteGDIplus()
    {
        GdiplusShutdown(m_gdiToken.m_id);
    }

    //==================================================================================
    // Hàm tạo handle và thông số                                                       
    //==================================================================================
    void _CreateWindow(const char * strWndClassName)
    {
        // Create GDI+ startup incantation
        this->CreateGDIplus();

        // Kích thức thực tế của vùng có thể vẽ
        RECT wr = { 0, 0, m_width, m_height };     // set the size, but not the position
        AdjustWindowRect(&wr, m_pProp.m_dwStyle, FALSE);// adjust the size

        m_hWnd = CreateWindowEx(             //
                m_pProp.m_dwExStyle,        //
                strWndClassName,            //
                m_title.c_str(),            //
                m_pProp.m_dwStyle,          //
                m_x, m_y,                   // Postion 
                wr.right - wr.left,         // Actual width size
                wr.bottom - wr.top,         // Actual height size
                NULL,                       //
                NULL,                       //
                NULL,                       //
                NULL                        //
        );

        // Create window failed
        if (!m_hWnd)
        {
            this->DeleteGDIplus();
            bug_msg("Error", "Create Window failed !");
            return;
        }

        // Update size window after created
        RECT rect;
        if (GetClientRect(m_hWnd, &rect))
        {
            m_width  = rect.right - rect.left;
            m_height = rect.bottom - rect.top;
        }
    }

    //==================================================================================
    // Khởi tạo window và thiết lập thông số                                            
    //==================================================================================
    bool _OnCreate(const char* strClassname, bool bInitOpenGLEx = false)
    {
        // Update get style window
        this->SetUpHint();

        // Create a window HWND use class name
        this->_CreateWindow(strClassname);

        // Update get style window
        this->UpdateHint();

        // Create OpenGL context
        this->CreateOpenGLContext(bInitOpenGLEx);

        this->OnCreated();

        // Update title after created ok
        this->UpdateTitle();

        // Initialization control
        this->OnInitControl();

        // Update font control after initialization control
        this->UpdateFont();

        // Update and setup properties when everything is done
        this->InitProperties();

        return true;
    }

//=======================================================================================
//⮟⮟ Triển khai cập nhật trạng thái của window                                          
//=======================================================================================
private:
    //===================================================================================
    // Lưu giữ trạng thái thông tin của window                                           
    //===================================================================================
    void PushWindowStatus()
    {
        WndStatus status;
        status.m_title  = m_title;
        status.m_x      = m_x;
        status.m_y      = m_y;
        status.m_width  = m_width;
        status.m_height = m_height;
        status.m_RelState = 1;

        m_StatusStack.push(status);
    }

    //===================================================================================
    // Lấy lại trạng thái đã được lưu dữ trước đó                                        
    //===================================================================================
    WndStatus PopWindowStatus()
    {
        WndStatus status;
        if (!m_StatusStack.empty())
        {
            status = m_StatusStack.top();
            m_StatusStack.pop();
        }
        return status;
    }

    //===================================================================================
    // Cập nhật trạng thái thời gian mỗi khi một frame trôi qua                          
    //===================================================================================
    void UpdateTime()
    {
        //TimePoint current_time = high_resolution_clock::now();
        //Tduration elapsed = current_time - m_last_time;
        //m_timeElapsed  = elapsed.count();
        //m_last_time    = current_time;

        m_fpscounter.Update();
    }

    //===================================================================================
    // Khởi tạo trạng thái và thông tin thuộc tính khi đã tạo windows xong               
    //===================================================================================
    void InitProperties()
    {
        m_fpscounter.Reset();
        //m_update_timer.Reset();

        // Setup Text render;
        this->ReloadTextRender();
    }

    //===================================================================================
    // Cập nhật lại title window                                                         
    //===================================================================================
    void UpdateTitle()
    {
        if (!m_hWnd) return;

        string gpu_device = "No Device";
        if ((char*)glGetString(GL_RENDERER))
        {
            gpu_device = (char*)glGetString(GL_RENDERER);
        }

        char titlebuff[256];
        sprintf(titlebuff, "%s - %d x %d - %s",
                            m_title.c_str(),
                            m_width, m_height,
                            gpu_device.c_str());
        SetWindowText(m_hWnd, titlebuff);
    }

    //===================================================================================
    // Cập nhật lại thông số cho text render window                                      
    //===================================================================================
    void UpdateTextRender()
    {
        m_text_render.UpdateView(m_width, m_height);
    }

    //===================================================================================
    // Load và cập nhật lại text render trên window                                      
    //===================================================================================
    void ReloadTextRender()
    {
        // Setup Text render;
        m_text_render.Init(this->GetHDC(), m_width, m_height);
        m_text_render.LoadFont(m_fontNameTextRender, m_fontSizeTextRender);
    }

    //===================================================================================
    // Cập nhật trạng thái hiển thị                                                      
    //===================================================================================
    void UpdateState(bool bShow = true)
    {
        if (!m_hWnd) return;

        if (bShow)
        {
            ShowWindow(this->m_hWnd, SW_SHOW);
        }
        else
        {
            ShowWindow(this->m_hWnd, SW_HIDE);
        }
    }

    //===================================================================================
    // Cập nhật thông tin stype của window                                               
    //===================================================================================
    void UpdateHint()
    {
        if (m_pProp.m_bFullScreen)
        {
            this->PushWindowStatus();

            // Set new window style and size.
            SetWindowLong(m_hWnd, GWL_STYLE, m_pProp.m_dwStyle);
            SetWindowLong(m_hWnd, GWL_EXSTYLE, m_pProp.m_dwExStyle);

            // On expand, if we're given a window_rect, grow to it, otherwise do not resize.
            MonitorInfo monitor = GetMonitorInforEx();
            SetWindowPos(m_hWnd, NULL, 0, 0, monitor.WIDTH, monitor.HEIGHT, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

            // Update width ,height screen
            m_width  = monitor.WIDTH;
            m_height = monitor.HEIGHT;
        }
        else
        {
            WndStatus winState = this->PopWindowStatus();
            if (winState.m_RelState)
            {
                // Cập nhật lại style của Window;
                this->UpdateStyleWindow();

                if (ChangeDisplaySettings(NULL, CDS_RESET) == DISP_CHANGE_SUCCESSFUL)
                {
                    SetWindowLong(m_hWnd, GWL_STYLE , m_pProp.m_dwStyle);
                    SetWindowLong(m_hWnd, GWL_EXSTYLE, m_pProp.m_dwExStyle);
                    m_x      = winState.m_x;
                    m_y      = winState.m_y;
                    m_width  = winState.m_width;
                    m_height = winState.m_height;

                    RECT wr = { 0, 0, m_width, m_height };           // set the size, but not the position
                    AdjustWindowRect(&wr, m_pProp.m_dwStyle, FALSE); // adjust the size

                    SetWindowPos(m_hWnd, HWND_NOTOPMOST, m_x, m_y, (wr.right - wr.left), (wr.bottom - wr.top), SWP_SHOWWINDOW);

                    // Update size window after created
                    m_width  = winState.m_width;
                    m_height = winState.m_height;

                    //ShowWindow(m_hWnd, SW_RESTORE);
                }
            }
        }
    }
//======================================================================================
//⮟⮟ Triển khai chức năng hỗ trợ cho window                                            
//======================================================================================
public:

    //===================================================================================
    // Hiển thị text ra window                                                           
    //===================================================================================
    void WriteText(const char* text, int x, int y, float r = 1.0f, float g = 1.0f,
                                                   float b = 1.0f, float a = 1.0f)
    {
        m_text_render.Use();
        m_text_render.Write(x, y, text, r, g, b, a);
        m_text_render.DontUse();
    }

    //===================================================================================
    // Có hiển thị thông tin hệ thống lên window hay không                               
    //===================================================================================
    void WriteSystemInfo(bool bShow = true)
    {
        m_bSysInfo = bShow;
    }

public:

    Window(const char* title, int xpos, int ypos, int width = 640, int height = 480)
    {
        this->m_title = title;
        this->m_x     = xpos;
        this->m_y     = ypos;
        this->m_width = width;
        this->m_height= height;

        // setup default font
        this->m_fontName   = "Arial";
        this->m_fontSize   = 12;
        this->m_fontWeight = FontWeight::Normal;

        // setup default text render
        this->m_fontNameTextRender = "Consolas";
        this->m_fontSizeTextRender = 16;

        this->m_bSysInfo = false;

        //ResetTimer();
    }

    Window(const Window& win)
    {
        this->m_x       = win.m_x;
        this->m_y       = win.m_y;
        this->m_width   = win.m_width;
        this->m_height  = win.m_height;
        this->m_pProp   = win.m_pProp;
        //ResetTimer();
    }

    void SetupAdvanced(WndProp prop)
    {
        this->m_pProp = prop;
    }

    void UpdateStyleWindow()
    {
        m_pProp.m_dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;     // Window Extended Style
        m_pProp.m_dwStyle   = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;  // Windows Style
        //@@ WS_CLIPCHILDREN: Control của window sẽ không được vẽ khi SwapBuffer

        if (m_pProp.m_bFullScreen)                           // Are We Still In Fullscreen Mode?
        {
            // Window Extended Style
            m_pProp.m_dwExStyle = m_pProp.m_dwExStyle &~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
            // Windows Style
            m_pProp.m_dwStyle   = m_pProp.m_dwStyle &~(WS_CAPTION | WS_THICKFRAME);
        }
    }

    void SetUpHint()
    {
        this->UpdateStyleWindow();
    }

    void ExitFullScreen()
    {
        m_pProp.m_bFullScreen = false;
        this->UpdateHint();
    }

    void FullScreen()
    {
        m_pProp.m_bFullScreen = true;
        this->UpdateHint();
    }

    void Show()
    {
        this->m_bShow = true;
        this->UpdateState(m_bShow);
    }

    void Hide()
    {
        this->m_bShow = false;
        this->UpdateState(m_bShow);
    }

    void SetTitle(string title)
    {
        m_title = title;
        this->UpdateTitle();
    }

    // Activate openGL context
    bool MakeContext()
    {
        if (m_pRender.m_hDc && m_pRender.m_hGLRC)
        {
            if (wglMakeCurrent(m_pRender.m_hDc, m_pRender.m_hGLRC))
            {
                return true;
            }
        }
        assert(0);
        return false;
    }

    void AddControl(Control* control)
    {
        m_controls.push_back(control);
    }

    GLWindow* GetGLWindow()    { return m_pGLWin;   }
    int       GetWidth()       { return m_width;    }
    int       GetHeight()      { return m_height;   }
    int       GetMouseScroll() { return (int)m_zDeltaScroll;}

    //==================================================================================
    // Lấy vị trí tương đối của con trỏ so điểm gốc (trái trên)                         
    //==================================================================================
    bool GetCursorPos(int& xpos, int& ypos)
    {
        POINT cursor_pos;
        if (::GetCursorPos(&cursor_pos) && ScreenToClient(m_hWnd, &cursor_pos))
        {
            xpos = cursor_pos.x;
            ypos = cursor_pos.y;
            return true;
        }
        else
        {
            xpos = 0;
            ypos = 0;
            return false;
        }
    }
    bool GetCursorPos(float& xpos, float& ypos)
    {
        POINT cursor_pos;
        if (::GetCursorPos(&cursor_pos) && ::ScreenToClient(m_hWnd, &cursor_pos))
        {
            xpos = (float)cursor_pos.x;
            ypos = (float)cursor_pos.y;
            return true;
        }
        else
        {
            xpos = 0;
            ypos = 0;
            return false;
        }
    }

    //==================================================================================
    // Lấy vị trí tương đối của con trỏ so với điểm giữa handle                         
    //==================================================================================
    bool GetCursorPosCenter(float& xpos, float& ypos)
    {
        POINT cursor_pos;
        if (::GetCursorPos(&cursor_pos) && ScreenToClient(m_hWnd, &cursor_pos))
        {
            xpos = -(m_width / 2 - (float)cursor_pos.x);
            ypos = (m_height / 2 - (float)cursor_pos.y);
            return true;
        }
        else
        {
            xpos = 0;
            ypos = 0;
            return false;
        }
    }

    //==================================================================================
    // Sử dụng cho trường hợp dùng GDIplus (xóa sau khi sử dụng xong)                   
    //==================================================================================
    WndGDIplus* GetGraphicsFromHWND()
    {
        return Graphics::FromHWND(m_hWnd);;
    }

    //==================================================================================
    // Kiểm tra trạng thái key, key sẽ lấy ở GLWinDef                                   
    //==================================================================================
    bool GetKeyboardStatus(int keyboard)
    {
        return m_keyboard[keyboard];
    }

    //==================================================================================
    // Kiểm tra trạng thái button mouse, mouse sẽ lấy ở GLWinDef                        
    //==================================================================================
    bool GetMouseButtonStatus(int btn)
    {
        return m_mouse[btn];
    }

    //==================================================================================
    // Lấy thời gian trôi qua từ frame trước sang frame hiện tại                        
    //==================================================================================
    double GetFrameTime()
    {
        //return m_timeElapsed;
        return m_fpscounter.FrameTime();
    }

    //==================================================================================
    // Lấy FPS window                                                                   
    //==================================================================================
    unsigned int GetFPS()
    {
        //return m_timeElapsed;
        return m_fpscounter.FPS();
    }

    //==================================================================================
    // Hàm hủy Window                                                                   
    //==================================================================================
    virtual void OnDestroy()
    {
        // Gọi hàm mở rộng trước
        if (m_funOnDestroy)
        {
            m_funOnDestroy(this);
        }

        // Xử lý hủy mặc định
        DeleteGDIplus();
        DeleteOpenGLContext();
        DestroyControl();       //N.V.Thuong 22.04.2022
        DestroyWindow(m_hWnd);
    }

    friend class GLWindow;
    friend class WindowManager;
};

class WindowManager
{
private:
    unordered_map<HWND     , Window*>            windows;
    unordered_map<GLWindow*, vector<Window*>>    windowGLs;

public:

    //==================================================================================
    // Hàm quản lý window chung                                                         
    //==================================================================================
    Window* Find(HWND hwnd)
    {
        auto found = windows.find(hwnd);
        if (found != windows.end())
        {
            return found->second;
        }
        return NULL;
    }

    bool Add(Window* win)
    {
        if (!win || !win->m_hWnd) return false;

        auto found = windows.find(win->m_hWnd);
        if (found == windows.end())
        {
            windows[win->m_hWnd] = win;
            windowGLs[win->m_pGLWin].push_back(win);
            return true;
        }
        return false;
    }

    void Remove(HWND hwnd)
    {
        auto found = windows.find(hwnd);
        if (found != windows.end())
        {
            Window* win = found->second;

            // Xóa window mà gl quản lý
            for (auto it = windowGLs[win->m_pGLWin].begin();
                      it != windowGLs[win->m_pGLWin].end();)
            {
                if (win == *it)
                {
                    it = windowGLs[win->m_pGLWin].erase(it);
                }
                else
                {
                    ++it;
                }
            }
            windows.erase(found);
        }
    }

    int GetSize()
    {
        return (int)windows.size();
    }

    void Clear()
    {
        windows.clear();
        windowGLs.clear();
    }

    //==================================================================================
    // Hàm quản lý theo windowGL                                                        
    //==================================================================================
    bool winGL_IsMain(GLWindow* winGL,const Window* win)
    {
        if (windowGLs[winGL].empty()) return false;

        if (windowGLs[winGL].front() == win)
        {
            return true;
        }
        return false;
    }

    Window* winGL_GetFront(GLWindow* winGL)
    {
        if (windowGLs[winGL].empty()) return NULL;

        return windowGLs[winGL].front();
    }

    // Trả về tham chiếu đến danh sách của GLWindow
    vector<Window*>& winGL_GetListWindow(GLWindow* pGL)
    {
        return windowGLs[pGL];
    }
};

class GLWindow
{
private:
    MSG                 m_msg;
    Window*             m_pMainWin;

    bool                bClosed = false;

public:

    static WindowManager* GetManaIntance()
    {
        static WindowManager winmana;
        return &winmana;
    }

public:

    static LRESULT CALLBACK WndMainProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        WindowManager* pWinMana = GetManaIntance();

        switch (message)
        {
            case WM_DESTROY:
            {
                Window* win = pWinMana->Find(hWnd);
                if (!win) break;

                if (pWinMana->winGL_IsMain(win->m_pGLWin, win))
                {
                    win->GetGLWindow()->bClosed = true;
                    //PostQuitMessage(0);
                }
                else
                {
                    pWinMana->Remove(win->m_hWnd);
                    win->OnDestroy();
                }
                break;
            }
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            {
                Window* win = pWinMana->Find(hWnd);
                win->SetKeyboardStatus((int)wParam, true);
                win->OnKeyBoard();
                break;
            }
            case WM_KEYUP:
            case WM_SYSKEYUP:
            {
                Window* win = pWinMana->Find(hWnd);
                win->SetKeyboardStatus((int)wParam , false);
                win->OnKeyBoard();
                break;
            }
            // Button event
            case WM_LBUTTONUP:
            {
                Window* win = pWinMana->Find(hWnd);
                win->SetMouseButtonStatus(VK_LBUTTON, false);
                win->OnMouseButton();
                break;
            }
            case WM_RBUTTONUP:
            {
                Window* win = pWinMana->Find(hWnd);
                win->SetMouseButtonStatus(VK_RBUTTON, false);
                win->OnMouseButton();
                break;
            }
            case WM_LBUTTONDOWN:
            {
                Window* win = pWinMana->Find(hWnd);
                win->SetMouseButtonStatus(VK_LBUTTON, true);
                win->OnMouseButton();
                break;
            }
            case WM_RBUTTONDOWN:
            {
                Window* win = pWinMana->Find(hWnd);
                win->SetMouseButtonStatus(VK_RBUTTON, true);
                win->OnMouseButton();
                break;
            }
            case WM_MOUSEMOVE:
            {
                Window* win = pWinMana->Find(hWnd);
                win->OnMouseMove();
                break;
            }
            case WM_SIZE: //Check if the window has been resized
            {
                Window* win = pWinMana->Find(hWnd);
                if (!win) break;

                win->m_width  = LOWORD(lParam); // width
                win->m_height = HIWORD(lParam); // height

                glViewport(0, 0, win->m_width, win->m_height);
                win->OnResize();
                win->UpdateTitle();
                win->UpdateTextRender();
                win->OnDraw();
                win->SwapBuffer();

                return 0;
            }
            case WM_SIZING:
            {
                Window* win = pWinMana->Find(hWnd);
                if (!win) break;
                glViewport(0, 0, win->m_width, win->m_height);
                win->UpdateTitle();
                return TRUE;
            }
            case WM_MOUSEWHEEL:
            {
                Window* win = pWinMana->Find(hWnd);
                if (win)
                {
                    win->m_zDeltaScroll = GET_WHEEL_DELTA_WPARAM(wParam)/WHEEL_DELTA;
                    win->OnMouseScroll();
                }
                break;
            }
            case WM_COMMAND:
            {
                Window* win = pWinMana->Find(hWnd);
                if (win)
                {
                    WORD wID  = LOWORD(wParam); // item, control, or accelerator identifier
                    WORD wEvt = HIWORD(wParam); // item event
                    HWND hwnd = (HWND) lParam;  // handle of control
                    win->OnCommandControl(hwnd, wID, wEvt);
                }
                break;
            }
            case WM_DRAWITEM:
            {
                Window* win = pWinMana->Find(hWnd);
                if (win)
                {
                    WORD wID  = LOWORD(wParam); // item, control, or accelerator identifier
                    HWND hwnd = (HWND) lParam;  // handle of control
                    LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
                    win->OnDrawControl(hwnd, wID);
                }
                break;
            }
            case WM_PAINT:
            {
                Window* win = pWinMana->Find(hWnd);
                if (win)
                {
                    win->OnPaint();
                }
                break;  //[BUG] always drawing
            }
            case WM_ERASEBKGND:
            {
                return TRUE;
            }
            default:
            {
                break;
            }
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

public:
    void AddWindow(Window* win)
    {
        // Init openGL extention use glew
        static bool bInitOpenGLEx = false;
        if (!bInitOpenGLEx)
        {
            bInitOpenGLEx = LoadOpenGLExtension();
        }

        if (win->_OnCreate(GL_WIN_CLASS, bInitOpenGLEx))
        {
            WindowManager* pWinMana = GetManaIntance();

            win->m_pGLWin = this;
            if (pWinMana->Add(win))
            {
                win->Show();
            }
            else
            {
                win->OnDestroy();
            }
        }
    }

    Window* GetMainWin()
    {
        WindowManager* pWinMana = GetManaIntance();
        return pWinMana->winGL_GetFront(this);
    }

    GLWindow()
    {
        RegisterWindowClass(GL_WIN_CLASS, WndMainProc, GetModuleHandle(NULL));
    }

    ~GLWindow()
    {
        WindowManager* pWinMana = GetManaIntance();
        auto winlist = pWinMana->winGL_GetListWindow(this);

        for (int i = 0; i < winlist.size(); i++)
        {
            pWinMana->Remove(winlist[i]->m_hWnd);
            winlist[i]->OnDestroy();
        }
    }

public:

    void Process()
    {
        WindowManager* pWinMana = GetManaIntance();
        auto winlist = pWinMana->winGL_GetListWindow(this);

        for (int i = 0; i < winlist.size(); i++) {
            winlist[i]->UpdateTime();
            winlist[i]->OnProcess();
        }
    }

    void Draw()
    {
        WindowManager* pWinMana = GetManaIntance();
        auto winlist = pWinMana->winGL_GetListWindow(this);

        for (int i = 0; i < winlist.size(); i++) {
            if (winlist[i]->MakeContext())
            {
                winlist[i]->OnDraw();
                winlist[i]->SwapBuffer();
            }
        }
    }

    bool Closed()
    {
        return !bClosed;
    }

    void PollEvent()
    {
        // Xử lý trong trường hợp vẽ liên tục
        // Check to see if any messages are waiting in the queue
        while (PeekMessage(&m_msg, NULL, 0, 0, PM_REMOVE))
        {
            // Translate the message and dispatch it to WindowProc()
            TranslateMessage(&m_msg);
            DispatchMessage(&m_msg);
        }
    }

    void WaitEvent()
    {
        // Xử lý trong trường hợp vẽ theo sự kiện
        if (GetMessage(&m_msg, NULL, 0, 0) > 0)
        {
            TranslateMessage(&m_msg);
            DispatchMessage(&m_msg);
        }
    }
};

#endif // !GLWINDOW_H
