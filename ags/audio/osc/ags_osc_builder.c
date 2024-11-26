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

#include <ags/audio/osc/ags_osc_builder.h>

#include <ags/audio/osc/ags_osc_util.h>
#include <ags/audio/osc/ags_osc_buffer_util.h>

#include <stdlib.h>
#include <string.h>

#include <ags/i18n.h>

void ags_osc_builder_class_init(AgsOscBuilderClass *osc_builder);
void ags_osc_builder_init(AgsOscBuilder *osc_builder);
void ags_osc_builder_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_osc_builder_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_osc_builder_finalize(GObject *gobject);

void ags_osc_builder_real_osc_putc(AgsOscBuilder *osc_builder,
				   gint c);
void ags_osc_builder_real_on_error(AgsOscBuilder *osc_builder,
				   GError **error);

void ags_osc_builder_real_append_packet(AgsOscBuilder *osc_builder);

void ags_osc_builder_real_append_bundle(AgsOscBuilder *osc_builder,
					AgsOscBuilderBundle *parent_bundle,
					gint tv_secs, gint tv_fraction, gboolean immediately);

void ags_osc_builder_real_append_message(AgsOscBuilder *osc_builder,
					 AgsOscBuilderBundle *parent_bundle,
					 gchar *address_pattern,
					 gchar *type_tag);

void ags_osc_builder_real_append_value(AgsOscBuilder *osc_builder,
				       AgsOscBuilderMessage *message,
				       gint v_type,
				       GValue *value);

void ags_osc_builder_skip_empty_types(AgsOscBuilder *osc_builder);
void ags_osc_builder_bundle_resize(AgsOscBuilder *osc_builder,
				   AgsOscBuilderBundle *parent_bundle,
				   gsize grow_data);
void ags_osc_builder_message_check_resize(AgsOscBuilder *osc_builder,
					  AgsOscBuilderMessage *message,
					  gsize append_size);

void ags_osc_builder_build_bundle(AgsOscBuilder *osc_builder,
				  AgsOscBuilderBundle *bundle,
				  guchar *data,
				  gsize *offset);
void ags_osc_builder_build_message(AgsOscBuilder *osc_builder,
				   AgsOscBuilderMessage *message,
				   guchar *data,
				   gsize *offset);

/**
 * SECTION:ags_osc_builder
 * @short_description: OSC builder
 * @title: AgsOscBuilder
 * @section_id:
 * @include: ags/audio/osc/ags_osc_builder.h
 *
 * The #AgsOscBuilder lets you create OSC files using its builder functions.
 */

enum{
  PROP_0,
};

enum{
  OSC_PUTC,
  ON_ERROR,
  APPEND_PACKET,
  APPEND_BUNDLE,
  APPEND_MESSAGE,
  APPEND_VALUE,
  LAST_SIGNAL,
};

static gpointer ags_osc_builder_parent_class = NULL;

static guint osc_builder_signals[LAST_SIGNAL];

GType
ags_osc_builder_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_osc_builder = 0;

    static const GTypeInfo ags_osc_builder_info = {
      sizeof (AgsOscBuilderClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_builder_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscBuilder),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_builder_init,
    };

    ags_type_osc_builder = g_type_register_static(G_TYPE_OBJECT,
						  "AgsOscBuilder", &ags_osc_builder_info,
						  0);

    g_once_init_leave(&g_define_type_id__static, ags_type_osc_builder);
  }

  return(g_define_type_id__static);
}

