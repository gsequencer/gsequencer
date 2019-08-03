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

#include <ags/audio/osc/ags_osc_response.h>

#include <ags/libags.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_osc_response_class_init(AgsOscResponseClass *osc_response);
void ags_osc_response_init(AgsOscResponse *osc_response);
void ags_osc_response_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_osc_response_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_osc_response_dispose(GObject *gobject);
void ags_osc_response_finalize(GObject *gobject);

/**
 * SECTION:ags_osc_response
 * @short_description: the OSC server side response
 * @title: AgsOscResponse
 * @section_id:
 * @include: ags/audio/osc/ags_osc_response.h
 *
 * #AgsOscResponse your OSC server side response.
 */

enum{
  PROP_0,
  PROP_PACKET,
  PROP_PACKET_SIZE,
  PROP_ERROR_MESSAGE,
};

static gpointer ags_osc_response_parent_class = NULL;

static pthread_mutex_t ags_osc_response_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_osc_response_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_response = 0;

    static const GTypeInfo ags_osc_response_info = {
      sizeof (AgsOscResponseClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_response_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscResponse),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_response_init,
    };

    ags_type_osc_response = g_type_register_static(G_TYPE_OBJECT,
						   "AgsOscResponse", &ags_osc_response_info,
						   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_response);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_response_class_init(AgsOscResponseClass *osc_response)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_osc_response_parent_class = g_type_class_peek_parent(osc_response);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_response;

  gobject->set_property = ags_osc_response_set_property;
  gobject->get_property = ags_osc_response_get_property;
  
  gobject->dispose = ags_osc_response_dispose;
  gobject->finalize = ags_osc_response_finalize;

  /* properties */
  /**
   * AgsOscResponse:packet:
   *
   * The response packet.
   * 
   * Since: 2.1.0
   */
  param_spec = g_param_spec_pointer("packet",
				    i18n_pspec("response packet"),
				    i18n_pspec("The response packet"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PACKET,
				  param_spec);
  
  /**
   * AgsOscResponse:packet-size:
   *
   * The response packet's size.
   * 
   * Since: 2.1.0
   */
  param_spec = g_param_spec_uint("packet-size",
				 i18n_pspec("response packet size"),
				 i18n_pspec("The response packet size"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PACKET_SIZE,
				  param_spec);

  /**
   * AgsOscResponse:error-message:
   *
   * The error message.
   * 
   * Since: 2.1.8
   */
  param_spec = g_param_spec_string("error-message",
				   i18n_pspec("error message"),
				   i18n_pspec("The error message"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ERROR_MESSAGE,
				  param_spec);
}

void
ags_osc_response_init(AgsOscResponse *osc_response)
{
  osc_response->flags = 0;
  
  /* osc response mutex */
  osc_response->obj_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(osc_response->obj_mutexattr);
  pthread_mutexattr_settype(osc_response->obj_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(osc_response->obj_mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  osc_response->obj_mutex =  (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(osc_response->obj_mutex,
		     osc_response->obj_mutexattr);

  osc_response->packet = NULL;
  osc_response->packet_size = 0;

  osc_response->error_message = NULL;
}

void
ags_osc_response_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsOscResponse *osc_response;

  pthread_mutex_t *osc_response_mutex;

  osc_response = AGS_OSC_RESPONSE(gobject);

  /* get osc response mutex */
  osc_response_mutex = AGS_OSC_RESPONSE_GET_OBJ_MUTEX(osc_response);
  
  switch(prop_id){
  case PROP_PACKET:
    {
      gpointer packet;

      packet = g_value_get_pointer(value);

      pthread_mutex_lock(osc_response_mutex);

      if(osc_response->packet == packet){
	pthread_mutex_unlock(osc_response_mutex);

	return;
      }
      
      osc_response->packet = packet;

      pthread_mutex_unlock(osc_response_mutex);
    }
    break;
  case PROP_PACKET_SIZE:
    {
      guint packet_size;

      packet_size = g_value_get_uint(value);

      pthread_mutex_lock(osc_response_mutex);

      osc_response->packet_size = packet_size;

      pthread_mutex_unlock(osc_response_mutex);
    }
    break;
  case PROP_ERROR_MESSAGE:
    {
      gchar *error_message;

      error_message = g_value_get_string(value);

      pthread_mutex_lock(osc_response_mutex);

      if(osc_response->error_message == error_message){
	pthread_mutex_unlock(osc_response_mutex);

	return;
      }

      g_free(osc_response->error_message);
      
      osc_response->error_message = g_strdup(error_message);

      pthread_mutex_unlock(osc_response_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_response_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsOscResponse *osc_response;

  pthread_mutex_t *osc_response_mutex;

  osc_response = AGS_OSC_RESPONSE(gobject);

  /* get osc response mutex */
  osc_response_mutex = AGS_OSC_RESPONSE_GET_OBJ_MUTEX(osc_response);
  
  switch(prop_id){
  case PROP_PACKET:
    {
      pthread_mutex_lock(osc_response_mutex);

      g_value_set_pointer(value, osc_response->packet);

      pthread_mutex_unlock(osc_response_mutex);
    }
    break;
  case PROP_PACKET_SIZE:
    {
      pthread_mutex_lock(osc_response_mutex);
      
      g_value_set_uint(value,
		       osc_response->packet_size);
      
      pthread_mutex_unlock(osc_response_mutex);
    }
    break;
  case PROP_ERROR_MESSAGE:
    {
      pthread_mutex_lock(osc_response_mutex);

      g_value_set_pointer(value, osc_response->error_message);

      pthread_mutex_unlock(osc_response_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_response_dispose(GObject *gobject)
{
  AgsOscResponse *osc_response;
    
  osc_response = (AgsOscResponse *) gobject;

  /* call parent */
  G_OBJECT_CLASS(ags_osc_response_parent_class)->dispose(gobject);
}

void
ags_osc_response_finalize(GObject *gobject)
{
  AgsOscResponse *osc_response;
    
  osc_response = (AgsOscResponse *) gobject;

  pthread_mutex_destroy(osc_response->obj_mutex);
  free(osc_response->obj_mutex);

  pthread_mutexattr_destroy(osc_response->obj_mutexattr);
  free(osc_response->obj_mutexattr);

  if(osc_response->packet != NULL){
    free(osc_response->packet);
  }

  g_free(osc_response->error_message);

  /* call parent */
  G_OBJECT_CLASS(ags_osc_response_parent_class)->finalize(gobject);
}

/**
 * ags_osc_response_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.1.0
 */
pthread_mutex_t*
ags_osc_response_get_class_mutex()
{
  return(&ags_osc_response_class_mutex);
}

/**
 * ags_osc_response_test_flags:
 * @osc_response: the #AgsOscResponse
 * @flags: the flags
 *
 * Test @flags to be set on @osc_response.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.1.0
 */
gboolean
ags_osc_response_test_flags(AgsOscResponse *osc_response, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *osc_response_mutex;

  if(!AGS_IS_OSC_RESPONSE(osc_response)){
    return(FALSE);
  }

  /* get osc_response mutex */
  osc_response_mutex = AGS_OSC_RESPONSE_GET_OBJ_MUTEX(osc_response);

  /* test */
  pthread_mutex_lock(osc_response_mutex);

  retval = (flags & (osc_response->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(osc_response_mutex);

  return(retval);
}

/**
 * ags_osc_response_set_flags:
 * @osc_response: the #AgsOscResponse
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 2.1.0
 */
void
ags_osc_response_set_flags(AgsOscResponse *osc_response, guint flags)
{
  pthread_mutex_t *osc_response_mutex;

  if(!AGS_IS_OSC_RESPONSE(osc_response)){
    return;
  }

  /* get osc_response mutex */
  osc_response_mutex = AGS_OSC_RESPONSE_GET_OBJ_MUTEX(osc_response);

  /* set flags */
  pthread_mutex_lock(osc_response_mutex);

  osc_response->flags |= flags;

  pthread_mutex_unlock(osc_response_mutex);
}

/**
 * ags_osc_response_unset_flags:
 * @osc_response: the #AgsOscResponse
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 2.1.0
 */
void
ags_osc_response_unset_flags(AgsOscResponse *osc_response, guint flags)
{
  pthread_mutex_t *osc_response_mutex;

  if(!AGS_IS_OSC_RESPONSE(osc_response)){
    return;
  }

  /* get osc_response mutex */
  osc_response_mutex = AGS_OSC_RESPONSE_GET_OBJ_MUTEX(osc_response);

  /* set flags */
  pthread_mutex_lock(osc_response_mutex);

  osc_response->flags &= (~flags);

  pthread_mutex_unlock(osc_response_mutex);
}

/**
 * ags_osc_response_new:
 * 
 * Creates a new instance of #AgsOscResponse
 *
 * Returns: the new #AgsOscResponse
 * 
 * Since: 2.1.0
 */
AgsOscResponse*
ags_osc_response_new()
{
  AgsOscResponse *osc_response;
   
  osc_response = (AgsOscResponse *) g_object_new(AGS_TYPE_OSC_RESPONSE,
						 NULL);
  
  return(osc_response);
}
