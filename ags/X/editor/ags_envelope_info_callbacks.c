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

#include <ags/X/editor/ags_envelope_info_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

void
ags_envelope_info_plot_callback(GtkCellRendererToggle *cell_renderer,
				gchar *path_str,
				AgsEnvelopeInfo *envelope_info)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  GtkTreePath *path;
  
  gboolean fixed;

  model = gtk_tree_view_get_model(envelope_info->tree_view);
  path = gtk_tree_path_new_from_string(path_str);
  
  /* get toggled iter */
  gtk_tree_model_get_iter(model, &iter, path);
  gtk_tree_model_get(model, &iter,
		     AGS_ENVELOPE_INFO_COLUMN_PLOT, &fixed,
		     -1);

  /* do something with the value */
  fixed ^= 1;

  /* set new value */
  gtk_list_store_set(GTK_LIST_STORE(model), &iter,
		     AGS_ENVELOPE_INFO_COLUMN_PLOT, fixed,
		     -1);

  /* clean up */
  gtk_tree_path_free (path);

  /* plot */
  ags_envelope_info_plot(envelope_info);
}
