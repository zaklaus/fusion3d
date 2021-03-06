// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_WINDOW_H)

global_variable window GlobalWindowArea;
global_variable HDC GlobalDeviceContext;
global_variable HWND GlobalWindow;

#define F3D_SHADOW_RES 4096

// NOTE(ZaKlaus): Global buffers..

enum {
    Framebuffer_Depth,
    Framebuffer_ColorDepth,
    Framebuffer_Color,
    Num_Of_Framebuffers,
};
global_variable GLuint GlobalRenderBuffers[Num_Of_Framebuffers] = {0};
global_variable GLuint GlobalFrameTextures[Num_Of_Framebuffers] = {0};

global_variable GLuint GlobalScreenArray = 0;
global_variable GLuint GlobalScreenBuffer = 0;


global_variable b32 GlobalKeyDown[0xFE] = {0};
global_variable b32 GlobalKeyUp[0xFE] = {0};
global_variable b32 GlobalKeyPress[0xFE] = {0};

global_variable s32 GlobalMouseX = 0;
global_variable s32 GlobalMouseY = 0;

void MeshReleaseAll();
void TextureReleaseAll();
void ShaderProgramReleaseAll();

extern b32 Running;

LRESULT
WndProc(HWND Window,
        UINT Message,
        WPARAM WParam,
        LPARAM LParam)
{
    if(GUIProcessFrame(Window, Message, WParam, LParam))
    {
        return(0);
    }
    
    switch(Message)
    {
        case WM_ACTIVATEAPP:
        case WM_SIZE:
        {
        }break;
        
        case WM_CLOSE:
        case WM_DESTROY:
        {
            Running = 0;
        }break;
        
        case WM_KEYDOWN:
        {
            GlobalKeyDown[WParam] = 1;
            GlobalKeyPress[WParam] = 1;
            GlobalKeyUp[WParam] = 0;
        }break;
        
        case WM_KEYUP:
        {
            GlobalKeyDown[WParam] = 0;
            GlobalKeyUp[WParam] = 1;
        }break;
    }
    return(DefWindowProc(Window, Message, WParam, LParam));
}

internal void
WindowInitialize(HINSTANCE Instance)
{
    WindowCreateClass(Instance, "Handmade FTW", &WndProc);
    
    window_dim PosDim = {0};
    window_dim ResDim = {1280, 800};
    
    WindowCreateWindowed("Handmade FTW", "Fusion3D", Instance, 0, 0, ResDim, PosDim, CW_USEDEFAULT, &GlobalWindow);
    
    window_dim Dim = WindowGetClientRect(GlobalWindow);
    window_resize_result ResizeResult = WindowResize(Dim.X, Dim.Y, GlobalWindowArea, 1);
    
    GlobalWindowArea = ResizeResult;
    
    GlobalDeviceContext = GetDC(GlobalWindow);
    b32 ModernContext = 1;
    Win32InitOpenGL(GlobalDeviceContext, &ModernContext);
    GUIInitialize(GlobalDeviceContext, GlobalWindow);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    static const GLfloat VertexArrayData[] = {
        -1, -1, 0,
        1, -1, 0,
        -1, 1,  0,
        -1, 1,  0,
        1, -1, 0,
        1, 1,  0,
    };
    
    glGenBuffers(1, &GlobalScreenBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, GlobalScreenBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexArrayData), VertexArrayData, GL_STATIC_DRAW);
    
    // NOTE(ZaKlaus): Generate global buffers
    {
        glGenFramebuffers(Num_Of_Framebuffers, GlobalRenderBuffers); 
        glGenTextures(Num_Of_Framebuffers, GlobalFrameTextures);
        
        // NOTE(ZaKlaus): Color render buffer
        {
            glBindFramebuffer(GL_FRAMEBUFFER, GlobalRenderBuffers[Framebuffer_Color]);   
            glBindTexture(GL_TEXTURE_2D, GlobalFrameTextures[Framebuffer_Color]);
            
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ResDim.X, ResDim.Y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GlobalFrameTextures[Framebuffer_Color], 0);
            
            glBindTexture(GL_TEXTURE_2D, GlobalFrameTextures[Framebuffer_ColorDepth]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, ResDim.X, ResDim.Y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
            
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, GlobalFrameTextures[Framebuffer_ColorDepth], 0);
            
            GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
            glDrawBuffers(1, DrawBuffers);
        }
        
        // NOTE(ZaKlaus): Depth render buffer
        //if(0)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, GlobalRenderBuffers[Framebuffer_Depth]);
            glBindTexture(GL_TEXTURE_2D, GlobalFrameTextures[Framebuffer_Depth]);
            
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, F3D_SHADOW_RES, F3D_SHADOW_RES, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  
            
            glBindFramebuffer(GL_FRAMEBUFFER, GlobalRenderBuffers[Framebuffer_Depth]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, GlobalFrameTextures[Framebuffer_Depth], 0);
            
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
    }
    
    wglSwapIntervalEXT(0);
}

