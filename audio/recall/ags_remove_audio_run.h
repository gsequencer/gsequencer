#ifndef __AGS_REMOVE_AUDIO_RUN_H__
#define __AGS_REMOVE_AUDIO_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_audio_run.h>

#include <pthread.h>

#define AGS_TYPE_REMOVE_AUDIO_RUN                (ags_remove_audio_run_get_type ())
#define AGS_REMOVE_AUDIO_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_REMOVE_AUDIO_RUN, AgsRemoveAudioRun))
#define AGS_REMOVE_AUDIO_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_REMOVE_AUDIO_RUN, AgsRemoveAudioRunClass))
#define AGS_IS_REMOVE_AUDIO_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_REMOVE_AUDIO_RUN))
#define AGS_IS_REMOVE_AUDIO_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_REMOVE_AUDIO_RUN))
#define AGS_REMOVE_AUDIO_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_REMOVE_AUDIO_RUN, AgsRemoveAudioRunClass))

typedef struct _AgsRemoveAudioRun AgsRemoveAudioRun;
typedef struct _AgsRemoveAudioRunClass AgsRemoveAudioRunClass;
typedef struct _AgsRemoveAudioRunUnref AgsRemoveAudioRunUnref;

typedef enum{
  AGS_REMOVE_AUDIO_RUN_WAIT_EXISTS   = 1,
  AGS_REMOVE_AUDIO_RUN_WAIT_APPEND   = 1 <<  1,
  AGS_REMOVE_AUDIO_RUN_WAIT_REMOVE   = 1 <<  2,
}AgsRemoveAudioRunFlags;

#define AGS_REMOVE_AUDIO_RUN_ERROR (ags_remove_audio_run_error_quark())

typedef enum{
  AGS_REMOVE_AUDIO_RUN_ERROR_REMOVE_FAILED,
}AgsRemoveAudioRunError;

struct _AgsRemoveAudioRun
{
  AgsRecallAudioRun recall_audio_run;

  guint flags;

  GList *unref_list;
};

struct _AgsRemoveAudioRunClass
{
  AgsRecallAudioRunClass recall_audio_run;
};

struct _AgsRemoveAudioRunUnref
{
  GObject *object;

  gboolean locked;
};

GType ags_remove_audio_run_get_type();

GQuark ags_remove_audio_run_error_quark();

gboolean ags_remove_audio_run_unref_exists(AgsRemoveAudioRun *remove_audio_run, GObject *object);
void ags_remove_audio_run_unref_append(AgsRemoveAudioRun *remove_audio_run, GObject *object);
void ags_remove_audio_run_unref_remove(AgsRemoveAudioRun *remove_audio_run, GObject *object,
				       GError **error);

AgsRemoveAudioRun* ags_remove_audio_run_new();

#endif /*__AGS_REMOVE_AUDIO_RUN_H__*/
