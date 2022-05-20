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

#ifndef __AGS_AUDIO_UNIT_CLIENT_H__
#define __AGS_AUDIO_UNIT_CLIENT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/ags_api_config.h>

#ifdef AGS_WITH_AUDIO_UNIT
#include <AudioToolbox/AudioToolbox.h>
#include <AudioUnit/AudioUnit.h>
#include <AudioUnit/AUComponent.h>
#include <AudioUnit/AudioComponent.h>
#endif

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUDIO_UNIT_CLIENT                (ags_audio_unit_client_get_type())
#define AGS_AUDIO_UNIT_CLIENT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_UNIT_CLIENT, AgsAudioUnitClient))
#define AGS_AUDIO_UNIT_CLIENT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUDIO_UNIT_CLIENT, AgsAudioUnitClient))
#define AGS_IS_AUDIO_UNIT_CLIENT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUDIO_UNIT_CLIENT))
#define AGS_IS_AUDIO_UNIT_CLIENT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUDIO_UNIT_CLIENT))
#define AGS_AUDIO_UNIT_CLIENT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_AUDIO_UNIT_CLIENT, AgsAudioUnitClientClass))

#define AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(obj) (&(((AgsAudioUnitClient *) obj)->obj_mutex))

typedef struct _AgsAudioUnitClient AgsAudioUnitClient;
typedef struct _AgsAudioUnitClientClass AgsAudioUnitClientClass;

/**
 * AgsAudioUnitClientFlags:
 * @AGS_AUDIO_UNIT_CLIENT_ACTIVATED: the client was activated
 * @AGS_AUDIO_UNIT_CLIENT_READY: the client is ready
 * 
 * Enum values to control the behavior or indicate internal state of #AgsAudioUnitClient by
 * enable/disable as flags.
 */
typedef enum{
  AGS_AUDIO_UNIT_CLIENT_ACTIVATED          = 1,
  AGS_AUDIO_UNIT_CLIENT_READY              = 1 <<  1,
}AgsAudioUnitClientFlags;

struct _AgsAudioUnitClient
{
  GObject gobject;

  guint flags;
  guint connectable_flags;
  
  GRecMutex obj_mutex;

  GObject *audio_unit_server;
  
  AgsUUID *uuid;

  gchar *client_uuid;
  gchar *client_name;

  GList *device;
  GList *port;
};

struct _AgsAudioUnitClientClass
{
  GObjectClass gobject;
};

GType ags_audio_unit_client_get_type();
GType ags_audio_unit_client_flags_get_type();

gboolean ags_audio_unit_client_test_flags(AgsAudioUnitClient *audio_unit_client, guint flags);
void ags_audio_unit_client_set_flags(AgsAudioUnitClient *audio_unit_client, guint flags);
void ags_audio_unit_client_unset_flags(AgsAudioUnitClient *audio_unit_client, guint flags);

GList* ags_audio_unit_client_find_uuid(GList *audio_unit_client,
				       gchar *client_uuid);
GList* ags_audio_unit_client_find(GList *audio_unit_client,
				  gchar *client_name);

void ags_audio_unit_client_open(AgsAudioUnitClient *audio_unit_client,
				gchar *client_name);

void ags_audio_unit_client_add_device(AgsAudioUnitClient *audio_unit_client,
				      GObject *audio_unit_device);
void ags_audio_unit_client_remove_device(AgsAudioUnitClient *audio_unit_client,
					 GObject *audio_unit_device);

void ags_audio_unit_client_add_port(AgsAudioUnitClient *audio_unit_client,
				    GObject *audio_unit_port);
void ags_audio_unit_client_remove_port(AgsAudioUnitClient *audio_unit_client,
				       GObject *audio_unit_port);

void ags_audio_unit_client_activate(AgsAudioUnitClient *audio_unit_client);
void ags_audio_unit_client_deactivate(AgsAudioUnitClient *audio_unit_client);

AgsAudioUnitClient* ags_audio_unit_client_new(GObject *audio_unit_server);

G_END_DECLS

#endif /*__AGS_AUDIO_UNIT_CLIENT_H__*/
