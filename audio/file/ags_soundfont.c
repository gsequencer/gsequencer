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

#include <ags/audio/file/ags_soundfont.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_playable.h>

void ags_soundfont_class_init(AgsSoundfontClass *soundfont);
void ags_soundfont_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_soundfont_playable_interface_init(AgsPlayableInterface *playable);
void ags_soundfont_init(AgsSoundfont *soundfont);
void ags_soundfont_connect(AgsConnectable *connectable);
void ags_soundfont_disconnect(AgsConnectable *connectable);
void ags_soundfont_finalize(GObject *gobject);

gboolean ags_soundfont_open(AgsPlayable *playable, gchar *filename);
guint ags_soundfont_level_count(AgsPlayable *playable);
gchar** ags_soundfont_sublevel_names(AgsPlayable *playable);
void ags_soundfont_level_select(AgsPlayable *playable, guint nth_level, gchar *sublevel_name);
void ags_soundfont_iter_start(AgsPlayable *playable);
gboolean ags_soundfont_iter_next(AgsPlayable *playable);
void ags_soundfont_start_sample(AgsPlayable *playable);
void ags_soundfont_next_sample(AgsPlayable *playable);
void ags_soundfont_info(AgsPlayable *playable, guint *channels, guint *frames, guint *loop_start, guint *loop_end);
short* ags_soundfont_read(AgsPlayable *playable, guint channel);
void ags_soundfont_close(AgsPlayable *playable);

gchar** ags_soundfont_get_banks(AgsSoundfont *soundfont);
void ags_soundfont_select_bank(AgsSoundfont *soundfont, gchar *bank_name);
gchar** ags_soundfont_get_samples(AgsSoundfont *soundfont);
void ags_soundfont_select_sample(AgsSoundfont *soundfont, gchar *sample_name);

static gpointer ags_soundfont_parent_class = NULL;
static AgsConnectableInterface *ags_soundfont_parent_connectable_interface;
static AgsPlayableInterface *ags_soundfont_parent_playable_interface;

