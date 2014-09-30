#include <glib.h>
#include <glib-object.h>

#include <sndfile.h>

static const gchar *filename = "/usr/share/hydrogen/data/drumkits/Synthie-1/chh_01.wav\0";

int
main(int argc, char **argv)
{
  SF_INFO *info;
  SNDFILE *file;
  signed short *input;
  short *output;
  sf_count_t multi_frames, retval;
  guint i;

  info = (SF_INFO *) malloc(sizeof(SF_INFO));
  info->format = 0;

  /* read buffer */
  file = (SNDFILE *) sf_open(filename, SFM_READ, info);
  g_message("open %s\n  format: %x\0", filename, info->format);

  input = (signed short *) malloc((size_t) info->channels *
				   info->frames *
				   sizeof(signed short));
  
  sf_seek(file, 0, SEEK_SET);
  sf_read_short(file, input, info->frames * info->channels);

  sf_close(file);

  /* write buffer */
  file = (SNDFILE *) sf_open("test.wav\0", SFM_WRITE, info);
  g_message("open %s\n  format: %x\0", "test.wav\0", info->format);

  output = (signed short *) malloc((size_t) info->channels *
				   info->frames *
				   sizeof(long int));
  
  multi_frames = info->frames * info->channels;
  info->seekable = 0;
  info->sections = 0;
  info->frames = 0;

  for(i = 0; i < multi_frames; i++)
    output[i] = input[i];

  retval = sf_write_raw(file, output, multi_frames * sizeof(short));

  if(retval > multi_frames){
    g_warning("retval > multi_frames");
    //    sf_seek(sndfile->file, (multi_frames - retval), SEEK_CUR);
  }

  sf_write_sync(file);
  sf_close(file);

  return(0);
}

