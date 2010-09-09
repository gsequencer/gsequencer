#ifndef __AGS_SYNTH_H__
#define __AGS_SYNTH_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include "../ags_machine.h"

#define AGS_TYPE_SYNTH                (ags_synth_get_type())
#define AGS_SYNTH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SYNTH, AgsSynth))
#define AGS_SYNTH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SYNTH, AgsSynthClass))
#define AGS_IS_SYNTH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SYNTH))
#define AGS_IS_SYNTH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SYNTH))
#define AGS_SYNTH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SYNTH, AgsSynthClass))

typedef struct _AgsSynth AgsSynth;
typedef struct _AgsSynthClass AgsSynthClass;

struct _AgsSynth
{
  AgsMachine machine;

  GtkHBox *hbox;

  GtkOptionMenu *oscillator;

  GtkSpinButton *lower; // how many channels until to lowest freq

  GtkCheckButton *auto_update;
  GtkButton *update;

  GtkOptionMenu* amplifier[2];
};

struct _AgsSynthClass
{
  AgsMachineClass machine;
};

AgsSynth* ags_synth_new();

#endif /*__AGS_SYNTH_H__*/
