/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/audio/ags_automation.h>

#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_port.h>

#include <ags/config.h>

#include <stdlib.h>

#include <math.h>
#include <errno.h>

#include <ags/i18n.h>

void ags_automation_class_init(AgsAutomationClass *automation);
void ags_automation_init(AgsAutomation *automation);
void ags_automation_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_automation_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_automation_dispose(GObject *gobject);
void ags_automation_finalize(GObject *gobject);
  
void ags_automation_insert_from_clipboard_version_0_4_3(AgsAutomation *automation,
							xmlNode *root_node, char *version,
							char *x_boundary, char *y_boundary,
							gboolean from_x_offset, guint x_offset,
							gboolean from_y_offset, guint y_offset,
							gboolean match_line, gboolean no_duplicates,
							guint current_line,  
							gboolean match_timestamp);

void ags_automation_insert_native_scale_from_clipboard(AgsAutomation *automation,
						       xmlNode *root_node, char *version,
						       char *x_boundary, char *y_boundary,
						       gboolean from_x_offset, guint x_offset,
						       gboolean from_y_offset, guint y_offset,
						       gboolean match_line, gboolean no_duplicates);

/**
 * SECTION:ags_automation
 * @short_description: Automation class supporting selection and clipboard
 * @title: AgsAutomation
 * @section_id:
 * @include: ags/audio/ags_automation.h
 *
 * #AgsAutomation acts as a container of #AgsAcceleration. The `timestamp` property tells the
 * engine what the first x offset of #AgsAcceleration applies.
 * 
 * You can lookup #AgsAutomation by start x offset with ags_automation_find_near_timestamp().
 * The next x offset is calculated as following:
 *
 * next_x_offset = x_offset + AGS_AUTOMATION_DEFAULT_OFFSET;
 *
 * Use ags_automation_add_acceleration() to add #AgsAcceleration to #AgsAutomation and
 * ags_automation_remove_acceleration() to remove it again.
 *
 * In order to copy or cut accelerations you select them first by calling ags_automation_add_region_to_selection().
 */

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_CHANNEL_TYPE,
  PROP_LINE,
  PROP_TIMESTAMP,
  PROP_CONTROL_NAME,
  PROP_STEPS,
  PROP_UPPER,
  PROP_LOWER,
  PROP_DEFAULT_VALUE,
  PROP_PORT,
  PROP_ACCELERATION,
};

static gpointer ags_automation_parent_class = NULL;

GType
ags_automation_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_automation = 0;

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

    ags_type_automation = g_type_register_static(G_TYPE_OBJECT,
						 "AgsAutomation",
						 &ags_automation_info,
						 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_automation);
  }

  return g_define_type_id__volatile;
}

GType
ags_automation_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_AUTOMATION_BYPASS, "AGS_AUTOMATION_BYPASS", "automation-bypass" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsAutomationFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
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

  gobject->dispose = ags_automation_dispose;
  gobject->finalize = ags_automation_finalize;

  /* properties */
  /**
   * AgsAutomation:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio of automation"),
				   i18n_pspec("The audio of automation"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsAutomation:channel-type:
   *
   * The effect's assigned channel type.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_gtype("channel-type",
				   i18n_pspec("channel type to apply"),
				   i18n_pspec("The channel type to apply"),
				   G_TYPE_NONE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL_TYPE,
				  param_spec);

  /**
   * AgsAutomation:line:
   *
   * The effect's line.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("line",
				  i18n_pspec("line of effect"),
				  i18n_pspec("The numerical line of effect"),
				  0,
				  65535,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LINE,
				  param_spec);

  /**
   * AgsAutomation:timestamp:
   *
   * The automation's timestamp.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("timestamp",
				   i18n_pspec("timestamp of automation"),
				   i18n_pspec("The timestamp of automation"),
				   AGS_TYPE_TIMESTAMP,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TIMESTAMP,
				  param_spec);
  
  /**
   * AgsAutomation:control-name:
   *
   * The effect's assigned control name.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_string("control-name",
				    i18n_pspec("control name"),
				    i18n_pspec("The control name"),
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONTROL_NAME,
				  param_spec);

  /**
   * AgsAutomation:steps:
   *
   * The effect's steps.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("steps",
				  i18n_pspec("steps of effect"),
				  i18n_pspec("The steps of effect"),
				  0,
				  G_MAXUINT32,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_STEPS,
				  param_spec);

  /**
   * AgsAutomation:upper:
   *
   * The effect's upper.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_double("upper",
				    i18n_pspec("upper of effect"),
				    i18n_pspec("The upper of effect"),
				    -1.0 * G_MAXDOUBLE,
				    G_MAXDOUBLE,
				    0.0,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_UPPER,
				  param_spec);

  /**
   * AgsAutomation:lower:
   *
   * The effect's lower.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_double("lower",
				    i18n_pspec("lower of effect"),
				    i18n_pspec("The lower of effect"),
				    -1.0 * G_MAXDOUBLE,
				    G_MAXDOUBLE,
				    0.0,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOWER,
				  param_spec);


  /**
   * AgsAutomation:default-value:
   *
   * The effect's default-value.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_double("default-value",
				    i18n_pspec("default value of effect"),
				    i18n_pspec("The default value of effect"),
				    -1.0 * G_MAXDOUBLE,
				    G_MAXDOUBLE,
				    0.0,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEFAULT_VALUE,
				  param_spec);

  /**
   * AgsAutomation:port:
   *
   * The assigned #AgsPort
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("port",
				   i18n_pspec("port of automation"),
				   i18n_pspec("The port of automation"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT,
				  param_spec);

  /**
   * AgsAutomation:acceleration: (type GList(AgsAcceleration)) (transfer full)
   *
   * The acceleration list.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("acceleration",
				    i18n_pspec("acceleration"),
				    i18n_pspec("The acceleration"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ACCELERATION,
				  param_spec);
}

void
ags_automation_init(AgsAutomation *automation)
{
  automation->flags = 0; // AGS_AUTOMATION_BYPASS

  /* add automation mutex */
  g_rec_mutex_init(&(automation->obj_mutex));
  
  /*  */
  automation->audio = NULL;
  automation->channel_type = G_TYPE_NONE;
  automation->line = 0;

  automation->timestamp = ags_timestamp_new();

  automation->timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  automation->timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  automation->timestamp->timer.ags_offset.offset = 0;

  g_object_ref(automation->timestamp);
  
  automation->control_name = NULL;

  automation->steps = 8;
  automation->upper = 1.0;
  automation->lower = 0.0;
  automation->default_value = 0.0;

  automation->source_function = NULL;

  automation->port = NULL;
  
  automation->acceleration = NULL;  
  automation->selection = NULL;
}

