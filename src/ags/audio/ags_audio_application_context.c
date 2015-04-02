/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_audio_loop.h>
#include <ags/audio/ags_gui_audio.h>
#include <ags/audio/ags_autosave_audio.h>
#include <ags/audio/ags_single_audio.h>

void ags_audio_application_context_class_init(AgsAudioApplicationContextClass *audio_application_context);
void ags_audio_application_context_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_application_context_init(AgsAudioApplicationContext *audio_application_context);
void ags_audio_application_context_set_property(GObject *gobject,
						guint prop_id,
						const GValue *value,
						GParamSpec *param_spec);
void ags_audio_application_context_get_property(GObject *gobject,
						guint prop_id,
						GValue *value,
						GParamSpec *param_spec);
void ags_audio_application_context_connect(AgsConnectable *connectable);
void ags_audio_application_context_disconnect(AgsConnectable *connectable);
void ags_audio_application_context_finalize(GObject *gobject);

static AgsConnectableInterface* ags_audio_application_context_parent_connectable_interface;

GType
ags_audio_application_context_get_type()
{
  static GType ags_type_audio_application_context = 0;

  if(!ags_type_audio_application_context){
    static const GTypeInfo ags_audio_application_context_info = {
      sizeof (AgsAudioApplicationContextClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_application_context_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioApplicationContext),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_application_context_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_application_context_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_application_context = g_type_register_static(AGS_TYPE_APPLICATION_CONTEXT,
								"AgsAudioApplicationContext\0",
								&ags_audio_application_context_info,
								0);

    g_type_add_interface_static(ags_type_audio_application_context,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_audio_application_context);
}

void
ags_audio_application_context_class_init(AgsAudioApplicationContextClass *audio_application_context)
{
  GObjectClass *gobject;
  AgsApplicationContextClass *application_context;

  ags_audio_application_context_parent_class = g_type_class_peek_parent(ags_audio_application_context);

  /* GObjectClass */
  gobject = (GObjectClass *) ags_audio_application_context;

  gobject->set_property = ags_audio_application_context_set_property;
  gobject->get_property = ags_audio_application_context_get_property;

  gobject->finalize = ags_audio_application_context_finalize;

  /* AgsAudioApplicationContextClass */
  application_context = (AgsApplicationContextClass *) audio_application_context_class;
  
  application_context->load_config = ags_audio_application_context_load_config;
  application_context->register_types = ags_audio_application_context_register_types;
}

void
ags_audio_application_context_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_audio_application_context_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_audio_application_context_connect;
  connectable->disconnect = ags_audio_application_context_disconnect;
}

void
ags_audio_application_context_init(AgsAudioApplicationContext *audio_application_context)
{
  audio_application_context->flags = 0;

  application_context->soundcard = NULL;
}

void
ags_audio_application_context_set_property(GObject *gobject,
					   guint prop_id,
					   const GValue *value,
					   GParamSpec *param_spec)
{
  AgsAudioApplicationContext *audio_application_context;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_application_context_get_property(GObject *gobject,
					   guint prop_id,
					   GValue *value,
					   GParamSpec *param_spec)
{
  AgsAudioApplicationContext *audio_application_context;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_application_context_connect(AgsConnectable *connectable)
{
  AgsAudioApplicationContext *audio_application_context;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(connectable);

  if((AGS_AUDIO_APPLICATION_CONTEXT_CONNECTED & (audio_application_context->flags)) != 0){
    return;
  }

  ags_audio_application_context_parent_connectable_interface->connect(connectable);
}

void
ags_audio_application_context_disconnect(AgsConnectable *connectable)
{
  AgsAudioApplicationContext *audio_application_context;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(connectable);

  if((AGS_AUDIO_APPLICATION_CONTEXT_CONNECTED & (audio_application_context->flags)) == 0){
    return;
  }

  ags_audio_application_context_parent_connectable_interface->disconnect(connectable);
}

void
ags_audio_application_context_finalize(GObject *gobject)
{
  AgsAudioApplicationContext *audio_application_context;

  G_OBJECT_CLASS(ags_audio_application_context_parent_class)->finalize(gobject);

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(gobject);
}

AgsAudioApplicationContext*
ags_audio_application_context_new(AgsMainLoop *main_loop,
				  AgsConfig *config)
{
  AgsAudioApplicationContext *audio_application_context;

  audio_application_context = (AgsAudioApplicationContext *) g_object_new(AGS_TYPE_AUDIO_APPLICATION_CONTEXT,
									  "main-loop\0", main_loop,
									  "config\0", config,
									  NULL);

  return(audio_application_context);
}


