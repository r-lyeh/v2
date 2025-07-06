// io utilities,
// - rlyeh, public domain

#if !CODE

#define DIR_SEP '/'

#if   defined MAX_PATH
#define DIR_MAX MAX_PATH
#elif defined PATH_MAX
#define DIR_MAX PATH_MAX
#else
#define DIR_MAX 260
#endif

API FILE * file_handle( const char *pathfile );
API char*  file_read(const char *pathfile, int *size); // do not free() after use
API int    file_size(const char *pathfile); // @fixme: sub-optimal
API int    file_write(const char *pathfile, const void *blob, int len);
API bool   file_append( const char *file, const void *ptr, int len );
API char** file_list(const char *pathmasks, int *count); // do not FREE(). semi-colon separated list of files and masks. recursive with `**` mask
API const char *file_find(const char *pathfile);

API int    is_file(const char *pathfile); // @fixme: handle embed/cook cases
API int    is_folder(const char *pathfile); // @fixme: handle embed/cook cases

API const char *file_path( const char *pathfile ); // c:/prj/file.txt > c:/prj/
API const char *file_name( const char *pathfile ); // c:/prj/file.txt > file.txt
API const char *file_ext ( const char *pathfile ); // c:/prj/file.txt > .txt
API const char *file_norm( const char *pathfile ); // converts to native slashes and quotes

#else

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <sys/stat.h>

#if 0
static
const char *file_find_recursively(const char *dir, const char *name) { // file.txt > ./art/dir/file.txt
    // @fixme: optimize me
    int count;
    char **list = SDL_GlobDirectory(dir, "*", SDL_GLOB_CASEINSENSITIVE, &count);
    if( list ) {
        for( int i = 0; i < count; ++i ) {
            char built[DIR_MAX];
            snprintf(built, DIR_MAX, "%s/%s", dir, list[i]);

            if( is_file(built) ) {
                const char *match = strendi(name, list[i]);
                if( match ? match == name || (match > name && match[-1] == '/') : false ) {
                    return SDL_free(list), va("%s", built);
                }
            } else if( list[i][0] != '.' ) {
                const char *result = file_find_recursively(built, name);
                if( result ) {
                    return SDL_free(list), va("%s", result);
                }
            }
        }
        SDL_free(list);
    }

    return NULL;
}
#else
#include "3rd/3rd_archive_dir.h"
#include "3rd/3rd_archive_zip.h"

struct zip_t *cook_zip;

static
const char *file_find_recursively(const char *dir, const char *name) { // file.txt > ./art/dir/file.txt
    static map(char*,char*) lookup;
    do_once {
        map_init_str(lookup);

        struct dir_t *d = 0;
        struct zip_t *z = 0;
        z = z ? z : zip_open("cook.zip", "rb");
        z = z ? z : zip_open(".art[00].zip", "rb");
        z = z ? z : zip_open(va("%s.zip", app_name()), "rb");
        if( z ) {
            cook_zip = z;
            for( int i = 0; i < zip_count(z); ++i ) {
                if( zip_file(z,i) ) {
                    // zip_name() should return full normalized paths "C:/project/demos/art/fx/fxBloom.fs". should exclude system dirs as well
                    const char *entry = zip_name(z,i);
                    char *copy = STRDUP(entry);
                    char *stem = STRDUP(strlower(entry));

                    while( *stem ) {
                        map_insert(lookup, stem, copy);

                        stem = strchr(stem, '/');
                        if( !stem ) break;
                        stem++;
                    }
                }
            }
            // zip_close(z);
        }
        else d = dir_open("./", "r"); // (r)ecurse

        if( d ) {
            for( int i = 0; i < dir_count(d); ++i ) {
                if( dir_file(d,i) ) {
                    // dir_name() should return full normalized paths "C:/project/demos/art/fx/fxBloom.fs". should exclude system dirs as well
                    const char *entry = dir_name(d,i);
                    char *copy = STRDUP(entry);
                    char *stem = STRDUP(strlower(entry));

                    while( *stem ) {
                        map_insert(lookup, stem, copy);

                        stem = strchr(stem, '/');
                        if( !stem ) break;
                        stem++;
                    }
                }
            }
            dir_close(d);
        }
    }
    char *norm = strlower(name);
    strswap(norm, "\\", "/");
    strswap(norm, "//", "/");
    while(*norm == '/') ++norm;
//    while(strchr(norm, '\\')) *strchr(norm, '\\') = '/'; // replace(norm, '\\', '/');
    char **found = map_find(lookup, (char*)norm);
    if(DEV) printf("%s -> %s -> %s\n", name, norm, found ? *found : "not found");
    return found ? *found : name;
}
#endif

