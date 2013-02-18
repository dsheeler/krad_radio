use core::libc::*;
pub type Struct_kr_client_St = c_void;
pub type kr_client_t = Struct_kr_client_St;
pub type Struct_kr_shm_St = c_void;
pub type kr_shm_t = Struct_kr_shm_St;
pub type kr_response_t = Struct_kr_crate_St;
pub type kr_crate_t = Struct_kr_crate_St;
pub type kr_unit_path_t = Struct_kr_unit_path_St;
pub type kr_unit_control_t = Struct_kr_unit_control_St;
pub type kr_address_t = Struct_kr_address_St;
pub type kr_unit_t = c_uint;
pub const KR_STATION: c_uint = 1;
pub const KR_MIXER: c_uint = 2;
pub const KR_COMPOSITOR: c_uint = 3;
pub const KR_TRANSPONDER: c_uint = 4;
pub type kr_mixer_subunit_t = c_uint;
pub const KR_PORTGROUP: c_uint = 1;
pub const KR_EFFECT: c_uint = 2;
pub type kr_transponder_subunit_t = c_uint;
pub const KR_TRANSMITTER: c_uint = 1;
pub const KR_RECEIVER: c_uint = 2;
pub const KR_DEMUXER: c_uint = 3;
pub const KR_MUXER: c_uint = 4;
pub const KR_ENCODER: c_uint = 5;
pub const KR_DECODER: c_uint = 6;
pub const KR_ADAPTER: c_uint = 7;
pub type kr_station_subunit_t = c_uint;
pub const KR_STATION_UNIT: c_uint = 1;
pub const KR_CPU: c_uint = 2;
pub const KR_REMOTE: c_uint = 3;
pub type kr_unit_control_data_t = c_uint;
pub const KR_FLOAT: c_uint = 0;
pub const KR_INT32: c_uint = 1;
pub const KR_STRING: c_uint = 2;
pub struct kr_unit_control_value_t {
    pub integer: c_int,
    pub string: *c_schar,
    pub real: c_float,
}
pub type kr_transponder_control_t = c_uint;
pub const KR_BUFFER: c_uint = 1;
pub const KR_BITRATE: c_uint = 2;
pub struct kr_subunit_t {
    pub ptr: *c_void,
    pub zero: uint64_t,
    pub mixer_subunit: kr_mixer_subunit_t,
    pub compositor_subunit: kr_compositor_subunit_t,
    pub transponder_subunit: kr_transponder_subunit_t,
    pub station_subunit: kr_station_subunit_t,
}
pub struct kr_unit_control_name_t {
    pub portgroup_control: kr_mixer_portgroup_control_t,
    pub effect_control: kr_mixer_effect_control_t,
    pub compositor_control: kr_compositor_control_t,
    pub transponder_control: kr_transponder_control_t,
}
pub struct Struct_kr_unit_path_St {
    pub unit: kr_unit_t,
    pub subunit: kr_subunit_t,
}
pub struct kr_unit_id_t {
    pub number: c_int,
    pub name: [c_schar * 64],
}
pub struct Struct_kr_address_St {
    pub path: kr_unit_path_t,
    pub control: kr_unit_control_name_t,
    pub id: kr_unit_id_t,
    pub sub_id: c_int,
    pub sub_id2: c_int,
}
pub struct Struct_kr_unit_control_St {
    pub address: kr_address_t,
    pub data_type: kr_unit_control_data_t,
    pub value: kr_unit_control_value_t,
    pub duration: c_int,
}
pub struct kr_rep_ptr_t {
    pub actual: *c_void,
    pub tag: *kr_tag_t,
    pub remote: *kr_remote_t,
    pub mixer: *kr_mixer_t,
    pub portgroup: *kr_mixer_portgroup_t,
    pub compositor: *kr_compositor_t,
    pub radio: *kr_radio_t,
}
pub struct kr_rep_actual_t {
    pub actual: c_schar,
    pub tag: kr_tag_t,
    pub remote: kr_remote_t,
    pub mixer: kr_mixer_t,
    pub portgroup: kr_mixer_portgroup_t,
    pub compositor: kr_compositor_t,
    pub radio: kr_radio_t,
}
pub struct Struct_kr_crate_St {
    pub kr_client: *kr_client_t,
    pub addr: *kr_address_t,
    pub address: kr_address_t,
    pub inside: kr_rep_ptr_t,
    pub rep: kr_rep_actual_t,
    pub contains: uint32_t,
    pub notice: uint32_t,
    pub buffer: *c_uchar,
    pub size: uint32_t,
    pub integer: c_int,
    pub real: c_float,
    pub has_int: c_int,
    pub has_float: c_int,
}
#[link_args = "-lkradradio_client"]
pub extern "C" {
    fn kr_unit_control_data_type_from_address(++address: *kr_address_t,
                                              ++data_type:
                                                  *kr_unit_control_data_t) ->
     c_int;
    fn kr_response_get_event(++response: *kr_response_t) -> uint32_t;
    fn kr_string_to_address(++string: *c_schar, ++addr: *kr_address_t) ->
     c_int;
    fn kr_unit_control_set(++client: *kr_client_t, ++uc: *kr_unit_control_t)
     -> c_int;
    fn kr_unit_destroy(++client: *kr_client_t, ++address: *kr_address_t);

    fn kr_client_create(++client_name: *c_schar) -> *kr_client_t;
    fn kr_connect(++client: *kr_client_t, ++sysname: *c_schar) -> c_int;
    fn kr_connect_remote(++client: *kr_client_t, ++host: *c_schar,
                         ++port: c_int) -> c_int;
    fn kr_connected(++client: *kr_client_t) -> c_int;
    fn kr_disconnect(++client: *kr_client_t) -> c_int;
    fn kr_client_destroy(++client: **kr_client_t) -> c_int;
    fn kr_client_local(++client: *kr_client_t) -> c_int;
    fn kr_client_get_fd(++client: *kr_client_t) -> c_int;
    fn kr_broadcast_subscribe(++client: *kr_client_t,
                              ++broadcast_id: uint32_t);
    fn kr_subscribe_all(++client: *kr_client_t);
    fn kr_poll(++client: *kr_client_t, ++timeout_ms: uint32_t) -> c_int;

    fn kr_response_free(++kr_response: **kr_response_t);
    fn kr_response_to_string(++kr_response: *kr_response_t,
                             ++string: **c_schar) -> c_int;
    fn kr_response_to_int(++response: *kr_response_t, ++number: *c_int) ->
     c_int;
    fn kr_response_to_float(++response: *kr_response_t, ++number: *c_float) ->
     c_int;
    fn kr_response_alloc_string(++length: c_int) -> *c_schar;
    fn kr_response_free_string(++string: **c_schar);

    fn kr_response_to_rep(++crate: *kr_crate_t) -> c_int;
    fn kr_response_address(++response: *kr_response_t,
                           ++address: **kr_address_t);
    fn kr_address_debug_print(++addr: *kr_address_t);
    fn krad_message_notice_has_payload(++_type: uint32_t) -> c_int;

    fn kr_response_size(++kr_response: *kr_response_t) -> uint32_t;
    fn kr_client_response_wait_print(++client: *kr_client_t);

    fn kr_client_response_get(++client: *kr_client_t,
                              ++kr_response: **kr_response_t);
    fn kr_crate_loaded(++crate: *kr_crate_t) -> c_int;
    fn kr_crate_has_int(++crate: *kr_crate_t) -> c_int;
    fn kr_crate_has_float(++crate: *kr_crate_t) -> c_int;
    fn kr_delivery_wait_until_final(++client: *kr_client_t,
                                    ++timeout_ms: uint32_t) -> c_int;
    fn kr_delivery_final(++client: *kr_client_t) -> c_int;
    fn kr_client_response_wait(++client: *kr_client_t,
                               ++kr_response: **kr_response_t);
    fn kr_shm_create(++client: *kr_client_t) -> *kr_shm_t;
    fn kr_shm_destroy(++kr_shm: *kr_shm_t);
    fn kr_system_info(++client: *kr_client_t);
    fn kr_set_dir(++client: *kr_client_t, ++dir: *c_schar);
    fn kr_remote_list(++client: *kr_client_t);
    fn kr_remote_on(++client: *kr_client_t, ++interface: *c_schar,
                    ++port: c_int) -> c_int;
    fn kr_remote_off(++client: *kr_client_t, ++interface: *c_schar,
                     ++port: c_int) -> c_int;
    fn kr_web_enable(++client: *kr_client_t, ++http_port: c_int,
                     ++websocket_port: c_int, ++headcode: *c_schar,
                     ++header: *c_schar, ++footer: *c_schar);
    fn kr_web_disable(++client: *kr_client_t);
    fn kr_osc_enable(++client: *kr_client_t, ++port: c_int);
    fn kr_osc_disable(++client: *kr_client_t);

    fn kr_read_tag_inner(++client: *kr_client_t, ++tag_item: **c_schar,
                         ++tag_name: **c_schar, ++tag_value: **c_schar);
    fn kr_read_tag(++client: *kr_client_t, ++tag_item: **c_schar,
                   ++tag_name: **c_schar, ++tag_value: **c_schar) -> c_int;
    fn kr_tags(++client: *kr_client_t, ++item: *c_schar);
    fn kr_tag(++client: *kr_client_t, ++item: *c_schar, ++tag_name: *c_schar);
    fn kr_set_tag(++client: *kr_client_t, ++item: *c_schar,
                  ++tag_name: *c_schar, ++tag_value: *c_schar);
}


