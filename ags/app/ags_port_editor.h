/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_PORT_EDITOR_H__
#define __AGS_PORT_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_PORT_EDITOR                (ags_port_editor_get_type())
#define AGS_PORT_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_PORT_EDITOR, AgsPortEditor))
#define AGS_PORT_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_PORT_EDITOR, AgsPortEditorClass))
#define AGS_IS_PORT_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PORT_EDITOR))
#define AGS_IS_PORT_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PORT_EDITOR))
#define AGS_PORT_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PORT_EDITOR, AgsPortEditorClass))

#define AGS_PORT_EDITOR_CONTROL_BUTTON "GtkButton"
#define AGS_PORT_EDITOR_CONTROL_TOGGLE_BUTTON "GtkToggleButton"
#define AGS_PORT_EDITOR_CONTROL_CHECK_BUTTON "GtkCheckButton"
#define AGS_PORT_EDITOR_CONTROL_SPIN_BUTTON "GtkSpinButton"
#define AGS_PORT_EDITOR_CONTROL_SCALE "GtkScale"
#define AGS_PORT_EDITOR_CONTROL_DIAL "AgsDial"
#define AGS_PORT_EDITOR_CONTROL_LED "AgsLed"
#define AGS_PORT_EDITOR_CONTROL_INDICATOR "AgsIndicator"
#define AGS_PORT_EDITOR_CONTROL_VECTOR "AgsVector"
#define AGS_PORT_EDITOR_CONTROL_PIANO "AgsPiano"
#define AGS_PORT_EDITOR_CONTROL_CARTESIAN "AgsCartesian"

typedef struct _AgsPortEditor AgsPortEditor;
typedef struct _AgsPortEditorClass AgsPortEditorClass;

typedef enum{
  AGS_PORT_EDITOR_IS_OUTPUT      = 1,
  AGS_PORT_EDITOR_IS_INPUT       = 1 <<  1,
  AGS_PORT_EDITOR_IS_TRIGGER     = 1 <<  2,
  AGS_PORT_EDITOR_IS_BOOLEAN     = 1 <<  3,
  AGS_PORT_EDITOR_IS_ADJUSTMENT  = 1 <<  4,
  AGS_PORT_EDITOR_IS_COMPLEX     = 1 <<  5,
  AGS_PORT_EDITOR_IS_MIDI        = 1 <<  6,
  AGS_PORT_EDITOR_IS_FFTW3       = 1 <<  7,
}AgsPortEditorFlags;

struct _AgsPortEditor
{
  GtkGrid grid;

  guint flags;
  guint connectable_flags;
  
  GtkLabel *port_name;

  GtkComboBox *port_control;

  GtkComboBox *port_control_orientation;
};

struct _AgsPortEditorClass
{
  GtkGridClass grid;
};

GType ags_port_editor_get_type(void);

gboolean ags_port_editor_test_flags(AgsPortEditor *port_editor,
				    guint flags);
void ags_port_editor_set_flags(AgsPortEditor *port_editor,
			       guint flags);
void ags_port_editor_unset_flags(AgsPortEditor *port_editor,
				 guint flags);

AgsPortEditor* ags_port_editor_new();

G_END_DECLS

#endif /*__AGS_PORT_EDITOR_H__*/
