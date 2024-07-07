// -----------------------------------------------------------------------------
// in-game editor
// - rlyeh, public domain.

// ## Editor plan
// - editor = tree of nodes. levels and objects are nodes, and their widgets are also nodes
// - game is root folder and systems are the first initial top folders.
// - you can perform actions on folders or nodes, with or without descendants, top-bottom or bottom-top order.
// - these operations include load/save, undo/redo, reset, play/render, ddraw, etc
// - nodes are saved to disk as a filesystem layout: parents are folders, and leafs are files
// - network replication can be done by external tools by comparing the filesystems and by sending the resulting diff
//
// the api to handle the tree are modelled after shell commands: mv,cp,rm,rmrf, etc
// nodes in the tree can be attached to multiple parents
//

// @fixme: obj_destroy() + obj_parents

#define EDITOR_VERSION "2024.11"
#include "engine.h"

obj* root;
obj*  root_assets;
obj*  root_cache;
obj*  root_init;
obj*   root_draw;
obj*   root_tick;
obj*   root_edit;
obj*  root_quit;

AUTORUN {
    obj_setname(root = obj_new(obj), "/");
    obj_attach(root, obj_setname(root_assets = obj_new(obj), "assets/"));
    obj_attach(root, obj_setname(root_cache = obj_new(obj), "cache/"));
    obj_attach(root, obj_setname(root_init = obj_new(obj), "init/"));
    obj_attach(root, obj_setname(root_tick = obj_new(obj),  "tick/"));
    obj_attach(root, obj_setname(root_draw = obj_new(obj),  "draw/"));
    obj_attach(root, obj_setname(root_edit = obj_new(obj),  "edit/"));
    obj_attach(root, obj_setname(root_quit = obj_new(obj), "quit/"));
}


static set(obj*) watches;
void editor_watch(void* o) {
    if(!watches) set_init_ptr(watches);
    set_find_or_add(watches, (obj*)o);
}
void editor_attach(void *o) {
    if(obj_hasmethod(o, init)) obj_attach(root_init, o);
    if(obj_hasmethod(o, quit)) obj_attach(root_quit, o);
    if(obj_hasmethod(o, tick)) obj_attach(root_tick, o);
    if(obj_hasmethod(o, draw)) obj_attach(root_draw, o);
    if(obj_hasmethod(o, edit)) obj_attach(root_edit, o);
}

EDITOR_KEY(key_freecam, "held(RMB)");
EDITOR_KEY(key_freecam_slow, "held(SHIFT)");
//EDITOR_KEY(key_freecam_fov_in, "held(CTRL) & held(PLUS)");
//EDITOR_KEY(key_freecam_fov_out, "held(CTRL) & held(PLUS)");

EDITOR_KEY(key_select, "down(LMB)");
EDITOR_KEY(key_unselect, "held(CTRL) & down(D)");

EDITOR_FUNCTION(key_restart, "down(F5) && held(CTRL)") {
    app_send("restart", "");
}
EDITOR_FUNCTION(key_screenshot, "down(F12) | down(PRINT)") {
    app_send("screenshot", __FILE__ ".png");
}
EDITOR_FUNCTION(key_record, "held(ALT) & down(Z)") {
    app_send("record", __FILE__ ".mp4");
}

EDITOR_KEY(key_freecam_focus, "down(F)");
vec3 freecam_focus; bool freecam_focus_enabled; // position in locked view

set(int) selected;
AUTORUN { set_init_int(selected); } // @deprecated

EDITOR_FUNCTION(key_select_all, "held(CTRL) & (down(A)|down(E)) ; Select all entities in view") {
    if( set_isempty(selected) ) { // @fixme
        set_find_or_add(selected, 0);
        set_find_or_add(selected, 1);
    } else {
        set_clear(selected);
    }
}

// @todo: Ctrl-I invert selection

#if 0 // @todo
const char *key_freecam_hires = "held(RMB) & held(SHIFT)"; // x100 smaller increments
const char *key_freecam_pan = "held(ALT)";
const char *key_freecam_speed = "input(WMB)";

const char *key_toolbar1 = "down(Q)";
const char *key_toolbar2 = "down(W)";
const char *key_toolbar3 = "down(E)";
const char *key_toolbar4 = "down(R)";
const char *key_toolbar5 = "down(T)";
const char *key_toolbar6 = "down(Y)";
const char *key_focus = "down(F)";

