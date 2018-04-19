/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/libags.h>

#include <ags/i18n.h>

void ags_plugin_port_class_init(AgsPluginPortClass *plugin_port);
void ags_plugin_port_init (AgsPluginPort *plugin_port);
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
  PROP_SCALE_POINTS,
  PROP_SCALE_VALUE,
  PROP_LOWER_VALUE,
  PROP_UPPER_VALUE,
  PROP_DEFAULT_VALUE,
};

static gpointer ags_plugin_port_parent_class = NULL;

static pthread_mutex_t ags_plugin_port_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_plugin_port_get_type (void)
{
  static GType ags_type_plugin_port = 0;

  if(!ags_type_plugin_port){
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
  }

  return (ags_type_plugin_port);
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
}

void
ags_plugin_port_init(AgsPluginPort *plugin_port)
{
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  plugin_port->flags = 0;

  /* add base plugin mutex */
  plugin_port->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  plugin_port->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  plugin_port->port_index = 0;

  plugin_port->port_name = NULL;
  plugin_port->port_symbol = NULL;

  plugin_port->scale_steps = -1;
  plugin_port->scale_points = NULL;
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

  pthread_mutex_t *plugin_port_mutex;

  plugin_port = AGS_PLUGIN_PORT(gobject);

  /* get base plugin mutex */
  pthread_mutex_lock(ags_plugin_port_get_class_mutex());
  
  plugin_port_mutex = plugin_port->obj_mutex;
  
  pthread_mutex_unlock(ags_plugin_port_get_class_mutex());

  switch(prop_id){
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

  pthread_mutex_t *plugin_port_mutex;

  plugin_port = AGS_PLUGIN_PORT(gobject);

  /* get base plugin mutex */
  pthread_mutex_lock(ags_plugin_port_get_class_mutex());
  
  plugin_port_mutex = plugin_port->obj_mutex;
  
  pthread_mutex_unlock(ags_plugin_port_get_class_mutex());

  switch(prop_id){
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

  /* destroy object mutex */
  pthread_mutex_destroy(plugin_port->obj_mutex);
  free(plugin_port->obj_mutex);

  pthread_mutexattr_destroy(plugin_port->obj_mutexattr);
  free(plugin_port->obj_mutexattr);
  
  if(plugin_port->port_name != NULL){
    g_free(plugin_port->port_name);
  }
  
  if(plugin_port->port_symbol != NULL){
    g_free(plugin_port->port_symbol);
  }

  if(plugin_port->scale_points != NULL){
    g_free(plugin_port->scale_points);
  }

  g_free(plugin_port->lower_value);
  g_free(plugin_port->upper_value);

  g_free(plugin_port->default_value);

  /* call parent */
  G_OBJECT_CLASS(ags_plugin_port_parent_class)->finalize(gobject);
}

/**
 * ags_plugin_port_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_plugin_port_get_class_mutex()
{
  return(&ags_plugin_port_class_mutex);
}

/**
 * ags_plugin_port_find_symbol:
 * @plugin_port: the #GList-struct containing #AgsPluginPort
 * @port_symbol: the port symbol
 * 
 * Find @port_symbol within @plugin_port.
 * 
 * Returns: the matching #GList-struct containing #AgsPluginPort
 * 
 * Since: 2.0.0
 */
GList*
ags_plugin_port_find_symbol(GList *plugin_port,
			    gchar *port_symbol)
{
  if(port_symbol == NULL){
    return(NULL);
  }

  while(plugin_port != NULL){
    if(!g_strcmp0(port_symbol,
		  AGS_PLUGIN_PORT(plugin_port->data)->port_symbol)){
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
 * Since: 2.0.0
 */
AgsPluginPort*
ags_plugin_port_new()
{
  AgsPluginPort *plugin_port;

  plugin_port = (AgsPluginPort *) g_object_new(AGS_TYPE_PLUGIN_PORT,
					       NULL);

  return(plugin_port);
}
