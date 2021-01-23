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

#include <ags/X/ags_property_collection_editor_callbacks.h>

#include <ags/i18n.h>

void
ags_property_collection_editor_add_collection_callback(GtkButton *button,
						       AgsPropertyCollectionEditor *property_collection_editor)
{
  GtkGrid *grid;
  GtkWidget *collection;
  GtkButton *remove_collection;

  grid = (GtkGrid *) gtk_grid_new();
  gtk_box_pack_start(GTK_BOX(property_collection_editor->child),
		     GTK_WIDGET(grid),
		     FALSE, FALSE,
		     0);

  //FIXME:JK: deprecated function
  collection = (GtkWidget *) g_object_new_with_properties(property_collection_editor->child_type,
							  g_strv_length(property_collection_editor->child_strv),
							  property_collection_editor->child_strv,
							  property_collection_editor->child_value);
  g_object_set_data(G_OBJECT(grid), "AgsChild", collection);

  gtk_widget_set_valign(GTK_WIDGET(collection),
			GTK_ALIGN_FILL);
  gtk_widget_set_halign(GTK_WIDGET(collection),
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  GTK_WIDGET(collection),
		  0, 0,
		  2, 1);


  remove_collection = (GtkButton *) gtk_button_new_with_label(i18n("_Remove"));

  gtk_widget_set_valign(GTK_WIDGET(remove_collection),
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		   GTK_WIDGET(remove_collection),
		   1, 1,
		   1, 1);

  g_signal_connect_after(G_OBJECT(remove_collection), "clicked",
			 G_CALLBACK(ags_property_collection_editor_remove_collection_callback), grid);

  ags_connectable_connect(AGS_CONNECTABLE(collection));

  gtk_widget_show_all(GTK_WIDGET(grid));
}

void
ags_property_collection_editor_remove_collection_callback(GtkButton *button,
							  GtkGrid *grid)
{
  gtk_widget_destroy(GTK_WIDGET(grid));
}
