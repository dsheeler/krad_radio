#include "kr_client.h"

static int test_jack_input_create(kr_client *client);
static int test_jack_output_create(kr_client *client);
static int test_decklink_input_create(kr_client *client, int dev);
static int test_v4l2_input_create(kr_client *client, int dev);
static int test_wayland_output_create(kr_client *client);

static int test_jack_input_create(kr_client *client) {
  int ret;
  kr_xpdr_path_info info;
  char *test_name;
  char *bus_name;
  int channels;
  /* init function? */
  memset(&info, 0, sizeof(kr_xpdr_path_info));
  channels = 2;
  test_name = "Music";
  bus_name = "Master";
  strcpy(info.name, test_name);
  info.input.type = KR_XPDR_ADAPTER;
  info.input.info.adapter_path_info.api = KR_ADP_JACK;
  strcpy(info.input.info.adapter_path_info.info.jack.name, test_name);
  info.input.info.adapter_path_info.info.jack.channels = channels;
  info.input.info.adapter_path_info.info.jack.direction = KR_JACK_INPUT;
  info.output.type = KR_XPDR_MIXER;
  strcpy(info.output.info.mixer_path_info.name, test_name);
  strcpy(info.output.info.mixer_path_info.bus, bus_name);
  info.output.info.mixer_path_info.channels = channels;
  info.output.info.mixer_path_info.type = KR_MXR_INPUT;
  ret = kr_xpdr_mkpath(client, &info);
  return ret;
}

static int test_jack_output_create(kr_client *client) {
  int ret;
  kr_xpdr_path_info info;
  char *test_name;
  char *bus_name;
  int channels;
  /* init func? */
  memset(&info, 0, sizeof(kr_xpdr_path_info));
  channels = 2;
  test_name = "Main";
  bus_name = "Master";
  strcpy(info.name, test_name);
  info.input.type = KR_XPDR_MIXER;
  strcpy(info.input.info.mixer_path_info.name, test_name);
  strcpy(info.input.info.mixer_path_info.bus, bus_name);
  info.input.info.mixer_path_info.channels = channels;
  info.input.info.mixer_path_info.type = KR_MXR_OUTPUT;
  info.output.type = KR_XPDR_ADAPTER;
  info.output.info.adapter_path_info.api = KR_ADP_JACK;
  strcpy(info.output.info.adapter_path_info.info.jack.name, test_name);
  info.output.info.adapter_path_info.info.jack.channels = channels;
  info.output.info.adapter_path_info.info.jack.direction = KR_JACK_OUTPUT;
  ret = kr_xpdr_mkpath(client, &info);
  return ret;
}

static int test_v4l2_input_create(kr_client *client, int dev) {
  int ret;
  kr_xpdr_path_info info;
  int device_num;
  char *test_name;
  int width;
  int height;
  int num;
  int den;
  test_name = "V4L2 Test";
  device_num = 0;
  device_num = dev;
  width = 640;
  height = 360;
  num = 30;
  den = 1;
  /* init func? */
  memset(&info, 0, sizeof(kr_xpdr_path_info));
  strcpy(info.name, test_name);
  info.input.type = KR_XPDR_ADAPTER;
  info.input.info.adapter_path_info.api = KR_ADP_V4L2;
  info.input.info.adapter_path_info.info.v4l2.dev = device_num;
  info.input.info.adapter_path_info.info.v4l2.mode.num = num;
  info.input.info.adapter_path_info.info.v4l2.mode.den = den;
  info.input.info.adapter_path_info.info.v4l2.mode.width = width;
  info.input.info.adapter_path_info.info.v4l2.mode.height = height;
  info.output.type = KR_XPDR_COMPOSITOR;
  strcpy(info.output.info.compositor_path_info.name, test_name);
  info.output.info.compositor_path_info.width = width;
  info.output.info.compositor_path_info.height = height;
  info.output.info.compositor_path_info.type = KR_CMP_INPUT;
  ret = kr_xpdr_mkpath(client, &info);
  return ret;
}

