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

#include <ags/audio/ags_port.h>

#include <ags/libags.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_automation.h>

#include <stdlib.h>
#include <string.h>

#include <ags/i18n.h>

void ags_port_class_init(AgsPortClass *port_class);
void ags_port_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_port_init(AgsPort *port);
void ags_port_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec);
void ags_port_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec);
void ags_port_dispose(GObject *gobject);
void ags_port_finalize(GObject *gobject);

AgsUUID* ags_port_get_uuid(AgsConnectable *connectable);
gboolean ags_port_has_resource(AgsConnectable *connectable);
gboolean ags_port_is_ready(AgsConnectable *connectable);
void ags_port_add_to_registry(AgsConnectable *connectable);
void ags_port_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_port_list_resource(AgsConnectable *connectable);
xmlNode* ags_port_xml_compose(AgsConnectable *connectable);
void ags_port_xml_parse(AgsConnectable *connectable,
			  xmlNode *node);
gboolean ags_port_is_connected(AgsConnectable *connectable);
void ags_port_connect(AgsConnectable *connectable);
void ags_port_disconnect(AgsConnectable *connectable);

void ags_port_real_safe_read(AgsPort *port, GValue *value);
void ags_port_real_safe_write(AgsPort *port, GValue *value);
void ags_port_real_safe_get_property(AgsPort *port, gchar *property_name, GValue *value);
void ags_port_real_safe_set_property(AgsPort *port, gchar *property_name, GValue *value);

/**
 * SECTION:ags_port
 * @short_description: Perform thread-safe operations
 * @title: AgsPort
 * @section_id:
 * @include: ags/audio/ags_port.h
 *
 * #AgsPort provides a thread-safe way to access or change values or properties.
 */

enum{
  SAFE_READ,
  SAFE_WRITE,
  SAFE_GET_PROPERTY,
  SAFE_SET_PROPERTY,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_PLUGIN_NAME,
  PROP_SPECIFIER,
  PROP_CONTROL_PORT,
  PROP_PORT_VALUE_IS_POINTER,
  PROP_PORT_VALUE_TYPE,
  PROP_PORT_VALUE_SIZE,
  PROP_PORT_VALUE_LENGTH,
  PROP_PLUGIN_PORT,
  PROP_CONVERSION,
  PROP_AUTOMATION,
  PROP_PORT_VALUE,
};

static gpointer ags_port_parent_class = NULL;
static guint port_signals[LAST_SIGNAL];

static pthread_mutex_t ags_port_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_port_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_port = 0;

    static const GTypeInfo ags_port_info = {
      sizeof (AgsPortClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_port_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPort),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_port_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_port_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_port = g_type_register_static(G_TYPE_OBJECT,
					   "AgsPort",
					   &ags_port_info,
					   0);

    g_type_add_interface_static(ags_type_port,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_port);
  }

  return g_define_type_id__volatile;
}

