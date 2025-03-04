// sprite routines
// - rlyeh,
//
// credits: original lovely demo by rxi (MIT License).
// see https://github.com/rxi/autobatch/tree/master/demo/cats

#include "engine.h"

texture_t kids, catImage, shadowImage, inputs;
int NUM_SPRITES = 100, NUM_SPRITES_CHANGED = 1;

typedef struct Cat {
    int cat, flip;
    double x, y;
    double vx, vy;
    double animSpeed;
    double moveTimer;
    double elapsed;
} Cat;

void demo_cats() {
    static array(Cat) cats = 0;

    // init
    if( NUM_SPRITES_CHANGED ) {
        NUM_SPRITES_CHANGED = 0;

        array_resize(cats, NUM_SPRITES); int i = 0;
        for each_array_ptr(cats, Cat, c) {
            randset(i++);
            c->x = randf() * app_width();
            c->y = randf() * app_height();
            c->vx = c->vy = 0;
            c->cat = randi(0, 4);
            c->flip = randf() < 0.5;
            c->animSpeed = 0.8 + randf() * 0.3;
            c->moveTimer = 0;
            c->elapsed = 0;
        }
    }

    // move
    const float dt = 1/120.f;
    const int appw = app_width(), apph = app_height();

    enum { yscale = 1 };
    for( int i = 0; i < NUM_SPRITES; ++i ) {
        Cat *c = &cats[i];
        // Add velocity to position //and wrap to screen
        c->x += yscale * c->vx * dt; // % ;
        c->y += yscale * c->vy * dt; // % (int)app_height();
        if( c->x < 0 ) c->x += appw; else if( c->x > appw ) c->x -= appw;
        if( c->y < 0 ) c->y += apph; else if( c->y > apph ) c->y -= apph;
        // Faster animation if walking
        int boost = c->vx == 0 && c->vy == 0 ? 1 : 3;
        // Update elapsed time
        c->elapsed += dt * boost;
        // Update move timer -- if we hit zero then change or zero velocity
        c->moveTimer -= dt * boost;
        if (c->moveTimer < 0) {
            if (randf() < .2) {
                c->vx = (randf() * 2 - 1) * 30 * 2;
                c->vy = (randf() * 2 - 1) * 15 * 2;
                c->flip = c->vx < 0;
            } else {
                c->vx = c->vy = 0;
            }
            c->moveTimer = 1 + randf() * 5;
        }
    }

    // render
    uint32_t white = rgba(255,255,255,255);
    uint32_t alpha = rgba(255,255,255,255*0.6);
    for( int i = 0; i < NUM_SPRITES; ++i ) {
        Cat *c = &cats[i];
        // Get current animation frame (8x4 tilesheet)
        double e = c->elapsed * c->animSpeed;
        double frame_num = c->cat * 8 + floor( ((int)(e * 8)) % 4 );
        frame_num = c->vx != 0 || c->vy != 0 ? frame_num + 4 : frame_num;
        // Get x scale based on flip flag
        int xscale = yscale * (c->flip ? -1 : 1);
        // Draw
        float angle = 0; //fmod(app_time()*360/5, 360);
        float scale[2] = { 2*xscale, 2*yscale };
        float position[3] = { c->x,c->y,c->y }, no_offset[2] = {0,0}, spritesheet[3] = { frame_num,8,4 };
        sprite_sheet(catImage,
            spritesheet,                // frame_number in a 8x4 spritesheet
            position, angle,            // position(x,y,depth: sort by Y), angle
            no_offset, scale,           // offset(x,y), scale(x,y)
            white,0                     // tint_color, flags
        );
        float position_neg_sort[3] = { c->x,c->y,-c->y }, offset[2] = {-1,5}, no_spritesheet[3] = {0,0,0};
        sprite_sheet(shadowImage,
            no_spritesheet,             // no frame_number (0x0 spritesheet)
            position_neg_sort, angle,   // position(x,y,depth: sort by Y), angle
            offset, scale,              // offset(x,y), scale(x,y)
            alpha,0                     // tint_color, flags
        );
    }
}

