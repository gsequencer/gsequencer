/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#define AGS_TYPE_AUDIO_APPLICATION_CONTEXT                (ags_audio_application_context_get_type())
#define AGS_AUDIO_APPLICATION_CONTEXT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_APPLICATION_CONTEXT, AgsAudioApplicationContext))
#define AGS_AUDIO_APPLICATION_CONTEXT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUDIO_APPLICATION_CONTEXT, AgsAudioApplicationContextClass))
#define AGS_IS_AUDIO_APPLICATION_CONTEXT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUDIO_APPLICATION_CONTEXT))
#define AGS_IS_AUDIO_APPLICATION_CONTEXT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUDIO_APPLICATION_CONTEXT))
#define AGS_AUDIO_APPLICATION_CONTEXT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_AUDIO_APPLICATION_CONTEXT, AgsAudioApplicationContextClass))

#define AGS_AUDIO_RT_PRIORITY (49)

#define AGS_AUDIO_DEFAULT_VERSION "2.1.6"
#define AGS_AUDIO_BUILD_ID "Mon Dec  3 13:42:15 UTC 2018"

#define AGS_EFFECTS_DEFAULT_VERSION "0.7.13"

typedef struct _AgsAudioApplicationContext AgsAudioApplicationContext;
typedef struct _AgsAudioApplicationContextClass AgsAudioApplicationContextClass;

/**
 * AgsAudioApplicationContextFlags:
 * @AGS_AUDIO_APPLICATION_CONTEXT_USE_ALSA: use alsa backend
 * 
 * Enum values to control the behavior or indicate internal state of #AgsAudioApplicationContextFlags by
 * enable/disable as flags.
 */
typedef enum{
  AGS_AUDIO_APPLICATION_CONTEXT_USE_ALSA      = 1,
}AgsAudioApplicationContextFlags;

struct _AgsAudioApplicationContext
{
  AgsApplicationContext application_context;

  guint flags;

  gchar *version;
  gchar *build_id;

  AgsThreadPool *thread_pool;

  AgsPollingThread *polling_thread;

  GList *worker;

  GObject *default_soundcard;

  AgsThread *default_soundcard_thread;
  AgsThread *default_export_thread;

  AgsThread *autosave_thread;

  AgsServer *server;

  GList *soundcard;
  GList *sequencer;

  GList *sound_server;
  GList *audio;

  GList *osc_server;
};

struct _AgsAudioApplicationContextClass
{
  AgsApplicationContextClass application_context;
};

GType ags_audio_application_context_get_type();

AgsAudioApplicationContext* ags_audio_application_context_new();

#endif /*__AGS_AUDIO_APPLICATION_CONTEXT_H__*/
