/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AGS_OUTPUT_EDITOR_H__
#define __AGS_OUTPUT_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_OUTPUT_EDITOR                (ags_output_editor_get_type())
#define AGS_OUTPUT_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OUTPUT_EDITOR, AgsOutputEditor))
#define AGS_OUTPUT_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OUTPUT_EDITOR, AgsOutputEditorClass))
#define AGS_IS_OUTPUT_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OUTPUT_EDITOR))
#define AGS_IS_OUTPUT_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OUTPUT_EDITOR))
#define AGS_OUTPUT_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_OUTPUT_EDITOR, AgsOutputEditorClass))

#define AGS_OUTPUT_EDITOR_DEFAULT_VERSION "0.7.131"
#define AGS_OUTPUT_EDITOR_DEFAULT_BUILD_ID "CEST 2017 18-02-2014 04:53"

typedef struct _AgsOutputEditor AgsOutputEditor;
typedef struct _AgsOutputEditorClass AgsOutputEditorClass;

typedef enum{
  AGS_OUTPUT_EDITOR_CONNECTED    = 1,
}AgsOutputEditorFlags;

struct _AgsOutputEditor
{
  GtkHBox hbox;

  guint flags;
  
  gchar *version;
  gchar *build_id;

  GtkComboBoxText *soundcard;
  GtkSpinButton *audio_channel;
};

struct _AgsOutputEditorClass
{
  GtkHBoxClass hbox;
};

GType ags_output_editor_get_type(void);

void ags_output_editor_check(AgsOutputEditor *output_editor);

AgsOutputEditor* ags_output_editor_new();

G_END_DECLS

#endif /*__AGS_OUTPUT_EDITOR_H__*/
