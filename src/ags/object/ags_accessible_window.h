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

#ifndef __AGS_ACCESSIBLE_WINDOW_H__
#define __AGS_ACCESSIBLE_WINDOW_H__

#include <glib-object.h>

#define AGS_TYPE_ACCESSIBLE_WINDOW                    (ags_accessible_window_get_type())
#define AGS_ACCESSIBLE_WINDOW(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ACCESSIBLE_WINDOW, AgsAccessibleWindow))
#define AGS_ACCESSIBLE_WINDOW_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_ACCESSIBLE_WINDOW, AgsAccessibleWindowInterface))
#define AGS_IS_ACCESSIBLE_WINDOW(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ACCESSIBLE_WINDOW))
#define AGS_IS_ACCESSIBLE_WINDOW_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_ACCESSIBLE_WINDOW))
#define AGS_ACCESSIBLE_WINDOW_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_ACCESSIBLE_WINDOW, AgsAccessibleWindowInterface))

typedef void AgsAccessibleWindow;
typedef struct _AgsAccessibleWindowInterface AgsAccessibleWindowInterface;

struct _AgsAccessibleWindowInterface
{
  AgsGlobalAccessible global_accessible;

  gboolean (*move_to_machines_pane)(AgsAccessibleWindow *accessible_window);
  gboolean (*move_to_editor_index_pane)(AgsAccessibleWindow *accessible_window);
  gboolean (*move_to_notation_editor_pane)(AgsAccessibleWindow *accessible_window);
  
  gboolean (*open_file)(AgsAccessibleWindow *accessible_window);
  gboolean (*save_file)(AgsAccessibleWindow *accessible_window);
  gboolean (*save_file_as)(AgsAccessibleWindow *accessible_window);
  gboolean (*export_to_audio_file)(AgsAccessibleWindow *accessible_window);
  gboolean (*quit)(AgsAccessibleWindow *accessible_window);

  gboolean (*add_panel)(AgsAccessibleWindow *accessible_window);
  gboolean (*add_mixer)(AgsAccessibleWindow *accessible_window);
  gboolean (*add_drum)(AgsAccessibleWindow *accessible_window);
  gboolean (*add_matrix)(AgsAccessibleWindow *accessible_window);
  gboolean (*add_synth)(AgsAccessibleWindow *accessible_window);
  gboolean (*add_ffplayer)(AgsAccessibleWindow *accessible_window);
  gboolean (*remove_machine)(AgsAccessibleWindow *accessible_window);
  gboolean (*show_preferences)(AgsAccessibleWindow *accessible_window);

  gboolean (*show_about_dialog)(AgsAccessibleWindow *accessible_window);
};

GType ags_accessible_window_get_type();

gboolean ags_accessible_window_move_to_machines_pane(AgsAccessibleWindow *accessible_window);
gboolean ags_accessible_window_move_to_editor_index_pane(AgsAccessibleWindow *accessible_window);
gboolean ags_accessible_window_move_to_notation_editor_pane(AgsAccessibleWindow *accessible_window);
  
gboolean ags_accessible_window_open_file(AgsAccessibleWindow *accessible_window);
gboolean ags_accessible_window_save_file(AgsAccessibleWindow *accessible_window);
gboolean ags_accessible_window_save_file_as(AgsAccessibleWindow *accessible_window);
gboolean ags_accessible_window_export_to_audio_file(AgsAccessibleWindow *accessible_window);
gboolean ags_accessible_window_quit(AgsAccessibleWindow *accessible_window);

gboolean ags_accessible_window_add_panel(AgsAccessibleWindow *accessible_window);
gboolean ags_accessible_window_add_mixer(AgsAccessibleWindow *accessible_window);
gboolean ags_accessible_window_add_drum(AgsAccessibleWindow *accessible_window);
gboolean ags_accessible_window_add_matrix(AgsAccessibleWindow *accessible_window);
gboolean ags_accessible_window_add_synth(AgsAccessibleWindow *accessible_window);
gboolean ags_accessible_window_add_ffplayer(AgsAccessibleWindow *accessible_window);
gboolean ags_accessible_window_remove_machine(AgsAccessibleWindow *accessible_window);
gboolean ags_accessible_window_show_preferences(AgsAccessibleWindow *accessible_window);

gboolean ags_accessible_window_show_about_dialog(AgsAccessibleWindow *accessible_window);

#endif /*__AGS_ACCESSIBLE_WINDOW_H__*/