static int test_decklink_input_create(kr_client *client, int dev) {
  int ret;
  kr_xpdr_path_info info;
  char *test_name;
  char *video_connector;
  char *audio_connector;
  int width;
  int height;
  int num;
  int den;
  test_name = "Decklink Test";
  video_connector = "hdmi";
  audio_connector = "hdmi";
  width = 1280;
  height = 720;
  num = 60000;
  den = 1001;
  /* init func? */
  memset(&info, 0, sizeof(kr_xpdr_path_info));
  strcpy(info.name, test_name);
  info.input.type = KR_XPDR_ADAPTER;
  info.input.info.adapter_path_info.api = KR_ADP_DECKLINK;
  snprintf(info.input.info.adapter_path_info.info.decklink.device,
   sizeof(info.input.info.adapter_path_info.info.decklink.device), "%d", dev);
  strcpy(info.input.info.adapter_path_info.info.decklink.video_connector,
   video_connector);
  strcpy(info.input.info.adapter_path_info.info.decklink.audio_connector,
   audio_connector);
  info.input.info.adapter_path_info.info.decklink.num = num;
  info.input.info.adapter_path_info.info.decklink.den = den;
  info.input.info.adapter_path_info.info.decklink.width = width;
  info.input.info.adapter_path_info.info.decklink.height = height;
  info.output.type = KR_XPDR_COMPOSITOR;
  strcpy(info.output.info.compositor_path_info.name, test_name);
  info.output.info.compositor_path_info.width = width;
  info.output.info.compositor_path_info.height = height;
  info.output.info.compositor_path_info.type = KR_CMP_INPUT;
  ret = kr_xpdr_mkpath(client, &info);
  return ret;
}

static int test_wayland_output_create(kr_client *client) {
  int ret;
  kr_xpdr_path_info info;
  char *test_name;
  char *display_name;
  int width;
  int height;
  int fullscreen;
  display_name = "";
  test_name = "Wayland Test";
  width = 640;
  height = 360;
  fullscreen = 0;
  /* init func ? */
  memset(&info, 0, sizeof(kr_xpdr_path_info));
  strcpy(info.name, test_name);
  info.input.type = KR_XPDR_COMPOSITOR;
  strcpy(info.input.info.compositor_path_info.name, test_name);
  info.input.info.compositor_path_info.width = width;
  info.input.info.compositor_path_info.height = height;
  info.input.info.compositor_path_info.type = KR_CMP_OUTPUT;
  info.output.type = KR_XPDR_ADAPTER;
  info.output.info.adapter_path_info.api = KR_ADP_WAYLAND;
  strcpy(info.output.info.adapter_path_info.info.wayland.display_name,
   display_name);
  info.output.info.adapter_path_info.info.wayland.width = width;
  info.output.info.adapter_path_info.info.wayland.height = height;
  info.output.info.adapter_path_info.info.wayland.fullscreen = fullscreen;
  ret = kr_xpdr_mkpath(client, &info);
  return ret;
}


int run_tests(kr_client *client, int test_kode) {
  int ret;
  ret = test_jack_input_create(client);
  if (ret != 0) return ret;
  ret = test_jack_output_create(client);
  if (ret != 0) return ret;
  ret = test_wayland_output_create(client);
  if (ret != 0) return ret;
  if (test_kode < 2) {
    ret = test_v4l2_input_create(client, 0);
    if (ret != 0) return ret;
  } else {
    ret = test_decklink_input_create(client, 0);
    if (ret != 0) return ret;
  }
  return 0;
}

int main (int argc, char *argv[]) {
  kr_client *client;
  char *sysname;
  int ret;
  client = NULL;
  sysname = NULL;
  ret = 0;
  if (argc < 2) {
    fprintf(stderr, "Specify a station sysname!\n");
    return 1;
  }
  if (krad_valid_host_and_port(argv[1])) {
    sysname = argv[1];
  } else {
    if (!krad_valid_sysname(argv[1])) {
      fprintf(stderr, "Invalid station sysname!\n");
      return 1;
    } else {
      sysname = argv[1];
    }
  }
  client = kr_client_create("krad simple client");
  if (client == NULL) {
    fprintf(stderr, "Could create client\n");
    return 1;
  }
  if (!kr_connect(client, sysname)) {
    fprintf(stderr, "Could not connect to %s krad radio daemon\n", sysname);
    kr_client_destroy(&client);
    return 1;
  }
  printf("Connected to %s!\n", sysname);
  printf("Running Tests\n");
  ret = run_tests(client, argc);
  printf("Disconnecting from %s..\n", sysname);
  kr_disconnect(client);
  printf("Disconnected from %s.\n", sysname);
  printf("Destroying client..\n");
  kr_client_destroy(&client);
  printf("Client Destroyed.\n");
  return ret;
}
