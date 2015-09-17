#include <windows.h>

#include <fstream>
#include <stdio.h>
#include <tchar.h>

#include <glade/exception/GladeException.h>
#include <glade/debug/log.h>
#include <glade/debug/crashHandler.h>
#include <glade/opengl/functions.h>
#include <glade/render/GladeRenderer.h>
#include <glade/render/DrawFrameHook.h>
#include <glade/Context.h>
#include <glade/State.h>
#include <glade/util/DesktopFileManager.h>
#include <glade/util/ResourceManager.h>
#include <strug/ResourceManager.h>
#include <strug/controls/StrugController.h>
#include <strug/exception/StrugException.h>
#include <strug/states/MazeTest.h>
#include <strug/states/GeneratorTest.h>

#define VIEWPORT_WIDTH 600
#define VIEWPORT_HEIGHT 600
#define ASSETS_DIR "assets"

// FIXME global resource manager is shit. Make it a part of context (Game Context probably should extend Glade Context)
FileManager *file_manager = NULL;
Glade::ResourceManager *resource_manager;
Strug::ResourceManager *game_resource_manager;

Context *gameContext = NULL;

class StandardDrawFrameHook : public DrawFrameHook
{
  private:
    Context* context;
  
  public:
    StandardDrawFrameHook(Context &context) : context(&context)
    {
    }
   
    virtual void onBeforeDraw(void)
    {
      context->processRequests();
      
      if (context->getCurrentState() != NULL) {
        if (context->enableSimulator) {
          //context->getSimulator()->stepSimulation(context->getDeltaTime());  
        }
        
        if (context->enableCollisionDetector) {
          //context->getCollisionDetector()->detectCollisions(context->getDeltaTime());
        }
        
        if (context->enableAiContainer) {
          //context->getAiContainer()->process(context->getCurrentState());
        }
        
        context->getCurrentState()->applyRules(*context);
        
        if (context->enableSoundPlayer) {
          //context->getSoundPlayer()->process();
        }
      }
    }
    
    virtual void onAfterDraw(void)
    {
      if (context->getCurrentState() != NULL) {
      //  context->resetTimer();
      }
    }
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    case WM_INPUT:
    {
      VirtualController *controller = gameContext->getController();
      
      UINT dwSize;

      GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
      LPBYTE lpb = new BYTE[dwSize];
      
      if (lpb == NULL) {
        return 0;
      }

      if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize ) {
        log("GetRawInputData does not return correct size!");
      }

      RAWINPUT* rawInput = (RAWINPUT*)lpb;

      if (controller) {
        if (rawInput->header.dwType == RIM_TYPEKEYBOARD) {
          RAWKEYBOARD *keyboard = (RAWKEYBOARD*) &rawInput->data;
          
          // button is down
          if (keyboard->Flags == RI_KEY_MAKE
            || keyboard->Flags == RI_KEY_MAKE + RI_KEY_E0
            || keyboard->Flags == RI_KEY_MAKE + RI_KEY_E1)
          {
            switch (keyboard->VKey) {
              case VK_UP:
                controller->buttonPress(StrugController::BUTTON_UP,    0);
                break;
              case VK_DOWN:
                controller->buttonPress(StrugController::BUTTON_DOWN,  0);
                break;
              case VK_LEFT:
                controller->buttonPress(StrugController::BUTTON_LEFT,  0);
                break;
              case VK_RIGHT:
                controller->buttonPress(StrugController::BUTTON_RIGHT, 0);
                break;
              case VK_SPACE:
                controller->buttonPress(StrugController::BUTTON_FIRE, 0);
                break;
              case VK_RETURN:
                controller->buttonPress(StrugController::BUTTON_USE, 0);
                break;
              case VK_ESCAPE:
                controller->buttonPress(StrugController::BUTTON_ESCAPE, 0);
                break;
            }
            // button is up
          } else if (keyboard->Flags == RI_KEY_BREAK
            || keyboard->Flags == RI_KEY_BREAK + RI_KEY_E0
            || keyboard->Flags == RI_KEY_BREAK + RI_KEY_E1)
          {
            switch (keyboard->VKey) {
              case VK_UP:
                controller->buttonRelease(StrugController::BUTTON_UP,    0);
                break;
              case VK_DOWN:
                controller->buttonRelease(StrugController::BUTTON_DOWN,  0);
                break;
              case VK_LEFT:
                controller->buttonRelease(StrugController::BUTTON_LEFT,  0);
                break;
              case VK_RIGHT:
                controller->buttonRelease(StrugController::BUTTON_RIGHT, 0);
                break;
              case VK_SPACE:
                controller->buttonRelease(StrugController::BUTTON_FIRE,  0);
                break;
              case VK_RETURN:
                controller->buttonRelease(StrugController::BUTTON_USE,   0);
                break;
              case VK_ESCAPE:
                controller->buttonRelease(StrugController::BUTTON_ESCAPE,  0);
                break;
            }
          }
        } else if (rawInput->header.dwType == RIM_TYPEMOUSE) {
          
        }
      } else {
        log("Warning: no virtual controller in the game context. You can use stub controller to suppress this message");
      }
      