use core::libc::*;
pub type Struct_kr_videoport_St = c_void;
pub type kr_videoport_t = Struct_kr_videoport_St;
#[link_args = "-lkradradio_client"]
pub extern "C" {

    fn kr_compositor_response_to_string(++kr_response: *kr_response_t,
                                        ++string: **c_schar) -> c_int;
    fn kr_compositor_subunit_list(++client: *kr_client_t);
    fn kr_compositor_subunit_create(++client: *kr_client_t,
                                    ++_type: kr_compositor_subunit_t,
                                    ++option: *c_schar) -> c_int;
    fn kr_compositor_subunit_destroy(++client: *kr_client_t,
                                     ++address: *kr_address_t);
    fn kr_compositor_info(++client: *kr_client_t);
    fn kr_compositor_set_frame_rate(++client: *kr_client_t,
                                    ++numerator: c_int, ++denominator: c_int);
    fn kr_compositor_set_resolution(++client: *kr_client_t, ++width: c_int,
                                    ++height: c_int);
    fn kr_compositor_close_display(++client: *kr_client_t);
    fn kr_compositor_open_display(++client: *kr_client_t, ++width: c_int,
                                  ++height: c_int);
    fn kr_compositor_background(++client: *kr_client_t, ++filename: *c_schar)
     -> c_int;
    fn kr_compositor_snapshot(++client: *kr_client_t);
    fn kr_compositor_snapshot_jpeg(++client: *kr_client_t);
    fn kr_videoport_set_callback(++kr_videoport: *kr_videoport_t,
                                 ++callback: *u8, ++pointer: *c_void);
    fn kr_videoport_activate(++kr_videoport: *kr_videoport_t);
    fn kr_videoport_deactivate(++kr_videoport: *kr_videoport_t);
    fn kr_videoport_create(++client: *kr_client_t) -> *kr_videoport_t;
    fn kr_videoport_destroy(++kr_videoport: *kr_videoport_t);
}