void
ags_osc_builder_class_init(AgsOscBuilderClass *osc_builder)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_osc_builder_parent_class = g_type_class_peek_parent(osc_builder);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_builder;
  
  gobject->set_property = ags_osc_builder_set_property;
  gobject->get_property = ags_osc_builder_get_property;  

  gobject->finalize = ags_osc_builder_finalize;

  /* properties */

  /* AgsOscBuilderClass */
  osc_builder->osc_putc = ags_osc_builder_real_osc_putc;
  osc_builder->on_error = ags_osc_builder_real_on_error;

  osc_builder->append_packet = ags_osc_builder_real_append_packet;
  osc_builder->append_bundle = ags_osc_builder_real_append_bundle;
  osc_builder->append_message = ags_osc_builder_real_append_message;
  osc_builder->append_value = ags_osc_builder_real_append_value;

  /* signals */
  /**
   * AgsOscBuilder::osc-putc:
   * @osc_builder: the builder
   * @error: the #GError
   *
   * The ::osc-putc signal is emited during putting char to file.
   *
   * Since: 3.0.0
   */
  osc_builder_signals[OSC_PUTC] =
    g_signal_new("osc-putc",
		 G_TYPE_FROM_CLASS(osc_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscBuilderClass, osc_putc),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__INT,
		 G_TYPE_NONE, 1,
		 G_TYPE_INT);

  /**
   * AgsOscBuilder::on-error:
   * @osc_builder: the builder
   * @error: the #GError
   *
   * The ::on-error signal is emited during building of event.
   *
   * Since: 3.0.0
   */
  osc_builder_signals[ON_ERROR] =
    g_signal_new("on-error",
		 G_TYPE_FROM_CLASS(osc_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscBuilderClass, on_error),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__POINTER,
		 G_TYPE_NONE, 1,
		 G_TYPE_POINTER);

  /**
   * AgsOscBuilder::append-packet:
   * @osc_builder: the builder
   *
   * The ::append-packet signal is emited during building packet.
   *
   * Since: 3.0.0
   */
  osc_builder_signals[APPEND_PACKET] =
    g_signal_new("append-packet",
		 G_TYPE_FROM_CLASS(osc_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscBuilderClass, append_packet),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsOscBuilder::append-bundle:
   * @osc_builder: the builder
   * @parent_bundle: the parent #AgsOscBuilderBundle-struct or %NULL
   * @tv_secs: time value secondes since midnight January 1900
   * @tv_fraction: time value fractions of second
   * @immediately: if %TRUE apply immediately, otherwise %FALSE
   *
   * The ::append-bundle signal is emited during building bundle.
   *
   * Since: 3.0.0
   */
  osc_builder_signals[APPEND_BUNDLE] =
    g_signal_new("append-bundle",
		 G_TYPE_FROM_CLASS(osc_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscBuilderClass, append_bundle),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__POINTER_INT_INT_BOOLEAN,
		 G_TYPE_NONE, 4,
		 G_TYPE_POINTER,
		 G_TYPE_INT,
		 G_TYPE_INT,
		 G_TYPE_BOOLEAN);

  /**
   * AgsOscBuilder::append-message:
   * @osc_builder: the builder
   * @parent_bundle: the parent #AgsOscBuilderBundle-struct or %NULL
   * @address_pattern: the address pattern string
   * @type_tag: the type tag string
   *
   * The ::append-message signal is emited during building message.
   *
   * Since: 3.0.0
   */
  osc_builder_signals[APPEND_MESSAGE] =
    g_signal_new("append-message",
		 G_TYPE_FROM_CLASS(osc_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscBuilderClass, append_message),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__POINTER_POINTER_POINTER,
		 G_TYPE_NONE, 3,
		 G_TYPE_POINTER,
		 G_TYPE_POINTER,
		 G_TYPE_POINTER);

  /**
   * AgsOscBuilder::append-value:
   * @osc_builder: the builder
   * @message: the message
   * @v_type: the type as ASCII char
   * @value: the #GValue-struct
   *
   * The ::append-value signal is emited during building value.
   *
   * Since: 3.0.0
   */
  osc_builder_signals[APPEND_VALUE] =
    g_signal_new("append-value",
		 G_TYPE_FROM_CLASS(osc_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscBuilderClass, append_value),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__POINTER_INT_POINTER,
		 G_TYPE_NONE, 2,
		 G_TYPE_INT,
		 G_TYPE_POINTER);
}

void
ags_osc_builder_init(AgsOscBuilder *osc_builder)
{
  osc_builder->flags = 0;

  /* osc builder mutex */
  g_rec_mutex_init(&(osc_builder->obj_mutex));

  osc_builder->data = NULL;
  osc_builder->length = 0;

  osc_builder->offset = 0;
  
  osc_builder->packet = NULL;

  osc_builder->current_type_tag = NULL;
  osc_builder->offset_type_tag = NULL;
}

void
ags_osc_builder_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsOscBuilder *osc_builder;

  GRecMutex *osc_builder_mutex;

  osc_builder = AGS_OSC_BUILDER(gobject);

  /* get osc builder mutex */
  osc_builder_mutex = AGS_OSC_BUILDER_GET_OBJ_MUTEX(osc_builder);
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_builder_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsOscBuilder *osc_builder;

  GRecMutex *osc_builder_mutex;

  osc_builder = AGS_OSC_BUILDER(gobject);

  /* get osc builder mutex */
  osc_builder_mutex = AGS_OSC_BUILDER_GET_OBJ_MUTEX(osc_builder);
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_builder_finalize(GObject *gobject)
{
  AgsOscBuilder *osc_builder;
    
  osc_builder = (AgsOscBuilder *) gobject;

  if(osc_builder->data != NULL){
    free(osc_builder->data);
  }
  
  g_list_free_full(osc_builder->packet,
		   (GDestroyNotify) ags_osc_builder_packet_free);
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_builder_parent_class)->finalize(gobject);
}

