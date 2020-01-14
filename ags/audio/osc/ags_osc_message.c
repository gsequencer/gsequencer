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

#include <ags/audio/osc/ags_osc_message.h>

#include <ags/i18n.h>

void ags_osc_message_class_init(AgsOscMessageClass *osc_message);
void ags_osc_message_init(AgsOscMessage *osc_message);
void ags_osc_message_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_osc_message_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_osc_message_dispose(GObject *gobject);
void ags_osc_message_finalize(GObject *gobject);

/**
 * SECTION:ags_osc_message
 * @short_description: the OSC server side message
 * @title: AgsOscMessage
 * @section_id:
 * @include: ags/audio/osc/ags_osc_message.h
 *
 * #AgsOscMessage your OSC server side message.
 */

enum{
  PROP_0,
  PROP_OSC_CONNECTION,
  PROP_TV_SEC,
  PROP_TV_FRACTION,
  PROP_IMMEDIATELY,
  PROP_MESSAGE_SIZE,
  PROP_MESSAGE,
};

static gpointer ags_osc_message_parent_class = NULL;

GType
ags_osc_message_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_message = 0;

    static const GTypeInfo ags_osc_message_info = {
      sizeof (AgsOscMessageClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_message_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscMessage),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_message_init,
    };

    ags_type_osc_message = g_type_register_static(G_TYPE_OBJECT,
						  "AgsOscMessage", &ags_osc_message_info,
						  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_message);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_message_class_init(AgsOscMessageClass *osc_message)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_osc_message_parent_class = g_type_class_peek_parent(osc_message);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_message;

  gobject->set_property = ags_osc_message_set_property;
  gobject->get_property = ags_osc_message_get_property;
  
  gobject->dispose = ags_osc_message_dispose;
  gobject->finalize = ags_osc_message_finalize;

  /* properties */
  /**
   * AgsOscMessage:osc-connection:
   *
   * The assigned #AgsOscConnection.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("osc-connection",
				   i18n_pspec("assigned OSC connection"),
				   i18n_pspec("The OSC connection it is assigned with"),
				   AGS_TYPE_OSC_CONNECTION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OSC_CONNECTION,
				  param_spec);

  /**
   * AgsOscMessage:tv-sec:
   *
   * The time value in seconds.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_int("tv-sec",
				i18n_pspec("time value seconds"),
				i18n_pspec("The time value in seconds"),
				0,
				G_MAXINT32,
				0,
				G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TV_SEC,
				  param_spec);

  /**
   * AgsOscMessage:tv-fraction:
   *
   * The time value's fraction.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_int("tv-fraction",
				i18n_pspec("time value fraction"),
				i18n_pspec("The fraction of time value"),
				0,
				G_MAXINT32,
				0,
				G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TV_FRACTION,
				  param_spec);

  /**
   * AgsOscMessage:immediately:
   *
   * The immediately switch.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_boolean("immediately",
				    i18n_pspec("immediately"),
				    i18n_pspec("The immediately switch"),
				    FALSE,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_IMMEDIATELY,
				  param_spec);

  /**
   * AgsOscMessage:message-size:
   *
   * The size of message in bytes.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("message-size",
				 i18n_pspec("message size"),
				 i18n_pspec("The size of message"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MESSAGE_SIZE,
				  param_spec);

  /**
   * AgsOscMessage:message:
   *
   * The message data.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("message",
				    i18n_pspec("message"),
				    i18n_pspec("The IPv6 address of the server"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MESSAGE,
				  param_spec);
}

void
ags_osc_message_init(AgsOscMessage *osc_message)
{
  /* osc message mutex */
  g_rec_mutex_init(&(osc_message->obj_mutex));

  osc_message->osc_connection = NULL;
  
  osc_message->tv_sec = 0;
  osc_message->tv_fraction = 0;
  osc_message->immediately = FALSE;

  osc_message->message_size = 0;
  osc_message->message = NULL; 
}

