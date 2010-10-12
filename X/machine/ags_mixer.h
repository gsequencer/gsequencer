#ifndef __AGS_MIXER_H__
#define __AGS_MIXER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "../ags_machine.h"

#define AGS_TYPE_MIXER                (ags_mixer_get_type())
#define AGS_MIXER(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_MIXER, AgsMixer))
#define AGS_MIXER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_MIXER, AgsMixerClass))
#define AGS_IS_MIXER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE(obj, AGS_TYPE_MIXER))
#define AGS_IS_MIXER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_MIXER))
#define AGS_MIXER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MIXER, AgsMixerClass))

typedef struct _AgsMixer AgsMixer;
typedef struct _AgsMixerClass AgsMixerClass;

struct _AgsMixer
{
  AgsMachine machine;

  GtkHBox *pad;
};

struct _AgsMixerClass
{
  AgsMachineClass machine;
};

GType ags_mixer_get_type(void);

AgsMixer* ags_mixer_new();

#endif /*__AGS_MIXER_H__*/
