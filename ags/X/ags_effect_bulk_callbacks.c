/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015,2017 Joël Krähemann
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

#include <ags/X/ags_effect_bulk_callbacks.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/plugin/ags_base_plugin.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_port.h>

#include <ags/widget/ags_led.h>
#include <ags/widget/ags_vindicator.h>
#include <ags/widget/ags_hindicator.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_bulk_member.h>
#include <ags/X/ags_plugin_browser.h>

void
ags_effect_bulk_add_callback(GtkWidget *button,
			     AgsEffectBulk *effect_bulk)
{
  gtk_widget_show_all((GtkWidget *) effect_bulk->plugin_browser);
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

  bulk_member = gtk_container_get_children((GtkContainer *) effect_bulk->bulk_member);
  
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
      filename = ags_plugin_browser_get_plugin_filename((AgsPluginBrowser *) effect_bulk->plugin_browser);
      effect = ags_plugin_browser_get_plugin_effect((AgsPluginBrowser *) effect_bulk->plugin_browser);

      ags_effect_bulk_plugin_browser_response_create_entry();
	
      /* add effect */
      ags_effect_bulk_add_effect(effect_bulk,
				 NULL,
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

void
ags_effect_bulk_output_port_run_post_callback(AgsRecall *recall,
					      AgsEffectBulk *effect_bulk)
{
  GtkWidget *child;

  AgsChannel *channel, *start_channel;

  AgsMutexManager *mutex_manager;

  GList *list, *list_start;
  GList *port, *port_start;
  
  //  guint input_lines;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  /* lock gdk threads */
  gdk_threads_enter();

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  list_start = 
    list = gtk_container_get_children((GtkContainer *) effect_bulk->bulk_member);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) effect_bulk->audio);
      
  pthread_mutex_unlock(application_mutex);

  /* get start channel */
  pthread_mutex_lock(audio_mutex);

  start_channel = effect_bulk->audio->input;
  //  input_lines = effect_bulk->audio->input_lines;
      
  pthread_mutex_unlock(audio_mutex);

  /* check members */
  while(list != NULL){
    if(AGS_IS_BULK_MEMBER(list->data) &&
       (AGS_BULK_MEMBER(list->data)->widget_type == AGS_TYPE_VINDICATOR ||
	AGS_BULK_MEMBER(list->data)->widget_type == AGS_TYPE_HINDICATOR ||
	AGS_BULK_MEMBER(list->data)->widget_type == AGS_TYPE_LED)){
      GtkAdjustment *adjustment;

      gdouble average_peak;
      
      child = GTK_BIN(list->data)->child;
      channel = start_channel;
      
      while(channel != NULL){	
	/* get channel mutex */
	pthread_mutex_lock(application_mutex);
	
	channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) channel);
	
	pthread_mutex_unlock(application_mutex);

	/* copy port list */
	pthread_mutex_lock(channel_mutex);
      
	port_start = g_list_copy(recall->port);

	pthread_mutex_unlock(channel_mutex);

	/* get display value */
	port = port_start;

	average_peak = 0.0;
	
	while(port != NULL){
	  gdouble lower, upper;
	  gdouble range;
	  gdouble peak;
	  
	  GValue value = {0,};
	  
	  /* check if output port and specifier matches */
	  pthread_mutex_lock(AGS_PORT(port->data)->mutex);

	  if((AGS_PORT_IS_OUTPUT & (AGS_PORT(port->data)->flags)) == 0 ||
	     g_ascii_strcasecmp(AGS_PORT(port->data)->specifier,
				AGS_BULK_MEMBER(list->data)->specifier)){
	    pthread_mutex_unlock(AGS_PORT(port->data)->mutex);
	    
	    port = port->next;

	    continue;
	  }
	  
	  lower = g_value_get_float(AGS_PORT_DESCRIPTOR(AGS_PORT(port->data)->port_descriptor)->lower_value);
	  upper = g_value_get_float(AGS_PORT_DESCRIPTOR(AGS_PORT(port->data)->port_descriptor)->upper_value);
	  
	  pthread_mutex_unlock(AGS_PORT(port->data)->mutex);

	  /*  */
	  g_value_init(&value, G_TYPE_DOUBLE);
	  ags_port_safe_read(port->data,
			     &value);
	  
	  peak = g_value_get_double(&value);
	  g_value_unset(&value);

	  range = upper - lower;

	  if(range == 0.0 ||
	     AGS_PORT(port->data)->port_value_type == G_TYPE_BOOLEAN){
	    if(peak != 0.0){
	      average_peak = 10.0;
	      break;
	    }
	  }else{
	    average_peak += ((1.0 / range * peak) * 10.0);
	  }
	  
	  port = port->next;
	}

	g_list_free(port_start);
	  
	/* iterate */
	pthread_mutex_lock(channel_mutex);

	channel = channel->next;
	
	pthread_mutex_unlock(channel_mutex);
      }
      
      /* apply */
      if(AGS_IS_LED(child)){
	ags_led_set_active(child);
      }else{
	g_object_get(child,
		     "adjustment\0", &adjustment,
		     NULL);
	
	gtk_adjustment_set_value(adjustment,
				 average_peak);
      }
    }
    
    list = list->next;
  }

  g_list_free(list_start);

  /* unlock gdk threads */
  gdk_threads_leave();
}
