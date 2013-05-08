#include <krad_vorbis.h>

static krad_vorbis_t *krad_vorbis_decoder_hdr_error (krad_vorbis_t *vorbis,
                                                     ogg_packet *op);

int krad_vorbis_test_headers (krad_codec_header_t *hdr) {

  krad_vorbis_t *vorbis_dec;
  
  vorbis_dec = NULL;

  vorbis_dec = krad_vorbis_decoder_create (hdr);
  if (vorbis_dec != NULL) {
    krad_vorbis_decoder_destroy (&vorbis_dec);
    return 0;
  }
  return -1;
}

/* Encoding */

int32_t krad_vorbis_encoder_destroy (krad_vorbis_t **vorbis) {
  if ((vorbis != NULL) && (*vorbis != NULL)) {
    vorbis_block_clear (&(*vorbis)->vblock);
    vorbis_comment_clear (&(*vorbis)->vc);
    vorbis_dsp_clear (&(*vorbis)->vdsp);
    vorbis_info_clear (&(*vorbis)->vinfo);
    free (*vorbis);
    *vorbis = NULL;
    return 0;
  }
  return -1;
}

krad_vorbis_t *krad_vorbis_encoder_create (int channels,
                                           int sample_rate,
                                           float quality) {

  int ret;
  int pos;
  krad_vorbis_t *vorbis;
  ogg_packet header_main;
  ogg_packet header_comments;
  ogg_packet header_codebooks;
  
  vorbis = calloc (1, sizeof(krad_vorbis_t));

  pos = 0;

  vorbis->error = 0;
  vorbis->state_str = "";

  vorbis->channels = channels;
  vorbis->sample_rate = sample_rate;
  vorbis->quality = quality;  
  
  vorbis_info_init (&vorbis->vinfo);

  printk ("KR Vorbis Encoder Version: %s", vorbis_version_string());

  ret = vorbis_encode_init_vbr (&vorbis->vinfo,
                                vorbis->channels,
                                vorbis->sample_rate,
                                vorbis->quality);
  if (ret < 0) {
    vorbis->error = ret;
    vorbis->state_str = "Krad Vorbis Encoder: vorbis_encode_init_vbr fail";
    if (ret == OV_EIMPL) {
      vorbis->state_str = "Krad Vorbis Encoder: vorbis mode not supported";
    }
    if (ret == OV_EINVAL) {
      vorbis->state_str = "Krad Vorbis Encoder: illegal vorbis mode";
    }
    printke (vorbis->state_str);
    return vorbis;
  }

  vorbis->small_blocksz = vorbis_info_blocksize (&vorbis->vinfo, 0);
  vorbis->large_blocksz = vorbis_info_blocksize (&vorbis->vinfo, 1);

  printk ("KR Vorbis Encoder Setup: Channels %d Sample Rate: %d Quality: %f",
          vorbis->channels, vorbis->sample_rate, vorbis->quality);

  printk ("KR Vorbis Encoder Setup: Small Block Size: %u Large Block Size: %u",
          vorbis->small_blocksz, vorbis->large_blocksz);

  vorbis_analysis_init (&vorbis->vdsp, &vorbis->vinfo);
  vorbis_block_init (&vorbis->vdsp, &vorbis->vblock);
  vorbis_comment_init (&vorbis->vc);

#ifdef APPVERSION
  vorbis_comment_add_tag (&vorbis->vc, "ENCODER", APPVERSION);
#endif

  vorbis->header.codec = VORBIS;
  vorbis_analysis_headerout (&vorbis->vdsp,
                             &vorbis->vc, 
                             &header_main,
                             &header_comments,
                             &header_codebooks);

  vorbis->hdrdata[0] = 0x02;
  vorbis->hdrdata[1] = (char)header_main.bytes;
  vorbis->hdrdata[2] = (char)header_comments.bytes;

  pos = 3;
  
  memcpy (vorbis->hdrdata + pos,
          header_main.packet,
          header_main.bytes);

  vorbis->header.data[0] = vorbis->hdrdata + pos;
  vorbis->header.sz[0] = header_main.bytes;
  pos += header_main.bytes;

  memcpy (vorbis->hdrdata + pos,
          header_comments.packet,
          header_comments.bytes);

  vorbis->header.data[1] = vorbis->hdrdata + pos;
  vorbis->header.sz[1] = header_comments.bytes;
  pos += header_comments.bytes;

  memcpy (vorbis->hdrdata + pos,
          header_codebooks.packet,
          header_codebooks.bytes);
          
  vorbis->header.data[2] = vorbis->hdrdata + pos;
  vorbis->header.sz[2] = header_codebooks.bytes;    
  pos += header_codebooks.bytes;

  //vorbis->header.header_combined = vorbis->hdrdata;
  //vorbis->header.header_combined_size = pos;
  vorbis->header.count = 3;

  if (1) {
    printk ("KR Vorbis Encoder Setup: header test start"); 
    krad_vorbis_test_headers (&vorbis->header);
    printk ("KR Vorbis Encoder Setup: header test end");   
  }  

  vorbis->state_str = "Krad Vorbis Encoder: Init OK";

  return vorbis;
}