void
ags_automation_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsAutomation *automation;

  GRecMutex *automation_mutex;

  automation = AGS_AUTOMATION(gobject);

  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      g_rec_mutex_lock(automation_mutex);

      if(automation->audio == (GObject *) audio){      
	g_rec_mutex_unlock(automation_mutex);
	
	return;
      }

      if(automation->audio != NULL){
	g_object_unref(automation->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      automation->audio = (GObject *) audio;
      
      g_rec_mutex_unlock(automation_mutex);
    }
    break;
  case PROP_LINE:
    {
      guint line;

      line = g_value_get_uint(value);

      g_rec_mutex_lock(automation_mutex);

      automation->line = line;
      
      g_rec_mutex_unlock(automation_mutex);
    }
    break;
  case PROP_CHANNEL_TYPE:
    {
      GType channel_type;

      channel_type = (GType) g_value_get_gtype(value);

      g_rec_mutex_lock(automation_mutex);

      automation->channel_type = channel_type;
      
      g_rec_mutex_unlock(automation_mutex);
    }
    break;
  case PROP_CONTROL_NAME:
    {
      gchar *control_name;

      control_name = g_value_get_string(value);

      g_rec_mutex_lock(automation_mutex);

      if(automation->control_name != NULL){
	g_free(automation->control_name);
      }

      automation->control_name = g_strdup(control_name);
      
      g_rec_mutex_unlock(automation_mutex);
    }
    break;
  case PROP_PORT:
    {
      AgsPort *port;
      AgsPluginPort *plugin_port;
      
      port = g_value_get_object(value);

      g_rec_mutex_lock(automation_mutex);

      if(automation->port == (GObject *) port){      
	g_rec_mutex_unlock(automation_mutex);
	
	return;
      }

      if(automation->port != NULL){
	g_object_unref(automation->port);
      }

      plugin_port = NULL;
      
      if(port != NULL){
	g_object_ref(port);

	g_object_get(port,
		     "plugin-port", &plugin_port,
		     NULL);
	
	if((AGS_PORT_INFINITE_RANGE & (port->flags)) != 0){
	  automation->steps = AGS_AUTOMATION_MAXIMUM_STEPS;
	}
      }

      automation->port = (GObject *) port;
      
      g_rec_mutex_unlock(automation_mutex);
      
      if(plugin_port != NULL){
	gfloat upper, lower;
	guint steps;
	guint plugin_port_flags;
	
	GRecMutex *plugin_port_mutex;	

	/* get plugin port mutex */
	plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(plugin_port);

	/* get some fields */
	g_rec_mutex_lock(plugin_port_mutex);

	plugin_port_flags = plugin_port->flags;
	
	lower = g_value_get_float(plugin_port->lower_value);
	upper = g_value_get_float(plugin_port->upper_value);

	steps = plugin_port->scale_steps;

	g_rec_mutex_unlock(plugin_port_mutex);

	g_object_set(automation,
		     "upper", upper,
		     "lower", lower,
		     NULL);
	
	if((AGS_PLUGIN_PORT_TOGGLED & (plugin_port_flags)) != 0){
	  automation->lower = 0.0;
	  automation->upper = 1.0;
	  automation->steps = 1;
	}else if((AGS_PLUGIN_PORT_INTEGER & (plugin_port_flags)) != 0){
	  automation->steps = steps;
	}else{
	  automation->steps = AGS_AUTOMATION_DEFAULT_PRECISION;
	}

	g_object_unref(plugin_port);
      }
    }
    break;
  case PROP_TIMESTAMP:
    {
      AgsTimestamp *timestamp;

      timestamp = (AgsTimestamp *) g_value_get_object(value);

      g_rec_mutex_lock(automation_mutex);

      if(automation->timestamp == timestamp){
	g_rec_mutex_unlock(automation_mutex);
	
	return;
      }

      if(automation->timestamp != NULL){
	g_object_unref(G_OBJECT(automation->timestamp));
      }

      if(timestamp != NULL){
	g_object_ref(G_OBJECT(timestamp));
      }

      automation->timestamp = timestamp;

      g_rec_mutex_unlock(automation_mutex);
    }
    break;
  case PROP_STEPS:
    {
      guint steps;

      steps = g_value_get_uint(value);

      g_rec_mutex_lock(automation_mutex);

      automation->steps = steps;

      g_rec_mutex_unlock(automation_mutex);
    }
    break;
  case PROP_UPPER:
    {
      gdouble upper;

      upper = g_value_get_double(value);

      g_rec_mutex_lock(automation_mutex);

      automation->upper = upper;      

      g_rec_mutex_unlock(automation_mutex);
    }
    break;
  case PROP_LOWER:
    {
      gdouble lower;

      lower = g_value_get_double(value);

      g_rec_mutex_lock(automation_mutex);

      automation->lower = lower;      

      g_rec_mutex_unlock(automation_mutex);
    }
    break;
  case PROP_DEFAULT_VALUE:
    {
      gdouble default_value;

      default_value = g_value_get_double(value);

      g_rec_mutex_lock(automation_mutex);

      automation->default_value = default_value;      

      g_rec_mutex_unlock(automation_mutex);
    }
    break;
  case PROP_ACCELERATION:
    {
      AgsAcceleration *acceleration;

      acceleration = (AgsAcceleration *) g_value_get_object(value);

      g_rec_mutex_lock(automation_mutex);

      if(!AGS_IS_ACCELERATION(acceleration) ||
	 g_list_find(automation->acceleration, acceleration) != NULL){
	g_rec_mutex_unlock(automation_mutex);
	
	return;
      }

      g_rec_mutex_unlock(automation_mutex);

      ags_automation_add_acceleration(automation,
				      acceleration,
				      FALSE);
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

  GRecMutex *automation_mutex;

  automation = AGS_AUTOMATION(gobject);

  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_rec_mutex_lock(automation_mutex);

      g_value_set_object(value, automation->audio);

      g_rec_mutex_unlock(automation_mutex);
    }
    break;
  case PROP_LINE:
    {
      g_rec_mutex_lock(automation_mutex);

      g_value_set_uint(value, automation->line);

      g_rec_mutex_unlock(automation_mutex);
    }
    break;
  case PROP_CHANNEL_TYPE:
    {
      g_rec_mutex_lock(automation_mutex);

      g_value_set_gtype(value, automation->channel_type);

      g_rec_mutex_unlock(automation_mutex);
    }
    break;
  case PROP_TIMESTAMP:
    {
      g_rec_mutex_lock(automation_mutex);

      g_value_set_object(value, automation->timestamp);

      g_rec_mutex_unlock(automation_mutex);
    }
    break;
  case PROP_CONTROL_NAME:
    {
      g_rec_mutex_lock(automation_mutex);

      g_value_set_string(value, automation->control_name);

      g_rec_mutex_unlock(automation_mutex);
    }
    break;
  case PROP_STEPS:
    {
      g_rec_mutex_lock(automation_mutex);

      g_value_set_uint(value, automation->steps);

      g_rec_mutex_unlock(automation_mutex);
    }
    break;
  case PROP_UPPER:
    {
      g_rec_mutex_lock(automation_mutex);

      g_value_set_double(value, automation->upper);

      g_rec_mutex_unlock(automation_mutex);
    }
    break;
  case PROP_LOWER:
    {
      g_rec_mutex_lock(automation_mutex);

      g_value_set_double(value, automation->lower);

      g_rec_mutex_unlock(automation_mutex);
    }
    break;
  case PROP_DEFAULT_VALUE:
    {
      g_rec_mutex_lock(automation_mutex);

      g_value_set_double(value, automation->default_value);

      g_rec_mutex_unlock(automation_mutex);
    }
    break;
  case PROP_PORT:
    {
      g_rec_mutex_lock(automation_mutex);

      g_value_set_object(value, automation->port);

      g_rec_mutex_unlock(automation_mutex);
    }
    break;
  case PROP_ACCELERATION:
    {
      g_rec_mutex_lock(automation_mutex);

      g_value_set_pointer(value, g_list_copy_deep(automation->acceleration,
						  (GCopyFunc) g_object_ref,
						  NULL));

      g_rec_mutex_unlock(automation_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_automation_dispose(GObject *gobject)
{
  AgsAutomation *automation;

  GList *list;
  
  automation = AGS_AUTOMATION(gobject);

  /* audio */
  if(automation->audio != NULL){
    g_object_unref(automation->audio);

    automation->audio = NULL;
  }

  /* timestamp */
  if(automation->timestamp != NULL){
    g_object_unref(automation->timestamp);

    automation->timestamp = NULL;
  }

  /* source function */
  if(automation->source_function != NULL){
    g_object_run_dispose((GObject *) automation->source_function);
    
    g_object_unref(automation->source_function);

    automation->source_function = NULL;
  }

  /* port */
  if(automation->port != NULL){
    g_object_unref(automation->port);

    automation->port = NULL;
  }

  /* acceleration */
  list = automation->acceleration;

  while(list != NULL){
    g_object_run_dispose(G_OBJECT(list->data));

    list = list->next;
  }
  
  g_list_free_full(automation->acceleration,
		   g_object_unref);
  g_list_free_full(automation->selection,
		   g_object_unref);

  automation->acceleration = NULL;
  automation->selection = NULL;

  /* call parent */
  G_OBJECT_CLASS(ags_automation_parent_class)->dispose(gobject);
}

void
ags_automation_finalize(GObject *gobject)
{
  AgsAutomation *automation;

  automation = AGS_AUTOMATION(gobject);
  
  /* audio */
  if(automation->audio != NULL){
    g_object_unref(automation->audio);
  }

  /* timestamp */
  if(automation->timestamp != NULL){
    g_object_unref(automation->timestamp);
  }

  /* control name */
  if(automation->control_name != NULL){
    free(automation->control_name);
  }

  /* source function */
  if(automation->source_function != NULL){
    g_object_unref(automation->source_function);
  }

  /* port */
  if(automation->port != NULL){
    g_object_unref(automation->port);
  }
  
  /* acceleration */
  g_list_free_full(automation->acceleration,
		   g_object_unref);

  g_list_free_full(automation->selection,
		   g_object_unref);
  
  /* call parent */
  G_OBJECT_CLASS(ags_automation_parent_class)->finalize(gobject);
}

/**
 * ags_automation_test_flags:
 * @automation: the #AgsAutomation
 * @flags: the flags
 * 
 * Test @flags to be set on @automation.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_automation_test_flags(AgsAutomation *automation, AgsAutomationFlags flags)
{
  gboolean retval;
  
  GRecMutex *automation_mutex;

  if(!AGS_IS_AUTOMATION(automation)){
    return(FALSE);
  }
      
  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);

  /* test */
  g_rec_mutex_lock(automation_mutex);

  retval = (flags & (automation->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(automation_mutex);

  return(retval);
}

/**
 * ags_automation_set_flags:
 * @automation: the #AgsAutomation
 * @flags: the flags
 * 
 * Set @flags on @automation.
 * 
 * Since: 3.0.0
 */
void
ags_automation_set_flags(AgsAutomation *automation, AgsAutomationFlags flags)
{
  GRecMutex *automation_mutex;

  if(!AGS_IS_AUTOMATION(automation)){
    return;
  }
      
  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);

  /* set */
  g_rec_mutex_lock(automation_mutex);

  automation->flags |= flags;
  
  g_rec_mutex_unlock(automation_mutex);
}

/**
 * ags_automation_unset_flags:
 * @automation: the #AgsAutomation
 * @flags: the flags
 * 
 * Unset @flags on @automation.
 * 
 * Since: 3.0.0
 */
void
ags_automation_unset_flags(AgsAutomation *automation, AgsAutomationFlags flags)
{
  GRecMutex *automation_mutex;

  if(!AGS_IS_AUTOMATION(automation)){
    return;
  }
      
  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);

  /* set */
  g_rec_mutex_lock(automation_mutex);

  automation->flags &= (~flags);
  
  g_rec_mutex_unlock(automation_mutex);
}

/**
 * ags_automation_get_obj_mutex:
 * @automation: the #AgsAutomation
 * 
 * Get object mutex.
 * 
 * Returns: the #GRecMutex to lock @automation
 * 
 * Since: 3.1.0
 */
GRecMutex*
ags_automation_get_obj_mutex(AgsAutomation *automation)
{
  if(!AGS_IS_AUTOMATION(automation)){
    return(NULL);
  }

  return(AGS_AUTOMATION_GET_OBJ_MUTEX(automation));
}

/**
 * ags_automation_find_port:
 * @automation: (element-type AgsAudio.Automation) (transfer none): the #GList-struct containing #AgsAutomation
 * @port: the #AgsPort to match
 * 
 * Find automation by port.
 * 
 * Returns: (element-type AgsAudio.Automation) (transfer none): next matching automation as #GList-struct or %NULL if not found
 * 
 * Since: 3.0.0
 */
GList*
ags_automation_find_port(GList *automation,
			 GObject *port)
{
  if(automation == NULL ||
     !AGS_IS_PORT(port)){
    return(NULL);
  }

  while(automation != NULL){
    GObject *current_port;
  
    gboolean success;
    
    g_object_get(automation->data,
		 "port", &current_port,
		 NULL);
    
    success = (current_port == port) ? TRUE: FALSE;
    
    g_object_unref(current_port);

    if(success){
      break;
    }
    
    automation = automation->next;
  }

  return(automation);
}

/**
 * ags_automation_find_near_timestamp:
 * @automation: (element-type AgsAudio.Automation) (transfer none): the #GList-struct containing #AgsAutomation
 * @line: the matching audio channel
 * @timestamp: the matching timestamp
 *
 * Retrieve appropriate automation for timestamp.
 *
 * Returns: (element-type AgsAudio.Automation) (transfer none): Next matching #GList-struct or %NULL if not found
 *
 * Since: 3.0.0
 */
GList*
ags_automation_find_near_timestamp(GList *automation, guint line,
				   AgsTimestamp *timestamp)
{
  AgsTimestamp *current_timestamp;

  GList *retval;
  GList *current_start, *current_end, *current;

  guint current_line;
  guint64 current_x, x;
  guint length, position;
  gboolean use_ags_offset;
  gboolean success;

  if(automation == NULL){
    return(NULL);
  }

  current_start = automation;
  current_end = g_list_last(automation);
  
  length = g_list_length(automation);
  position = (length - 1) / 2;

  current = g_list_nth(current_start,
		       position);

  x = 0;
  use_ags_offset = FALSE;
  
  if(ags_timestamp_test_flags(timestamp,
			      AGS_TIMESTAMP_OFFSET)){
    x = ags_timestamp_get_ags_offset(timestamp);

    use_ags_offset = TRUE;
  }else if(ags_timestamp_test_flags(timestamp,
				    AGS_TIMESTAMP_UNIX)){
    x = ags_timestamp_get_unix_time(timestamp);

    use_ags_offset = FALSE;
  }
  
  retval = NULL;
  success = FALSE;
  
  while(!success && current != NULL){
    current_x = 0;
    
    /* check current - start */
    g_object_get(current_start->data,
		 "line", &current_line,
		 NULL);
    
    if(current_line == line){
      if(timestamp == NULL){
	retval = current_start;

	break;
      }

      g_object_get(current_start->data,
		   "timestamp", &current_timestamp,
		   NULL);

      if(current_timestamp != NULL){
	if(use_ags_offset){
	  current_x = ags_timestamp_get_ags_offset(current_timestamp);
	  
	  g_object_unref(current_timestamp);
	    
	  if(current_x > x){
	    break;
	  }
	}else{
	  current_x = ags_timestamp_get_unix_time(current_timestamp);
	  
	  g_object_unref(current_timestamp);
	  
	  if(current_x > x){
	    break;
	  }
	}

	if(use_ags_offset){
	  if(current_x >= x &&
	     current_x < x + AGS_AUTOMATION_DEFAULT_OFFSET){
	    retval = current_start;
	    
	    break;
	  }
	}else{
	  if(current_x >= x &&
	     current_x < x + AGS_AUTOMATION_DEFAULT_DURATION){
	    retval = current_start;
	    
	    break;
	  }
	}
      }else{
	g_warning("inconsistent data");
      }
    }

    /* check current - end */
    g_object_get(current_end->data,
		 "line", &current_line,
		 NULL);
    
    if(current_line == line){
      if(timestamp == NULL){
	retval = current_end;
	
	break;
      }

      g_object_get(current_end->data,
		   "timestamp", &current_timestamp,
		   NULL);

      if(current_timestamp != NULL){
	if(use_ags_offset){
	  current_x = ags_timestamp_get_ags_offset(current_timestamp);
	  
	  g_object_unref(current_timestamp);
	  
	  if(current_x < x){
	    break;
	  }
	}else{
	  current_x = ags_timestamp_get_unix_time(current_timestamp);
	  
	  g_object_unref(current_timestamp);
	  
	  if(current_x < x){
	    break;
	  }
	}

	if(use_ags_offset){
	  if(current_x >= x &&
	     current_x < x + AGS_AUTOMATION_DEFAULT_OFFSET){
	    retval = current_end;
	    
	    break;
	  }
	}else{
	  if(current_x >= x &&
	     current_x < x + AGS_AUTOMATION_DEFAULT_DURATION){
	    retval = current_end;
	    
	    break;
	  }
	}
      }else{
	g_warning("inconsistent data");
      }
    }

    /* check current - center */
    g_object_get(current->data,
		 "line", &current_line,
		 NULL);
    
    if(current_line == line){
      if(timestamp == NULL){
	retval = current;
	
	break;
      }
    }
    
    g_object_get(current->data,
		 "timestamp", &current_timestamp,
		 NULL);

    if(current_timestamp != NULL){
      if(use_ags_offset){
	current_x = ags_timestamp_get_ags_offset(current_timestamp);

	g_object_unref(current_timestamp);
	  
	if(current_x >= x &&
	   current_x < x + AGS_AUTOMATION_DEFAULT_OFFSET &&
	  current_line == line){
	  retval = current;
	    
	  break;
	}
      }else{
	current_x = ags_timestamp_get_unix_time(current_timestamp);

	g_object_unref(current_timestamp);
	  
	if(current_x >= x &&
	   current_x < x + AGS_AUTOMATION_DEFAULT_DURATION &&
	   current_line == line){
	  retval = current;
	    
	  break;
	}
      }
    }else{
      g_warning("inconsistent data");
    }
    
    if(length <= 3){
      break;
    }

    if(current_x < x){
      current_start = current->next;
      current_end = current_end->prev;
    }else if(current_x > x){
      current_start = current_start->next;
      current_end = current->prev;
    }else{
      current_start = current_start->next;
      //NOTE:JK: we want progression
      //current_end = current_end->prev;
    }

    length = g_list_position(current_start,
			     current_end) + 1;
    position = (length - 1) / 2;

    current = g_list_nth(current_start,
			 position);
  }

  return(retval);
}

/**
 * ags_automation_find_near_timestamp_extended:
 * @automation: (element-type AgsAudio.Automation) (transfer none): the #GList-struct containing #AgsAutomation
 * @line: the matching audio channel
 * @channel_type: the matching channel type
 * @control_name: the matching control name
 * @timestamp: the matching timestamp
 *
 * Retrieve appropriate automation for timestamp.
 *
 * Returns: (element-type AgsAudio.Automation) (transfer none): Next matching #GList-struct or %NULL if not found
 *
 * Since: 3.0.0
 */
GList*
ags_automation_find_near_timestamp_extended(GList *automation, guint line,
					    GType channel_type, gchar *control_name,
					    AgsTimestamp *timestamp)
{
  AgsTimestamp *current_timestamp;

  GList *retval;
  GList *current_start, *current_end, *current;

  GType current_channel_type;

  gchar *current_control_name;

  guint current_line;
  guint64 current_x, x;
  guint length, position;
  gboolean use_ags_offset;
  gboolean success;

  if(automation == NULL){
    return(NULL);
  }

  current_start = automation;
  current_end = g_list_last(automation);
  
  length = g_list_length(automation);
  position = (length - 1) / 2;

  current = g_list_nth(current_start,
		       position);

  x = 0;
  use_ags_offset = FALSE;

  if(ags_timestamp_test_flags(timestamp,
			      AGS_TIMESTAMP_OFFSET)){
    x = ags_timestamp_get_ags_offset(timestamp);

    use_ags_offset = TRUE;
  }else if(ags_timestamp_test_flags(timestamp,
				    AGS_TIMESTAMP_UNIX)){
    x = ags_timestamp_get_unix_time(timestamp);

    use_ags_offset = FALSE;
  }
  
  retval = NULL;
  success = FALSE;
  
  while(!success && current != NULL){
    current_x = 0;

    /* check current - start */
    g_object_get(current_start->data,
		 "line", &current_line,
		 "channel-type", &current_channel_type,
		 "control-name", &current_control_name,
		 NULL);
    
    if(current_line == line &&
       g_type_is_a(current_channel_type,
		   channel_type) &&
       !g_strcmp0(current_control_name,
		  control_name)){
      g_free(current_control_name);
      
      if(timestamp == NULL){
	retval = current_start;
	
	break;
      }

      g_object_get(current_start->data,
		   "timestamp", &current_timestamp,
		   NULL);

      if(current_timestamp != NULL){
	if(use_ags_offset){
	  current_x = ags_timestamp_get_ags_offset(current_timestamp);
	  
	  g_object_unref(current_timestamp);
	    
	  if(current_x > x){
	    break;
	  }
	}else{
	  current_x = ags_timestamp_get_unix_time(current_timestamp);
	  
	  g_object_unref(current_timestamp);
	  
	  if(current_x > x){
	    break;
	  }
	}

	if(use_ags_offset){
	  if(current_x >= x &&
	     current_x < x + AGS_AUTOMATION_DEFAULT_OFFSET){
	    retval = current_start;
	    
	    break;
	  }
	}else{
	  if(current_x >= x &&
	     current_x < x + AGS_AUTOMATION_DEFAULT_DURATION){
	    retval = current_start;
	    
	    break;
	  }
	}
      }else{
	g_warning("inconsistent data");
      }
    }else{
      g_free(current_control_name);
    }

    /* check current - end */
    g_object_get(current_end->data,
		 "line", &current_line,
		 "channel-type", &current_channel_type,
		 "control-name", &current_control_name,
		 NULL);
    
    if(current_line == line &&
       g_type_is_a(current_channel_type,
		   channel_type) &&
       !g_strcmp0(current_control_name,
		  control_name)){
      g_free(current_control_name);
      
      if(timestamp == NULL){
	retval = current_end;
	
	break;
      }

      g_object_get(current_end->data,
		   "timestamp", &current_timestamp,
		   NULL);

      if(current_timestamp != NULL){
	if(use_ags_offset){
	  current_x = ags_timestamp_get_ags_offset(current_timestamp);
	  
	  g_object_unref(current_timestamp);
	  
	  if(current_x < x){
	    break;
	  }
	}else{
	  current_x = ags_timestamp_get_unix_time(current_timestamp);
	  
	  g_object_unref(current_timestamp);
	  
	  if(current_x < x){
	    break;
	  }
	}

	if(use_ags_offset){
	  if(current_x >= x &&
	     current_x < x + AGS_AUTOMATION_DEFAULT_OFFSET){
	    retval = current_end;
	    
	    break;
	  }
	}else{
	  if(current_x >= x &&
	     current_x < x + AGS_AUTOMATION_DEFAULT_DURATION){
	    retval = current_end;
	    
	    break;
	  }
	}
      }else{
	g_warning("inconsistent data");
      }
    }else{
      g_free(current_control_name);
    }

    /* check current - center */
    current_x = 0;
    
    g_object_get(current->data,
		 "line", &current_line,
		 "channel-type", &current_channel_type,
		 "control-name", &current_control_name,
		 NULL);
    
    if(current_line == line &&
       g_type_is_a(current_channel_type,
		   channel_type) &&
       !g_strcmp0(current_control_name,
		  control_name)){
      if(timestamp == NULL){
	retval = current;

	g_free(current_control_name);
	
	break;
      }
    }
    
    g_object_get(current->data,
		 "timestamp", &current_timestamp,
		 NULL);

    if(current_timestamp != NULL){
      if(use_ags_offset){
	current_x = ags_timestamp_get_ags_offset(current_timestamp);

	g_object_unref(current_timestamp);
	  
	if(current_x >= x &&
	   current_x < x + AGS_AUTOMATION_DEFAULT_OFFSET &&
	   current_line == line &&
	   g_type_is_a(current_channel_type,
		       channel_type) &&
	   !g_strcmp0(current_control_name,
		      control_name)){
	  retval = current;
	    
	  break;
	}
      }else{
	current_x = ags_timestamp_get_unix_time(current_timestamp);

	g_object_unref(current_timestamp);
	  
	if(current_x >= x &&
	   current_x < x + AGS_AUTOMATION_DEFAULT_DURATION &&
	   current_line == line &&
	   g_type_is_a(current_channel_type,
		       channel_type) &&
	   !g_strcmp0(current_control_name,
		      control_name)){
	  retval = current;
	    
	  break;
	}
      }
    }else{
      g_warning("inconsistent data");
    }
    
    g_free(current_control_name);
      
    if(position == 0){
      break;
    }

    if(current_x < x){
      current_start = current->next;
      current_end = current_end->prev;
    }else if(current_x > x){
      current_start = current_start->next;
      current_end = current->prev;
    }else{
      current_start = current_start->next;
      //NOTE:JK: we want progression
      //current_end = current_end->prev;
    }

    length = g_list_position(current_start,
			     current_end) + 1;
    position = (length - 1) / 2;

    current = g_list_nth(current_start,
			 position);
  }

  return(retval);
}

/**
 * ags_automation_sort_func:
 * @a: an #AgsAutomation
 * @b: an other #AgsAutomation
 * 
 * Compare @a to @b.
 * 
 * Returns: 0 if equal, -1 if smaller and 1 if bigger offset
 * 
 * Since: 3.0.0
 */
gint
ags_automation_sort_func(gconstpointer a,
			 gconstpointer b)
{
  AgsTimestamp *timestamp_a, *timestamp_b;

  gchar *control_name_a, *control_name_b;
  
  guint64 offset_a, offset_b;
  guint line_a, line_b;
  gint retval;
  
  timestamp_a = NULL;
  timestamp_b = NULL;

  control_name_a = NULL;
  control_name_b = NULL;

  line_a = 0;
  line_b = 0;
  
  g_object_get(a,
	       "timestamp", &timestamp_a,
	       "line", &line_a,
	       "control-name", &control_name_a,
	       NULL);

  g_object_get(b,
	       "timestamp", &timestamp_b,
	       "line", &line_b,
	       "control-name", &control_name_b,
	       NULL);

  offset_a = ags_timestamp_get_ags_offset(timestamp_a);
  offset_b = ags_timestamp_get_ags_offset(timestamp_b);

  g_object_unref(timestamp_a);
  g_object_unref(timestamp_b);

  retval = 0;
  
  if(offset_a == offset_b){
    if(line_a == line_b){
      retval = g_strcmp0(control_name_a, control_name_b);
    }else{
      retval = (line_a > line_b) ? -1: 1;
    }
  }else{
    retval = (offset_a > offset_b) ? -1: 1;
  }

  g_free(control_name_a);
  g_free(control_name_b);
  
  return(retval);
}

/**
 * ags_automation_add:
 * @automation: (element-type AgsAudio.Automation) (transfer none): the #GList-struct containing #AgsAutomation
 * @new_automation: the #AgsAutomation to add
 * 
 * Add @new_automation sorted to @automation
 * 
 * Returns: (element-type AgsAudio.Automation) (transfer none): the new beginning of @automation
 * 
 * Since: 3.0.0
 */
GList*
ags_automation_add(GList *automation,
		   AgsAutomation *new_automation)
{  
  if(!AGS_IS_AUTOMATION(new_automation)){
    return(automation);
  }

  automation = g_list_insert_sorted(automation,
				    new_automation,
				    ags_automation_sort_func);
  
  return(automation);
}

/**
 * ags_automation_remove_all_empty:
 * @automation: (element-type AgsAudio.Automation) (transfer none): the #GList-struct containing #AgsAutomation
 * 
 * Remove all empty @automation.
 * 
 * Returns: (element-type AgsAudio.Automation) (transfer none): the new beginning of @automation
 * 
 * Since: 5.3.8
 */
GList*
ags_automation_remove_all_empty(GList *automation)
{
  GList *list;

  list = automation;

  while(list != NULL){
    GList *next;

    GRecMutex *automation_mutex;
    
    next = list->next;

    automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(list->data);

    g_rec_mutex_lock(automation_mutex);

    if(AGS_AUTOMATION(list->data)->acceleration == NULL){
      automation = g_list_delete_link(automation,
				      list);
    }
    
    g_rec_mutex_unlock(automation_mutex);
    
    list = next;
  }

  return(automation);
}

/**
 * ags_automation_get_audio:
 * @automation: the #AgsAutomation
 * 
 * Get audio.
 * 
 * Returns: (transfer full): the #AgsAudio
 * 
 * Since: 3.1.0
 */
GObject*
ags_automation_get_audio(AgsAutomation *automation)
{
  GObject *audio;

  if(!AGS_IS_AUTOMATION(automation)){
    return(NULL);
  }

  g_object_get(automation,
	       "audio", &audio,
	       NULL);

  return(audio);
}

/**
 * ags_automation_set_audio:
 * @automation: the #AgsAutomation
 * @audio: the #AgsAudio
 * 
 * Set audio.
 * 
 * Since: 3.1.0
 */
void
ags_automation_set_audio(AgsAutomation *automation, GObject *audio)
{
  if(!AGS_IS_AUTOMATION(automation)){
    return;
  }

  g_object_set(automation,
	       "audio", audio,
	       NULL);
}

/**
 * ags_automation_get_channel_type:
 * @automation: the #AgsAutomation
 *
 * Gets channel type.
 * 
 * Returns: the channel type
 * 
 * Since: 3.1.0
 */
GType
ags_automation_get_channel_type(AgsAutomation *automation)
{
  GType channel_type;
  
  if(!AGS_IS_AUTOMATION(automation)){
    return(0);
  }

  g_object_get(automation,
	       "channel_type", &channel_type,
	       NULL);

  return(channel_type);
}

/**
 * ags_automation_set_channel_type:
 * @automation: the #AgsAutomation
 * @channel_type: the channel type
 *
 * Sets channel type.
 * 
 * Since: 3.1.0
 */
void
ags_automation_set_channel_type(AgsAutomation *automation, GType channel_type)
{
  if(!AGS_IS_AUTOMATION(automation)){
    return;
  }

  g_object_set(automation,
	       "channel-type", channel_type,
	       NULL);
}

/**
 * ags_automation_get_line:
 * @automation: the #AgsAutomation
 *
 * Gets line.
 * 
 * Returns: the line
 * 
 * Since: 3.1.0
 */
guint
ags_automation_get_line(AgsAutomation *automation)
{
  guint line;
  
  if(!AGS_IS_AUTOMATION(automation)){
    return(0);
  }

  g_object_get(automation,
	       "line", &line,
	       NULL);

  return(line);
}

/**
 * ags_automation_set_line:
 * @automation: the #AgsAutomation
 * @line: the line
 *
 * Sets line.
 * 
 * Since: 3.1.0
 */
void
ags_automation_set_line(AgsAutomation *automation, guint line)
{
  if(!AGS_IS_AUTOMATION(automation)){
    return;
  }

  g_object_set(automation,
	       "line", line,
	       NULL);
}

/**
 * ags_automation_get_timestamp:
 * @automation: the #AgsAutomation
 * 
 * Get timestamp.
 * 
 * Returns: (transfer full): the #AgsTimestamp
 * 
 * Since: 3.1.0
 */
AgsTimestamp*
ags_automation_get_timestamp(AgsAutomation *automation)
{
  AgsTimestamp *timestamp;

  if(!AGS_IS_AUTOMATION(automation)){
    return(NULL);
  }

  g_object_get(automation,
	       "timestamp", &timestamp,
	       NULL);

  return(timestamp);
}

/**
 * ags_automation_set_timestamp:
 * @automation: the #AgsAutomation
 * @timestamp: the #AgsTimestamp
 * 
 * Set timestamp.
 * 
 * Since: 3.1.0
 */
void
ags_automation_set_timestamp(AgsAutomation *automation, AgsTimestamp *timestamp)
{
  if(!AGS_IS_AUTOMATION(automation)){
    return;
  }

  g_object_set(automation,
	       "timestamp", timestamp,
	       NULL);
}

/**
 * ags_automation_get_control_name:
 * @automation: the #AgsAutomation
 * 
 * Get control name.
 * 
 * Returns: (transfer full): the control name
 * 
 * Since: 3.1.0
 */
gchar*
ags_automation_get_control_name(AgsAutomation *automation)
{
  gchar *control_name;

  if(!AGS_IS_AUTOMATION(automation)){
    return(NULL);
  }

  g_object_get(automation,
	       "control-name", &control_name,
	       NULL);

  return(control_name);
}

/**
 * ags_automation_set_control_name:
 * @automation: the #AgsAutomation
 * @control_name: the control name
 * 
 * Set control name.
 * 
 * Since: 3.1.0
 */
void
ags_automation_set_control_name(AgsAutomation *automation, gchar *control_name)
{
  if(!AGS_IS_AUTOMATION(automation)){
    return;
  }

  g_object_set(automation,
	       "control-name", control_name,
	       NULL);
}

/**
 * ags_automation_get_steps:
 * @automation: the #AgsAutomation
 *
 * Gets steps.
 * 
 * Returns: the steps
 * 
 * Since: 3.1.0
 */
guint
ags_automation_get_steps(AgsAutomation *automation)
{
  guint steps;
  
  if(!AGS_IS_AUTOMATION(automation)){
    return(0);
  }

  g_object_get(automation,
	       "steps", &steps,
	       NULL);

  return(steps);
}

/**
 * ags_automation_set_steps:
 * @automation: the #AgsAutomation
 * @steps: the steps
 *
 * Sets steps.
 * 
 * Since: 3.1.0
 */
void
ags_automation_set_steps(AgsAutomation *automation, guint steps)
{
  if(!AGS_IS_AUTOMATION(automation)){
    return;
  }

  g_object_set(automation,
	       "steps", steps,
	       NULL);
}

/**
 * ags_automation_get_upper:
 * @automation: the #AgsAutomation
 *
 * Gets upper.
 * 
 * Returns: the upper
 * 
 * Since: 3.1.0
 */
gdouble
ags_automation_get_upper(AgsAutomation *automation)
{
  gdouble upper;
  
  if(!AGS_IS_AUTOMATION(automation)){
    return(0.0);
  }

  g_object_get(automation,
	       "upper", &upper,
	       NULL);

  return(upper);
}

/**
 * ags_automation_set_upper:
 * @automation: the #AgsAutomation
 * @upper: the upper
 *
 * Sets upper.
 * 
 * Since: 3.1.0
 */
void
ags_automation_set_upper(AgsAutomation *automation, gdouble upper)
{
  if(!AGS_IS_AUTOMATION(automation)){
    return;
  }

  g_object_set(automation,
	       "upper", upper,
	       NULL);
}

/**
 * ags_automation_get_lower:
 * @automation: the #AgsAutomation
 *
 * Gets lower.
 * 
 * Returns: the lower
 * 
 * Since: 3.1.0
 */
gdouble
ags_automation_get_lower(AgsAutomation *automation)
{
  gdouble lower;
  
  if(!AGS_IS_AUTOMATION(automation)){
    return(0.0);
  }

  g_object_get(automation,
	       "lower", &lower,
	       NULL);

  return(lower);
}

/**
 * ags_automation_set_lower:
 * @automation: the #AgsAutomation
 * @lower: the lower
 *
 * Sets lower.
 * 
 * Since: 3.1.0
 */
void
ags_automation_set_lower(AgsAutomation *automation, gdouble lower)
{
  if(!AGS_IS_AUTOMATION(automation)){
    return;
  }

  g_object_set(automation,
	       "lower", lower,
	       NULL);
}

/**
 * ags_automation_get_default_value:
 * @automation: the #AgsAutomation
 *
 * Gets default value.
 * 
 * Returns: the default value
 * 
 * Since: 3.1.0
 */
gdouble
ags_automation_get_default_value(AgsAutomation *automation)
{
  gdouble default_value;
  
  if(!AGS_IS_AUTOMATION(automation)){
    return(0.0);
  }

  g_object_get(automation,
	       "default-value", &default_value,
	       NULL);

  return(default_value);
}

/**
 * ags_automation_set_default_value:
 * @automation: the #AgsAutomation
 * @default_value: the default value
 *
 * Sets default value.
 * 
 * Since: 3.1.0
 */
void
ags_automation_set_default_value(AgsAutomation *automation, gdouble default_value)
{
  if(!AGS_IS_AUTOMATION(automation)){
    return;
  }

  g_object_set(automation,
	       "default-value", default_value,
	       NULL);
}

/**
 * ags_automation_get_port:
 * @automation: the #AgsAutomation
 * 
 * Get port.
 * 
 * Returns: (transfer full): the #AgsPort
 * 
 * Since: 3.1.0
 */
GObject*
ags_automation_get_port(AgsAutomation *automation)
{
  GObject *port;

  if(!AGS_IS_AUTOMATION(automation)){
    return(NULL);
  }

  g_object_get(automation,
	       "port", &port,
	       NULL);

  return(port);
}

/**
 * ags_automation_set_port:
 * @automation: the #AgsAutomation
 * @port: the #AgsPort
 * 
 * Set port.
 * 
 * Since: 3.1.0
 */
void
ags_automation_set_port(AgsAutomation *automation, GObject *port)
{
  if(!AGS_IS_AUTOMATION(automation)){
    return;
  }

  g_object_set(automation,
	       "port", port,
	       NULL);
}


/**
 * ags_automation_get_acceleration:
 * @automation: the #AgsAutomation
 * 
 * Get acceleration.
 * 
 * Returns: (element-type AgsAudio.Acceleration) (transfer full): the #GList-struct containig #AgsAcceleration
 * 
 * Since: 3.1.0
 */
GList*
ags_automation_get_acceleration(AgsAutomation *automation)
{
  GList *acceleration;

  if(!AGS_IS_AUTOMATION(automation)){
    return(NULL);
  }

  g_object_get(automation,
	       "acceleration", &acceleration,
	       NULL);

  return(acceleration);
}

/**
 * ags_automation_set_acceleration:
 * @automation: the #AgsAutomation
 * @acceleration: (element-type AgsAudio.Acceleration) (transfer full): the #GList-struct containing #AgsAcceleration
 * 
 * Set acceleration by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_automation_set_acceleration(AgsAutomation *automation, GList *acceleration)
{
  GList *start_acceleration;
  
  GRecMutex *automation_mutex;

  if(!AGS_IS_AUTOMATION(automation)){
    return;
  }

  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);
    
  g_rec_mutex_lock(automation_mutex);

  start_acceleration = automation->acceleration;
  automation->acceleration = acceleration;
  
  g_rec_mutex_unlock(automation_mutex);

  g_list_free_full(start_acceleration,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_automation_add_acceleration:
 * @automation: the #AgsAutomation
 * @acceleration: the #AgsAcceleration to add
 * @use_selection_list: if %TRUE add to selection, else to default automation
 *
 * Adds @acceleration to @automation.
 *
 * Since: 3.0.0
 */
void
ags_automation_add_acceleration(AgsAutomation *automation,
				AgsAcceleration *acceleration,
				gboolean use_selection_list)
{
  AgsAcceleration *current;

  GRecMutex *automation_mutex;

  if(!AGS_IS_AUTOMATION(automation) ||
     !AGS_IS_ACCELERATION(acceleration)){
    return;
  }

  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);

  /* insert sorted */
  g_rec_mutex_lock(automation_mutex);

  if(use_selection_list){
    if((current = ags_automation_find_point(automation, acceleration->x, acceleration->y, TRUE)) != NULL){
      current->x = acceleration->x;
      current->y = acceleration->y;
    }else{
      if(g_list_find(automation->selection, acceleration) == NULL &&
	 ags_automation_find_point(automation, acceleration->x, acceleration->y, TRUE) == NULL){
	g_object_ref(acceleration);
      
	automation->selection = g_list_insert_sorted(automation->selection,
						     acceleration,
						     (GCompareFunc) ags_acceleration_sort_func);
	ags_acceleration_set_flags(acceleration,
				   AGS_ACCELERATION_IS_SELECTED);
      }
    }
  }else{
    if((current = ags_automation_find_point(automation, acceleration->x, acceleration->y, FALSE)) != NULL){
      current->x = acceleration->x;
      current->y = acceleration->y;
    }else{
      if(g_list_find(automation->acceleration, acceleration) == NULL){
	g_object_ref(acceleration);
  
	automation->acceleration = g_list_insert_sorted(automation->acceleration,
							acceleration,
							(GCompareFunc) ags_acceleration_sort_func);
      }
    }
  }

  g_rec_mutex_unlock(automation_mutex);
}

/**
 * ags_automation_remove_acceleration:
 * @automation: the #AgsAutomation
 * @acceleration: the #AgsAcceleration to remove
 * @use_selection_list: if %TRUE remove from selection, else from default automation
 *
 * Removes @acceleration from @automation.
 *
 * Since: 3.0.0
 */
void
ags_automation_remove_acceleration(AgsAutomation *automation,
				   AgsAcceleration *acceleration,
				   gboolean use_selection_list)
{
  GRecMutex *automation_mutex;

  if(!AGS_IS_AUTOMATION(automation) ||
     !AGS_IS_ACCELERATION(acceleration)){
    return;
  }

  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);

  /* remove if found */
  g_rec_mutex_lock(automation_mutex);
  
  if(!use_selection_list){
    if(g_list_find(automation->acceleration,
		   acceleration) != NULL){
      automation->acceleration = g_list_remove(automation->acceleration,
					       acceleration);
      g_object_unref(acceleration);
    }
  }else{
    if(g_list_find(automation->selection,
		   acceleration) != NULL){
      automation->selection = g_list_remove(automation->selection,
					    acceleration);
      g_object_unref(acceleration);
    }
  }

  g_rec_mutex_unlock(automation_mutex);
}

