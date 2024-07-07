// ----------------------------------------------------------------------------
// editor bindings

#if !CODE

typedef struct editor_binding_t {
    const char *command;
    const char *keys;
    void (*fn)();
} editor_binding_t;

API const char* editor_key(const char *cmd, const char *defaults); // find a binding key

#define EDITOR_FUNCTION(CMD,KEYS) \
    void CMD(); AUTORUN { array_push(editor_bindings, ((editor_binding_t){#CMD,editor_key(#CMD,KEYS),CMD}) ); } void CMD()

#define EDITOR_KEY(CMD,KEYS) \
    const char *CMD; AUTORUN { CMD = editor_key(#CMD, KEYS); }

API extern array(editor_binding_t) editor_bindings;

#else

#ifndef EDITOR_INI
#define EDITOR_INI ".editor.ini"
#endif

array(editor_binding_t) editor_bindings;

const char* editor_key(const char *key_, const char *defaults) {
    static ini_t inis; do_once inis = ini(EDITOR_INI);
    if( inis ) {
    char key[128]; snprintf(key, 128, "editor.%s", key_);
    char** found = map_find(inis, (char*)key);
    if(found) return *found;
    }
    ini_write(EDITOR_INI, "editor", key_, defaults);
    return defaults;
}

int editor_exec(const char *cmd, const char *val) {
    for each_array_ptr(editor_bindings,editor_binding_t,it) {
        if( it->command && !strcmp(it->command, cmd) ) {
            return it->fn(val), 1;
        }
    }
    return 0;
}

void editor_bindings_tick() {
    do_once
    for each_array_ptr(editor_bindings,editor_binding_t,it) {
        igAddCommandPalette(it->command, it->fn);
    }
    for each_array_ptr(editor_bindings,editor_binding_t,it) {
        if( it->keys && binding(it->keys) ) {
            it->fn();
        }
    }
}

AUTORUN {
    hooks("tick,exec", editor_bindings_tick,editor_exec);
}

#endif
