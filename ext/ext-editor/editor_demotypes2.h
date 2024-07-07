// ----------------------------------------------------------------------------
// demo

// @fixme: legacy code uses 'menu' to handle UI
// @fixme: legacy code uses 'edit' to manipulate as physical object
// @fixme: legacy code uses 'aabb' to measure bounding boxes
// @fixme: legacy code uses 'icon' to retrieve class icon
// @fixme: allow annotations in STRUCT() members, like shaders: /// set:1,1,0,1 max:1,1,1,1 min:-1,-1,-1,-1 tip:"blabla"

#if !CODE

typedef struct lit { OBJ
    vec3 pos;
    vec3 dir;
    int type;
} lit;

typedef struct kid { OBJ
    int kid;
    vec2 pos;
    vec2 vel;
    float angle;
    unsigned color;
    int controllerid;

    // --- private
    char *filename;
    texture_t texture_;
} kid;

typedef struct model2 { OBJ
    const char *filename_;
    model_t mdl_;
} model2;

OBJTYPEDEF(model2,199);

#else

void model2_ctor(model2 *obj) {
    obj->mdl_ = model(obj->filename_, 0);
    compose44(obj->mdl_.pivot, vec3(0,0,0), eulerq(vec3(0,0,0)), vec3(2,2,2)); // position, rotation, scale
}
const char *model2_icon(model2 *obj) {
    return UI_ICON(GAMEPAD);
}
int model2_edit(model2 *obj) {
    //if( obj_flag(obj,&,IS_SELECTED) )
    return 1;
}
int model2_menu(model2 *obj) {
    //if( obj_flag(obj,&,IS_SELECTED) )
    {
        ui_mat44("xform", obj->mdl_.pivot);
        editor_label3(ptr3(obj->mdl_.pivot+12), obj_name(obj));
        float *list[] = { &obj->mdl_.pivot[0] };
        gizmo44(1, list); //ui_label2("obj2", obj_name(obj));
    }
    return 1;
}
void model2_tick(model2 *obj) {
    float delta = 30 * app_delta(); // @fixme: supply this via arg instead
    obj->mdl_.curframe = model_animate(obj->mdl_, obj->mdl_.curframe + delta);
}
void model2_draw(model2 *obj) {
    camera_t *cam = camera_get_active(); // @fixme: supply this via proj[16],view[16] args instead
    model_render(&obj->mdl_, cam->proj, cam->view, &obj->mdl_.pivot, 1, -1);
}
AUTORUN {
    EXTEND(model2, ctor,icon,edit,draw,tick,menu);
}





int lit_aabb(lit *obj, aabb *box) {
    *box = aabb( vec3(obj->pos.x-16,obj->pos.y-16,0), vec3(obj->pos.x+16,obj->pos.y+16,1) );
    return 1;
}
const char *lit_icon(lit *obj) {
    const char *icon =
        obj->type == 0 ? UI_ICON(WB_IRIDESCENT) :
        obj->type == 1 ? UI_ICON(WB_INCANDESCENT) :
        obj->type == 2 ? UI_ICON(FLARE) :
        obj->type == 3 ? UI_ICON(WB_SUNNY) : "";
    return icon;
}
int lit_edit(lit *obj) {
    const char *all_icons =
        UI_ICON(WB_IRIDESCENT)
        UI_ICON(WB_INCANDESCENT)
        UI_ICON(FLARE)
        UI_ICON(WB_SUNNY)

        UI_ICON(LIGHT_MODE)
        UI_ICON(LIGHT)

        UI_ICON(FLASHLIGHT_OFF)
        UI_ICON(FLASHLIGHT_ON)
        UI_ICON(HIGHLIGHT)
        UI_ICON(HIGHLIGHT_ALT)
        UI_ICON(LIGHTBULB)
        UI_ICON(LIGHTBULB_OUTLINE)
        UI_ICON(NIGHTLIGHT)
        UI_ICON(NIGHTLIGHT_ROUND)

        UI_ICON(WALL_LAMP)           //
        UI_ICON(SUNNY)               // directional
    ;
    // editor_label2(vec2(obj->pos.x+16,obj->pos.y-32),all_icons);
    if( obj_flag(obj,&,IS_SELECTED) ) {
    obj->pos.x += input(KEY_RIGHT) - input(KEY_LEFT);
    obj->pos.y += input(KEY_DOWN) - input(KEY_UP);
    obj->type = (obj->type + !!input_down(KEY_SPACE)) % 4;
    }
    editor_label2(vec2(obj->pos.x,obj->pos.y),lit_icon(obj));

    return 1;
}

