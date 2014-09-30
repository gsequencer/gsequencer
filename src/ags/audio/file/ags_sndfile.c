/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/file/ags_sndfile.h>

#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_playable.h>

void ags_sndfile_class_init(AgsSndfileClass *sndfile);
void ags_sndfile_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sndfile_playable_interface_init(AgsPlayableInterface *playable);
void ags_sndfile_init(AgsSndfile *sndfile);
void ags_sndfile_finalize(GObject *gobject);

void ags_sndfile_connect(AgsConnectable *connectable);
void ags_sndfile_disconnect(AgsConnectable *connectable);

gboolean ags_sndfile_open(AgsPlayable *playable, gchar *name);
gboolean ags_sndfile_rw_open(AgsPlayable *playable, gchar *name,
			     gboolean create,
			     guint samplerate, guint channels,
			     guint frames,
			     guint format);
guint ags_sndfile_level_count(AgsPlayable *playable);
gchar** ags_sndfile_sublevel_names(AgsPlayable *playable);
void ags_sndfile_iter_start(AgsPlayable *playable);
gboolean ags_sndfile_iter_next(AgsPlayable *playable);
void ags_sndfile_info(AgsPlayable *playable,
		      guint *channels, guint *frames,
		      guint *loop_start, guint *loop_end,
		      GError **error);
signed short* ags_sndfile_read(AgsPlayable *playable, guint channel, GError **error);
void ags_sndfile_write(AgsPlayable *playable, signed short *buffer, guint buffer_length);
void ags_sndfile_flush(AgsPlayable *playable);
void ags_sndfile_seek(AgsPlayable *playable, guint frames, gint whence);
void ags_sndfile_close(AgsPlayable *playable);

sf_vio_get_filelen ags_sndfile_vio_get_filelen(void *user_data);
sf_vio_seek ags_sndfile_vio_seek(sf_count_t offset, int whence, void *user_data);
sf_vio_read ags_sndfile_vio_read(void *ptr, sf_count_t count, void *user_data);
sf_vio_write ags_sndfile_vio_write(const void *ptr, sf_count_t count, void *user_data);
sf_vio_tell ags_sndfile_vio_tell(const void *ptr, sf_count_t count, void *user_data);

static gpointer ags_sndfile_parent_class = NULL;
static AgsConnectableInterface *ags_sndfile_parent_connectable_interface;
static AgsPlayableInterface *ags_sndfile_parent_playable_interface;
static SF_VIRTUAL_IO *ags_sndfile_virtual_io = NULL;

