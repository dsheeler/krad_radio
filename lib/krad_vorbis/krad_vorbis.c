#include <krad_vorbis.h>

int krad_vorbis_test_headers (krad_codec_header_t *hdr) {

  krad_vorbis_t *vorbis_dec;
  
  vorbis_dec = NULL;

  vorbis_dec = krad_vorbis_decoder_create (hdr);
  if (vorbis_dec != NULL) {
    krad_vorbis_decoder_destroy (vorbis_dec);
    return 0;
  }
  return -1;
}

/* Encoding */

void krad_vorbis_encoder_destroy (krad_vorbis_t *vorbis) {
  vorbis_block_clear (&vorbis->vblock);
  vorbis_dsp_clear (&vorbis->vdsp);
  vorbis_info_clear (&vorbis->vinfo);
  free (vorbis);
}

krad_vorbis_t *krad_vorbis_encoder_create (int channels,
                                           int sample_rate,
                                           float quality) {

  int ret;

  krad_vorbis_t *vorbis;
  
  vorbis = calloc (1, sizeof(krad_vorbis_t));

  vorbis->channels = channels;
  vorbis->sample_rate = sample_rate;
  vorbis->quality = quality;  
  
  vorbis_info_init (&vorbis->vinfo);

  //printk ("KR Vorbis: Version: %d", vorbis->vinfo.version);

  ret = vorbis_encode_init_vbr (&vorbis->vinfo,
                                vorbis->channels,
                                vorbis->sample_rate,
                                vorbis->quality);

  if (ret == OV_EIMPL) {
    failfast ("Krad Vorbis Encoder: vorbis mode is not supported currently...");
  }

  if (ret == OV_EINVAL) {
    failfast ("Krad Vorbis Encoder: illegal vorbis mode...");
  }

  ret = vorbis_analysis_init (&vorbis->vdsp, &vorbis->vinfo);
  ret = vorbis_block_init (&vorbis->vdsp, &vorbis->vblock);
  
  vorbis_comment_init (&vorbis->vc);

  vorbis_comment_add_tag (&vorbis->vc, "ENCODER", APPVERSION);

  vorbis->krad_codec_header.codec = VORBIS;

  vorbis_analysis_headerout (&vorbis->vdsp,
                             &vorbis->vc, 
                             &vorbis->header_main,
                             &vorbis->header_comments,
                             &vorbis->header_codebooks);

  vorbis->header[0] = 0x02;
  vorbis->headerpos++;

  if (vorbis->header_main.bytes > 255) {
    failfast ("Krad Vorbis Encoder: mainheader to long for code");
  }
  
  vorbis->head_count = vorbis->header_main.bytes;
  vorbis->header[1] = (char)vorbis->head_count;
  vorbis->headerpos++;
  
  if (vorbis->header_comments.bytes > 255) {
    failfast ("Krad Vorbis Encoder: comments header to long for code..");
  }
  
  vorbis->head_count = vorbis->header_comments.bytes;
  vorbis->header[2] = (char)vorbis->head_count;
  vorbis->headerpos++;
  
  memcpy (vorbis->header + vorbis->headerpos,
          vorbis->header_main.packet,
          vorbis->header_main.bytes);

  vorbis->krad_codec_header.header[0] =
    vorbis->header + vorbis->headerpos;
  vorbis->headerpos += vorbis->header_main.bytes;
  vorbis->krad_codec_header.header_size[0] =
    vorbis->header_main.bytes;
    
  memcpy (vorbis->header + vorbis->headerpos,
          vorbis->header_comments.packet,
          vorbis->header_comments.bytes);
  vorbis->krad_codec_header.header[1] =
    vorbis->header + vorbis->headerpos;
    
  vorbis->headerpos += vorbis->header_comments.bytes;
  
  vorbis->krad_codec_header.header_size[1] =
    vorbis->header_comments.bytes;
    
  memcpy (vorbis->header + vorbis->headerpos,
          vorbis->header_codebooks.packet,
          vorbis->header_codebooks.bytes);
          
  vorbis->krad_codec_header.header[2] =
    vorbis->header + vorbis->headerpos;
    
  vorbis->headerpos += vorbis->header_codebooks.bytes;
  
  vorbis->krad_codec_header.header_size[2] =
    vorbis->header_codebooks.bytes;

  vorbis->krad_codec_header.header_combined = vorbis->header;
  vorbis->krad_codec_header.header_combined_size = vorbis->headerpos;
  vorbis->krad_codec_header.header_count = 3;

  if (0) {
    krad_vorbis_test_headers (&vorbis->krad_codec_header);
  }
  
  return vorbis;
}

