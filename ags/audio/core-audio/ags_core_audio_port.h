/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/ags_api_config.h>

#ifdef AGS_WITH_CORE_AUDIO
#include <AudioToolbox/AudioToolbox.h>

#include <AudioUnit/AudioUnit.h>
#include <AudioUnit/AUComponent.h>
#include <AudioUnit/AudioComponent.h>

#include <CoreMIDI/CoreMIDI.h>
#endif

#include <ags/libags.h>

#include <ags/audio/ags_audio_buffer_util.h>

G_BEGIN_DECLS

#define AGS_TYPE_CORE_AUDIO_PORT                (ags_core_audio_port_get_type())
#define AGS_CORE_AUDIO_PORT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CORE_AUDIO_PORT, AgsCoreAudioPort))
#define AGS_CORE_AUDIO_PORT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_CORE_AUDIO_PORT, AgsCoreAudioPort))
#define AGS_IS_CORE_AUDIO_PORT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CORE_AUDIO_PORT))
#define AGS_IS_CORE_AUDIO_PORT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CORE_AUDIO_PORT))
#define AGS_CORE_AUDIO_PORT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_CORE_AUDIO_PORT, AgsCoreAudioPortClass))

#define AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(obj) (&(((AgsCoreAudioPort *) obj)->obj_mutex))

#define AGS_CORE_AUDIO_PORT_DEFAULT_CACHE_BUFFER_SIZE (4096)

typedef struct _AgsCoreAudioPort AgsCoreAudioPort;
typedef struct _AgsCoreAudioPortClass AgsCoreAudioPortClass;

/**
 * AgsCoreAudioPortFlags:
 * @AGS_CORE_AUDIO_PORT_REGISTERED: the port was registered
 * @AGS_CORE_AUDIO_PORT_IS_AUDIO: the port provides audio data
 * @AGS_CORE_AUDIO_PORT_IS_MIDI: the port provides midi data
 * @AGS_CORE_AUDIO_PORT_IS_OUTPUT: the port does output
 * @AGS_CORE_AUDIO_PORT_IS_INPUT: the port does input
 * 
 * Enum values to control the behavior or indicate internal state of #AgsCoreAudioPort by
 * enable/disable as flags.
 */
typedef enum{
  AGS_CORE_AUDIO_PORT_REGISTERED         = 1,
  AGS_CORE_AUDIO_PORT_IS_AUDIO           = 1 <<  1,
  AGS_CORE_AUDIO_PORT_IS_MIDI            = 1 <<  2,
  AGS_CORE_AUDIO_PORT_IS_OUTPUT          = 1 <<  3,
  AGS_CORE_AUDIO_PORT_IS_INPUT           = 1 <<  4,
}AgsCoreAudioPortFlags;

struct _AgsCoreAudioPort
{
  GObject gobject;

  AgsCoreAudioPortFlags flags;
  AgsConnectableFlags connectable_flags;
  
  GRecMutex obj_mutex;

  GObject *core_audio_client;

  AgsUUID *uuid;

  GObject *core_audio_device;
  
  gchar *port_uuid;
  gchar *port_name;

  guint pcm_channels;

  guint samplerate;
  guint buffer_size;
  AgsSoundcardFormat format;

  gboolean use_cache;
  guint cache_buffer_size;

  guint current_cache;
  guint completed_cache;
  guint cache_offset;
  void **cache;
  
#ifdef AGS_WITH_CORE_AUDIO
  AudioQueueRef aq_ref;
  AudioStreamBasicDescription data_format;

  AudioQueueRef record_aq_ref;
  AudioStreamBasicDescription record_format;

  AudioQueueBufferRef buf_ref[16];
  AudioQueueBufferRef record_buf_ref[16];
  
  MIDIClientRef *midi_client;
  MIDIPortRef *midi_port;
#else
  gpointer aq_ref;
  gpointer data_format;

  gpointer buffer;
  
  gpointer midi_client;
  gpointer midi_port;
#endif