internal void
WindowSetMousePos(s32 x, s32 y)
{
    POINT MousePos = {x,y};
    ClientToScreen(GlobalWindow, &MousePos);
    
    SetCursorPos(MousePos.x, MousePos.y);
    
    POINT MousePos2 = {0};
    GetCursorPos(&MousePos2);
    ScreenToClient(GlobalWindow, &MousePos2);
    
    GlobalMouseX = MousePos2.x;
    GlobalMouseY = MousePos2.y;
}

internal void
MainWindowUpdate(void)
{
    wglMakeCurrent(GlobalDeviceContext, OpenGLRC);
    
    for(s32 Idx = 0;
        Idx < 0xFE;
        ++Idx)
    {
        GlobalKeyPress[Idx] = 0;
    }
    
    POINT MousePos = {0};
    GetCursorPos(&MousePos);
    ScreenToClient(GlobalWindow, &MousePos);
    
    GlobalMouseX = MousePos.x;
    GlobalMouseY = MousePos.y;
    
    WindowUpdate();
    window_dim Dim = {0};
    s32 GlobalWindowWidth = GlobalWindowArea.Width;
    s32 GlobalWindowHeight = GlobalWindowArea.Height;
    
    Dim = WindowGetClientRect(GlobalWindow);
    window_resize_result ResizeResult = WindowResize(Dim.X, Dim.Y, GlobalWindowArea, 1);
    glViewport(0, 0, GlobalWindowArea.Width, GlobalWindowArea.Height);
    GlobalWindowArea = ResizeResult;
    
    // TODO(ZaKlaus): Resize based on window dim changes.
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Dim.X, Dim.Y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
}

global_variable GLuint renderType = 0;
global_variable GLuint depthT = 0;

internal void
WindowBlit(GLuint Program)
{
    glUseProgram(Program);
    local_persist GLuint OldProgram = 0;    
    if(OldProgram != Program)
    {
        local_persist GLuint renderT = glGetUniformLocation(Program, "renderTexture");
        GLuint depthT = glGetUniformLocation(Program, "depthTexture");
        renderType = glGetUniformLocation(Program, "renderType");
        
        blit_send:
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, GlobalFrameTextures[Framebuffer_Color]);
            glUniform1i(renderT, 0);
            
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, GlobalFrameTextures[Framebuffer_Depth]);
            glUniform1i(depthT, 1);
            
            glUniform1i(renderType, 1);
        }
        goto blit_end;
    }
    
    goto blit_send;
    
    blit_end:
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glViewport(0, 0, GlobalWindowArea.Width, GlobalWindowArea.Height);
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, GlobalScreenBuffer);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        0, (void *)0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(0);
    OldProgram = Program;
}

internal void
WindowShutdown(void)
{
    MeshReleaseAll();
    TextureReleaseAll();
    ShaderProgramReleaseAll();
    Win32ReleaseOpenGL();
    
    ReleaseDC(GlobalWindow, GlobalDeviceContext);
}

#define F3D_WINDOW_H
#endif