int32_t kr_vorbis_encode (krad_vorbis_t *vorbis,
                          kr_codeme_t *codeme,
                          kr_medium_t *medium) {
  int32_t ret;
  int c;
  float **pcm;
  ogg_packet op;

  if (medium != NULL) {
    pcm = vorbis_analysis_buffer (&vorbis->vdsp, medium->a.count);
    for (c = 0; c < 2; c++) {
      memcpy (pcm[c], medium->a.samples[c], medium->a.count * 4);
    }
    vorbis_analysis_wrote (&vorbis->vdsp, medium->a.count);

    printk ("KR Vorbis Encoder: wrote %d samples", medium->a.count);
  }

  ret = vorbis_analysis_blockout (&vorbis->vdsp, &vorbis->vblock);
  
  if (ret > 0) {
    vorbis_analysis (&vorbis->vblock, &op);
    printk ("KR Vorbis encoder: op gpos: %"PRIi64" size %ld",
            op.granulepos, op.bytes);
    codeme->sz = op.bytes;
    codeme->count = op.granulepos - vorbis->frames_encoded;
    vorbis->frames_encoded = op.granulepos;
    
    memcpy (codeme->data, op.packet, codeme->sz);
    
    printk ("KR Vorbis Encoder: codeme size: %zu Count: %d",
            codeme->sz, codeme->count);
  }

  return ret;                         
}


/* Decoding */

void krad_vorbis_decoder_destroy (krad_vorbis_t *vorbis) {

  vorbis_info_clear (&vorbis->vinfo);
  vorbis_comment_clear (&vorbis->vc);
  vorbis_block_clear (&vorbis->vblock);
  
  free (vorbis);
}