void
ags_osc_builder_skip_empty_types(AgsOscBuilder *osc_builder)
{
  gchar *iter;
  
  if(!AGS_IS_OSC_BUILDER(osc_builder)){
    return;
  }

  for(iter = osc_builder->offset_type_tag; iter[0] != '\0'; iter++){
    if(!(iter[0] == 'T' || iter[0] == 'F' || iter[0] == 'N' || iter[0] == 'I' || iter[0] == '[' || iter[0] == ']')){
      break;
    }
  }

  if(iter[0] != '\0'){
    osc_builder->offset_type_tag = iter;
  }else{
    osc_builder->current_type_tag = NULL;
    osc_builder->offset_type_tag = NULL;
  }
}

void
ags_osc_builder_bundle_resize(AgsOscBuilder *osc_builder,
			      AgsOscBuilderBundle *parent_bundle,
			      gsize grow_data)
{
  if(parent_bundle == NULL){
    return;
  }
  
  do{
    parent_bundle->bundle_size += grow_data;

    parent_bundle = parent_bundle->parent_bundle;
  }while(parent_bundle->parent_bundle != NULL);

  parent_bundle->packet->packet_size += grow_data;
}

void
ags_osc_builder_message_check_resize(AgsOscBuilder *osc_builder,
				     AgsOscBuilderMessage *message,
				     gsize append_size)
{
  if(message->data_length + append_size > message->data_allocated_length){
    gsize new_data_allocated_length;

    new_data_allocated_length = message->data_allocated_length + AGS_OSC_BUILDER_MESSAGE_DEFAULT_CHUNK_SIZE;

    if(message->data == NULL){
      message->data = (guchar *) malloc(AGS_OSC_BUILDER_MESSAGE_DEFAULT_CHUNK_SIZE * sizeof(guchar));
      memset(message->data,
	     0,
	     AGS_OSC_BUILDER_MESSAGE_DEFAULT_CHUNK_SIZE * sizeof(guchar));
    }else{
      message->data = (guchar *) realloc(message->data,
						new_data_allocated_length * sizeof(guchar));
      memset(message->data + (new_data_allocated_length - AGS_OSC_BUILDER_MESSAGE_DEFAULT_CHUNK_SIZE),
	     0,
	     AGS_OSC_BUILDER_MESSAGE_DEFAULT_CHUNK_SIZE * sizeof(guchar));
    }

    message->data_allocated_length = new_data_allocated_length;
  }
}

/**
 * ags_osc_builder_packet_alloc:
 * @offset: the current offset
 * 
 * Allocate #AgsOscBuilderPacket-struct.
 * 
 * Returns: (type gpointer) (transfer none): the newly allocated #AgsOscBuilderPacket-struct
 *
 * Since: 3.0.0
 */
AgsOscBuilderPacket*
ags_osc_builder_packet_alloc(guint64 offset)
{
  AgsOscBuilderPacket *packet;

  packet = (AgsOscBuilderPacket *) malloc(sizeof(AgsOscBuilderPacket));

  packet->offset = offset;

  packet->packet_size = 0;

  packet->builder = NULL;

  packet->message = NULL;
  packet->bundle = NULL;
  
  return(packet);
}

/**
 * ags_osc_builder_packet_free:
 * @packet: (type gpointer) (transfer none): the #AgsOscBuilderPacket-struct
 *
 * Free @packet.
 * 
 * Since: 3.0.0
 */
void
ags_osc_builder_packet_free(AgsOscBuilderPacket *packet)
{
  if(packet == NULL){
    return;
  }

  g_list_free_full(packet->message,
		   (GDestroyNotify) ags_osc_builder_message_free);

  g_list_free_full(packet->bundle,
		   (GDestroyNotify) ags_osc_builder_bundle_free);

  free(packet);
}

/**
 * ags_osc_builder_bundle_alloc:
 * @offset: the current offset
 * 
 * Allocate #AgsOscBuilderBundle-struct.
 * 
 * Returns: (type gpointer) (transfer none): the newly allocated #AgsOscBuilderBundle-struct
 * 
 * Since: 3.0.0
 */
AgsOscBuilderBundle*
ags_osc_builder_bundle_alloc(guint64 offset)
{
  AgsOscBuilderBundle *bundle;

  bundle = (AgsOscBuilderBundle *) malloc(sizeof(AgsOscBuilderBundle));

  bundle->offset = offset;

  bundle->tv_secs = -1;
  bundle->tv_fraction = -1;
  bundle->immediately = FALSE;

  bundle->packet = NULL;  
  bundle->parent_bundle = NULL;
  
  bundle->message = NULL;
  bundle->bundle = NULL;

  return(bundle);
}

/**
 * ags_osc_builder_bundle_free:
 * @bundle: (type gpointer) (transfer none): the #AgsOscBuilderBundle-struct
 *
 * Free @bundle.
 * 
 * Since: 3.0.0
 */
