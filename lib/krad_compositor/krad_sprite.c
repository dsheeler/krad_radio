#include "krad_sprite.h"

struct kr_sprite {
  kr_sprite_info info;
  kr_compositor_control_easers easers;
  int frames;
  int tick;
  int frame;
  cairo_surface_t *sprite;
  cairo_surface_t **sprite_frames;
  int multisurface;
  cairo_pattern_t *sprite_pattern;
  int sheet_width;
  int sheet_height;
};

static void sprite_tick(kr_sprite *sprite);

#ifdef KRAD_GIF

#if GIFLIB_MAJOR < 5
typedef struct GraphicsControlBlock {
  int DisposalMode;
#define DISPOSAL_UNSPECIFIED      0       /* No disposal specified. */
#define DISPOSE_DO_NOT            1       /* Leave image in place */
#define DISPOSE_BACKGROUND        2       /* Set area too background color */
#define DISPOSE_PREVIOUS          3       /* Restore to previous content */
  bool UserInputFlag;      /* User confirmation required before disposal */
  int DelayTime;           /* pre-display delay in 0.01sec units */
  int TransparentColor;    /* Palette index for transparency, -1 if none */
#define NO_TRANSPARENT_COLOR  -1
} GraphicsControlBlock;
#define UNSIGNED_LITTLE_ENDIAN(lo, hi)  ((lo) | ((hi) << 8))
int DGifExtensionToGCB(const size_t GifExtensionLength,
 const GifByteType *GifExtension, GraphicsControlBlock *GCB) {

  if (GifExtensionLength != 4) {
    return GIF_ERROR;
  }
  GCB->DisposalMode = (GifExtension[0] >> 2) & 0x07;
  GCB->UserInputFlag = (GifExtension[0] & 0x02) != 0;
  GCB->DelayTime = UNSIGNED_LITTLE_ENDIAN(GifExtension[1], GifExtension[2]);
  if (GifExtension[0] & 0x01) {
    GCB->TransparentColor = (int)GifExtension[3];
  } else {
    GCB->TransparentColor = NO_TRANSPARENT_COLOR;
  }
  return GIF_OK;
}
#endif

