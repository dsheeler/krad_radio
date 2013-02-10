#ifndef KRAD_CLIENT_H
#define KRAD_CLIENT_H


/**
 * @file krad_radio_client.h
 * @brief Krad Radio Client API
 */

/**
 * @mainpage Krad Radio Client API
 *
 * These documents contain all types and functions needed to to 
 * IPC conmmunication to manage, control and modify krad radio stations.
 * 
 * Sections:
 * @li @ref krad_radio_client_ctl
 * @li @ref krad_radio_client
 * @li @ref krad_mixer_client
 * @li @ref krad_compositor_client
 * @li @ref krad_transponder_client
 *
 */


#include <inttypes.h>


#define ALL_BROADCASTS 1


/** @defgroup krad_radio_client Krad Radio Client API
 * @brief Get and Manage general client functions like IPC-connection, 
 * IPC-responses, system information, uptime, enable or disbale remote 
 * control and list / manage tags for one station
 * @{
 */

/** @name General Functions
 * General functions for connection handling
 * @{
 */

/** Krad Radio Client connection handle.
  * @brief Type to identify a single IPC-connection to a station
  * @see kr_connect,kr_disconnect
  */
typedef struct kr_client_St kr_client_t;

#include "krad_easing_common.h"
#include "krad_mixer_common.h"
#include "krad_compositor_common.h"
#include "krad_radio_common.h"


//typedef void (*rep_callback_t)( void *, void * );


/** Shared memory buffer.
 * @brief Variable sized buffer used to get data in and out of local A/V ports
 * @see kr_shm_create,kr_shm_destroy
 */
typedef struct kr_shm_St kr_shm_t;

typedef struct kr_response_St kr_response_t;
typedef struct kr_response_St kr_crate_t;

typedef struct kr_unit_path_St kr_unit_path_t;
typedef struct kr_unit_control_St kr_unit_control_t;

typedef struct kr_remote_St kr_remote_t;
typedef struct kr_tag_St kr_tag_t;

/*
*  An address can be:
*    Unit:           compositor/sprite/2
*    Unit & Control: compositor/sprite/2/opacity
*/

typedef struct kr_address_St kr_address_t;

/* Top level Units */

typedef enum {
  KR_STATION = 1,
  KR_MIXER,
  KR_COMPOSITOR,
  KR_TRANSPONDER,
} kr_unit_t;

#define KR_UNIT 0

/* Subunits */

typedef enum {
  KR_PORTGROUP = 1,
  KR_EFFECT,
} kr_mixer_subunit_t;

typedef enum {
  KR_VIDEOPORT = 1,
  KR_SPRITE,
  KR_TEXT,
  KR_VECTOR,
} kr_compositor_subunit_t;

typedef enum {
  KR_TRANSMITTER = 1,
  KR_RECEIVER,
  KR_DEMUXER,
  KR_MUXER,
  KR_ENCODER,
  KR_DECODER,
} kr_transponder_subunit_t;

typedef enum {
  KR_STATION_UNIT = 1,
  KR_CPU,
  KR_REMOTE,  
} kr_station_subunit_t;

/* Control Types */

typedef enum {
  KR_FLOAT,
  KR_INT32,
  KR_STRING,
} kr_unit_control_data_t;

typedef union {
  int integer;
  char *string;
  float real;
} kr_unit_control_value_t;

/* Control Names */

typedef enum {
  KR_X,
  KR_Y,
  KR_Z,
  KR_WIDTH,
  KR_HEIGHT,
  KR_ROTATION,
  KR_OPACITY,
  KR_XSCALE,
  KR_YSCALE,
  KR_RED,
  KR_GREEN,
  KR_BLUE,
  KR_ALPHA,
} kr_compositor_control_t;

typedef enum {
  KR_BUFFER = 1,
  KR_BITRATE,
} kr_transponder_control_t;

typedef union {
  void *ptr;
  uint64_t zero;
  kr_mixer_subunit_t mixer_subunit;
  kr_compositor_subunit_t compositor_subunit;
  kr_transponder_subunit_t transponder_subunit;
  kr_station_subunit_t station_subunit;
} kr_subunit_t;

typedef union {
  kr_mixer_portgroup_control_t portgroup_control;
  kr_mixer_effect_control_t effect_control;
  kr_compositor_control_t compositor_control;
  kr_transponder_control_t transponder_control;
} kr_unit_control_name_t;

struct kr_unit_path_St {
  kr_unit_t unit;
  kr_subunit_t subunit;
};

typedef union {
  int number;
  char name[64];
} kr_unit_id_t;

struct kr_address_St {
  kr_unit_path_t path;
  kr_unit_control_name_t control;
  kr_unit_id_t id;
  int sub_id;
  int sub_id2;
};

struct kr_unit_control_St {
  kr_address_t address;
  kr_unit_control_data_t data_type;
  kr_unit_control_value_t value;
  int duration;
};

