#ifndef __AGS_DRUM_H__
#define __AGS_DRUM_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "../ags_machine.h"

#include "ags_drum_input_pad.h"

#include "../../audio/recall/ags_delay_audio.h"
#include "../../audio/recall/ags_copy_pattern_audio.h"
#include "../../audio/recall/ags_copy_pattern_audio_run.h"

#define AGS_TYPE_DRUM                (ags_drum_get_type())
#define AGS_DRUM(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DRUM, AgsDrum))
#define AGS_DRUM_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DRUM, AgsDrumClass))
#define AGS_IS_DRUM(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DRUM))
#define AGS_IS_DRUM_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_DRUM))
#define AGS_DRUM_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_DRUM, AgsDrumClass))

typedef struct _AgsDrum AgsDrum;
typedef struct _AgsDrumClass AgsDrumClass;

typedef enum{
  AGS_DRUM_BLOCK_PATTERN    =  1,
}AgsDrumFlags;

struct _AgsDrum
{
  AgsMachine machine;

  guint flags;

  AgsDelayAudio *delay_audio;

  AgsCopyPatternAudio *copy_pattern_audio;
  AgsCopyPatternAudioRun *copy_pattern_audio_run;

  GtkButton *open;

  GtkCheckButton *loop_button;
  GtkSpinButton *length_spin;
  GtkOptionMenu *tic;
  gulong bpm_handler;

  GtkToggleButton *run;

  GtkVBox *vbox;

  GtkHBox *input_pad;
  GtkVBox *output_pad;

  GtkToggleButton *selected_edit_button;

  AgsDrumInputPad *selected_pad;
  AgsLine *selected_line;

  GtkToggleButton *index0[4];
  GtkToggleButton *selected0;

  GtkToggleButton *index1[12];
  GtkToggleButton *selected1;

  GtkHBox *pattern;

  GtkVBox *offset;
};

struct _AgsDrumClass
{
  AgsMachineClass machine;
};

void ags_drum_set_pattern(AgsDrum *drum);

AgsDrum* ags_drum_new();

#endif /*__AGS_DRUM_H__*/
