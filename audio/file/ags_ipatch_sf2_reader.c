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
void ags_ipatch_sf2_reader_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_ipatch_sf2_reader_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
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

enum{
  PROP_0,
  PROP_IPATCH,
};

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

  gobject->set_property = ags_ipatch_sf2_reader_set_property;
  gobject->get_property = ags_ipatch_sf2_reader_get_property;

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
  playable->level_up = ags_ipatch_sf2_reader_level_up;

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
  ipatch_sf2_reader->selected_sublevel_name = NULL;

  ipatch_sf2_reader->reader = NULL;
  ipatch_sf2_reader->preset = NULL;
  ipatch_sf2_reader->instrument = NULL;
  ipatch_sf2_reader->sample = NULL;
}

void
ags_ipatch_sf2_reader_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsAudio *audio;

  audio = AGS_AUDIO(gobject);

  switch(prop_id){
  case PROP_IPATCH:
    {
      AgsDevout *devout;

      ipatch = (AgsDevout *) g_value_get_object(value);

      if(ipatch_sf2_reader->ipatch != NULL){
	g_object_unref(ipatch_sf2_reader->ipatch);
      }

      ipatch_sf2_reader->ipatch = ipatch;
     
      if(ipatch != NULL){
	g_object_ref(ipatch);
	ipatch->reader = ipatch_sf2_reader;
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ipatch_sf2_reader_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsIpatchSf2Reader *ipatch_sf2_reader;

  ipatch_sf2_reader = AGS_AUDIO(gobject);

  switch(prop_id){
  case PROP_IPATCH:
    g_value_set_object(value, ipatch_sf2_reader->ipatch);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
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

  ipatch_sf2_reader->sf2 = ipatch_sf2_reader_load(ipatch_sf2_reader->reader,
						  &error);
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
  GList *list;
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
	*iter = ipatch_sf2_reader->ipatch->filename;
	break;
      }
    case AGS_SF2_PHDR:
      {
	IpatchSF2Phdr phdr;

	if(initial){
	  ipatch_sf2_get_presets(ipatch_sf2_reader->sf2);
	  list = ipatch_container_get_children(IPATCH_CONTAINER(ipatch_sf2_reader->sf2),
					       IPATCH_TYPE_SF2_PRESET);
	  
	  /*
	  int sf2_phdr_position;
	  int offset;

	  offset = ;
	  lseek(fd, SEEK_SET, fd_pos0);
	  lseek(fd, SEEK_SET, offset);

	  fcntl(ipatch_get_fd(ipatch_sf2_reader->ipatch), FD_SET, &fd_pos0, NULL);
	  fcntl(ipatch_get_fd(ipatch_sf2_reader->ipatch), FD_SET, &offset, NULL);

	  read(fd, &sf2_phdr_position, sizeof(int));
	  sf2_phdr_position -= offset;

	  ipatch_buf_seek(sf2_phdr_position);
	  */
	}

	/*
	*iter = ipatch_sf2_reader_load_phdr(ipatch->handle,
					    phdr);

	ipatch_buf_seek(sizeof(IpatchSF2Phdr));
	*/

	*iter = ((IpatchSF2Phdr *) (list->data))->name;
      }
    case AGS_SF2_IHDR:
      {
	IpatchSF2Ihdr ihdr;

	if(initial){
	  ipatch_sf2_get_insts(ipatch_sf2_reader->sf2);
	  list = ipatch_container_get_children(IPATCH_CONTAINER(ipatch_sf2_reader->sf2),
					       IPATCH_TYPE_SF2_INST);

	  /*
	  int sf2_ihdr_position;
	  int offset;
	  
	  offset = ;
	  lseek(fd, SEEK_SET, fd_pos0);
	  lseek(fd, SEEK_SET, offset);

	  read(fd, &sf2_phdr_position, sizeof(int));
	  sf2_phdr_position -= offset;

	  ipatch_buf_seek(sf2_ihdr_position);
	  */
	}

	/*
	*iter = ipatch_sf2_reader_load_phdr(ipatch->handle,
					    ihdr);

	ipatch_buf_seek(sizeof(IpatchSF2Ihdr));
	*/

	*iter = ((IpatchSF2Ihdr *) (list->data))->name;
      }
    case AGS_SF2_SHDR:
      {
	IpatchSF2Shdr shdr;

	if(initial){
	  ipatch_sf2_get_samples(ipatch_sf2_reader->sf2);
	  list = ipatch_container_get_children(IPATCH_CONTAINER(ipatch_sf2_reader->sf2),
					       IPATCH_TYPE_SF2_SAMPLE);
	  /*
	  int sf2_shdr_position;
	  int offset;
	
	  offset = ;
	  lseek(fd, SEEK_SET, fd_pos0);
	  lseek(fd, SEEK_SET, offset);

	  read(fd, &sf2_phdr_position, sizeof(int));
	  sf2_phdr_position -= offset;

	  ipatch_buf_seek(sf2_shdr_position);
	  */
	}

	/*
	*iter = ipatch_sf2_reader_load_phdr(ipatch->handle,
					    shdr);

	ipatch_buf_seek(sizeof(IpatchSF2Shdr));
	*/

	*iter = ((IpatchSF2Shdr *) (list->data))->name;
      }
    };

    list = list->next;
    (*iter)++;
  }

  *iter = NULL;

  return(names);
}

void
ags_ipatch_sf2_reader_level_select(AgsPlayable *playable,
				   guint nth_level, gchar *sublevel_name,
				   GError **error)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(playable);

  if(sublevel_name == NULL){
    ipatch_sf2_reader->nth_level = 0;
    ipatch_sf2_reader->selected_sublevel_name = NULL;
  }else{
    gchar **sublevel_names;

    sublevel_names = ags_ipatch_sf2_reader_sublevel_names(playable);

    while(!strncmp(sublevel_names, sublevel_names, 20)){
      sublevel_names++;
    }
   
    if(*sublevel_names != NULL){
      ipatch_sf2_reader->nth_level++;
    }else{
      g_set_error(error,
		  AGS_PLAYABLE_ERROR,
		  AGS_PLAYABLE_ERROR_NO_SUCH_LEVEL,
		  "no level called %s in soundfont2 file: %s\0",
		  sublevel_name, ipatch_sf2_reader->ipatch->filename);
    }
  }
}

void
ags_ipatch_sf2_reader_iter_level_up(AgsPlayable *playable, guint levels, GError **error)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(playable);

  if(ipatch_sf2_reader->nth_level >= levels){
    ipatch_sf2_reader->nth_level -= levels;
  }else{
    g_set_error(error,
		AGS_CHANNEL_ERROR,
		AGS_CHANNEL_ERROR_LOOP_IN_LINK,
		"Not able to go %u steps higher in soundfont2 file: %s\0",
		sublevel_name, ipatch_sf2_reader->ipatch->filename);
  }
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
ags_ipatch_sf2_reader_info(AgsPlayable *playable,
			   guint *channels, guint *frames,
			   guint *loop_start, guint *loop_end)
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