EDITOR_FUNC(key_stop,       "") { app_pause(1); }
EDITOR_FUNC(key_mute,       "") { audio_volume_master( 1 ^ !!audio_volume_master(-1) ); }
EDITOR_FUNC(key_pause,      "") { app_pause( app_has_pause() ^ 1 ); }
EDITOR_FUNC(key_browser,    "") { ui_show("File Browser", ui_visible("File Browser") ^ true); }
EDITOR_FUNC(key_outliner,   "") { ui_show("Outliner", ui_visible("Outliner") ^ true); }
EDITOR_FUNC(key_record,     "") { if(record_active()) record_stop(), ui_notify(va("Video recorded"), date_string());
                                  else app_beep(), name = file_counter(va("%s.mp4",app_name())), app_record(name); }
EDITOR_FUNC(key_quit,       "") { record_stop(), exit(0); }
EDITOR_FUNC(key_screenshot, "") { char *name = file_counter(va("%s.png",app_name())), app_screenshot(name), ui_notify(va("Screenshot: %s", name), date_string()); }
EDITOR_FUNC(key_profiler,   "") { ui_show("Profiler", profiler_enable(ui_visible("Profiler") ^ true)); }
EDITOR_FUNC(key_fullscreen, "") { record_stop(), app_fullscreen( app_has_fullscreen() ^ 1 ); } // framebuffer resizing corrupts video stream, so stop any recording beforehand
EDITOR_FUNC(key_gamepad,    "") { *gamepadmask = (*gamepadmask & ~1u) | ((*gamepadmask & 1) ^ 1); }
EDITOR_FUNC(key_lit,        "") { *rendermask = (*rendermask & ~1u) | ((*rendermask & 1) ^ 1); }
EDITOR_FUNC(key_ddraw,      "") { *rendermask = (*rendermask & ~2u) | ((*rendermask & 2) ^ 2); }
#endif


typedef set(unsigned) list_all_t;

void list_all(list_all_t list, int count, model_t *models, camera_t *cam, vec2 from, vec2 to) {
    static pickbuffer_t pick; do_once pick = pickbuffer();

    pickbuffer_clear(&pick);

    fbo_bind(pick.fb.id);
        for(int i = 0; i < count; ++i) {
            model_render(models+i, cam->proj, cam->view, &models[i].pivot, 1, RENDER_PASS_PICK);
        }
    fbo_unbind();

    pickset_t sel = pickbuffer_pick(&pick, from, to);

    for each_set(sel, unsigned, value) {
        set_find_or_add(list, value);
    }
}


void editor_edit1(obj *o) {
    if( obj_name(o) )
    ui_separator(va("%s (%s)", obj_name(o), obj_type(o)));
    else
    ui_separator(va("%p (%s)", o, obj_type(o)));

    if( obj_hasmethod(o, edit) ) obj_edit(o);
    if( obj_hasmethod(o, menu) ) obj_menu(o);

    for each_objmember(o,TYPE,NAME,PTR) {
        igPushID_Ptr(PTR);

        if( !editor_changed(PTR) ) {
            obj_push(o);
        }

        int undo_clicked = 0;
        if( editor_changed(PTR) ) {
            undo_clicked = igSmallButton(UI_ICON(UNDO) " ");
            igSameLine(0,0);
        }

        char *label = va("%s", NAME);
        int changed = 0;
        /**/ if( !strcmp(TYPE,"float") )   changed = ui_float(label, PTR);
        else if( !strcmp(TYPE,"int") )     changed = ui_int(label, PTR);
        else if( !strcmp(TYPE,"vec2") )    changed = ui_float2(label, PTR);
        else if( !strcmp(TYPE,"vec3") )    changed = ui_float3(label, PTR);
        else if( !strcmp(TYPE,"vec4") )    changed = ui_float4(label, PTR);
        else if( !strcmp(TYPE,"rgb") )     changed = ui_color3(label, PTR);
        else if( !strcmp(TYPE,"rgba") )    changed = ui_color4(label, PTR);
        else if( !strcmp(TYPE,"color") )   changed = ui_color4f(label, PTR);
        else if( !strcmp(TYPE,"color3f") ) changed = ui_color3f(label, PTR);
        else if( !strcmp(TYPE,"color4f") ) changed = ui_color4f(label, PTR);
        else if( !strcmp(TYPE,"char*") )   changed = ui_string(label, PTR);
        else ui_label2(label, va("(%s)", TYPE)); // INFO instead of (TYPE)?
        if( changed ) {
            editor_setchanged(PTR, 1);
        }
        if( undo_clicked ) {
            editor_setchanged(PTR, 0);
        }
        if( !editor_changed(PTR) ) {
            obj_pop(o);
        }

        igPopID();
    }
}
void editor_edit_all(obj*o) {
    if( o )
    for each_objchild(o, obj*, oo) {
        if( oo && (obj_flag(oo,&,IS_SELECTED) /*|| obj_flag(oo,&,IS_OPEN)*/)) {
            editor_edit1(oo);
        }
        editor_edit_all(oo);
    }
}
void editor_tick_all(obj*o) {
    if( o )
    for each_objchild(o, obj*, oo) {
        if( obj_hasmethod(oo, tick) ) obj_tick(oo);
        editor_tick_all(oo);
    }
}
void editor_draw_all(obj*o) {
    if( o )
    for each_objchild(o, obj*, oo) {
        if( obj_hasmethod(oo, draw) ) obj_draw(oo);
        editor_draw_all(oo);
    }
}