int32_t kr_vorbis_encode (krad_vorbis_t *vorbis,
                          kr_codeme_t *codeme,
                          kr_medium_t *medium) {
  int32_t bo_ret;
  int32_t ret;
  int c;
  float **pcm;
  ogg_packet op;

  if (medium != NULL) {
    if (medium->a.count > 0) {
      pcm = vorbis_analysis_buffer (&vorbis->vdsp, medium->a.count);
      for (c = 0; c < vorbis->channels; c++) {
        memcpy (pcm[c], medium->a.samples[c], medium->a.count * 4);
      }
    } else {
      printk ("KR Vorbis Encoder: Got finish notice");
    }
    ret = vorbis_analysis_wrote (&vorbis->vdsp, medium->a.count);
    if (ret < 0) {
      vorbis->error = ret;
      vorbis->state_str = "Krad Vorbis Encoder: vorbis_analysis_wrote fail";
      printke (vorbis->state_str);
      return -1;
    }
    //printk ("KR Vorbis Encoder: wrote %d samples", medium->a.count);
  }

  bo_ret = vorbis_analysis_blockout (&vorbis->vdsp, &vorbis->vblock);
  if (bo_ret < 0) {
    vorbis->error = bo_ret;
    vorbis->state_str = "Krad Vorbis Encoder: vorbis_analysis_blockout fail";
    printke (vorbis->state_str);
    return -1;
  }
  if (bo_ret == 1) {
    ret = vorbis_analysis (&vorbis->vblock, &op);
    if (ret < 0) {
      vorbis->error = ret;
      vorbis->state_str = "Krad Vorbis Encoder: vorbis_analysis fail";
      printke (vorbis->state_str);
      return -1;
    }
    //printk ("KR Vorbis Encoder: op gpos: %"PRIi64" size %ld",
    //        op.granulepos, op.bytes);
    codeme->sz = op.bytes;
    codeme->count = op.granulepos - vorbis->frames;
    vorbis->frames = op.granulepos;
    
    memcpy (codeme->data, op.packet, codeme->sz);
    
    //printk ("KR Vorbis Encoder: codeme size: %zu Count: %d",
    //        codeme->sz, codeme->count);
  }

  return bo_ret;                         
}

/* Decoding */

int32_t krad_vorbis_decoder_destroy (krad_vorbis_t **vorbis) {
  if ((vorbis != NULL) && (*vorbis != NULL)) {
    vorbis_block_clear (&(*vorbis)->vblock);
    vorbis_comment_clear (&(*vorbis)->vc);
    vorbis_info_clear (&(*vorbis)->vinfo);
    free (*vorbis);
    *vorbis = NULL;
    return 0;
  }
  return -1;
}

static krad_vorbis_t *krad_vorbis_decoder_hdr_error (krad_vorbis_t *vorbis,
                                                     ogg_packet *op) {

  vorbis->state_str = "KR Vorbis Decoder: header error";
  if (vorbis->error == OV_ENOTVORBIS) {
    snprintf (vorbis->state_str_custom, sizeof(vorbis->state_str_custom),
              "KR Vorbis Decoder: Not a vorbis on packet %"PRIi64"",
              op->packetno);
  }
  if (vorbis->error == OV_EBADHEADER) {
    snprintf (vorbis->state_str_custom, sizeof(vorbis->state_str_custom),
              "KR Vorbis Decoder: bad header on packet %"PRIi64"",
              op->packetno);
  }
  if (vorbis->error == OV_EFAULT) {
    snprintf (vorbis->state_str_custom, sizeof(vorbis->state_str_custom),
              "KR Vorbis Decoder: fault on packet %"PRIi64"",
              op->packetno);
  }
  vorbis->state_str = vorbis->state_str_custom;
  printke (vorbis->state_str);
  return vorbis;
}

