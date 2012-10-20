/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_REMOVE_AUDIO_RUN_H__
#define __AGS_REMOVE_AUDIO_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_audio_run.h>

#define __USE_UNIX98
#include <pthread.h>

#define AGS_TYPE_REMOVE_AUDIO_RUN                (ags_remove_audio_run_get_type ())
#define AGS_REMOVE_AUDIO_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_REMOVE_AUDIO_RUN, AgsRemoveAudioRun))
#define AGS_REMOVE_AUDIO_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_REMOVE_AUDIO_RUN, AgsRemoveAudioRunClass))
#define AGS_IS_REMOVE_AUDIO_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_REMOVE_AUDIO_RUN))
#define AGS_IS_REMOVE_AUDIO_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_REMOVE_AUDIO_RUN))
#define AGS_REMOVE_AUDIO_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_REMOVE_AUDIO_RUN, AgsRemoveAudioRunClass))

typedef struct _AgsRemoveAudioRun AgsRemoveAudioRun;
typedef struct _AgsRemoveAudioRunClass AgsRemoveAudioRunClass;

typedef enum{
  AGS_REMOVE_AUDIO_RUN_FIFO_IS_BUSY    = 1,
}AgsRemoveAudioRunFlags;

#define AGS_REMOVE_AUDIO_RUN_ERROR (ags_remove_audio_run_error_quark())

typedef enum{
  AGS_REMOVE_AUDIO_RUN_ERROR_REMOVE_FAILED,
}AgsRemoveAudioRunError;

struct _AgsRemoveAudioRun
{
  AgsRecallAudioRun recall_audio_run;

  guint flags;

  guint fifo_wait_ref;
  pthread_cond_t fifo_cond;
  pthread_mutex_t fifo_mutex;

  GList *unref_list;
};

struct _AgsRemoveAudioRunClass
{
  AgsRecallAudioRunClass recall_audio_run;
};

typedef enum{
  AGS_REMOVE_AUDIO_RUN_UNREF_EXISTS,
  AGS_REMOVE_AUDIO_RUN_UNREF_APPEND,
  AGS_REMOVE_AUDIO_RUN_UNREF_REMOVE,
}AgsRemoveAudioRunFunction;

GType ags_remove_audio_run_get_type();

GQuark ags_remove_audio_run_error_quark();

void ags_remove_audio_run_enter_fifo(AgsRemoveAudioRun *remove_audio_run);
void ags_remove_audio_run_leave_fifo(AgsRemoveAudioRun *remove_audio_run);

gboolean ags_remove_audio_run_unref_exists(AgsRemoveAudioRun *remove_audio_run, GObject *object);
void ags_remove_audio_run_unref_append(AgsRemoveAudioRun *remove_audio_run, GObject *object);
void ags_remove_audio_run_unref_remove(AgsRemoveAudioRun *remove_audio_run, GObject *object,
				       GError **error);

AgsRemoveAudioRun* ags_remove_audio_run_new();

#endif /*__AGS_REMOVE_AUDIO_RUN_H__*/
