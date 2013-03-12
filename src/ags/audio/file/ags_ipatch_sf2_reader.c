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

#include <stdlib.h>

void ags_ipatch_sf2_reader_class_init(AgsIpatchSF2ReaderClass *ipatch_sf2_reader);
void ags_ipatch_sf2_reader_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ipatch_sf2_reader_playable_interface_init(AgsPlayableInterface *playable);
void ags_ipatch_sf2_reader_init(AgsIpatchSF2Reader *ipatch_sf2_reader);
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
guint ags_ipatch_sf2_reader_nth_level(AgsPlayable *playable);
gchar* ags_ipatch_sf2_reader_selected_level(AgsPlayable *playable);

gchar** ags_ipatch_sf2_reader_sublevel_names(AgsPlayable *playable);
void ags_ipatch_sf2_reader_level_select(AgsPlayable *playable,
					guint nth_level, gchar *sublevel_name,
					GError **error);
void ags_ipatch_sf2_reader_level_up(AgsPlayable *playable,
				    guint levels,
				    GError **error);

void ags_ipatch_sf2_reader_iter_start(AgsPlayable *playable);
gboolean ags_ipatch_sf2_reader_iter_next(AgsPlayable *playable);

void ags_ipatch_sf2_reader_info(AgsPlayable *playable,
				guint *channels, guint *frames,
				guint *loop_start, guint *loop_end,
				GError **error);
short* ags_ipatch_sf2_reader_read(AgsPlayable *playable,
				  guint channel,
				  GError **error);

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
      sizeof (AgsIpatchSF2ReaderClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ipatch_sf2_reader_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsIpatchSF2Reader),
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
					     "AgsIpatchSF2Reader",
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
ags_ipatch_sf2_reader_class_init(AgsIpatchSF2ReaderClass *ipatch_sf2_reader)
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
  playable->nth_level = ags_ipatch_sf2_reader_nth_level;
  playable->selected_level = ags_ipatch_sf2_reader_selected_level;

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
ags_ipatch_sf2_reader_init(AgsIpatchSF2Reader *ipatch_sf2_reader)
{
  ipatch_sf2_reader->ipatch = NULL;

  ipatch_sf2_reader->nth_level = 0;
  ipatch_sf2_reader->bank = -1;
  ipatch_sf2_reader->program = -1;
  ipatch_sf2_reader->selected_sublevel_name = NULL;

  ipatch_sf2_reader->selected = (gchar **) malloc(4 * sizeof(gchar *));

  ipatch_sf2_reader->reader = NULL;
}