/* automatically generated by rust-bindgen */

use core::libc::*;
pub type Struct_kr_audioport_St = c_void;
pub type kr_audioport_t = Struct_kr_audioport_St;
#[link_args = "-lkradradio_client"]
pub extern "C" {


    fn kr_mixer_response_to_string(++kr_response: *kr_response_t,
                                   ++string: **c_schar) -> c_int;
    fn kr_mixer_read_control(++client: *kr_client_t,
                             ++portgroup_name: **c_schar,
                             ++control_name: **c_schar, ++value: *c_float) ->
     c_int;
    fn kr_mixer_portgroup_xmms2_cmd(++client: *kr_client_t,
                                    ++portgroupname: *c_schar,
                                    ++xmms2_cmd: *c_schar);
    fn kr_mixer_set_sample_rate(++client: *kr_client_t, ++sample_rate: c_int);
    fn kr_mixer_plug_portgroup(++client: *kr_client_t, ++name: *c_schar,
                               ++remote_name: *c_schar);
    fn kr_mixer_unplug_portgroup(++client: *kr_client_t, ++name: *c_schar,
                                 ++remote_name: *c_schar);
    fn kr_mixer_update_portgroup(++client: *kr_client_t,
                                 ++portgroupname: *c_schar,
                                 ++update_command: uint64_t,
                                 ++string: *c_schar);
    fn kr_mixer_update_portgroup_map_channel(++client: *kr_client_t,
                                             ++portgroupname: *c_schar,
                                             ++in_channel: c_int,
                                             ++out_channel: c_int);
    fn kr_mixer_update_portgroup_mixmap_channel(++client: *kr_client_t,
                                                ++portgroupname: *c_schar,
                                                ++in_channel: c_int,
                                                ++out_channel: c_int);
    fn kr_mixer_push_tone(++client: *kr_client_t, ++tone: *c_schar);
    fn kr_mixer_bind_portgroup_xmms2(++client: *kr_client_t,
                                     ++portgroupname: *c_schar,
                                     ++ipc_path: *c_schar);
    fn kr_mixer_unbind_portgroup_xmms2(++client: *kr_client_t,
                                       ++portgroupname: *c_schar);
    fn kr_mixer_create_portgroup(++client: *kr_client_t, ++name: *c_schar,
                                 ++direction: *c_schar, ++channels: c_int);
    fn kr_mixer_remove_portgroup(++client: *kr_client_t,
                                 ++portgroupname: *c_schar);
    fn kr_mixer_portgroup_list(++client: *kr_client_t);
    fn kr_mixer_info(++client: *kr_client_t);
    fn kr_mixer_portgroup_info(++client: *kr_client_t,
                               ++portgroupname: *c_schar);
    fn kr_mixer_set_control(++client: *kr_client_t,
                            ++portgroup_name: *c_schar,
                            ++control_name: *c_schar,
                            ++control_value: c_float, ++duration: c_int);
    fn kr_mixer_set_effect_control(++client: *kr_client_t,
                                   ++portgroup_name: *c_schar,
                                   ++effect_num: c_int, ++control_id: c_int,
                                   ++control_name: *c_schar,
                                   ++control_value: c_float,
                                   ++duration: c_int, ++ease: krad_ease_t);
    fn kr_audioport_get_buffer(++kr_audioport: *kr_audioport_t,
                               ++channel: c_int) -> *c_float;
    fn kr_audioport_set_callback(++kr_audioport: *kr_audioport_t,
                                 ++callback: *u8, ++pointer: *c_void);
    fn kr_audioport_activate(++kr_audioport: *kr_audioport_t);
    fn kr_audioport_deactivate(++kr_audioport: *kr_audioport_t);
    fn kr_audioport_create(++client: *kr_client_t,
                           ++direction: krad_mixer_portgroup_direction_t) ->
     *kr_audioport_t;
    fn kr_audioport_destroy(++kr_audioport: *kr_audioport_t);
}

