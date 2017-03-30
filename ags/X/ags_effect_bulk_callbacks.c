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

#include <ags/object/ags_application_context.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/plugin/ags_base_plugin.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_port.h>

#include <ags/widget/ags_led.h>
#include <ags/widget/ags_vindicator.h>
#include <ags/widget/ags_hindicator.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_bulk_member.h>
#include <ags/X/ags_plugin_browser.h>

#include <ags/X/thread/ags_gui_thread.h>

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
  AgsWindow *window;
  
  AgsGuiThread *gui_thread;
  
  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;

  gdk_threads_enter();

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) effect_bulk);

  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;
  
  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  gui_thread = (AgsGuiThread *) ags_thread_find_type((AgsThread *) main_loop,
						      AGS_TYPE_GUI_THREAD);

  /*  */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  ags_effect_bulk_resize_audio_channels(effect_bulk,
					audio_channels,
					audio_channels_old);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  gdk_threads_leave();
}

void
ags_effect_bulk_set_pads_callback(AgsAudio *audio,
				  GType channel_type,
				  guint pads,
				  guint pads_old,
				  AgsEffectBulk *effect_bulk)
{
  AgsWindow *window;
  
  AgsGuiThread *gui_thread;
  
  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;

  gdk_threads_enter();

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) effect_bulk);

  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;
  
  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  gui_thread = (AgsGuiThread *) ags_thread_find_type((AgsThread *) main_loop,
						      AGS_TYPE_GUI_THREAD);

  /*  */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  if(channel_type == effect_bulk->channel_type){
    ags_effect_bulk_resize_pads(effect_bulk,
				pads,
				pads_old);    
  }

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  gdk_threads_leave();
}

void
ags_effect_bulk_output_port_run_post_callback(AgsRecall *recall,
					      AgsEffectBulk *effect_bulk)
{
  GtkWidget *child;

  GList *list, *list_start;
  GList *port, *port_start;

  /* lock gdk threads */
  gdk_threads_enter();
  
  list_start = 
    list = gtk_container_get_children((GtkContainer *) effect_bulk->table);

  /* check members */
  while(list != NULL){
    if(AGS_IS_BULK_MEMBER(list->data) &&
       (AGS_BULK_MEMBER(list->data)->widget_type == AGS_TYPE_VINDICATOR ||
	AGS_BULK_MEMBER(list->data)->widget_type == AGS_TYPE_HINDICATOR ||
	AGS_BULK_MEMBER(list->data)->widget_type == AGS_TYPE_LED)){
      AgsBulkMember *bulk_member;
      GtkAdjustment *adjustment;

      gdouble average_peak;

      bulk_member = AGS_BULK_MEMBER(list->data);
      child = GTK_BIN(bulk_member)->child;
      
      average_peak = 0.0;
      
      /* copy port list */
      port_start = g_list_concat(g_list_copy(bulk_member->bulk_port),
				 g_list_copy(bulk_member->recall_bulk_port));

      /* get display value */
      port = port_start;
	
      while(port != NULL){
	AgsPort *current;
	
	gdouble lower, upper;
	gdouble range;
	gdouble peak;
	  
	GValue value = {0,};

	current = AGS_BULK_PORT(port->data)->port;

	if(current == NULL){
	  port = port->next;
	
	  continue;
	}      
	
	/* check if output port and specifier matches */
	pthread_mutex_lock(current->mutex);

	if((AGS_PORT_IS_OUTPUT & (current->flags)) == 0 ||
	   current->port_descriptor == NULL ||
	   g_ascii_strcasecmp(current->specifier,
			      bulk_member->specifier)){
	  pthread_mutex_unlock(current->mutex);
	    
	  port = port->next;

	  continue;
	}

	/* lower and upper */
	lower = g_value_get_float(AGS_PORT_DESCRIPTOR(current->port_descriptor)->lower_value);
	upper = g_value_get_float(AGS_PORT_DESCRIPTOR(current->port_descriptor)->upper_value);
	  
	pthread_mutex_unlock(current->mutex);

	/* get range */
	if(bulk_member->conversion != NULL){
	  lower = ags_conversion_convert(bulk_member->conversion,
					 lower,
					 TRUE);
	  
	  upper = ags_conversion_convert(bulk_member->conversion,
					 upper,
					 TRUE);
	}

	range = upper - lower;

	/* port read value */
	g_value_init(&value, G_TYPE_FLOAT);
	ags_port_safe_read(current,
			   &value);
	  
	peak = g_value_get_float(&value);
	g_value_unset(&value);

	if(bulk_member->conversion != NULL){
	  peak = ags_conversion_convert(bulk_member->conversion,
					peak,
					TRUE);
	}
	
	/* calculate peak */
	if(range == 0.0 ||
	   current->port_value_type == G_TYPE_BOOLEAN){
	  if(peak != 0.0){
	    average_peak = 10.0;
	    break;
	  }
	}else{
	  average_peak += ((1.0 / (range / peak)) * 10.0);
	}

	/* iterate port */
	port = port->next;
      }

      g_list_free(port_start);
      
      /* apply */
      if(AGS_IS_LED(child)){
	if(average_peak != 0.0){
	  ags_led_set_active(child);
	}
      }else{
	g_object_get(child,
		     "adjustment\0", &adjustment,
		     NULL);
	
	gtk_adjustment_set_value(adjustment,
				 average_peak);
      }
    }

    /* iterate bulk member */
    list = list->next;
  }

  g_list_free(list_start);

  /* unlock gdk threads */
  gdk_threads_leave();
}
