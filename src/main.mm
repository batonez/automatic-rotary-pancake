#import <AppKit/NSWindow.h>
#import <AppKit/NSCursor.h>
#import <AppKit/NSOpenGL.h>
#import <AppKit/NSOpenGLView.h>

#include <OpenGL/gl.h>

#include "glade/glade.h"

#include <signal.h>
#include <unistd.h>
#include <execinfo.h>

#define VIEWPORT_WIDTH 400
#define VIEWPORT_HEIGHT 400
/*
@interface MyOpenGLView : NSOpenGLView {

}

- (void) drawRect: (NSRect) bounds;
@end

@implementation MyOpenGLView
-(void) drawRect: (NSRect) bounds {
    NSLog(@"Draw RECT");
    glClearColor(0, 1, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}
@end

@interface MyApplicationDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate> {
    NSWindow * window;
}
@end

@implementation MyApplicationDelegate : NSObject
- (id)init {
    if (self = [super init]) {
        NSRect rect = NSMakeRect(0, 0, 400, 400);
        NSWindowStyleMask mask = NSWindowStyleMaskMiniaturizable|NSWindowStyleMaskTitled|NSWindowStyleMaskClosable;
        window = [[NSWindow alloc] initWithContentRect:  rect
                                              styleMask: mask
                                                backing: NSBackingStoreBuffered
                                                  defer: YES];

        [window setLevel:NSNormalWindowLevel];
        [window setTitle:[[NSString alloc] initWithUTF8String:"Test app"]];
        [window setOpaque:YES];

        MyOpenGLView *drawView = [[MyOpenGLView alloc] initWithFrame:rect];
        [window setContentView: drawView];
    }
    return self;
}


- (void)applicationWillFinishLaunching:(NSNotification *)notification {
    [window makeKeyAndOrderFront:self];
}

- (void)dealloc {
    [window release];
    [super dealloc];
}

- (void)main {
    NSLog(@"DELEGATE MAIN");
}

@end

int main(int argc, char * argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    NSApplication * application = [NSApplication sharedApplication];

    MyApplicationDelegate * appDelegate = [[[MyApplicationDelegate alloc] init] autorelease];

    [application setDelegate:appDelegate];
    [application run];

    [pool drain];

    return EXIT_SUCCESS;
}
*/

////////////////////////////////

bool windowed = false;
NSWindow* mainWnd = NULL;

NSOpenGLView* drawView = NULL;

void segfault_handler(int sig_num)
{
  write(STDOUT_FILENO, "OH MY!\n", 7);
  //char backtrace[2000];
  //backtrace_symbols_fd(backtrace, 2000, STDOUT_FILENO);
  _exit(1);
}

@interface GladeAppDelegate : NSObject <NSApplicationDelegate>
{
}
@end 

@implementation GladeAppDelegate
-(void)applicationWillFinishLaunching: (NSNotification*)application
{
  NSLog(@"APP WILL FINISH LAUNCHING");
}

-(void)dealloc
{
  [super dealloc];
}
@end

NSWindow* macosx_create_window(const char* title, bool windowed, int scr_w, int scr_h, NSView* drawView, NSRect rect)
{
  NSWindowStyleMask mask = windowed ? NSWindowStyleMaskMiniaturizable|NSWindowStyleMaskTitled|NSWindowStyleMaskClosable : NSWindowStyleMaskBorderless;
  mainWnd = [[NSWindow alloc] initWithContentRect: rect
                                        styleMask: mask
                                          backing: NSBackingStoreBuffered
                                            defer: YES];

  [mainWnd setLevel:(windowed ? NSNormalWindowLevel : NSMainMenuWindowLevel+1)];
  [mainWnd setTitle:[[NSString alloc] initWithUTF8String:title]];
  [mainWnd setOpaque:YES];

  [mainWnd setContentView: drawView];
  [mainWnd setInitialFirstResponder:drawView];

  [mainWnd makeKeyAndOrderFront:NSApp];
  [mainWnd makeFirstResponder: drawView];

  [mainWnd makeMainWindow];

  return mainWnd;
}

int main(int argc, char *argv[])
{
  signal(SIGSEGV, segfault_handler);
  NSLog(@"HELLO");

  NSRect rect = NSMakeRect(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

  NSOpenGLPixelFormatAttribute attrs[] =
  {
    NSOpenGLPFAOpenGLProfile, 0x4100, //NSOpenGLProfileVersion4_1Core,
    NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)24,
    NSOpenGLPFAStencilSize, (NSOpenGLPixelFormatAttribute)8,
    NSOpenGLPFAAccelerated,
    NSOpenGLPFADoubleBuffer,
    (NSOpenGLPixelFormatAttribute)0
  };

  NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
  drawView = [[NSOpenGLView alloc] initWithFrame:rect pixelFormat:pixelFormat];

  macosx_create_window("Jebraxapp", true, 500, 500, drawView, rect);

  // Getting assets directory absolute path
  //TCHAR lpFilename[MAX_PATH];
  //GetModuleFileName(NULL, lpFilename, MAX_PATH * sizeof(TCHAR));
  //Path modulePath(lpFilename);
  //modulePath = modulePath.base();
  //modulePath.append(ASSETS_DIR);
  //log("Assets directory: %s", modulePath.cString());

  glade_init(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, "/Users/jebrax/sourcery/sources/strug/build/strug/assets/");

  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
  NSApplication *app = [NSApplication sharedApplication];
  [app setDelegate:[[[GladeAppDelegate alloc] init] autorelease]];

  [mainWnd makeKeyAndOrderFront:nil];
  [NSApp activateIgnoringOtherApps:YES];
  [app run];
  [pool release];

  glade_deinit();

  return EXIT_SUCCESS;
}