model_t girl[2];


int editor( void (*game)(unsigned frame, float dt, double t) ) {

    do_once {
        // enable outlines
        fx_load("**/fx/editorOutline.glsl");
        fx_enable(fx_find("editorOutline.glsl"), 1);
        //
        editor_watch(root);
    }

    // camera
    static camera_t cam;
    do_once { cam = camera(); cam.damping = 1; }

    // fps camera
    bool active = ui_active() || ui_hovered() || gizmo_active() ? false : binding(key_freecam);
    app_cursor( !active );

    static int camera_mode;
    ui_radio("camera_mode", &camera_mode, 5, "fps","orbit","pan","fov","ortho");

    do_once active = 1;
    if( active ) { // clicked[>0] dragged[<0]
        vec2 mouse_sensitivity = vec2(0.1, -0.1); // sensitivity + polarity
        vec2 drag = mul2( vec2(input_diff(MOUSE_X), input_diff(MOUSE_Y)), mouse_sensitivity );

        if( camera_mode == 0 ) camera_fps(&cam, drag.x, drag.y); // camera_freefly(&cam, !active);
        if( camera_mode == 1 ) camera_orbit(&cam, drag.x, drag.y, 0); //len3(cam->position) );
        if( camera_mode == 2 ) camera_moveby(&cam, scale3(vec3(drag.x, drag.y, 0), 10)) ;
        if( camera_mode == 3 ) camera_fov(&cam, cam.fov += drag.y - drag.x);
        if( camera_mode == 4 ) cam.orthographic ^= 1, camera_fps(&cam, 0, 0);

        int div_speed = binding(key_freecam_slow) ? 0.1f : 1.0f;
        cam.speed = clampf(cam.speed + input_diff(MOUSE_W) / 10, 0.05f, 5.0f);
        vec3 wasdecq = scale3(vec3(input(KEY_D)-input(KEY_A),input(KEY_E)-(input(KEY_C)||input(KEY_Q)),input(KEY_W)-input(KEY_S)), cam.speed);
        cam.speed_buildup = !len3sq(wasdecq) ? 1.f : cam.speed_buildup + (cam.speed * cam.accel * div_speed * app_delta());
        camera_moveby(&cam, scale3(wasdecq, app_delta() * 60 * cam.speed_buildup * div_speed));
    }

#if 0
    // gamepad, fps camera
    if( input(GAMEPAD_CONNECTED) ) {
        vec2 filtered_lpad = input_filter_deadzone(input2(GAMEPAD_LPAD), 0.15f/*do_gamepad_deadzone*/ + 1e-3 );
        vec2 filtered_rpad = input_filter_deadzone(input2(GAMEPAD_RPAD), 0.15f/*do_gamepad_deadzone*/ + 1e-3 );
        vec2 mouse = scale2(vec2(filtered_rpad.x, filtered_rpad.y), 1.0f);
        vec3 wasdec = scale3(vec3(filtered_lpad.x, input(GAMEPAD_LT) - input(GAMEPAD_RT), filtered_lpad.y), 1.0f);
        camera_moveby(&cam, scale3(wasdec, app_delta() * 60));
        camera_fps(&cam, mouse.x,mouse.y);
        app_cursor( true );
        return;
    }
#endif

    // tick old demo objects
    static unsigned frame;
    static double t;
    static float dt;
    dt = 1.0/60;
    t += dt;
    if( game ) game(frame++, dt, t);

    // tick new demo objects
    camera_enable(&cam);
    editor_tick_all(root_tick);

    // user object selection
    static list_all_t list; do_once set_init_int(list); set_clear(list);
    // check that we're not moving a window + not in fps cam
    if( gizmo_hovered() || gizmo_active() || ui_active() ? 0 : !!mouse().cursor ) {
        // rect box
        if( 1 ) {
            static vec2 from = {0}, to = {0};
            if( input_down(MOUSE_L) ) to = vec2(input(MOUSE_X), input(MOUSE_Y)), from = to;
            if( input(MOUSE_L)      ) to = vec2(input(MOUSE_X), input(MOUSE_Y));
            if( len2sq(sub2(from,to)) > 0 ) {
                vec2 a = min2(from, to), b = max2(from, to);
                ddraw_push_2d();
                ddraw_color_push(YELLOW);
                ddraw_line( vec3(a.x,a.y,0),vec3(b.x-1,a.y,0) );
                ddraw_line( vec3(b.x,a.y,0),vec3(b.x,b.y-1,0) );
                ddraw_line( vec3(b.x,b.y,0),vec3(a.x-1,b.y,0) );
                ddraw_line( vec3(a.x,b.y,0),vec3(a.x,a.y-1,0) );
                ddraw_color_pop();
                ddraw_pop_2d();
            }
            if( input_up(MOUSE_L) ) {
                vec2 aa = min2(from, to), bb = max2(from, to);
                from = to = vec2(0,0);

                list_all(list, countof(girl), girl, &cam, aa, bb);
            }
        }
        // single click
        if( input_up(MOUSE_L) ) {
            mouse_t m = mouse();
            list_all(list, countof(girl), girl, &cam, vec2(m.x,m.y), vec2(m.x+1,m.y+1));
        }
        // single selection
        if( set_count(list) == 1 ) {
            for each_set(list,unsigned,id) {
                if( !id ) 
                    set_clear(selected);
                else
                for(int i = 0; i < countof(girl); ++i) {
                    vec4 color = girl[i].object_id;
                    if( id == rgbaf(color.x,color.y,color.z,color.w) ) {
                        id = i;
                        if( !input(KEY_CTRL) && !input(KEY_SHIFT) ) {
                            set_clear(selected);
                            set_find_or_add(selected, id);
                        }
                        else if( input(KEY_SHIFT) ) {
                            set_find_or_add(selected, id);
                        }
                        else {
                            if( set_find(selected, id) )
                                set_erase(selected, id);
                            else
                                set_find_or_add(selected, id);
                        }
                    }
                }
            }
        }
        // multi selection
        if( set_count(list) > 1 ) {
            if( !input(KEY_CTRL) && !input(KEY_SHIFT) ) {
                set_clear(selected);
            }

            for each_set(list,unsigned,id) {
                if( id ) 
                for(int i = 0; i < countof(girl); ++i) {
                    vec4 color = girl[i].object_id;
                    if( id == rgbaf(color.x,color.y,color.z,color.w) ) {
                        id = i;
                        if( !input(KEY_CTRL) ) {
                            set_find_or_add(selected, id);
                        }
                        else {
                            if( set_find(selected, id) )
                                set_erase(selected, id);
                            else
                                set_find_or_add(selected, id);
                        }
                    }
                }
            }
        }
    }

    // draw game scene
    editor_draw_all(root_draw);

    // draw editor world
    ddraw_ground(0);
    ddraw_flush();


    // collect matrices
    static array(float*) selected_matrices; array_resize(selected_matrices, 0);

    // draw object outlines + gizmo
    if( !set_isempty(selected) ) {
        // centroid of selection
        vec3 center = vec3(0,0,0);
        int ids = 0;

        // draw outline
        fx_begin();
        for each_set(selected, int, id) {
            // draw model outline
            editor_draw_all(root_draw);
#if 0 // @fixme
            // collect matrices
            array_push(selected_matrices, girl[id].pivot);
            // eval centroid
            center = add3(center, ptr3(&girl[id].pivot[12]));
#endif
            ++ids;
        }
        fx_end(0,0);

        freecam_focus = scale3(center, 1.f / ids);
    }

    // focus camera, lock on. @todo: reposition camera and enable orbit mode
    if( gizmo_hovered() || gizmo_active() || binding(key_freecam) || set_isempty(selected) || mouse().l ) freecam_focus_enabled = 0;
    if( binding(key_freecam_focus) ) freecam_focus_enabled = !freecam_focus_enabled;
    if( freecam_focus_enabled ) camera_lookat(&cam, freecam_focus);

    // draw gizmo(s)
    gizmo44( array_count(selected_matrices), selected_matrices );

    // inspect UI
    if( ui_panel("Inspector", UI_OPEN) ) {
        ui_separator("Game");
        for each_set(watches,obj*,it) {
            igPushID_Ptr(it);
            ui_editor_tree(it, obj_name(it));
            igPopID();
        }
        ui_separator("Dev Utils");
        ui_label2("Selected objects", va("%d", set_count(selected)));
        ui_label2("Gizmo hovered", va("%d", gizmo_hovered()));
        ui_label2("Gizmo active", va("%d", gizmo_active()));
        if( !set_isempty(selected) ) {
            ui_separator("Object(s)");
            for each_set(selected, int, id) ui_label2("Object id", va("%08x", id));
        }
        editor_edit_all(root_edit);
        ui_panel_end();
    }

    return 0;
}













