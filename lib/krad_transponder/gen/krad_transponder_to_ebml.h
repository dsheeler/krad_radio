#include <stdio.h>
#include <stdint.h>
#include "gen.h"
#include "krad_ebmlx.h"
#include "krad_adapter_common.h"
#include <stdio.h>
#include <stdint.h>
#include "gen.h"
#include "krad_ebmlx.h"
#include "krad_transponder_common.h"
int kr_adapter_path_direction_to_ebml(kr_ebml *ebml, void *st);
int kr_adapter_api_to_ebml(kr_ebml *ebml, void *st);
int kr_adapter_api_info_to_ebml(kr_ebml *ebml, void *st);
int kr_adapter_api_path_info_to_ebml(kr_ebml *ebml, void *st);
int kr_adapter_info_to_ebml(kr_ebml *ebml, void *st);
int kr_adapter_path_info_to_ebml(kr_ebml *ebml, void *st);
int krad_link_av_mode_t_to_ebml(kr_ebml *ebml, void *st);
int kr_txpdr_su_type_t_to_ebml(kr_ebml *ebml, void *st);
int krad_link_transport_mode_t_to_ebml(kr_ebml *ebml, void *st);
int kr_transponder_path_io_type_to_ebml(kr_ebml *ebml, void *st);
int kr_transponder_info_to_ebml(kr_ebml *ebml, void *st);
int kr_transponder_path_io_path_info_to_ebml(kr_ebml *ebml, void *st);
int kr_transponder_path_io_info_to_ebml(kr_ebml *ebml, void *st);
int kr_transponder_path_info_to_ebml(kr_ebml *ebml, void *st);
