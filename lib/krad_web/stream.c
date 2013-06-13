int32_t interweb_client_get_stream(kr_iws_client_t *client) {

  //client->type = KR_IWS_STREAM_OUT;

  printk("fake looking for stream %s and not finding it", client->get);

  return 0;
}

int32_t krad_interweb_stream_in_client_handle(kr_iws_client_t *client) {

  printk("fake reading stream in %zu bytes", client->in->len);
  kr_io2_pulled(client->in, client->in->len);

  return 0;
}
