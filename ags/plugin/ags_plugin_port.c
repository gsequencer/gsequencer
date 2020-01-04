/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

void ags_plugin_port_class_init(AgsPluginPortClass *plugin_port);
void ags_plugin_port_init(AgsPluginPort *plugin_port);
void ags_plugin_port_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_plugin_port_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_plugin_port_dispose(GObject *gobject);
void ags_plugin_port_finalize(GObject *gobject);

/**
 * SECTION:ags_plugin_port
 * @short_description: The plugin port class
 * @title: AgsPluginPort
 * @section_id:
 * @include: ags/plugin/ags_plugin_port.h
 *
 * The #AgsPluginPort describes a plugin's port. For real ports used by the
 * processing tree please take a look at #AgsPort.
 */

enum{
  PROP_0,
  PROP_PORT_INDEX,
  PROP_PORT_NAME,
  PROP_PORT_SYMBOL,
  PROP_SCALE_STEPS,
  PROP_SCALE_POINT,
  PROP_SCALE_VALUE,
  PROP_LOWER_VALUE,
  PROP_UPPER_VALUE,
  PROP_DEFAULT_VALUE,
};

static gpointer ags_plugin_port_parent_class = NULL;

GType
ags_plugin_port_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_plugin_port = 0;

    static const GTypeInfo ags_plugin_port_info = {
      sizeof(AgsPluginPortClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_plugin_port_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsPluginPort),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_plugin_port_init,
    };

    ags_type_plugin_port = g_type_register_static(G_TYPE_OBJECT,
						  "AgsPluginPort",
						  &ags_plugin_port_info,
						  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_plugin_port);
  }

  return g_define_type_id__volatile;
}

void
ags_plugin_port_class_init(AgsPluginPortClass *plugin_port)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_plugin_port_parent_class = g_type_class_peek_parent(plugin_port);

  /* GObjectClass */
  gobject = (GObjectClass *) plugin_port;

  gobject->set_property = ags_plugin_port_set_property;
  gobject->get_property = ags_plugin_port_get_property;

  gobject->dispose = ags_plugin_port_dispose;
  gobject->finalize = ags_plugin_port_finalize;

  /* properties */
  /**
   * AgsPluginPort:port-index:
   *
   * The assigned port-index.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("port-index",
				 i18n_pspec("port index of the plugin"),
				 i18n_pspec("The port's index of the plugin"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT_INDEX,
				  param_spec);
  
  /**
   * AgsPluginPort:port-name:
   *
   * The port's name.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("port-name",
				   i18n_pspec("name of the port"),
				   i18n_pspec("The port's name"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT_NAME,
				  param_spec);
  
  /**
   * AgsPluginPort:port-symbol:
   *
   * The port's symbol.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("port-symbol",
				   i18n_pspec("symbol of the port"),
				   i18n_pspec("The port's symbol"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT_SYMBOL,
				  param_spec);

  /**
   * AgsPluginPort:scale-steps:
   *
   * The number of scale steps.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_int("scale-steps",
				i18n_pspec("port index of the plugin"),
				i18n_pspec("The port's index of the plugin"),
				-1,
				G_MAXINT32,
				0,
				G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SCALE_STEPS,
				  param_spec);
  
  /**
   * AgsPluginPort:scale-point:
   *
   * The scale point string vector.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("scale-point",
				    i18n_pspec("string vector of scale points"),
				    i18n_pspec("The string vector of scale points"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_SCALE_POINT,
				  param_spec);

  /**
   * AgsPluginPort:scale-value:
   *
   * The scale value array.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("scale-value",
				    i18n_pspec("array of scale values"),
				    i18n_pspec("The array of scale values"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_SCALE_VALUE,
				  param_spec);

  /**
   * AgsPluginPort:lower-value:
   *
   * The lower value.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("lower-value",
				    i18n_pspec("lower value"),
				    i18n_pspec("The lower value"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOWER_VALUE,
				  param_spec);

  /**
   * AgsPluginPort:upper-value:
   *
   * The upper value.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("upper-value",
				    i18n_pspec("upper value"),
				    i18n_pspec("The upper value"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_UPPER_VALUE,
				  param_spec);

  /**
   * AgsPluginPort:default-value:
   *
   * The default value.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("default-value",
				    i18n_pspec("default value"),
				    i18n_pspec("The default value"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEFAULT_VALUE,
				  param_spec);
}

void
ags_plugin_port_init(AgsPluginPort *plugin_port)
{
  plugin_port->flags = 0;

  /* add base plugin mutex */
  g_rec_mutex_init(&(plugin_port->obj_mutex));

  plugin_port->port_index = 0;

  plugin_port->port_name = NULL;
  plugin_port->port_symbol = NULL;

  plugin_port->scale_steps = -1;
  plugin_port->scale_point = NULL;
  plugin_port->scale_value = NULL;
  
  plugin_port->lower_value = g_new0(GValue,
				    1);
  plugin_port->upper_value = g_new0(GValue,
				    1);

  plugin_port->default_value = g_new0(GValue,
				      1);

  plugin_port->user_data = NULL;
}