/* automatically generated by rust-bindgen */

use core::libc::*;
#[link_args = "-lkradradio_client"]
pub extern "C" {
    fn krad_radio_launch(++sysname: *c_schar);
    fn krad_radio_destroy(++sysname: *c_schar) -> c_int;
    fn krad_radio_running(++sysname: *c_schar) -> c_int;
    fn krad_radio_running_stations() -> *c_schar;
}

use core::libc::*;
pub type kr_compositor_subunit_t = c_uint;
pub const KR_VIDEOPORT: c_uint = 1;
pub const KR_SPRITE: c_uint = 2;
pub const KR_TEXT: c_uint = 3;
pub const KR_VECTOR: c_uint = 4;
pub type krad_vector_type_t = c_uint;
pub const NOTHING: c_uint = 0;
pub const HEX: c_uint = 1;
pub const CIRCLE: c_uint = 2;
pub const RECT: c_uint = 3;
pub const TRIANGLE: c_uint = 4;
pub const VIPER: c_uint = 5;
pub const METER: c_uint = 6;
pub const GRID: c_uint = 7;
pub const CURVE: c_uint = 8;
pub const ARROW: c_uint = 9;
pub const CLOCK: c_uint = 10;
pub const SHADOW: c_uint = 11;
pub type kr_compositor_control_t = c_uint;
pub const KR_NO: c_uint = 0;
pub const KR_X: c_uint = 1;
pub const KR_Y: c_uint = 2;
pub const KR_Z: c_uint = 3;
pub const KR_WIDTH: c_uint = 4;
pub const KR_HEIGHT: c_uint = 5;
pub const KR_ROTATION: c_uint = 6;
pub const KR_OPACITY: c_uint = 7;
pub const KR_XSCALE: c_uint = 8;
pub const KR_YSCALE: c_uint = 9;
pub const KR_RED: c_uint = 10;
pub const KR_GREEN: c_uint = 11;
pub const KR_BLUE: c_uint = 12;
pub const KR_ALPHA: c_uint = 13;
pub const KR_TICKRATE: c_uint = 14;
pub type krad_text_rep_t = Struct_krad_text_rep_St;
pub type kr_text_t = Struct_krad_text_rep_St;
pub type krad_sprite_rep_t = Struct_krad_sprite_rep_St;
pub type kr_sprite_t = Struct_krad_sprite_rep_St;
pub type krad_vector_rep_t = Struct_krad_vector_rep_St;
pub type kr_vector_t = Struct_krad_vector_rep_St;
pub type krad_port_rep_t = Struct_krad_port_rep_St;
pub type kr_port_t = Struct_krad_port_rep_St;
pub type kr_compositor_subunit_controls_t =
    Struct_kr_compositor_subunit_controls_St;
