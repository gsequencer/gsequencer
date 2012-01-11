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

#include <ags/audio/file/ags_ipatch_sf2_reader.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_playable.h>

void ags_ipatch_sf2_reader_class_init(AgsIpatchSf2ReaderClass *ipatch_sf2_reader);
void ags_ipatch_sf2_reader_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ipatch_sf2_reader_playable_interface_init(AgsPlayableInterface *playable);
void ags_ipatch_sf2_reader_init(AgsIpatchSf2Reader *ipatch_sf2_reader);
void ags_ipatch_sf2_reader_finalize(GObject *gobject);

void ags_ipatch_sf2_reader_connect(AgsConnectable *connectable);
void ags_ipatch_sf2_reader_disconnect(AgsConnectable *connectable);

gboolean ags_ipatch_sf2_reader_open(AgsPlayable *playable, gchar *name);

guint ags_ipatch_sf2_reader_level_count(AgsPlayable *playable);
gchar** ags_ipatch_sf2_reader_sublevel_names(AgsPlayable *playable);
void ags_ipatch_sf2_reader_level_select(AgsPlayable *playable, guint nth_level, gchar *sublevel_name);

void ags_ipatch_sf2_reader_iter_start(AgsPlayable *playable);
gboolean ags_ipatch_sf2_reader_iter_next(AgsPlayable *playable);

void ags_ipatch_sf2_reader_info(AgsPlayable *playable, guint *channels, guint *frames, guint *loop_start, guint *loop_end);
short* ags_ipatch_sf2_reader_read(AgsPlayable *playable, guint channel);

void ags_ipatch_sf2_reader_close(AgsPlayable *playable);

static gpointer ags_ipatch_sf2_reader_parent_class = NULL;
static AgsConnectableInterface *ags_ipatch_sf2_reader_parent_connectable_interface;
static AgsPlayableInterface *ags_ipatch_sf2_reader_parent_playable_interface;

