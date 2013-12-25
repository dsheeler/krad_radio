#include <stdio.h>
#include <stdint.h>
#include "gen.h"
#include "krad_ebmlx.h"
#include "krad_perspective.h"
int kr_pos_to_ebml(kr_ebml *ebml, void *st);
int kr_perspective_view_to_ebml(kr_ebml *ebml, void *st);
int kr_perspective_to_ebml(kr_ebml *ebml, void *st);
