/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_GSTREAMER_PORT_H__
#define __AGS_GSTREAMER_PORT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/ags_api_config.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_GSTREAMER_PORT                (ags_gstreamer_port_get_type())
#define AGS_GSTREAMER_PORT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_GSTREAMER_PORT, AgsGstreamerPort))
#define AGS_GSTREAMER_PORT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_GSTREAMER_PORT, AgsGstreamerPort))
#define AGS_IS_GSTREAMER_PORT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_GSTREAMER_PORT))
#define AGS_IS_GSTREAMER_PORT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_GSTREAMER_PORT))
#define AGS_GSTREAMER_PORT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_GSTREAMER_PORT, AgsGstreamerPortClass))

#define AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(obj) (&(((AgsGstreamerPort *) obj)->obj_mutex))

#define AGS_GSTREAMER_PORT_DEFAULT_CACHE_BUFFER_SIZE (4096)

typedef struct _AgsGstreamerPort AgsGstreamerPort;
typedef struct _AgsGstreamerPortClass AgsGstreamerPortClass;

/**
 * AgsGstreamerPortFlags:
 * @AGS_GSTREAMER_PORT_REGISTERED: the port was registered
 * @AGS_GSTREAMER_PORT_IS_AUDIO: the port provides audio data
 * @AGS_GSTREAMER_PORT_IS_MIDI: the port provides midi data
 * @AGS_GSTREAMER_PORT_IS_OUTPUT: the port does output
 * @AGS_GSTREAMER_PORT_IS_INPUT: the port does input
 * 
 * Enum values to control the behavior or indicate internal state of #AgsGstreamerPort by
 * enable/disable as flags.
 */
typedef enum{
  AGS_GSTREAMER_PORT_REGISTERED         = 1,
  AGS_GSTREAMER_PORT_IS_AUDIO           = 1 <<  1,
  AGS_GSTREAMER_PORT_IS_MIDI            = 1 <<  2,
  AGS_GSTREAMER_PORT_IS_OUTPUT          = 1 <<  3,
  AGS_GSTREAMER_PORT_IS_INPUT           = 1 <<  4,
}AgsGstreamerPortFlags;

struct _AgsGstreamerPort
{
  GObject gobject;

  AgsGstreamerPortFlags flags;
  AgsConnectableFlags connectable_flags;

  GRecMutex obj_mutex;

  GObject *gstreamer_client;

  AgsUUID *uuid;

  GObject *gstreamer_devout;
  GObject *gstreamer_devin;
  
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
    
  void *empty_buffer;

  volatile gboolean is_empty;
  volatile gint underflow;
  volatile gboolean restart;
  
  guint nth_empty_buffer;
  
  volatile guint queued;
};

struct _AgsGstreamerPortClass
{
  GObjectClass gobject;
};

GType ags_gstreamer_port_get_type();
GType ags_gstreamer_port_flags_get_type();

gboolean ags_gstreamer_port_test_flags(AgsGstreamerPort *gstreamer_port, AgsGstreamerPortFlags flags);
void ags_gstreamer_port_set_flags(AgsGstreamerPort *gstreamer_port, AgsGstreamerPortFlags flags);
void ags_gstreamer_port_unset_flags(AgsGstreamerPort *gstreamer_port, AgsGstreamerPortFlags flags);

GList* ags_gstreamer_port_find(GList *gstreamer_port,
			       gchar *port_name);

void ags_gstreamer_port_register(AgsGstreamerPort *gstreamer_port,
				 gchar *port_name,
				 gboolean is_audio, gboolean is_midi,
				 gboolean is_output);
void ags_gstreamer_port_unregister(AgsGstreamerPort *gstreamer_port);

guint ags_gstreamer_port_get_fixed_size(AgsGstreamerPort *gstreamer_port);

void ags_gstreamer_port_set_samplerate(AgsGstreamerPort *gstreamer_port,
				       guint samplerate);
void ags_gstreamer_port_set_pcm_channels(AgsGstreamerPort *gstreamer_port,
					 guint pcm_channels);
void ags_gstreamer_port_set_buffer_size(AgsGstreamerPort *gstreamer_port,
					guint buffer_size);
void ags_gstreamer_port_set_format(AgsGstreamerPort *gstreamer_port,
				   AgsSoundcardFormat format);

void ags_gstreamer_port_set_cache_buffer_size(AgsGstreamerPort *gstreamer_port,
					      guint cache_buffer_size);

guint ags_gstreamer_port_get_latency(AgsGstreamerPort *gstreamer_port);

AgsGstreamerPort* ags_gstreamer_port_new(GObject *gstreamer_client);

G_END_DECLS

#endif /*__AGS_GSTREAMER_PORT_H__*/