GType
ags_soundfont_get_type()
{
  static GType ags_type_soundfont = 0;

  if(!ags_type_soundfont){
    static const GTypeInfo ags_soundfont_info = {
      sizeof (AgsSoundfontClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_soundfont_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSoundfont),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_soundfont_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_soundfont_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_playable_interface_info = {
      (GInterfaceInitFunc) ags_soundfont_playable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_soundfont = g_type_register_static(G_TYPE_OBJECT,
						"AgsSoundfont\0",
						&ags_soundfont_info,
						0);
    
    g_type_add_interface_static(ags_type_soundfont,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
    
    g_type_add_interface_static(ags_type_soundfont,
				AGS_TYPE_PLAYABLE,
				&ags_playable_interface_info);
  }
  
  return (ags_type_soundfont);
}

void
ags_soundfont_class_init(AgsSoundfontClass *soundfont)
{
  GObjectClass *gobject;

  ags_soundfont_parent_class = g_type_class_peek_parent(soundfont);

  gobject = (GObjectClass *) soundfont;

  gobject->finalize = ags_soundfont_finalize;
}

void
ags_soundfont_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_soundfont_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_soundfont_connect;
  connectable->disconnect = ags_soundfont_disconnect;
}

void
ags_soundfont_playable_interface_init(AgsPlayableInterface *playable)
{
  ags_soundfont_parent_playable_interface = g_type_interface_peek_parent(playable);

  playable->open = ags_soundfont_open;

  playable->level_count = ags_soundfont_level_count;
  playable->sublevel_names = ags_soundfont_sublevel_names;
  playable->level_select = ags_soundfont_level_select;

  playable->iter_start = ags_soundfont_iter_start;
  playable->iter_next = ags_soundfont_iter_next;

  playable->info = ags_soundfont_info;
  playable->read = ags_soundfont_read;

  playable->close = ags_soundfont_close;
}

void
ags_soundfont_init(AgsSoundfont *soundfont)
{
  soundfont->file = NULL;

  soundfont->selected_level = 0;

  soundfont->bank = NULL;

  soundfont->current_bank = NULL;
  soundfont->i_sample = 0;

  soundfont->current_sample = NULL;

  soundfont->audio_file = NULL;
}

void
ags_soundfont_connect(AgsConnectable *connectable)
{
}

void
ags_soundfont_disconnect(AgsConnectable *connectable)
{
}

void
ags_soundfont_finalize(GObject *gobject)
{
}

gboolean
ags_soundfont_open(AgsPlayable *playable, gchar *filename)
{
  AgsSoundfont *soundfont;
  auto gboolean ags_soundfont_read_level1(AgsSoundfont *soundfont);
  auto void ags_soundfont_read_level2(AgsSoundfont *soundfont);
  auto void ags_soundfont_read_level3(AgsSoundfont *soundfont);
  gboolean ags_soundfont_read_level1(AgsSoundfont *soundfont){
    FILE *file;
    char *data, *tmp;
    gchar *id;
    unsigned int chunk_size;

    file = soundfont->file;

    data = (char *) malloc(8 * sizeof(char));
    fread(data, sizeof(char), 8, file);

    /* get id */
    id = g_strndup((gchar *) data, 4);

    /* get chuck_size */
    tmp = g_strndup(&(data[4]), 4);
    chunk_size = strtoul(tmp, NULL, 10);
    g_free(tmp);

    return(TRUE);
  }
  void ags_soundfont_read_level2(AgsSoundfont *soundfont){
  }
  void ags_soundfont_read_level3(AgsSoundfont *soundfont){
  }

  soundfont = AGS_SOUNDFONT(playable);

  soundfont->file = fopen(filename, "rb\0");

  /* read meta info */
  ags_soundfont_read_level1(soundfont);
}

guint
ags_soundfont_level_count(AgsPlayable *playable)
{
  return(3);
}

gchar**
ags_soundfont_sublevel_names(AgsPlayable *playable)
{
  AgsSoundfont *soundfont;

  soundfont = AGS_SOUNDFONT(playable);

  switch(soundfont->selected_level){
  case 0:
    return(ags_soundfont_get_banks(soundfont));
  case 1:
    return(ags_soundfont_get_samples(soundfont));
  default:
    return(NULL);
  }
}

void
ags_soundfont_level_select(AgsPlayable *playable, guint nth_level, gchar *sublevel_name)
{
  AgsSoundfont *soundfont;

  soundfont = AGS_SOUNDFONT(playable);


  switch(nth_level){
  case 1:
    {
      ags_soundfont_select_bank(soundfont, sublevel_name);
      soundfont->selected_level = nth_level;
    }
    break;
  case 2:
    {
      ags_soundfont_select_sample(soundfont, sublevel_name);
      soundfont->selected_level = nth_level;
    }
    break;
  default:
    soundfont->selected_level = 0;
  }
}

void
ags_soundfont_iter_start(AgsPlayable *playable)
{
  AgsSoundfont *soundfont;

  soundfont = AGS_SOUNDFONT(playable);
}

gboolean
ags_soundfont_iter_next(AgsPlayable *playable)
{
  AgsSoundfont *soundfont;

  soundfont = AGS_SOUNDFONT(playable);
}

gchar**
ags_soundfont_get_banks(AgsSoundfont *soundfont)
{
  gchar **names;
  size_t size;

  size = 1;
  names = (gchar **) malloc(size * sizeof(gchar *));

  if(soundfont->bank != NULL){
    for(; soundfont->bank[size - 1] != NULL; size++){

      names = realloc(names, size * sizeof(gchar *));
      names[size - 1] = g_strdup(soundfont->bank[size - 1]->bank_name);
    }
  }

  names[size - 1] = NULL;

  return(names);
}

void
ags_soundfont_select_bank(AgsSoundfont *soundfont, gchar *bank)
{
  guint i;

  if(soundfont->bank != NULL){
    for(i = 0; soundfont->bank[i] != NULL; i++){
      if(g_strcmp0(soundfont->bank[i]->bank_name, bank) == 0){
	soundfont->current_bank = soundfont->bank[i];
	return;
      }
    }
  }

  soundfont->current_bank = NULL;
  soundfont->i_sample = 0;

  soundfont->current_sample = NULL;
}

gchar**
ags_soundfont_get_samples(AgsSoundfont *soundfont)
{
  gchar **names;
  size_t size;

  size = 1;
  names = (gchar **) malloc(size * sizeof(gchar *));

  if(soundfont->current_bank != NULL &&
     soundfont->current_bank->sample != NULL){
    for(; soundfont->current_bank->sample[size - 1] != NULL; size++){

      names = realloc(names, size * sizeof(gchar *));
      names[size - 1] = g_strdup(soundfont->current_bank->sample[size - 1]->sample_name);
    }
  }


  names[size - 1] = NULL;

  return(names);
}

void
ags_soundfont_select_sample(AgsSoundfont *soundfont, gchar *sample_name)
{
  guint i;

  if(soundfont->current_bank != NULL &&
     soundfont->current_bank->sample != NULL){
    for(i = 0; soundfont->current_bank->sample[i] != NULL; i++){
      if(g_strcmp0(soundfont->current_bank->sample[i]->sample_name, sample_name) == 0){
	soundfont->i_sample = i;

	soundfont->current_sample = soundfont->current_bank->sample[i];

	return;
      }
    }
  }

  soundfont->i_sample = 0;

  soundfont->current_sample = NULL;
}

void
ags_soundfont_start_sample(AgsPlayable *playable)
{
  AgsSoundfont *soundfont;

  soundfont = AGS_SOUNDFONT(playable);
  
  soundfont->current_sample = soundfont->current_bank->sample[soundfont->i_sample];
}

void
ags_soundfont_next_sample(AgsPlayable *playable)
{
  AgsSoundfont *soundfont;

  soundfont = AGS_SOUNDFONT(playable);
  
  soundfont->current_sample = soundfont->current_bank->sample[soundfont->current_sample - soundfont->current_bank->sample[0] + 1];
}

void
ags_soundfont_info(AgsPlayable *playable, guint *channels, guint *frames, guint *loop_start, guint *loop_end)
{
  AgsSoundfont *soundfont;

  soundfont = AGS_SOUNDFONT(playable);

  if((AGS_SOUNDFONT_SAMPLE_MONO & (soundfont->current_sample->flags)) == 0){
    //    if((AGS_SOUNDFONT_SAMPLE_LEFT & (soundfont->current_sample->flags)) != 0){
    //      if((AGS_SOUNDFONT_SAMPLE_RIGHT & (soundfont->current_sample->flags)) != 0){
	*channels = 2;
	//      }else{
	//	*channels = 1;
	//      }
	//    }
  }else{
    *channels = 1;
  }

  *frames = soundfont->current_sample->frames;
  *loop_start = soundfont->current_sample->loop_start;
  *loop_end = soundfont->current_sample->loop_end;
}

short*
ags_soundfont_read(AgsPlayable *playable, guint channel)
{
  short *buffer;
}

void
ags_soundfont_close(AgsPlayable *playable)
{
  AgsSoundfont *soundfont;
  guint i, j;

  soundfont = AGS_SOUNDFONT(playable);

  fclose(soundfont->file);
  soundfont->file = NULL;

  soundfont->selected_level = 0;

  if(soundfont->bank != NULL){
    for(i = 0; soundfont->bank[i] != NULL; i++){
      for(j = 0; soundfont->bank[i]->sample[j] != NULL; j++){
	free(soundfont->bank[i]->sample[j]);
      }

      free(soundfont->bank[i]);
    }
  }

  soundfont->current_bank = NULL;
  soundfont->current_sample = NULL;
}

AgsSoundfont*
ags_soundfont_new()
{
  AgsSoundfont *soundfont;

  soundfont = (AgsSoundfont *) g_object_new(AGS_TYPE_SOUNDFONT,
					    NULL);

  return(soundfont);
}
