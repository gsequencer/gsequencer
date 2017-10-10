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

#ifndef __AGS_JACK_PORT_H__
#define __AGS_JACK_PORT_H__

#include <glib.h>
#include <glib-object.h>

#include <pthread.h>

#include <ags/config.h>

#ifdef AGS_WITH_JACK
#include <jack/jack.h>
#endif

#define AGS_TYPE_JACK_PORT                (ags_jack_port_get_type())
#define AGS_JACK_PORT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_JACK_PORT, AgsJackPort))
#define AGS_JACK_PORT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_JACK_PORT, AgsJackPort))
#define AGS_IS_JACK_PORT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_JACK_PORT))
#define AGS_IS_JACK_PORT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_JACK_PORT))
#define AGS_JACK_PORT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_JACK_PORT, AgsJackPortClass))

typedef struct _AgsJackPort AgsJackPort;
typedef struct _AgsJackPortClass AgsJackPortClass;

/**
 * AgsJackPortFlags:
 * @AGS_JACK_PORT_CONNECTED: indicates the port was connected by calling #AgsConnectable::connect()
 * @AGS_JACK_PORT_REGISTERED: the port was registered
 * @AGS_JACK_PORT_IS_AUDIO: the port provides audio data
 * @AGS_JACK_PORT_IS_MIDI: the port provides midi data
 * @AGS_JACK_PORT_IS_OUTPUT: the port does output
 * @AGS_JACK_PORT_IS_INPUT: the port does input
 * 
 * Enum values to control the behavior or indicate internal state of #AgsJackPort by
 * enable/disable as flags.
 */
typedef enum{
  AGS_JACK_PORT_CONNECTED       = 1,
  AGS_JACK_PORT_REGISTERED      = 1 <<  1,
  AGS_JACK_PORT_IS_AUDIO        = 1 <<  2,
  AGS_JACK_PORT_IS_MIDI         = 1 <<  3,
  AGS_JACK_PORT_IS_OUTPUT       = 1 <<  4,
  AGS_JACK_PORT_IS_INPUT        = 1 <<  5,
}AgsJackPortFlags;

struct _AgsJackPort
{
  GObject object;

  guint flags;

  pthread_mutex_t *mutex;
  pthread_mutexattr_t *mutexattr;

  GObject *jack_client;
  
  gchar *uuid;
  gchar *name;
  
#ifdef AGS_WITH_JACK
  jack_port_t *port;
#else
  gpointer port;
#endif
};

struct _AgsJackPortClass
{
  GObjectClass object;
};

GType ags_jack_port_get_type();

GList* ags_jack_port_find(GList *jack_port,
			  gchar *port_name);

void ags_jack_port_register(AgsJackPort *jack_port,
			    gchar *port_name,
			    gboolean is_audio, gboolean is_midi,
			    gboolean is_output);
void ags_jack_port_unregister(AgsJackPort *jack_port);

AgsJackPort* ags_jack_port_new(GObject *jack_client);

#endif /*__AGS_JACK_PORT_H__*/