void
ags_plugin_port_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsPluginPort *plugin_port;

  GRecMutex *plugin_port_mutex;

  plugin_port = AGS_PLUGIN_PORT(gobject);

  /* get plugin port mutex */
  plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(plugin_port);

  switch(prop_id){
  case PROP_PORT_INDEX:
    {
      g_rec_mutex_lock(plugin_port_mutex);

      plugin_port->port_index = g_value_get_uint(value);
      
      g_rec_mutex_unlock(plugin_port_mutex);
    }
    break;
  case PROP_PORT_NAME:
    {
      gchar *port_name;

      port_name = g_value_get_string(value);

      g_rec_mutex_lock(plugin_port_mutex);

      if(port_name == plugin_port->port_name){
	g_rec_mutex_unlock(plugin_port_mutex);
	
	return;
      }      

      plugin_port->port_name = g_strdup(port_name);
      
      g_rec_mutex_unlock(plugin_port_mutex);
    }
    break;
  case PROP_PORT_SYMBOL:
    {
      gchar *port_symbol;

      port_symbol = g_value_get_string(value);

      g_rec_mutex_lock(plugin_port_mutex);

      if(port_symbol == plugin_port->port_symbol){
	g_rec_mutex_unlock(plugin_port_mutex);
	
	return;
      }      

      plugin_port->port_symbol = g_strdup(port_symbol);
      
      g_rec_mutex_unlock(plugin_port_mutex);
    }
    break;
  case PROP_SCALE_STEPS:
    {
      gint scale_steps;
      guint i;
      
      scale_steps = g_value_get_int(value);

      g_rec_mutex_lock(plugin_port_mutex);

      if(scale_steps == plugin_port->scale_steps){
	g_rec_mutex_unlock(plugin_port_mutex);

	return;
      }

      if(scale_steps > 0){
	/* scale point */
	if(plugin_port->scale_point == NULL){
	  plugin_port->scale_point = (gchar **) malloc((scale_steps + 1) * sizeof(gchar *));
	}else{
	  plugin_port->scale_point = (gchar **) realloc(plugin_port->scale_point,
							(scale_steps + 1) * sizeof(gchar *));	  
	}

	for(i = scale_steps; i < plugin_port->scale_steps; i++){
	  plugin_port->scale_point[i] = NULL;
	}

	plugin_port->scale_point[scale_steps] = NULL;

	/* scale value */
	if(plugin_port->scale_value == NULL){
	  plugin_port->scale_value = (gdouble *) malloc(scale_steps * sizeof(gdouble));	  
	}else{
	  plugin_port->scale_value = (gdouble *) realloc(plugin_port->scale_value,
							 scale_steps * sizeof(gdouble));	  	  
	}

	for(i = scale_steps; i < plugin_port->scale_steps; i++){
	  plugin_port->scale_value[i] = 0.0;
	}
      }else{
	g_free(plugin_port->scale_point);
	g_free(plugin_port->scale_value);

	plugin_port->scale_point = NULL;
	plugin_port->scale_value = NULL;
      }

      plugin_port->scale_steps = scale_steps;
      
      g_rec_mutex_unlock(plugin_port_mutex);
    }
    break;
  case PROP_LOWER_VALUE:
    {
      GValue *lower_value;

      lower_value = (GValue *) g_value_get_pointer(value);

      g_rec_mutex_lock(plugin_port_mutex);

      g_value_copy(plugin_port->lower_value,
		   lower_value);
      
      g_rec_mutex_unlock(plugin_port_mutex);      
    }
    break;
  case PROP_UPPER_VALUE:
    {
      GValue *upper_value;

      upper_value = (GValue *) g_value_get_pointer(value);

      g_rec_mutex_lock(plugin_port_mutex);

      g_value_copy(plugin_port->upper_value,
		   upper_value);
      
      g_rec_mutex_unlock(plugin_port_mutex);
    }
    break;
  case PROP_DEFAULT_VALUE:
    {
      GValue *default_value;

      default_value = (GValue *) g_value_get_pointer(value);

      g_rec_mutex_lock(plugin_port_mutex);

      g_value_copy(plugin_port->default_value,
		   default_value);
      
      g_rec_mutex_unlock(plugin_port_mutex);
    }
    break;  
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_plugin_port_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsPluginPort *plugin_port;

  GRecMutex *plugin_port_mutex;

  plugin_port = AGS_PLUGIN_PORT(gobject);

  /* get plugin port mutex */
  plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(plugin_port);

  switch(prop_id){
  case PROP_PORT_INDEX:
    {
      g_rec_mutex_lock(plugin_port_mutex);

      g_value_set_uint(value,
		       plugin_port->port_index);
      
      g_rec_mutex_unlock(plugin_port_mutex);
    }
    break;
  case PROP_PORT_NAME:
    {
      g_rec_mutex_lock(plugin_port_mutex);

      g_value_set_string(value,
			 plugin_port->port_name);
      
      g_rec_mutex_unlock(plugin_port_mutex);
    }
    break;
  case PROP_PORT_SYMBOL:
    {
      g_rec_mutex_lock(plugin_port_mutex);

      g_value_set_string(value,
			 plugin_port->port_symbol);
      
      g_rec_mutex_unlock(plugin_port_mutex);
    }
    break;
  case PROP_SCALE_STEPS:
    {
      g_rec_mutex_lock(plugin_port_mutex);

      g_value_set_int(value,
		      plugin_port->scale_steps);
      
      g_rec_mutex_unlock(plugin_port_mutex);
    }
    break;
  case PROP_SCALE_POINT:
    {
      g_rec_mutex_lock(plugin_port_mutex);

      g_value_set_pointer(value,
			  plugin_port->scale_point);
      
      g_rec_mutex_unlock(plugin_port_mutex);
    }
    break;
  case PROP_SCALE_VALUE:
    {      
      g_rec_mutex_lock(plugin_port_mutex);

      g_value_set_pointer(value,
			  plugin_port->scale_value);
      
      g_rec_mutex_unlock(plugin_port_mutex);
    }
    break;
  case PROP_LOWER_VALUE:
    {
      g_rec_mutex_lock(plugin_port_mutex);

      g_value_set_pointer(value,
			  plugin_port->lower_value);
      
      g_rec_mutex_unlock(plugin_port_mutex);
    }
    break;
  case PROP_UPPER_VALUE:
    {
      g_rec_mutex_lock(plugin_port_mutex);

      g_value_set_pointer(value,
			  plugin_port->upper_value);
      
      g_rec_mutex_unlock(plugin_port_mutex);
    }
    break;
  case PROP_DEFAULT_VALUE:
    {
      g_rec_mutex_lock(plugin_port_mutex);

      g_value_set_pointer(value,
			  plugin_port->default_value);
      
      g_rec_mutex_unlock(plugin_port_mutex);
    }
    break;  
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_plugin_port_dispose(GObject *gobject)
{
  AgsPluginPort *plugin_port;

  plugin_port = AGS_PLUGIN_PORT(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_plugin_port_parent_class)->dispose(gobject);
}

void
ags_plugin_port_finalize(GObject *gobject)
{
  AgsPluginPort *plugin_port;

  plugin_port = AGS_PLUGIN_PORT(gobject);
  
  if(plugin_port->port_name != NULL){
    g_free(plugin_port->port_name);
  }
  
  if(plugin_port->port_symbol != NULL){
    g_free(plugin_port->port_symbol);
  }

  if(plugin_port->scale_point != NULL){
    g_free(plugin_port->scale_point);
  }

  g_free(plugin_port->lower_value);
  g_free(plugin_port->upper_value);

  g_free(plugin_port->default_value);

  /* call parent */
  G_OBJECT_CLASS(ags_plugin_port_parent_class)->finalize(gobject);
}

/**
 * ags_plugin_port_test_flags:
 * @plugin_port: the #AgsPluginPort
 * @flags: the flags
 * 
 * Test @flags to be set on @plugin_port.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_plugin_port_test_flags(AgsPluginPort *plugin_port, guint flags)
{
  gboolean retval;
  
  GRecMutex *plugin_port_mutex;

  if(!AGS_IS_PLUGIN_PORT(plugin_port)){
    return(FALSE);
  }
      
  /* get plugin_port mutex */
  plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(plugin_port);

  /* test */
  g_rec_mutex_lock(plugin_port_mutex);

  retval = (flags & (plugin_port->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(plugin_port_mutex);

  return(retval);
}

/**
 * ags_plugin_port_set_flags:
 * @plugin_port: the #AgsPluginPort
 * @flags: the flags
 * 
 * Set @flags on @plugin_port.
 * 
 * Since: 3.0.0
 */
void
ags_plugin_port_set_flags(AgsPluginPort *plugin_port, guint flags)
{
  GRecMutex *plugin_port_mutex;

  if(!AGS_IS_PLUGIN_PORT(plugin_port)){
    return;
  }
      
  /* get plugin_port mutex */
  plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(plugin_port);

  /* set */
  g_rec_mutex_lock(plugin_port_mutex);

  plugin_port->flags |= flags;
  
  g_rec_mutex_unlock(plugin_port_mutex);
}

/**
 * ags_plugin_port_unset_flags:
 * @plugin_port: the #AgsPluginPort
 * @flags: the flags
 * 
 * Unset @flags on @plugin_port.
 * 
 * Since: 3.0.0
 */
void
ags_plugin_port_unset_flags(AgsPluginPort *plugin_port, guint flags)
{
  GRecMutex *plugin_port_mutex;

  if(!AGS_IS_PLUGIN_PORT(plugin_port)){
    return;
  }
      
  /* get plugin_port mutex */
  plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(plugin_port);

  /* unset */
  g_rec_mutex_lock(plugin_port_mutex);

  plugin_port->flags &= (~flags);
  
  g_rec_mutex_unlock(plugin_port_mutex);
}

/**
 * ags_plugin_port_find_symbol:
 * @plugin_port: (element-type AgsAudio.PluginPort) (transfer none): the #GList-struct containing #AgsPluginPort
 * @port_symbol: the port symbol
 * 
 * Find @port_symbol within @plugin_port.
 * 
 * Returns: (element-type AgsAudio.PluginPort) (transfer none): the matching #GList-struct containing #AgsPluginPort
 * 
 * Since: 3.0.0
 */
GList*
ags_plugin_port_find_symbol(GList *plugin_port,
			    gchar *port_symbol)
{
  AgsPluginPort *current_plugin_port;

  gboolean success;
  
  GRecMutex *plugin_port_mutex;

  if(port_symbol == NULL){
    return(NULL);
  }

  while(plugin_port != NULL){
    current_plugin_port = AGS_PLUGIN_PORT(plugin_port->data);
    
    /* get plugin port mutex */
    plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(current_plugin_port);
    
    /* check port symbol */
    g_rec_mutex_lock(plugin_port_mutex);

    success = (!g_strcmp0(port_symbol,
			  current_plugin_port->port_symbol)) ? TRUE: FALSE;
    
    g_rec_mutex_unlock(plugin_port_mutex);

    if(success){
      return(plugin_port);
    }
    
    plugin_port = plugin_port->next;
  }
  
  return(NULL);
}

/**
 * ags_plugin_port_find_port_index:
 * @plugin_port: (element-type AgsAudio.PluginPort) (transfer none): the #GList-struct containing #AgsPluginPort
 * @port_index: the port index
 * 
 * Find @port_index within @plugin_port.
 * 
 * Returns: (element-type AgsAudio.PluginPort) (transfer none): the matching #GList-struct containing #AgsPluginPort
 * 
 * Since: 3.0.0
 */
GList*
ags_plugin_port_find_port_index(GList *plugin_port,
				guint port_index)
{
  AgsPluginPort *current_plugin_port;

  gboolean success;
  
  GRecMutex *plugin_port_mutex;

  while(plugin_port != NULL){
    current_plugin_port = AGS_PLUGIN_PORT(plugin_port->data);
    
    /* get plugin port mutex */
    plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(current_plugin_port);
    
    /* check port symbol */
    g_rec_mutex_lock(plugin_port_mutex);

    success = (port_index == current_plugin_port->port_index) ? TRUE: FALSE;
    
    g_rec_mutex_unlock(plugin_port_mutex);

    if(success){
      return(plugin_port);
    }
    
    plugin_port = plugin_port->next;
  }
  
  return(NULL);
}

/**
 * ags_plugin_port_new:
 *
 * Creates an #AgsPluginPort
 *
 * Returns: a new #AgsPluginPort
 *
 * Since: 3.0.0
 */
AgsPluginPort*
ags_plugin_port_new()
{
  AgsPluginPort *plugin_port;

  plugin_port = (AgsPluginPort *) g_object_new(AGS_TYPE_PLUGIN_PORT,
					       NULL);

  return(plugin_port);
}