void
ags_port_class_init(AgsPortClass *port)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_port_parent_class = g_type_class_peek_parent(port);

  /* GObjectClass */
  gobject = (GObjectClass *) port;

  gobject->set_property = ags_port_set_property;
  gobject->get_property = ags_port_get_property;

  gobject->dispose = ags_port_dispose;
  gobject->finalize = ags_port_finalize;
  
  /* properties */
  /**
   * AgsPort:plugin-name:
   *
   * The assigned plugin.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("plugin-name",
				   i18n_pspec("plugin-name of port"),
				   i18n_pspec("The plugin-name this port belongs to"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLUGIN_NAME,
				  param_spec);

  /**
   * AgsPort:specifier:
   *
   * The assigned plugin identifier.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("specifier",
				   i18n_pspec("specifier of port"),
				   i18n_pspec("The specifier this port is identified by"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SPECIFIER,
				  param_spec);

  /**
   * AgsPort:control-port:
   *
   * The assigned plugin control port.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("control-port",
				   i18n_pspec("control-port of port"),
				   i18n_pspec("The control-port this port is numbered"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONTROL_PORT,
				  param_spec);

  /**
   * AgsPort:port-value-is-pointer:
   *
   * Specify port data as pointer.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_boolean("port-value-is-pointer",
				    i18n_pspec("port-value-is-pointer indicates if value is a pointer"),
				    i18n_pspec("The port-value-is-pointer indicates if value is a pointer"),
				    FALSE,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT_VALUE_IS_POINTER,
				  param_spec);

  /**
   * AgsPort:port-value-type:
   *
   * The port's data type.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_gtype("port-value-type",
				  i18n_pspec("port-value-type tells you the type of the values"),
				  i18n_pspec("The port-value-type tells you the type of the values"),
				  G_TYPE_NONE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT_VALUE_TYPE,
				  param_spec);

  /**
   * AgsPort:port-value-size:
   *
   * The port's data type size.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("port-value-size",
				 i18n_pspec("port-value-size is the size of a single entry"),
				 i18n_pspec("The port-value-size is the size of a single entry"),
				 1,
				 8,
				 sizeof(gdouble),
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT_VALUE_SIZE,
				  param_spec);

  /**
   * AgsPort:port-value-length:
   *
   * The port's data array length.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("port-value-length",
				 i18n_pspec("port-value-length is the array size"),
				 i18n_pspec("The port-value-length is the array size"),
				 0,
				 G_MAXUINT32,
				 1,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT_VALUE_LENGTH,
				  param_spec);
  
  /**
   * AgsPort:plugin-port:
   *
   * The plugin-port.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("plugin-port",
				   i18n_pspec("plugin port"),
				   i18n_pspec("The plugin port"),
				   AGS_TYPE_PLUGIN_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLUGIN_PORT,
				  param_spec);

  /**
   * AgsPort:conversion:
   *
   * The port's conversion object.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("conversion",
				   i18n_pspec("conversion converts values"),
				   i18n_pspec("The conversion is able to translate values"),
				   AGS_TYPE_CONVERSION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONVERSION,
				  param_spec);

  /**
   * AgsPort:automation:
   *
   * The port's automation.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("automation",
				    i18n_pspec("automation"),
				    i18n_pspec("The automation to apply"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUTOMATION,
				  param_spec);

  /* AgsPortClass */
  port->safe_read = ags_port_real_safe_read;
  port->safe_write = ags_port_real_safe_write;

  port->safe_get_property = ags_port_real_safe_get_property;
  port->safe_set_property = ags_port_real_safe_set_property;

  /* signals */
  /**
   * AgsPort::safe-read:
   * @port: the object providing safe read
   *
   * The ::safe-read signal is emited while doing safe read operation.
   * 
   * Since: 2.0.0
   */
  port_signals[SAFE_READ] =
    g_signal_new("safe-read",
		 G_TYPE_FROM_CLASS (port),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsPortClass, safe_read),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__POINTER,
		 G_TYPE_NONE, 1,
		 G_TYPE_POINTER);

  /**
   * AgsPort::safe-write:
   * @port: the object providing safe write
   *
   * The ::safe-write signal is emited while doing safe write operation.
   * 
   * Since: 2.0.0
   */
  port_signals[SAFE_WRITE] =
    g_signal_new("safe-write",
		 G_TYPE_FROM_CLASS (port),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsPortClass, safe_write),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__POINTER,
		 G_TYPE_NONE, 1,
		 G_TYPE_POINTER);

  /**
   * AgsPort::safe-get-property:
   * @port: the object providing safe get property
   *
   * The ::safe-get-property signal is emited while safe get property.
   * 
   * Since: 2.0.0
   */
  port_signals[SAFE_GET_PROPERTY] =
    g_signal_new("safe-get-property",
		 G_TYPE_FROM_CLASS (port),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsPortClass, safe_get_property),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__STRING_POINTER,
		 G_TYPE_NONE, 2,
		 G_TYPE_STRING, G_TYPE_POINTER);

  /**
   * AgsPort::safe-set-property:
   * @port: the object providing safe set property
   *
   * The ::safe-set-property signal is emited while safe set property.
   * 
   * Since: 2.0.0
   */
  port_signals[SAFE_SET_PROPERTY] =
    g_signal_new("safe-set-property",
		 G_TYPE_FROM_CLASS (port),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsPortClass, safe_set_property),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__STRING_POINTER,
		 G_TYPE_NONE, 2,
		 G_TYPE_STRING, G_TYPE_POINTER);
}

void
ags_port_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_port_get_uuid;
  connectable->has_resource = ags_port_has_resource;

  connectable->is_ready = ags_port_is_ready;
  connectable->add_to_registry = ags_port_add_to_registry;
  connectable->remove_from_registry = ags_port_remove_from_registry;

  connectable->list_resource = ags_port_list_resource;
  connectable->xml_compose = ags_port_xml_compose;
  connectable->xml_parse = ags_port_xml_parse;

  connectable->is_connected = ags_port_is_connected;  
  connectable->connect = ags_port_connect;
  connectable->disconnect = ags_port_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_port_init(AgsPort *port)
{
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  port->flags = 0; // AGS_PORT_CONVERT_ALWAYS;

  /* add port mutex */
  port->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  port->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);  

  /* common fields */
  port->plugin_name = NULL;
  port->specifier = NULL;

  port->control_port = NULL;
  
  port->port_value_is_pointer = FALSE;
  port->port_value_type = G_TYPE_DOUBLE;

  port->port_value_size = sizeof(gdouble);
  port->port_value_length = 1;

  port->plugin_port = NULL;
  
  port->conversion = ags_conversion_new();
  g_object_ref(port->conversion);
  
  port->automation = NULL;

  port->port_value.ags_port_double = 0.0;
}