/**
 * ags_automation_remove_acceleration_at_position:
 * @automation: the #AgsAutomation
 * @x: x offset
 * @y: y value of acceleration
 *
 * Removes one #AgsAcceleration of automation.
 *
 * Returns: %TRUE if successfully removed acceleration, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_automation_remove_acceleration_at_position(AgsAutomation *automation,
					       guint x, gdouble y)
{
  AgsAcceleration *acceleration;
  
  GList *start_list, *list;

  gdouble upper, lower;
  guint current_x;
  gdouble current_y;
  gboolean retval;

  GRecMutex *automation_mutex;

  if(!AGS_IS_AUTOMATION(automation)){
    return(FALSE);
  }

  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);

  /* find acceleration */
  g_rec_mutex_lock(automation_mutex);

  upper = automation->upper;
  lower = automation->lower;
  
  list =
    start_list = g_list_copy_deep(automation->acceleration,
				  (GCopyFunc) g_object_ref,
				  NULL);
  
  g_rec_mutex_unlock(automation_mutex);

  acceleration = NULL;

  retval = FALSE;
  
  while(list != NULL){
    g_object_get(list->data,
		 "x", &current_x,
		 "y", &current_y,
		 NULL);
    
    if(current_x == x &&
       (current_y - ((upper - lower) / AGS_AUTOMATION_MAXIMUM_STEPS) <= y &&
	current_y + ((upper - lower) / AGS_AUTOMATION_MAXIMUM_STEPS) >= y)){
      acceleration = list->data;
      
      retval = TRUE;
      
      break;
    }

    if(current_x > x){
      break;
    }
    
    list = list->next;
  }

  /* delete link and unref */
  if(retval){
    g_rec_mutex_lock(automation_mutex);
    
    automation->acceleration = g_list_remove(automation->acceleration,
					     acceleration);
    g_object_unref(acceleration);
  
    g_rec_mutex_unlock(automation_mutex);
  }

  g_list_free_full(start_list,
		   g_object_unref);

  return(retval);
}