void
ags_osc_builder_bundle_free(AgsOscBuilderBundle *bundle)
{
  if(bundle == NULL){
    return;
  }

  g_list_free_full(bundle->message,
		   (GDestroyNotify) ags_osc_builder_message_free);

  g_list_free_full(bundle->bundle,
		   (GDestroyNotify) ags_osc_builder_bundle_free);

  free(bundle);
}

/**
 * ags_osc_builder_message_alloc:
 * @offset: the current offset
 * 
 * Allocate #AgsOscBuilderMessage-struct.
 * 
 * Returns: (type gpointer) (transfer none): the newly allocated #AgsOscBuilderMessage-struct
 * 
 * Since: 3.0.0
 */
AgsOscBuilderMessage*
ags_osc_builder_message_alloc(guint64 offset)
{
  AgsOscBuilderMessage *message;

  message = (AgsOscBuilderMessage *) malloc(sizeof(AgsOscBuilderMessage));

  message->offset = offset;

  message->address_pattern = NULL;
  message->type_tag = NULL;

  message->data_allocated_length = 0;
  message->data_length = 0;

  message->data = NULL;
  
  message->packet = NULL;  
  message->parent_bundle = NULL;

  return(message);
}

/**
 * ags_osc_builder_message_free:
 * @message: (type gpointer) (transfer none): the #AgsOscBuilderMessage-struct
 *
 * Free @message.
 * 
 * Since: 3.0.0
 */
void
ags_osc_builder_message_free(AgsOscBuilderMessage *message)
{
  if(message == NULL){
    return;
  }

  if(message->address_pattern != NULL){
    free(message->address_pattern);
  }
  
  if(message->type_tag != NULL){
    free(message->type_tag);
  }

  if(message->data != NULL){
    free(message->data);
  }

  free(message);
}

void
ags_osc_builder_real_osc_putc(AgsOscBuilder *osc_builder,
			      gint c)
{
  //TODO:JK: implement me
}

/**
 * ags_osc_builder_osc_putc:
 * @osc_builder: the #AgsOscBuilder
 * @c: the character
 * 
 * Put character
 * 
 * Since: 3.0.0
 */
void
ags_osc_builder_osc_putc(AgsOscBuilder *osc_builder,
			 gint c)
{
  g_return_if_fail(AGS_IS_OSC_BUILDER(osc_builder));
  
  g_object_ref((GObject *) osc_builder);
  g_signal_emit(G_OBJECT(osc_builder),
		osc_builder_signals[OSC_PUTC], 0,
		c);
  g_object_unref((GObject *) osc_builder);
}

void
ags_osc_builder_real_on_error(AgsOscBuilder *osc_builder,
			      GError **error)
{
  if(error != NULL &&
     *error != NULL){
    g_warning("%s", (*error)->message);
  }
}

/**
 * ags_osc_builder_on_error:
 * @osc_builder: the #AgsOscBuilder
 * @error: the return location of #GError-struct
 * 
 * Report error.
 * 
 * Since: 3.0.0
 */
void
ags_osc_builder_on_error(AgsOscBuilder *osc_builder,
			 GError **error)
{
  g_return_if_fail(AGS_IS_OSC_BUILDER(osc_builder));
  
  g_object_ref((GObject *) osc_builder);
  g_signal_emit(G_OBJECT(osc_builder),
		osc_builder_signals[ON_ERROR], 0,
		error);
  g_object_unref((GObject *) osc_builder);
}

void
ags_osc_builder_real_append_packet(AgsOscBuilder *osc_builder)
{
  AgsOscBuilderPacket *packet;

  if(osc_builder->current_type_tag != NULL){
    g_critical("current type tag != NULL - complete values first");
    
    return;
  }
  
  packet = ags_osc_builder_packet_alloc(osc_builder->offset);
  osc_builder->packet = g_list_prepend(osc_builder->packet,
				       packet);
  
  osc_builder->offset += 4;
}

/**
 * ags_osc_builder_append_packet:
 * @osc_builder: the #AgsOscBuilder
 * 
 * Since: 3.0.0
 */
void
ags_osc_builder_append_packet(AgsOscBuilder *osc_builder)
{
  g_return_if_fail(AGS_IS_OSC_BUILDER(osc_builder));
  
  g_object_ref((GObject *) osc_builder);
  g_signal_emit(G_OBJECT(osc_builder),
		osc_builder_signals[APPEND_PACKET], 0);
  g_object_unref((GObject *) osc_builder);
}