pub type kr_comp_controls_t = Struct_kr_compositor_subunit_controls_St;
pub type krad_compositor_rep_t = Struct_krad_compositor_rep_St;
pub type kr_compositor_t = Struct_krad_compositor_rep_St;
pub struct Struct_kr_compositor_subunit_controls_St {
    pub number: c_int,
    pub x: c_int,
    pub y: c_int,
    pub z: c_int,
    pub tickrate: c_int,
    pub width: c_int,
    pub height: c_int,
    pub xscale: c_float,
    pub yscale: c_float,
    pub rotation: c_float,
    pub opacity: c_float,
}
pub struct Struct_krad_sprite_rep_St {
    pub filename: [c_schar * 256],
    pub controls: kr_comp_controls_t,
}
pub struct Struct_krad_text_rep_St {
    pub text: [c_schar * 1024],
    pub font: [c_schar * 128],
    pub red: c_float,
    pub green: c_float,
    pub blue: c_float,
    pub controls: kr_comp_controls_t,
}
pub struct Struct_krad_vector_rep_St {
    pub krad_vector_type: krad_vector_type_t,
    pub red: c_float,
    pub green: c_float,
    pub blue: c_float,
    pub controls: kr_comp_controls_t,
}
pub struct Struct_krad_port_rep_St {
    pub sysname: [c_schar * 128],
    pub direction: c_int,
    pub source_width: c_int,
    pub source_height: c_int,
    pub crop_x: c_int,
    pub crop_y: c_int,
    pub crop_width: c_int,
    pub crop_height: c_int,
    pub controls: kr_comp_controls_t,
}
pub struct Struct_krad_compositor_rep_St {
    pub width: uint32_t,
    pub height: uint32_t,
    pub fps_numerator: uint32_t,
    pub fps_denominator: uint32_t,
    pub sprites: uint32_t,
    pub vectors: uint32_t,
    pub texts: uint32_t,
    pub inputs: uint32_t,
    pub outputs: uint32_t,
    pub frames: uint64_t,
    pub snapshot_filename: [c_schar * 256],
    pub background_filename: [c_schar * 256],
}
#[link_args = "-lkradradio_client"]
pub extern "C" {

    fn kr_compositor_subunit_type_to_string(++_type: kr_compositor_subunit_t)
     -> *c_schar;
    fn krad_string_to_vector_type(++string: *c_schar) -> krad_vector_type_t;
    fn krad_string_to_compositor_control(++string: *c_schar) ->
     kr_compositor_control_t;
    fn krad_vector_type_to_string(++_type: krad_vector_type_t) -> *c_schar;
}


