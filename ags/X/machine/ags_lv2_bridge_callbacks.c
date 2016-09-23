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

#include <ags/X/machine/ags_lv2_bridge_callbacks.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_lv2.h>
#include <ags/audio/ags_port.h>

void
ags_lv2_bridge_show_gui_callback(GtkMenuItem *item, AgsLv2Bridge *lv2_bridge)
{
  gtk_widget_show_all(lv2_bridge->lv2_gui);
  gtk_widget_show_all(gtk_bin_get_child((GtkBin *) lv2_bridge->lv2_gui));
}

gboolean
ags_lv2_bridge_delete_event_callback(GtkWidget *widget, GdkEvent *event, AgsLv2Bridge *lv2_bridge)
{
  gtk_widget_hide(lv2_bridge->lv2_gui);
  
  return(TRUE);
}

void
ags_lv2_bridge_lv2ui_write_function(LV2UI_Controller controller, uint32_t port_index, uint32_t buffer_size, uint32_t port_protocol, const void *buffer)
{
  AgsLv2Bridge *lv2_bridge;
  GtkWidget *widget;

  AgsChannel *channel;
  AgsRecallLv2 *recall_lv2;
  
  GList *effect_plugin;
  GList *recall;
  GList *port;

  gchar *control_port;

  GValue value = {0,};
  
  lv2_bridge = (AgsLv2Bridge *) controller;
  
  if(lv2_bridge == NULL){
    g_warning("ags_lv2_bridge_lv2ui_write_function() - lv2_bridge == NULL\0");
    return;
  }

  channel = AGS_MACHINE(lv2_bridge)->audio->input;
 
  switch(port_protocol){
  case 0:
    {
      if(buffer_size == sizeof(float)){
	g_value_init(&value, G_TYPE_FLOAT);
	g_value_set_float(&value, * ((float *) buffer));
      }else{
	g_value_init(&value, G_TYPE_POINTER);
	g_value_set_pointer(&value, buffer);
      }
    }
    break;
  default:
    g_warning("unknown lv2 port protocol\0");
  }
    
  while(channel != NULL){
    recall = ags_recall_get_by_effect(channel->play,
				      lv2_bridge->filename, lv2_bridge->effect);

    if(recall != NULL){
      recall_lv2 = recall->data;

      port = AGS_RECALL(recall_lv2)->port;
      control_port = g_strdup_printf("%d/%d\0",
				     port_index + 1,
				     g_list_length(port));
	
      while(port != NULL){
	if(!g_ascii_strncasecmp(AGS_PORT(port->data)->control_port,
				control_port,
				strlen(control_port))){
	  ags_port_safe_write(port->data,
			      &value);
	    
	  break;
	}
	  
	port = port->next;
      }

      free(control_port);
    }

    g_list_free(recall);
    channel = channel->next;
  }
}
