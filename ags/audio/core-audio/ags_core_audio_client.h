/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AGS_CORE_AUDIO_CLIENT_H__
#define __AGS_CORE_AUDIO_CLIENT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/ags_api_config.h>

#ifdef AGS_WITH_CORE_AUDIO
#include <AudioToolbox/AudioToolbox.h>
#include <AudioUnit/AudioUnit.h>
#include <AudioUnit/AUComponent.h>
#include <AudioUnit/AudioComponent.h>
#endif

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_CORE_AUDIO_CLIENT                (ags_core_audio_client_get_type())
#define AGS_CORE_AUDIO_CLIENT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CORE_AUDIO_CLIENT, AgsCoreAudioClient))
#define AGS_CORE_AUDIO_CLIENT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_CORE_AUDIO_CLIENT, AgsCoreAudioClient))
#define AGS_IS_CORE_AUDIO_CLIENT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CORE_AUDIO_CLIENT))
#define AGS_IS_CORE_AUDIO_CLIENT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CORE_AUDIO_CLIENT))
#define AGS_CORE_AUDIO_CLIENT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_CORE_AUDIO_CLIENT, AgsCoreAudioClientClass))

#define AGS_CORE_AUDIO_CLIENT_GET_OBJ_MUTEX(obj) (&(((AgsCoreAudioClient *) obj)->obj_mutex))

typedef struct _AgsCoreAudioClient AgsCoreAudioClient;
typedef struct _AgsCoreAudioClientClass AgsCoreAudioClientClass;

/**
 * AgsCoreAudioClientFlags:
 * @AGS_CORE_AUDIO_CLIENT_ACTIVATED: the client was activated
 * @AGS_CORE_AUDIO_CLIENT_READY: the client is ready
 * 
 * Enum values to control the behavior or indicate internal state of #AgsCoreAudioClient by
 * enable/disable as flags.
 */
typedef enum{
  AGS_CORE_AUDIO_CLIENT_ACTIVATED          = 1,
  AGS_CORE_AUDIO_CLIENT_READY              = 1 <<  1,
}AgsCoreAudioClientFlags;

struct _AgsCoreAudioClient
{
  GObject gobject;

  guint flags;
  guint connectable_flags;
  
  GRecMutex obj_mutex;

  GObject *core_audio_server;
  
  AgsUUID *uuid;

  gchar *client_uuid;
  gchar *client_name;

#ifdef AGS_WITH_CORE_AUDIO
  AUGraph *graph;
#else
  gpointer graph;
#endif

  GList *device;
  GList *port;
};

struct _AgsCoreAudioClientClass
{
  GObjectClass gobject;
};

GType ags_core_audio_client_get_type();
GType ags_core_audio_client_flags_get_type();

gboolean ags_core_audio_client_test_flags(AgsCoreAudioClient *core_audio_client, guint flags);
void ags_core_audio_client_set_flags(AgsCoreAudioClient *core_audio_client, guint flags);
void ags_core_audio_client_unset_flags(AgsCoreAudioClient *core_audio_client, guint flags);

GList* ags_core_audio_client_find_uuid(GList *core_audio_client,
				       gchar *client_uuid);
GList* ags_core_audio_client_find(GList *core_audio_client,
				  gchar *client_name);

void ags_core_audio_client_open(AgsCoreAudioClient *core_audio_client,
				gchar *client_name);

void ags_core_audio_client_add_device(AgsCoreAudioClient *core_audio_client,
				      GObject *core_audio_device);
void ags_core_audio_client_remove_device(AgsCoreAudioClient *core_audio_client,
					 GObject *core_audio_device);

void ags_core_audio_client_add_port(AgsCoreAudioClient *core_audio_client,
				    GObject *core_audio_port);
void ags_core_audio_client_remove_port(AgsCoreAudioClient *core_audio_client,
				       GObject *core_audio_port);

void ags_core_audio_client_activate(AgsCoreAudioClient *core_audio_client);
void ags_core_audio_client_deactivate(AgsCoreAudioClient *core_audio_client);

AgsCoreAudioClient* ags_core_audio_client_new(GObject *core_audio_server);

G_END_DECLS

#endif /*__AGS_CORE_AUDIO_CLIENT_H__*/