/**
 * ags_automation_get_selection:
 * @automation: the #AgsAutomation
 *
 * Retrieve selection.
 *
 * Returns: (element-type AgsAudio.Acceleration) (transfer none): the selection.
 *
 * Since: 3.0.0
 */
GList*
ags_automation_get_selection(AgsAutomation *automation)
{
  GList *selection;

  GRecMutex *automation_mutex;

  if(!AGS_IS_AUTOMATION(automation)){
    return(NULL);
  }

  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);

  /* selection */
  g_rec_mutex_lock(automation_mutex);

  selection = automation->selection;
  
  g_rec_mutex_unlock(automation_mutex);
  
  return(selection);
}

/**
 * ags_automation_is_acceleration_selected:
 * @automation: the #AgsAutomation
 * @acceleration: the #AgsAcceleration to check for
 *
 * Check selection for acceleration.
 *
 * Returns: %TRUE if selected, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_automation_is_acceleration_selected(AgsAutomation *automation, AgsAcceleration *acceleration)
{
  GList *selection;

  guint x, current_x;
  gboolean retval;
  
  GRecMutex *automation_mutex;

  if(!AGS_IS_AUTOMATION(automation) ||
     !AGS_IS_ACCELERATION(acceleration)){
    return(FALSE);
  }

  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);

  /* get x */
  g_object_get(acceleration,
	       "x", &x,
	       NULL);
  
  /* match acceleration */
  g_rec_mutex_lock(automation_mutex);

  selection = automation->selection;
  retval = FALSE;
  
  while(selection != NULL){
    /* get current x */
    g_object_get(selection->data,
		 "x", &current_x,
		 NULL);

    if(current_x > x){
      break;
    }
    
    if(selection->data == acceleration){
      retval = TRUE;

      break;
    }

    selection = selection->next;
  }

  g_rec_mutex_unlock(automation_mutex);

  return(retval);
}

