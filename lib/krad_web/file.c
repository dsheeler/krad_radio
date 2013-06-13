int32_t krad_interweb_file_client_handle(kr_iws_client_t *client) {

  int32_t len;
  char *get;
  krad_interweb_t *s;

  s = client->server;
  get = client->get;

  if (get[0] == '/') {
    get = client->get + 1;
  }
  len = strlen(get);

  for (;;) {
    if ((len > -1) && (len < 32)) {
      if (strmatch(get, "krad.js")) {
        krad_interweb_pack_headers(client, "text/javascript");
        krad_interweb_pack_buffer(client, s->api_js, s->api_js_len);
        krad_interweb_pack_buffer(client, s->iface_js, s->iface_js_len);
        break;
      }
      if (strmatch(get, "dev/krad.js")) {
        krad_interweb_pack_headers(client, "text/javascript");
        krad_interweb_pack_buffer(client, s->api_js, s->api_js_len);
        krad_interweb_pack_buffer(client, s->deviface_js, s->deviface_js_len);
        break;
      }
      if ((len == 0) || (strmatch(get, "dev/"))) {
        krad_interweb_pack_headers(client, "text/html");
        krad_interweb_pack_buffer(client, s->html, s->html_len);
        break;
      }
    }
    krad_interweb_pack_404(client);
    break;
  }
  client->drop_after_sync = 1;
  return 0;
}
