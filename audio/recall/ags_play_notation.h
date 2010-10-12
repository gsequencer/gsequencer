#ifndef __AGS_PLAY_NOTATION_H__
#define __AGS_PLAY_NOTATION_H__

#include <glib-object.h>

#include "../ags_recall.h"
#include "../ags_channel.h"
#include "../ags_notation.h"

#define AGS_TYPE_PLAY_NOTATION             (ags_play_notation_get_type())
#define AGS_PLAY_NOTATION(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_NOTATION, AgsPlayNotation))
#define AGS_PLAY_NOTATION_CLASS(class)     (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAY_NOTATION, AgsPlayNotation))

typedef struct _AgsPlayNotation AgsPlayNotation;
typedef struct _AgsPlayNotationClass AgsPlayNotationClass;

struct _AgsPlayNotation
{
  AgsRecall recall;

  AgsDevout *devout;

  AgsChannel *channel;
  AgsNotation *notation;

  //  double *bpm; now in AgsDevout

  GList *current; // 
};

struct _AgsPlayNotationClass
{
  AgsRecallClass recall;
};

GType ags_play_notation_get_type();

AgsPlayNotation* ags_play_notation_new();

#endif /*__AGS_PLAY_NOTATION_H__*/
