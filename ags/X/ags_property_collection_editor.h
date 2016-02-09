/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_PROPERTY_COLLECTION_EDITOR_H__
#define __AGS_PROPERTY_COLLECTION_EDITOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_property_editor.h>

#include <stdarg.h>

#define AGS_TYPE_PROPERTY_COLLECTION_EDITOR                (ags_property_collection_editor_get_type())
#define AGS_PROPERTY_COLLECTION_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PROPERTY_COLLECTION_EDITOR, AgsPropertyCollectionEditor))
#define AGS_PROPERTY_COLLECTION_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PROPERTY_COLLECTION_EDITOR, AgsPropertyCollectionEditorClass))
#define AGS_IS_PROPERTY_COLLECTION_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PROPERTY_COLLECTION_EDITOR))
#define AGS_IS_PROPERTY_COLLECTION_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PROPERTY_COLLECTION_EDITOR))
#define AGS_PROPERTY_COLLECTION_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PROPERTY_COLLECTION_EDITOR, AgsPropertyCollectionEditorClass))

typedef struct _AgsPropertyCollectionEditor AgsPropertyCollectionEditor;
typedef struct _AgsPropertyCollectionEditorClass AgsPropertyCollectionEditorClass;

struct _AgsPropertyCollectionEditor
{
  AgsPropertyEditor property_editor;

  GType child_type;
  guint child_parameter_count;
  GParameter *child_parameter;

  GtkVBox *child;

  GtkButton *add_collection;
};

struct _AgsPropertyCollectionEditorClass
{
  AgsPropertyEditorClass property_editor;
};

GType ags_property_collection_editor_get_type();

AgsPropertyCollectionEditor* ags_property_collection_editor_new(GType child_type,
								guint child_parameter_count,
								GParameter *child_parameter);

#endif /*__AGS_PROPERTY_COLLECTION_EDITOR_H__*/

