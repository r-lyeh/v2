// playground tests
// - rlyeh, public domain
//
// # quickstart
// Double click MAKE.bat, or invoke `MAKE.bat`, or invoke `MAKE.bat source_file.c`

#include "engine.h"

typedef struct app_t {
    const char *name;
    int(*main)();
} app_t;

array(app_t) apps;

int app_cmp( const void *a, const void *b ) {
    return strcmp( ((app_t*)a)->name, ((app_t*)b)->name );
}

const char *trim(const char *pathfile) {
    // clean up. returns either extension paths or demo names
    if( strstr(pathfile, "ext-") ) return file_path(strstr(pathfile, "ext-"));
    const char *name = file_name(pathfile);
    return strchr(name, '-') ? strchr(name, '-')+1 : name;
}

int find_demo(const char *name) {
    if( isalpha(*name) )
    for( int i = 0; i < array_count(apps); ++i ) {
        if( strstri(trim(apps[i].name), name) ) return i;
    }
    return isdigit(*name) ? atoi(name) : -1;
}

int main() {
    if( argc() > 1 && is_file(argv(1)) ) return lua_init(), luaj_init(), lua_runfile(argv(1)), 0;

    array_sort(apps, app_cmp);
    int demo = argc() > 1 ? find_demo(argv(1)) : -1;
    if( demo >= 0 && demo < array_count(apps) ) return apps[demo].main();

    app_create(100, APP_TRANSPARENT|APP_MSAA2);
    command("ui.debug=0");

    static int open = UI_OPEN | UI_CENTER;
    while( app_swap() && !input(KEY_ESC) && (open & UI_OPEN) ) {
        if( ui_window("Demo Launcher", &open) ) {
            for( int i = 0; i < array_count(apps); ++i) {
            int choice = ui_buttons(2, va("%d: %s", i, trim(apps[i].name)), "...");
            if( choice == 1 ) app_send("alpha","0"), app_send("show","0"), system(va("%s %d", argv(0), i)), app_send("show","1"), app_send("alpha", "1");
            if( choice == 2 ) system(va("%s \"%s\"", ifdef(win32,"start \"\"",ifdef(osx,"open","xdg-open")), apps[i].name));
            }
            ui_window_end();
        }
    }
    return 0;
}

#pragma warning(disable : 4716)
#define ma1n(file,func) static macro(x); int func(); AUTORUN { array_push(apps, ((app_t){file, func})); } int func
#define main            ma1n( __FILE__, CONCAT(app,__COUNTER__) )

#include "demos/00-null.c"
#include "demos/01-window.c"
#include "demos/02-imgui.c"
#include "demos/03-opengl.c"
#include "demos/04-image.c"
#include "demos/05-ddraw.c"
#include "demos/06-fx.c"
#include "demos/07-font.c"
#include "demos/08-input.c"
#include "demos/09-shadertoy.c"
#include "demos/10-tiled.c"
#include "demos/11-sprite.c"
#include "demos/12-sprite3d.c"
#include "demos/13-model.c"
#include "demos/14-script.c"
#include "demos/15-audio.c"
#include "demos/16-shadows.c"
#include "demos/17-cubemap.c"
#include "demos/18-webcam.c"

#if __has_include("ext/ext-demos.c")
#include "ext/ext-demos.c"
#endif

#if __has_include("ext-private/ext-demos.c")
#include "ext-private/ext-demos.c"
#endif