/**
 * ags_automation_find_point:
 * @automation: the #AgsAutomation
 * @x: x offset
 * @y: y value acceleration, will be ignored
 * @use_selection_list: if %TRUE selection is searched
 *
 * Find acceleration by offset and acceleration.
 *
 * Returns: (transfer none): the matching acceleration.
 *
 * Since: 3.0.0
 */ 
AgsAcceleration*
ags_automation_find_point(AgsAutomation *automation,
			  guint x, gdouble y,
			  gboolean use_selection_list)
{
  AgsAcceleration *retval;
  
  GList *acceleration;

  guint current_x;

  GRecMutex *automation_mutex;

  if(!AGS_IS_AUTOMATION(automation)){
    return(NULL);
  }

  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);

  /* find acceleration */
  g_rec_mutex_lock(automation_mutex);

  if(use_selection_list){
    acceleration = automation->selection;
  }else{
    acceleration = automation->acceleration;
  }

  retval = NULL;
  
  while(acceleration != NULL){
    g_object_get(acceleration->data,
		 "x", &current_x,
		 NULL);
    
    if(current_x > x){
      break;
    }

    if(current_x == x){
      retval = acceleration->data;

      break;
    }
    
    acceleration = acceleration->next;
  }

  g_rec_mutex_unlock(automation_mutex);

  return(retval);
}

/**
 * ags_automation_find_region:
 * @automation: the #AgsAutomation
 * @x0: start offset
 * @y0: value start
 * @x1: end offset
 * @y1: value end
 * @use_selection_list: if %TRUE selection is searched
 *
 * Find acceleration by offset and value region.
 *
 * Returns: (element-type AgsAudio.Acceleration) (transfer full): the matching accelerations as #GList-struct
 *
 * Since: 3.0.0
 */
GList*
ags_automation_find_region(AgsAutomation *automation,
			   guint x0, gdouble y0,
			   guint x1, gdouble y1,
			   gboolean use_selection_list)
{
  GList *acceleration;
  GList *region;

  guint current_x;
  gdouble current_y;
  
  GRecMutex *automation_mutex;

  if(!AGS_IS_AUTOMATION(automation)){
    return(NULL);
  }

  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);

  if(x0 > x1){
    guint tmp;

    tmp = x1;
    x1 = x0;
    x0 = x1;
  }

  if(y0 > y1){
    gdouble tmp_y;

    tmp_y = y0;
    y0 = y1;
    y1 = tmp_y;
  }
  
  /* find acceleration */
  g_rec_mutex_lock(automation_mutex);

  if(use_selection_list){
    acceleration = automation->selection;
  }else{
    acceleration = automation->acceleration;
  }

  while(acceleration != NULL){
    g_object_get(acceleration->data,
		 "x", &current_x,
		 NULL);
    
    if(current_x >= x0){
      break;
    }
    
    acceleration = acceleration->next;
  }

  region = NULL;

  while(acceleration != NULL){
    g_object_get(acceleration->data,
		 "x", &current_x,
		 "y", &current_y,
		 NULL);

    if(current_x > x1){
      break;
    }

    if(current_y >= y0 && current_y < y1){
      region = g_list_prepend(region,
			      acceleration->data);
    }

    acceleration = acceleration->next;
  }

  g_rec_mutex_unlock(automation_mutex);

  region = g_list_reverse(region);

  return(region);
}

/**
 * ags_automation_free_selection:
 * @automation: the #AgsAutomation
 *
 * Clear selection.
 *
 * Since: 3.0.0
 */
void
ags_automation_free_selection(AgsAutomation *automation)
{
  AgsAcceleration *acceleration;

  GList *list_start, *list;

  GRecMutex *automation_mutex;

  if(!AGS_IS_AUTOMATION(automation)){
    return;
  }

  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);

  /* free selection */
  g_rec_mutex_lock(automation_mutex);

  list =
    list_start = automation->selection;
  
  while(list != NULL){
    ags_acceleration_unset_flags(list->data,
				 AGS_ACCELERATION_IS_SELECTED);
    
    list = list->next;
  }

  automation->selection = NULL;

  g_rec_mutex_unlock(automation_mutex);
  
  g_list_free_full(list_start,
		   g_object_unref);
}