krad_vorbis_t *krad_vorbis_decoder_create (kr_codec_hdr_t *header) {

  krad_vorbis_t *vorbis;
  int ret;
  ogg_packet op;
  
  vorbis = calloc (1, sizeof(krad_vorbis_t));

  vorbis->error = 0;
  vorbis->state_str = "";

  vorbis_info_init (&vorbis->vinfo);
  vorbis_comment_init (&vorbis->vc);
  
  op.packet = header->data[0];
  op.bytes = header->sz[0];
  op.b_o_s = 1;
  op.packetno = 0;

  ret = vorbis_synthesis_headerin (&vorbis->vinfo, &vorbis->vc, &op);
  if (ret != 0) {
    vorbis->error = ret;
    return krad_vorbis_decoder_hdr_error (vorbis, &op);
  }

  op.packet = header->data[1];
  op.bytes = header->sz[1];
  op.b_o_s = 0;
  op.packetno = 1;

  ret = vorbis_synthesis_headerin (&vorbis->vinfo, &vorbis->vc, &op);
  if (ret != 0) {
    vorbis->error = ret;
    return krad_vorbis_decoder_hdr_error (vorbis, &op);
  }

  op.packet = header->data[2];
  op.bytes = header->sz[2];
  op.packetno = 2;

  ret = vorbis_synthesis_headerin (&vorbis->vinfo, &vorbis->vc, &op);
  if (ret != 0) {
    vorbis->error = ret;
    return krad_vorbis_decoder_hdr_error (vorbis, &op);
  }

  ret = vorbis_synthesis_init (&vorbis->vdsp, &vorbis->vinfo);
  if (ret != 0) {
    vorbis->error = ret;
    vorbis->state_str = "KR Vorbis Decoder: synthesis init fails!";
    printke (vorbis->state_str);
    return vorbis;
  }
  
  vorbis_block_init (&vorbis->vdsp, &vorbis->vblock);
  
  printk ("KR Vorbis Decoder: Info - Version: %d Channels: %d Sample Rate: %ld",
          vorbis->vinfo.version, vorbis->vinfo.channels, vorbis->vinfo.rate);

  if (((vorbis->vinfo.bitrate_lower == 0) ||
       (vorbis->vinfo.bitrate_lower == 4294967295)) && 
      ((vorbis->vinfo.bitrate_upper == 0) ||
       (vorbis->vinfo.bitrate_upper == 4294967295))) {
    printk ("KR Vorbis Decoder: Nominal Bitrate: %ld",
            vorbis->vinfo.bitrate_nominal);  
  } else {
    printk ("KR Vorbis Decoder: Bitrate: Low: %ld Nominal: %ld High: %ld",
            vorbis->vinfo.bitrate_lower,
            vorbis->vinfo.bitrate_nominal,
            vorbis->vinfo.bitrate_upper);
  }

  vorbis->channels = vorbis->vinfo.channels;
  vorbis->sample_rate = vorbis->vinfo.rate;

  vorbis->state_str = "Krad Vorbis Decoder: Init OK";

  return vorbis;
}

int32_t kr_vorbis_decode (krad_vorbis_t *vorbis,
                          kr_medium_t *medium,
                          kr_codeme_t *codeme) {

  int32_t c;
  int32_t sample_count;
  float **pcm;
  int32_t ret;
  ogg_packet op;

  op.packet = codeme->data;
  op.bytes = codeme->sz;
  op.packetno++;

  //printk ("KR Vorbis Decoder: packet size %zu", codeme->sz);

  ret = vorbis_synthesis (&vorbis->vblock, &op);
  if (ret != 0) {
    vorbis->error = ret;
    vorbis->state_str = "KR Vorbis Decoder: vorbis_synthesis fail";
    if (vorbis->error == OV_ENOTAUDIO) {
      snprintf (vorbis->state_str_custom, sizeof(vorbis->state_str_custom),
                "KR Vorbis Decoder: Not audio packet %"PRIi64" Gpos %"PRIi64""
                "packet size: %zu",
                op.packetno, op.granulepos, codeme->sz);
    }
    if (vorbis->error == OV_EBADPACKET) {
      snprintf (vorbis->state_str_custom, sizeof(vorbis->state_str_custom),
                "KR Vorbis Decoder: Bad packet %"PRIi64" Gpos %"PRIi64""
                "packet size: %zu",
                op.packetno, op.granulepos, codeme->sz);
    }
    vorbis->state_str = vorbis->state_str_custom;
    printke (vorbis->state_str);
    return vorbis->error;
  }

  ret = vorbis_synthesis_blockin (&vorbis->vdsp, &vorbis->vblock);
  if (ret != 0) {
    vorbis->error = ret;
    vorbis->state_str = "KR Vorbis Decoder: vorbis_synthesis_blockin fail";
    if (ret == OV_EINVAL) {
      vorbis->state_str = "KR Vorbis Decoder: not ready for blockin!";
    }
    printke (vorbis->state_str);
    return vorbis->error;
  }  

  sample_count = vorbis_synthesis_pcmout (&vorbis->vdsp, &pcm);
  
  if (sample_count > 0) {

    vorbis->frames += sample_count;

    //printk ("KR Vorbis Decoder: %d samples total %"PRIu64"",
    //        sample_count, vorbis->frames);

    medium->a.channels = vorbis->channels;

    for (c = 0; c < vorbis->channels; c++) {
      memcpy (medium->a.samples[c], pcm[c], sample_count * 4);
    }

    medium->a.count = sample_count;

    ret = vorbis_synthesis_read (&vorbis->vdsp, sample_count);
    if (ret != 0) {
      vorbis->error = ret;
      vorbis->state_str = "KR Vorbis Decoder: vorbis_synthesis_read fail";
      if (ret == OV_EINVAL) {
        vorbis->state_str = "KR Vorbis Decoder: synth read > buffer sz!";
      }
      printke (vorbis->state_str);
      return vorbis->error;
    }
  }
 
  return 0;                     
}