typedef union {
  void *actual;
  kr_tag_t *tag;
  kr_remote_t *remote;
  kr_mixer_t *mixer;
  kr_mixer_portgroup_t *portgroup;
  kr_compositor_t *compositor;
  kr_radio_t *radio;
} kr_rep_ptr_t;

typedef struct kr_rep_St kr_rep_t;
struct kr_rep_St {
  kr_rep_ptr_t rep_ptr;
  uint32_t type;
  //char *buffer;
};

uint32_t kr_response_get_event (kr_response_t *response);
int kr_string_to_address (char *string, kr_address_t *addr);
int kr_unit_control_set (kr_client_t *client, kr_unit_control_t *uc);
int krad_radio_address_to_ebml (krad_ebml_t *krad_ebml, uint64_t *element_loc, kr_address_t *address);


kr_client_t *kr_client_create (char *client_name);

/**
 * @brief connect to a krad radio daemon identified by sysname
 * @param sysname of local station or ip:port remote station
 * @return connection handle or NULL on error
 */
int kr_connect (kr_client_t *client, char *sysname);


int kr_connect_remote (kr_client_t *client, char *host, int port);


int kr_connected (kr_client_t *client);

/**
 * @brief disconnect an open IPC-connection
 * @param kr_client pointer to handle of the connection to be closed
 */
int kr_disconnect (kr_client_t *client);


int kr_client_destroy (kr_client_t **client);

/**
 * @brief determines if a connection is local or remote
 * @param kr_client handle of the IPC-connection to the station
 * @return 1 if local, 0 otherwise
 */
int kr_client_local (kr_client_t *client);


int kr_client_get_fd (kr_client_t *client);

/**
 * @brief subscribe to broadcast messages of a specific type on one station
 * @param kr_client handle of the IPC-connection to the station
 * @param broadcast_id type of the broadcast messages
 * @see kr_poll
 */
void kr_broadcast_subscribe (kr_client_t *client, uint32_t broadcast_id);

void kr_subscribe_all (kr_client_t *client);


/**
 * @brief check for new broadcast messages after having subscribed to them
 * @param kr_client handle of the IPC-connection to the station
 * @param timeout_ms timeout on checking
 * @return > 0 if there are new messages, 0 otherwise
 * @see kr_broadcast_subscribe
 */
 
int kr_poll (kr_client_t *client, uint32_t timeout_ms);
int kr_ebml_to_remote_status_rep (unsigned char *ebml_frag, kr_remote_t *remote);
void kr_response_free (kr_response_t **kr_response);
int kr_response_to_string (kr_response_t *kr_response, char **string);
int kr_response_to_int (kr_response_t *response, int *number);
int kr_response_to_float (kr_response_t *response, float *number);
char *kr_response_alloc_string (int length);
void kr_response_free_string (char **string);
int kr_response_get_string (unsigned char *ebml_frag, uint64_t ebml_data_size, char **string);
int kr_rep_free (kr_rep_t **);
int kr_response_to_rep (kr_response_t *response, kr_rep_t **kr_rep_in);
void kr_response_address (kr_response_t *response, kr_address_t **address);
void kr_address_debug_print (kr_address_t *addr);
int krad_read_address_from_ebml (krad_ebml_t *ebml, kr_address_t *address);
inline int krad_message_type_has_payload (uint32_t type);
inline int krad_read_message_type_from_ebml (krad_ebml_t *ebml, uint32_t *message_type);
uint32_t kr_response_size (kr_response_t *kr_response);
void kr_client_response_wait_print (kr_client_t *client);
int kr_response_read_into_string (unsigned char *ebml_frag, uint64_t ebml_data_size, char *string);

/**
 * @brief get a response
 * @param kr_client handle of the IPC-connection to the station
 */
void kr_client_response_get (kr_client_t *client, kr_response_t **kr_response);

#define kr_delivery_get kr_client_response_get
#define kr_delivery_wait kr_poll
#define kr_wait kr_poll
#define kr_crate_address_get kr_response_address
#define kr_address_get kr_response_address
#define kr_crate_to_string kr_response_to_string
#define kr_uncrate_string kr_response_to_string
#define kr_uncrate_to_string kr_response_to_string

#define kr_uncrate_int kr_response_to_int
#define kr_uncrate_float kr_response_to_float
#define kr_uncrate_rep kr_response_to_rep
#define kr_uncrate kr_response_to_rep

#define kr_string_release kr_response_free_string
#define kr_string_recycle kr_response_free_string
#define kr_string_goodbye kr_response_free_string

#define kr_crate_recycle kr_response_free
#define kr_crate_go_away kr_response_free

#define kr_subscribe kr_broadcast_subscribe

#define kr_mixer_portgroups kr_mixer_portgroups_list

int kr_delivery_wait_until_final (kr_client_t *client, uint32_t timeout_ms);
int kr_delivery_final (kr_client_t *client);

/**
 * @brief waits for a response
 * @param kr_client handle of the IPC-connection to the station
 */
void kr_client_response_wait (kr_client_t *client, kr_response_t **kr_response);


/** @} */

/** @name Shared Memory Functions
 * Functions for managing the shared memory buffer for local A/V ports
 * @{
 */