void game_demo(unsigned frame, float dt, double t) {
    static kid *kid0;
    static kid *kid1;
    static kid *kid2;
    static camera_t cam;
    if( !frame ) {
        // init camera (x,y) (z = zoom)
        cam = camera();
        cam.position = vec3(app_width()/2,app_height()/2,1);

        kid0 = obj_make(
            "[kid]\n"
            "filename=spriteSheetExample.png\n"
            "pos=5,2\n"
            "angle=pi/12\n"
            "color=#ffcf\n"
        );
        kid1 = obj_make(
            "[kid]\n"
            "filename=spriteSheetExample.png\n"
            "pos=1,100\n"
            "angle=pi/12\n"
            "color=#fcc\n"
        );
        kid2 = obj_make(
            "[kid]\n"
            "filename=spriteSheetExample.png\n"
            "pos=50,200\n"
            "angle=pi/12\n"
            "color=#ccccffff\n"
        );

        obj_setname(kid0, "kid0");
        obj_setname(kid1, "kid1");
        obj_setname(kid2, "kid2");

        obj*lit3 = obj_make(
            "[lit]\n"
            "pos=300,300,0\n"
            "type=1"
        );
        obj*obj4 = obj_new_name(obj, "obj4");
        obj*obj5 = obj_new_name(obj, "obj5");

        obj_attach(root_edit, kid0);
        obj_attach(root_draw, kid0);
            obj_attach(kid0, kid1);
                obj_attach(kid1, kid2);
                    obj_attach(kid2, lit3);
                obj_attach(kid1, obj4);
            obj_attach(kid0, obj5);
    }

    camera_enable(&cam);

    // camera panning (x,y) & zooming (z)
    if(0)
    if( !ui_hovered() && !ui_active() ) {
        if( input(MOUSE_L) ) cam.position.x -= input_diff(MOUSE_X);
        if( input(MOUSE_L) ) cam.position.y -= input_diff(MOUSE_Y);
        cam.position.z += input_diff(MOUSE_W) * 0.1; // cam.p.z += 0.001f; for tests
    }

    // tick game
    if( dt ) {
        kid_tick(kid0, dt);
        kid_tick(kid1, dt);
        kid_tick(kid2, dt);

        kid0->angle = 5 * sin(t+dt);
    }

#if 0
    // draw world
    ddraw_ontop_push(0);
    ddraw_grid(0); // 1+10+100
      // ddraw_grid(1000);
    ddraw_ontop_pop();
    ddraw_flush();
#endif

    // draw game
    kid_draw(kid0);
    kid_draw(kid1);
    kid_draw(kid2);

    sprite_flush();
}

int main() {
    // create app (80% sized, no flags)
    app_create(80, APP_VIEWPORTS|APP_DOCKING);
    app_send("title", "Editor 2024.11");

    // animated models loading
    model2 *witch = obj_new_name(model2, "witch", "witch.gltf");
    { witch->mdl_.object_id = vec4(1,0,1,1); } // memcpy(&witch.mdl_.object_id, sizeof() , obj_id(model2));
    // editor_watch(witch);
    editor_attach(witch);

    // animated models loading
    model2 *kgirl = obj_new_name(model2, "kgirl", "kgirls01.fbx");
    { kgirl->mdl_.object_id = vec4(1,1,0,1); } // memcpy(&kgirl.mdl_.object_id, sizeof() , obj_id(model2));
    // editor_watch(kgirl);
    editor_attach(kgirl);

    // editor loop
    while( app_swap() ) {
        editor( game_demo );
    }
}