cairo_surface_t **gif2surface(char *filename, int *frames) {

  int  i, j, Size, Row, Col, Width, Height, ExtCode, Count;
  int InterlacedOffset[] = { 0, 4, 2, 1 };
  int InterlacedJumps[] = { 8, 8, 4, 2 };
  int frame_num;

  GifRecordType RecordType;
  GifByteType *Extension;
  GifRowType *ScreenBuffer;
  GifFileType *gif;
  ColorMapObject *ColorMap;
  GifRowType GifRow;
  GifColorType *ColorMapEntry;
  GraphicsControlBlock gcb;

  int fail;

  cairo_surface_t *surface;
  cairo_surface_t **surfaces;
  //FIXME this is going to leak on free, make this be the correct amount
  surfaces = calloc (512, sizeof(cairo_surface_t *));
  cairo_t *cr;

  int image_stride;
  unsigned char *image_data;

  surface = NULL;
  fail = 0;
  memset(&gcb, 0, sizeof(gcb));
  gcb.TransparentColor = -1;

  //typedef int (*InputFunc) (GifFileType *, GifByteType *, int);
  //if ((gif = DGifOpen(void *userPtr, InputFunc readFunc, &error)) == NULL) {
#if GIFLIB_MAJOR < 5
  if ((gif = DGifOpenFileName(filename)) == NULL) {
#else
  int error;
  if ((gif = DGifOpenFileName(filename, &error)) == NULL) {
#endif
    printk ("Gif failed to open");
    return NULL;
  }

  Size = gif->SWidth * sizeof(GifPixelType);
  ScreenBuffer = (GifRowType *)malloc(gif->SHeight * sizeof(GifRowType));

  for (i = 0; i < gif->SHeight; i++) {
    ScreenBuffer[i] = (GifRowType)malloc(Size);
  }

  for (i = 0; i < gif->SWidth; i++)  {
    ScreenBuffer[0][i] = gif->SBackGroundColor;
  }

  for (i = 1; i < gif->SHeight; i++) {
    memcpy(ScreenBuffer[i], ScreenBuffer[0], Size);
  }

  frame_num = 0;
  *frames = 0;

  while (1) {
    if (DGifGetRecordType(gif, &RecordType) == GIF_ERROR) {
      printk ("Gif fail");
      fail = 1;
      break;
    }
    if (RecordType == IMAGE_DESC_RECORD_TYPE) {
      if (DGifGetImageDesc(gif) == GIF_ERROR) {
        printk ("Gif fail");
        fail = 1;
        break;
      }
      Row = gif->Image.Top;
      Col = gif->Image.Left;
      Width = gif->Image.Width;
      Height = gif->Image.Height;
      printk("Image %d at (%d, %d) [%dx%d]: \n", frame_num, Col, Row,
       Width, Height);
      if ((gif->Image.Left + gif->Image.Width > gif->SWidth)
       || (gif->Image.Top + gif->Image.Height > gif->SHeight)) {
          printk("Image %d is not confined to screen dimension, aborted.",
           frame_num);
          fail = 1;
          break;
      }

      surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, gif->SWidth,
       gif->SHeight);
      surfaces[frame_num] = surface;
      cr = cairo_create (surface);
      if (frame_num == 0) {
        //if ((gcb.TransparentColor > -1) && (gcb.TransparentColor != gif->SBackGroundColor)) {
          //cairo_set_source_rgba (cr, gif->SBackGroundColor, gif->SBackGroundColor, gif->SBackGroundColor, 1);
          //cairo_set_source_rgba (cr, gif->SBackGroundColor, gif->SBackGroundColor, gif->SBackGroundColor, 1);
          //cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
          //cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
          //cairo_paint (cr);
        //}
        //cairo_set_source_rgba (cr, GREY, 0.5);
        //cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
        //cairo_paint (cr);
      } else {
        if (gcb.DisposalMode == DISPOSE_BACKGROUND) {
          //cairo_set_source_rgba (cr, gif->SBackGroundColor, gif->SBackGroundColor, gif->SBackGroundColor, 1);
          //cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
          cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
          cairo_paint (cr);
        } else {
          //if ((gcb.DisposalMode == DISPOSE_PREVIOUS) || (gcb.DisposalMode == DISPOSE_DO_NOT) || (gcb.DisposalMode == DISPOSAL_UNSPECIFIED)) {
          if ((gcb.DisposalMode == DISPOSE_PREVIOUS) || (gcb.DisposalMode == DISPOSE_DO_NOT)) {
          //if (!(DISPOSAL_UNSPECIFIED)) {
            cairo_set_source_surface (cr, surfaces[frame_num - 1], 0, 0);
            cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
            cairo_paint (cr);
          }
        }
      }
      cairo_destroy (cr);
      image_stride = cairo_format_stride_for_width (CAIRO_FORMAT_ARGB32, gif->SWidth);
      image_data = cairo_image_surface_get_data (surface);
      cairo_surface_flush (surface);
      if (gif->Image.Interlace) {
        //printf("Gif interlaced\n");
        /* Need to perform 4 passes on the images: */
        for (Count = i = 0; i < 4; i++) {
          for (j = Row + InterlacedOffset[i]; j < Row + Height; j += InterlacedJumps[i]) {
            Count++;
            if (DGifGetLine(gif, &ScreenBuffer[j][Col], Width) == GIF_ERROR) {
              printk ("Gif fail");
              fail = 1;
              break;
            }
          }
        }
      } else {
        for (i = 0; i < Height; i++) {
          if (DGifGetLine(gif, &ScreenBuffer[Row++][Col], Width) == GIF_ERROR) {
            printk ("Gif fail");
            fail = 1;
            break;
          }
        }
      }

      ColorMap = gif->Image.ColorMap ? gif->Image.ColorMap : gif->SColorMap;

      if (ColorMap == NULL) {
        printk ("Gif Image does not have a colormap");
        fail = 1;
        break;
      }

      if (gcb.TransparentColor > -1) {
        for (i = 0; i < gif->SHeight; i++) {
          GifRow = ScreenBuffer[i];
          for (j = 0; j < gif->SWidth; j++) {
            if (ScreenBuffer[i][j] != gcb.TransparentColor) {
              ColorMapEntry = &ColorMap->Colors[GifRow[j]];
              image_data[((i * (image_stride)) + (j * 4)) + 3] = 255;
              image_data[((i * (image_stride)) + (j * 4)) + 2] = ColorMapEntry->Red;
              image_data[((i * (image_stride)) + (j * 4)) + 1] = ColorMapEntry->Green;
              image_data[((i * (image_stride)) + (j * 4)) + 0] = ColorMapEntry->Blue;
            } else {
              //image_data[((i * (image_stride)) + (j * 4)) + 3] = 0;
            }
          }
        }
      } else {
        for (i = 0; i < gif->SHeight; i++) {
          GifRow = ScreenBuffer[i];
          for (j = 0; j < gif->SWidth; j++) {
            ColorMapEntry = &ColorMap->Colors[GifRow[j]];
            image_data[((i * (image_stride)) + (j * 4)) + 3] = 255;
            image_data[((i * (image_stride)) + (j * 4)) + 2] = ColorMapEntry->Red;
            image_data[((i * (image_stride)) + (j * 4)) + 1] = ColorMapEntry->Green;
            image_data[((i * (image_stride)) + (j * 4)) + 0] = ColorMapEntry->Blue;
          }
        }
      }
      cairo_surface_mark_dirty (surface);
      frame_num++;
    }

    if (RecordType == EXTENSION_RECORD_TYPE) {
      if (DGifGetExtension (gif, &ExtCode, &Extension) == GIF_ERROR) {
          printk ("Gif fail");
          fail = 1;
          break;
      }

      if (ExtCode == GRAPHICS_EXT_FUNC_CODE) {
        if (DGifExtensionToGCB(Extension[0], Extension+1, &gcb) == GIF_ERROR) {
            printk ("Gif fail");
            fail = 1;
            break;
        }
      }

      while (Extension != NULL) {
        if (DGifGetExtensionNext (gif, &Extension) == GIF_ERROR) {
          printk ("Gif fail");
          fail = 1;
          break;
        }
      }
    }

    if (RecordType == TERMINATE_RECORD_TYPE) {
      //printf ("the end\n");
      break;
    }
  }

  if (fail == 1) {
    for (i = 0; i < frame_num; i++) {
      surface = surfaces[frame_num];
      cairo_surface_destroy (surface);
    }
    frame_num = 0;
  }

  for (i = 0; i < gif->SHeight; i++)  {
    free (ScreenBuffer[i]);
  }
  free (ScreenBuffer);

  if (DGifCloseFile (gif) == GIF_ERROR) {
    printk ("Gif failure on close\n");
  }

  *frames = frame_num;

  return surfaces;
}
#endif