krad_vorbis_t *
krad_vorbis_decoder_create (krad_codec_header_t *header) {

  krad_vorbis_t *vorbis = calloc(1, sizeof(krad_vorbis_t));

  int ret;

  vorbis_info_init (&vorbis->vinfo);
  vorbis_comment_init (&vorbis->vc);
  
  vorbis->op.packet = header->header[0];
  vorbis->op.bytes = header->header_size[0];
  vorbis->op.b_o_s = 1;
  vorbis->op.packetno = 0;
  ret = vorbis_synthesis_headerin (&vorbis->vinfo, &vorbis->vc, &vorbis->op);
  if (ret != 0) {
    if (ret == OV_ENOTVORBIS) {
      printke ("KR Vorbis Decoder: Not a vorbis on packet %llu",
              vorbis->op.packetno);
    }
    if (ret == OV_EBADHEADER) {
      printke ("KR Vorbis Decoder: bad header on packet %llu",
                vorbis->op.packetno);
    }
    if (ret == OV_EFAULT) {
      printke ("KR Vorbis Decoder: fault on packet %llu",
               vorbis->op.packetno);
    }
  }

  vorbis->op.packet = header->header[1];
  vorbis->op.bytes = header->header_size[1];
  vorbis->op.b_o_s = 0;
  vorbis->op.packetno = 1;
  ret = vorbis_synthesis_headerin(&vorbis->vinfo, &vorbis->vc, &vorbis->op);
  if (ret != 0) {
    if (ret == OV_ENOTVORBIS) {
      printke ("KR Vorbis Decoder: not a vorbis on packet %llu",
              vorbis->op.packetno);
    }
    if (ret == OV_EBADHEADER) {
      printke ("KR Vorbis Decoder: bad header on packet %llu",
               vorbis->op.packetno);
    }
    if (ret == OV_EFAULT) {
      printke ("KR Vorbis Decoder: fault on packet %llu",
                vorbis->op.packetno);
    }
  }

  vorbis->op.packet = header->header[2];
  vorbis->op.bytes = header->header_size[2];
  vorbis->op.packetno = 2;
  ret = vorbis_synthesis_headerin(&vorbis->vinfo, &vorbis->vc, &vorbis->op);
  if (ret != 0) {
    if (ret == OV_ENOTVORBIS) {
      printke ("KR Vorbis Decoder: not a vorbis on packet %llu",
               vorbis->op.packetno);
    }
    if (ret == OV_EBADHEADER) {
      printke ("KR Vorbis Decoder: says bad header on packet %llu",
               vorbis->op.packetno);
    }
    if (ret == OV_EFAULT) {
      printke ("KR Vorbis Decoder: fault on packet %llu",
               vorbis->op.packetno);
    }
  }

  ret = vorbis_synthesis_init(&vorbis->vdsp, &vorbis->vinfo);
  if (ret != 0) {
    printke ("KR Vorbis Decoder: synthesis init fails!");
  }
  
  vorbis_block_init(&vorbis->vdsp, &vorbis->vblock);
  
  printk ("KR Vorbis Decoder: Info - Version: %d Channels: %d Sample Rate: %ld",
          vorbis->vinfo.version, vorbis->vinfo.channels, vorbis->vinfo.rate);
  printk ("KR Vorbis Decoder: Bitrate: %ld %ld %ld",
          vorbis->vinfo.bitrate_upper,
          vorbis->vinfo.bitrate_nominal,
          vorbis->vinfo.bitrate_lower);
  
  vorbis->channels = vorbis->vinfo.channels;
  vorbis->sample_rate = vorbis->vinfo.rate;

  return vorbis;
}

int32_t kr_vorbis_decode (krad_vorbis_t *vorbis,
                          kr_codeme_t *codeme,
                          kr_medium_t *medium) {
 
  int32_t c;
  int32_t sample_count;
  float **pcm;
  int32_t ret;

  vorbis->op.packet = codeme->data;
  vorbis->op.bytes = codeme->sz;
  vorbis->op.packetno++;

  ret = vorbis_synthesis (&vorbis->vblock, &vorbis->op);
  
  if (ret != 0) {
    if (ret == OV_ENOTAUDIO) {
      printke ("KR Vorbis Decoder: not audio packet %llu - %d",
               vorbis->op.packetno, codeme->sz);
    }
    if (ret == OV_EBADPACKET) {
      printke ("KR Vorbis Decoder: bad packet %llu - %d",
               vorbis->op.packetno, codeme->sz);
    }
    return -1;
  }
  
  ret = vorbis_synthesis_blockin (&vorbis->vdsp, &vorbis->vblock);
  
  if (ret != 0) {
    if (ret == OV_EINVAL) {
      printke ("KR Vorbis Decoder: not ready for blockin!");
    }
    return -1;
  }  
  
  sample_count = vorbis_synthesis_pcmout (&vorbis->vdsp, &pcm);
  
  if (sample_count) {

    printk ("KR Vorbis Decoder: %d samples", sample_count);

    medium->a.channels = vorbis->channels;

    for (c = 0; c < vorbis->channels; c++) {
      memcpy (medium->a.samples[c], pcm[c], sample_count * 4);
    }

    medium->a.count = sample_count;

    ret = vorbis_synthesis_read (&vorbis->vdsp, sample_count);

    if (ret != 0) {
      if (ret == OV_EINVAL) {
        printke ("KR Vorbis Decoder: synth read more than in buffer!");
      }
      /* What ?? */
    }
  }
 
  return 0;                     
}