  unsigned int midi_port_number;

  volatile gboolean output_running;
  volatile gboolean input_running;
  volatile gboolean is_empty;
  volatile guint queued;

#ifdef AGS_WITH_CORE_AUDIO
  AudioObjectPropertyAddress *output_samplerate_property_address;
  AudioObjectPropertyAddress *output_buffer_size_property_address;
  AudioObjectPropertyAddress *output_stream_format_property_address;

  AudioStreamBasicDescription output_format;
  
  AudioObjectPropertyAddress *output_property_address;
  AudioObjectID output_device;
  AudioDeviceIOProcID output_proc_id;
  
  AudioObjectPropertyAddress *input_samplerate_property_address;
  AudioObjectPropertyAddress *input_buffer_size_property_address;
  AudioObjectPropertyAddress *input_stream_format_property_address;

  AudioStreamBasicDescription input_format;
  
  AudioObjectPropertyAddress *input_property_address;
  AudioObjectID input_device; 
  AudioDeviceIOProcID input_proc_id;
#else
  gpointer output_samplerate_property_address;
  gpointer output_buffer_size_property_address;
  gpointer output_stream_format_property_address;
  
  gpointer output_property_address;
  gint64 output_device;
  gint64 output_proc_id;

  gpointer input_samplerate_property_address;
  gpointer input_buffer_size_property_address;
  gpointer input_stream_format_property_address;
  
  gpointer input_property_address;
  gint64 input_device;
  gint64 input_proc_id;
#endif

  AgsAudioBufferUtil *audio_buffer_util;
};

struct _AgsCoreAudioPortClass
{
  GObjectClass gobject;
};

GType ags_core_audio_port_get_type();
GType ags_core_audio_port_flags_get_type();

gboolean ags_core_audio_port_test_flags(AgsCoreAudioPort *core_audio_port, AgsCoreAudioPortFlags flags);
void ags_core_audio_port_set_flags(AgsCoreAudioPort *core_audio_port, AgsCoreAudioPortFlags flags);
void ags_core_audio_port_unset_flags(AgsCoreAudioPort *core_audio_port, AgsCoreAudioPortFlags flags);

/* presets */
void ags_core_audio_port_set_pcm_channels(AgsCoreAudioPort *core_audio_port,
					  guint pcm_channels);
guint ags_core_audio_port_get_pcm_channels(AgsCoreAudioPort *core_audio_port);

void ags_core_audio_port_set_buffer_size(AgsCoreAudioPort *core_audio_port,
					 guint buffer_size);
guint ags_core_audio_port_get_buffer_size(AgsCoreAudioPort *core_audio_port);

void ags_core_audio_port_set_format(AgsCoreAudioPort *core_audio_port,
				    AgsSoundcardFormat format);
AgsSoundcardFormat ags_core_audio_port_get_format(AgsCoreAudioPort *core_audio_port);

void ags_core_audio_port_set_samplerate(AgsCoreAudioPort *core_audio_port,
					  guint samplerate);
guint ags_core_audio_port_get_samplerate(AgsCoreAudioPort *core_audio_port);

/*  */
GList* ags_core_audio_port_find(GList *core_audio_port,
				gchar *port_name);

/* register/unregister */
void ags_core_audio_port_register(AgsCoreAudioPort *core_audio_port,
				  gchar *port_name,
				  gboolean is_audio, gboolean is_midi,
				  gboolean is_output);
void ags_core_audio_port_unregister(AgsCoreAudioPort *core_audio_port);

void ags_core_audio_port_set_cache_buffer_size(AgsCoreAudioPort *core_audio_port,
					       guint cache_buffer_size);

guint ags_core_audio_port_get_latency(AgsCoreAudioPort *core_audio_port);

/* instantiate */
AgsCoreAudioPort* ags_core_audio_port_new(GObject *core_audio_client);

G_END_DECLS

#endif /*__AGS_CORE_AUDIO_PORT_H__*/