void
ags_ipatch_sf2_reader_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(gobject);

  switch(prop_id){
  case PROP_IPATCH:
    {
      AgsIpatch *ipatch;

      ipatch = (AgsIpatch *) g_value_get_object(value);

      if(ipatch_sf2_reader->ipatch != NULL){
	g_object_unref(ipatch_sf2_reader->ipatch);
      }

      ipatch_sf2_reader->ipatch = ipatch;
     
      if(ipatch != NULL){
	g_object_ref(ipatch);
	ipatch->reader = (GObject *) ipatch_sf2_reader;
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
  AgsIpatchSF2Reader *ipatch_sf2_reader;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(gobject);

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
ags_ipatch_sf2_reader_open(AgsPlayable *playable, gchar *filename)
{
  AgsIpatch *ipatch_file;
  AgsIpatchSF2Reader *ipatch_sf2_reader;
  gchar *mode;
  GError *error;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(playable);

  ipatch_file = ipatch_sf2_reader->ipatch;

  ipatch_sf2_reader->reader = ipatch_sf2_reader_new(ipatch_file->handle);

  error = NULL;
  ipatch_sf2_reader->sf2 = ipatch_sf2_reader_load(ipatch_sf2_reader->reader,
						  &error);

  if(error != NULL){
    g_error("%s", error->message);
  }
}

guint
ags_ipatch_sf2_reader_level_count(AgsPlayable *playable)
{
  return(3);
}

guint
ags_ipatch_sf2_reader_nth_level(AgsPlayable *playable)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(playable);

  return(ipatch_sf2_reader->nth_level);
}

gchar*
ags_ipatch_sf2_reader_selected_level(AgsPlayable *playable)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(playable);

  return(ipatch_sf2_reader->selected_sublevel_name);
}

gchar**
ags_ipatch_sf2_reader_sublevel_names(AgsPlayable *playable)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;
  AgsIpatch *ipatch;
  IpatchList *ipatch_list;
  GList *list;
  gchar **names;
  gchar *name;
  guint sublevel;
  guint i;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(playable);
  ipatch = ipatch_sf2_reader->ipatch;

  names = (gchar **) malloc(1 * sizeof(gchar*));
  names[0] = NULL;

  sublevel = ipatch_sf2_reader->nth_level + 1;

  g_message("ags_ipatch_sf2_reader_sublevel_names: %u\n", sublevel);

  switch(sublevel){
  case AGS_SF2_FILENAME:
    {
      names = realloc(names, 2 * sizeof(char*));

      names[0] = ipatch_sf2_reader->ipatch->filename;
      names[1] = NULL;
      return(names);
    }
  case AGS_SF2_PHDR:
    {
      ipatch_list = ipatch_sf2_get_presets(ipatch_sf2_reader->sf2);
      
      if(ipatch_list != NULL){
	list = ipatch_list->items;
      }else{
	return(names);
      }
    }
    break;
  case AGS_SF2_IHDR:
    {
      IpatchContainer *container;

      ipatch_list = ipatch_sf2_get_insts(ipatch_sf2_reader->sf2);
      
      if(ipatch_list != NULL){
	list = ipatch_list->items;
      }else{
	return(names);
      }
    }
    break;
  case AGS_SF2_SHDR:
    {
      IpatchContainer *container;
      
      ipatch_list = ipatch_sf2_get_samples(ipatch_sf2_reader->sf2);
      
      if(ipatch_list != NULL){
	list = ipatch_list->items;
      }else{
	return(names);
      }
    }
    break;
  };

  for(i = 0; list != NULL; i++){
    names = realloc(names, (i + 2) * sizeof(char*));

    switch(sublevel){
    case AGS_SF2_PHDR:
      {
	if(IPATCH_IS_SF2_PRESET(list->data))
	  names[i] = g_strndup(IPATCH_SF2_PRESET(list->data)->name, 20);
      }
      break;
    case AGS_SF2_IHDR:
      {
	if(IPATCH_IS_SF2_INST(list->data))
	  names[i] = g_strndup(IPATCH_SF2_INST(list->data)->name, 20);
      }
      break;
    case AGS_SF2_SHDR:
      {
	if(IPATCH_IS_SF2_SAMPLE(list->data))
	  names[i] = g_strndup(IPATCH_SF2_SAMPLE(list->data)->name, 20);
      }
      break;
    };

    list = list->next;
  }

  names[i] = NULL;

  return(names);
}

void
ags_ipatch_sf2_reader_level_select(AgsPlayable *playable,
				   guint nth_level, gchar *sublevel_name,
				   GError **error)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;
  gboolean success;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(playable);

  //TODO:JK: apply mods and gens

  if(sublevel_name == NULL){
    ipatch_sf2_reader->nth_level = 0;
    ipatch_sf2_reader->selected_sublevel_name = NULL;
  }else{
    IpatchList *ipatch_list;
    GList *list;

    if(ipatch_sf2_reader->nth_level == 0 && !g_strcmp0(ipatch_sf2_reader->ipatch->filename, sublevel_name)){
      //      ipatch_sf2_reader->nth_level = 0;
      return;
    }

    if(ipatch_sf2_reader->nth_level == 1){
      /* preset */
      ipatch_list = ipatch_sf2_get_presets(ipatch_sf2_reader->sf2);
      list = ipatch_list->items;

      while(list != NULL && strncmp(IPATCH_SF2_PRESET(list->data)->name, sublevel_name, 20) != 0){
	list = list->next;
      }

      if(list != NULL){
	success = TRUE;

	/* some extra code for bank and program */
	ipatch_sf2_preset_get_midi_locale(IPATCH_SF2_PRESET(list->data),
					  &(ipatch_sf2_reader->bank),
					  &(ipatch_sf2_reader->program));
	
       g_message("debug: bank %d program %d\n", ipatch_sf2_reader->bank, ipatch_sf2_reader->program);
      }else{
	success = FALSE;
      }
    }else{
      if(ipatch_sf2_reader->nth_level == 2){
	/* instrument */
	ipatch_list = ipatch_sf2_get_insts(ipatch_sf2_reader->sf2);
	list = ipatch_list->items;
	
	while(list != NULL && strncmp(IPATCH_SF2_INST(list->data)->name, sublevel_name, 20) != 0){
	  list = list->next;
	}

	if(list != NULL){
	  success = TRUE;
	}else{
	  success = FALSE;
	}
      }else if(ipatch_sf2_reader->nth_level == 3){
	/* sample */
	ipatch_list = ipatch_sf2_get_samples(ipatch_sf2_reader->sf2);
	list = ipatch_list->items;
	
	while(list != NULL && strncmp(IPATCH_SF2_SAMPLE(list->data)->name, sublevel_name, 20) != 0){
	  list = list->next;
	}

	if(list != NULL){
	  success = TRUE;
	}else{
	  success = FALSE;
	}
      }else{
	success = FALSE;
      }
    }

    if(success){
      ipatch_sf2_reader->selected_sublevel_name = sublevel_name;
      //      ipatch_sf2_reader->nth_level += 1;
    }else{
      g_set_error(error,
		  AGS_PLAYABLE_ERROR,
		  AGS_PLAYABLE_ERROR_NO_SUCH_LEVEL,
		  "no level called %s in soundfont2 file: %s",
		  sublevel_name, ipatch_sf2_reader->ipatch->filename);
    }
  }
}