/**
 * ags_automation_free_all_selection:
 * @automation: the #GList-struct containing #AgsAutomation
 *
 * Clear all selection of @automation.
 *
 * Since: 3.14.10
 */
void
ags_automation_free_all_selection(GList *automation)
{
  while(automation != NULL){
    ags_automation_free_selection(automation->data);

    automation = automation->next;
  }
}

/**
 * ags_automation_add_point_to_selection:
 * @automation: the #AgsAutomation
 * @x: x offset
 * @y: y acceleration value
 * @replace_current_selection: if %TRUE selection is replaced
 *
 * Select acceleration at position.
 *
 * Since: 3.0.0
 */ 
void
ags_automation_add_point_to_selection(AgsAutomation *automation,
				      guint x, gdouble y,
				      gboolean replace_current_selection)
{
  AgsAcceleration *acceleration;

  GRecMutex *automation_mutex;

  if(!AGS_IS_AUTOMATION(automation)){
    return;
  }

  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);

  /* find acceleration */
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
    ags_acceleration_set_flags(acceleration, AGS_ACCELERATION_IS_SELECTED);

    if(replace_current_selection){
      GList *list;

      list = g_list_alloc();
      list->data = acceleration;
      g_object_ref(acceleration);
      
      ags_automation_free_selection(automation);

      /* replace */
      g_rec_mutex_lock(automation_mutex);

      automation->selection = list;
      
      g_rec_mutex_unlock(automation_mutex);
    }else{
      if(!ags_automation_is_acceleration_selected(automation,
						  acceleration)){
	/* add */
	ags_automation_add_acceleration(automation,
					acceleration, TRUE);
      }
    }
  }
}

/**
 * ags_automation_remove_point_from_selection:
 * @automation: the #AgsAutomation
 * @x: x offset
 * @y: y acceleration value
 *
 * Remove acceleration at position of selection.
 *
 * Since: 3.0.0
 */ 
void
ags_automation_remove_point_from_selection(AgsAutomation *automation,
					   guint x, gdouble y)
{
  AgsAcceleration *acceleration;

  GRecMutex *automation_mutex;

  if(!AGS_IS_AUTOMATION(automation)){
    return;
  }

  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);

  /* find point */
  acceleration = ags_automation_find_point(automation,
					   x, y,
					   TRUE);

  if(acceleration != NULL){
    ags_acceleration_unset_flags(acceleration,
				 AGS_ACCELERATION_IS_SELECTED);

    /* remove acceleration from selection */
    g_rec_mutex_lock(automation_mutex);
    
    automation->selection = g_list_remove(automation->selection,
					  acceleration);
    g_object_unref(acceleration);

    g_rec_mutex_unlock(automation_mutex);
  }
}

/**
 * ags_automation_add_region_from_selection:
 * @automation: the #AgsAutomation
 * @x0: x start offset
 * @y0: y start acceleration
 * @x1: x end offset
 * @y1: y end acceleration
 * @replace_current_selection: if %TRUE current selection is replaced, else if %FALSE
 * it is added to current selection.
 *
 * Add acceleration within region of selection.
 *
 * Since: 3.0.0
 */ 
void
ags_automation_add_region_to_selection(AgsAutomation *automation,
				       guint x0, gdouble y0,
				       guint x1, gdouble y1,
				       gboolean replace_current_selection)
{
  AgsAcceleration *acceleration;

  GList *region, *list;

  GRecMutex *automation_mutex;

  if(!AGS_IS_AUTOMATION(automation)){
    return;
  }

  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);

  /* find region */
  region = ags_automation_find_region(automation,
				      x0, y0,
				      x1, y1,
				      FALSE);

  if(replace_current_selection){
    ags_automation_free_selection(automation);

    list = region;

    while(list != NULL){
      ags_acceleration_set_flags(list->data,
				 AGS_ACCELERATION_IS_SELECTED);
      g_object_ref(list->data);

      list = list->next;
    }

    /* replace */
    g_rec_mutex_lock(automation_mutex);
     
    automation->selection = region;

    g_rec_mutex_unlock(automation_mutex);
  }else{
    list = region;
    
    while(list != NULL){
      if(!ags_automation_is_acceleration_selected(automation, list->data)){
	/* add */
	ags_automation_add_acceleration(automation,
					list->data,
					TRUE);
      }
      
      list = list->next;
    }
    
    g_list_free(region);
  }
}

/**
 * ags_automation_remove_region_from_selection:
 * @automation: the #AgsAutomation
 * @x0: x start offset
 * @y0: y start acceleration
 * @x1: x end offset
 * @y1: y end acceleration
 *
 * Remove acceleration within region of selection.
 *
 * Since: 3.0.0
 */ 
void
ags_automation_remove_region_from_selection(AgsAutomation *automation,
					    guint x0, gdouble y0,
					    guint x1, gdouble y1)
{
  AgsAcceleration *acceleration;

  GList *region;
  GList *list;
  
  GRecMutex *automation_mutex;

  if(!AGS_IS_AUTOMATION(automation)){
    return;
  }

  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);

  /* find region */
  region = ags_automation_find_region(automation,
				      x0, y0,
				      x1, y1,
				      TRUE);

  list = region;
  
  while(list != NULL){
    ags_acceleration_unset_flags(list->data,
				 AGS_ACCELERATION_IS_SELECTED);

    /* remove */
    g_rec_mutex_lock(automation_mutex);

    automation->selection = g_list_remove(automation->selection,
					  list->data);

    g_rec_mutex_unlock(automation_mutex);

    g_object_unref(list->data);

    /* iterate */
    list = list->next;
  }

  g_list_free(region);
}

/**
 * ags_automation_add_all_to_selection:
 * @automation: the #AgsAutomation
 * 
 * Add all acceleration to selection.
 * 
 * Since: 3.0.0
 */
void
ags_automation_add_all_to_selection(AgsAutomation *automation)
{
  GList *list;
  
  GRecMutex *automation_mutex;

  if(!AGS_IS_AUTOMATION(automation)){
    return;
  }

  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);

  /* select all */
  g_rec_mutex_lock(automation_mutex);

  list = automation->acceleration;

  while(list != NULL){
    ags_automation_add_acceleration(automation,
				    list->data, TRUE);
    
    list = list->next;
  }

  g_rec_mutex_unlock(automation_mutex);
}

/**
 * ags_automation_copy_selection:
 * @automation: the #AgsAutomation
 *
 * Copy selection to clipboard.
 *
 * Returns: (transfer none): the selection as xmlNode
 *
 * Since: 3.0.0
 */
xmlNode*
ags_automation_copy_selection(AgsAutomation *automation)
{
  AgsTimestamp *timestamp;
  
  xmlNode *automation_node, *current_acceleration;
  xmlNode *timestamp_node;

  GList *selection;

  guint current_x;
  gdouble current_y;
  guint x_boundary;
  gdouble y_boundary;
  
  GRecMutex *automation_mutex;

  if(!AGS_IS_AUTOMATION(automation)){
    return(NULL);
  }

  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);

  /* create root node */
  g_rec_mutex_lock(automation_mutex);

  automation_node = xmlNewNode(NULL, BAD_CAST "automation");

  xmlNewProp(automation_node, BAD_CAST "program", BAD_CAST "ags");
  xmlNewProp(automation_node, BAD_CAST "type", BAD_CAST AGS_AUTOMATION_CLIPBOARD_TYPE);
  xmlNewProp(automation_node, BAD_CAST "version", BAD_CAST AGS_AUTOMATION_CLIPBOARD_VERSION);
  xmlNewProp(automation_node, BAD_CAST "format", BAD_CAST AGS_AUTOMATION_CLIPBOARD_FORMAT);
  xmlNewProp(automation_node, "control-name", automation->control_name);
  xmlNewProp(automation_node, "line", g_strdup_printf("%u", automation->line));

  /* timestamp */
  timestamp = automation->timestamp;
  
  if(timestamp != NULL){
    timestamp_node = xmlNewNode(NULL,
				BAD_CAST "timestamp");
    xmlAddChild(automation_node,
		timestamp_node);

    xmlNewProp(timestamp_node,
	       BAD_CAST "offset",
	       BAD_CAST (g_strdup_printf("%lu", ags_timestamp_get_ags_offset(timestamp))));
  }

  /* selection */
  selection = automation->selection;

  if(selection != NULL){
    g_object_get(selection->data,
		 "x", &current_x,
		 NULL);
     
    x_boundary = current_x;
    y_boundary = G_MAXDOUBLE;
  }else{
    x_boundary = 0;
    y_boundary = 0.0;
  }

  while(selection != NULL){
    g_object_get(selection->data,
		 "x", &current_x,
		 "y", &current_y,
		 NULL);

    current_acceleration = xmlNewChild(automation_node, NULL, BAD_CAST "acceleration", NULL);

    xmlNewProp(current_acceleration, BAD_CAST "x", BAD_CAST g_strdup_printf("%u", current_x));
    xmlNewProp(current_acceleration, BAD_CAST "y", BAD_CAST g_strdup_printf("%f", current_y));

    if(y_boundary > current_y){
      y_boundary = current_y;
    }

    selection = selection->next;
  }

  g_rec_mutex_unlock(automation_mutex);

  xmlNewProp(automation_node, BAD_CAST "x-boundary", BAD_CAST g_strdup_printf("%u", x_boundary));
  xmlNewProp(automation_node, BAD_CAST "y-boundary", BAD_CAST g_strdup_printf("%f", y_boundary));
  
  return(automation_node);
}

/**
 * ags_automation_cut_selection:
 * @automation: the #AgsAutomation
 *
 * Cut selection to clipboard.
 *
 * Returns: (transfer none): the selection as xmlNod
 *
 * Since: 3.0.0
 */
xmlNode*
ags_automation_cut_selection(AgsAutomation *automation)
{
  xmlNode *automation_node;
  
  GList *selection, *acceleration;
  
  GRecMutex *automation_mutex;

  if(!AGS_IS_AUTOMATION(automation)){
    return(NULL);
  }

  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);

  /* copy selection */
  automation_node = ags_automation_copy_selection(automation);

  /* cut */
  g_rec_mutex_lock(automation_mutex);

  selection = automation->selection;

  while(selection != NULL){
    automation->acceleration = g_list_remove(automation->acceleration,
					     selection->data);
    g_object_unref(selection->data);

    selection = selection->next;
  }

  g_rec_mutex_unlock(automation_mutex);

  /* free selection */
  ags_automation_free_selection(automation);

  return(automation_node);
}

