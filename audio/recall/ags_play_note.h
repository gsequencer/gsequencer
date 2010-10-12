#ifndef __AGS_PLAY_NOTE_H__
#define __AGS_PLAY_NOTE_H__

#include <glib-object.h>

#include "../ags_devout.h"
#include "../ags_recall.h"
#include "../ags_channel.h"

#define AGS_TYPE_PLAY_NOTE             (ags_play_note_get_type())
#define AGS_PLAY_NOTE(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_NOTE, AgsPlayNote))
#define AGS_PLAY_NOTE_CLASS(class)     (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAY_NOTE, AgsPlayNote))

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
