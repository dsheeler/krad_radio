#define KR_INFO_TYPES_COUNT 11
#define KR_META_INFO_MEMBERS_MAX 16
#define KR_META_MEMB_NAME_MAX 17
#define KR_META_STRUCT_NAME_MAX 25

typedef struct kr_info kr_info;
typedef struct kr_meta_info kr_meta_info;
typedef struct kr_meta_info_member kr_meta_info_member;

typedef enum {
  KR_JACK_INFO,
  KR_JACK_PATH_INFO,
  KR_MIXER_INFO,
  KR_MIXER_PATH_INFO,
  KR_EQ_BAND_INFO,
  KR_EQ_INFO,
  KR_LOWPASS_INFO,
  KR_HIGHPASS_INFO,
  KR_ANALOG_INFO,
  KR_TRANSPONDER_INFO,
  KR_TRANSPONDER_PATH_INFO
} kr_info_type;

struct kr_info {
  kr_info_type type;
  union {
    kr_jack_info jack_info;
    kr_jack_path_info jack_path_info;
    kr_mixer_info mixer_info;
    kr_mixer_path_info mixer_path_info;
    kr_eq_band_info eq_band_info;
    kr_eq_info eq_info;
    kr_lowpass_info lowpass_info;
    kr_highpass_info highpass_info;
    kr_analog_info analog_info;
    kr_transponder_info transponder_info;
    kr_transponder_path_info transponder_path_info;
  } info;
};

typedef enum {
  KR_META_INT32,
  KR_META_UINT32,
  KR_META_UINT64,
  KR_META_FLOAT,
  KR_META_CHAR,
  KR_META_STRING,
  KR_META_STRUCT,
  KR_META_ENUM 
} kr_member_type;

struct kr_meta_info_member {
  char name[KR_META_MEMB_NAME_MAX]; 
  kr_member_type type;
  kr_info_type struct_type;
  char struct_name[KR_META_STRUCT_NAME_MAX];
  union {
    int *integer;
    uint32_t *uint32;
    int64_t *integer64;
    float *real;
    char *ch;
    char *string[64];
  } value_ptr; 
  uint8_t array;
  uint32_t array_len;
 };

struct kr_meta_info {
  char name[KR_META_STRUCT_NAME_MAX]; 
  uint32_t members_count; 
  kr_info_type type;
  kr_info *info;
  uint8_t sub; 
  kr_meta_info_member members[KR_META_INFO_MEMBERS_MAX];
 };

static const kr_meta_info kr_jack_info_meta = {
  .name = "kr_jack_info", 
  .type = KR_JACK_INFO, 
  .members_count = 8,
  .members[0].name = "client_name",
  .members[0].type = KR_META_STRING
  .members[0].value_ptr = .info->kr_jack_info.client_name,
  .members[1].name = "server_name",
  .members[1].type = KR_META_STRING
  .members[1].value_ptr = .info->kr_jack_info.server_name,
  .members[2].name = "inputs",
  .members[2].type = KR_META_INT32
  .members[2].value_ptr = .info->kr_jack_info.inputs,
  .members[3].name = "outputs",
  .members[3].type = KR_META_INT32
  .members[3].value_ptr = .info->kr_jack_info.outputs,
  .members[4].name = "xruns",
  .members[4].type = KR_META_UINT32
  .members[4].value_ptr = .info->kr_jack_info.xruns,
  .members[5].name = "active",
  .members[5].type = KR_META_INT32
  .members[5].value_ptr = .info->kr_jack_info.active,
  .members[6].name = "sample_rate",
  .members[6].type = KR_META_UINT32
  .members[6].value_ptr = .info->kr_jack_info.sample_rate,
  .members[7].name = "period_size",
  .members[7].type = KR_META_UINT32
  .members[7].value_ptr = .info->kr_jack_info.period_size
};

static const kr_meta_info kr_jack_path_info_meta = {
  .name = "kr_jack_path_info", 
  .type = KR_JACK_PATH_INFO, 
  .members_count = 3,
  .members[0].name = "name",
  .members[0].type = KR_META_STRING
  .members[0].value_ptr = .info->kr_jack_path_info.name,
  .members[1].name = "channels",
  .members[1].type = KR_META_INT32
  .members[1].value_ptr = .info->kr_jack_path_info.channels,
  .members[2].name = "direction",
  .members[2].type = KR_META_ENUM
  .members[2].value_ptr = .info->kr_jack_path_info.direction
};

