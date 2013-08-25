static float read_peak_scaled(kr_mixer_path *path);
static float read_peak(kr_mixer_path *path);
static void compute_meters(kr_mixer_path *path, uint32_t nframes);
static void update_meter_readings(kr_mixer_path *path);

static float read_peak_scaled(kr_mixer_path *path) {

  float db;
  float def;

  db = 20.0f * log10f(read_peak(path) * 1.0f);
  db = 20.0f * log10f(path->avg[0] * 1.0f);
  if (db < -70.0f) {
      def = 0.0f;
    } else if (db < -60.0f) {
      def = (db + 70.0f) * 0.25f;
    } else if (db < -50.0f) {
      def = (db + 60.0f) * 0.5f + 2.5f;
    } else if (db < -40.0f) {
      def = (db + 50.0f) * 0.75f + 7.5;
    } else if (db < -30.0f) {
      def = (db + 40.0f) * 1.5f + 15.0f;
    } else if (db < -20.0f) {
      def = (db + 30.0f) * 2.0f + 30.0f;
    } else if (db < 0.0f) {
      def = (db + 20.0f) * 2.5f + 50.0f;
    } else {
      def = 100.0f;
  }

  return def;
}

static float read_peak(kr_mixer_path *path) {

  //FIXME N channels

  float tmp = path->peak[0];
  path->peak[0] = 0.0f;

  float tmp2 = path->peak[1];
  path->peak[1] = 0.0f;
  if (tmp > tmp2) {
    return tmp;
  } else {
    return tmp2;
  }
}

static void compute_meters(kr_mixer_path *path, uint32_t nframes) {

  int c;
  int s;
  int w;
  int mw;
  float sample;
  float level;
  int cur_frame;
  int cframes;

  cur_frame = 0;
  cframes = MIN(32, nframes);
  while (cur_frame < nframes) {
    for (c = 0; c < path->channels; c++) {
      level = 0;
      for(s = cur_frame; s < cur_frame + cframes; s++) {
        sample = fabs(path->samples[c][s]);
        level += sample;
        if (sample > path->peak[c]) {
          path->peak[c] = sample;
        }
      }
      path->wins[c][path->win] = level / (float)cframes;
    }
    path->winss[path->win] = cframes;
    cur_frame += cframes;
    cframes = MIN(32, nframes - cur_frame);
    path->win++;
    path->win = path->win % KR_MXR_MAX_MINIWINS;
  }
  for (c = 0; c < path->channels; c++) {
    level = 0;
    s = 0;
    w = path->win;
    mw = path->win + KR_MXR_MAX_MINIWINS;
    while (s < path->mixer->avg_window_size) {
      level += path->wins[c][w % KR_MXR_MAX_MINIWINS];
      s += path->winss[w % KR_MXR_MAX_MINIWINS];
      w++;
      if (w == mw) break;
    }
    path->avg[c] = level / (float)(w - path->win);
  }
}

static void update_meter_readings(kr_mixer_path *path) {

  float peak;

  peak = read_peak_scaled(path);
  if (peak != path->peak_last[0]) {
    path->peak_last[0] = peak;
   /* krad_radio_broadcast_subpath_control(path->mixer->broadcaster,
    *  &path->address, KR_PEAK, peak, NULL);
    */
  }
}