size_t kr_sprite_size() {
  return sizeof(kr_sprite);
}

int kr_sprite_open(kr_sprite *sprite, char *filename) {

  int64_t size;
  kr_compositor_controls *ctrl;

  if ((sprite == NULL) || (filename == NULL)) {
    return -1;
  }

  ctrl = &sprite->info.controls;
  if (sprite->sprite != NULL) {
    kr_sprite_clear(sprite);
  }
  sprite->frames = 1;
  sprite->info.rate = KRAD_SPRITE_DEFAULT_TICKRATE;

  size = file_size(filename);
  if (size < 0) {
    printke("Krad Sprite: File is not right: %s", filename);
    return -1;
  }
  if (size < 1) {
    printke("Krad Sprite: File is zero bytes: %s", filename);
    return -1;
  }
  if (size > 50000000) {
    printke("Krad Sprite: File is too frigging big IMO: %"PRIi64" for %s",
     size, filename);
    return -1;
  }

  if (strstr(filename, "_frames_") != NULL) {
    sprite->frames = atoi(strstr(filename, "_frames_") + 8);
  }

  if ((strstr(filename, ".png") != NULL)
   || (strstr(filename, ".PNG") != NULL)) {
    sprite->sprite = cairo_image_surface_create_from_png(filename);
  }

#ifdef KRAD_GIF
  if ((strstr(filename, ".gif") != NULL)
   || (strstr(filename, ".GIF") != NULL)) {
    sprite->sprite_frames = gif2surface(filename, &sprite->frames);
    sprite->sprite = sprite->sprite_frames[0];
    sprite->multisurface = 1;
  }
#endif

  if ((strstr(filename, ".jpg") != NULL)
   || (strstr(filename, ".JPG") != NULL)) {

    tjhandle jpeg_dec;
    int jpegsubsamp;
    int jpeg_size;
    unsigned char *argb_buffer;
    unsigned char *jpeg_buffer;
    int jpeg_fd;
    int stride;

    jpeg_buffer = malloc(size);
    if (jpeg_buffer == NULL) {
      return -1;
    }

    jpeg_fd = open(filename, O_RDONLY);

    if (jpeg_fd < 1) {
      free(jpeg_buffer);
      return -1;
    }

    jpeg_size = read(jpeg_fd, jpeg_buffer, size);

    jpeg_dec = tjInitDecompress();

    if (tjDecompressHeader2(jpeg_dec, jpeg_buffer, jpeg_size, &sprite->sheet_width, &sprite->sheet_height, &jpegsubsamp)) {
      printke("JPEG header decoding error: %s", tjGetErrorStr());
    } else {
      sprite->sprite = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, sprite->sheet_width, sprite->sheet_height);
      stride = cairo_image_surface_get_stride(sprite->sprite);
      argb_buffer = cairo_image_surface_get_data(sprite->sprite);
      cairo_surface_flush(sprite->sprite);
      if (tjDecompress2(jpeg_dec, jpeg_buffer, jpeg_size,
                argb_buffer, sprite->sheet_width, stride, sprite->sheet_height, TJPF_BGRA, 0 )) {
        printke ("JPEG decoding error: %s", tjGetErrorStr());
        cairo_surface_destroy ( sprite->sprite );
        sprite->sprite = NULL;
      } else {
        tjDestroy(jpeg_dec);
        cairo_surface_mark_dirty(sprite->sprite);
      }
    }

    free(jpeg_buffer);
    close(jpeg_fd);
  }

  if (cairo_surface_status(sprite->sprite) != CAIRO_STATUS_SUCCESS) {
    sprite->sprite = NULL;
    return -1;
  }

  sprite->sheet_width = cairo_image_surface_get_width(sprite->sprite);
  sprite->sheet_height = cairo_image_surface_get_height(sprite->sprite);
  if ((sprite->frames > 1) && (sprite->multisurface == 0)) {
    if (sprite->frames >= 10) {
      ctrl->w = sprite->sheet_width / 10;
      ctrl->h = sprite->sheet_height / ((sprite->frames / 10) + MIN (1, (sprite->frames % 10)));
    } else {
      ctrl->w = sprite->sheet_width / sprite->frames;
      ctrl->h = sprite->sheet_height;
    }
  } else {
    ctrl->w = sprite->sheet_width;
    ctrl->h = sprite->sheet_height;
  }
  sprite->sprite_pattern = cairo_pattern_create_for_surface(sprite->sprite);
  cairo_pattern_set_extend(sprite->sprite_pattern, CAIRO_EXTEND_REPEAT);
  strcpy(sprite->info.filename, filename);
  printk("Loaded Sprite: %s Sheet Width: %d Frames: %d Width: %d Height: %d",
   sprite->info.filename, sprite->sheet_width, sprite->frames, ctrl->w,
   ctrl->h);
  ctrl->opacity = 1.0f;