OBJTYPEDEF(lit,200);

AUTORUN {
    STRUCT(lit, vec3, pos);
    STRUCT(lit, vec3, dir);
    STRUCT(lit, int, type);
    EXTEND(lit, edit,icon,aabb);
}






void kid_ctor(kid *obj) {
    obj->kid = randi(0,3);
    obj->pos = vec2(randi(0, window_width()), randi(0, window_height()));
    obj->vel.x = obj->vel.y = 100 + 200 * randf();
    obj->controllerid = randi(0,3);

    obj->texture_ = texture(obj->filename, TEXTURE_RGBA|TEXTURE_LINEAR);
}
void kid_tick(kid *obj, float dt) {
    // add velocity to position
    vec2 off = vec2( input(KEY_RIGHT)-input(KEY_LEFT), input(KEY_DOWN)-input(KEY_UP) );
    obj->pos = add2(obj->pos, scale2(mul2(obj->vel, off), dt * (obj->controllerid == 0)));

    // wrap at screen boundaries
    const int appw = window_width(), apph = window_height();
    if( obj->pos.x < 0 ) obj->pos.x += appw; else if( obj->pos.x > appw ) obj->pos.x -= appw;
    if( obj->pos.y < 0 ) obj->pos.y += apph; else if( obj->pos.y > apph ) obj->pos.y -= apph;
}
void kid_draw(kid *obj) {
    // 4x4 tilesheet
    int col = (((int)obj->kid) % 4);
    int row = (((int)obj->pos.x / 10 ^ (int)obj->pos.y / 10) % 4);
    float position[3] = {obj->pos.x,obj->pos.y,obj->pos.y}; // position(x,y,depth: sort by Y)
    float offset[2]={0,0}, scale[2]={1,1};
    float coords[3]={col * 4 + row,4,4}; // num_frame(x) in a 4x4(y,z) spritesheet
    unsigned flags = 0;
    sprite_sheet(obj->texture_, coords, position, obj->angle*TO_DEG, offset, scale, obj->color, flags);
}
int kid_aabb(kid *obj, aabb *box) {
    *box = aabb( vec3(obj->pos.x-16,obj->pos.y-16,0), vec3(obj->pos.x+16,obj->pos.y+16,1) );
    return 1;
}
int kid_edit(kid *obj) {
    aabb box;
    if( kid_aabb(obj, &box) ) {
        ddraw_color_push(YELLOW);
        ddraw_push_2d();
        ddraw_aabb(box.min, box.max);
        ddraw_pop_2d();
        ddraw_color_pop();
    }
    if( obj_flag(obj,&,IS_SELECTED) ) {
        obj->pos.x += input(KEY_RIGHT) - input(KEY_LEFT);
        obj->pos.y += input(KEY_DOWN) - input(KEY_UP);

        editor_label2(vec2(obj->pos.x+16,obj->pos.y-16),UI_ICON(VIDEOGAME_ASSET));
    }
    return 1;
}
void kid_menu(kid *obj, const char *argv) {
    if( ui_section("Private section") ) {
        ui_color4("Color_", &obj->color);
        ui_texture("Texture_", obj->texture_);
    }
}
const char* kid_icon(kid *obj) {
    return UI_ICON(IMAGE); // ACCOUNT_BOX);
}

OBJTYPEDEF(kid,201);

AUTORUN {
    // reflect
    STRUCT(kid, int, kid);
    STRUCT(kid, vec2, pos);
    STRUCT(kid, vec2, vel);
    STRUCT(kid, float, angle, "Tilt degrees");
    STRUCT(kid, rgba,  color, "Tint color");
    STRUCT(kid, char*, filename, "Filename" );
    EXTEND(kid, ctor,tick,draw,aabb,edit,menu,icon);
}

#endif
