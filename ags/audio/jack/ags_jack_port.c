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

#include <ags/audio/jack/ags_jack_port.h>

#include <ags/audio/ags_sound_provider.h>

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_client.h>

#include <ags/i18n.h>

void ags_jack_port_class_init(AgsJackPortClass *jack_port);
void ags_jack_port_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_jack_port_init(AgsJackPort *jack_port);
void ags_jack_port_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_jack_port_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_jack_port_dispose(GObject *gobject);
void ags_jack_port_finalize(GObject *gobject);

AgsUUID* ags_jack_port_get_uuid(AgsConnectable *connectable);
gboolean ags_jack_port_has_resource(AgsConnectable *connectable);
gboolean ags_jack_port_is_ready(AgsConnectable *connectable);
void ags_jack_port_add_to_registry(AgsConnectable *connectable);
void ags_jack_port_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_jack_port_list_resource(AgsConnectable *connectable);
xmlNode* ags_jack_port_xml_compose(AgsConnectable *connectable);
void ags_jack_port_xml_parse(AgsConnectable *connectable,
			       xmlNode *node);
gboolean ags_jack_port_is_connected(AgsConnectable *connectable);
void ags_jack_port_connect(AgsConnectable *connectable);
void ags_jack_port_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_jack_port
 * @short_description: JACK resource.
 * @title: AgsJackPort
 * @section_id:
 * @include: ags/audio/jack/ags_jack_port.h
 *
 * The #AgsJackPort represents either a JACK sequencer or soundcard to communicate
 * with.
 */

enum{
  PROP_0,
  PROP_JACK_CLIENT,
  PROP_PORT_NAME,
};

static gpointer ags_jack_port_parent_class = NULL;

GType
ags_jack_port_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_jack_port = 0;

    static const GTypeInfo ags_jack_port_info = {
      sizeof(AgsJackPortClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_jack_port_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsJackPort),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_jack_port_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_jack_port_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_jack_port = g_type_register_static(G_TYPE_OBJECT,
						"AgsJackPort",
						&ags_jack_port_info,
						0);

    g_type_add_interface_static(ags_type_jack_port,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_jack_port);
  }

  return g_define_type_id__static;
}

