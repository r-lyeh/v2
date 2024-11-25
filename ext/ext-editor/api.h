// -----------------------------------------------------------------------------
// in-game editor
// - rlyeh, public domain.

// current obj impl provides 19 available flags starting from bit-19
#define IS_SELECTED  (1ull<<0)
#define IS_OPEN      (1ull<<1)
//#define IS_CHANGED   (1ull<<2)
//#define IS_CACHED    (1ull<<X) // whether objects will be cached to ram
//#define IS_PERSIST   (1ull<<X) // whether objects will be saved to disk
//#define IS_BREAK     breakpoint: request to break on any given node
//#define IS_DEBUG     debug: request to monitor/isolate any specific node

#define HAS_EDITOR \
    ( optioni("--editor", flag("--editor")) || strstr(argvcmdline(), "editor") )

#include "editor_bindings2.h"
#include "editor_gizmo2.h"
#include "editor_pick2.h"
#include "editor_ui2.h"
#include "editor_tree2.h"
#include "editor_label2.h"
#include "editor_fps2.h"
#include "editor_prop2.h"
#include "editor_sub2.h"
#include "editor_sub_browser2.h"
#include "editor_sub_console2.h"
#include "editor_sub_text2.h"

#define editor_first_selected() 0

#include "editor_demotypes2.h"

API void igAssetsBrowser(unsigned library); // 0:all, 1:cache

