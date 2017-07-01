/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_ENVELOPE_EDITOR_H__
#define __AGS_ENVELOPE_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/audio/ags_preset.h>

#include <ags/widget/ags_cartesian.h>

#define AGS_TYPE_ENVELOPE_EDITOR                (ags_envelope_editor_get_type())
#define AGS_ENVELOPE_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ENVELOPE_EDITOR, AgsEnvelopeEditor))
#define AGS_ENVELOPE_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ENVELOPE_EDITOR, AgsEnvelopeEditorClass))
#define AGS_IS_ENVELOPE_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ENVELOPE_EDITOR))
#define AGS_IS_ENVELOPE_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_ENVELOPE_EDITOR))
#define AGS_ENVELOPE_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_ENVELOPE_EDITOR, AgsEnvelopeEditorClass))

#define AGS_ENVELOPE_EDITOR_DEFAULT_VERSION "0.8.2\0"
#define AGS_ENVELOPE_EDITOR_DEFAULT_BUILD_ID "CEST 24-05-2017 10:16\0"

typedef struct _AgsEnvelopeEditor AgsEnvelopeEditor;
typedef struct _AgsEnvelopeEditorClass AgsEnvelopeEditorClass;

typedef enum{
  AGS_ENVELOPE_EDITOR_CONNECTED      = 1,
  AGS_ENVELOPE_EDITOR_NO_UPDATE      = 1 <<  1,
}AgsEnvelopeEditorFlags;

struct _AgsEnvelopeEditor
{
  GtkVBox vbox;

  guint flags;
  
  gchar *version;
  gchar *build_id;

  GtkCheckButton *enabled;

  GtkDialog *rename;

  GtkComboBoxText *preset;
  GtkButton *add;
  GtkButton *remove;
  
  AgsCartesian *cartesian;
  
  GtkHScale *attack_x;
  GtkHScale *attack_y;
  
  GtkHScale *decay_x;
  GtkHScale *decay_y;

  GtkHScale *sustain_x;
  GtkHScale *sustain_y;

  GtkHScale *release_x;
  GtkHScale *release_y;

  GtkHScale *ratio;
};

struct _AgsEnvelopeEditorClass
{
  GtkVBoxClass vbox;
};

GType ags_envelope_editor_get_type(void);

AgsPreset* ags_envelope_editor_get_active_preset(AgsEnvelopeEditor *envelope_editor);

void ags_envelope_editor_load_preset(AgsEnvelopeEditor *envelope_editor);
void ags_envelope_editor_add_preset(AgsEnvelopeEditor *envelope_editor,
				    gchar *preset_name);
void ags_envelope_editor_remove_preset(AgsEnvelopeEditor *envelope_editor,
				       guint nth);

void ags_envelope_editor_reset_control(AgsEnvelopeEditor *envelope_editor);

void ags_envelope_editor_plot(AgsEnvelopeEditor *envelope_editor);

AgsEnvelopeEditor* ags_envelope_editor_new();

#endif /*__AGS_ENVELOPE_EDITOR_H__*/