void
ags_ipatch_sf2_reader_level_up(AgsPlayable *playable, guint levels, GError **error)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;
  guint i;

  if(levels == 0)
    return;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(playable);

  g_message("debug: %u\n", ipatch_sf2_reader->nth_level);

  if(ipatch_sf2_reader->nth_level >= levels){
    ipatch_sf2_reader->nth_level -= levels;

    /* you may expect something like this but it's wrong */
    //    for(i = 0; i < levels; i++){
    //      ipatch_sf2_reader->selected[4 - i] = NULL;
    //    }

    ipatch_sf2_reader->selected_sublevel_name = ipatch_sf2_reader->selected[4 - i];
  }else{
    g_set_error(error,
		AGS_PLAYABLE_ERROR,
		AGS_PLAYABLE_ERROR_NO_SUCH_LEVEL,
		"Not able to go %u steps higher in soundfont2 file: %s",
		levels, ipatch_sf2_reader->ipatch->filename);
  }
}

void
ags_ipatch_sf2_reader_iter_start(AgsPlayable *playable)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;
  int count;
  IpatchIter *copy;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(playable);

  if(ipatch_sf2_reader->nth_level == 0 ||
     ipatch_sf2_reader->nth_level == 3){
    ipatch_sf2_reader->iter = NULL;
    ipatch_sf2_reader->count = 0;

    return;
  }
  
  ipatch_sf2_reader->iter = NULL;

  if(ipatch_sf2_reader->nth_level == 1){
    ipatch_sf2_reader->iter = ipatch_sf2_preset_get_zones(ipatch_sf2_find_preset(ipatch_sf2_reader->sf2,
										      ipatch_sf2_reader->selected_sublevel_name,
										      ipatch_sf2_reader->bank,
										      ipatch_sf2_reader->program,
										      NULL))->items;
  }else if(ipatch_sf2_reader->nth_level == 2){
    IpatchSF2Inst *inst;
    IpatchList *zones;

    inst = ipatch_sf2_find_inst(ipatch_sf2_reader->sf2,
				ipatch_sf2_reader->selected_sublevel_name,
				NULL);
    zones = ipatch_sf2_inst_get_zones(inst);
    ipatch_sf2_reader->iter = zones->items;
  }

  ipatch_sf2_reader->count = g_list_length(ipatch_sf2_reader->iter);
}

gboolean
ags_ipatch_sf2_reader_iter_next(AgsPlayable *playable)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(playable);

  if(ipatch_sf2_reader->iter != NULL){
    ipatch_sf2_reader->zone = IPATCH_SF2_ZONE(ipatch_sf2_reader->iter->data);

    ipatch_sf2_reader->iter = ipatch_sf2_reader->iter->next;
    return(TRUE);
  }else{
    ipatch_sf2_reader->zone = NULL;

    return(FALSE);
  }
}

