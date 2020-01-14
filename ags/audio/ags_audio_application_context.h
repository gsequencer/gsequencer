/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_AUDIO_APPLICATION_CONTEXT_H__
#define __AGS_AUDIO_APPLICATION_CONTEXT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUDIO_APPLICATION_CONTEXT                (ags_audio_application_context_get_type())
#define AGS_AUDIO_APPLICATION_CONTEXT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_APPLICATION_CONTEXT, AgsAudioApplicationContext))
#define AGS_AUDIO_APPLICATION_CONTEXT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUDIO_APPLICATION_CONTEXT, AgsAudioApplicationContextClass))
#define AGS_IS_AUDIO_APPLICATION_CONTEXT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUDIO_APPLICATION_CONTEXT))
#define AGS_IS_AUDIO_APPLICATION_CONTEXT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUDIO_APPLICATION_CONTEXT))
#define AGS_AUDIO_APPLICATION_CONTEXT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_AUDIO_APPLICATION_CONTEXT, AgsAudioApplicationContextClass))

#define AGS_AUDIO_DEFAULT_VERSION "3.0.0"
#define AGS_AUDIO_BUILD_ID "Fri Nov  8 21:47:01 UTC 2019"

#define AGS_EFFECTS_DEFAULT_VERSION "0.7.13"

typedef struct _AgsAudioApplicationContext AgsAudioApplicationContext;
typedef struct _AgsAudioApplicationContextClass AgsAudioApplicationContextClass;

struct _AgsAudioApplicationContext
{
  AgsApplicationContext application_context;

  guint flags;
  
  AgsThreadPool *thread_pool;

  GList *worker;

  GMainContext *server_main_context;

  gboolean is_operating;

  AgsServerStatus *server_status;
  
  AgsRegistry *registry;
  
  GList *server;

  GMainContext *audio_main_context;
  GMainContext *osc_server_main_context;
  
  GObject *default_soundcard;

  AgsThread *default_soundcard_thread;
  AgsThread *default_export_thread;
  
  GList *soundcard;
  GList *sequencer;

  GList *audio;

  GList *sound_server;

  GList *osc_server;
};

struct _AgsAudioApplicationContextClass
{
  AgsApplicationContextClass application_context;
};

GType ags_audio_application_context_get_type();

AgsAudioApplicationContext* ags_audio_application_context_new();

G_END_DECLS

#endif /*__AGS_AUDIO_APPLICATION_CONTEXT_H__*/
