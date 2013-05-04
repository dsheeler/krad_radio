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

void krad_vorbis_encoder_destroy (krad_vorbis_t *krad_vorbis) {
  vorbis_block_clear (&krad_vorbis->vblock);
  vorbis_dsp_clear (&krad_vorbis->vdsp);
  vorbis_info_clear (&krad_vorbis->vinfo);
  free (krad_vorbis);
}

krad_vorbis_t *krad_vorbis_encoder_create (int channels,
                                           int sample_rate,
                                           float quality) {

  krad_vorbis_t *krad_vorbis = calloc (1, sizeof(krad_vorbis_t));
  int ret;

  krad_vorbis->channels = channels;
  krad_vorbis->sample_rate = sample_rate;
  krad_vorbis->quality = quality;  
  
  vorbis_info_init (&krad_vorbis->vinfo);

  //printk ("KR Vorbis: Version: %d", krad_vorbis->vinfo.version);

  //printk ("KR Vorbis: params: c%d sr%f q%f\n",
  //        krad_vorbis->channels,
  //        krad_vorbis->sample_rate,
  //        krad_vorbis->quality);

  ret = vorbis_encode_init_vbr (&krad_vorbis->vinfo,
                                krad_vorbis->channels,
                                krad_vorbis->sample_rate,
                                krad_vorbis->quality);

  if (ret == OV_EIMPL) {
    failfast ("Krad Vorbis Encoder: vorbis mode is not supported currently...");
  }

  if (ret == OV_EINVAL) {
    failfast ("Krad Vorbis Encoder: illegal vorbis mode...");
  }

  ret = vorbis_analysis_init (&krad_vorbis->vdsp, &krad_vorbis->vinfo);
  ret = vorbis_block_init (&krad_vorbis->vdsp, &krad_vorbis->vblock);
  
  vorbis_comment_init (&krad_vorbis->vc);

  vorbis_comment_add_tag (&krad_vorbis->vc, "ENCODER", APPVERSION);

  krad_vorbis->krad_codec_header.codec = VORBIS;

  vorbis_analysis_headerout (&krad_vorbis->vdsp,
                             &krad_vorbis->vc, 
                             &krad_vorbis->header_main,
                             &krad_vorbis->header_comments,
                             &krad_vorbis->header_codebooks);

  krad_vorbis->header[0] = 0x02;
  krad_vorbis->headerpos++;

  if (krad_vorbis->header_main.bytes > 255) {
    failfast ("Krad Vorbis Encoder: mainheader to long for code");
  }
  
  krad_vorbis->head_count = krad_vorbis->header_main.bytes;
  krad_vorbis->header[1] = (char)krad_vorbis->head_count;
  krad_vorbis->headerpos++;
  
  if (krad_vorbis->header_comments.bytes > 255) {
    failfast ("Krad Vorbis Encoder: comments header to long for code..");
  }
  
  krad_vorbis->head_count = krad_vorbis->header_comments.bytes;
  krad_vorbis->header[2] = (char)krad_vorbis->head_count;
  krad_vorbis->headerpos++;
  
  //printk ("Vorbis header sizes: %ld %ld %ld\n",
  //        krad_vorbis->header_main.bytes,
  //       krad_vorbis->header_comments.bytes,
  //        krad_vorbis->header_codebooks.bytes);
  
  memcpy (krad_vorbis->header + krad_vorbis->headerpos,
          krad_vorbis->header_main.packet,
          krad_vorbis->header_main.bytes);
  krad_vorbis->krad_codec_header.header[0] =
    krad_vorbis->header + krad_vorbis->headerpos;
  krad_vorbis->headerpos += krad_vorbis->header_main.bytes;
  krad_vorbis->krad_codec_header.header_size[0] =
    krad_vorbis->header_main.bytes;
    
  memcpy (krad_vorbis->header + krad_vorbis->headerpos,
          krad_vorbis->header_comments.packet,
          krad_vorbis->header_comments.bytes);
  krad_vorbis->krad_codec_header.header[1] =
    krad_vorbis->header + krad_vorbis->headerpos;
    
  krad_vorbis->headerpos += krad_vorbis->header_comments.bytes;
  
  krad_vorbis->krad_codec_header.header_size[1] =
    krad_vorbis->header_comments.bytes;
    
  memcpy (krad_vorbis->header + krad_vorbis->headerpos,
          krad_vorbis->header_codebooks.packet,
          krad_vorbis->header_codebooks.bytes);
          
  krad_vorbis->krad_codec_header.header[2] =
    krad_vorbis->header + krad_vorbis->headerpos;
    
  krad_vorbis->headerpos += krad_vorbis->header_codebooks.bytes;
  
  krad_vorbis->krad_codec_header.header_size[2] =
    krad_vorbis->header_codebooks.bytes;

  krad_vorbis->krad_codec_header.header_combined = krad_vorbis->header;
  krad_vorbis->krad_codec_header.header_combined_size = krad_vorbis->headerpos;
  krad_vorbis->krad_codec_header.header_count = 3;

  if (0) {
    krad_vorbis_test_headers (&krad_vorbis->krad_codec_header);
  }
  
  return krad_vorbis;
}

void krad_vorbis_encoder_prepare (krad_vorbis_t *krad_vorbis,
                                  int frames, float ***buffer) {
  *buffer = vorbis_analysis_buffer (&krad_vorbis->vdsp, frames);
}

int krad_vorbis_encoder_wrote (krad_vorbis_t *krad_vorbis, int frames) {
  return vorbis_analysis_wrote (&krad_vorbis->vdsp, frames);
}

int krad_vorbis_encoder_finish (krad_vorbis_t *krad_vorbis) {
  return vorbis_analysis_wrote (&krad_vorbis->vdsp, 0);
}

int krad_vorbis_encoder_write (krad_vorbis_t *krad_vorbis,
                               float **samples, int frames) {
  int c;

  krad_vorbis_encoder_prepare (krad_vorbis, frames, &krad_vorbis->buffer);
  for (c = 0; c < krad_vorbis->channels; c++) {
    memcpy (krad_vorbis->buffer[c], (unsigned char *)samples[c], frames * 4);
  }
  return krad_vorbis_encoder_wrote (krad_vorbis, frames);
}
  
int krad_vorbis_encoder_read (krad_vorbis_t *krad_vorbis,
                              int *out_frames, uint8_t **buffer) {

  if (vorbis_analysis_blockout(&krad_vorbis->vdsp, &krad_vorbis->vblock)) {
    vorbis_analysis (&krad_vorbis->vblock, &krad_vorbis->op);
    *out_frames = krad_vorbis->op.granulepos - krad_vorbis->frames_encoded;
    krad_vorbis->frames_encoded = krad_vorbis->op.granulepos;    
    *buffer = krad_vorbis->op.packet;
    return krad_vorbis->op.bytes;
  }
  return 0;
}

int32_t kr_vorbis_encode (krad_vorbis_t *vorbis,
                          kr_codeme_t *codeme,
                          kr_medium_t *medium) {
  int c;
  float **pcm;
  uint8_t *buffer;

  krad_vorbis_encoder_prepare (vorbis, codeme->count, &pcm);
  for (c = 0; c < 2; c++) {
    memcpy (pcm[c], medium->a.samples[c], codeme->count * 4);
  }
  krad_vorbis_encoder_wrote (vorbis, codeme->count);
  codeme->sz = krad_vorbis_encoder_read (vorbis, &codeme->count, &buffer);
  memcpy (codeme->data, buffer, codeme->sz);

  return 0;                         
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
