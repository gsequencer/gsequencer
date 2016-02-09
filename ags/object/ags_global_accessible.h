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

#ifndef __AGS_GLOBAL_ACCESSIBLE_H__
#define __AGS_GLOBAL_ACCESSIBLE_H__

#include <glib-object.h>

#define AGS_TYPE_GLOBAL_ACCESSIBLE                    (ags_global_accessible_get_type())
#define AGS_GLOBAL_ACCESSIBLE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_GLOBAL_ACCESSIBLE, AgsGlobalAccessible))
#define AGS_GLOBAL_ACCESSIBLE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_GLOBAL_ACCESSIBLE, AgsGlobalAccessibleInterface))
#define AGS_IS_GLOBAL_ACCESSIBLE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_GLOBAL_ACCESSIBLE))
#define AGS_IS_GLOBAL_ACCESSIBLE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_GLOBAL_ACCESSIBLE))
#define AGS_GLOBAL_ACCESSIBLE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_GLOBAL_ACCESSIBLE, AgsGlobalAccessibleInterface))

typedef void AgsGlobalAccessible;
typedef struct _AgsGlobalAccessibleInterface AgsGlobalAccessibleInterface;

struct _AgsGlobalAccessibleInterface
{
  AtkAction action;

  gboolean (*move_to_insert_note_dialog)(AgsGlobalAccessible *global_accessible);
  gboolean (*move_to_delete_note_dialog)(AgsGlobalAccessible *global_accessible);
  gboolean (*move_to_edit_pattern_dialog)(AgsGlobalAccessible *global_accessible);
  gboolean (*move_to_set_sequence_length_dialog)(AgsGlobalAccessible *global_accessible);

  gboolean (*move_to_main_window)(AgsGlobalAccessible *global_accessible);
  gboolean (*move_to_preferences_dialog)(AgsGlobalAccessible *global_accessible);
  gboolean (*move_to_export_window)(AgsGlobalAccessible *global_accessible);
  gboolean (*move_to_rename_dialog)(AgsGlobalAccessible *global_accessible);
  gboolean (*move_to_properties_dialog)(AgsGlobalAccessible *global_accessible);
  gboolean (*move_to_link_index_dialog)(AgsGlobalAccessible *global_accessible);
  gboolean (*move_to_open_dialog)(AgsGlobalAccessible *global_accessible);
  gboolean (*move_to_save_as_dialog)(AgsGlobalAccessible *global_accessible);
  gboolean (*move_to_about_dialog)(AgsGlobalAccessible *global_accessible);
  gboolean (*move_to_confirm_close_dialog)(AgsGlobalAccessible *global_accessible);
  gboolean (*move_to_error_dialog)(AgsGlobalAccessible *global_accessible);
};

GType ags_global_accessible_get_type();

gboolean ags_global_accessible_move_to_insert_note_dialog(AgsGlobalAccessible *global_accessible);
gboolean ags_global_accessible_move_to_delete_note_dialog(AgsGlobalAccessible *global_accessible);
gboolean ags_global_accessible_move_to_edit_pattern_dialog(AgsGlobalAccessible *global_accessible);
gboolean ags_global_accessible_move_to_sequence_length_dialog(AgsGlobalAccessible *global_accessible);

gboolean ags_global_accessible_move_to_main_window(AgsGlobalAccessible *global_accessible);
gboolean ags_global_accessible_move_to_preferences_dialog(AgsGlobalAccessible *global_accessible);
gboolean ags_global_accessible_move_to_export_window(AgsGlobalAccessible *global_accessible);
gboolean ags_global_accessible_move_to_rename_dialog(AgsGlobalAccessible *global_accessible);
gboolean ags_global_accessible_move_to_properties_dialog(AgsGlobalAccessible *global_accessible);
gboolean ags_global_accessible_move_to_link_index_dialog(AgsGlobalAccessible *global_accessible);
gboolean ags_global_accessible_move_to_confirm_open_dialog(AgsGlobalAccessible *global_accessible);
gboolean ags_global_accessible_move_to_confirm_save_as_dialog(AgsGlobalAccessible *global_accessible);
gboolean ags_global_accessible_move_to_about_dialog(AgsGlobalAccessible *global_accessible);
gboolean ags_global_accessible_move_to_confirm_close_dialog(AgsGlobalAccessible *global_accessible);
gboolean ags_global_accessible_move_to_error_dialog(AgsGlobalAccessible *global_accessible);

#endif /*__AGS_GLOBAL_ACCESSIBLE_H__*/