use core::libc::*;
pub type krad_mixer_rep_t = Struct_krad_mixer_rep_St;
pub type kr_mixer_t = Struct_krad_mixer_rep_St;
pub type kr_portgroup_t = Struct_krad_mixer_portgroup_rep_St;
pub type kr_mixer_portgroup_t = Struct_krad_mixer_portgroup_rep_St;
pub type krad_mixer_portgroup_rep_t = Struct_krad_mixer_portgroup_rep_St;
pub type kr_mixer_portgroup_control_t = c_uint;
pub const KR_VOLUME: c_uint = 1;
pub const KR_CROSSFADE: c_uint = 2;
pub const KR_CROSSFADE_GROUP: c_uint = 3;
pub const KR_XMMS2_IPC_PATH: c_uint = 4;
pub type kr_mixer_effect_control_t = c_uint;
pub const DB: c_uint = 1;
pub const BANDWIDTH: c_uint = 2;
pub const HZ: c_uint = 3;
pub const TYPE: c_uint = 4;
pub const DRIVE: c_uint = 5;
pub const BLEND: c_uint = 6;
pub type krad_mixer_portgroup_io_t = c_uint;
pub const KRAD_TONE: c_uint = 1;
pub const KLOCALSHM: c_uint = 2;
pub const KRAD_AUDIO: c_uint = 3;
pub const KRAD_LINK: c_uint = 4;
pub const MIXBUS: c_uint = 5;
pub type krad_mixer_output_t = c_uint;
pub const NOTOUTPUT: c_uint = 0;
pub const DIRECT: c_uint = 1;
pub const AUX: c_uint = 2;
pub type channels_t = c_uint;
pub const NIL: c_uint = 0;
pub const MONO: c_uint = 1;
pub const STEREO: c_uint = 2;
pub const THREE: c_uint = 3;
pub const QUAD: c_uint = 4;
pub const FIVE: c_uint = 5;
pub const SIX: c_uint = 6;
pub const SEVEN: c_uint = 7;
pub const EIGHT: c_uint = 8;
pub type krad_mixer_portgroup_direction_t = c_uint;
pub const OUTPUT: c_uint = 0;
pub const INPUT: c_uint = 1;
pub const MIX: c_uint = 2;
pub struct Struct_krad_mixer_portgroup_rep_St {
    pub sysname: [c_schar * 64],
    pub channels: channels_t,
    pub io_type: c_int,
    pub mixbus: [c_schar * 64],
    pub crossfade_group: [c_schar * 64],
    pub fade: c_float,
    pub volume: [c_float * 8],
    pub map: [c_int * 8],
    pub mixmap: [c_int * 8],
    pub rms: [c_float * 8],
    pub peak: [c_float * 8],
    pub delay: c_int,
    pub eq: kr_eq_rep_t,
    pub lowpass: kr_lowpass_rep_t,
    pub highpass: kr_highpass_rep_t,
    pub analog: kr_analog_rep_t,
    pub has_xmms2: c_int,
    pub xmms2_ipc_path: [c_schar * 128],
}
pub struct Struct_krad_mixer_rep_St {
    pub sample_rate: uint32_t,
    pub inputs: uint32_t,
    pub outputs: uint32_t,
    pub buses: uint32_t,
    pub time_source: [c_schar * 32],
}
pub struct Struct_krad_effects_rep_St {
    pub effect_typ: kr_effect_type_t,
    pub effect: [*c_void * 8],
}

