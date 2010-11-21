#ifndef __AGS_PLAY_NOTE_H__
#define __AGS_PLAY_NOTE_H__

#include <glib-object.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_channel.h>

#define AGS_TYPE_PLAY_NOTE             (ags_play_note_get_type())
#define AGS_PLAY_NOTE(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_NOTE, AgsPlayNote))
#define AGS_PLAY_NOTE_CLASS(class)     (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAY_NOTE, AgsPlayNote))
#define AGS_IS_PLAY_NOTE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLAY_NOTE))
#define AGS_IS_PLAY_NOTE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLAY_NOTE))
#define AGS_PLAY_NOTE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PLAY_NOTE, AgsPlayNoteClass))

typedef struct _AgsPlayNote AgsPlayNote;
typedef struct _AgsPlayNoteClass AgsPlayNoteClass;

struct _AgsPlayNote
{
  AgsRecall recall;

  AgsDevout *devout;

  AgsChannel *channel;
  AgsNote *note;
};

struct _AgsPlayNoteClass
{
  AgsRecallClass recall;
};

GType ags_play_note_get_type();

AgsPlayNote* ags_play_note_new();

#endif /*__AGS_PLAY_NOTE_H__*/