void
ags_osc_message_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsOscMessage *osc_message;

  GRecMutex *osc_message_mutex;

  osc_message = AGS_OSC_MESSAGE(gobject);

  /* get osc message mutex */
  osc_message_mutex = AGS_OSC_MESSAGE_GET_OBJ_MUTEX(osc_message);
  
  switch(prop_id){
  case PROP_OSC_CONNECTION:
    {
      GObject *osc_connection;

      osc_connection = g_value_get_object(value);

      g_rec_mutex_lock(osc_message_mutex);

      if(osc_message->osc_connection == osc_connection){
	g_rec_mutex_unlock(osc_message_mutex);

	return;
      }

      if(osc_message->osc_connection != NULL){
	g_object_unref(osc_message->osc_connection);
      }
      
      if(osc_connection != NULL){
	g_object_ref(osc_connection);
      }
      
      osc_message->osc_connection = osc_connection;

      g_rec_mutex_unlock(osc_message_mutex);
    }
    break;
  case PROP_TV_SEC:
    {
      gint32 tv_sec;

      tv_sec = (gint32) g_value_get_int(value);

      g_rec_mutex_lock(osc_message_mutex);
      
      osc_message->tv_sec = tv_sec;

      g_rec_mutex_unlock(osc_message_mutex);
    }
    break;
  case PROP_TV_FRACTION:
    {
      gint32 tv_fraction;

      tv_fraction = (gint32) g_value_get_int(value);

      g_rec_mutex_lock(osc_message_mutex);
      
      osc_message->tv_fraction = tv_fraction;

      g_rec_mutex_unlock(osc_message_mutex);
    }
    break;
  case PROP_IMMEDIATELY:
    {
      gboolean immediately;

      immediately = g_value_get_boolean(value);

      g_rec_mutex_lock(osc_message_mutex);
      
      osc_message->immediately = immediately;

      g_rec_mutex_unlock(osc_message_mutex);
    }
    break;
  case PROP_MESSAGE_SIZE:
    {
      guint message_size;

      message_size = g_value_get_uint(value);

      g_rec_mutex_lock(osc_message_mutex);
      
      osc_message->message_size = message_size;

      g_rec_mutex_unlock(osc_message_mutex);
    }
    break;
  case PROP_MESSAGE:
    {
      guchar *message;

      message = g_value_get_pointer(value);

      g_rec_mutex_lock(osc_message_mutex);
      
      osc_message->message = message;

      g_rec_mutex_unlock(osc_message_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_message_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsOscMessage *osc_message;

  GRecMutex *osc_message_mutex;

  osc_message = AGS_OSC_MESSAGE(gobject);

  /* get osc message mutex */
  osc_message_mutex = AGS_OSC_MESSAGE_GET_OBJ_MUTEX(osc_message);
  
  switch(prop_id){
  case PROP_OSC_CONNECTION:
    {
      g_rec_mutex_lock(osc_message_mutex);

      g_value_set_object(value, osc_message->osc_connection);

      g_rec_mutex_unlock(osc_message_mutex);
    }
    break;
  case PROP_TV_SEC:
    {
      g_rec_mutex_lock(osc_message_mutex);
      
      g_value_set_int(value,
		      (gint) osc_message->tv_sec);

      g_rec_mutex_unlock(osc_message_mutex);
    }
    break;    
  case PROP_TV_FRACTION:
    {
      g_rec_mutex_lock(osc_message_mutex);
      
      g_value_set_int(value,
		      (gint) osc_message->tv_fraction);

      g_rec_mutex_unlock(osc_message_mutex);
    }
    break;
  case PROP_IMMEDIATELY:
    {
      g_rec_mutex_lock(osc_message_mutex);
      
      g_value_set_boolean(value,
			  osc_message->immediately);

      g_rec_mutex_unlock(osc_message_mutex);
    }
    break;    
  case PROP_MESSAGE_SIZE:
    {
      g_rec_mutex_lock(osc_message_mutex);
      
      g_value_set_uint(value,
		       osc_message->message_size);

      g_rec_mutex_unlock(osc_message_mutex);
    }
    break;    
  case PROP_MESSAGE:
    {
      g_rec_mutex_lock(osc_message_mutex);
      
      g_value_set_pointer(value,
			  osc_message->message);

      g_rec_mutex_unlock(osc_message_mutex);
    }
    break;    
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_message_dispose(GObject *gobject)
{
  AgsOscMessage *osc_message;
    
  osc_message = (AgsOscMessage *) gobject;
  
  if(osc_message->osc_connection != NULL){
    g_object_unref(osc_message->osc_connection);

    osc_message->osc_connection = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_osc_message_parent_class)->dispose(gobject);
}

void
ags_osc_message_finalize(GObject *gobject)
{
  AgsOscMessage *osc_message;
    
  osc_message = (AgsOscMessage *) gobject;

  if(osc_message->osc_connection != NULL){
    g_object_unref(osc_message->osc_connection);
  }
  
  if(osc_message->message != NULL){
    free(osc_message->message);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_message_parent_class)->finalize(gobject);
}

/**
 * ags_osc_front_controller_message_sort_func:
 * @a: the #AgsOscFrontControllerMessage-struct
 * @b: the other #AgsOscFrontControllerMessage-struct
 * 
 * Compare @a and @b in view of timing.
 *
 * Returns: -1 if @a happens before @b, 0 if at the very same time or 1 if after
 * 
 * Since: 3.0.0
 */
gint
ags_osc_message_sort_func(gconstpointer a,
			  gconstpointer b)
{
  AgsOscMessage *message_a, *message_b;

  if(a == NULL || b == NULL){
    return(0);
  }

  message_a = a;
  message_b = b;

  if(message_a->immediately &&
     message_b->immediately){
    return(0);
  }

  if(message_a->immediately){
    return(-1);
  }
  
  if(message_b->immediately){
    return(1);
  }

  if(message_a->tv_sec < message_b->tv_sec ||
     (message_a->tv_sec == message_b->tv_sec &&
      message_a->tv_fraction < message_b->tv_fraction)){
    return(-1);
  }
  
  if(message_a->tv_sec > message_b->tv_sec ||
     (message_a->tv_sec == message_b->tv_sec &&
      message_a->tv_fraction > message_b->tv_fraction)){
    return(1);
  }

  return(0);
}

/**
 * ags_osc_message_new:
 * 
 * Creates a new instance of #AgsOscMessage
 *
 * Returns: the new #AgsOscMessage
 * 
 * Since: 3.0.0
 */
AgsOscMessage*
ags_osc_message_new()
{
  AgsOscMessage *osc_message;
   
  osc_message = (AgsOscMessage *) g_object_new(AGS_TYPE_OSC_MESSAGE,
					       NULL);
  
  return(osc_message);
}