const char *file_find(const char *pathfile) { // file.txt > ./art/dir/file.txt
    if( is_file(pathfile) ) {
        return pathfile;
    }
    return file_find_recursively(".", pathfile);
}

const char *file_name( const char *pathfile ) { // c:/prj/file.txt > file.txt
    const char *a = strrchr(pathfile, '/');  a += !!a;
    const char *b = strrchr(pathfile, '\\'); b += !!b;
    return a > b ? a : b > a ? b : pathfile;
}
const char *file_path( const char *pathfile ) { // c:/prj/file.txt > c:/prj/
    const char *name = file_name(pathfile);
    return va("%.*s", (int)(strlen(pathfile) - strlen(name)), pathfile );
}
const char *file_ext(const char *pathfile) { // c:/prj/file.txt > .txt
    const char *name = file_name(pathfile), *dot = strchr(name, '.');
    return dot ? dot : (const char*)va("");
}

FILE *file_handle(const char *pathfile) {
    const char *solved = file_find(pathfile);
    if( !solved ) return NULL;

    if( cook_zip ) {
        return (FILE*)solved; // @fixme
    }

    // process/cook it if needed
    pathfile = cook( solved );
    return fopen( pathfile, "rb" );
}

static
char *file_slot(unsigned len) {
    enum { NUM_SLOTS = 1024 }; // @fixme: use block of mem similar to how va() works
    static __thread char* slot[NUM_SLOTS] = {0};
    static __thread int index = 0; index = (index+1) % NUM_SLOTS;
    return slot[index] = REALLOC( slot[index], len );
}

char *file_read(const char *pathfile, int *size) { // do not free() after use
    FILE *fp = file_handle(pathfile);
    if( size ) *size = 0;

#if 1 // COOK.ZIP
    if( fp && cook_zip ) {
        for( int found = zip_find(cook_zip, (const char*)fp); found >= 0; ) {
            int len = zip_size(cook_zip, found);
            if( size ) *size = len;
            char *bin = file_slot(len + 256); // EXCESS(256)
            zip_extract_inplace(cook_zip, found, bin, len + 256); // EXCESS(256)
            return bin[len] = '\0', bin;
        }
        fp = NULL;
    }
#endif

    if( fp ) {
        fseek(fp, 0L, SEEK_END);
        size_t len = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        if(size) *size = (int)len;

        char *bin = file_slot(len+1);

        if( fread(bin, 1, len, fp) != len ) return fclose(fp), NULL;

        return fclose(fp), bin[len] = '\0', bin;
    }

#if HAS_EMBED
    // try embedded resources as fallback
    for( int i = 0; i < countof(resources); ++i ) {
        if( resources[i].data && strendi(resources[i].name, pathfile) ) {
            printf("found embedded resource: %s (%d bytes)\n", resources[i].name, resources[i].size);
            if( size ) *size = resources[i].size;
            return (char*)resources[i].data; // @fixme: remove cast. should return a (char*)COPY of such data, and must re-use the slot[] logics below
        }
    }
#endif

    PRINTF("cannot find file (%s)\n", pathfile);
    return NULL;
}
int file_write(const char *pathfile, const void *blob, int len) {
    int ok = 0;
    FILE *fp = fopen(pathfile, "wb");
    if( fp ) {
        ok = fwrite(blob, len, 1, fp) == 1;
        fclose(fp);
    }
    return ok;
}
bool file_append(const char *name, const void *ptr, int len) {
    bool ok = 0;
    for( FILE *fp = name && ptr && len >= 0 ? fopen(name, "a+b") : NULL; fp; fclose(fp), fp = 0) {
        ok = fwrite(ptr, len,1, fp) == 1;
    }
    return ok;
}