void
ags_port_set_property(GObject *gobject,
		      guint prop_id,
		      const GValue *value,
		      GParamSpec *param_spec)
{
  AgsPort *port;

  pthread_mutex_t *port_mutex;

  port = AGS_PORT(gobject);

  /* get port mutex */
  pthread_mutex_lock(ags_port_get_class_mutex());
  
  port_mutex = port->obj_mutex;
  
  pthread_mutex_unlock(ags_port_get_class_mutex());

  switch(prop_id){
  case PROP_PLUGIN_NAME:
    {
      gchar *plugin_name;

      plugin_name = (gchar *) g_value_get_string(value);

      pthread_mutex_lock(port_mutex);
      
      if(port->plugin_name == plugin_name){
	pthread_mutex_unlock(port_mutex);
	
	return;
      }
      
      if(port->plugin_name != NULL){
	g_free(port->plugin_name);
      }

      port->plugin_name = g_strdup(plugin_name);

      pthread_mutex_unlock(port_mutex);
    }
    break;
  case PROP_SPECIFIER:
    {
      gchar *specifier;

      specifier = (gchar *) g_value_get_string(value);

      pthread_mutex_lock(port_mutex);

      if(port->specifier == specifier){
	pthread_mutex_unlock(port_mutex);
	
	return;
      }

      if(port->specifier != NULL){
	g_free(port->specifier);
      }

      port->specifier = g_strdup(specifier);

      pthread_mutex_unlock(port_mutex);
    }
    break;
  case PROP_CONTROL_PORT:
    {
      gchar *control_port;

      control_port = (gchar *) g_value_get_string(value);
      
      pthread_mutex_lock(port_mutex);

      if(port->control_port == control_port){
	pthread_mutex_unlock(port_mutex);
	
	return;
      }

      if(port->control_port != NULL){
	g_free(port->control_port);
      }

      port->control_port = g_strdup(control_port);

      pthread_mutex_unlock(port_mutex);
    }
    break;
  case PROP_PORT_VALUE_IS_POINTER:
    {
      pthread_mutex_lock(port_mutex);

      port->port_value_is_pointer = g_value_get_boolean(value);

      pthread_mutex_unlock(port_mutex);
    }
    break;
  case PROP_PORT_VALUE_TYPE:
    {
      pthread_mutex_lock(port_mutex);

      port->port_value_type = g_value_get_gtype(value);

      pthread_mutex_unlock(port_mutex);
    }
    break;
  case PROP_PORT_VALUE_SIZE:
    {
      pthread_mutex_lock(port_mutex);

      port->port_value_size = g_value_get_uint(value);

      pthread_mutex_unlock(port_mutex);
    }
    break;
  case PROP_PORT_VALUE_LENGTH:
    {
      pthread_mutex_lock(port_mutex);

      port->port_value_length = g_value_get_uint(value);

      pthread_mutex_unlock(port_mutex);
    }
    break;
  case PROP_PLUGIN_PORT:
    {
      AgsPluginPort *plugin_port;
      
      plugin_port = g_value_get_object(value);

      pthread_mutex_lock(port_mutex);

      if(plugin_port == port->plugin_port){
	pthread_mutex_unlock(port_mutex);
	
	return;
      }

      if(port->plugin_port != NULL){
	g_object_unref(port->plugin_port);
      }

      if(plugin_port != NULL){
	g_object_ref(plugin_port);
      }

      port->plugin_port = plugin_port;

      pthread_mutex_unlock(port_mutex);
    }
    break;
  case PROP_CONVERSION:
    {
      AgsConversion *conversion;
      
      conversion = g_value_get_object(value);

      pthread_mutex_lock(port_mutex);

      if(conversion == port->conversion){
	pthread_mutex_unlock(port_mutex);
	
	return;
      }

      if(port->conversion != NULL){
	g_object_unref(port->conversion);
      }

      if(conversion != NULL){
	g_object_ref(conversion);
      }

      port->conversion = conversion;

      pthread_mutex_unlock(port_mutex);
    }
    break;
  case PROP_AUTOMATION:
    {
      AgsAutomation *automation;
      
      automation = g_value_get_pointer(value);

      if(g_list_find(port->automation, automation) != NULL){
	pthread_mutex_unlock(port_mutex);
	
	return;
      }

      if(automation != NULL){
	g_object_ref(automation);
      }

      port->automation = ags_automation_add(port->automation,
					    automation);
      
      pthread_mutex_unlock(port_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_port_get_property(GObject *gobject,
		      guint prop_id,
		      GValue *value,
		      GParamSpec *param_spec)
{
  AgsPort *port;

  pthread_mutex_t *port_mutex;

  port = AGS_PORT(gobject);

  /* get port mutex */
  pthread_mutex_lock(ags_port_get_class_mutex());
  
  port_mutex = port->obj_mutex;
  
  pthread_mutex_unlock(ags_port_get_class_mutex());

  switch(prop_id){
  case PROP_PLUGIN_NAME:
    {
      pthread_mutex_lock(port_mutex);

      g_value_set_string(value, port->plugin_name);

      pthread_mutex_unlock(port_mutex);
    }
    break;
  case PROP_SPECIFIER:
    {
      pthread_mutex_lock(port_mutex);

      g_value_set_string(value, port->specifier);

      pthread_mutex_unlock(port_mutex);
    }
    break;
  case PROP_CONTROL_PORT:
    {
      pthread_mutex_lock(port_mutex);

      g_value_set_string(value, port->control_port);

      pthread_mutex_unlock(port_mutex);
    }
    break;
  case PROP_PORT_VALUE_IS_POINTER:
    {
      pthread_mutex_lock(port_mutex);

      g_value_set_boolean(value, port->port_value_is_pointer);

      pthread_mutex_unlock(port_mutex);
    }
    break;
  case PROP_PORT_VALUE_TYPE:
    {
      pthread_mutex_lock(port_mutex);

      g_value_set_gtype(value, port->port_value_type);

      pthread_mutex_unlock(port_mutex);
    }
    break;
  case PROP_PORT_VALUE_SIZE:
    {
      pthread_mutex_lock(port_mutex);

      g_value_set_uint(value, port->port_value_size);

      pthread_mutex_unlock(port_mutex);
    }
    break;
  case PROP_PORT_VALUE_LENGTH:
    {
      pthread_mutex_lock(port_mutex);

      g_value_set_uint(value, port->port_value_length);

      pthread_mutex_unlock(port_mutex);
    }
    break;
  case PROP_PLUGIN_PORT:
    {
      pthread_mutex_lock(port_mutex);

      g_value_set_object(value, port->plugin_port);

      pthread_mutex_unlock(port_mutex);
    }
    break;
  case PROP_CONVERSION:
    {
      pthread_mutex_lock(port_mutex);

      g_value_set_object(value, port->conversion);

      pthread_mutex_unlock(port_mutex);
    }
    break;
  case PROP_AUTOMATION:
    {
      pthread_mutex_lock(port_mutex);

      g_value_set_pointer(value, g_list_copy(port->automation));

      pthread_mutex_unlock(port_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_port_dispose(GObject *gobject)
{
  AgsPort *port;

  port = AGS_PORT(gobject);

  if(port->plugin_port != NULL){
    g_object_unref(port->plugin_port);

    port->plugin_port = NULL;
  }
  
  if(port->conversion != NULL){
    g_object_unref(port->conversion);

    port->conversion = NULL;
  }

  if(port->automation != NULL){
    g_list_free_full(port->automation,
		     g_object_unref);

    port->automation = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_port_parent_class)->dispose(gobject);
}

void
ags_port_finalize(GObject *gobject)
{
  AgsPort *port;

  port = AGS_PORT(gobject);

  pthread_mutex_destroy(port->obj_mutex);
  free(port->obj_mutex);

  pthread_mutexattr_destroy(port->obj_mutexattr);
  free(port->obj_mutexattr);

  g_free(port->plugin_name);
  g_free(port->specifier);

  g_free(port->control_port);
  
  if(port->plugin_port != NULL){
    g_object_unref(port->plugin_port);
  }
  
  if(port->conversion != NULL){
    g_object_unref(port->conversion);
  }

  if(port->automation != NULL){
    g_list_free_full(port->automation,
		     g_object_unref);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_port_parent_class)->finalize(gobject);
}

AgsUUID*
ags_port_get_uuid(AgsConnectable *connectable)
{
  AgsPort *port;
  
  AgsUUID *ptr;

  pthread_mutex_t *port_mutex;

  port = AGS_PORT(connectable);

  /* get port mutex */
  pthread_mutex_lock(ags_port_get_class_mutex());
  
  port_mutex = port->obj_mutex;
  
  pthread_mutex_unlock(ags_port_get_class_mutex());

  /* get UUID */
  pthread_mutex_lock(port_mutex);

  ptr = port->uuid;

  pthread_mutex_unlock(port_mutex);
  
  return(ptr);
}

gboolean
ags_port_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_port_is_ready(AgsConnectable *connectable)
{
  AgsPort *port;
  
  gboolean is_ready;

  pthread_mutex_t *port_mutex;

  port = AGS_PORT(connectable);

  /* get port mutex */
  pthread_mutex_lock(ags_port_get_class_mutex());
  
  port_mutex = port->obj_mutex;
  
  pthread_mutex_unlock(ags_port_get_class_mutex());

  /* check is added */
  pthread_mutex_lock(port_mutex);

  is_ready = (((AGS_PORT_ADDED_TO_REGISTRY & (port->flags)) != 0) ? TRUE: FALSE);

  pthread_mutex_unlock(port_mutex);
  
  return(is_ready);
}

void
ags_port_add_to_registry(AgsConnectable *connectable)
{
  AgsPort *port;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  GList *list;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  port = AGS_PORT(connectable);

  ags_port_set_flags(port, AGS_PORT_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(&(entry->entry),
		       (gpointer) port);
    ags_registry_add_entry(registry,
			   entry);
  }

  //TODO:JK: implement me
}

void
ags_port_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_port_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_port_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_port_xml_parse(AgsConnectable *connectable,
		     xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_port_is_connected(AgsConnectable *connectable)
{
  AgsPort *port;
  
  gboolean is_connected;

  pthread_mutex_t *port_mutex;

  port = AGS_PORT(connectable);

  /* get port mutex */
  pthread_mutex_lock(ags_port_get_class_mutex());
  
  port_mutex = port->obj_mutex;
  
  pthread_mutex_unlock(ags_port_get_class_mutex());

  /* check is connected */
  pthread_mutex_lock(port_mutex);

  is_connected = (((AGS_PORT_CONNECTED & (port->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(port_mutex);
  
  return(is_connected);
}

void
ags_port_connect(AgsConnectable *connectable)
{
  AgsPort *port;

  GList *list_start, *list;

  pthread_mutex_t *port_mutex;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  port = AGS_PORT(connectable);

  ags_port_set_flags(port, AGS_PORT_CONNECTED);  
}

void
ags_port_disconnect(AgsConnectable *connectable)
{
  AgsPort *port;

  GList *list_start, *list;

  pthread_mutex_t *port_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  port = AGS_PORT(connectable);

  ags_port_unset_flags(port, AGS_PORT_CONNECTED);    
}

/**
 * ags_port_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_port_get_class_mutex()
{
  return(&ags_port_class_mutex);
}

/**
 * ags_port_test_flags:
 * @port: the #AgsPort
 * @flags: the flags
 *
 * Test @flags to be set on @port.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_port_test_flags(AgsPort *port, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *port_mutex;

  if(!AGS_IS_PORT(port)){
    return(FALSE);
  }

  /* get port mutex */
  pthread_mutex_lock(ags_port_get_class_mutex());
  
  port_mutex = port->obj_mutex;
  
  pthread_mutex_unlock(ags_port_get_class_mutex());

  /* test */
  pthread_mutex_lock(port_mutex);

  retval = (flags & (port->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(port_mutex);

  return(retval);
}

/**
 * ags_port_set_flags:
 * @port: the #AgsPort
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 2.0.0
 */
void
ags_port_set_flags(AgsPort *port, guint flags)
{
  pthread_mutex_t *port_mutex;

  if(!AGS_IS_PORT(port)){
    return;
  }

  /* get port mutex */
  pthread_mutex_lock(ags_port_get_class_mutex());
  
  port_mutex = port->obj_mutex;
  
  pthread_mutex_unlock(ags_port_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(port_mutex);

  port->flags |= flags;

  pthread_mutex_unlock(port_mutex);
}

/**
 * ags_port_unset_flags:
 * @port: the #AgsPort
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 2.0.0
 */
void
ags_port_unset_flags(AgsPort *port, guint flags)
{
  pthread_mutex_t *port_mutex;

  if(!AGS_IS_PORT(port)){
    return;
  }

  /* get port mutex */
  pthread_mutex_lock(ags_port_get_class_mutex());
  
  port_mutex = port->obj_mutex;
  
  pthread_mutex_unlock(ags_port_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(port_mutex);

  port->flags &= (~flags);

  pthread_mutex_unlock(port_mutex);
}

void
ags_port_real_safe_read(AgsPort *port, GValue *value)
{
  guint overall_size;
  gpointer data;

  pthread_mutex_t *port_mutex;

  /* get port mutex */
  pthread_mutex_lock(ags_port_get_class_mutex());
  
  port_mutex = port->obj_mutex;
  
  pthread_mutex_unlock(ags_port_get_class_mutex());

  /* safe read */
  pthread_mutex_lock(port_mutex);

  overall_size = port->port_value_length * port->port_value_size;

  if(!port->port_value_is_pointer){
    if(port->port_value_type == G_TYPE_BOOLEAN){
      g_value_set_boolean(value, port->port_value.ags_port_boolean);
    }else if(port->port_value_type == G_TYPE_INT64){
      g_value_set_int64(value, port->port_value.ags_port_int);
    }else if(port->port_value_type == G_TYPE_UINT64){
      g_value_set_uint64(value, port->port_value.ags_port_uint);
    }else if(port->port_value_type == G_TYPE_FLOAT){
      gfloat new_value;
      
      if((AGS_PORT_CONVERT_ALWAYS & (port->flags)) != 0){
        new_value = (gfloat) ags_conversion_convert(port->conversion,
						    (double) port->port_value.ags_port_float,
						    TRUE);
      }else{
	new_value = port->port_value.ags_port_float;
      }
      
      g_value_set_float(value, new_value);
    }else if(port->port_value_type == G_TYPE_DOUBLE){
      gdouble new_value;
      
      if((AGS_PORT_CONVERT_ALWAYS & (port->flags)) != 0){
        new_value = ags_conversion_convert(port->conversion,
					   port->port_value.ags_port_double,
					   TRUE);
      }else{
	new_value = port->port_value.ags_port_double;
      }
      
      g_value_set_double(value, new_value);
    }else{
      data = NULL;
      
      if(port->port_value_type == G_TYPE_POINTER){
	data = port->port_value.ags_port_pointer;
      }else if(port->port_value_type == G_TYPE_OBJECT){
	data = port->port_value.ags_port_object;
      }

      g_value_set_pointer(value, data);
    }
  }else{
    data = NULL;
    
    if(port->port_value_type == G_TYPE_POINTER){
      data = port->port_value.ags_port_pointer;
    }else if(port->port_value_type == G_TYPE_OBJECT){
      data = port->port_value.ags_port_object;
    }else{
      data = (gpointer) malloc(overall_size);

      if(port->port_value_type == G_TYPE_BOOLEAN){
	memcpy(data, port->port_value.ags_port_boolean_ptr, overall_size);
      }else if(port->port_value_type == G_TYPE_INT64){
	memcpy(data, port->port_value.ags_port_int_ptr, overall_size);
      }else if(port->port_value_type == G_TYPE_UINT64){
	memcpy(data, port->port_value.ags_port_uint_ptr, overall_size);
      }else if(port->port_value_type == G_TYPE_FLOAT){
	guint i;

	for(i = 0; i < port->port_value_length; i++){
	  ((gfloat *) data)[i] = port->port_value.ags_port_float_ptr[i];
	}
      }else if(port->port_value_type == G_TYPE_DOUBLE){
	guint i;

	for(i = 0; i < port->port_value_length; i++){
	  ((gdouble *) data)[i] = port->port_value.ags_port_double_ptr[i];
	}
      }
    }
   
    g_value_set_pointer(value, data);
  }
  
  pthread_mutex_unlock(port_mutex);
}

/**
 * ags_port_safe_read:
 * @port: an #AgsPort
 * @value: the #GValue to store result
 *
 * Perform safe read.
 *
 * Since: 2.0.0
 */
void
ags_port_safe_read(AgsPort *port, GValue *value)
{
  g_return_if_fail(AGS_IS_PORT(port));
  g_object_ref(G_OBJECT(port));
  g_signal_emit(G_OBJECT(port),
		port_signals[SAFE_READ], 0,
		value);
  g_object_unref(G_OBJECT(port));
}

/**
 * ags_port_safe_read_raw:
 * @port: an #AgsPort
 * @value: the #GValue to store result
 *
 * Perform safe read.
 *
 * Since: 2.0.0
 */
void
ags_port_safe_read_raw(AgsPort *port, GValue *value)
{
  guint overall_size;
  gpointer data;

  pthread_mutex_t *port_mutex;

  /* get port mutex */
  pthread_mutex_lock(ags_port_get_class_mutex());
  
  port_mutex = port->obj_mutex;
  
  pthread_mutex_unlock(ags_port_get_class_mutex());

  /* safe read */
  pthread_mutex_lock(port_mutex);

  overall_size = port->port_value_length * port->port_value_size;

  if(!port->port_value_is_pointer){
    if(port->port_value_type == G_TYPE_BOOLEAN){
      g_value_set_boolean(value, port->port_value.ags_port_boolean);
    }else if(port->port_value_type == G_TYPE_INT64){
      g_value_set_int64(value, port->port_value.ags_port_int);
    }else if(port->port_value_type == G_TYPE_UINT64){
      g_value_set_uint64(value, port->port_value.ags_port_uint);
    }else if(port->port_value_type == G_TYPE_FLOAT){
      gfloat new_value;
      
      new_value = port->port_value.ags_port_float;
      
      g_value_set_float(value, new_value);
    }else if(port->port_value_type == G_TYPE_DOUBLE){
      gdouble new_value;
      
      new_value = port->port_value.ags_port_double;
      
      g_value_set_double(value, new_value);
    }else{
      data = NULL;
      
      if(port->port_value_type == G_TYPE_POINTER){
	data = port->port_value.ags_port_pointer;
      }else if(port->port_value_type == G_TYPE_OBJECT){
	data = port->port_value.ags_port_object;
      }

      g_value_set_pointer(value, data);
    }
  }else{
    data = NULL;
    
    if(port->port_value_type == G_TYPE_POINTER){
      data = port->port_value.ags_port_pointer;
    }else if(port->port_value_type == G_TYPE_OBJECT){
      data = port->port_value.ags_port_object;
    }else{
      data = (gpointer) malloc(overall_size);

      if(port->port_value_type == G_TYPE_BOOLEAN){
	memcpy(data, port->port_value.ags_port_boolean_ptr, overall_size);
      }else if(port->port_value_type == G_TYPE_INT64){
	memcpy(data, port->port_value.ags_port_int_ptr, overall_size);
      }else if(port->port_value_type == G_TYPE_UINT64){
	memcpy(data, port->port_value.ags_port_uint_ptr, overall_size);
      }else if(port->port_value_type == G_TYPE_FLOAT){
	guint i;

	for(i = 0; i < port->port_value_length; i++){
	  ((gfloat *) data)[i] = port->port_value.ags_port_float_ptr[i];
	}
      }else if(port->port_value_type == G_TYPE_DOUBLE){
	guint i;

	for(i = 0; i < port->port_value_length; i++){
	  ((gdouble *) data)[i] = port->port_value.ags_port_double_ptr[i];
	}
      }
    }
   
    g_value_set_pointer(value, data);
  }
  
  pthread_mutex_unlock(port_mutex);
}

void
ags_port_real_safe_write(AgsPort *port, GValue *value)
{
  guint overall_size;
  gpointer data;

  pthread_mutex_t *port_mutex;

  /* get port mutex */
  pthread_mutex_lock(ags_port_get_class_mutex());
  
  port_mutex = port->obj_mutex;
  
  pthread_mutex_unlock(ags_port_get_class_mutex());

  /* write */
  pthread_mutex_lock(port_mutex);

  overall_size = port->port_value_length * port->port_value_size;

  if(!port->port_value_is_pointer){
    if(port->port_value_type == G_TYPE_BOOLEAN){
      port->port_value.ags_port_boolean = g_value_get_boolean(value);
    }else if(port->port_value_type == G_TYPE_INT64){
      port->port_value.ags_port_int = g_value_get_int64(value);
    }else if(port->port_value_type == G_TYPE_UINT64){
      port->port_value.ags_port_uint = g_value_get_uint64(value);
    }else if(port->port_value_type == G_TYPE_FLOAT){
      if((AGS_PORT_CONVERT_ALWAYS & (port->flags)) != 0 &&
	 port->conversion != NULL){
	if((AGS_PORT_USE_LADSPA_FLOAT & (port->flags)) == 0){
	  port->port_value.ags_port_float = (gfloat) ags_conversion_convert(port->conversion,
									    (double) g_value_get_float(value),
									    FALSE);
	}else{
	  LADSPA_Data val;
	  
	  val = g_value_get_float(value);
	  port->port_value.ags_port_ladspa = (LADSPA_Data) ags_conversion_convert(port->conversion,
										  (double) val,
										  FALSE);
	}
      }else{
	if((AGS_PORT_USE_LADSPA_FLOAT & (port->flags)) == 0){
	  port->port_value.ags_port_float = (gfloat) g_value_get_float(value);
	}else{
	  port->port_value.ags_port_ladspa = (LADSPA_Data) g_value_get_float(value);
	}
      }
    }else if(port->port_value_type == G_TYPE_DOUBLE){
      if((AGS_PORT_CONVERT_ALWAYS & (port->flags)) != 0 &&
	 port->conversion != NULL){
	port->port_value.ags_port_double = ags_conversion_convert(port->conversion,
								  g_value_get_double(value),
								  FALSE);
      }else{
	port->port_value.ags_port_double = g_value_get_double(value);
      }
    }else if(port->port_value_type == G_TYPE_POINTER){
      port->port_value.ags_port_pointer = g_value_get_pointer(value);
    }else if(port->port_value_type == G_TYPE_OBJECT){
      port->port_value.ags_port_object = g_value_get_object(value);
    }else{
      g_warning("ags_port.c: unknown type");
    }
  }else{
    data = g_value_get_pointer(value);

    if(port->port_value_type == G_TYPE_BOOLEAN){
      memcpy(port->port_value.ags_port_boolean_ptr, data, overall_size);
    }else if(port->port_value_type == G_TYPE_INT64){
      memcpy(port->port_value.ags_port_int_ptr, data, overall_size);
    }else if(port->port_value_type == G_TYPE_UINT64){
      memcpy(port->port_value.ags_port_uint_ptr, data, overall_size);
    }else if(port->port_value_type == G_TYPE_DOUBLE){
      memcpy(port->port_value.ags_port_double_ptr, data, overall_size);
    }else if(port->port_value_type == G_TYPE_POINTER){
      port->port_value.ags_port_pointer = data;
    }else{
      data = g_value_get_object(value);

      if(port->port_value_type == G_TYPE_OBJECT){
	port->port_value.ags_port_object = data;
      }else{
	g_warning("ags_port.c: unknown type");
      }
    }
  }

  pthread_mutex_unlock(port_mutex);
}

/**
 * ags_port_safe_write:
 * @port: an #AgsPort
 * @value: the #GValue containing data
 *
 * Perform safe write.
 *
 * Since: 2.0.0
 */
void
ags_port_safe_write(AgsPort *port, GValue *value)
{
  g_return_if_fail(AGS_IS_PORT(port));
  g_object_ref(G_OBJECT(port));
  g_signal_emit(G_OBJECT(port),
		port_signals[SAFE_WRITE], 0,
		value);
  g_object_unref(G_OBJECT(port));
}

void
ags_port_safe_write_raw(AgsPort *port, GValue *value)
{
  guint overall_size;
  gpointer data;

  pthread_mutex_t *port_mutex;

  if(!AGS_IS_PORT(port)){
    return;
  }

  /* get port mutex */
  pthread_mutex_lock(ags_port_get_class_mutex());
  
  port_mutex = port->obj_mutex;
  
  pthread_mutex_unlock(ags_port_get_class_mutex());

  /* write raw */
  pthread_mutex_lock(port_mutex);

  overall_size = port->port_value_length * port->port_value_size;

  if(!port->port_value_is_pointer){
    if(port->port_value_type == G_TYPE_BOOLEAN){
      port->port_value.ags_port_boolean = g_value_get_boolean(value);
    }else if(port->port_value_type == G_TYPE_INT64){
      port->port_value.ags_port_int = g_value_get_int64(value);
    }else if(port->port_value_type == G_TYPE_UINT64){
      port->port_value.ags_port_uint = g_value_get_uint64(value);
    }else if(port->port_value_type == G_TYPE_FLOAT){
      if((AGS_PORT_USE_LADSPA_FLOAT & (port->flags)) == 0){
	port->port_value.ags_port_float = (gfloat) g_value_get_float(value);
      }else{
	port->port_value.ags_port_ladspa = (LADSPA_Data) g_value_get_float(value);
      }
    }else if(port->port_value_type == G_TYPE_DOUBLE){
      port->port_value.ags_port_double = g_value_get_double(value);
    }else if(port->port_value_type == G_TYPE_POINTER){
      port->port_value.ags_port_pointer = g_value_get_pointer(value);
    }else if(port->port_value_type == G_TYPE_OBJECT){
      port->port_value.ags_port_object = g_value_get_object(value);
    }else{
      g_warning("ags_port.c: unknown type");
    }
  }else{
    data = g_value_get_pointer(value);

    if(port->port_value_type == G_TYPE_BOOLEAN){
      memcpy(port->port_value.ags_port_boolean_ptr, data, overall_size);
    }else if(port->port_value_type == G_TYPE_INT64){
      memcpy(port->port_value.ags_port_int_ptr, data, overall_size);
    }else if(port->port_value_type == G_TYPE_UINT64){
      memcpy(port->port_value.ags_port_uint_ptr, data, overall_size);
    }else if(port->port_value_type == G_TYPE_DOUBLE){
      memcpy(port->port_value.ags_port_double_ptr, data, overall_size);
    }else if(port->port_value_type == G_TYPE_POINTER){
      port->port_value.ags_port_pointer = data;
    }else{
      data = g_value_get_object(value);

      if(port->port_value_type == G_TYPE_OBJECT){
	port->port_value.ags_port_object = data;
      }else{
	g_warning("ags_port.c: unknown type");
      }
    }
  }

  pthread_mutex_unlock(port_mutex);
}

void
ags_port_real_safe_get_property(AgsPort *port, gchar *property_name, GValue *value)
{
  pthread_mutex_t *port_mutex;

  /* get port mutex */
  pthread_mutex_lock(ags_port_get_class_mutex());
  
  port_mutex = port->obj_mutex;
  
  pthread_mutex_unlock(ags_port_get_class_mutex());

  /* get property */
  pthread_mutex_lock(port_mutex);

  g_object_get_property(port->port_value.ags_port_object,
			property_name,
			value);

  pthread_mutex_unlock(port_mutex);
}

/**
 * ags_port_safe_get_property:
 * @port: an #AgsPort
 * @property_name: the property's name
 * @value: the #GValue to store the result
 *
 * Perform safe get property.
 *
 * Since: 2.0.0
 */
void
ags_port_safe_get_property(AgsPort *port, gchar *property_name, GValue *value)
{
  g_return_if_fail(AGS_IS_PORT(port));
  g_object_ref(G_OBJECT(port));
  g_signal_emit(G_OBJECT(port),
		port_signals[SAFE_GET_PROPERTY], 0,
		property_name, value);
  g_object_unref(G_OBJECT(port));
}

void
ags_port_real_safe_set_property(AgsPort *port, gchar *property_name, GValue *value)
{
  pthread_mutex_t *port_mutex;

  /* get port mutex */
  pthread_mutex_lock(ags_port_get_class_mutex());
  
  port_mutex = port->obj_mutex;
  
  pthread_mutex_unlock(ags_port_get_class_mutex());

  /* set property */
  pthread_mutex_lock(port_mutex);

  g_object_set_property(port->port_value.ags_port_object,
			property_name,
			value);

  pthread_mutex_unlock(port_mutex);
}

/**
 * ags_port_safe_set_property:
 * @port: an #AgsPort
 * @property_name: the property's name
 * @value: the #GValue containing data
 *
 * Perform safe set property.
 *
 * Since: 2.0.0
 */
void
ags_port_safe_set_property(AgsPort *port, gchar *property_name, GValue *value)
{
  g_return_if_fail(AGS_IS_PORT(port));
  g_object_ref(G_OBJECT(port));
  g_signal_emit(G_OBJECT(port),
		port_signals[SAFE_SET_PROPERTY], 0,
		property_name, value);
  g_object_unref(G_OBJECT(port));
}

/**
 * ags_port_find_specifier:
 * @port: the #GList-struct containing #AgsPort
 * @specifier: the recall specifier to match
 *
 * Retrieve port by specifier.
 *
 * Returns: Next matching #GList-struct or %NULL
 *
 * Since: 2.0.0
 */
GList*
ags_port_find_specifier(GList *port, gchar *specifier)
{
  AgsPort *current_port;
  
  gboolean success;

  pthread_mutex_t *port_mutex;
  
  while(port != NULL){
    current_port = port->data;

    /* get port mutex */
    pthread_mutex_lock(ags_port_get_class_mutex());
  
    port_mutex = current_port->obj_mutex;
  
    pthread_mutex_unlock(ags_port_get_class_mutex());

    /* check specifier */
    pthread_mutex_lock(port_mutex);

    success = (!g_strcmp0(current_port->specifier,
			  specifier)) ? TRUE: FALSE;

    pthread_mutex_unlock(port_mutex);

    if(success){
      return(port);
    }

    port = port->next;
  }

  return(NULL);
}

/**
 * ags_port_add_automation:
 * @port: the #AgsPort
 * @automation: the #AgsAutomation
 *
 * Adds an automation.
 *
 * Since: 2.0.0
 */
void
ags_port_add_automation(AgsPort *port, GObject *automation)
{
  pthread_mutex_t *port_mutex;

  if(!AGS_IS_PORT(port) ||
     !AGS_IS_AUTOMATION(automation)){
    return;
  }

  /* get port mutex */
  pthread_mutex_lock(ags_port_get_class_mutex());

  port_mutex = port->obj_mutex;
  
  pthread_mutex_unlock(ags_port_get_class_mutex());

  /* add recall id */
  pthread_mutex_lock(port_mutex);

  if(g_list_find(port->automation,
		 automation) == NULL){
    g_object_ref(automation);
    port->automation = ags_automation_add(port->automation,
					   automation);

    g_object_set(automation,
		 "port", port,
		 NULL);
  }
  
  pthread_mutex_unlock(port_mutex);
}

/**
 * ags_port_remove_automation:
 * @port: the #AgsPort
 * @automation: the #AgsAutomation
 *
 * Removes an automation.
 *
 * Since: 2.0.0
 */
void
ags_port_remove_automation(AgsPort *port, GObject *automation)
{
  pthread_mutex_t *port_mutex;

  if(!AGS_IS_PORT(port) ||
     !AGS_IS_AUTOMATION(automation)){
    return;
  }

  /* get port mutex */
  pthread_mutex_lock(ags_port_get_class_mutex());

  port_mutex = port->obj_mutex;
  
  pthread_mutex_unlock(ags_port_get_class_mutex());

  /* remove automation */
  pthread_mutex_lock(port_mutex);

  if(g_list_find(port->automation,
		 automation) != NULL){
    port->automation = g_list_remove(port->automation,
				      automation);

    g_object_set(automation,
		 "port", NULL,
		 NULL);

    g_object_unref(automation);
  }
  
  pthread_mutex_unlock(port_mutex);
}

/**
 * ags_port_new:
 *
 * Creates an #AgsPort.
 *
 * Returns: a new #AgsPort.
 *
 * Since: 2.0.0
 */
AgsPort*
ags_port_new()
{
  AgsPort *port;

  port = (AgsPort *) g_object_new(AGS_TYPE_PORT,
				  NULL);

  return(port);
}
