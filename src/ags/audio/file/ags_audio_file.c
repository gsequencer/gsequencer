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

#include <ags/audio/file/ags_audio_file.h>

#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_playable.h>

#include <ags/audio/ags_audio_signal.h>

#include <ags/audio/file/ags_sndfile.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sndfile.h>
#include <string.h>

void ags_audio_file_class_init(AgsAudioFileClass *audio_file);
void ags_audio_file_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_file_init(AgsAudioFile *audio_file);
void ags_audio_file_connect(AgsConnectable *connectable);
void ags_audio_file_disconnect(AgsConnectable *connectable);
void ags_audio_file_finalize(GObject *object);

enum{
  READ_BUFFER,
  LAST_SIGNAL,
};

static gpointer ags_audio_file_parent_class = NULL;
static AgsConnectableInterface *ags_audio_file_parent_connectable_interface;
static guint signals[LAST_SIGNAL];

GType
ags_audio_file_get_type()
{
  static GType ags_type_audio_file = 0;

  if(!ags_type_audio_file){
    static const GTypeInfo ags_audio_file_info = {
      sizeof (AgsAudioFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_file_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_file_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_file_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_file = g_type_register_static(G_TYPE_OBJECT,
						 "AgsAudioFile\0",
						 &ags_audio_file_info,
						 0);

    g_type_add_interface_static(ags_type_audio_file,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_audio_file);
}

void
ags_audio_file_class_init(AgsAudioFileClass *audio_file)
{
  GObjectClass *gobject;

  ags_audio_file_parent_class = g_type_class_peek_parent(audio_file);

  gobject = (GObjectClass *) audio_file;
  gobject->finalize = ags_audio_file_finalize;
}

void
ags_audio_file_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_audio_file_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_audio_file_connect;
  connectable->disconnect = ags_audio_file_disconnect;
}

void
ags_audio_file_init(AgsAudioFile *audio_file)
{
  audio_file->devout = NULL;

  audio_file->name = NULL;
  audio_file->frames = 0;
  audio_file->channels = 0;

  audio_file->start_channel = 0;
  audio_file->audio_channels = 0;

  audio_file->audio_signal = NULL;

  audio_file->file = NULL;
}

void
ags_audio_file_finalize(GObject *gobject)
{
  AgsAudioFile *audio_file;
  GList *list, *list_next;

  audio_file = AGS_AUDIO_FILE(gobject);

  /* AgsAudioSignal */
  list = audio_file->audio_signal;

  while(list != NULL){
    list_next = list->next;

    g_object_unref(G_OBJECT(list->data));
    g_list_free1(list);
    
    list = list_next;
  }

  /* file */
  g_object_unref(audio_file->file);

  G_OBJECT_CLASS(ags_audio_file_parent_class)->finalize(gobject);
}

void
ags_audio_file_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_audio_file_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

gboolean
ags_audio_file_open(AgsAudioFile *audio_file)
{
#ifdef AGS_DEBUG
  g_message("ags_audio_file_open: %s\0", audio_file->name);
#endif

  if(g_file_test(audio_file->name, G_FILE_TEST_EXISTS)){
    if(g_str_has_suffix(audio_file->name, ".wav\0") ||
       g_str_has_suffix(audio_file->name, ".ogg\0") ||
       g_str_has_suffix(audio_file->name, ".flac\0")){
      GError *error;
      guint loop_start, loop_end;

      audio_file->file = (GObject *) ags_sndfile_new();

      if(ags_playable_open(AGS_PLAYABLE(audio_file->file),
			   audio_file->name)){
	//FIXME:JK: this call should occure just before reading frames because of the new iterate functions of an AgsPlayable

	error = NULL;

	ags_playable_info(AGS_PLAYABLE(audio_file->file),
			  &(audio_file->channels), &(audio_file->frames),
			  &loop_start, &loop_end,
			  &error);

	if(error != NULL){
	  g_error("%s\0", error->message);
	}

	return(TRUE);
      }else{
	return(FALSE);
      }
    }else{
      g_message("ags_audio_file_open: unknown file type\n\0");
      return(FALSE);
    }
  }
}

gboolean
ags_audio_file_open_from_data(AgsAudioFile *audio_file, gchar *data)
{
#ifdef AGS_DEBUG
  g_message("ags_audio_file_open_from_data:\0");
#endif

  if(data != NULL){
    if(g_str_has_suffix(audio_file->name, ".wav\0") ||
       g_str_has_suffix(audio_file->name, ".ogg\0") ||
       g_str_has_suffix(audio_file->name, ".flac\0")){
      GError *error;
      guint loop_start, loop_end;

      audio_file->file = (GObject *) ags_sndfile_new();
      AGS_SNDFILE(audio_file->file)->flags = AGS_SNDFILE_VIRTUAL;

      if(ags_playable_open(AGS_PLAYABLE(audio_file->file),
			   audio_file->name)){
	AGS_SNDFILE(audio_file->file)->pointer = g_base64_decode(data,
								 &(AGS_SNDFILE(audio_file->file)->length));
	AGS_SNDFILE(audio_file->file)->current = AGS_SNDFILE(audio_file->file)->pointer;

	error = NULL;

	ags_playable_info(AGS_PLAYABLE(audio_file->file),
			  &(audio_file->channels), &(audio_file->frames),
			  &loop_start, &loop_end,
			  &error);

	if(error != NULL){
	  g_error("%s\0", error->message);
	}

	return(TRUE);
      }else{
	return(FALSE);
      }
    }else{
      g_message("ags_audio_file_open: unknown file type\n\0");
      return(FALSE);
    }
  }
}

void
ags_audio_file_close(AgsAudioFile *audio_file)
{
  ags_playable_close(AGS_PLAYABLE(audio_file->file));
}


void
ags_audio_file_read_audio_signal(AgsAudioFile *audio_file)
{
  GList *list;

  list = ags_playable_read_audio_signal(AGS_PLAYABLE(audio_file->file),
					audio_file->devout,
					audio_file->start_channel, audio_file->audio_channels);

  audio_file->audio_signal = list;
}

void
ags_audio_file_seek(AgsAudioFile *audio_file, guint frames, gint whence)
{
  ags_playable_seek(AGS_PLAYABLE(audio_file->file),
		    frames, whence);
}

void
ags_audio_file_write(AgsAudioFile *audio_file,
		     signed short *buffer, guint buffer_size)
{
  ags_playable_write(AGS_PLAYABLE(audio_file->file),
		     buffer, buffer_size);
}

void
ags_audio_file_flush(AgsAudioFile *audio_file)
{
  ags_playable_flush(AGS_PLAYABLE(audio_file->file));
}

AgsAudioFile*
ags_audio_file_new(gchar *name,
		   AgsDevout *devout,
		   guint start_channel, guint audio_channels)
{
  AgsAudioFile *audio_file;

  audio_file = (AgsAudioFile *) g_object_new(AGS_TYPE_AUDIO_FILE, NULL);

  audio_file->name = g_strdup(name);
  audio_file->devout = devout;
  audio_file->start_channel = start_channel;
  audio_file->audio_channels = audio_channels;

  return(audio_file);
}