static const kr_meta_info kr_mixer_info_meta = {
  .name = "kr_mixer_info", 
  .type = KR_MIXER_INFO, 
  .members_count = 8,
  .members[0].name = "period_size",
  .members[0].type = KR_META_UINT32
  .members[0].value_ptr = .info->kr_mixer_info.period_size,
  .members[1].name = "sample_rate",
  .members[1].type = KR_META_UINT32
  .members[1].value_ptr = .info->kr_mixer_info.sample_rate,
  .members[2].name = "inputs",
  .members[2].type = KR_META_UINT32
  .members[2].value_ptr = .info->kr_mixer_info.inputs,
  .members[3].name = "buses",
  .members[3].type = KR_META_UINT32
  .members[3].value_ptr = .info->kr_mixer_info.buses,
  .members[4].name = "auxes",
  .members[4].type = KR_META_UINT32
  .members[4].value_ptr = .info->kr_mixer_info.auxes,
  .members[5].name = "frames",
  .members[5].type = KR_META_UINT64
  .members[5].value_ptr = .info->kr_mixer_info.frames,
  .members[6].name = "timecode",
  .members[6].type = KR_META_UINT64
  .members[6].value_ptr = .info->kr_mixer_info.timecode,
  .members[7].name = "clock",
  .members[7].type = KR_META_STRING
  .members[7].value_ptr = .info->kr_mixer_info.clock
};

static const kr_meta_info kr_mixer_path_info_meta = {
  .name = "kr_mixer_path_info", 
  .type = KR_MIXER_PATH_INFO, 
  .members_count = 16,
  .members[0].name = "name",
  .members[0].type = KR_META_STRING
  .members[0].value_ptr = .info->kr_mixer_path_info.name,
  .members[1].name = "bus",
  .members[1].type = KR_META_STRING
  .members[1].value_ptr = .info->kr_mixer_path_info.bus,
  .members[2].name = "crossfade_group",
  .members[2].type = KR_META_STRING
  .members[2].value_ptr = .info->kr_mixer_path_info.crossfade_group,
  .members[3].name = "channels",
  .members[3].type = KR_META_ENUM
  .members[3].value_ptr = .info->kr_mixer_path_info.channels,
  .members[4].name = "type",
  .members[4].type = KR_META_ENUM
  .members[4].value_ptr = .info->kr_mixer_path_info.type,
  .members[5].name = "fade",
  .members[5].type = KR_META_FLOAT
  .members[5].value_ptr = .info->kr_mixer_path_info.fade,
  .members[6].name = "volume",
  .members[6].type = KR_META_FLOAT
  .members[6].value_ptr = .info->kr_mixer_path_info.volume,
  .members[7].name = "map",
  .members[7].type = KR_META_INT32
  .members[7].value_ptr = .info->kr_mixer_path_info.map,
  .members[8].name = "mixmap",
  .members[8].type = KR_META_INT32
  .members[8].value_ptr = .info->kr_mixer_path_info.mixmap,
  .members[9].name = "rms",
  .members[9].type = KR_META_FLOAT
  .members[9].value_ptr = .info->kr_mixer_path_info.rms,
  .members[10].name = "peak",
  .members[10].type = KR_META_FLOAT
  .members[10].value_ptr = .info->kr_mixer_path_info.peak,
  .members[11].name = "delay",
  .members[11].type = KR_META_INT32
  .members[11].value_ptr = .info->kr_mixer_path_info.delay,
  .members[12].name = "lowpass",
  .members[12].type = KR_META_STRUCT,
  .members[12].struct_type = KR_LOWPASS_INFO,
  .members[13].name = "highpass",
  .members[13].type = KR_META_STRUCT,
  .members[13].struct_type = KR_HIGHPASS_INFO,
  .members[14].name = "analog",
  .members[14].type = KR_META_STRUCT,
  .members[14].struct_type = KR_ANALOG_INFO,
  .members[15].name = "eq",
  .members[15].type = KR_META_STRUCT,
  .members[15].struct_type = KR_EQ_INFO
};

