/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_PROPERTY_EDITOR_H__
#define __AGS_PROPERTY_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_PROPERTY_EDITOR                (ags_property_editor_get_type())
#define AGS_PROPERTY_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PROPERTY_EDITOR, AgsPropertyEditor))
#define AGS_PROPERTY_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PROPERTY_EDITOR, AgsPropertyEditorClass))
#define AGS_IS_PROPERTY_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PROPERTY_EDITOR))
#define AGS_IS_PROPERTY_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PROPERTY_EDITOR))
#define AGS_PROPERTY_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PROPERTY_EDITOR, AgsPropertyEditorClass))

typedef struct _AgsPropertyEditor AgsPropertyEditor;
typedef struct _AgsPropertyEditorClass AgsPropertyEditorClass;

typedef enum{
  AGS_PROPERTY_EDITOR_CONNECTED  = 1,
  AGS_PROPERTY_EDITOR_ENABLED    = 1 << 1,
}AgsPropertyEditorFlags;

struct _AgsPropertyEditor
{
  GtkBox box;

  guint flags;

  GtkCheckButton *enabled;
};

struct _AgsPropertyEditorClass
{
  GtkBoxClass box;

  void (*apply)(AgsPropertyEditor *property_editor);
};

GType ags_property_editor_get_type();

AgsPropertyEditor* ags_property_editor_new();

G_END_DECLS

#endif /*__AGS_PROPERTY_EDITOR_H__*/

