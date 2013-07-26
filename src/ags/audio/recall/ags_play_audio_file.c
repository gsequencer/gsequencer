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

#include <ags/audio/recall/ags_play_audio_file.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_recall_id.h>

void ags_play_audio_file_class_init(AgsPlayAudioFileClass *play_audio_file);
void ags_play_audio_file_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_audio_file_init(AgsPlayAudioFile *play_audio_file);
void ags_play_audio_file_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_play_audio_file_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_play_audio_file_connect(AgsConnectable *connectable);
void ags_play_audio_file_disconnect(AgsConnectable *connectable);
void ags_play_audio_file_finalize(GObject *gobject);

void ags_play_audio_file_run_inter(AgsRecall *recall);
void ags_play_audio_file_remove(AgsRecall *recall);
void ags_play_audio_file_cancel(AgsRecall *recall);

enum{
  PROP_0,
  PROP_DEVOUT,
  PROP_AUDIO_FILE,
  PROP_CURRENT_FRAME,
};

static gpointer ags_play_audio_file_parent_class = NULL;
static AgsConnectableInterface *ags_play_audio_file_parent_connectable_interface;

GType
ags_play_audio_file_get_type()
{
  static GType ags_type_play_audio_file = 0;

  if(!ags_type_play_audio_file){
    static const GTypeInfo ags_play_audio_file_info = {
      sizeof (AgsPlayAudioFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_audio_file_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayAudioFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_audio_file_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_audio_file_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_audio_file = g_type_register_static(AGS_TYPE_RECALL,
						      "AgsPlayAudioFile\0",
						      &ags_play_audio_file_info,
						      0);

    g_type_add_interface_static(ags_type_play_audio_file,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_play_audio_file);
}

void
ags_play_audio_file_class_init(AgsPlayAudioFileClass *play_audio_file)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_play_audio_file_parent_class = g_type_class_peek_parent(play_audio_file);

  /* GObjectClass */
  gobject = (GObjectClass *) play_audio_file;

  gobject->set_property = ags_play_audio_file_set_property;
  gobject->get_property = ags_play_audio_file_get_property;

  gobject->finalize = ags_play_audio_file_finalize;

  /* properties */
  param_spec = g_param_spec_gtype("devout\0",
				  "assigned devout\0",
				  "The devout this recall is assigned to\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVOUT,
				  param_spec);

  param_spec = g_param_spec_gtype("audio_file\0",
				  "assigned audio file\0",
				  "The audio file this recall is assigned to\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_FILE,
				  param_spec);

  param_spec = g_param_spec_gtype("current\0",
				  "current frame\0",
				  "The current frame this recall is playing\0",
				   G_TYPE_UINT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CURRENT_FRAME,
				  param_spec);

  /* AgsRecallClass */
  recall->run_inter = ags_play_audio_file_run_inter;
  recall->remove = ags_play_audio_file_remove;
  recall->cancel = ags_play_audio_file_cancel;
}

void
ags_play_audio_file_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_audio_file_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_audio_file_connect;
  connectable->disconnect = ags_play_audio_file_disconnect;
}

void
ags_play_audio_file_init(AgsPlayAudioFile *play_audio_file)
{
  play_audio_file->audio_file = NULL;
  play_audio_file->current_frame = 0;

  play_audio_file->devout = NULL;
}

void
ags_play_audio_file_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsPlayAudioFile *play_audio_file;

  play_audio_file = AGS_PLAY_AUDIO_FILE(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    {
      AgsDevout *devout;

      devout = (AgsDevout *) g_value_get_object(value);

      if(play_audio_file->devout == devout)
	return;

      if(play_audio_file->devout != NULL)
	g_object_unref(play_audio_file->devout);

      if(devout != NULL)
	g_object_ref(devout);

      play_audio_file->devout = devout;
    }
    break;
  case PROP_AUDIO_FILE:
    {
      AgsAudioFile *audio_file;

      audio_file = (AgsAudioFile *) g_value_get_object(value);

      if(play_audio_file->audio_file == audio_file)
	return;

      if(play_audio_file->audio_file != NULL)
	g_object_unref(play_audio_file->audio_file);

      if(play_audio_file != NULL)
	g_object_ref(play_audio_file);

      play_audio_file->audio_file = audio_file;
    }
    break;
  case PROP_CURRENT_FRAME:
    {
      play_audio_file->current_frame = (guint) g_value_get_uint(value);

      /*
       * TODO:JK: implement seeking over the buffer
       */
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_audio_file_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsPlayAudioFile *play_audio_file;

  play_audio_file = AGS_PLAY_AUDIO_FILE(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    {
      g_value_set_object(value, play_audio_file->devout);
    }
    break;
  case PROP_AUDIO_FILE:
    {
      g_value_set_object(value, play_audio_file->audio_file);
    }
    break;
  case PROP_CURRENT_FRAME:
    {
      g_value_set_uint(value, play_audio_file->current_frame);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_audio_file_connect(AgsConnectable *connectable)
{
  ags_play_audio_file_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_play_audio_file_disconnect(AgsConnectable *connectable)
{
  ags_play_audio_file_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_play_audio_file_finalize(GObject *gobject)
{
  AgsPlayAudioFile *play_audio_file;

  play_audio_file = AGS_PLAY_AUDIO_FILE(gobject);

  g_object_unref(G_OBJECT(play_audio_file->audio_file));

  g_object_unref(G_OBJECT(play_audio_file->devout));

  G_OBJECT_CLASS(ags_play_audio_file_parent_class)->finalize(gobject);
}

void
ags_play_audio_file_run_inter(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_play_audio_file_parent_class)->run_inter(recall);

  /*
  AgsPlayAudioFile *play_audio_file;
  signed short *buffer;
  guint i0, i1, j, stop;
  gboolean play_done;

  play_audio_file = (AgsPlayAudioFile *) recall;

  if((AGS_DEVOUT_BUFFER0 & play_audio_file->devout->flags) != 0){
    buffer = play_audio_file->devout->buffer[1];
  }else if((AGS_DEVOUT_BUFFER1 & play_audio_file->devout->flags) != 0){
    buffer = play_audio_file->devout->buffer[2];
  }else if((AGS_DEVOUT_BUFFER2 & play_audio_file->devout->flags) != 0){
    buffer = play_audio_file->devout->buffer[3];
  }else if((AGS_DEVOUT_BUFFER3 & play_audio_file->devout->flags) != 0){
    buffer = play_audio_file->devout->buffer[0];
  }

  i0 = play_audio_file->current;
  stop = i0 + play_audio_file->devout->buffer_size;

  if(stop < play_audio_file->audio_file->frames)
    play_done = FALSE;
  else{
    stop = play_audio_file->audio_file->frames;
    play_done = TRUE;
  }

  for(i1 = 0; i0 < stop; i0++, i1++){
    for(j = 0; j < play_audio_file->audio_file->channels || j < play_audio_file->devout->dsp_channels; j++)
      buffer[i1 * play_audio_file->devout->dsp_channels + j] = ((buffer[i1 * play_audio_file->devout->dsp_channels + j]) / 2) + ((play_audio_file->audio_file->buffer[i0 * play_audio_file->audio_file->channels + j]) / 2);
  }

  play_audio_file->current = i0;

  if(play_done)
    g_signal_emit_by_name((GObject *) recall, "done\0", recall_id);
  */
}

void
ags_play_audio_file_remove(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_play_audio_file_parent_class)->remove(recall);
}

void
ags_play_audio_file_cancel(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_play_audio_file_parent_class)->cancel(recall);
}

AgsPlayAudioFile*
ags_play_audio_file_new(AgsAudioFile *audio_file,
			AgsDevout *devout)
{
  AgsPlayAudioFile *play_audio_file;

  play_audio_file = (AgsPlayAudioFile *) g_object_new(AGS_TYPE_PLAY_AUDIO_FILE,
						      "audio_file\0", audio_file,
						      "devout\0", devout,
						      NULL);

  return(play_audio_file);
}

