// ----------------------------------------------------------------------------
// editor properties

#if !CODE

API int ui_editor_tree(obj* root, const char *name);

#else

// aliases

float  WindowContentRegionAvailX;
float* FramePaddingX, FramePaddingX_Restore;
float* ButtonTextAlignX, ButtonTextAlignX_Restore;

int _ui_editor_tree(obj *o, int tabs, int numicons, const char *icons[], int xoffset) {
    int num_children = array_count(o->objchildren) - 1;
    const char *icon = obj_hasmethod(o, icon) ? obj_icon(o) : UI_ICON(DEPLOYED_CODE);
    const char *open = array_count(*obj_children(o)) <= 1 ? "  " : obj_flag(o, &, IS_OPEN) ? UI_ICON(KEYBOARD_ARROW_DOWN) : UI_ICON(KEYBOARD_ARROW_RIGHT);
    char icon_name[32]; snprintf(icon_name, 32, "%s%s %s%s", open, icon, obj_name(o), editor_changed(o)/*obj_flag(o,&,IS_CHANGED)*/ ? "*" : "");
    bool show = ui_filter && ui_filter[0] ? !!strstri(icon_name, ui_filter) : 1;

    igPushID_Ptr(o);

    if( show ) {
            bool b = obj_flag(o, &, IS_SELECTED);
            if( igCheckbox("##a",&b) ) {
                obj_flag(o, ^=, IS_SELECTED);
            }
            igSameLine(0,0);
                igDummy(ImVec2(1+12*tabs,0));
                igSameLine(0,0);
                if( igButton(icon_name,ImVec2(WindowContentRegionAvailX-12*tabs-xoffset,0)) ) {
                    if( input(KEY_SHIFT) )
                    obj_flag(o, |=, IS_SELECTED);
                    if( input(KEY_CTRL) )
                    obj_flag(o, ^=, IS_SELECTED);
                    else
                    if( num_children ) obj_flag(o, ^=, IS_OPEN);
                }
            *FramePaddingX = -2;
                for( int i = 0; i < numicons; ++i ) {
                    igSameLine(0,0);
                    if( i == (numicons-1) ) *FramePaddingX = FramePaddingX_Restore;
                    igSmallButton(icons[i]);
                }
            *FramePaddingX = FramePaddingX_Restore;

            if( b ) {
                obj_hasmethod(o,edit) && obj_edit(o);
            }
    }

    for each_objchild(o, obj*, oo) {
        if( obj_flag(o, &, IS_OPEN) ) {
            _ui_editor_tree(oo, tabs+1, numicons, icons, xoffset);
        }
    }

    igPopID();
    return 0;
}

int ui_editor_tree(obj *root, const char *name) {
    FramePaddingX = &igGetStyle()->FramePadding.x;
    FramePaddingX_Restore = *FramePaddingX;

    ImVec2 size; igGetContentRegionAvail(&size);
    WindowContentRegionAvailX = size.x;

    ButtonTextAlignX = &igGetStyle()->ButtonTextAlign.x;
    ButtonTextAlignX_Restore = *ButtonTextAlignX;
    *ButtonTextAlignX = 0;

    do_once obj_flag(root, |=, IS_OPEN);

//    UI_DETACHABLE_SECTION(name,
        const char *icons[] = {
            UI_ICON(PLAY_ARROW), UI_ICON(PAUSE), UI_ICON(STOP), "..." // FAST_FORWARD
//          UI_ICON(VISIBILITY), UI_ICON(LIGHT_MODE), "..."
        };
        _ui_editor_tree(root, 0, countof(icons), icons, countof(icons)*16+32 );
//    );

    *ButtonTextAlignX = ButtonTextAlignX_Restore;

    return 0;
}

#endif