GType
ags_ipatch_sf2_reader_get_type()
{
  static GType ags_type_ipatch_sf2_reader = 0;

  if(!ags_type_ipatch_sf2_reader){
    static const GTypeInfo ags_ipatch_sf2_reader_info = {
      sizeof (AgsIpatchSf2ReaderClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ipatch_sf2_reader_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsIpatchSf2Reader),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ipatch_sf2_reader_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ipatch_sf2_reader_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_playable_interface_info = {
      (GInterfaceInitFunc) ags_ipatch_sf2_reader_playable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ipatch_sf2_reader = g_type_register_static(G_TYPE_OBJECT,
					     "AgsIpatchSf2Reader\0",
					     &ags_ipatch_sf2_reader_info,
					     0);

    g_type_add_interface_static(ags_type_ipatch_sf2_reader,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_ipatch_sf2_reader,
				AGS_TYPE_PLAYABLE,
				&ags_playable_interface_info);
  }
  
  return (ags_type_ipatch_sf2_reader);
}

void
ags_ipatch_sf2_reader_class_init(AgsIpatchSf2ReaderClass *ipatch_sf2_reader)
{
  GObjectClass *gobject;

  ags_ipatch_sf2_reader_parent_class = g_type_class_peek_parent(ipatch_sf2_reader);

  gobject = (GObjectClass *) ipatch_sf2_reader;

  gobject->finalize = ags_ipatch_sf2_reader_finalize;
}

void
ags_ipatch_sf2_reader_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_ipatch_sf2_reader_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_ipatch_sf2_reader_connect;
  connectable->disconnect = ags_ipatch_sf2_reader_disconnect;
}

void
ags_ipatch_sf2_reader_playable_interface_init(AgsPlayableInterface *playable)
{
  ags_ipatch_sf2_reader_parent_playable_interface = g_type_interface_peek_parent(playable);

  playable->open = ags_ipatch_sf2_reader_open;

  playable->level_count = ags_ipatch_sf2_reader_level_count;
  playable->sublevel_names = ags_ipatch_sf2_reader_sublevel_names;
  playable->level_select = ags_ipatch_sf2_reader_level_select;

  playable->iter_start = ags_ipatch_sf2_reader_iter_start;
  playable->iter_next = ags_ipatch_sf2_reader_iter_next;

  playable->info = ags_ipatch_sf2_reader_info;
  playable->read = ags_ipatch_sf2_reader_read;

  playable->close = ags_ipatch_sf2_reader_close;
}

void
ags_ipatch_sf2_reader_init(AgsIpatchSf2Reader *ipatch_sf2_reader)
{
  ipatch_sf2_reader->ipatch = NULL;

  ipatch_sf2_reader->nth_level = 0;

  ipatch_sf2_reader->reader = NULL;
  ipatch_sf2_reader->preset = NULL;
  ipatch_sf2_reader->instrument = NULL;
  ipatch_sf2_reader->sample = NULL;
}

void
ags_ipatch_sf2_reader_connect(AgsConnectable *connectable)
{
  ags_ipatch_sf2_reader_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_ipatch_sf2_reader_disconnect(AgsConnectable *connectable)
{
  ags_ipatch_sf2_reader_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

gboolean
ags_ipatch_sf2_reader_open(AgsPlayable *playable, gchar *name)
{
  AgsIpatch *ipatch_file;
  AgsIpatchSf2Reader *ipatch_sf2_reader;
  gchar *filename;
  gchar *mode;
  GError *error;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(playable);

  ipatch_file = g_object_new(AGS_TYPE_IPATCH,
			     "filename\0", filename,
			     "mode\0", mode,
			     NULL);

  error = NULL;
  ags_ipatch_open(AGS_PLAYABLE(ipatch_file), &error);

  ipatch_sf2_reader->reader = ipatch_sf2_reader_new(ipatch_file->handle);
}

guint
ags_ipatch_sf2_reader_level_count(AgsPlayable *playable)
{
  return(3);
}

gchar**
ags_ipatch_sf2_reader_sublevel_names(AgsPlayable *playable)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;
  AgsIpatch *ipatch;
  gchar **names, **iter;
  gchar *name;
  gboolean initial;

  ags_ipatch_sf2_reader_iter_start(playable);

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(playable);
  ipatch = ipatch_sf2_reader->ipatch;

  names = (gchar *) malloc(1 * sizeof(gchar));
  names[0] = NULL;
  iter = names;

  initial = TRUE;

  while(ags_ipatch_sf2_iter_next(playable) != FALSE){
    realloc(names, iter - names + 1);

    switch(ipatch_sf2_reader->nth_level){
    case AGS_SF2_FILENAME:
      {
	*iter = ags_ipatch_sf2_reader();
	break;
      }
    case AGS_SF2_PHDR:
      {
	IpatchSF2Phdr phdr;

	if(initial){
	  int sf2_phdr_position;
	  int offset;
	  int position;

	  sf2_phdr_position = ;

	  fcntl(ipatch_get_fd(ipatch_sf2_reader->ipatch), FD_, &offset, NULL);
	  ipatch_buf_seek(AGS_SF2_PHDR_POSITION);
	}

	*iter = ipatch_sf2_reader_load_phdr(ipatch->handle,
					    phdr);

	ipatch_buf_seek(sizeof(IpatchSF2Phdr));
      }
    case AGS_SF2_IHDR:
      {
	IpatchSF2Ihdr ihdr;

	if(initial){
	  int sf2_ihdr_position;
	  int offset;
	  int position;

	  sf2_ihdr_position = ;

	  fcntl(ipatch_get_fd(ipatch_sf2_reader->ipatch), FD_, &offset, NULL);
	  ipatch_buf_seek(AGS_SF2_IHDR_POSITION);
	}

	*iter = ipatch_sf2_reader_load_phdr(ipatch->handle,
					    ihdr);

	ipatch_buf_seek(sizeof(IpatchSF2Phdr));
      }
    case AGS_SF2_SHDR:
      {
	IpatchSF2Shdr shdr;

	if(initial){
	  int sf2_shdr_position;
	  int offset;
	  int position;
	
	  sf2_shdr_position = ;

	  fcntl(ipatch_get_fd(ipatch_sf2_reader->ipatch), FD_, &offset, NULL);
	  ipatch_buf_seek(AGS_SF2_SHDR_POSITION);
	}

	*iter = ipatch_sf2_reader_load_phdr(ipatch->handle,
					    shdr);

	ipatch_buf_seek(sizeof(IpatchSF2Phdr));
      }
    };

    (*iter)++;
    *iter = NULL;
  }

  return(names);
}

void
ags_ipatch_sf2_reader_level_select(AgsPlayable *playable, guint nth_level, gchar *sublevel_name)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(playable);

}

void
ags_ipatch_sf2_reader_iter_start(AgsPlayable *playable)
{
  AgsIpatchSf2Reader *ipatch_sf2_reader;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER_SF2_READER(playable);

  //TODO:JK: implement me
}

gboolean
ags_ipatch_sf2_reader_iter_next(AgsPlayable *playable)
{
  AgsIpatchSf2Reader *ipatch_sf2_reader;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER_SF2_READER(playable);

  //TODO:JK: implement me
}

void
ags_ipatch_sf2_reader_info(AgsPlayable *playable, guint *channels, guint *frames, guint *loop_start, guint *loop_end)
{
  AgsIpatchSf2Reader *ipatch_sf2_reader;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER_SF2_READER(playable);

  //TODO:JK: implement me
}

short*
ags_ipatch_sf2_reader_read(AgsPlayable *playable, guint channel)
{
  AgsIpatchSf2Reader *ipatch_sf2_reader;
  short *buffer, *source;
  guint i;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER_SF2_READER(playable);

  //TODO:JK: implement me
}

void
ags_ipatch_sf2_reader_close(AgsPlayable *playable)
{
  AgsIpatchSf2Reader *ipatch_sf2_reader;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER_SF2_READER(playable);

  //TODO:JK: implement me
}

void
ags_ipatch_sf2_reader_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_ipatch_sf2_reader_parent_class)->finalize(gobject);

  /* empty */
}

AgsIpatchSf2Reader*
ags_ipatch_sf2_reader_new()
{
  AgsIpatchSf2Reader *ipatch_sf2_reader;

  ipatch_sf2_reader = (AgsIpatchSf2Reader *) g_object_new(AGS_TYPE_IPATCH_SF2_READER,
							  NULL);

  return(ipatch_sf2_reader);
}
