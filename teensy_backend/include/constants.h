#ifndef CONSTANTS_H
#define CONSTANTS_H

#if defined(TARGET_AHPC)
#include "constants_ahpc.h"
#elif defined(TARGET_ALIVE)
#include "constants_alive.h"
#elif defined(TARGET_SPAIC)
#include "constants_spaic.h"
#elif defined(TARGET_TEXEL)
#include "constants_texel.h"
#else
#error "No known TARGET_* defined"
#endif

#endif