void
ags_ipatch_sf2_reader_info(AgsPlayable *playable,
			   guint *channels, guint *frames,
			   guint *loop_start, guint *loop_end,
			   GError **error)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;
  IpatchSF2Sample *sample;
  IpatchSampleData *sample_data;
  IpatchSampleStoreSndFile *sample_store;
  IpatchList *ipatch_list;
  GList *list;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(playable);

  if(ipatch_sf2_reader->nth_level == 1 ||
     ipatch_sf2_reader->nth_level == 2){

    if(ipatch_sf2_reader->zone != NULL){
      g_object_get(IPATCH_SAMPLE(ipatch_sf2_reader->zone),
		   //		   "format", channels,
		   "sample-size", frames,
		   "loop-start", loop_start,
		   "loop-end", loop_end,
		   NULL);
      *channels = 2;
    }else{
      *channels = 0;
      *frames = 0;
      *loop_start = 0;
      *loop_end = 0;
    }
    g_message("  channels = %d \n frames = %d  \n", *channels, *frames);
  }else if(ipatch_sf2_reader->nth_level == 3){
    IpatchSF2Sample *sample;

    sample = ipatch_sf2_find_sample(ipatch_sf2_reader->sf2,
				    ipatch_sf2_reader->selected_sublevel_name,
				    NULL);

    if(ipatch_sf2_reader->zone != NULL){
      //TODO:JK: get endianess and set it for format
      g_object_get(G_OBJECT(sample),
		   //"format", channels,
		   "sample-size", frames,
		   "loop-start", loop_start,
		   "loop-end", loop_end,
		   NULL);

      *channels = 2;
    }else{
      *channels = 0;
      *frames = 0;
      *loop_start = 0;
      *loop_end = 0;
    }

    g_message("channels = %d\nframes = %d", *channels, *frames);
  }
}

short*
ags_ipatch_sf2_reader_read(AgsPlayable *playable,
			   guint channel,
			   GError **error)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;
  short *buffer;
  guint frames;
  GError *this_error;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(playable);

  if(ipatch_sf2_reader->nth_level == 1 ||
     ipatch_sf2_reader->nth_level == 2){

    if(ipatch_sf2_reader->zone != NULL){
      //TODO:JK: get endianess and set it for format
      frames = ipatch_sample_get_frame_size(IPATCH_SAMPLE(ipatch_sf2_reader->zone));
      buffer = (short *) malloc(2 * frames * sizeof(short));

      this_error = NULL;
      ipatch_sample_read_transform(IPATCH_SAMPLE(ipatch_sf2_reader->zone),
				   0,
				   frames,
				   buffer,
				   IPATCH_SAMPLE_16BIT | IPATCH_SAMPLE_MONO | IPATCH_SAMPLE_SIGNED,
				   IPATCH_SAMPLE_UNITY_CHANNEL_MAP,
				   &this_error);
      
      if(this_error != NULL){
	g_error("%s", this_error->message);
      }

      *error = this_error;
      
      return(buffer);
    }else{
      return(NULL);
    }
  }else if(ipatch_sf2_reader->nth_level == 3){
    IpatchSF2Sample *sample;

    sample = ipatch_sf2_find_sample(ipatch_sf2_reader->sf2,
				    ipatch_sf2_reader->selected_sublevel_name,
				    NULL);

    frames = ipatch_sample_get_frame_size(IPATCH_SAMPLE(sample));
    buffer = (short *) malloc(2 * frames * sizeof(short));

    if(ipatch_sf2_reader->zone != NULL){
      //TODO:JK: get endianess and set it for format
      this_error = NULL;

      ipatch_sample_read_transform(IPATCH_SAMPLE(sample),
				   0,
				   ipatch_sample_get_frame_size(IPATCH_SAMPLE(sample)),
				   buffer,
				   IPATCH_SAMPLE_16BIT | IPATCH_SAMPLE_STEREO | IPATCH_SAMPLE_SIGNED,
				   IPATCH_SAMPLE_UNITY_CHANNEL_MAP,
				   &this_error);
      
      if(this_error != NULL){
	g_error("%s", this_error->message);
      }

      *error = this_error;
      
      return(buffer);
    }else{
      return(NULL);
    }
  }
}

void
ags_ipatch_sf2_reader_close(AgsPlayable *playable)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(playable);

  //TODO:JK: implement me
}

void
ags_ipatch_sf2_reader_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_ipatch_sf2_reader_parent_class)->finalize(gobject);

  /* empty */
}

AgsIpatchSF2Reader*
ags_ipatch_sf2_reader_new()
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;

  ipatch_sf2_reader = (AgsIpatchSF2Reader *) g_object_new(AGS_TYPE_IPATCH_SF2_READER,
							  NULL);

  return(ipatch_sf2_reader);
}
