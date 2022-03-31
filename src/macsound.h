/*
 *	macsound.h
 */

#include "types.h"
#include "sound.h"

void mac_sound_play(int32 id, int32 volume, Fixed pitch, int32 pan);
void mac_sound_free();
