/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/app/ags_line_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_line_member.h>

void
ags_line_update_ui_callback(GObject *ui_provider,
			    AgsLine *line)
{
  GList *start_queued_refresh, *queued_refresh;

  queued_refresh = 
    start_queued_refresh = g_list_copy(line->queued_refresh);

  while(queued_refresh != NULL){
    AgsLineMember *line_member;
    
    GtkWidget *child_widget;
  
    GtkAdjustment *adjustment;

    AgsPort *current;
	
    AgsPluginPort *plugin_port;
	
    gdouble average_peak;
    gdouble lower, upper;
    gdouble range;
    gdouble peak;
    gboolean success;
	
    GValue value = {0,};

    GRecMutex *port_mutex;
    GRecMutex *plugin_port_mutex;

    child_widget = queued_refresh->data;

    line_member = (AgsLineMember *) gtk_widget_get_ancestor(child_widget,
							    AGS_TYPE_LINE_MEMBER);

    if(line_member == NULL){
      queued_refresh = queued_refresh->next;

      continue;
    }
    	
    average_peak = 0.0;
      
    /* play port */
    current = line_member->port;

    if(current == NULL){
      queued_refresh = queued_refresh->next;

      continue;
    }
	
    /* check if output port and specifier matches */
    if(!ags_port_test_flags(current, AGS_PORT_IS_OUTPUT)){
      queued_refresh = queued_refresh->next;

      continue;
    }

    plugin_port = NULL;
	
    g_object_get(current,
		 "plugin-port", &plugin_port,
		 NULL);
	
    if(plugin_port == NULL){
      queued_refresh = queued_refresh->next;

      continue;
    }

    /* get port mutex */
    port_mutex = AGS_PORT_GET_OBJ_MUTEX(current);

    /* match specifier */
    g_rec_mutex_lock(port_mutex);

    success = (!g_ascii_strcasecmp(current->specifier,
				   line_member->specifier)) ? TRUE: FALSE;
	
    g_rec_mutex_unlock(port_mutex);

    if(!success){
      queued_refresh = queued_refresh->next;

      g_object_unref(plugin_port);
	
      continue;
    }

    /* get plugin port mutex */
    plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(plugin_port);
	
    /* lower and upper */
    g_rec_mutex_lock(plugin_port_mutex);
	
    lower = (gdouble) g_value_get_float(plugin_port->lower_value);
    upper = (gdouble) g_value_get_float(plugin_port->upper_value);
      
    g_rec_mutex_unlock(plugin_port_mutex);
	
    /* get range */
    if(line_member->conversion != NULL){
      lower = ags_conversion_convert(line_member->conversion,
				     lower,
				     TRUE);

      upper = ags_conversion_convert(line_member->conversion,
				     upper,
				     TRUE);
    }
      
    range = upper - lower;
      
    /* play port - read value */
    g_value_init(&value, G_TYPE_FLOAT);
    ags_port_safe_read(current,
		       &value);
      
    peak = (gdouble) g_value_get_float(&value);
    g_value_unset(&value);

    if(line_member->conversion != NULL){
      peak = ags_conversion_convert(line_member->conversion,
				    peak,
				    TRUE);
    }
      
    /* calculate peak */
    if(range == 0.0 ||
       current->port_value_type == G_TYPE_BOOLEAN){
      if(peak != 0.0){
	average_peak = 1.0;
      }
    }else{
      average_peak += peak;
    }

    /* recall port */
    current = line_member->recall_port;

    /* recall port - read value */
    g_value_init(&value, G_TYPE_FLOAT);
    ags_port_safe_read(current,
		       &value);
      
    peak = (gdouble) g_value_get_float(&value);
    g_value_unset(&value);

    if(line_member->conversion != NULL){
      peak = ags_conversion_convert(line_member->conversion,
				    peak,
				    TRUE);
    }

    /* calculate peak */
    if(range == 0.0 ||
       current->port_value_type == G_TYPE_BOOLEAN){
      if(peak != 0.0){
	average_peak = 1.0;
      }
    }else{
      average_peak += peak;
    }
      
    /* apply */
    if(AGS_IS_LED(child_widget)){
      if(average_peak != 0.0){
	ags_led_set_active(AGS_LED(child_widget),
			   TRUE);
      }
    }else{
      adjustment = NULL;
	  
      g_object_get(child_widget,
		   "adjustment", &adjustment,
		   NULL);

      //	  g_message("%f", average_peak);
	  
      gtk_adjustment_set_value(adjustment,
			       10.0 * average_peak);
    }

    g_object_unref(plugin_port);
    
    /* iterate */
    queued_refresh = queued_refresh->next;
  }

  g_list_free(start_queued_refresh);
}

void
ags_line_check_message_callback(GObject *application_context, AgsLine *line)
{
  ags_line_check_message(line);
}

void
ags_line_group_callback(GObject *gobject,
			GParamSpec *pspec,
			AgsLine *line)
{
  AgsPad *pad;
  AgsLine *current;

  GList *start_list, *list;

  pad = (AgsPad *) line->parent_pad;

  list =
    start_list = ags_pad_get_line(pad);

  if(gtk_toggle_button_get_active(line->group)){
    ags_line_group_changed(line);

    while(list != NULL){
      current = AGS_LINE(list->data);

      if(!gtk_toggle_button_get_active(current->group)){
	g_list_free(start_list);

	return;
      }

      list = list->next;
    }

    gtk_toggle_button_set_active(pad->group, TRUE);
  }else{
    if(g_list_length(list) > 1){
      if(gtk_toggle_button_get_active(pad->group)){
	gtk_toggle_button_set_active(pad->group, FALSE);
      }

      while(list != NULL){
	current = AGS_LINE(list->data);

	if(gtk_toggle_button_get_active(current->group)){
	  ags_line_group_changed(line);
	  g_list_free(start_list);

	  return;
	}

	list = list->next;
      } 
    }

    gtk_toggle_button_set_active(line->group, TRUE);
  }

  g_list_free(start_list);
}

void
ags_line_stop_callback(AgsLine *line,
		       GList *recall_id, gint sound_scope,
		       gpointer data)
{
  AgsPad *pad;
  
  gboolean reset_active;

  pad = AGS_PAD(line->parent_pad);
  
  if((AGS_PAD_BLOCK_STOP & (pad->flags)) != 0){
    return;
  }
  
  pad->flags |= AGS_PAD_BLOCK_STOP;

  /* play button - check reset active */
  reset_active = (sound_scope == AGS_SOUND_SCOPE_PLAYBACK) ? TRUE: FALSE;
  
  if(reset_active){
    gtk_toggle_button_set_active(pad->play, FALSE);
  }
  
  pad->flags &= (~AGS_PAD_BLOCK_STOP);
}