void
ags_osc_builder_real_append_bundle(AgsOscBuilder *osc_builder,
				   AgsOscBuilderBundle *parent_bundle,
				   gint tv_secs, gint tv_fraction, gboolean immediately)
{
  AgsOscBuilderBundle *bundle;

  if(osc_builder->current_type_tag != NULL){
    g_critical("current type tag != NULL - complete values first");
    
    return;
  }
  
  bundle = ags_osc_builder_bundle_alloc(osc_builder->offset);

  bundle->tv_secs = tv_secs;
  bundle->tv_fraction = tv_fraction;
  bundle->immediately = immediately;
  
  if(parent_bundle == NULL){
    if(osc_builder->packet != NULL){
      bundle->packet = osc_builder->packet->data;
      bundle->packet->bundle = g_list_prepend(bundle->packet->bundle,
					      bundle);

      bundle->packet->packet_size += 16;
    }else{
      g_warning("can't append OSC bundle because no package available");

      return;
    }
  }else{
    bundle->parent_bundle = parent_bundle;
    parent_bundle->bundle = g_list_prepend(parent_bundle->bundle,
					   bundle);
    
    ags_osc_builder_bundle_resize(osc_builder,
				  parent_bundle,
				  16);
  }
}

/**
 * ags_osc_builder_append_bundle:
 * @osc_builder: the #AgsOscBuilder
 * @parent_bundle: the parent #AgsOscBuilderBundle-struct or %NULL
 * @tv_secs: number of seconds since midnight on January 1, 1900
 * @tv_fraction: fraction of seconds to a precision of about 200 picoseconds
 * @immediately: if %TRUE apply immediately, otherwise %FALSE not immediately
 * 
 * Append bundle.
 * 
 * Since: 3.0.0
 */
void
ags_osc_builder_append_bundle(AgsOscBuilder *osc_builder,
			      AgsOscBuilderBundle *parent_bundle,
			      gint tv_secs, gint tv_fraction, gboolean immediately)
{
  g_return_if_fail(AGS_IS_OSC_BUILDER(osc_builder));
  
  g_object_ref((GObject *) osc_builder);
  g_signal_emit(G_OBJECT(osc_builder),
		osc_builder_signals[APPEND_BUNDLE], 0,
		parent_bundle,
		tv_secs, tv_fraction, immediately);
  g_object_unref((GObject *) osc_builder);
}

void
ags_osc_builder_real_append_message(AgsOscBuilder *osc_builder,
				    AgsOscBuilderBundle *parent_bundle,
				    gchar *address_pattern,
				    gchar *type_tag)
{
  AgsOscBuilderMessage *message;

  guint64 address_pattern_length;
  guint64 type_tag_length;

  if(osc_builder->current_type_tag != NULL){
    g_critical("current type tag != NULL - complete values first");
    
    return;
  }
  
  message = ags_osc_builder_message_alloc(osc_builder->offset);

  message->address_pattern = g_strdup(address_pattern);
  address_pattern_length = strlen(address_pattern);  
  
  message->type_tag = g_strdup(type_tag);
  type_tag_length = strlen(type_tag);

  osc_builder->current_type_tag = message->type_tag;
  osc_builder->offset_type_tag = message->type_tag;

  ags_osc_builder_skip_empty_types(osc_builder);
  
  if(parent_bundle == NULL){
    if(osc_builder->packet != NULL){
      message->packet = osc_builder->packet->data;
      message->packet->message = g_list_prepend(message->packet->message,
						message);

      message->packet->packet_size += (address_pattern_length + 1) + (type_tag_length + 1);
    }else{
      g_warning("can't append OSC message because no package available");

      return;
    }
  }else{
    ags_osc_builder_bundle_resize(osc_builder,
				  parent_bundle,
				  (address_pattern_length + 1) + (type_tag_length + 1));
  }  
}

/**
 * ags_osc_builder_append_message:
 * @osc_builder: the #AgsOscBuilder
 * @parent_bundle: the parent #AgsOscBuilderBundle-struct or %NULL
 * @address_pattern: the address pattern
 * @type_tag: the type tag
 * 
 * Append message.
 * 
 * Since: 3.0.0
 */
void
ags_osc_builder_append_message(AgsOscBuilder *osc_builder,
			       AgsOscBuilderBundle *parent_bundle,
			       gchar *address_pattern,
			       gchar *type_tag)
{
  g_return_if_fail(AGS_IS_OSC_BUILDER(osc_builder));
  
  g_object_ref((GObject *) osc_builder);
  g_signal_emit(G_OBJECT(osc_builder),
		osc_builder_signals[APPEND_MESSAGE], 0,
		parent_bundle,
		address_pattern, type_tag);
  g_object_unref((GObject *) osc_builder);
}