void
ags_automation_insert_from_clipboard_version_0_4_3(AgsAutomation *automation,
						   xmlNode *root_node, char *version,
						   char *x_boundary, char *y_boundary,
						   gboolean from_x_offset, guint x_offset,
						   gboolean from_y_offset, guint y_offset,
						   gboolean match_line, gboolean no_duplicates,
						   guint current_line,  
						   gboolean match_timestamp)
{
  AgsAcceleration *acceleration;

  AgsTimestamp *timestamp;
    
  xmlNode *node;

  char *endptr;
  char *x, *y;

  guint64 timestamp_offset;
  guint x_boundary_val, y_boundary_val;
  guint x_val;
  gdouble y_val;
  guint base_x_difference, base_y_difference;
  gboolean subtract_x, subtract_y;

  node = root_node->children;

  /* retrieve x values for resetting */
  base_x_difference = 0;
  subtract_x = FALSE;

  if(from_x_offset){
    if(x_boundary != NULL){
      errno = 0;
      x_boundary_val = strtoul(x_boundary, &endptr, 10);

      if(errno == ERANGE){
	goto dont_reset_x_offset;
      } 

      if(x_boundary == endptr){
	goto dont_reset_x_offset;
      }

      if(x_boundary_val < x_offset){
	base_x_difference = x_offset - x_boundary_val;
	subtract_x = FALSE;
      }else{
	base_x_difference = x_boundary_val - x_offset;
	subtract_x = TRUE;
      }
    }else{
    dont_reset_x_offset:
      from_x_offset = FALSE;
    }
  }

  /* retrieve y values for resetting */
  base_y_difference = 0;
  subtract_y = FALSE;
    
  if(from_y_offset){
    if(y_boundary != NULL){
      errno = 0;
      y_boundary_val = strtoul(y_boundary, &endptr, 10);

      if(errno == ERANGE){
	goto dont_reset_y_offset;
      } 

      if(y_boundary == endptr){
	goto dont_reset_y_offset;
      }

      if(y_boundary_val < y_offset){
	base_y_difference = y_offset - y_boundary_val;
	subtract_y = FALSE;
      }else{
	base_y_difference = y_boundary_val - y_offset;
	subtract_y = TRUE;
      }
    }else{
    dont_reset_y_offset:
      from_y_offset = FALSE;
    }
  }
    
  for(; node != NULL; node = node->next){
    if(node->type == XML_ELEMENT_NODE && !xmlStrncmp("acceleration", node->name, 13)){
      /* retrieve x0 offset */
      x = xmlGetProp(node, "x");

      if(x == NULL){
	continue;
      }

      errno = 0;
      x_val = strtoul(x, &endptr, 10);

      if(errno == ERANGE){
	continue;
      } 

      if(x == endptr){
	continue;
      }
	
      /* retrieve y offset */
      y = xmlGetProp(node, "y");

      if(y == NULL){
	continue;
      }

      errno = 0;
      y_val = strtod(y,
		     &endptr);

      if(errno == ERANGE){
	continue;
      } 

      if(y == endptr){
	continue;
      }

      /* calculate new offset */
      if(from_x_offset){
	errno = 0;

	if(subtract_x){
	  x_val -= base_x_difference;

	  if(errno != 0){
	    continue;
	  }
	}else{
	  x_val += base_x_difference;

	  if(errno != 0){
	    continue;
	  }
	}
      }

      if(from_y_offset){
	errno = 0;

	if(subtract_y){
	  y_val -= base_y_difference;
	}else{
	  y_val += base_y_difference;
	}

	if(errno != 0){
	  continue;
	}
      }

      /* check duplicate */
      if(no_duplicates &&
	 ags_automation_find_point(automation,
				   x_val, y_val,
				   FALSE) != NULL){
	continue;
      }
	
      /* add acceleration */
      g_object_get(automation,
		   "timestamp", &timestamp,
		   NULL);
	
      timestamp_offset = ags_timestamp_get_ags_offset(timestamp);
      g_object_unref(timestamp);
	
      if(!match_timestamp ||
	 (x_val >= timestamp_offset &&
	  x_val < timestamp_offset + AGS_AUTOMATION_DEFAULT_OFFSET)){
	acceleration = ags_acceleration_new();
 	  
	acceleration->x = x_val;
	acceleration->y = y_val;

#ifdef AGS_DEBUG
	g_message("adding acceleration at: [%u|%f]\n", x_val, y_val);
#endif
	  
	ags_automation_add_acceleration(automation,
					acceleration,
					FALSE);
      }
    }
  }
}

void
ags_automation_insert_native_scale_from_clipboard(AgsAutomation *automation,
						  xmlNode *root_node, char *version,
						  char *x_boundary, char *y_boundary,
						  gboolean from_x_offset, guint x_offset,
						  gboolean from_y_offset, guint y_offset,
						  gboolean match_line, gboolean no_duplicates)
{
  guint current_line;
  
  gboolean match_timestamp;  
  
  if(!AGS_IS_AUTOMATION(automation)){
    return;
  }  

  g_object_get(automation,
	       "line", &current_line,
	       NULL);
  
  match_timestamp = TRUE;
  
  if(!xmlStrncmp("0.4.3", version, 6)){
    ags_automation_insert_from_clipboard_version_0_4_3(automation,
						       root_node, version,
						       x_boundary, y_boundary,
						       from_x_offset, x_offset,
						       from_y_offset, y_offset,
						       match_line, no_duplicates,
						       current_line,  
						       match_timestamp);
  }else if(!xmlStrncmp("1.3.0", version, 6)){
    match_timestamp = TRUE;
    
    if(match_line &&
       current_line != g_ascii_strtoull(xmlGetProp(root_node,
						   "line"),
					NULL,
					10)){
      return;
    }

    ags_automation_insert_from_clipboard_version_0_4_3(automation,
						       root_node, version,
						       x_boundary, y_boundary,
						       from_x_offset, x_offset,
						       from_y_offset, y_offset,
						       match_line, no_duplicates,
						       current_line,  
						       match_timestamp);
  }
}

/**
 * ags_automation_insert_from_clipboard:
 * @automation: the #AgsAutomation
 * @automation_node: the xmlNode
 * @reset_x_offset: if %TRUE reset x offset
 * @x_offset: the x offset to use
 * @reset_y_offset: if %TRUE reset y offset
 * @y_offset: the y offset to use
 * 
 * Insert clipboard @automation_node to @automation.
 * 
 * Since: 3.0.0
 */
void
ags_automation_insert_from_clipboard(AgsAutomation *automation,
				     xmlNode *automation_node,
				     gboolean reset_x_offset, guint x_offset,
				     gboolean reset_y_offset, gdouble y_offset)
{
  ags_automation_insert_from_clipboard_extended(automation,
						automation_node,
						reset_x_offset, x_offset,
						reset_y_offset, y_offset,
						FALSE, FALSE);
}

/**
 * ags_automation_insert_from_clipboard_extended:
 * @automation: the #AgsAutomation
 * @automation_node: the xmlNode
 * @reset_x_offset: if %TRUE reset x offset
 * @x_offset: the x offset to use
 * @reset_y_offset: if %TRUE reset y offset
 * @y_offset: the y offset to use
 * @match_line: if %TRUE match line or discard
 * @no_duplicates: if %TRUE eliminate duplicates
 * 
 * Insert clipboard @automation_node to @automation.
 * 
 * Since: 3.0.0
 */
void
ags_automation_insert_from_clipboard_extended(AgsAutomation *automation,
					      xmlNode *automation_node,
					      gboolean reset_x_offset, guint x_offset,
					      gboolean reset_y_offset, gdouble y_offset,
					      gboolean match_line, gboolean no_duplicates)
{
  char *program, *version, *type, *format;
  char *base_frequency;
  char *x_boundary, *y_boundary;

  if(!AGS_IS_AUTOMATION(automation)){
    return;
  }
  
  while(automation_node != NULL){
    if(automation_node->type == XML_ELEMENT_NODE && !xmlStrncmp("automation", automation_node->name, 11)){
      break;
    }

    automation_node = automation_node->next;
  }

  if(automation_node != NULL){
    program = xmlGetProp(automation_node, "program");

    if(!xmlStrncmp("ags", program, 4)){
      version = xmlGetProp(automation_node, "version");
      type = xmlGetProp(automation_node, "type");
      format = xmlGetProp(automation_node, "format");

      if(!xmlStrcmp(AGS_AUTOMATION_CLIPBOARD_FORMAT,
		    format) ||
	 !xmlStrcmp(AGS_AUTOMATION_CLIPBOARD_LEGACY_FORMAT,
		    format)){
	x_boundary = xmlGetProp(automation_node, "x-boundary");
	y_boundary = xmlGetProp(automation_node, "y-boundary");

	ags_automation_insert_native_scale_from_clipboard(automation,
							  automation_node, version,
							  x_boundary, y_boundary,
							  reset_x_offset, x_offset,
							  reset_y_offset, y_offset,
							  match_line, no_duplicates);
      }
    }
  }
}

/**
 * ags_automation_get_specifier_unique:
 * @automation: (element-type AgsAudio.Automation) (transfer none): the #GList-struct containing #AgsAutomation
 *
 * Retrieve automation port specifier.
 *
 * Returns: (element-type utf8) (array zero-terminated=1) (transfer full): a %NULL terminated string array
 *
 * Since: 3.0.0
 */
gchar**
ags_automation_get_specifier_unique(GList *automation)
{
  AgsAutomation *current_automation;
  
  gchar **specifier;
  gchar *current_control_name;
  
  guint length, i;
  gboolean contains_control_name;

  GRecMutex *automation_mutex;

  if(automation == NULL){
    return(NULL);
  }
  
  specifier = (gchar **) malloc(sizeof(gchar*));
  specifier[0] = NULL;
  length = 1;
  
  while(automation != NULL){
    current_automation = automation->data;
    
    /* get automation mutex */
    automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(current_automation);

    /* duplicate control name */
    g_rec_mutex_lock(automation_mutex);

    current_control_name = g_strdup(current_automation->control_name);
    
    g_rec_mutex_unlock(automation_mutex);
    
#ifdef HAVE_GLIB_2_44
    contains_control_name = g_strv_contains(specifier,
					    current_control_name);
#else
    contains_control_name = ags_strv_contains(specifier,
					      current_control_name);
#endif
    
    if(!contains_control_name){
      specifier = (gchar **) realloc(specifier,
				     (length + 1) * sizeof(gchar *));
      specifier[length - 1] = current_control_name;
      specifier[length] = NULL;

      length++;
    }else{
      g_free(current_control_name);
    }

    /* iterate */
    automation = automation->next;
  }
  
  return(specifier);
}

/**
 * ags_automation_get_specifier_unique_with_channel_type:
 * @automation: (element-type AgsAudio.Automation) (transfer none): the #GList-struct containing #AgsAutomation
 * @channel_type: the channel's #GType
 *
 * Retrieve automation port specifier.
 *
 * Returns: (element-type utf8) (array zero-terminated=1) (transfer full): a %NULL terminated string array
 *
 * Since: 3.0.0
 */
gchar**
ags_automation_get_specifier_unique_with_channel_type(GList *automation,
						      GType channel_type)
{
  AgsAutomation *current_automation;  

  GType current_channel_type;

  gchar **specifier;
  gchar *current_control_name;

  guint length, i;
  gboolean contains_control_name;

  GRecMutex *automation_mutex;

  if(automation == NULL){
    return(NULL);
  }
    
  specifier = (gchar **) malloc(sizeof(gchar*));
  specifier[0] = NULL;
  length = 1;
  
  while(automation != NULL){
    current_automation = automation->data;
    
    /* get automation mutex */
    automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(current_automation);

    /* get channel type */
    g_rec_mutex_lock(automation_mutex);

    current_channel_type = current_automation->channel_type;

    g_rec_mutex_unlock(automation_mutex);
    
    if(current_channel_type != channel_type){
      automation = automation->next;

      continue;
    }

    /* duplicate control name */
    g_rec_mutex_lock(automation_mutex);

    current_control_name = g_strdup(current_automation->control_name);
    
    g_rec_mutex_unlock(automation_mutex);
    
#ifdef HAVE_GLIB_2_44
    contains_control_name = g_strv_contains(specifier,
					    current_control_name);
#else
    contains_control_name = ags_strv_contains(specifier,
					      current_control_name);
#endif
    
    if(!contains_control_name){
      specifier = (gchar **) realloc(specifier,
				     (length + 1) * sizeof(gchar *));
      specifier[length - 1] = current_control_name;
      specifier[length] = NULL;

      length++;
    }else{
      g_free(current_control_name);
    }

    /* iterate */
    automation = automation->next;
  }
    
  return(specifier);
}