GType
ags_sndfile_get_type()
{
  static GType ags_type_sndfile = 0;

  if(!ags_type_sndfile){
    static const GTypeInfo ags_sndfile_info = {
      sizeof (AgsSndfileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sndfile_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSndfile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sndfile_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sndfile_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_playable_interface_info = {
      (GInterfaceInitFunc) ags_sndfile_playable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_sndfile = g_type_register_static(G_TYPE_OBJECT,
					      "AgsSndfile\0",
					      &ags_sndfile_info,
					      0);

    g_type_add_interface_static(ags_type_sndfile,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_sndfile,
				AGS_TYPE_PLAYABLE,
				&ags_playable_interface_info);
  }
  
  return (ags_type_sndfile);
}

void
ags_sndfile_class_init(AgsSndfileClass *sndfile)
{
  GObjectClass *gobject;

  ags_sndfile_parent_class = g_type_class_peek_parent(sndfile);

  gobject = (GObjectClass *) sndfile;

  gobject->finalize = ags_sndfile_finalize;

  /* sndfile callbacks */
  if(ags_sndfile_virtual_io == NULL){
    ags_sndfile_virtual_io = (SF_VIRTUAL_IO *) malloc(sizeof(SF_VIRTUAL_IO));

    ags_sndfile_virtual_io->get_filelen = ags_sndfile_vio_get_filelen;
    ags_sndfile_virtual_io->seek = ags_sndfile_vio_seek;
    ags_sndfile_virtual_io->read = ags_sndfile_vio_read;
    ags_sndfile_virtual_io->write = ags_sndfile_vio_write;
    ags_sndfile_virtual_io->tell = ags_sndfile_vio_tell;
  }
}

void
ags_sndfile_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_sndfile_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_sndfile_connect;
  connectable->disconnect = ags_sndfile_disconnect;
}

void
ags_sndfile_playable_interface_init(AgsPlayableInterface *playable)
{
  ags_sndfile_parent_playable_interface = g_type_interface_peek_parent(playable);

  playable->open = ags_sndfile_open;
  playable->rw_open = ags_sndfile_rw_open;

  playable->level_count = ags_sndfile_level_count;
  playable->sublevel_names = ags_sndfile_sublevel_names;
  playable->level_select = NULL;

  playable->iter_start = ags_sndfile_iter_start;
  playable->iter_next = ags_sndfile_iter_next;

  playable->info = ags_sndfile_info;
  playable->read = ags_sndfile_read;

  playable->write = ags_sndfile_write;
  playable->flush = ags_sndfile_flush;

  playable->seek = ags_sndfile_seek;

  playable->close = ags_sndfile_close;
}

void
ags_sndfile_init(AgsSndfile *sndfile)
{
  sndfile->flags = 0;

  sndfile->info = NULL;
  sndfile->file = NULL;

  sndfile->pointer = NULL;
  sndfile->current = NULL;
  sndfile->length = 0;
}

void
ags_sndfile_connect(AgsConnectable *connectable)
{
  ags_sndfile_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_sndfile_disconnect(AgsConnectable *connectable)
{
  ags_sndfile_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

gboolean
ags_sndfile_open(AgsPlayable *playable, gchar *name)
{
  AgsSndfile *sndfile;

  sndfile = AGS_SNDFILE(playable);

  sndfile->info = (SF_INFO *) malloc(sizeof(SF_INFO));
  sndfile->info->format = 0;

  if((AGS_SNDFILE_VIRTUAL & (sndfile->flags)) == 0){
    if(name != NULL){
      sndfile->file = (SNDFILE *) sf_open(name, SFM_READ, sndfile->info);
    }
  }else{
    sndfile->file = (SNDFILE *) sf_open_virtual(ags_sndfile_virtual_io, SFM_READ, sndfile->info, sndfile);
  }

  if(sndfile->file == NULL)
    return(FALSE);
  else
    return(TRUE);
}

gboolean
ags_sndfile_rw_open(AgsPlayable *playable, gchar *name,
		    gboolean create,
		    guint samplerate, guint channels,
		    guint frames,
		    guint format)
{
  AgsSndfile *sndfile;
  sf_count_t multi_frames;
  
  sndfile = AGS_SNDFILE(playable);

  sndfile->info = (SF_INFO *) malloc(sizeof(SF_INFO));

  sndfile->info->samplerate = samplerate;
  sndfile->info->channels = channels;
  sndfile->info->format = format;
  sndfile->info->frames = 0;

  g_message("export to: %s\n  samplerate: %d\n  channels: %d\n  format: %x\0",
	    name,
	    samplerate,
	    channels,
	    format);

  if(!sf_format_check(sndfile->info)){
    g_warning("invalid format");
  }

  if((AGS_SNDFILE_VIRTUAL & (sndfile->flags)) == 0){
    if(name != NULL){
      sndfile->file = (SNDFILE *) sf_open(name, SFM_RDWR, sndfile->info);
    }
  }else{
    sndfile->file = (SNDFILE *) sf_open_virtual(ags_sndfile_virtual_io, SFM_RDWR, sndfile->info, sndfile);
  }

  multi_frames = frames * sndfile->info->channels;
  //  sf_command(sndfile->file, SFC_FILE_TRUNCATE, &(multi_frames), sizeof(multi_frames));
  //  sf_command (sndfile, SFC_SET_SCALE_INT_FLOAT_WRITE, NULL, SF_TRUE);
  sf_seek(sndfile->file, 0, SEEK_SET);

  //  sndfile->info->frames = multi_frames;

  if(sndfile->file == NULL)
    return(FALSE);
  else
    return(TRUE);
}

guint
ags_sndfile_level_count(AgsPlayable *playable)
{
  return(1);
}

gchar**
ags_sndfile_sublevel_names(AgsPlayable *playable)
{
  return(NULL);
}

void
ags_sndfile_iter_start(AgsPlayable *playable)
{
  AgsSndfile *sndfile;

  sndfile = AGS_SNDFILE(playable);

  sndfile->flags |= AGS_SNDFILE_ITER_START;
}

gboolean
ags_sndfile_iter_next(AgsPlayable *playable)
{
  AgsSndfile *sndfile;

  sndfile = AGS_SNDFILE(playable);

  if((AGS_SNDFILE_ITER_START & (sndfile->flags)) != 0){
    sndfile->flags &= (~AGS_SNDFILE_ITER_START);

    return(TRUE);
  }else{
    return(FALSE);
  }
}

void
ags_sndfile_info(AgsPlayable *playable,
		 guint *channels, guint *frames,
		 guint *loop_start, guint *loop_end,
		 GError **error)
{
  AgsSndfile *sndfile;

  sndfile = AGS_SNDFILE(playable);

  *channels = sndfile->info->channels;
  *frames = sndfile->info->frames;
  *loop_start = 0;
  *loop_end = 0;
}

signed short*
ags_sndfile_read(AgsPlayable *playable, guint channel, GError **error)
{
  AgsSndfile *sndfile;
  signed short *buffer, *source;
  guint i;

  sndfile = AGS_SNDFILE(playable);

  source = (signed short *) malloc((size_t) sndfile->info->channels *
				   sndfile->info->frames *
				   sizeof(signed short));
  
  sf_seek(sndfile->file, 0, SEEK_SET);
  sf_read_short(sndfile->file, source, sndfile->info->frames * sndfile->info->channels);

  buffer = (signed short *) malloc((size_t) sndfile->info->frames *
				   sizeof(signed short));

  for(i = 0; i < sndfile->info->frames; i++){
    buffer[i] = source[i * sndfile->info->channels + channel];
  }

  free(source);

  return(buffer);
}

void
ags_sndfile_write(AgsPlayable *playable, signed short *buffer, guint buffer_length)
{
  AgsSndfile *sndfile;
  sf_count_t multi_frames, retval;
  unsigned char *interleaved_buffer;
  double scale = 1.0 / G_MAXINT16 * G_MAXINT32;
  guint i, j;
  guint channel, offset;
  guint mask = 0xff;

  sndfile = AGS_SNDFILE(playable);

  multi_frames = buffer_length * (1.0 / (sndfile->info->channels * (AGS_DEVOUT_DEFAULT_FORMAT / 8)));
  interleaved_buffer = (unsigned char *) malloc(multi_frames * (AGS_DEVOUT_DEFAULT_FORMAT / 8) * sizeof(unsigned char));

  for(i = 0; i < multi_frames; i++){
    for(j = 0; j < AGS_DEVOUT_DEFAULT_FORMAT / 8; j++){
      channel = i % sndfile->info->channels;
      offset = i * sndfile->info->channels;
      interleaved_buffer[i * (AGS_DEVOUT_DEFAULT_FORMAT / 8) + j] = ((mask << (j * 8)) & (buffer[i + channel + offset])) >> (j * 8);

    }
  }

  retval = sf_write_raw(sndfile->file, interleaved_buffer, multi_frames);

  if(retval > multi_frames){
    g_warning("retval > multi_frames");
    //    sf_seek(sndfile->file, (multi_frames - retval), SEEK_CUR);
  }
}

void
ags_sndfile_flush(AgsPlayable *playable)
{
  AgsSndfile *sndfile;

  sndfile = AGS_SNDFILE(playable);

  sf_write_sync(sndfile->file);
}

void
ags_sndfile_seek(AgsPlayable *playable, guint frames, gint whence)
{
  AgsSndfile *sndfile;

  sndfile = AGS_SNDFILE(playable);

  sf_seek(sndfile->file, frames, whence);
}

void
ags_sndfile_close(AgsPlayable *playable)
{
  AgsSndfile *sndfile;

  sndfile = AGS_SNDFILE(playable);

  sf_close(sndfile->file);
  free(sndfile->info);
}

void
ags_sndfile_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_sndfile_parent_class)->finalize(gobject);

  /* empty */
}

sf_vio_get_filelen
ags_sndfile_vio_get_filelen(void *user_data)
{
  return(AGS_SNDFILE(user_data)->length);
}

sf_vio_seek
ags_sndfile_vio_seek(sf_count_t offset, int whence, void *user_data)
{
  switch(whence){
  case SEEK_CUR:
    AGS_SNDFILE(user_data)->current += offset;
  case SEEK_SET:
    AGS_SNDFILE(user_data)->current = &(AGS_SNDFILE(user_data)->pointer[offset]);
  case SEEK_END:
    AGS_SNDFILE(user_data)->current = &(AGS_SNDFILE(user_data)->pointer[AGS_SNDFILE(user_data)->length - offset]);
  }

  return(AGS_SNDFILE(user_data)->current - AGS_SNDFILE(user_data)->pointer);
}

sf_vio_read
ags_sndfile_vio_read(void *ptr, sf_count_t count, void *user_data)
{
  guchar *retval;

  retval = memcpy(ptr, AGS_SNDFILE(user_data)->current, count * sizeof(guchar));

  return(retval - AGS_SNDFILE(user_data)->pointer);
}

sf_vio_write
ags_sndfile_vio_write(const void *ptr, sf_count_t count, void *user_data)
{
  guchar *retval;

  retval = memcpy(AGS_SNDFILE(user_data)->current, ptr, count * sizeof(guchar));

  return(retval - AGS_SNDFILE(user_data)->pointer);
}

sf_vio_tell
ags_sndfile_vio_tell(const void *ptr, sf_count_t count, void *user_data)
{
  return(AGS_SNDFILE(user_data)->current - AGS_SNDFILE(user_data)->pointer);
}

AgsSndfile*
ags_sndfile_new()
{
  AgsSndfile *sndfile;

  sndfile = (AgsSndfile *) g_object_new(AGS_TYPE_SNDFILE,
					NULL);

  return(sndfile);
}
