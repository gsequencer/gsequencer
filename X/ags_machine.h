#ifndef __AGS_MACHINE_H__
#define __AGS_MACHINE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "../audio/ags_audio.h"
#include "../file/ags_file.h"

#define AGS_TYPE_MACHINE                (ags_machine_get_type())
#define AGS_MACHINE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MACHINE, AgsMachine))
#define AGS_MACHINE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MACHINE, AgsMachineClass))
#define AGS_IS_MACHINE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MACHINE))
#define AGS_IS_MACHINE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_MACHINE))
#define AGS_MACHINE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MACHINE, AgsMachineClass))

typedef struct _AgsMachine AgsMachine;
typedef struct _AgsMachineClass AgsMachineClass;

typedef enum{
  AGS_MACHINE_SOLO,
}AgsMachineFlags;

struct _AgsMachine
{
  GtkHandleBox handle_box;

  guint flags;

  char *name;

  AgsAudio *audio;

  GtkContainer *output;
  GtkContainer *input;

  GtkMenu *popup;
  GtkDialog *properties;
  GtkDialog *rename;
};

struct _AgsMachineClass
{
  GtkHandleBoxClass handle_box;
};

GType ags_machine_get_type(void);

AgsMachine* ags_machine_new();

AgsMachine* ags_machine_find_by_name(GList *list, char *name);

#endif /*__AGS_MACHINE_H__*/