/**
 * ags_automation_find_specifier:
 * @automation: (element-type AgsAudio.Automation) (transfer none): the #GList-struct containing #AgsAutomation
 * @specifier: the string specifier to find
 *
 * Find port specifier.
 *
 * Returns: (element-type AgsAudio.Automation) (transfer none): Next matching #GList
 *
 * Since: 3.0.0
 */
GList*
ags_automation_find_specifier(GList *automation,
			      gchar *specifier)
{
  AgsAutomation *current_automation;
  
  gchar *current_control_name;

  gboolean success;
  
  GRecMutex *automation_mutex;
 
  while(automation != NULL){
    current_automation = automation->data;

    /* get automation mutex */
    automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(current_automation);

    /* duplicate control name */
    g_rec_mutex_lock(automation_mutex);

    current_control_name = g_strdup(current_automation->control_name);
    
    g_rec_mutex_unlock(automation_mutex);

    /* check control name */
    success = (!g_ascii_strcasecmp(current_control_name,
				   specifier)) ? TRUE: FALSE;
    g_free(current_control_name);
    
    if(success){
      break;
    }

    automation = automation->next;
  }

  return(automation);
}

/**
 * ags_automation_find_channel_type_with_control_name:
 * @automation: (element-type AgsAudio.Automation) (transfer none): the #GList-struct containing #AgsAutomation
 * @channel_type: the #GType to match
 * @specifier: the control name
 * 
 * Find automation by @channel_type.
 * 
 * Returns: (element-type AgsAudio.Automation) (transfer none): next matching automation as #GList-struct or %NULL if not found
 * 
 * Since: 3.0.0
 */
GList*
ags_automation_find_channel_type_with_control_name(GList *automation,
						   GType channel_type, gchar *specifier)
{
  AgsAutomation *current_automation;

  GType current_channel_type;
  
  gchar *current_control_name;
 
  gboolean success;
  
  GRecMutex *automation_mutex;

  if(automation == NULL){
    return(NULL);
  }

  while(automation != NULL){
    current_automation = automation->data;
    
    /* get automation mutex */
    automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(current_automation);

    /* duplicate control name */
    g_rec_mutex_lock(automation_mutex);

    current_channel_type = current_automation->channel_type;
    
    current_control_name = g_strdup(current_automation->control_name);
    
    g_rec_mutex_unlock(automation_mutex);

    /* check channel type and control name */
    success = (current_channel_type == channel_type &&
	       !g_strcmp0(current_control_name,
			  specifier)) ? TRUE: FALSE;

    g_free(current_control_name);
    
    if(success){
      break;
    }
    
    automation = automation->next;
  }

  return(automation);
}

/**
 * ags_automation_find_specifier_with_type_and_line:
 * @automation: (element-type AgsAudio.Automation) (transfer none): the #GList-struct containing #AgsAutomation
 * @specifier: the string specifier to find
 * @channel_type: the channel #GType
 * @line: the line
 *
 * Find port specifier with channel type and line.
 *
 * Returns: (element-type AgsAudio.Automation) (transfer none): Next matching #GList-struct
 *
 * Since: 3.0.0
 */
GList*
ags_automation_find_specifier_with_type_and_line(GList *automation,
						 gchar *specifier,
						 GType channel_type,
						 guint line)
{
  AgsAutomation *current_automation;
  
  GType current_channel_type;

  gchar *current_control_name;

  guint current_line;
  gboolean success;
  
  GRecMutex *automation_mutex;

  if(automation == NULL){
    return(NULL);
  }
  
  while(automation != NULL){
    current_automation = automation->data;
    
    /* get automation mutex */
    automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(current_automation);

    /* duplicate control name */
    g_rec_mutex_lock(automation_mutex);

    current_channel_type = current_automation->channel_type;
    current_line = current_automation->line;    
    
    current_control_name = g_strdup(current_automation->control_name);
    
    g_rec_mutex_unlock(automation_mutex);

    /* check channel type, line and control name */
    success = (!g_ascii_strcasecmp(current_control_name,
				   specifier) &&
	       current_channel_type == channel_type &&
	       current_line == line) ? TRUE: FALSE;

    g_free(current_control_name);

    if(success){
      break;
    }

    automation = automation->next;
  }

  return(automation);
}

/**
 * ags_automation_get_value:
 * @automation: the #AgsAutomation
 * @x: the x-offset
 * @x_end: the x-end-offset
 * @use_prev_on_failure: if %TRUE use previous value, else return G_MAXUINT
 * @value: (out caller-allocates): the return location of value
 *
 * Get automation value.
 *
 * Returns: the x_offset
 *
 * Since: 3.0.0
 */
guint
ags_automation_get_value(AgsAutomation *automation,
			 guint x, guint x_end,
			 gboolean use_prev_on_failure,
			 GValue *value)
{
  AgsAcceleration *matching_acceleration;
  AgsPort *port;

  GType port_value_type;
  
  GList *acceleration;
  GList *next, *prev;
  GList *current_start, *current_end, *current;

  guint current_start_x, current_end_x, current_x;
  guint length, position;
  gboolean success;
  guint ret_x;
  gdouble default_value;
  gdouble y;
  gboolean port_value_is_pointer;
  
  GRecMutex *automation_mutex;

  if(!AGS_IS_AUTOMATION(automation)){
    return(G_MAXUINT);
  }

  /* get automation mutex */
  automation_mutex = AGS_AUTOMATION_GET_OBJ_MUTEX(automation);

  /*  */
  g_rec_mutex_lock(automation_mutex);

  port = (AgsPort *) automation->port;
  acceleration = automation->acceleration;

  current_start = acceleration;
  current_end = g_list_last(acceleration);
  
  length = g_list_length(acceleration);
  position = (length - 1) / 2;

  current = g_list_nth(current_start,
		       position);
  
  if(acceleration == NULL){
    g_rec_mutex_unlock(automation_mutex);

    return(G_MAXUINT);
  }

  matching_acceleration = NULL;

  ret_x = 0;  
  success = FALSE;
  
  while(!success && current != NULL){
    g_object_get(current_start->data,
		 "x", &current_start_x,
		 NULL);

    if(current_start_x > x){
      break;
    }
    
    if(current_start_x >= x &&
       current_start_x < x_end){
      matching_acceleration = current_start->data;

      break;
    }
    
    g_object_get(current_end->data,
		 "x", &current_end_x,
		 NULL);

    if(current_end_x < x){
      break;
    }

    if(current_end_x >= x &&
       current_end_x < x_end){
      matching_acceleration = current_end->data;

      break;
    }

    g_object_get(current->data,
		 "x", &current_x,
		 NULL);
    
    if(current_x >= x &&
       current_x < x_end){
      matching_acceleration = current->data;
      
      break;
    }

    if(length <= 3){
      break;
    }

    if(current_x < x){
      current_start = current->next;
      current_end = current_end->prev;
    }else if(current_x > x){
      current_start = current_start->next;
      current_end = current->prev;
    }else{
      current_start = current_start->next;
      //NOTE:JK: we want progression
      //current_end = current_end->prev;
    }

    length = g_list_position(current_start,
			     current_end) + 1;
    position = (length - 1) / 2;

    current = g_list_nth(current_start,
			 position);
  }

  if(matching_acceleration != NULL){
    guint tmp_x;

    next = NULL;
    
    if(current_start->data == matching_acceleration){
      next = current_start->next;

      ret_x = current_start_x;
    }else if(current_end->data == matching_acceleration){
      next = current_end->next;

      ret_x = current_end_x;
    }else if(current->data == matching_acceleration){
      next = current->next;

      ret_x = current_x;
    }

    while(next != NULL){
      g_object_get(next->data,
		   "x", &tmp_x,
		   NULL);

      if(tmp_x > x_end){
	break;
      }

      matching_acceleration = next->data;
      
      next = next->next;
    }
  }else{
    if(use_prev_on_failure){
      guint tmp_x;

      prev = current_end;

      while(prev != NULL){
	g_object_get(prev->data,
		     "x", &tmp_x,
		     NULL);

      
	if(tmp_x < x){	
	  matching_acceleration = prev->data;

	  break;
	}
      
	prev = prev->prev;
      }
    }
  }
  
  g_rec_mutex_unlock(automation_mutex);
  
  if(matching_acceleration == NULL){
    return(G_MAXUINT);
  }

  /* apply port */
  g_object_get(automation,
	       "default-value", &default_value,
	       NULL);
  
  g_object_get(port,
	       "port-value-is-pointer", &port_value_is_pointer,
	       "port-value-type", &port_value_type,
	       NULL);
  
  g_object_get(matching_acceleration,
	       "y", &y,
	       NULL);

  if(!port_value_is_pointer){
    if(port_value_type == G_TYPE_BOOLEAN){
      gboolean current;

      current = FALSE;

      if(acceleration == NULL){
	if(default_value != 0.0){
	  current = TRUE;
	}
      }else{
	if(y != 0.0){
	  current = TRUE;
	}
      }
      
      g_value_init(value,
		   G_TYPE_BOOLEAN);
      g_value_set_boolean(value,
			  current);
    }else if(port_value_type == G_TYPE_INT64){
      gint64 current;
      
      current = floor(y);

      g_value_init(value,
		   G_TYPE_INT64);
      g_value_set_int64(value,
			current);
    }else if(port_value_type == G_TYPE_UINT64){
      guint64 current;

      current = floor(y);
      
      g_value_init(value,
		   G_TYPE_UINT64);
      g_value_set_uint64(value,
			 current);
    }else if(port_value_type == G_TYPE_FLOAT){
      gfloat current;
	
      current = y;
      
      g_value_init(value,
		   G_TYPE_FLOAT);
      g_value_set_float(value,
			current);
    }else if(port_value_type == G_TYPE_DOUBLE){
      gdouble current;

      current = y;
      
      g_value_init(value,
		   G_TYPE_DOUBLE);
      g_value_set_double(value,
			 current);
    }else if(port_value_type == G_TYPE_POINTER){
      g_warning("ags_automation.c - unsupported value type pointer");
    }else if(port_value_type == G_TYPE_OBJECT){
      g_warning("ags_automation.c - unsupported value type object");
    }else{
      g_warning("ags_automation.c - unknown type");
    }
  }else{
    g_warning("ags_automation.c - unsupported value type pointer");
  }

  return(ret_x);
}

/**
 * ags_automation_new:
 * @audio: the #AgsAudio
 * @line: the line to apply
 * @channel_type: the channel type
 * @control_name: the control name
 *
 * Creates a new instance of #AgsAutomation.
 *
 * Returns: the new #AgsAutomation
 *
 * Since: 3.0.0
 */
AgsAutomation*
ags_automation_new(GObject *audio,
		   guint line,
		   GType channel_type,
		   gchar *control_name)
{
  AgsAutomation *automation;

  automation = (AgsAutomation *) g_object_new(AGS_TYPE_AUTOMATION,
					      "audio", audio,
					      "line", line,
					      "channel-type", channel_type,
					      "control-name", control_name,
					      NULL);

  return(automation);
}
