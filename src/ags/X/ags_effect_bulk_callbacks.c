/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#include <ags/X/ags_effect_bulk_callbacks.h>

#include <ags/X/ags_plugin_browser.h>

void
ags_effect_bulk_add_callback(GtkWidget *button,
			     AgsEffectBulk *effect_bulk)
{
  gtk_widget_show_all(effect_bulk->plugin_browser);
}

void
ags_effect_bulk_remove_callback(GtkWidget *button,
				AgsEffectBulk *effect_bulk)
{
  GList *bulk_member;
  GList *children;
  guint nth;
  
  auto void ags_effect_bulk_editor_plugin_browser_response_destroy_entry();
  
  void ags_effect_bulk_editor_plugin_browser_response_destroy_entry(){
    /* destroy bulk member entry */
    gtk_widget_destroy(GTK_WIDGET(bulk_member->data));
  }
  
  if(button == NULL ||
     effect_bulk == NULL){
    return;
  }

  bulk_member = gtk_container_get_children(effect_bulk->bulk_member);
  
  /* iterate bulk member */
  for(nth = 0; bulk_member != NULL; nth++){
    children = gtk_container_get_children(GTK_CONTAINER(bulk_member->data));

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(children->data))){
      ags_effect_bulk_editor_plugin_browser_response_destroy_entry();
	
      /* remove effect */
      ags_effect_bulk_remove_effect(effect_bulk,
				    nth);
    }
      
    bulk_member = bulk_member->next;
  }
}

void
ags_effect_bulk_plugin_browser_response_callback(GtkDialog *dialog,
						 gint response,
						 AgsEffectBulk *effect_bulk)
{
  gchar *filename, *effect;
  
  auto void ags_effect_bulk_plugin_browser_response_create_entry();
  
  void ags_effect_bulk_plugin_browser_response_create_entry(){
    GtkHBox *hbox;
    GtkCheckButton *check_button;
    GtkLabel *label;

    /* create entry */
    hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(effect_bulk->bulk_member),
		       GTK_WIDGET(hbox),
		       FALSE, FALSE,
		       0);
      
    check_button = (GtkCheckButton *) gtk_check_button_new();
    gtk_box_pack_start(GTK_BOX(hbox),
		       GTK_WIDGET(check_button),
		       FALSE, FALSE,
		       0);

    //TODO:JK: ugly
    label = (GtkLabel *) gtk_label_new(g_strdup_printf("%s - %s\0",
						       filename,
						       effect));
    gtk_box_pack_start(GTK_BOX(hbox),
		       GTK_WIDGET(label),
		       FALSE, FALSE,
		       0);
    gtk_widget_show_all((GtkWidget *) hbox);
  }
  
  switch(response){
  case GTK_RESPONSE_ACCEPT:
    {
      /* retrieve plugin */
      filename = ags_plugin_browser_get_plugin_filename(effect_bulk->plugin_browser);
      effect = ags_plugin_browser_get_plugin_effect(effect_bulk->plugin_browser);

      ags_effect_bulk_plugin_browser_response_create_entry();
	
      /* add effect */
      ags_effect_bulk_add_effect(effect_bulk,
				 filename,
				 effect);
    }
    break;
  }
}

void
ags_effect_bulk_set_audio_channels_callback(AgsAudio *audio,
					    guint audio_channels,
					    guint audio_channels_old,
					    AgsEffectBulk *effect_bulk)
{
  ags_effect_bulk_resize_audio_channels(effect_bulk,
					audio_channels,
					audio_channels_old);
}

void
ags_effect_bulk_set_pads_callback(AgsAudio *audio,
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

