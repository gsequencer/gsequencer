#ifndef __AGS_LINK_EDITOR_H__
#define __AGS_LINK_EDITOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_machine.h"

#include "../audio/file/ags_audio_file.h"

#define AGS_TYPE_LINK_EDITOR                (ags_link_editor_get_type())
#define AGS_LINK_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_LINK_EDITOR, AgsLinkEditor))
#define AGS_LINK_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_LINK_EDITOR, AgsLinkEditorClass))
#define AGS_IS_LINK_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LINK_EDITOR))
#define AGS_IS_LINK_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LINK_EDITOR))
#define AGS_LINK_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LINK_EDITOR, AgsLinkEditorClass))

typedef struct _AgsLinkEditor AgsLinkEditor;
typedef struct _AgsLinkEditorClass AgsLinkEditorClass;

typedef enum{
  AGS_LINK_EDITOR_FILE_CHOOSER_PLAY_DONE = 1,
}AgsLinkEditorFlags;

struct _AgsLinkEditor
{
  GtkHBox hbox;

  guint flags;

  GtkOptionMenu *option;
  GtkSpinButton *spin_button;

  AgsAudioFile *audio_file;

  GtkFileChooserDialog *file_chooser;
};

struct _AgsLinkEditorClass
{
  GtkHBoxClass hbox;
};

AgsLinkEditor* ags_link_editor_new();

#endif /*__AGS_LINK_EDITOR_H__*/
