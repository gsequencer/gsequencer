/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/ags_ladspa_browser_callbacks.h>

#include <ags/lib/ags_combo_box_text.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ladspa.h>

void
ags_ladspa_browser_plugin_filename_callback(GtkComboBoxText *combo_box,
					    AgsLadspaBrowser *ladspa_browser)
{
  GtkComboBoxText *filename, *effect;
  GList *list;
  gchar *path;
  void *plugin_so;
  LADSPA_Descriptor_Function ladspa_descriptor;
  LADSPA_Descriptor *plugin_descriptor;

  list = gtk_container_get_children(GTK_CONTAINER(ladspa_browser->plugin));

  filename = GTK_COMBO_BOX(list->next->data);
  effect = GTK_COMBO_BOX(list->next->next->next->data);

  ags_combo_box_text_remove_all(effect);
  
  path = g_strdup_printf("%s/%s\0",
			 ladspa_browser->path,
			 gtk_combo_box_text_get_active_text(filename));

  plugin_so = dlopen(path,
		     RTLD_NOW);

  if(plugin_so){
    dlerror();
    ladspa_descriptor = (LADSPA_Descriptor_Function) dlsym(plugin_so,
							   "ladspa_descriptor\0");

    if(dlerror() == NULL && ladspa_descriptor){
      long index;

      /* We've successfully found a ladspa_descriptor function. Pass
	 it to the callback function. */

      for(index = 0; (plugin_descriptor = ladspa_descriptor(index)) != NULL; index++){
	gtk_combo_box_text_append_text(effect,
				       g_strdup_printf("%s:%lu\0 ", plugin_descriptor->Name, plugin_descriptor->UniqueID));
      }
    }
    
    dlclose(plugin_so);
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(list->data),
			   0);
}

void
ags_ladspa_browser_plugin_effect_callback(GtkComboBoxText *combo_box,
					  AgsLadspaBrowser *ladspa_browser)
{
  //TODO:JK: implement me
}

void
ags_ladspa_browser_preview_close_callback(GtkWidget *preview,
					  AgsLadspaBrowser *ladspa_browser)
{
  ladspa_browser->preview = NULL;
  gtk_widget_destroy(preview);
}
