#ifndef SDL_ALIGNED
#ifdef _MSC_VER
#define SDL_ALIGNED(x) __declspec( align(x) )
#else
#define SDL_ALIGNED(x) __attribute__((aligned(x)))
#endif
#endif

#ifndef SDL_RESTRICT
#define SDL_RESTRICT __restrict
#endif

#ifndef SDL_PutAudioStreamDataNoCopy
#define SDL_PutAudioStreamDataNoCopy(s,bin,len,cb,cbarg) SDL_PutAudioStreamData(s,bin,len)
#endif

#define AL_LIBTYPE_STATIC
#include "mojoal.c"
