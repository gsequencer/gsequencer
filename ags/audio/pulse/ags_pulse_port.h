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

#ifndef __AGS_PULSE_PORT_H__
#define __AGS_PULSE_PORT_H__

#include <glib.h>
#include <glib-object.h>

#include <pulse/pulseaudio.h>

#define AGS_TYPE_PULSE_PORT                (ags_pulse_port_get_type())
#define AGS_PULSE_PORT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PULSE_PORT, AgsPulsePort))
#define AGS_PULSE_PORT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_PULSE_PORT, AgsPulsePort))
#define AGS_IS_PULSE_PORT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PULSE_PORT))
#define AGS_IS_PULSE_PORT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PULSE_PORT))
#define AGS_PULSE_PORT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_PULSE_PORT, AgsPulsePortClass))

typedef struct _AgsPulsePort AgsPulsePort;
typedef struct _AgsPulsePortClass AgsPulsePortClass;

typedef enum{
  AGS_PULSE_PORT_CONNECTED       = 1,
  AGS_PULSE_PORT_REGISTERED      = 1 <<  1,
  AGS_PULSE_PORT_IS_AUDIO        = 1 <<  2,
  AGS_PULSE_PORT_IS_MIDI         = 1 <<  3,
  AGS_PULSE_PORT_IS_OUTPUT       = 1 <<  4,
}AgsPulsePortFlags;

struct _AgsPulsePort
{
  GObject object;

  guint flags;

  pthread_mutex_t *mutex;
  pthread_mutexattr_t *mutexattr;

  GObject *pulse_client;
  GObject *pulse_devout;
  
  gchar *uuid;
  gchar *name;

  guint format;
  guint buffer_size;

  pa_stream *stream;
  pa_sample_spec *sample_spec;
  pa_buffer_attr *buffer_attr;
  
  void *empty_buffer;
  volatile gboolean is_empty;
  volatile gboolean underflow;
  
  volatile guint queued;
};

struct _AgsPulsePortClass
{
  GObjectClass object;
};

GType ags_pulse_port_get_type();

GList* ags_pulse_port_find(GList *pulse_port,
			   gchar *port_name);

void ags_pulse_port_register(AgsPulsePort *pulse_port,
			     gchar *port_name,
			     gboolean is_audio, gboolean is_midi,
			     gboolean is_output);
void ags_pulse_port_unregister(AgsPulsePort *pulse_port);

guint ags_pulse_port_get_fixed_size(AgsPulsePort *pulse_port);

void ags_pulse_port_set_samplerate(AgsPulsePort *pulse_port,
				   guint samplerate);
void ags_pulse_port_set_pcm_channels(AgsPulsePort *pulse_port,
				     guint pcm_channels);
void ags_pulse_port_set_buffer_size(AgsPulsePort *pulse_port,
				    guint buffer_size);
void ags_pulse_port_set_format(AgsPulsePort *pulse_port,
			       guint format);

AgsPulsePort* ags_pulse_port_new(GObject *pulse_client);

#endif /*__AGS_PULSE_PORT_H__*/
