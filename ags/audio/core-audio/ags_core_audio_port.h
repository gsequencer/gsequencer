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

#ifndef __AGS_CORE_AUDIO_PORT_H__
#define __AGS_CORE_AUDIO_PORT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/config.h>

#ifdef AGS_WITH_CORE_AUDIO
#include <AudioToolbox/AudioToolbox.h>

#include <AudioUnit/AudioUnit.h>
#include <AudioUnit/AUComponent.h>
#include <AudioUnit/AudioComponent.h>

#include <CoreMIDI/CoreMIDI.h>
#endif

#define AGS_TYPE_CORE_AUDIO_PORT                (ags_core_audio_port_get_type())
#define AGS_CORE_AUDIO_PORT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CORE_AUDIO_PORT, AgsCoreAudioPort))
#define AGS_CORE_AUDIO_PORT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_CORE_AUDIO_PORT, AgsCoreAudioPort))
#define AGS_IS_CORE_AUDIO_PORT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CORE_AUDIO_PORT))
#define AGS_IS_CORE_AUDIO_PORT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CORE_AUDIO_PORT))
#define AGS_CORE_AUDIO_PORT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_CORE_AUDIO_PORT, AgsCoreAudioPortClass))

typedef struct _AgsCoreAudioPort AgsCoreAudioPort;
typedef struct _AgsCoreAudioPortClass AgsCoreAudioPortClass;

typedef enum{
  AGS_CORE_AUDIO_PORT_CONNECTED       = 1,
  AGS_CORE_AUDIO_PORT_REGISTERED      = 1 <<  1,
  AGS_CORE_AUDIO_PORT_IS_AUDIO        = 1 <<  2,
  AGS_CORE_AUDIO_PORT_IS_MIDI         = 1 <<  3,
  AGS_CORE_AUDIO_PORT_IS_OUTPUT       = 1 <<  4,
}AgsCoreAudioPortFlags;

struct _AgsCoreAudioPort
{
  GObject object;

  guint flags;

  pthread_mutex_t *mutex;
  pthread_mutexattr_t *mutexattr;

  GObject *core_audio_client;
  GObject *core_audio_device;
  
  gchar *uuid;
  gchar *name;

  guint format;
  guint buffer_size;
  guint pcm_channels;
  
#ifdef AGS_WITH_CORE_AUDIO
  AudioComponent *comp;
  AudioComponentDescription *desc;
  AudioComponentInstance *au_hal;

  AUNode *device_node;

  MIDIClientRef *client;
  MIDIPortRef *midi_port;
#else
  gpointer comp;
  gpointer desc;
  gpointer au_hal;

  gpointer device_node;
#endif
  
  void *empty_buffer;

  volatile gboolean is_empty;
  volatile gint underflow;
  volatile gboolean restart;
  
  guint nth_empty_buffer;
  
  volatile guint queued;
};

struct _AgsCoreAudioPortClass
{
  GObjectClass object;
};

GType ags_core_audio_port_get_type();

GList* ags_core_audio_port_find(GList *core_audio_port,
				gchar *port_name);

void ags_core_audio_port_register(AgsCoreAudioPort *core_audio_port,
				  gchar *port_name,
				  gboolean is_audio, gboolean is_midi,
				  gboolean is_output);
void ags_core_audio_port_unregister(AgsCoreAudioPort *core_audio_port);

AgsCoreAudioPort* ags_core_audio_port_new(GObject *core_audio_client);

#endif /*__AGS_CORE_AUDIO_PORT_H__*/
