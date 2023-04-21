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

#ifndef __AGS_AUDIO_UNIT_PORT_H__
#define __AGS_AUDIO_UNIT_PORT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/ags_api_config.h>

#ifdef AGS_WITH_AUDIO_UNIT
#include <AudioToolbox/AudioToolbox.h>

#include <AudioUnit/AudioUnit.h>
#include <AudioUnit/AUComponent.h>
#include <AudioUnit/AudioComponent.h>

#include <CoreMIDI/CoreMIDI.h>
#endif

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUDIO_UNIT_PORT                (ags_audio_unit_port_get_type())
#define AGS_AUDIO_UNIT_PORT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_UNIT_PORT, AgsAudioUnitPort))
#define AGS_AUDIO_UNIT_PORT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUDIO_UNIT_PORT, AgsAudioUnitPort))
#define AGS_IS_AUDIO_UNIT_PORT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUDIO_UNIT_PORT))
#define AGS_IS_AUDIO_UNIT_PORT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUDIO_UNIT_PORT))
#define AGS_AUDIO_UNIT_PORT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_AUDIO_UNIT_PORT, AgsAudioUnitPortClass))

#define AGS_AUDIO_UNIT_PORT_GET_OBJ_MUTEX(obj) (&(((AgsAudioUnitPort *) obj)->obj_mutex))

#define AGS_AUDIO_UNIT_PORT_DEFAULT_CACHE_BUFFER_SIZE (4096)

typedef struct _AgsAudioUnitPort AgsAudioUnitPort;
typedef struct _AgsAudioUnitPortClass AgsAudioUnitPortClass;

/**
 * AgsAudioUnitPortFlags:
 * @AGS_AUDIO_UNIT_PORT_REGISTERED: the port was registered
 * @AGS_AUDIO_UNIT_PORT_IS_AUDIO: the port provides audio data
 * @AGS_AUDIO_UNIT_PORT_IS_MIDI: the port provides midi data
 * @AGS_AUDIO_UNIT_PORT_IS_OUTPUT: the port does output
 * @AGS_AUDIO_UNIT_PORT_IS_INPUT: the port does input
 * 
 * Enum values to control the behavior or indicate internal state of #AgsAudioUnitPort by
 * enable/disable as flags.
 */
typedef enum{
  AGS_AUDIO_UNIT_PORT_REGISTERED         = 1 <<  2,
  AGS_AUDIO_UNIT_PORT_IS_AUDIO           = 1 <<  3,
  AGS_AUDIO_UNIT_PORT_IS_MIDI            = 1 <<  4,
  AGS_AUDIO_UNIT_PORT_IS_OUTPUT          = 1 <<  5,
  AGS_AUDIO_UNIT_PORT_IS_INPUT           = 1 <<  6,
}AgsAudioUnitPortFlags;

struct _AgsAudioUnitPort
{
  GObject gobject;

  AgsAudioUnitPortFlags flags;
  AgsConnectableFlags connectable_flags;
  
  GRecMutex obj_mutex;

  GObject *audio_unit_client;

  AgsUUID *uuid;

  GObject *audio_unit_device;
  
  gchar *port_uuid;
  gchar *port_name;

  guint pcm_channels;

  guint samplerate;
  guint buffer_size;
  AgsSoundcardFormat format;

#ifdef AGS_WITH_AUDIO_UNIT
  AudioStreamBasicDescription *data_format;
  
  AUGraph *graph;

  AudioComponentDescription *description;
  AUNode *node;

  AudioComponent audio_component;
  AudioUnit *audio_unit;

  AURenderCallbackStruct *render_callback;
#else
  gpointer data_format;
  
  gpointer graph;

  gpointer description;
  gpointer node;

  gpointer audio_component;
  gpointer audio_unit;

  gpointer render_callback;
#endif

  unsigned int midi_port_number;

  volatile gboolean output_running;
  volatile gboolean input_running;
  volatile gboolean is_empty;
  volatile guint queued;
};

struct _AgsAudioUnitPortClass
{
  GObjectClass gobject;
};

GType ags_audio_unit_port_get_type();
GType ags_audio_unit_port_flags_get_type();

gboolean ags_audio_unit_port_test_flags(AgsAudioUnitPort *audio_unit_port, AgsAudioUnitPortFlags flags);
void ags_audio_unit_port_set_flags(AgsAudioUnitPort *audio_unit_port, AgsAudioUnitPortFlags flags);
void ags_audio_unit_port_unset_flags(AgsAudioUnitPort *audio_unit_port, AgsAudioUnitPortFlags flags);

GList* ags_audio_unit_port_find(GList *audio_unit_port,
				gchar *port_name);

void ags_audio_unit_port_register(AgsAudioUnitPort *audio_unit_port,
				  gchar *port_name,
				  gboolean is_audio, gboolean is_midi,
				  gboolean is_output);
void ags_audio_unit_port_unregister(AgsAudioUnitPort *audio_unit_port);

void ags_audio_unit_port_set_format(AgsAudioUnitPort *audio_unit_port,
				    AgsSoundcardFormat format);
void ags_audio_unit_port_set_samplerate(AgsAudioUnitPort *audio_unit_port,
					guint samplerate);
void ags_audio_unit_port_set_pcm_channels(AgsAudioUnitPort *audio_unit_port,
					  guint pcm_channels);
void ags_audio_unit_port_set_buffer_size(AgsAudioUnitPort *audio_unit_port,
					 guint buffer_size);

AgsAudioUnitPort* ags_audio_unit_port_new(GObject *audio_unit_client);

G_END_DECLS

#endif /*__AGS_AUDIO_UNIT_PORT_H__*/