void
ags_osc_builder_real_append_value(AgsOscBuilder *osc_builder,
				  AgsOscBuilderMessage *message,
				  gint v_type,
				  GValue *value)
{
  AgsOscBufferUtil osc_buffer_util;
  
  if(osc_builder->offset_type_tag[0] != v_type){
    g_critical("message's value type not matching");
    
    return;
  }

  osc_buffer_util.major = 1;
  osc_buffer_util.minor = 0;
  
  switch(v_type){
  case AGS_OSC_UTIL_TYPE_TAG_STRING_INT32:
    {
      gint32 val;

      val = g_value_get_int(value);

      ags_osc_builder_message_check_resize(osc_builder,
					   message,
					   4);

      ags_osc_buffer_util_put_int32(&osc_buffer_util,
				    message->data + message->data_length,
				    val);

      message->data_length += 4;
      osc_builder->offset += 4;
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_FLOAT:
    {
      gfloat val;

      val = g_value_get_float(value);

      ags_osc_builder_message_check_resize(osc_builder,
					   message,
					   4);

      ags_osc_buffer_util_put_float(&osc_buffer_util,
				    message->data + message->data_length,
				    val);

      message->data_length += 4;
      osc_builder->offset += 4;
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_STRING:
    {
      gchar *str;

      guint64 length;

      str = g_value_get_string(value);
      length = strlen(str);
      
      ags_osc_builder_message_check_resize(osc_builder,
					   message,
					   length + 1);

      ags_osc_buffer_util_put_string(&osc_buffer_util,
				    message->data + message->data_length,
				     str,
				     -1);

      message->data_length += (length + 1);
      osc_builder->offset += (length + 1);
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_BLOB:
    {
      guchar *data;
      
      gint32 data_size;

      data_size = g_value_get_int(&(value[0]));
      data = g_value_get_pointer(&(value[1]));

      ags_osc_builder_message_check_resize(osc_builder,
					   message,
					   data_size + 4);

      ags_osc_buffer_util_put_blob(&osc_buffer_util,
				    message->data + message->data_length,
				   data_size, data);
      
      message->data_length += (data_size + 4);
      osc_builder->offset += (data_size + 4);
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_INT64:
    {
      gint64 val;

      val = g_value_get_int64(value);

      ags_osc_builder_message_check_resize(osc_builder,
					   message,
					   8);

      ags_osc_buffer_util_put_int64(&osc_buffer_util,
				    message->data + message->data_length,
				    val);

      message->data_length += 8;
      osc_builder->offset += 8;
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_TIMETAG:
    {
      gint32 tv_secs;
      gint32 tv_fraction;
      gboolean immediately;

      tv_secs = g_value_get_int(&(value[0]));
      tv_fraction = g_value_get_int(&(value[1]));
      immediately = g_value_get_boolean(&(value[2]));

      ags_osc_builder_message_check_resize(osc_builder,
					   message,
					   8);

      ags_osc_buffer_util_put_timetag(&osc_buffer_util,
				    message->data + message->data_length,
				      tv_secs, tv_fraction, immediately);
      
      message->data_length += 8;
      osc_builder->offset += 8;
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_DOUBLE:
    {
      gdouble val;

      val = g_value_get_double(value);

      ags_osc_builder_message_check_resize(osc_builder,
					   message,
					   8);

      ags_osc_buffer_util_put_double(&osc_buffer_util,
				    message->data + message->data_length,
				     val);

      message->data_length += 8;
      osc_builder->offset += 8;
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_SYMBOL:
    {
      gchar *str;

      guint64 length;

      str = g_value_get_string(value);
      length = strlen(str);
      
      ags_osc_builder_message_check_resize(osc_builder,
					   message,
					   length + 1);

      ags_osc_buffer_util_put_string(&osc_buffer_util,
				     message->data + message->data_length,
				     str,
				     -1);

      message->data_length += (length + 1);
      osc_builder->offset += (length + 1);
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_CHAR:
    {
      gint32 val;

      val = g_value_get_int(value);

      ags_osc_builder_message_check_resize(osc_builder,
					   message,
					   4);

      ags_osc_buffer_util_put_char(&osc_buffer_util,
				    message->data + message->data_length,
				   val);

      message->data_length += 4;
      osc_builder->offset += 4;
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_RGBA:
    {
      guint8 r, g, b, a;

      r = g_value_get_int(&(value[0]));
      g = g_value_get_int(&(value[1]));
      b = g_value_get_int(&(value[2]));
      a = g_value_get_int(&(value[3]));

      ags_osc_builder_message_check_resize(osc_builder,
					   message,
					   8);

      ags_osc_buffer_util_put_rgba(&osc_buffer_util,
				    message->data + message->data_length,
				   r, g, b, a);
      
      message->data_length += 8;
      osc_builder->offset += 8;
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_MIDI:
    {
      guint8 port;
      guint8 status_byte;
      guint8 data0, data1;

      port = g_value_get_int(&(value[0]));
      status_byte = g_value_get_int(&(value[1]));
      data0 = g_value_get_int(&(value[2]));
      data1 = g_value_get_int(&(value[3]));

      ags_osc_builder_message_check_resize(osc_builder,
					   message,
					   8);

      ags_osc_buffer_util_put_midi(&osc_buffer_util,
				    message->data + message->data_length,
				   port, status_byte, data0, data1);
      
      message->data_length += 8;
      osc_builder->offset += 8;
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_TRUE:
    {
      //empty
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_FALSE:
    {
      //empty
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_NIL:
    {
      //empty
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_INFINITE:
    {
      //empty
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_ARRAY_START:
    {
      //empty
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_ARRAY_END:
    {
      //empty
    }
    break;
  default:
    {
      g_warning("unknown type");

      return;
    }
  }

  osc_builder->offset_type_tag += 1;
  ags_osc_builder_skip_empty_types(osc_builder);
}

/**
 * ags_osc_builder_append_value:
 * @osc_builder: the #AgsOscBuilder
 * @message: the message
 * @v_type: the type as char
 * @value: the #GValue-struct containinig value
 * 
 * Append value.
 * 
 * Since: 3.0.0
 */
void
ags_osc_builder_append_value(AgsOscBuilder *osc_builder,
			     AgsOscBuilderMessage *message,
			     gint v_type,
			     GValue *value)
{
  g_return_if_fail(AGS_IS_OSC_BUILDER(osc_builder));
  
  g_object_ref((GObject *) osc_builder);
  g_signal_emit(G_OBJECT(osc_builder),
		osc_builder_signals[APPEND_VALUE], 0,
		message,
		v_type, value);
  g_object_unref((GObject *) osc_builder);
}

void
ags_osc_builder_build_bundle(AgsOscBuilder *osc_builder,
			     AgsOscBuilderBundle *current_bundle,
			     guchar *data,
			     gsize *offset)
{
  AgsOscBufferUtil osc_buffer_util;
  
  GList *message_start, *message;
  GList *bundle_start, *bundle;

  osc_buffer_util.major = 1;
  osc_buffer_util.minor = 0;

  /* #bundle */
  ags_osc_buffer_util_put_string(&osc_buffer_util,
				 data + offset[0],
				 "#bundle",
				 -1);
    
  offset[0] += 8;

  /* time tag */
  ags_osc_buffer_util_put_timetag(&osc_buffer_util,
				  data + offset[0],
				  current_bundle->tv_secs, current_bundle->tv_fraction, current_bundle->immediately);
    
  offset[0] += 8;

  /* content */
  message_start = g_list_copy(current_bundle->message);
  message_start = g_list_reverse(message_start);

  bundle_start = g_list_copy(current_bundle->bundle);
  bundle_start = g_list_reverse(bundle_start);

  message = message_start;
  bundle = bundle_start;

  while(message != NULL || bundle != NULL){
    if(bundle == NULL ||
       (message != NULL && AGS_OSC_BUILDER_MESSAGE(message->data)->offset < AGS_OSC_BUILDER_BUNDLE(bundle->data)->offset)){
      ags_osc_builder_build_message(osc_builder,
				    message->data,
				    data,
				    offset);	
    }else{
      ags_osc_builder_build_bundle(osc_builder,
				   bundle->data,
				   data,
				   offset);	
    }
      
    /* iterate */
    if(message != NULL){
      message = message->next;
    }

    if(bundle != NULL){
      bundle = bundle->next;
    }
  }
    
  g_list_free(message_start);
  g_list_free(bundle_start);
}

void
ags_osc_builder_build_message(AgsOscBuilder *osc_builder,
			      AgsOscBuilderMessage *message,
			      guchar *data,
			      gsize *offset)
{
  AgsOscBufferUtil osc_buffer_util;
  
  guint64 address_pattern_length;
  guint64 type_tag_length;

  osc_buffer_util.major = 1;
  osc_buffer_util.minor = 0;

  /* address pattern */
  address_pattern_length = strlen(message->address_pattern);

  ags_osc_buffer_util_put_string(&osc_buffer_util,
				 data + offset[0],
				 message->address_pattern,
				 -1);

  offset[0] += (address_pattern_length + 1);

  /* type tag */
  type_tag_length = strlen(message->type_tag);

  ags_osc_buffer_util_put_string(&osc_buffer_util,
				 data + offset[0],
				 message->type_tag,
				 -1);

  offset[0] += (type_tag_length + 1);

  /* data */
  memcpy(data + offset[0],
	 message->data,
	 message->data_length * sizeof(guchar));

  offset[0] += message->data_length;
}

/**
 * ags_osc_builder_build:
 * @osc_builder: the #AgsOscBuilder
 *
 * Build the OSC data.
 * 
 * Since: 3.0.0
 */
void
ags_osc_builder_build(AgsOscBuilder *osc_builder)
{
  AgsOscBufferUtil osc_buffer_util;

  GList *packet_start, *packet;

  guchar *data;

  gsize data_length;
  gsize offset;
  
  if(!AGS_IS_OSC_BUILDER(osc_builder)){
    return;
  }

  osc_buffer_util.major = 1;
  osc_buffer_util.minor = 0;

  packet_start = g_list_copy(osc_builder->packet);
  packet_start = g_list_reverse(packet_start);

  packet = packet_start;

  data = NULL;

  offset = 0;
  data_length = 0;
  
  while(packet != NULL){
    GList *message_start, *message;
    GList *bundle_start, *bundle;
    
    /* re-allocate data */
    if(data == NULL){
      data = (guchar *) malloc((4 + AGS_OSC_BUILDER_PACKET(packet->data)->packet_size) * sizeof(guchar));
      
      data_length = 4 + AGS_OSC_BUILDER_PACKET(packet->data)->packet_size;
    }else{
      data = (guchar *) realloc(data,
				       (data_length + 4 + AGS_OSC_BUILDER_PACKET(packet->data)->packet_size) * sizeof(guchar));

      data_length += (4 + AGS_OSC_BUILDER_PACKET(packet->data)->packet_size);
    }

    /* put packet size */
    ags_osc_buffer_util_put_int32(&osc_buffer_util,
				  data + offset,
				  AGS_OSC_BUILDER_PACKET(packet->data)->packet_size);

    /*  */
    message_start = g_list_copy(AGS_OSC_BUILDER_PACKET(packet->data)->message);
    message_start = g_list_reverse(message_start);

    bundle_start = g_list_copy(AGS_OSC_BUILDER_PACKET(packet->data)->bundle);
    bundle_start = g_list_reverse(bundle_start);

    message = message_start;
    bundle = bundle_start;
    
    while(message != NULL || bundle != NULL){
      if(bundle == NULL ||
	 (message != NULL && AGS_OSC_BUILDER_MESSAGE(message->data)->offset < AGS_OSC_BUILDER_BUNDLE(bundle->data)->offset)){
	ags_osc_builder_build_message(osc_builder,
				      message->data,
				      data,
				      &offset);	
      }else{
	ags_osc_builder_build_bundle(osc_builder,
				     bundle->data,
				     data,
				     &offset);	
      }
      
      /* iterate */
      if(message != NULL){
	message = message->next;
      }

      if(bundle != NULL){
	bundle = bundle->next;
      }
    }
    
    g_list_free(message_start);
    g_list_free(bundle_start);
    
    /* iterate */
    packet = packet->next;
  }

  g_list_free(packet_start);

  osc_builder->data = data;
  osc_builder->length = offset;
}

/**
 * ags_osc_builder_get_data:
 * @osc_builder: the #AgsOscBuilder
 * 
 * Get OSC data of @osc_builder.
 * 
 * Returns: the OSC data
 * 
 * Since: 3.7.24
 */
guchar*
ags_osc_builder_get_data(AgsOscBuilder *osc_builder)
{  
  if(!AGS_IS_OSC_BUILDER(osc_builder)){
    return(NULL);
  }

  return(osc_builder->data);
}

/**
 * ags_osc_builder_get_data_with_length:
 * @osc_builder: the #AgsOscBuilder
 * @length: (out): the length of data
 *
 * Get OSC data of @osc_builder.
 * 
 * Returns: (transfer full): the OSC data as array
 * 
 * Since: 3.7.24
 */
guchar*
ags_osc_builder_get_data_with_length(AgsOscBuilder *osc_builder,
				     guint *length)
{
  guchar *data;
  
  GRecMutex *osc_builder_mutex;
  
  if(!AGS_IS_OSC_BUILDER(osc_builder)){
    if(length != NULL){
      length[0] = 0;
    }
    
    return(NULL);
  }

  /* get osc builder mutex */
  osc_builder_mutex = AGS_OSC_BUILDER_GET_OBJ_MUTEX(osc_builder);

  g_rec_mutex_lock(osc_builder_mutex);

  data = NULL;

  if(osc_builder->data != NULL){
    data = (guchar *) g_malloc(osc_builder->length * sizeof(guchar));

    if(osc_builder->length > 0){
      memcpy(data, osc_builder->data, osc_builder->length * sizeof(guchar));
    }
  }  

  if(length != NULL){
    length[0] = osc_builder->length;
  }

  g_rec_mutex_unlock(osc_builder_mutex);
  
  return(data);
}

/**
 * ags_osc_builder_new:
 * 
 * Creates a new instance of #AgsOscBuilder
 *
 * Returns: the new #AgsOscBuilder
 * 
 * Since: 3.0.0
 */
AgsOscBuilder*
ags_osc_builder_new()
{
  AgsOscBuilder *osc_builder;
  
  osc_builder = (AgsOscBuilder *) g_object_new(AGS_TYPE_OSC_BUILDER,
					       NULL);

  
  return(osc_builder);
}
