/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_CORE_AUDIO_SERVER_H__
#define __AGS_CORE_AUDIO_SERVER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/config.h>

#ifdef AGS_WITH_CORE_AUDIO
#include <AudioToolbox/AudioToolbox.h>
#include <AudioUnit/AudioUnit.h>
#include <AudioUnit/AUComponent.h>
#include <AudioUnit/AudioComponent.h>
#endif

#include <pthread.h>

#define AGS_TYPE_CORE_AUDIO_SERVER                (ags_core_audio_server_get_type())
#define AGS_CORE_AUDIO_SERVER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CORE_AUDIO_SERVER, AgsCoreAudioServer))
#define AGS_CORE_AUDIO_SERVER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_CORE_AUDIO_SERVER, AgsCoreAudioServer))
#define AGS_IS_CORE_AUDIO_SERVER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CORE_AUDIO_SERVER))
#define AGS_IS_CORE_AUDIO_SERVER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CORE_AUDIO_SERVER))
#define AGS_CORE_AUDIO_SERVER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_CORE_AUDIO_SERVER, AgsCoreAudioServerClass))

typedef struct _AgsCoreAudioServer AgsCoreAudioServer;
typedef struct _AgsCoreAudioServerClass AgsCoreAudioServerClass;

typedef enum{
  AGS_CORE_AUDIO_SERVER_CONNECTED       = 1,
}AgsCoreAudioServerFlags;

struct _AgsCoreAudioServer
{
  GObject object;

  guint flags;

  pthread_mutex_t *mutex;
  pthread_mutexattr_t *mutexattr;

  volatile gboolean running;
  pthread_t *thread;
  
  GObject *application_context;
  
  gchar *url;

  guint *port;
  guint port_count;

  guint n_soundcards;
  guint n_sequencers;
  
  GObject *default_soundcard;  

  GObject *default_client;
  GList *client;
};

struct _AgsCoreAudioServerClass
{
  GObjectClass object;
};

GType ags_core_audio_server_get_type();

GList* ags_core_audio_server_find_url(GList *core_audio_server,
				      gchar *url);

GObject* ags_core_audio_server_find_client(AgsCoreAudioServer *core_audio_server,
					   gchar *client_uuid);

GObject* ags_core_audio_server_find_port(AgsCoreAudioServer *core_audio_server,
					 gchar *port_uuid);

void ags_core_audio_server_add_client(AgsCoreAudioServer *core_audio_server,
				      GObject *core_audio_client);
void ags_core_audio_server_remove_client(AgsCoreAudioServer *core_audio_server,
					 GObject *core_audio_client);

void ags_core_audio_server_connect_client(AgsCoreAudioServer *core_audio_server);

void ags_core_audio_server_start_poll(AgsCoreAudioServer *core_audio_server);

AgsCoreAudioServer* ags_core_audio_server_new(GObject *application_context,
					      gchar *url);

#endif /*__AGS_CORE_AUDIO_SERVER_H__*/
