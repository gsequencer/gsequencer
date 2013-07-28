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
guint ags_sndfile_level_count(AgsPlayable *playable);
gchar** ags_sndfile_sublevel_names(AgsPlayable *playable);
void ags_sndfile_iter_start(AgsPlayable *playable);
gboolean ags_sndfile_iter_next(AgsPlayable *playable);
void ags_sndfile_info(AgsPlayable *playable,
		      guint *channels, guint *frames,
		      guint *loop_start, guint *loop_end,
		      GError **error);
signed short* ags_sndfile_read(AgsPlayable *playable, guint channel, GError **error);
void ags_sndfile_close(AgsPlayable *playable);

static gpointer ags_sndfile_parent_class = NULL;
static AgsConnectableInterface *ags_sndfile_parent_connectable_interface;
static AgsPlayableInterface *ags_sndfile_parent_playable_interface;

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

  playable->level_count = ags_sndfile_level_count;
  playable->sublevel_names = ags_sndfile_sublevel_names;
  playable->level_select = NULL;

  playable->iter_start = ags_sndfile_iter_start;
  playable->iter_next = ags_sndfile_iter_next;

  playable->info = ags_sndfile_info;
  playable->read = ags_sndfile_read;

  playable->close = ags_sndfile_close;
}

void
ags_sndfile_init(AgsSndfile *sndfile)
{
  sndfile->info = NULL;
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

  sndfile->file = (SNDFILE *) sf_open(name, SFM_READ, sndfile->info);

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

AgsSndfile*
ags_sndfile_new()
{
  AgsSndfile *sndfile;

  sndfile = (AgsSndfile *) g_object_new(AGS_TYPE_SNDFILE,
					NULL);

  return(sndfile);
}