      delete[] lpb; 
    
      int defProcResult = DefRawInputProc(&rawInput, 1, sizeof(RAWINPUTHEADER));
      
      if (defProcResult != S_OK) {
        log("Error returned by DefRawInputProc: %d", defProcResult);
      }
      
      return defProcResult;
    }
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }

  return 0;
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{ 
  ::install_crash_handler();
  
  static TCHAR szWindowClass[] = _T("Strug");
  static TCHAR szTitle[] = _T("Strug prototype");  
  
  WNDCLASSEX wcex;
  ZeroMemory(&wcex, sizeof(WNDCLASSEX));

  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style          = CS_OWNDC;
  wcex.lpfnWndProc    = WndProc;
  wcex.cbClsExtra     = 0;
  wcex.cbWndExtra     = 0;
  wcex.hInstance      = hInstance;
  wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
  wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName   = NULL;
  wcex.lpszClassName  = szWindowClass;
  wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
  
  if (!RegisterClassEx(&wcex))
  {
      MessageBox(NULL,
          _T("Call to RegisterClassEx failed!"),
          _T("Glade"),
          NULL);

      return 1;
  }
  
  RECT viewportRect = {0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT};
  
  if (!::AdjustWindowRect(&viewportRect, WS_OVERLAPPEDWINDOW, false)) {
    log("Warning: Could not adjust window rectangle, error: %d", ::GetLastError());
  }
  
  // The parameters to CreateWindow explained:
  // szWindowClass: the name of the application
  // szTitle: the text that appears in the title bar
  // WS_OVERLAPPEDWINDOW: the type of window to create
  // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
  // 500, 500: initial size (width, length)
  // NULL: the parent of this window
  // NULL: this application does not have a menu bar
  // hInstance: the first parameter from WinMain
  // NULL: not used in this application
  HWND hWnd = CreateWindow(
      szWindowClass,
      szTitle,
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT,
      viewportRect.right - viewportRect.left,
      viewportRect.bottom - viewportRect.top,
      NULL,
      NULL,
      hInstance,
      NULL
  );
  
  if (!hWnd)
  {
      MessageBox(NULL,
          _T("Call to CreateWindow failed!"),
          _T("Glade"),
          NULL);

      return 1;
  }
      
  PIXELFORMATDESCRIPTOR pfd =
  {
      sizeof(PIXELFORMATDESCRIPTOR),
      1,
      PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
      PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
      32,                        //Colordepth of the framebuffer.
      0, 0, 0, 0, 0, 0,
      0,
      0,
      0,
      0, 0, 0, 0,
      24,                        //Number of bits for the depthbuffer
      8,                        //Number of bits for the stencilbuffer
      0,                        //Number of Aux buffers in the framebuffer.
      PFD_MAIN_PLANE,
      0,
      0, 0, 0
  };
  
  HDC hdc = GetDC(hWnd);
  
  int pfNumber = ChoosePixelFormat(hdc, &pfd);
  
  if (!pfNumber) {
    MessageBox(NULL,
        _T("Couldn't get pixel format number"),
        _T("Glade"),
        NULL);

    return 1;
  }
  
  SetPixelFormat(hdc, pfNumber, &pfd);
  
  HGLRC renderingContext = wglCreateContext(hdc);
  
  if (!renderingContext) {
    MessageBox(NULL,
      _T("Failed to create OpenGL context"),
      _T("Glade"),
      NULL);
      
      return 1;
  }
  
  if (!wglMakeCurrent(hdc, renderingContext)) {
    MessageBox(NULL,
      _T("Failed to make OpenGL context current"),
      _T("Glade"),
      NULL);
  }
  
  //OpenGL calls may start here
  
  // Uncomment to see OpenGL version at launch
  //MessageBox(0,(TCHAR*)glGetString(GL_VERSION), "OPENGL VERSION",0);
  
  log("OpenGL version: %s", glGetString(GL_VERSION));
  
  // load necessary OpenGL extensions and function pointers
  loadFunctions();
  // also, re-create OpenGL context using pixel format extensions if necessary
  
  // Now initialize windows raw input system
  
  RAWINPUTDEVICE device[2];
      
  device[0].usUsagePage = 0x01; 
  device[0].usUsage = 0x02; 
  device[0].dwFlags = 0;   // adds HID mouse
  device[0].hwndTarget = 0;

  device[1].usUsagePage = 0x01; 
  device[1].usUsage = 0x06; 
  device[1].dwFlags = 0;   // adds HID keyboard
  device[1].hwndTarget = 0;

  if (RegisterRawInputDevices(device, 2, sizeof(device[0])) == FALSE) {
    log("Could not register raw input device: error %d", GetLastError());
  }
  
  // Done initializing Windows raw input system
  
  // Getting assets directory absolute path
  TCHAR lpFilename[MAX_PATH];
  GetModuleFileName(NULL, lpFilename, MAX_PATH * sizeof(TCHAR));
  Path modulePath(lpFilename);
  modulePath = modulePath.base();
  modulePath.append(ASSETS_DIR);
  log("Assets directory: %s", modulePath.cString());
  
  // Set resource manager pointers
  file_manager = new DesktopFileManager(modulePath.cString());
  resource_manager = new Strug::ResourceManager(file_manager);
  game_resource_manager = (Strug::ResourceManager *) resource_manager;
  
  // Create game context and renderer
  GladeRenderer renderer;
  renderer.onSurfaceCreated();
  renderer.onSurfaceChanged(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
  gameContext = new Context(&renderer);
  StandardDrawFrameHook hook(*gameContext);
  renderer.addDrawFrameHook(hook);

  gameContext->requestStateChange(std::unique_ptr<State>(new MazeTest()));
  
  // The parameters to ShowWindow explained:
  // hWnd: the value returned from CreateWindow
  // nCmdShow: the fourth parameter from WinMain
  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);
  
  MSG msg;
  
  // we may start rendering
  for (;;) {
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT)
        goto cleanup;
      
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    
    renderer.onDrawFrame();
    
    SwapBuffers(hdc);
    
    Sleep(50);
  }
  
  cleanup:
      
  if (gameContext != NULL) {
    delete gameContext;
    gameContext = NULL;
  }
  
  if (resource_manager != NULL) {
    delete resource_manager;
    resource_manager = NULL;
    
    if (game_resource_manager != NULL) {
      game_resource_manager = NULL;
    }
  }
  
  if (file_manager != NULL) {
    delete file_manager;
    file_manager = NULL;
  }
  
  if (!wglMakeCurrent(hdc, NULL)) { // no OpenGL calls beyond this point
    MessageBox(NULL,
      _T("Failed to make OpenGL context noncurrent"),
      _T("Glade"),
      NULL);
  }
  
  wglDeleteContext(renderingContext); 
  
  log("Program is finishing");
  
  return (int) msg.wParam;
  
  log("No cleanup was made at exit!");
  
  return 1;
}

namespace gladen
{
  namespace system
  {
    void exit()
    {
      ::PostQuitMessage(0);
    }
  }
}