GType
ags_jack_port_flags_get_type()
{
  static gsize g_flags_type_id__static;

  if(g_once_init_enter(&g_flags_type_id__static)){
    static const GFlagsValue values[] = {
      { AGS_JACK_PORT_REGISTERED, "AGS_JACK_PORT_REGISTERED", "jack-port-registered" },
      { AGS_JACK_PORT_IS_AUDIO, "AGS_JACK_PORT_IS_AUDIO", "jack-port-is-audio" },
      { AGS_JACK_PORT_IS_MIDI, "AGS_JACK_PORT_IS_MIDI", "jack-port-is-midi" },
      { AGS_JACK_PORT_IS_OUTPUT, "AGS_JACK_PORT_IS_OUTPUT", "jack-port-is-output" },
      { AGS_JACK_PORT_IS_INPUT, "AGS_JACK_PORT_IS_INPUT", "jack-port-is-input" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsJackPortFlags"), values);

    g_once_init_leave(&g_flags_type_id__static, g_flags_type_id);
  }
  
  return(g_flags_type_id__static);
}

void
ags_jack_port_class_init(AgsJackPortClass *jack_port)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_jack_port_parent_class = g_type_class_peek_parent(jack_port);

  /* GObjectClass */
  gobject = (GObjectClass *) jack_port;

  gobject->set_property = ags_jack_port_set_property;
  gobject->get_property = ags_jack_port_get_property;

  gobject->dispose = ags_jack_port_dispose;
  gobject->finalize = ags_jack_port_finalize;

  /* properties */
  /**
   * AgsJackPort:jack-client:
   *
   * The assigned #AgsJackClient.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("jack-client",
				   i18n_pspec("assigned JACK client"),
				   i18n_pspec("The assigned JACK client"),
				   AGS_TYPE_JACK_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_JACK_CLIENT,
				  param_spec);

  /**
   * AgsJackPort:port-name:
   *
   * The JACK port name.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("port-name",
				   i18n_pspec("port name"),
				   i18n_pspec("The port name"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT_NAME,
				  param_spec);
}

void
ags_jack_port_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_jack_port_get_uuid;
  connectable->has_resource = ags_jack_port_has_resource;

  connectable->is_ready = ags_jack_port_is_ready;
  connectable->add_to_registry = ags_jack_port_add_to_registry;
  connectable->remove_from_registry = ags_jack_port_remove_from_registry;

  connectable->list_resource = ags_jack_port_list_resource;
  connectable->xml_compose = ags_jack_port_xml_compose;
  connectable->xml_parse = ags_jack_port_xml_parse;

  connectable->is_connected = ags_jack_port_is_connected;  
  connectable->connect = ags_jack_port_connect;
  connectable->disconnect = ags_jack_port_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_jack_port_init(AgsJackPort *jack_port)
{
  /* flags */
  jack_port->flags = 0;
  jack_port->connectable_flags = 0;

  /* port mutex */
  g_rec_mutex_init(&(jack_port->obj_mutex));

  /* parent */
  jack_port->jack_client = NULL;
  
  /* uuid */
  jack_port->uuid = ags_uuid_alloc();
  ags_uuid_generate(jack_port->uuid);

  /* fields */
  jack_port->port_uuid = NULL;
  jack_port->port_name = NULL;

  jack_port->port = NULL;
}

void
ags_jack_port_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsJackPort *jack_port;

  GRecMutex *jack_port_mutex;

  jack_port = AGS_JACK_PORT(gobject);

  /* get jack port mutex */
  jack_port_mutex = AGS_JACK_PORT_GET_OBJ_MUTEX(jack_port);

  switch(prop_id){
  case PROP_JACK_CLIENT:
    {
      AgsJackClient *jack_client;

      jack_client = (AgsJackClient *) g_value_get_object(value);

      g_rec_mutex_lock(jack_port_mutex);

      if(jack_port->jack_client == (GObject *) jack_client){
	g_rec_mutex_unlock(jack_port_mutex);
	
	return;
      }

      if(jack_port->jack_client != NULL){
	g_object_unref(jack_port->jack_client);
      }

      if(jack_client != NULL){
	g_object_ref(jack_client);
      }
      
      jack_port->jack_client = (GObject *) jack_client;

      g_rec_mutex_unlock(jack_port_mutex);
    }
    break;
  case PROP_PORT_NAME:
    {
      gchar *port_name;

      port_name = g_value_get_string(value);

      g_rec_mutex_lock(jack_port_mutex);

      if(jack_port->port_name == port_name ||
	 !g_ascii_strcasecmp(jack_port->port_name,
			     port_name)){
	g_rec_mutex_unlock(jack_port_mutex);
	
	return;
      }

      if(jack_port->port_name != NULL){
	g_free(jack_port->port_name);
      }

      jack_port->port_name = g_strdup(port_name);

#ifdef AGS_WITH_JACK
      if(jack_port->port != NULL){
	jack_port_set_name(jack_port->port,
			   port_name);
      }      
#endif

      g_rec_mutex_unlock(jack_port_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_port_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsJackPort *jack_port;

  GRecMutex *jack_port_mutex;

  jack_port = AGS_JACK_PORT(gobject);

  /* get jack port mutex */
  jack_port_mutex = AGS_JACK_PORT_GET_OBJ_MUTEX(jack_port);
  
  switch(prop_id){
  case PROP_JACK_CLIENT:
    {
      g_rec_mutex_lock(jack_port_mutex);

      g_value_set_object(value, jack_port->jack_client);

      g_rec_mutex_unlock(jack_port_mutex);
    }
    break;
  case PROP_PORT_NAME:
    {
      g_rec_mutex_lock(jack_port_mutex);

      g_value_set_string(value, jack_port->port_name);

      g_rec_mutex_unlock(jack_port_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_port_dispose(GObject *gobject)
{
  AgsJackPort *jack_port;

  jack_port = AGS_JACK_PORT(gobject);

  /* jack client */
  if(jack_port->jack_client != NULL){
    g_object_unref(jack_port->jack_client);

    jack_port->jack_client = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_jack_port_parent_class)->dispose(gobject);
}

void
ags_jack_port_finalize(GObject *gobject)
{
  AgsJackPort *jack_port;

  jack_port = AGS_JACK_PORT(gobject);

  /* jack client */
  if(jack_port->jack_client != NULL){
    g_object_unref(jack_port->jack_client);
  }

  /* name */
  g_free(jack_port->port_name);

  /* call parent */
  G_OBJECT_CLASS(ags_jack_port_parent_class)->finalize(gobject);
}

AgsUUID*
ags_jack_port_get_uuid(AgsConnectable *connectable)
{
  AgsJackPort *jack_port;
  
  AgsUUID *ptr;

  GRecMutex *jack_port_mutex;

  jack_port = AGS_JACK_PORT(connectable);

  /* get jack port signal mutex */
  jack_port_mutex = AGS_JACK_PORT_GET_OBJ_MUTEX(jack_port);

  /* get UUID */
  g_rec_mutex_lock(jack_port_mutex);

  ptr = jack_port->uuid;

  g_rec_mutex_unlock(jack_port_mutex);
  
  return(ptr);
}

gboolean
ags_jack_port_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_jack_port_is_ready(AgsConnectable *connectable)
{
  AgsJackPort *jack_port;
  
  gboolean is_ready;

  GRecMutex *jack_port_mutex;

  jack_port = AGS_JACK_PORT(connectable);

  /* get jack port mutex */
  jack_port_mutex = AGS_JACK_PORT_GET_OBJ_MUTEX(jack_port);

  /* check is ready */
  g_rec_mutex_lock(jack_port_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (jack_port->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(jack_port_mutex);
  
  return(is_ready);
}

void
ags_jack_port_add_to_registry(AgsConnectable *connectable)
{
  AgsJackPort *jack_port;

  GRecMutex *jack_port_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  jack_port = AGS_JACK_PORT(connectable);

  /* get jack port mutex */
  jack_port_mutex = AGS_JACK_PORT_GET_OBJ_MUTEX(jack_port);

  g_rec_mutex_lock(jack_port_mutex);

  jack_port->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(jack_port_mutex);
}

void
ags_jack_port_remove_from_registry(AgsConnectable *connectable)
{
  AgsJackPort *jack_port;

  GRecMutex *jack_port_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  jack_port = AGS_JACK_PORT(connectable);

  /* get jack port mutex */
  jack_port_mutex = AGS_JACK_PORT_GET_OBJ_MUTEX(jack_port);

  g_rec_mutex_lock(jack_port_mutex);

  jack_port->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(jack_port_mutex);
}

xmlNode*
ags_jack_port_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_jack_port_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_jack_port_xml_parse(AgsConnectable *connectable,
		      xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_jack_port_is_connected(AgsConnectable *connectable)
{
  AgsJackPort *jack_port;
  
  gboolean is_connected;

  GRecMutex *jack_port_mutex;

  jack_port = AGS_JACK_PORT(connectable);

  /* get jack port mutex */
  jack_port_mutex = AGS_JACK_PORT_GET_OBJ_MUTEX(jack_port);

  /* check is connected */
  g_rec_mutex_lock(jack_port_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (jack_port->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(jack_port_mutex);
  
  return(is_connected);
}

void
ags_jack_port_connect(AgsConnectable *connectable)
{
  AgsJackPort *jack_port;

  GRecMutex *jack_port_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  jack_port = AGS_JACK_PORT(connectable);

  /* get jack port mutex */
  jack_port_mutex = AGS_JACK_PORT_GET_OBJ_MUTEX(jack_port);

  g_rec_mutex_lock(jack_port_mutex);

  jack_port->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(jack_port_mutex);
}

void
ags_jack_port_disconnect(AgsConnectable *connectable)
{
  AgsJackPort *jack_port;

  GRecMutex *jack_port_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  jack_port = AGS_JACK_PORT(connectable);

  /* get jack port mutex */
  jack_port_mutex = AGS_JACK_PORT_GET_OBJ_MUTEX(jack_port);

  g_rec_mutex_lock(jack_port_mutex);

  jack_port->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(jack_port_mutex);
}

/**
 * ags_jack_port_test_flags:
 * @jack_port: the #AgsJackPort
 * @flags: the flags
 *
 * Test @flags to be set on @jack_port.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_jack_port_test_flags(AgsJackPort *jack_port, AgsJackPortFlags flags)
{
  gboolean retval;  
  
  GRecMutex *jack_port_mutex;

  if(!AGS_IS_JACK_PORT(jack_port)){
    return(FALSE);
  }

  /* get jack port mutex */
  jack_port_mutex = AGS_JACK_PORT_GET_OBJ_MUTEX(jack_port);

  /* test */
  g_rec_mutex_lock(jack_port_mutex);

  retval = (flags & (jack_port->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(jack_port_mutex);

  return(retval);
}

/**
 * ags_jack_port_set_flags:
 * @jack_port: the #AgsJackPort
 * @flags: see #AgsJackPortFlags-enum
 *
 * Enable a feature of @jack_port.
 *
 * Since: 3.0.0
 */
void
ags_jack_port_set_flags(AgsJackPort *jack_port, AgsJackPortFlags flags)
{
  GRecMutex *jack_port_mutex;

  if(!AGS_IS_JACK_PORT(jack_port)){
    return;
  }

  /* get jack port mutex */
  jack_port_mutex = AGS_JACK_PORT_GET_OBJ_MUTEX(jack_port);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(jack_port_mutex);

  jack_port->flags |= flags;
  
  g_rec_mutex_unlock(jack_port_mutex);
}
    
/**
 * ags_jack_port_unset_flags:
 * @jack_port: the #AgsJackPort
 * @flags: see #AgsJackPortFlags-enum
 *
 * Disable a feature of @jack_port.
 *
 * Since: 3.0.0
 */
void
ags_jack_port_unset_flags(AgsJackPort *jack_port, AgsJackPortFlags flags)
{  
  GRecMutex *jack_port_mutex;

  if(!AGS_IS_JACK_PORT(jack_port)){
    return;
  }

  /* get jack port mutex */
  jack_port_mutex = AGS_JACK_PORT_GET_OBJ_MUTEX(jack_port);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(jack_port_mutex);

  jack_port->flags &= (~flags);
  
  g_rec_mutex_unlock(jack_port_mutex);
}

/**
 * ags_jack_port_find:
 * @jack_port: (element-type AgsAudio.JackPort) (transfer none): the #GList-struct containing #AgsJackPort
 * @port_name: the port name to find
 *
 * Finds next match of @port_name in @jack_port.
 *
 * Returns: (element-type AgsAudio.JackPort) (transfer none): the next matching #GList-struct or %NULL
 * 
 * Since: 3.0.0
 */
GList*
ags_jack_port_find(GList *jack_port,
		   gchar *port_name)
{
#ifdef AGS_WITH_JACK
  jack_port_t *port;
#else
  gpointer port;
#endif

  GRecMutex *jack_port_mutex;

#ifdef AGS_WITH_JACK
  while(jack_port != NULL){
    /* get jack port mutex */
    jack_port_mutex = AGS_JACK_PORT_GET_OBJ_MUTEX(jack_port->data);

    /* get port */
    g_rec_mutex_lock(jack_port_mutex);

    port = AGS_JACK_PORT(jack_port->data)->port;
    
    g_rec_mutex_unlock(jack_port_mutex);
    
    if(!g_ascii_strcasecmp(jack_port_name(port),
			   port_name)){
      return(jack_port);
    }

    jack_port = jack_port->next;
  }
#endif

  return(NULL);
}

/**
 * ags_jack_port_register:
 * @jack_port: the #AgsJackPort
 * @port_name: the name as string
 * @is_audio: if %TRUE interpreted as audio port
 * @is_midi: if %TRUE interpreted as midi port
 * @is_output: if %TRUE port is acting as output, otherwise as input
 *
 * Register a new JACK port and read uuid. Creates a new AgsSequencer or AgsSoundcard
 * object.
 *
 * Since: 3.0.0
 */
void
ags_jack_port_register(AgsJackPort *jack_port,
		       gchar *port_name,
		       gboolean is_audio, gboolean is_midi,
		       gboolean is_output)
{
  AgsJackServer *jack_server;
  AgsJackClient *jack_client;
  
#ifdef AGS_WITH_JACK
  jack_client_t *client;
  jack_port_t *port;
#else
  gpointer client;
  gpointer port;
#endif
  
  GList *list;

  gchar *port_uuid;

  GRecMutex *jack_client_mutex;
  GRecMutex *jack_port_mutex;
  
  if(!AGS_IS_JACK_PORT(jack_port) ||
     port_name == NULL){
    return;
  }

  g_object_get(jack_port,
	       "jack-client", &jack_client,
	       NULL);
  
  if(jack_client == NULL){
    g_warning("ags_jack_port.c - no assigned AgsJackClient");
    
    return;
  }

  if(ags_jack_port_test_flags(jack_port, AGS_JACK_PORT_REGISTERED)){
    g_object_unref(jack_client);
    
    return;
  }

  /* get jack server and application context */
  g_object_get(jack_client,
	       "jack-server", &jack_server,
	       NULL);
  
  if(jack_server == NULL){
    g_object_unref(jack_client);

    return;
  }

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* get client */
  g_rec_mutex_lock(jack_client_mutex);

  client = jack_client->client;

  g_rec_mutex_unlock(jack_client_mutex);
  
  if(client == NULL){
    g_object_unref(jack_client);

    g_object_unref(jack_server);

    return;
  }

  /* get jack port mutex */
  jack_port_mutex = AGS_JACK_PORT_GET_OBJ_MUTEX(jack_port);

  /* get port name */
  //FIXME:JK: memory leak?
  g_rec_mutex_lock(jack_port_mutex);
  
  jack_port->port_name = g_strdup(port_name);

  g_rec_mutex_unlock(jack_port_mutex);

  /* create sequencer or soundcard */
  if(is_output){
    ags_jack_port_set_flags(jack_port, AGS_JACK_PORT_IS_OUTPUT);
  }else{
    ags_jack_port_set_flags(jack_port, AGS_JACK_PORT_IS_INPUT);
  }

#ifdef AGS_WITH_JACK
  port = NULL;
  
  if(is_audio){
    ags_jack_port_set_flags(jack_port, AGS_JACK_PORT_IS_AUDIO);
    
    port = jack_port_register(client,
			      jack_port->port_name,
			      JACK_DEFAULT_AUDIO_TYPE,
			      (is_output ? JackPortIsOutput: JackPortIsInput),
			      0);
  }else if(is_midi){
    ags_jack_port_set_flags(jack_port, AGS_JACK_PORT_IS_MIDI);
    
    port = jack_port_register(client,
			      jack_port->port_name,
			      JACK_DEFAULT_MIDI_TYPE,
			      (is_output ? JackPortIsOutput: JackPortIsInput),
			      0);
  }

  g_rec_mutex_lock(jack_port_mutex);

  jack_port->port = port;
  
  g_rec_mutex_unlock(jack_port_mutex);

  if(port != NULL){
    ags_jack_port_set_flags(jack_port, AGS_JACK_PORT_REGISTERED);
  }
    
#ifdef HAVE_JACK_PORT_UUID
  if(port != NULL){
    port_uuid = jack_port_uuid(port);

    g_rec_mutex_lock(jack_port_mutex);

    jack_port->port_uuid = g_strdup(port_uuid);
  
    g_rec_mutex_unlock(jack_port_mutex);    
  }
#endif
#endif

  g_object_unref(jack_client);

  g_object_unref(jack_server);
}

/**
 * ags_jack_port_unregister:
 * @jack_port: the #AgsJackPort
 * 
 * Unregister JACK port.
 * 
 * Since: 3.0.0
 */
void
ags_jack_port_unregister(AgsJackPort *jack_port)
{
  AgsJackClient *jack_client;
  
#ifdef AGS_WITH_JACK
  jack_client_t *client;
  jack_port_t *port;
#else
  gpointer client;
  gpointer port;
#endif

  GRecMutex *jack_client_mutex;
  GRecMutex *jack_port_mutex;

  if(!AGS_IS_JACK_PORT(jack_port)){
    return;
  }

  /* get jack port mutex */
  jack_port_mutex = AGS_JACK_PORT_GET_OBJ_MUTEX(jack_port);

  /* get port */
  g_rec_mutex_lock(jack_port_mutex);

  port = jack_port->port;
  
  g_rec_mutex_unlock(jack_port_mutex);

  if(port == NULL){
    return;
  }
  
  /* get jack client */
  g_object_get(jack_port,
	       "jack-client", &jack_client,
	       NULL);

  if(jack_client == NULL){
    return;
  }

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* get client */
  g_rec_mutex_lock(jack_client_mutex);

  client = jack_client->client;

  g_rec_mutex_unlock(jack_client_mutex);

  if(client == NULL){
    g_object_unref(jack_client);
    
    return;
  }

#ifdef AGS_WITH_JACK
  jack_port_unregister(client,
		       port);

  /* unset port and flags */
  g_rec_mutex_lock(jack_port_mutex);

  jack_port->port = NULL;

  g_rec_mutex_unlock(jack_port_mutex);

  ags_jack_port_unset_flags(jack_port, AGS_JACK_PORT_REGISTERED);
#endif

  g_object_unref(jack_client);
}

/**
 * ags_jack_port_new:
 * @jack_client: the #AgsJackClient assigned to
 *
 * Create a new instance of #AgsJackPort.
 *
 * Returns: the new #AgsJackPort
 *
 * Since: 3.0.0
 */
AgsJackPort*
ags_jack_port_new(GObject *jack_client)
{
  AgsJackPort *jack_port;

  jack_port = (AgsJackPort *) g_object_new(AGS_TYPE_JACK_PORT,
					   "jack-client", jack_client,
					   NULL);

  return(jack_port);
}