static const kr_meta_info kr_eq_band_info_meta = {
  .name = "kr_eq_band_info", 
  .type = KR_EQ_BAND_INFO, 
  .members_count = 3,
  .members[0].name = "db",
  .members[0].type = KR_META_FLOAT
  .members[0].value_ptr = .info->kr_eq_band_info.db,
  .members[1].name = "bw",
  .members[1].type = KR_META_FLOAT
  .members[1].value_ptr = .info->kr_eq_band_info.bw,
  .members[2].name = "hz",
  .members[2].type = KR_META_FLOAT
  .members[2].value_ptr = .info->kr_eq_band_info.hz
};

static const kr_meta_info kr_eq_info_meta = {
  .name = "kr_eq_info", 
  .type = KR_EQ_INFO, 
  .members_count = 1,
  .members[0].name = "band",
  .members[0].type = KR_META_STRUCT,
  .members[0].struct_type = KR_EQ_BAND_INFO
};

static const kr_meta_info kr_lowpass_info_meta = {
  .name = "kr_lowpass_info", 
  .type = KR_LOWPASS_INFO, 
  .members_count = 2,
  .members[0].name = "bw",
  .members[0].type = KR_META_FLOAT
  .members[0].value_ptr = .info->kr_lowpass_info.bw,
  .members[1].name = "hz",
  .members[1].type = KR_META_FLOAT
  .members[1].value_ptr = .info->kr_lowpass_info.hz
};

static const kr_meta_info kr_highpass_info_meta = {
  .name = "kr_highpass_info", 
  .type = KR_HIGHPASS_INFO, 
  .members_count = 2,
  .members[0].name = "bw",
  .members[0].type = KR_META_FLOAT
  .members[0].value_ptr = .info->kr_highpass_info.bw,
  .members[1].name = "hz",
  .members[1].type = KR_META_FLOAT
  .members[1].value_ptr = .info->kr_highpass_info.hz
};

static const kr_meta_info kr_analog_info_meta = {
  .name = "kr_analog_info", 
  .type = KR_ANALOG_INFO, 
  .members_count = 2,
  .members[0].name = "drive",
  .members[0].type = KR_META_FLOAT
  .members[0].value_ptr = .info->kr_analog_info.drive,
  .members[1].name = "blend",
  .members[1].type = KR_META_FLOAT
  .members[1].value_ptr = .info->kr_analog_info.blend
};

static const kr_meta_info kr_transponder_info_meta = {
  .name = "kr_transponder_info", 
  .type = KR_TRANSPONDER_INFO, 
  .members_count = 1,
  .members[0].name = "active_paths",
  .members[0].type = KR_META_UINT32
  .members[0].value_ptr = .info->kr_transponder_info.active_paths
};

static const kr_meta_info kr_transponder_path_info_meta = {
  .name = "kr_transponder_path_info", 
  .type = KR_TRANSPONDER_PATH_INFO, 
  .members_count = 4,
  .members[0].name = "name",
  .members[0].type = KR_META_STRING
  .members[0].value_ptr = .info->kr_transponder_path_info.name,
  .members[1].name = "direction",
  .members[1].type = KR_META_ENUM
  .members[1].value_ptr = .info->kr_transponder_path_info.direction,
  .members[2].name = "adapter",
  .members[2].type = KR_META_ENUM
  .members[2].value_ptr = .info->kr_transponder_path_info.adapter,
  .members[3].name = "klydgeinfokludge",
  .members[3].type = KR_META_ENUM
  .members[3].value_ptr = .info->kr_transponder_path_info.klydgeinfokludge
};

static const kr_meta_info *kr_meta_info_structs[KR_INFO_TYPES_COUNT] = {
  &kr_jack_info_meta,
  &kr_jack_path_info_meta,
  &kr_mixer_info_meta,
  &kr_mixer_path_info_meta,
  &kr_eq_band_info_meta,
  &kr_eq_info_meta,
  &kr_lowpass_info_meta,
  &kr_highpass_info_meta,
  &kr_analog_info_meta,
  &kr_transponder_info_meta,
  &kr_transponder_path_info_meta
};