kr_easer_set(&sprite->easers.rotation, 560.0f, 800, EASEINOUTSINE, NULL);
  return 0;
}

void kr_sprite_clear(kr_sprite *sprite) {

  int i;

  if (sprite->multisurface == 1) {
    for (i = 0; i < sprite->frames; i++) {
      cairo_surface_destroy(sprite->sprite_frames[i]);
    }
    sprite->sprite = NULL;
  } else {
    if (sprite->sprite_pattern != NULL) {
      cairo_pattern_destroy(sprite->sprite_pattern);
      sprite->sprite_pattern = NULL;
    }
    if (sprite->sprite != NULL) {
      cairo_surface_destroy(sprite->sprite);
      sprite->sprite = NULL;
    }
  }
  memset(sprite, 0, sizeof(kr_sprite));
}

int kr_sprite_rate_set(kr_sprite *sprite, int rate) {
  if ((sprite == NULL) || (rate < 0)) return -1;
  sprite->info.rate = rate;
  return 0;
}

void sprite_controls_tick(kr_compositor_controls *c, kr_compositor_control_easers *e) {
  if (kr_easer_active(&e->x)) {
    c->x = kr_easer_process(&e->x, c->x, NULL);
  }
  if (kr_easer_active(&e->y)) {
    c->y = kr_easer_process(&e->y, c->y, NULL);
  }
/*  if (kr_easer_active(&e->z)) {
    c->z = kr_easer_process(&e->z, c->z, NULL);
  }*/
  if (kr_easer_active(&e->w)) {
    c->w = kr_easer_process(&e->w, c->w, NULL);
  }
  if (kr_easer_active(&e->h)) {
    c->h = kr_easer_process(&e->h, c->h, NULL);
  }
  if (kr_easer_active(&e->opacity)) {
    c->opacity = kr_easer_process(&e->opacity, c->opacity, NULL);
  }
  if (kr_easer_active(&e->rotation)) {
    c->rotation = kr_easer_process(&e->rotation, c->rotation, NULL);
  }
}