void demo_kids() {
    static int angle; //++angle;
    static int *x, *y, *v;

    // init
    if( NUM_SPRITES_CHANGED ) {
        NUM_SPRITES_CHANGED = 0;

        y = (int*)REALLOC(y, 0 );
        x = (int*)REALLOC(x, NUM_SPRITES * sizeof(int) );
        y = (int*)REALLOC(y, NUM_SPRITES * sizeof(int) );
        v = (int*)REALLOC(v, NUM_SPRITES * sizeof(int) );
        for( int i = 0; i < NUM_SPRITES; ++i ) {
            randset(i);
            x[i] = randi(0, app_width());
            y[i] = randi(0, app_height());
            v[i] = randi(1, 3);
        }
    }

    // config
    const int appw = app_width(), apph = app_height();

    // move & render
    for( int i = 0; i < NUM_SPRITES; ++i ) {
        y[i] = (y[i] + v[i]) % (apph + 128);
        int col = ((x[i] / 10) % 4); // 4x4 tilesheet
        int row = ((y[i] / 10) % 4);
        int num_frame = col * 4 + row;
        float position[3] = {x[i],y[i],y[i]}, offset[2]={0,0}, scale[2]={1,1}, spritesheet[3]={num_frame,4,4};
        sprite_sheet(kids,
            spritesheet,      // num_frame in a 4x4 spritesheet
            position, angle,  // position(x,y,depth: sort by Y), angle
            offset, scale,    // offset(x,y), scale(x,y)
            ~0u, 0            // tint color, flags
        );
    }
}

int main(int argc, char **argv) {
    app_create(75.f, 0);
//    app_title("Sprites");
//    app_color( SILVER );

    // options
    int do_cats = 1;
    int MAX_SPRITES = 100 * (NUM_SPRITES = optioni("--num_sprites,-N", NUM_SPRITES));
    if(do_cats) NUM_SPRITES/=2; // cat-sprite+cat-shadow == 2 sprites

    // load sprites and sheets
    kids = texture( "spriteSheetExample.png", TEXTURE_LINEAR );
    catImage = texture( "cat.png", TEXTURE_LINEAR ); //
    shadowImage = texture( "cat-shadow.png", TEXTURE_LINEAR );
    inputs = texture( "prompts_tilemap_34x24_16x16x1.png", TEXTURE_LINEAR );

    // load all fx files, including subdirs
    // they can be tweaked from the engine UI settings directly
    fx_load("fx**.glsl");

    // init camera (x,y) (z = zoom)
    camera_t cam = camera();
    cam.position = vec3(app_width()/2,app_height()/2,1);
    camera_enable(&cam);

    while(app_swap() && !input_down(KEY_ESC)) {

        // camera panning (x,y) & zooming (z)
        if( !ui_hovered() && !ui_active() ) {
            if( input(MOUSE_L) ) cam.position.x -= input_diff(MOUSE_X);
            if( input(MOUSE_L) ) cam.position.y -= input_diff(MOUSE_Y);
            cam.position.z += input_diff(MOUSE_W) * 0.01; // cam.p.z += 0.001f; for tests
        }

        // apply post-fxs from here
        fx_begin();

            profile("Sprite batching") {
                if(do_cats) demo_cats(); else demo_kids();
            }

            // flush retained renderer, so we ensure the fbos are up to date before fx_end(0,0)
            profile("Sprite flushing") {
                sprite_flush();
            }

        // post-fxs end here
        fx_end(0,0);

        // draw pixel-art hud, 16x16 ui element, scaled and positioned in resolution-independant way
        {
            vec3 old_pos = camera_get_active()->position;

            sprite_flush();
            camera_get_active()->position = vec3(app_width()/2,app_height()/2,1);

            float zindex = app_height(); // large number, on top
            float spritesheet[3] = {17,34,24}, offset[2] = {0, - 2*absf(sin(app_time()*5))}; // sprite cell and animation
            float scale[2] = {3, 3}, tile_w = 16 * scale[0], tile_h = 16 * scale[1]; // scaling
            float position[3] = {app_width() - tile_w, app_height() - tile_h, zindex }; // position in screen-coordinates
            sprite_sheet(inputs, spritesheet, position, 0/*rotation*/, offset, scale, WHITE, SPRITE_RESOLUTION_INDEPENDANT);

            sprite_flush();
            camera_get_active()->position = old_pos;
        }

        if( ui_panel("Sprite", UI_OPEN) ) {
            const char *labels[] = {"Kids","Cats"};
            if( ui_list("Sprite type", &do_cats, countof(labels), labels) ) NUM_SPRITES_CHANGED = 1;
            if( ui_clampi("Number of Sprites", &NUM_SPRITES, 1, MAX_SPRITES) ) NUM_SPRITES_CHANGED = 1;
            if( ui_clampf("Zoom", &cam.position.z, 0.1, 10));
            ui_panel_end();
        }
    }

    return 0;
}
