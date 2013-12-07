/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#include <ags/audio/recall/ags_mute_audio.h>

void ags_mute_audio_class_init(AgsMuteAudioClass *mute_audio);
void ags_mute_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_mute_audio_mutable_interface_init(AgsMutableInterface *mutable);
void ags_mute_audio_init(AgsMuteAudio *mute_audio);
void ags_mute_audio_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_mute_audio_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_mute_audio_finalize(GObject *gobject);

void ags_mute_audio_set_muted(AgsMutable *mutable, gboolean muted);

enum{
  PROP_0,
  PROP_MUTED,
};

static gpointer ags_mute_audio_parent_class = NULL;
static AgsConnectableInterface *ags_mute_audio_parent_connectable_interface;
static AgsMutableInterface *ags_mute_audio_parent_mutable_interface;

GType
ags_mute_audio_get_type()
{
  static GType ags_type_mute_audio = 0;

  if(!ags_type_mute_audio){
    static const GTypeInfo ags_mute_audio_info = {
      sizeof (AgsMuteAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_mute_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsMuteAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_mute_audio_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_mute_audio_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_mutable_interface_info = {
      (GInterfaceInitFunc) ags_mute_audio_mutable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_mute_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						 "AgsMuteAudio\0",
						 &ags_mute_audio_info,
						 0);

    g_type_add_interface_static(ags_type_mute_audio,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_mute_audio,
				AGS_TYPE_MUTABLE,
				&ags_mutable_interface_info);
  }

  return(ags_type_mute_audio);
}

void
ags_mute_audio_class_init(AgsMuteAudioClass *mute_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_mute_audio_parent_class = g_type_class_peek_parent(mute_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) mute_audio;

  gobject->set_property = ags_mute_audio_set_property;
  gobject->get_property = ags_mute_audio_get_property;

  gobject->finalize = ags_mute_audio_finalize;

  /* properties */
  param_spec = g_param_spec_object("muted\0",
				   "mute audio\0",
				   "Mute the audio\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MUTED,
				  param_spec);
}

void
ags_mute_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_mute_audio_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_mute_audio_connect;
  connectable->disconnect = ags_mute_audio_disconnect;
}

void
ags_mute_audio_mutable_interface_init(AgsMutableInterface *mutable)
{
  ags_mute_audio_parent_mutable_interface = g_type_interface_peek_parent(mutable);

  mutable->set_muted = ags_mute_audio_set_muted;
}

void
ags_mute_audio_init(AgsMuteAudio *mute_audio)
{
  mute_audio->muted = g_object_new(AGS_TYPE_PORT,
				   "plugin-name\0", g_strdup("ags-mute\0"),
				   "specifier\0", "./muted[0]\0",
				   "control-port\0", "1/1\0",
				   "port-value-is-pointer\0", FALSE,
				   "port-value-type\0", G_TYPE_BOOLEAN,
				   "port-value-size\0", sizeof(gboolean),
				   "port-value-length\0", 1,
				   NULL);
  mute_audio->muted->port_value.ags_port_boolean = FALSE;
}


void
ags_mute_audio_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsMuteAudio *mute_audio;

  mute_audio = AGS_MUTE_AUDIO(gobject);

  switch(prop_id){
  case PROP_MUTED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == mute_audio->muted){
	return;
      }

      if(mute_audio->muted != NULL){
	g_object_unref(G_OBJECT(mute_audio->muted));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      mute_audio->muted = port;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_mute_audio_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsMuteAudio *mute_audio;

  mute_audio = AGS_MUTE_AUDIO(gobject);

  switch(prop_id){
  case PROP_MUTED:
    {
      g_value_set_object(value, mute_audio->muted);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_mute_audio_finalize(GObject *gobject)
{
  AgsMuteAudio *mute_audio;

  mute_audio = AGS_MUTE_AUDIO(gobject);

  if(mute_audio->muted != NULL){
    g_object_unref(G_OBJECT(mute_audio->muted));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_mute_audio_parent_class)->finalize(gobject);
}

void
ags_mute_audio_connect(AgsConnectable *connectable)
{
  ags_mute_audio_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_mute_audio_disconnect(AgsConnectable *connectable)
{
  ags_mute_audio_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_mute_audio_set_muted(AgsMutable *mutable, gboolean muted)
{
  GValue value = {0,};

  g_value_init(&value, G_TYPE_BOOLEAN);
  g_value_set_boolean(&value, muted);

  ags_port_safe_write(AGS_MUTE_AUDIO(mutable)->muted, &value);
}

AgsMuteAudio*
ags_mute_audio_new()
{
  AgsMuteAudio *mute_audio;

  mute_audio = (AgsMuteAudio *) g_object_new(AGS_TYPE_MUTE_AUDIO,
					     NULL);

  return(mute_audio);
}