#[link_args = "-lkradradio_client"]
pub extern "C" {
    fn krad_mixer_channel_number_to_string(++channel: c_int) -> *c_schar;
    fn effect_control_to_string(++effect_control: kr_mixer_effect_control_t)
     -> *c_schar;
    fn portgroup_control_to_string(++portgroup_control:
                                       kr_mixer_portgroup_control_t) ->
     *c_schar;

}

use core::libc::*;
pub type krad_radio_rep_t = Struct_krad_radio_rep_St;
pub type kr_radio_t = Struct_krad_radio_rep_St;
pub type kr_remote_t = Struct_kr_remote_St;
pub type kr_tag_t = Struct_kr_tag_St;
pub struct Struct_krad_radio_rep_St {
    pub uptime: uint64_t,
    pub cpu_usage: uint32_t,
    pub sysinfo: [c_schar * 256],
    pub logname: [c_schar * 256],
}
pub struct Struct_kr_remote_St {
    pub port: c_int,
    pub interface: [c_schar * 128],
}
pub struct Struct_kr_tag_St {
    pub unit: [c_schar * 256],
    pub name: [c_schar * 256],
    pub value: [c_schar * 256],
    pub source: [c_schar * 256],
}

use core::libc::*;
pub type kr_effect_type_t = c_uint;
pub const KRAD_NOFX: c_uint = 0;
pub const KRAD_EQ: c_uint = 1;
pub const KRAD_LOWPASS: c_uint = 2;
pub const KRAD_HIGHPASS: c_uint = 3;
pub const KRAD_ANALOG: c_uint = 4;
pub type krad_eq_rep_t = Struct_krad_eq_rep_St;
pub type kr_eq_rep_t = Struct_krad_eq_rep_St;
pub type krad_eq_band_rep_t = Struct_krad_eq_band_rep_St;
pub type kr_eq_band_rep_t = Struct_krad_eq_band_rep_St;
pub struct Struct_krad_eq_band_rep_St {
    pub db: c_float,
    pub bandwidth: c_float,
    pub hz: c_float,
}
pub struct Struct_krad_eq_rep_St {
    pub band: [kr_eq_band_rep_t * 32],
}
pub type krad_pass_rep_t = Struct_krad_pass_rep_St;
pub type kr_highpass_rep_t = Struct_krad_pass_rep_St;
pub type kr_lowpass_rep_t = Struct_krad_pass_rep_St;
pub struct Struct_krad_pass_rep_St {
    pub bandwidth: c_float,
    pub hz: c_float,
}
pub type krad_analog_rep_t = Struct_krad_analog_rep_St;
pub type kr_analog_rep_t = Struct_krad_analog_rep_St;
pub struct Struct_krad_analog_rep_St {
    pub drive: c_float,
    pub blend: c_float,
}

use core::libc::*;
pub type krad_ease_t = c_uint;
pub const LINEAR: c_uint = 33999;
pub const FIRSTEASING: c_uint = 33999;
pub const EASEINSINE: c_uint = 34000;
pub const EASEOUTSINE: c_uint = 34001;
pub const EASEINOUTSINE: c_uint = 34002;
pub const EASEINCUBIC: c_uint = 34003;
pub const EASEOUTCUBIC: c_uint = 34004;
pub const EASEINOUTCUBIC: c_uint = 34005;
pub const EASEINOUTELASTIC: c_uint = 34006;
pub const LASTEASING: c_uint = 34006;