static void sprite_tick(kr_sprite *sprite) {
  sprite->tick++;
  if (sprite->tick >= sprite->info.rate) {
    sprite->tick = 0;
    sprite->frame++;
    if (sprite->frame == sprite->frames) {
      sprite->frame = 0;
    }
    if (sprite->multisurface == 1) {
      sprite->sprite = sprite->sprite_frames[sprite->frame];
    }
  }
  sprite_controls_tick(&sprite->info.controls, &sprite->easers);
}

void kr_sprite_render(kr_sprite *sprite, cairo_t *cr) {

  kr_compositor_controls *ctrl;

  ctrl = &sprite->info.controls;
  cairo_save(cr);
  if (ctrl->rotation != 0.0f) {
    cairo_translate(cr, ctrl->x, ctrl->y);
    cairo_translate(cr, ctrl->w / 2, ctrl->h / 2);
    cairo_rotate(cr, ctrl->rotation * (M_PI/180.0));
    cairo_translate(cr, ctrl->w / -2, ctrl->h / -2);
    cairo_translate(cr, ctrl->x * -1, ctrl->y * -1);
  }
  if (sprite->multisurface == 0) {
    cairo_set_source_surface(cr, sprite->sprite,
     ctrl->x - (ctrl->w * (sprite->frame % 10)),
     ctrl->y - (ctrl->h * (sprite->frame / 10)));
  } else {
    cairo_set_source_surface(cr, sprite->sprite, ctrl->x, ctrl->y);
  }
  cairo_rectangle(cr, ctrl->x, ctrl->y, ctrl->w, ctrl->h);
  cairo_clip(cr);
  /*cairo_pattern_set_filter (cairo_get_source (cr), CAIRO_FILTER_FAST);*/
  if (ctrl->opacity == 1.0f) {
    cairo_paint(cr);
  } else {
    cairo_paint_with_alpha(cr, ctrl->opacity);
  }
  cairo_restore(cr);
  sprite_tick(sprite);
}

int kr_sprite_info_get(kr_sprite *sprite, kr_sprite_info *info) {
  if ((sprite == NULL) || (info == NULL)) {
    return 0;
  }
  *info = sprite->info;
  return 1;
}
