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

#include <ags/app/ags_plugin_browser_callbacks.h>

void
ags_plugin_browser_plugin_type_changed_callback(GtkWidget *combo_box,
						AgsPluginBrowser *plugin_browser)
{
  if(gtk_combo_box_get_active((GtkComboBox *) combo_box) == 0){
    plugin_browser->active_browser = plugin_browser->lv2_browser;
    gtk_widget_show(plugin_browser->lv2_browser);

    gtk_widget_hide(plugin_browser->ladspa_browser);
#if defined(AGS_WITH_VST3)
    gtk_widget_hide(plugin_browser->vst3_browser);
#endif
  }else if(gtk_combo_box_get_active((GtkComboBox *) combo_box) == 1){
    plugin_browser->active_browser = plugin_browser->ladspa_browser;
    gtk_widget_show(plugin_browser->ladspa_browser);

    gtk_widget_hide(plugin_browser->lv2_browser);
#if defined(AGS_WITH_VST3)
    gtk_widget_hide(plugin_browser->vst3_browser);
#endif
#if defined(AGS_WITH_VST3)
  }else if(gtk_combo_box_get_active((GtkComboBox *) combo_box) == 2){
    plugin_browser->active_browser = plugin_browser->vst3_browser;
    gtk_widget_show(plugin_browser->vst3_browser);

    gtk_widget_hide(plugin_browser->lv2_browser);
    gtk_widget_hide(plugin_browser->ladspa_browser);
#endif
  }
}

void
ags_plugin_browser_response_callback(GtkDialog *plugin_browser,
				     gint response,
				     gpointer user_data)
{  
  gtk_widget_hide((GtkWidget *) plugin_browser);
}