int is_file( const char *pathfile ) {
    struct stat st;
    return stat(pathfile, &st) >= 0 ? S_IFDIR != ( st.st_mode & S_IFMT ) : 0;
}
int is_folder( const char *pathfile ) {
    // @fixme: win32+tcc wont like ending slashes in stat()
    struct stat st;
    return stat(pathfile, &st) >= 0 ? S_IFDIR == ( st.st_mode & S_IFMT ) : 0;
}

const char *file_norm(const char *pathfile) {
    return ifdef(win32,
            strswap(strswap(va("%s", pathfile), "/", "\\"), "'", "\""),
            strswap(strswap(va("%s", pathfile), "\\", "/"), "\"", "'"));
}

char** file_list(const char *pathmasks, int *count) {
    static __thread array(char*) list = 0; // @fixme: add 16 slots

    for( int i = 0; i < array_count(list); ++i ) {
        FREE(list[i]);
    }
    array_resize(list, 0);

    for each_substring(pathmasks,";",pathmask) {
        const char *cwd = 0, *masks = 0;
#if 0
            char *slash = strrchr(pathmask, '/');
            if( !slash ) cwd = "./", masks = pathmask;
            else {
                masks = va("%s", slash+1);
                cwd = pathmask, slash[1] = '\0';
            }
            if( !masks[0] ) masks = "*";

        ASSERT(strend(cwd, "/"), "Error: dirs like '%s' must end with slash", cwd);

        int recurse = strstr(cwd, "**") || strstr(masks, "**");
        strswap(cwd, "**", "./");
#else
        cwd = "./";

        if(strchr("*.", pathmask[0])) masks = pathmask;
        else if(pathmask[0] == '/') masks = va("*%s", pathmask);
        else masks = va("*/%s", pathmask);

        int recurse = !!strstr(pathmask, "**");
#endif

        dir_t *d = 0;

#if 1 // COOK.ZIP
        if( cook_zip ) {
            for( int i = 0, end = zip_count(cook_zip); i < end; ++i ) {
                // zip_name() should return full normalized paths "C:/project/demos/art/fx/fxBloom.fs". should exclude system dirs as well
                const char *entry = zip_name(cook_zip, i);

                int allowed = 0;
                for each_substring(masks,";",mask) {
                    allowed |= strmatch(entry/*fname*/, mask);
                }
                if( !allowed ) continue;

                // insert copy
                char *copy = STRDUP(entry);
                array_push(list, copy);
            }
        }
        else
#endif

        d = dir_open(cwd, recurse ? "r" : "");
        if( d ) {
            for( int i = 0; i < dir_count(d); ++i ) {
                if( dir_file(d,i) ) {
                    // dir_name() should return full normalized paths "C:/project/demos/art/fx/fxBloom.fs". should exclude system dirs as well
                    const char *entry = dir_name(d,i);

                    int allowed = 0;
                    for each_substring(masks,";",mask) {
                        allowed |= strmatch(entry/*fname*/, mask);
                    }
                    if( !allowed ) continue;

                    // insert copy
                    char *copy = STRDUP(entry);
                    array_push(list, copy);
                }
            }
            dir_close(d);
        }
    }

    array_sort(list, strcmp);
    if(count) *count = array_count(list);
    return list;
}

int file_size(const char *pathfile) { // @fixme: suboptimal
    int len; char *bin = file_read(pathfile, &len);
    return len;
}

#endif
