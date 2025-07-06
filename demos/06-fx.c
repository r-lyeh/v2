#include "engine.h"

int main() {
    app_create(0.75, APP_MSAA2);

    camera_t cam = camera();

    fx_load("**/fx*.glsl");
    fx_enable(fx_find("fxAberration.glsl"), 1);

    while (app_swap()) {
        // fps camera
        camera_freefly(&cam, 0);

        fx_begin();

            ddraw_grid(0);
            ddraw_demo();
            ddraw_flush();

        fx_end(0,0);

        if( ui_panel("FXs", UI_OPEN) ) {
            ui_fxs();
            ui_panel_end();
        }
    }
}
