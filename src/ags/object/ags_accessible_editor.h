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

#ifndef __AGS_ACCESSIBLE_EDITOR_H__
#define __AGS_ACCESSIBLE_EDITOR_H__

#include <glib-object.h>

#define AGS_TYPE_ACCESSIBLE_EDITOR                    (ags_accessible_editor_get_type())
#define AGS_ACCESSIBLE_EDITOR(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ACCESSIBLE_EDITOR, AgsAccessibleEditor))
#define AGS_ACCESSIBLE_EDITOR_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_ACCESSIBLE_EDITOR, AgsAccessibleEditorInterface))
#define AGS_IS_ACCESSIBLE_EDITOR(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ACCESSIBLE_EDITOR))
#define AGS_IS_ACCESSIBLE_EDITOR_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_ACCESSIBLE_EDITOR))
#define AGS_ACCESSIBLE_EDITOR_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_ACCESSIBLE_EDITOR, AgsAccessibleEditorInterface))

typedef void AgsAccessibleEditor;
typedef struct _AgsAccessibleEditorInterface AgsAccessibleEditorInterface;

struct _AgsAccessibleEditorInterface
{
  AgsGenericAccessible generic_accessible;

  gboolean (*insert_note)(AgsAccessibleEditor *accessible_editor);
  gboolean (*delete_note)(AgsAccessibleEditor *accessible_editor);
  gboolean (*select_region)(AgsAccessibleEditor *accessible_editor);
  gboolean (*select_all)(AgsAccessibleEditor *accessible_editor);
  gboolean (*unselect_all)(AgsAccessibleEditor *accessible_editor);
  gboolean (*copy)(AgsAccessibleEditor *accessible_editor);
  gboolean (*cut)(AgsAccessibleEditor *accessible_editor);
  gboolean (*paste)(AgsAccessibleEditor *accessible_editor);

  gboolean (*add_index)(AgsAccessibleEditor *accessible_editor);
  gboolean (*remove_index)(AgsAccessibleEditor *accessible_editor);
  gboolean (*link_index)(AgsAccessibleEditor *accessible_editor);
  gboolean (*reverse_mapping)(AgsAccessibleEditor *accessible_editor);
  
  gboolean (*move_to_index)(AgsAccessibleEditor *accessible_editor);
};

GType ags_accessible_editor_get_type();

gboolean ags_accessible_editor_insert_note(AgsAccessibleEditor *accessible_editor);
gboolean ags_accessible_editor_delete_note(AgsAccessibleEditor *accessible_editor);
gboolean ags_accessible_editor_select_region(AgsAccessibleEditor *accessible_editor);
gboolean ags_accessible_editor_select_all(AgsAccessibleEditor *accessible_editor);
gboolean ags_accessible_editor_unselect_all(AgsAccessibleEditor *accessible_editor);
gboolean ags_accessible_editor_copy(AgsAccessibleEditor *accessible_editor);
gboolean ags_accessible_editor_cut(AgsAccessibleEditor *accessible_editor);
gboolean ags_accessible_editor_paste(AgsAccessibleEditor *accessible_editor);

gboolean ags_accessible_editor_add_index(AgsAccessibleEditor *accessible_editor);
gboolean ags_accessible_editor_remove_index(AgsAccessibleEditor *accessible_editor);
gboolean ags_accessible_editor_link_index(AgsAccessibleEditor *accessible_editor);
gboolean ags_accessible_editor_reverse_mapping(AgsAccessibleEditor *accessible_editor);
  
gboolean ags_accessible_editor_move_to_index(AgsAccessibleEditor *accessible_editor);

#endif /*__AGS_ACCESSIBLE_EDITOR_H__*/
