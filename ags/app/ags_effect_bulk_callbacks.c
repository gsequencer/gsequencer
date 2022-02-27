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

#include <ags/app/ags_effect_bulk_callbacks.h>

#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_bulk_member.h>
#include <ags/app/ags_plugin_browser.h>

void ags_effect_bulk_plugin_browser_response_create_entry(AgsEffectBulk *effect_bulk,
							  gchar *filename, gchar *effect);

void
ags_effect_bulk_add_callback(GtkWidget *button,
			     AgsEffectBulk *effect_bulk)
{
  gtk_widget_show((GtkWidget *) effect_bulk->plugin_browser);
}

void
ags_effect_bulk_remove_callback(GtkWidget *button,
				AgsEffectBulk *effect_bulk)
{
  GList *start_effect_bulk_entry, *effect_bulk_entry;

  guint nth;
    
  if(button == NULL ||
     effect_bulk == NULL){
    return;
  }

  effect_bulk_entry =
    start_effect_bulk_entry = ags_effect_bulk_get_bulk_member_entry(effect_bulk);
  
  /* iterate bulk member */
  for(nth = 0; effect_bulk_entry != NULL; nth++){
    if(gtk_check_button_get_active(AGS_EFFECT_BULK_ENTRY(effect_bulk_entry->data)->check_button)){
      ags_effect_bulk_remove_bulk_member_entry(effect_bulk,
					       effect_bulk_entry->data);

      g_object_run_dispose(effect_bulk_entry->data);
      g_object_unref(effect_bulk_entry->data);
      
      /* remove effect */
      ags_effect_bulk_remove_plugin(effect_bulk,
				    nth);
    }
    
    effect_bulk_entry = effect_bulk_entry->next;
  }

  g_list_free(start_effect_bulk_entry);
}

void
ags_effect_bulk_plugin_browser_response_create_entry(AgsEffectBulk *effect_bulk,
						     gchar *filename, gchar *effect)
{
  AgsEffectBulkEntry *effect_bulk_entry;
  
  gchar *str;
    
  /* create entry */
  effect_bulk_entry = ags_effect_bulk_entry_new();

  ags_effect_bulk_add_bulk_member_entry(effect_bulk,
					effect_bulk_entry);
  
  str = g_strdup_printf("%s - %s",
			filename,
			effect);

  gtk_label_set_text(effect_bulk_entry->label,
		     str);
  
  g_free(str);
    
  gtk_widget_show((GtkWidget *) effect_bulk_entry);
}

void
ags_effect_bulk_plugin_browser_response_callback(GtkDialog *dialog,
						 gint response,
						 AgsEffectBulk *effect_bulk)
{
  gchar *plugin_type;
  gchar *plugin_name;
  gchar *filename, *effect;  

  guint audio_channels;
  guint input_pads;
  
  switch(response){
  case GTK_RESPONSE_ACCEPT:
    {
      g_object_get(effect_bulk->audio,
		   "audio-channels", &audio_channels,
		   "input-pads", &input_pads,
		   NULL);
      
      plugin_type = gtk_combo_box_text_get_active_text(AGS_PLUGIN_BROWSER(effect_bulk->plugin_browser)->plugin_type);
      plugin_name = NULL;

      if(!g_ascii_strncasecmp(plugin_type,
			      "ladspa",
			      7)){
	plugin_name = "ags-fx-ladspa";
      }else if(!g_ascii_strncasecmp(plugin_type,
				    "lv2",
				    4)){
	plugin_name = "ags-fx-lv2";
#if defined(AGS_WITH_VST3)
      }else if(!g_ascii_strncasecmp(plugin_type,
				    "vst3",
				    5)){
	plugin_name = "ags-fx-vst3";
#endif
      }

      /* retrieve plugin */
      filename = ags_plugin_browser_get_plugin_filename((AgsPluginBrowser *) effect_bulk->plugin_browser);
      effect = ags_plugin_browser_get_plugin_effect((AgsPluginBrowser *) effect_bulk->plugin_browser);

      ags_effect_bulk_plugin_browser_response_create_entry(effect_bulk,
							   filename, effect);
	
      /* add effect */
      ags_effect_bulk_add_plugin(effect_bulk,
				 NULL,
				 ags_recall_container_new(), ags_recall_container_new(),
				 plugin_name,
				 filename,
				 effect,
				 0, audio_channels,
				 0, input_pads,
				 0,
				 (AGS_FX_FACTORY_ADD), 0);

      g_free(plugin_type);

      g_free(filename);
      g_free(effect);
    }
    break;
  }
}

void
ags_effect_bulk_resize_audio_channels_callback(AgsMachine *machine,
					       guint audio_channels,
					       guint audio_channels_old,
					       AgsEffectBulk *effect_bulk)
{
  ags_effect_bulk_resize_audio_channels(effect_bulk,
					audio_channels,
					audio_channels_old);
}

void
ags_effect_bulk_resize_pads_callback(AgsMachine *machine,
				     GType channel_type,
				     guint pads,
				     guint pads_old,
				     AgsEffectBulk *effect_bulk)
{
  if(channel_type == effect_bulk->channel_type){
    ags_effect_bulk_resize_pads(effect_bulk,
				pads,
				pads_old);    
  }
}
