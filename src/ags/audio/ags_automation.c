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

#include <ags/audio/ags_automation.h>

#include <ags/object/ags_connectable.h>

#include <ags/object/ags_tactable.h>
#include <ags/object/ags_portlet.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_timestamp.h>

#include <stdlib.h>
#include <errno.h>

void ags_automation_class_init(AgsAutomationClass *automation);
void ags_automation_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_automation_portlet_interface_init(AgsPortletInterface *portlet);
void ags_automation_init(AgsAutomation *automation);
void ags_automation_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_automation_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_automation_connect(AgsConnectable *connectable);
void ags_automation_disconnect(AgsConnectable *connectable);
void ags_automation_finalize(GObject *object);

void ags_automation_set_port(AgsPortlet *portlet, AgsPort *port);
AgsPort* ags_automation_get_port(AgsPortlet *portlet);
GList* ags_automation_list_safe_properties(AgsPortlet *portlet);
void ags_automation_safe_set_property(AgsPortlet *portlet, gchar *property_name, GValue *value);
void ags_automation_safe_get_property(AgsPortlet *portlet, gchar *property_name, GValue *value);

/**
 * SECTION:ags_automation
 * @short_description: Automation class supporting selection and clipboard.
 * @title: AgsAutomation
 * @section_id:
 * @include: ags/audio/ags_automation.h
 *
 * #AgsAutomation acts as a container of #AgsAcceleration.
 */

#define AGS_AUTOMATION_CLIPBOARD_VERSION "0.4.3\0"
#define AGS_AUTOMATION_CLIPBOARD_TYPE "AgsAutomationClipboardXml\0"
#define AGS_AUTOMATION_CLIPBOARD_FORMAT "AgsAutomationNativePiano\0"

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_LINE,
  PROP_CHANNEL_TYPE,
  PROP_CONTROL_NAME,
  PROP_PORT,
  PROP_ACCELERATION,
  PROP_CURRENT_ACCELERATIONS,
  PROP_NEXT_ACCELERATIONS,
};

static gpointer ags_automation_parent_class = NULL;

GType
ags_automation_get_type()
{
  static GType ags_type_automation = 0;

  if(!ags_type_automation){
    static const GTypeInfo ags_automation_info = {
      sizeof(AgsAutomationClass),
      NULL,
      NULL,
      (GClassInitFunc) ags_automation_class_init,
      NULL,
      NULL,
      sizeof(AgsAutomation),
      0,
      (GInstanceInitFunc) ags_automation_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_automation_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_portlet_interface_info = {
      (GInterfaceInitFunc) ags_automation_portlet_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_automation = g_type_register_static(G_TYPE_OBJECT,
						 "AgsAutomation\0",
						 &ags_automation_info,
						 0);

    g_type_add_interface_static(ags_type_automation,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_automation,
				AGS_TYPE_PORTLET,
				&ags_portlet_interface_info);
  }

  return(ags_type_automation);
}

void 
ags_automation_class_init(AgsAutomationClass *automation)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_automation_parent_class = g_type_class_peek_parent(automation);

  gobject = (GObjectClass *) automation;

  gobject->set_property = ags_automation_set_property;
  gobject->get_property = ags_automation_get_property;

  gobject->finalize = ags_automation_finalize;

  /* properties */
  /**
   * AgsAutomation:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_object("audio\0",
				   "audio of automation\0",
				   "The audio of automation\0",
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);


  /**
   * AgsAutomation:line:
   *
   * The effect's line.
   * 
   * Since: 0.4.3
   */
  param_spec =  g_param_spec_ulong("line\0",
				   "line of effect\0",
				   "The numerical line of effect\0",
				   0,
				   65535,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LINE,
				  param_spec);

  /**
   * AgsAutomation:port:
   *
   * The assigned #AgsPort
   * 
   * Since: 0.4.2
   */
  param_spec = g_param_spec_object("port\0",
				   "port of automation\0",
				   "The port of automation\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT,
				  param_spec);

  /**
   * AgsAutomation:current-accelerations:
   *
   * Offset of current position.
   * 
   * Since: 0.4.2
   */
  param_spec = g_param_spec_pointer("current-accelerations\0",
				    "current accelerations for offset\0",
				    "The current accelerations for offset\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CURRENT_ACCELERATIONS,
				  param_spec);

  /**
   * AgsAutomation:next-accelerations:
   *
   * Offset of next position.
   * 
   * Since: 0.4.2
   */
  param_spec = g_param_spec_pointer("next-accelerations\0",
				    "next accelerations for offset\0",
				    "The next accelerations for offset\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NEXT_ACCELERATIONS,
				  param_spec);
}

void
ags_automation_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_automation_connect;
  connectable->disconnect = ags_automation_disconnect;
}