/**
 * @brief creates and allocates a shared memory buffer
 * @param client handle of the IPC-connection to the station
 * @return handle for the shared memory buffer, NULL on error
 * @todo currently, this size is fixed for resolution 960x540. this needs to be changed!
 */
kr_shm_t *kr_shm_create (kr_client_t *client);

/**
 * @brief destroys and frees an allocated shared memory buffer
 * @param kr_shm handle of the buffer to be destroyed
 */
void kr_shm_destroy (kr_shm_t *kr_shm);

/** @} */

/** @name Information Functions
 * Functions for to query information from a station
 * @{
 */

/**
 * @brief Prints out the uptime of the station
 * @param client handle of the IPC-connection to the station
 */
//void kr_uptime (kr_client_t *client);

/**
 * @brief Prints out system information (hostname, architecture and kernel 
 * version) from where the station is running on
 * @param client handle of the IPC-connection to the station
 */
void kr_system_info (kr_client_t *client);

/**
 * @brief Prints out the current CPU usage of the station
 * @param client handle of the IPC-connection to the station
 */
//void kr_system_cpu_usage (kr_client_t *client);

/**
 * @brief Sets the "working directory" for the station where logfiles and 
 * snaphots are stored. The directory must exist, it will not be created 
 * for you!
 * @param client handle of the IPC-connection to the station
 * @param dir
 */
void kr_set_dir (kr_client_t *client, char *dir);

/**
 * @brief Prints out the currently used logfile of the station
 * @param client handle of the IPC-connection to the station
 */
//void kr_logname (kr_client_t *client);

/** @} */

/** @name Remote Control Functions
 * Functions to enable or disable remote control interfaces
 * @{
 */

void kr_remote_list (kr_client_t *client);

/**
 * @brief Enable IPC remote control on a specifed port. You can use 
 * "hostname:port" as sysname to specify this station from another machine. 
 * This can only be enabled once per station.
 * @param client handle of the IPC-connection to the station
 * @param port TCP-port on which to listen for incoming connections
 */
int kr_remote_on (kr_client_t *client, char *interface, int port);

/**
 * @brief Disable the previously enabled IPC remote control
 * @param client handle of the IPC-connection to the station
 */
int kr_remote_off (kr_client_t *client, char *interface, int port);

/**
 * @brief Enable web UI remote control on a specifed port. This can only be 
 * enabled once per station.
 * @param client handle of the IPC-connection to the station
 * @param http_port port on which to listen for incoming HTTP-connections
 * @param websocket_port port used for communication between the web-page
 * and the station. Must not be the same as http_port!
 * @param headcode override the header delivered to HTTP-clients
 * @param header override the header delivered to HTTP-clients
 * @param footer override the footer delivered to HTTP-clients
 */
void kr_web_enable (kr_client_t *client, int http_port, int websocket_port, char *headcode, char *header, char *footer);

/**
 * @brief Disable the previously enabled web UI remote control
 * @param client handle of the IPC-connection to the station
 */
void kr_web_disable (kr_client_t *client);

/**
 * @brief Enable Open Sound Control (OSC) remote control on a specifed port.
 * This can only be enabled once per station.
 * @param client handle of the IPC-connection to the station
 * @param port port on which to listen for incoming connections
 */
void kr_osc_enable (kr_client_t *client, int port);

/**
 * @brief Disable the previously enabled OSC remote control
 * @param client handle of the IPC-connection to the station
 */
void kr_osc_disable (kr_client_t *client);

/** @} */

/** @name Tag Control Functions
 * Functions to set and read tags
 * @{
 */

/**
 * @todo oneman document this! kripton has no clue here!
 * @brief kr_read_tag_inner
 * @param client
 * @param tag_item
 * @param tag_name
 * @param tag_value
 */
void kr_read_tag_inner ( kr_client_t *client, char **tag_item, char **tag_name, char **tag_value );

/**
 * @todo oneman document this! kripton has no clue here!
 * @brief kr_read_tag
 * @param client
 * @param tag_item
 * @param tag_name
 * @param tag_value
 * @return 
 */
int kr_read_tag ( kr_client_t *client, char **tag_item, char **tag_name, char **tag_value );

/**
 * @brief prints out a list of all tags in a group
 * @param client handle of the IPC-connection to the station
 * @param item item to print the tags of
 */
void kr_tags (kr_client_t *client, char *item);

/**
 * @brief prints out the value of one specified tag
 * @param client handle of the IPC-connection to the station
 * @param item item the tag is grouped under
 * @param tag_name name of the tag to be printed
 */
void kr_tag (kr_client_t *client, char *item, char *tag_name);

/**
 * @brief sets the value for a specified tag
 * @param client handle of the IPC-connection to the station
 * @param item item the tag is grouped under
 * @param tag_name name of the tag to be set
 * @param tag_value value to set the tag to
 */
void kr_set_tag (kr_client_t *client, char *item, char *tag_name, char *tag_value);

/** @} */

/** @} */

#endif