void
ags_automation_portlet_interface_init(AgsPortletInterface *portlet)
{
  portlet->set_port = ags_automation_set_port;
  portlet->get_port = ags_automation_get_port;
  portlet->list_safe_properties = ags_automation_list_safe_properties;
  portlet->safe_set_property = ags_automation_safe_set_property;
  portlet->safe_get_property = ags_automation_safe_get_property;
}

void
ags_automation_init(AgsAutomation *automation)
{
  automation->flags = 0;

  automation->timestamp = NULL;

  automation->audio = NULL;
  automation->line = 0;
  automation->channel_type = G_TYPE_NONE;
  automation->control_name = NULL;

  automation->steps = 1;
  automation->upper = 1.0;
  automation->lower = 0.0;

  automation->acceleration = NULL;

  automation->start_loop = 0.0;
  automation->end_loop = 0.0;
  automation->offset = 0.0;

  automation->selection = NULL;

  automation->port = NULL;

  automation->current_accelerations = NULL;
  automation->next_accelerations = NULL;
}

void
ags_automation_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_automation_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_automation_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsAutomation *automation;

  automation = AGS_AUTOMATION(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(automation->audio == audio){
	return;
      }

      if(automation->audio != NULL){
	g_object_unref(automation->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      automation->audio = audio;
    }
    break;
  case PROP_LINE:
    {
      guint line;

      line = g_value_get_uint(value);

      automation->line = line;
    }
    break;
  case PROP_CHANNEL_TYPE:
    {
      GType channel_type;

      channel_type = (GType) g_value_get_ulong(value);

      automation->channel_type = channel_type;
    }
    break;
  case PROP_CONTROL_NAME:
    {
      gchar *control_name;

      control_name = g_value_get_string(value);

      if(automation->control_name != NULL){
	g_free(automation->control_name);
      }

      automation->control_name = g_strdup(control_name);
    }
    break;
  case PROP_PORT:
    {
      AgsPort *port;

      port = g_value_get_object(value);

      if(port == automation->port){
	return;
      }

      if(automation->port != NULL){
	g_object_unref(automation->port);
      }

      if(port != NULL){
	g_object_ref(port);
      }

      automation->port = port;
    }
    break;
  case PROP_ACCELERATION:
    {
      AgsAcceleration *acceleration;

      acceleration = (AgsAcceleration *) g_value_get_object(value);

      if(acceleration == NULL ||
	 g_list_find(automation->acceleration, acceleration) != NULL){
	return;
      }

      ags_automation_add_acceleration(automation,
				      acceleration,
				      FALSE);
    }
    break;
  case PROP_CURRENT_ACCELERATIONS:
    {
      GList *current_accelerations;

      current_accelerations = g_value_get_pointer(value);

      if(automation->current_accelerations == current_accelerations){
	return;
      }

      automation->current_accelerations = current_accelerations;
    }
    break;
  case PROP_NEXT_ACCELERATIONS:
    {
      GList *next_accelerations;

      next_accelerations = g_value_get_pointer(value);

      if(automation->next_accelerations == next_accelerations){
	return;
      }

      automation->next_accelerations = next_accelerations;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_automation_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsAutomation *automation;

  automation = AGS_AUTOMATION(gobject);

  switch(prop_id){
  case PROP_PORT:
    {
      g_value_set_object(value, automation->port);
    }
    break;
  case PROP_CURRENT_ACCELERATIONS:
    {
      g_value_set_pointer(value, automation->current_accelerations);
    }
    break;
  case PROP_NEXT_ACCELERATIONS:
    {
      g_value_set_pointer(value, automation->next_accelerations);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_automation_finalize(GObject *gobject)
{
  AgsAutomation *automation;

  automation = AGS_AUTOMATION(gobject);
  
  if(automation->audio != NULL){
    g_object_unref(automation->audio);
  }
  
  g_list_free_full(automation->acceleration,
		   g_object_unref);

  G_OBJECT_CLASS(ags_automation_parent_class)->finalize(gobject);
}

void
ags_automation_set_port(AgsPortlet *portlet, AgsPort *port)
{
  g_object_set(G_OBJECT(portlet),
	       "port\0", port,
	       NULL);
}

AgsPort*
ags_automation_get_port(AgsPortlet *portlet)
{
  AgsPort *port;

  g_object_get(G_OBJECT(portlet),
	       "port\0", &port,
	       NULL);

  return(port);
}

GList*
ags_automation_list_safe_properties(AgsPortlet *portlet)
{
  //TODO:JK: implement me

  return(NULL);
}

void
ags_automation_safe_set_property(AgsPortlet *portlet, gchar *property_name, GValue *value)
{
  //TODO:JK: add check for safe property

  g_object_set_property(G_OBJECT(portlet),
			property_name, value);
}

void
ags_automation_safe_get_property(AgsPortlet *portlet, gchar *property_name, GValue *value)
{
  //TODO:JK: add check for safe property

  g_object_get_property(G_OBJECT(portlet),
			property_name, value);
}

/**
 * ags_automation_find_near_timestamp:
 * @automation: a #GList containing #AgsAutomation
 * @line: the matching audio channel
 * @timestamp: the matching timestamp
 *
 * Retrieve appropriate automation for timestamp.
 *
 * Returns: Next match.
 *
 * Since: 0.4.3
 */
GList*
ags_automation_find_near_timestamp(GList *automation, guint line,
				   GObject *gobject)
{
  AgsTimestamp *timestamp, *current_timestamp;

  if(gobject == NULL){
    return(NULL);
  }
  
  timestamp = AGS_TIMESTAMP(gobject);

  while(automation != NULL){
    if(AGS_AUTOMATION(automation->data)->line != line){
      automation = automation->next;
      continue;
    }

    current_timestamp = AGS_AUTOMATION(automation->data)->timestamp;

    if((AGS_TIMESTAMP_UNIX & (timestamp->flags)) != 0){
      if((AGS_TIMESTAMP_UNIX & (current_timestamp->flags)) != 0){
	if(current_timestamp->timer.unix_time.time_val >= timestamp->timer.unix_time.time_val &&
	   current_timestamp->timer.unix_time.time_val < timestamp->timer.unix_time.time_val + AGS_AUTOMATION_DEFAULT_DURATION){
	  return(automation);
	}
      }
    }

    automation = automation->next;
  }

  return(NULL);
}

/**
 * ags_automation_add_acceleration:
 * @automation: an #AgsAutomation
 * @acceleration: the #AgsAcceleration to add
 * @use_selection_list: if %TRUE add to selection, else to default automation
 *
 * Adds a acceleration to automation.
 *
 * Since: 0.4.3
 */
void
ags_automation_add_acceleration(AgsAutomation *automation,
				AgsAcceleration *acceleration,
				gboolean use_selection_list)
{
  //TODO:JK: implement me
}

/**
 * ags_automation_remove_acceleration_at_position:
 * @automation: an #AgsAutomation
 * @x: offset
 * @y: acceleration
 *
 * Removes one #AgsAcceleration of automation.
 *
 * Returns: %TRUE if successfully removed acceleration.
 *
 * Since: 0.4.3
 */
gboolean
ags_automation_remove_acceleration_at_position(AgsAutomation *automation,
					       guint x, guint y)
{
  //TODO:JK: implement me
}

/**
 * ags_automation_get_selection:
 * @automation: the #AgsAutomation
 *
 * Retrieve selection.
 *
 * Returns: the selection.
 *
 * Since: 0.4.3
 */
GList*
ags_automation_get_selection(AgsAutomation *automation)
{
  return(automation->selection);
}

/**
 * ags_automation_is_acceleration_selected:
 * @automation: the #AgsAutomation
 * @acceleration: the #AgsAcceleration to check for
 *
 * Check selection for acceleration.
 *
 * Returns: %TRUE if selected
 *
 * Since: 0.4.3
 */
gboolean
ags_automation_is_acceleration_selected(AgsAutomation *automation, AgsAcceleration *acceleration)
{
  //TODO:JK: implement me
  
  return(FALSE);
}

/**
 * ags_automation_find_point:
 * @automation: an #AgsAutomation
 * @x: offset
 * @y: acceleration
 * @use_selection_list: if %TRUE selection is searched
 *
 * Find acceleration by offset and tone.
 *
 * Returns: the matching acceleration.
 *
 * Since: 0.4.3
 */ 
AgsAcceleration*
ags_automation_find_point(AgsAutomation *automation,
			  guint x, guint y,
			  gboolean use_selection_list)
{
  //TODO:JK: implement me
  
  return(NULL);
}

/**
 * ags_automation_find_region:
 * @automation: an #AgsAutomation
 * @x0: start offset
 * @y0: start tone
 * @x1: end offset
 * @y1: end tone
 * @use_selection:_list if %TRUE selection is searched
 *
 * Find acceleration by offset and tone region.
 *
 * Returns: the matching acceleration as #GList.
 *
 * Since: 0.4.3
 */
GList*
ags_automation_find_region(AgsAutomation *automation,
			   guint x0, guint y0,
			   guint x1, guint y1,
			   gboolean use_selection_list)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_automation_free_selection:
 * @automation: an #AgsAutomation
 *
 * Clear selection.
 *
 * Since: 0.4.3
 */
void
ags_automation_free_selection(AgsAutomation *automation)
{
  AgsAcceleration *acceleration;
  GList *list;

  list = automation->selection;
  
  while(list != NULL){
    acceleration = AGS_ACCELERATION(list->data);
    acceleration->flags &= (~AGS_ACCELERATION_IS_SELECTED);
    g_object_unref(G_OBJECT(acceleration));
    
    list = list->next;
  }

  list = automation->selection;
  automation->selection = NULL;
  g_list_free(list);
}

/**
 * ags_automation_add_point_to_selection:
 * @automation: an #AgsAutomation
 * @x: offset
 * @y: tone
 * @replace_current_selection: if %TRUE selection is replaced
 *
 * Select acceleration at position.
 *
 * Since: 0.4.3
 */ 
void
ags_automation_add_point_to_selection(AgsAutomation *automation,
				      guint x, guint y,
				      gboolean replace_current_selection)
{
  AgsAcceleration *acceleration;

  acceleration = ags_automation_find_point(automation,
					   x, y,
					   FALSE);

  if(acceleration == NULL){
    /* there is nothing to be selected */
    if(replace_current_selection){
      ags_automation_free_selection(automation);
    }
  }else{
    /* add to or replace selection */
    acceleration->flags |= AGS_ACCELERATION_IS_SELECTED;
    g_object_ref(acceleration);

    if(replace_current_selection){
      GList *list;

      list = g_list_alloc();
      list->data = acceleration;
      
      ags_automation_free_selection(automation);
      automation->selection = list;
    }else{
      if(!ags_automation_is_acceleration_selected(automation, acceleration)){
	ags_automation_add_acceleration(automation, acceleration, TRUE);
      }
    }
  }
}

/**
 * ags_automation_remove_point_from_selection:
 * @automation: an #AgsAutomation
 * @x: offset
 * @y: tone
 *
 * Remove acceleration at position of selection.
 *
 * Since: 0.4.3
 */ 
void
ags_automation_remove_point_from_selection(AgsAutomation *automation,
					   guint x, guint y)
{
  AgsAcceleration *acceleration;

  acceleration = ags_automation_find_point(automation,
					   x, y,
					   FALSE);

  if(acceleration != NULL){
    acceleration->flags &= (~AGS_ACCELERATION_IS_SELECTED);

    /* remove acceleration from selection */
    automation->selection = g_list_remove(automation->selection, acceleration);

    g_object_unref(acceleration);
  }
}

void
ags_automation_add_region_to_selection(AgsAutomation *automation,
				       guint x0, guint y0,
				       guint x1, guint y1,
				       gboolean replace_current_selection)
{
  AgsAcceleration *acceleration;
  GList *region, *list;

  region = ags_automation_find_region(automation,
				      x0, y0,
				      x1, y1,
				      FALSE);

  if(replace_current_selection){
    ags_automation_free_selection(automation);

    list = region;

    while(list != NULL){
      AGS_ACCELERATION(list->data)->flags |= AGS_ACCELERATION_IS_SELECTED;
      g_object_ref(G_OBJECT(list->data));

      list = list->next;
    }

    automation->selection = region;
  }else{
    while(region != NULL){
      acceleration = AGS_ACCELERATION(region->data);

      if(!ags_automation_is_acceleration_selected(automation, acceleration)){
	acceleration->flags |= AGS_ACCELERATION_IS_SELECTED;
	g_object_ref(G_OBJECT(acceleration));
	ags_automation_add_acceleration(automation,
					acceleration,
					TRUE);
      }
      
      region = region->next;
    }
    
    g_list_free(region);
  }
}

/**
 * ags_automation_remove_region_from_selection:
 * @automation: an #AgsAutomation
 * @x0: start offset
 * @y0: start tone
 * @x1: end offset
 * @y1: end tone
 *
 * Remove acceleration within region of selection.
 *
 * Since: 0.4.3
 */ 
void
ags_automation_remove_region_from_selection(AgsAutomation *automation,
					    guint x0, guint y0,
					    guint x1, guint y1)
{
  AgsAcceleration *acceleration;
  GList *region;

  region = ags_automation_find_region(automation,
				      x0, y0,
				      x1, y1,
				      TRUE);

  while(region != NULL){
    acceleration = AGS_ACCELERATION(region->data);
    acceleration->flags &= (~AGS_ACCELERATION_IS_SELECTED);

    automation->selection = g_list_remove(automation->selection, acceleration);
    g_object_unref(G_OBJECT(acceleration));

    region = region->next;
  }

  g_list_free(region);
}

/**
 * ags_automation_copy_selection:
 * @automation: an #AgsAutomation
 *
 * Copy selection to clipboard.
 *
 * Returns: the selection as XML.
 *
 * Since: 0.4.3
 */
xmlNode*
ags_automation_copy_selection(AgsAutomation *automation)
{
  AgsAcceleration *acceleration;
  xmlNode *automation_node, *current_acceleration;
  GList *selection;
  guint x_boundary, y_boundary;

  selection = automation->selection;

  /* create root node */
  automation_node = xmlNewNode(NULL, BAD_CAST "automation\0");

  xmlNewProp(automation_node, BAD_CAST "program\0", BAD_CAST "ags\0");
  xmlNewProp(automation_node, BAD_CAST "type\0", BAD_CAST AGS_AUTOMATION_CLIPBOARD_TYPE);
  xmlNewProp(automation_node, BAD_CAST "version\0", BAD_CAST AGS_AUTOMATION_CLIPBOARD_VERSION);
  xmlNewProp(automation_node, BAD_CAST "format\0", BAD_CAST AGS_AUTOMATION_CLIPBOARD_FORMAT);

  //TODO:JK: implement me
  
  return(automation_node);
}

/**
 * ags_automation_cut_selection:
 * @automation: an #AgsAutomation
 *
 * Cut selection to clipboard.
 *
 * Returns: the selection as XML.
 *
 * Since: 0.4.3
 */
xmlNode*
ags_automation_cut_selection(AgsAutomation *automation)
{
  xmlNode *automation_node;
  GList *selection, *acceleration;
  
  automation_node = ags_automation_copy_selection(automation);

  selection = automation->selection;
  acceleration = automation->acceleration;

  while(selection != NULL){
    acceleration = g_list_find(acceleration, selection->data);

    if(acceleration->prev == NULL){
      automation->acceleration = g_list_remove_link(acceleration, acceleration);
      acceleration = automation->acceleration;
    }else{
      GList *next_acceleration;

      next_acceleration = acceleration->next;
      acceleration->prev->next = next_acceleration;

      if(next_acceleration != NULL)
	next_acceleration->prev = acceleration->prev;

      g_list_free1(acceleration);

      acceleration = next_acceleration;
    }

    AGS_ACCELERATION(selection->data)->flags &= (~AGS_ACCELERATION_IS_SELECTED);
    g_object_unref(selection->data);

    selection = selection->next;
  }

  ags_automation_free_selection(automation);

  return(automation_node);
}

void
ags_automation_insert_from_clipboard(AgsAutomation *automation,
				     xmlNode *content,
				     gboolean reset_x_offset, guint x_offset,
				     gboolean reset_y_offset, guint y_offset)
{
  //TODO:JK: implement me
}

GList*
ags_automation_get_current(AgsAutomation *automation)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_automation_new:
 * @audio: an #AgsAudio
 * @line: the line to apply
 * @channel_type: the channel type
 * @control_name: the control name
 *
 * Creates a #AgsAutomation.
 *
 * Returns: a new #AgsAutomation
 *
 * Since: 0.4.2
 */
AgsAutomation*
ags_automation_new(GObject *audio,
		   guint line,
		   GType channel_type,
		   gchar *control_name)
{
  AgsAutomation *automation;

  automation = (AgsAutomation *) g_object_new(AGS_TYPE_AUTOMATION,
					      "audio\0", audio,
					      "line\0", line,
					      "channel-type\0", channel_type,
					      "control-name\0", control_name,
					      NULL);

  return(automation);
}
