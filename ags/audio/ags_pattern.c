/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/ags_pattern.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_port.h>

#include <stdarg.h>
#include <math.h>
#include <string.h>

void ags_pattern_class_init(AgsPatternClass *pattern_class);
void ags_pattern_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pattern_tactable_interface_init(AgsTactableInterface *tactable);
void ags_pattern_init(AgsPattern *pattern);
void ags_pattern_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec);
void ags_pattern_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec);
void ags_pattern_dispose(GObject *gobject);
void ags_pattern_finalize(GObject *gobject);

AgsUUID* ags_pattern_get_uuid(AgsConnectable *connectable);
gboolean ags_pattern_has_resource(AgsConnectable *connectable);
gboolean ags_pattern_is_ready(AgsConnectable *connectable);
void ags_pattern_add_to_registry(AgsConnectable *connectable);
void ags_pattern_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_pattern_list_resource(AgsConnectable *connectable);
xmlNode* ags_pattern_xml_compose(AgsConnectable *connectable);
void ags_pattern_xml_parse(AgsConnectable *connectable,
			   xmlNode *node);
gboolean ags_pattern_is_connected(AgsConnectable *connectable);
void ags_pattern_connect(AgsConnectable *connectable);
void ags_pattern_disconnect(AgsConnectable *connectable);

void ags_pattern_change_bpm(AgsTactable *tactable, gdouble new_bpm, gdouble old_bpm);

/**
 * SECTION:ags_pattern
 * @short_description: Pattern representing tones
 * @title: AgsPattern
 * @section_id:
 * @include: ags/audio/ags_pattern.h
 *
 * #AgsPattern represents an audio pattern of tones.
 */

enum{
  PROP_0,
  PROP_CHANNEL,
  PROP_PORT,
  PROP_FIRST_INDEX,
  PROP_SECOND_INDEX,
  PROP_OFFSET,
  PROP_CURRENT_BIT,
  PROP_TIMESTAMP,
};

static gpointer ags_pattern_parent_class = NULL;

GType
ags_pattern_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_pattern = 0;

    static const GTypeInfo ags_pattern_info = {
      sizeof (AgsPatternClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pattern_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPattern),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pattern_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pattern_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_tactable_interface_info = {
      (GInterfaceInitFunc) ags_pattern_tactable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_pattern = g_type_register_static(G_TYPE_OBJECT,
					      "AgsPattern",
					      &ags_pattern_info,
					      0);

    g_type_add_interface_static(ags_type_pattern,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_pattern,
				AGS_TYPE_TACTABLE,
				&ags_tactable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_pattern);
  }

  return g_define_type_id__volatile;
}

void
ags_pattern_class_init(AgsPatternClass *pattern)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_pattern_parent_class = g_type_class_peek_parent(pattern);

  gobject = (GObjectClass *) pattern;

  gobject->set_property = ags_pattern_set_property;
  gobject->get_property = ags_pattern_get_property;

  gobject->dispose = ags_pattern_dispose;
  gobject->finalize = ags_pattern_finalize;

  /* properties */
  /**
   * AgsPattern:channel:
   *
   * The pattern's channel.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("channel",
				   "channel of pattern",
				   "The channel of pattern",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /**
   * AgsPattern:port:
   *
   * The pattern's port.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("port",
				   "port of pattern",
				   "The port of pattern",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT,
				  param_spec);

  /**
   * AgsPattern:first-index:
   *
   * Selected bank 0.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("first-index",
				 "the first index",
				 "The first index to select pattern",
				 0, 256,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FIRST_INDEX,
				  param_spec);

  /**
   * AgsPattern:second-index:
   *
   * Selected bank 1.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("second-index",
				 "the second index",
				 "The second index to select pattern",
				 0, 256,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SECOND_INDEX,
				  param_spec);

  /**
   * AgsPattern:offset:
   *
   * Position of pattern.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("offset",
				 "the offset",
				 "The offset within the pattern",
				 0, 65535,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OFFSET,
				  param_spec);

  /**
   * AgsPattern:current-bit:
   *
   * Offset of current position.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_boolean("current-bit",
				    "current bit for offset",
				    "The current bit for offset",
				    FALSE,
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_CURRENT_BIT,
				  param_spec);

  /**
   * AgsPattern:timestamp:
   *
   * The pattern's timestamp.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("timestamp",
				   "timestamp of pattern",
				   "The timestamp of pattern",
				   AGS_TYPE_TIMESTAMP,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TIMESTAMP,
				  param_spec);
}

void
ags_pattern_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_pattern_get_uuid;
  connectable->has_resource = ags_pattern_has_resource;

  connectable->is_ready = ags_pattern_is_ready;
  connectable->add_to_registry = ags_pattern_add_to_registry;
  connectable->remove_from_registry = ags_pattern_remove_from_registry;

  connectable->list_resource = ags_pattern_list_resource;
  connectable->xml_compose = ags_pattern_xml_compose;
  connectable->xml_parse = ags_pattern_xml_parse;

  connectable->is_connected = ags_pattern_is_connected;  
  connectable->connect = ags_pattern_connect;
  connectable->disconnect = ags_pattern_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_pattern_tactable_interface_init(AgsTactableInterface *tactable)
{
  tactable->change_bpm = ags_pattern_change_bpm;
}

void
ags_pattern_init(AgsPattern *pattern)
{
  /* base initialization */
  pattern->flags = 0;

  /* pattern mutex */
  g_rec_mutex_init(&(pattern->obj_mutex));

  /* channel */
  pattern->channel = NULL;

  /* timestamp */
  pattern->timestamp = NULL;

  /* dimension and pattern */
  pattern->dim[0] = 0;
  pattern->dim[1] = 0;
  pattern->dim[2] = 0;

  pattern->pattern = NULL;

  /* port */
  pattern->port = NULL;

  /* indices */
  pattern->i = 0;
  pattern->j = 0;
  pattern->bit = 0;

  /* note */
  pattern->note = NULL;
}

void
ags_pattern_set_property(GObject *gobject,
			 guint prop_id,
			 const GValue *value,
			 GParamSpec *param_spec)
{
  AgsPattern *pattern;

  GRecMutex *pattern_mutex;

  pattern = AGS_PATTERN(gobject);

  /* get pattern mutex */
  pattern_mutex = AGS_PATTERN_GET_OBJ_MUTEX(pattern);

  switch(prop_id){
  case PROP_CHANNEL:
  {
    AgsChannel *channel;

    channel = (AgsChannel *) g_value_get_object(value);

    g_rec_mutex_lock(pattern_mutex);

    if(channel == pattern->channel){
      g_rec_mutex_unlock(pattern_mutex);

      return;
    }

    if(pattern->channel != NULL){
      g_object_unref(G_OBJECT(pattern->channel));
    }

    if(channel != NULL){
      g_object_ref(G_OBJECT(channel));
    }

    pattern->channel = channel;

    g_rec_mutex_unlock(pattern_mutex);
  }
  break;
  case PROP_FIRST_INDEX:
  {
    guint i;

    i = g_value_get_uint(value);

    g_rec_mutex_lock(pattern_mutex);

    pattern->i = i;

    g_rec_mutex_unlock(pattern_mutex);
  }
  break;
  case PROP_SECOND_INDEX:
  {
    guint j;

    j = g_value_get_uint(value);

    g_rec_mutex_lock(pattern_mutex);

    pattern->j = j;

    g_rec_mutex_unlock(pattern_mutex);
  }
  break;
  case PROP_OFFSET:
  {
    guint bit;

    bit = g_value_get_uint(value);

    g_rec_mutex_lock(pattern_mutex);

    pattern->bit = bit;

    g_rec_mutex_unlock(pattern_mutex);
  }
  break;
  case PROP_TIMESTAMP:
  {
    AgsTimestamp *timestamp;

    timestamp = (AgsTimestamp *) g_value_get_object(value);

    g_rec_mutex_lock(pattern_mutex);

    if(timestamp == pattern->timestamp){
      g_rec_mutex_unlock(pattern_mutex);

      return;
    }

    if(pattern->timestamp != NULL){
      g_object_unref(G_OBJECT(pattern->timestamp));
    }

    if(timestamp != NULL){
      g_object_ref(G_OBJECT(timestamp));
    }

    pattern->timestamp = timestamp;

    g_rec_mutex_unlock(pattern_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pattern_get_property(GObject *gobject,
			 guint prop_id,
			 GValue *value,
			 GParamSpec *param_spec)
{
  AgsPattern *pattern;

  GRecMutex *pattern_mutex;

  pattern = AGS_PATTERN(gobject);

  /* get pattern mutex */
  pattern_mutex = AGS_PATTERN_GET_OBJ_MUTEX(pattern);

  switch(prop_id){
  case PROP_CHANNEL:
  {
    g_rec_mutex_lock(pattern_mutex);

    g_value_set_object(value,
		       pattern->channel);

    g_rec_mutex_unlock(pattern_mutex);
  }
  break;
  case PROP_PORT:
  {
    g_rec_mutex_lock(pattern_mutex);

    g_value_set_object(value, pattern->port);

    g_rec_mutex_unlock(pattern_mutex);
  }
  break;
  case PROP_FIRST_INDEX:
  {
    g_rec_mutex_lock(pattern_mutex);

    g_value_set_uint(value, pattern->i);

    g_rec_mutex_unlock(pattern_mutex);
  }
  break;
  case PROP_SECOND_INDEX:
  {
    g_rec_mutex_lock(pattern_mutex);

    g_value_set_uint(value, pattern->j);

    g_rec_mutex_unlock(pattern_mutex);
  }
  break;
  case PROP_OFFSET:
  {
    g_rec_mutex_lock(pattern_mutex);

    g_value_set_uint(value, pattern->bit);

    g_rec_mutex_unlock(pattern_mutex);
  }
  break;
  case PROP_CURRENT_BIT:
  {
    guint i, j;
    guint bit;

    g_rec_mutex_lock(pattern_mutex);

    i = pattern->i;
    j = pattern->j;
    bit = pattern->bit;

    g_rec_mutex_unlock(pattern_mutex);

    g_value_set_boolean(value, ags_pattern_get_bit(pattern,
						   i,
						   j,
						   bit));
  }
  break;
  case PROP_TIMESTAMP:
  {
    g_rec_mutex_lock(pattern_mutex);

    g_value_set_object(value,
		       pattern->timestamp);

    g_rec_mutex_unlock(pattern_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pattern_dispose(GObject *gobject)
{
  AgsPattern *pattern;

  pattern = AGS_PATTERN(gobject);

  /* channel */
  if(pattern->channel != NULL){
    g_object_unref(G_OBJECT(pattern->channel));

    pattern->channel = NULL;
  }

  /* timestamp */
  if(pattern->timestamp != NULL){
    g_object_run_dispose(G_OBJECT(pattern->timestamp));
    
    g_object_unref(G_OBJECT(pattern->timestamp));

    pattern->timestamp = NULL;
  }

  /* port */
  if(pattern->port != NULL){
    g_object_unref(G_OBJECT(pattern->port));

    pattern->port = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_pattern_parent_class)->dispose(gobject);
}

void
ags_pattern_finalize(GObject *gobject)
{
  AgsPattern *pattern;
  
  guint i, j;

  pattern = AGS_PATTERN(gobject);

  /* channel */
  if(pattern->channel != NULL){
    g_object_unref(G_OBJECT(pattern->channel));
  }

  /* timestamp */
  if(pattern->timestamp != NULL){
    g_object_unref(G_OBJECT(pattern->timestamp));
  }

  /* pattern */
  if(pattern->pattern != NULL){
    for(i = 0; i < pattern->dim[0]; i++){
      if(pattern->pattern[i] != NULL){
	for(j = 0; j < pattern->dim[1]; j++){
	  if(pattern->pattern[i][j] != NULL){
	    g_free(pattern->pattern[i][j]);
	  }
	}

	g_free(pattern->pattern[i]);
      }
    }

    g_free(pattern->pattern);
  }
 
  /* port */
  if(pattern->port != NULL){
    g_object_unref(G_OBJECT(pattern->port));
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_pattern_parent_class)->finalize(gobject);
}

AgsUUID*
ags_pattern_get_uuid(AgsConnectable *connectable)
{
  AgsPattern *pattern;
  
  AgsUUID *ptr;

  GRecMutex *pattern_mutex;

  pattern = AGS_PATTERN(connectable);

  /* get pattern mutex */
  pattern_mutex = AGS_PATTERN_GET_OBJ_MUTEX(pattern);

  /* get UUID */
  g_rec_mutex_lock(pattern_mutex);

  ptr = pattern->uuid;

  g_rec_mutex_unlock(pattern_mutex);
  
  return(ptr);
}

gboolean
ags_pattern_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_pattern_is_ready(AgsConnectable *connectable)
{
  AgsPattern *pattern;
  
  gboolean is_ready;

  pattern = AGS_PATTERN(connectable);

  /* check is added */
  is_ready = ags_pattern_test_flags(pattern, AGS_PATTERN_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_pattern_add_to_registry(AgsConnectable *connectable)
{
  AgsPattern *pattern;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  GList *list;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  pattern = AGS_PATTERN(connectable);

  ags_pattern_set_flags(pattern, AGS_PATTERN_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(entry->entry,
		       (gpointer) pattern);
    ags_registry_add_entry(registry,
			   entry);
  }

  //TODO:JK: implement me
}

void
ags_pattern_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_pattern_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_pattern_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_pattern_xml_parse(AgsConnectable *connectable,
		      xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_pattern_is_connected(AgsConnectable *connectable)
{
  AgsPattern *pattern;
  
  gboolean is_connected;

  pattern = AGS_PATTERN(connectable);

  /* check is connected */
  is_connected = ags_pattern_test_flags(pattern, AGS_PATTERN_CONNECTED);
  
  return(is_connected);
}

void
ags_pattern_connect(AgsConnectable *connectable)
{
  AgsPattern *pattern;

  GList *list_start, *list;

  GRecMutex *pattern_mutex;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  pattern = AGS_PATTERN(connectable);

  ags_pattern_set_flags(pattern, AGS_PATTERN_CONNECTED);  
}

void
ags_pattern_disconnect(AgsConnectable *connectable)
{
  AgsPattern *pattern;

  GList *list_start, *list;

  GRecMutex *pattern_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  pattern = AGS_PATTERN(connectable);

  ags_pattern_unset_flags(pattern, AGS_PATTERN_CONNECTED);    
}

/**
 * ags_pattern_get_obj_mutex:
 * @pattern: the #AgsPattern
 * 
 * Get object mutex.
 * 
 * Returns: the #GRecMutex to lock @pattern
 * 
 * Since: 3.1.0
 */
GRecMutex*
ags_pattern_get_obj_mutex(AgsPattern *pattern)
{
  if(!AGS_IS_PATTERN(pattern)){
    return(NULL);
  }

  return(AGS_PATTERN_GET_OBJ_MUTEX(pattern));
}

/**
 * ags_pattern_test_flags:
 * @pattern: the #AgsPattern
 * @flags: the flags
 *
 * Test @flags to be set on @pattern.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_pattern_test_flags(AgsPattern *pattern, guint flags)
{
  gboolean retval;  
  
  GRecMutex *pattern_mutex;

  if(!AGS_IS_PATTERN(pattern)){
    return(FALSE);
  }

  /* get pattern mutex */
  pattern_mutex = AGS_PATTERN_GET_OBJ_MUTEX(pattern);

  /* test */
  g_rec_mutex_lock(pattern_mutex);

  retval = (flags & (pattern->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(pattern_mutex);

  return(retval);
}

/**
 * ags_pattern_set_flags:
 * @pattern: the #AgsPattern
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 3.0.0
 */
void
ags_pattern_set_flags(AgsPattern *pattern, guint flags)
{
  GRecMutex *pattern_mutex;

  if(!AGS_IS_PATTERN(pattern)){
    return;
  }

  /* get pattern mutex */
  pattern_mutex = AGS_PATTERN_GET_OBJ_MUTEX(pattern);

  /* set flags */
  g_rec_mutex_lock(pattern_mutex);

  pattern->flags |= flags;

  g_rec_mutex_unlock(pattern_mutex);
}

/**
 * ags_pattern_unset_flags:
 * @pattern: the #AgsPattern
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 3.0.0
 */
void
ags_pattern_unset_flags(AgsPattern *pattern, guint flags)
{
  GRecMutex *pattern_mutex;

  if(!AGS_IS_PATTERN(pattern)){
    return;
  }

  /* get pattern mutex */
  pattern_mutex = AGS_PATTERN_GET_OBJ_MUTEX(pattern);

  /* set flags */
  g_rec_mutex_lock(pattern_mutex);

  pattern->flags &= (~flags);

  g_rec_mutex_unlock(pattern_mutex);
}

void
ags_pattern_change_bpm(AgsTactable *tactable, gdouble new_bpm, gdouble old_bpm)
{
  //TODO:JK: implement me
}

/**
 * ags_pattern_find_near_timestamp:
 * @pattern: (element-type AgsAudio.Pattern) (transfer none): the #GList-struct containing #AgsPattern
 * @timestamp: the matching #AgsTimestamp
 *
 * Retrieve appropriate pattern for timestamp.
 *
 * Returns: (element-type AgsAudio.Pattern) (transfer none): Next match.
 *
 * Since: 3.0.0
 */
GList*
ags_pattern_find_near_timestamp(GList *pattern, AgsTimestamp *timestamp)
{
  AgsTimestamp *current_timestamp;

  GList *retval;
  GList *current_start, *current_end, *current;

  guint64 current_x, x;
  guint length, position;
  gboolean use_ags_offset;
  gboolean success;

  if(pattern == NULL){
    return(NULL);
  }

  current_start = pattern;
  current_end = g_list_last(pattern);
  
  length = g_list_length(pattern);
  position = length / 2;

  current = g_list_nth(current_start,
		       position);

  if(ags_timestamp_test_flags(timestamp,
			      AGS_TIMESTAMP_OFFSET)){
    x = ags_timestamp_get_ags_offset(timestamp);

    use_ags_offset = TRUE;
  }else if(ags_timestamp_test_flags(timestamp,
				    AGS_TIMESTAMP_UNIX)){
    x = ags_timestamp_get_unix_time(timestamp);

    use_ags_offset = FALSE;
  }else{
    return(NULL);
  }
  
  retval = NULL;
  success = FALSE;
  
  while(!success && current != NULL){
    current_x = 0;
    
    /* check current - start */
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
	   current_x < x + AGS_PATTERN_DEFAULT_OFFSET){
	  retval = current_start;
	    
	  break;
	}
      }else{
	if(current_x >= x &&
	   current_x < x + AGS_PATTERN_DEFAULT_DURATION){
	  retval = current_start;
	    
	  break;
	}
      }
    }else{
      g_warning("inconsistent data");
    }

    /* check current - end */
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
	   current_x < x + AGS_PATTERN_DEFAULT_OFFSET){
	  retval = current_end;
	    
	  break;
	}
      }else{
	if(current_x >= x &&
	   current_x < x + AGS_PATTERN_DEFAULT_DURATION){
	  retval = current_end;
	    
	  break;
	}
      }
    }else{
      g_warning("inconsistent data");
    }

    /* check current - center */
    if(timestamp == NULL){
      retval = current;
	
      break;
    }

    g_object_get(current->data,
		 "timestamp", &current_timestamp,
		 NULL);

    if(current_timestamp != NULL){
      if(use_ags_offset){
	current_x = ags_timestamp_get_ags_offset(current_timestamp);

	g_object_unref(current_timestamp);

	if(current_x >= x &&
	   current_x < x + AGS_PATTERN_DEFAULT_OFFSET){
	  retval = current;
	    
	  break;
	}
      }else{
	current_x = ags_timestamp_get_unix_time(current_timestamp);
	  
	g_object_unref(current_timestamp);

	if(current_x >= x &&
	   current_x < x + AGS_PATTERN_DEFAULT_DURATION){
	  retval = current;
	    
	  break;
	}
      }
    }else{
      g_warning("inconsistent data");
    }
    
    if(position == 0){
      break;
    }

    position = position / 2;

    if(current_x < x){
      current_start = current->next;
      current_end = current_end->prev;
    }else{
      current_start = current_start->next;
      current_end = current->prev;
    }    

    current = g_list_nth(current_start,
			 position);
  }

  return(retval);
}

/**
 * ags_pattern_get_channel:
 * @pattern: the #AgsPattern
 * 
 * Get channel.
 * 
 * Returns: (transfer full): the #AgsChannel
 * 
 * Since: 3.1.0
 */
GObject*
ags_pattern_get_channel(AgsPattern *pattern)
{
  GObject *channel;

  if(!AGS_IS_PATTERN(pattern)){
    return(NULL);
  }

  g_object_get(pattern,
	       "channel", &channel,
	       NULL);

  return(channel);
}

/**
 * ags_pattern_set_channel:
 * @pattern: the #AgsPattern
 * @channel: the #AgsChannel
 * 
 * Set channel.
 * 
 * Since: 3.1.0
 */
void
ags_pattern_set_channel(AgsPattern *pattern, GObject *channel)
{
  if(!AGS_IS_PATTERN(pattern)){
    return;
  }

  g_object_set(pattern,
	       "channel", channel,
	       NULL);
}

/**
 * ags_pattern_get_timestamp:
 * @pattern: the #AgsPattern
 * 
 * Get timestamp.
 * 
 * Returns: (transfer full): the #AgsTimestamp
 * 
 * Since: 3.1.0
 */
AgsTimestamp*
ags_pattern_get_timestamp(AgsPattern *pattern)
{
  AgsTimestamp *timestamp;

  if(!AGS_IS_PATTERN(pattern)){
    return(NULL);
  }

  g_object_get(pattern,
	       "timestamp", &timestamp,
	       NULL);

  return(timestamp);
}

/**
 * ags_pattern_set_timestamp:
 * @pattern: the #AgsPattern
 * @timestamp: the #AgsTimestamp
 * 
 * Set timestamp.
 * 
 * Since: 3.1.0
 */
void
ags_pattern_set_timestamp(AgsPattern *pattern, AgsTimestamp *timestamp)
{
  if(!AGS_IS_PATTERN(pattern)){
    return;
  }

  g_object_set(pattern,
	       "timestamp", timestamp,
	       NULL);
}

/**
 * ags_pattern_get_dim:
 * @pattern: an #AgsPattern
 * @dim0: (out): bank 0 size
 * @dim1: (out): bank 1 size
 * @length: (out): amount of beats
 *
 * Get the pattern's dimensions.
 *
 * Since: 3.1.0
 */
void 
ags_pattern_get_dim(AgsPattern *pattern, guint *dim0, guint *dim1, guint *length)
{
  GRecMutex *pattern_mutex;

  if(!AGS_IS_PATTERN(pattern)){
    return;
  }
  
  /* get pattern mutex */
  pattern_mutex = AGS_PATTERN_GET_OBJ_MUTEX(pattern);

  /* get dim */
  g_rec_mutex_lock(pattern_mutex);

  if(dim0 != NULL){
    dim0[0] = pattern->dim[0];
  }

  if(dim1 != NULL){
    dim1[0] = pattern->dim[1];
  }

  if(length != NULL){
    length[0] = pattern->dim[2];
  }
  
  g_rec_mutex_unlock(pattern_mutex);
}

/**
 * ags_pattern_set_dim:
 * @pattern: an #AgsPattern
 * @dim0: bank 0 size
 * @dim1: bank 1 size
 * @length: amount of beats
 *
 * Reallocates the pattern's dimensions.
 *
 * Since: 3.0.0
 */
void 
ags_pattern_set_dim(AgsPattern *pattern, guint dim0, guint dim1, guint length)
{
  AgsChannel *channel;
  
  guint ***index0, **index1, *bitmap;

  guint pad;
  guint i, j, k, j_set, k_set;
  guint bitmap_size;

  GRecMutex *pattern_mutex;

  if(!AGS_IS_PATTERN(pattern)){
    return;
  }

  channel = NULL;

  pad = 0;
  
  /* get pattern mutex */
  pattern_mutex = AGS_PATTERN_GET_OBJ_MUTEX(pattern);

  /* set dim */
  g_rec_mutex_lock(pattern_mutex);

  if(dim0 == 0 && pattern->pattern == NULL){
    g_rec_mutex_unlock(pattern_mutex);
    
    return;
  }
  
  // shrink
  if(pattern->dim[0] > dim0){
    for(i = dim0; i < pattern->dim[0]; i++){
      for(j = 0; j < pattern->dim[1]; j++){
	g_free(pattern->pattern[i][j]);
      }

      g_free(pattern->pattern[i]);

      pattern->pattern[i] = NULL;
    }

    if(dim0 == 0){
      g_free(pattern->pattern);
      
      pattern->pattern = NULL;
      pattern->dim[0] = 0;

      g_rec_mutex_unlock(pattern_mutex);
    
      return;
    }else{
      pattern->pattern = (guint ***) g_realloc(pattern->pattern,
					       (int) dim0 * sizeof(guint **));

      pattern->dim[0] = dim0;
    }
  }

  if(pattern->dim[1] > dim1){
    if(dim1 == 0){
      for(i = 0; i < pattern->dim[0]; i++){
	for(j = dim1; j < pattern->dim[1]; j++){
	  g_free(pattern->pattern[i][j]);
	}

	pattern->pattern[i] = NULL;
      }

      pattern->dim[1] = 0;
      
      g_rec_mutex_unlock(pattern_mutex);
    
      return;
    }else{
      for(i = 0; i < pattern->dim[0]; i++){
	for(j = dim1; j < pattern->dim[1]; j++){
	  g_free(pattern->pattern[i][j]);
	}
      }

      for(i = 0; pattern->dim[0]; i++){
	pattern->pattern[i] = (guint **) g_realloc(pattern->pattern[i],
						   dim1 * sizeof(guint *));
      }

      pattern->dim[1] = dim1;
    }
  }

  if(pattern->dim[2] > length){
    if(length == 0){
      for(i = 0; i < pattern->dim[0]; i++){
	for(j = 0; j < pattern->dim[1]; j++){
      	  g_free(pattern->pattern[i][j]);
	  
	  pattern->pattern[i][j] = NULL;
	}
      }

      for(k = 0; k < pattern->dim[2]; k++){
	g_object_unref(pattern->note[k]);
      }
      
      g_free(pattern->note);
      
      pattern->note = NULL;
      
      pattern->dim[2] = 0;
    }else{
      for(i = 0; i < pattern->dim[0]; i++){
	for(j = 0; j < pattern->dim[1]; j++){
	  pattern->pattern[i][j] = (guint *) g_realloc(pattern->pattern[i][j],
						       (guint) ceil((double) length / (double) (sizeof(guint) * 8)) * sizeof(guint));
	}
      }

      for(k = length; k < pattern->dim[2]; k++){
	g_object_unref(pattern->note[k]);
      }
      
      pattern->note = g_realloc(pattern->note,
				length * sizeof(AgsNote *));

      pattern->dim[2] = length;
    }
  }

  // grow
  bitmap_size = (guint) ceil((double) pattern->dim[2] / (double) (sizeof(guint) * 8)) * sizeof(guint);
    
  if(pattern->dim[0] < dim0){
    if(pattern->pattern == NULL){
      pattern->pattern = (guint ***) g_malloc(dim0 * sizeof(guint **));
    }else{
      pattern->pattern = (guint ***) g_realloc(pattern->pattern,
					       dim0 * sizeof(guint **));
    }

    for(i = pattern->dim[0]; i < dim0; i++){
      pattern->pattern[i] = (guint **) g_malloc(pattern->dim[1] * sizeof(guint *));

      for(j = 0; j < pattern->dim[1]; j++){
	if(bitmap_size == 0){
	  pattern->pattern[i][j] = NULL;
	}else{
	  pattern->pattern[i][j] = (guint *) g_malloc(bitmap_size);
	  memset(pattern->pattern[i][j], 0, bitmap_size);
	}
      }
    }

    pattern->dim[0] = dim0;
  }

  if(pattern->dim[1] < dim1){  
    for(i = 0; i < pattern->dim[0]; i++){
      if(pattern->pattern[i] == NULL){
	pattern->pattern[i] = (guint **) g_malloc(dim1 * sizeof(guint *));
      }else{
	pattern->pattern[i] = (guint **) g_realloc(pattern->pattern[i],
						   dim1 * sizeof(guint *));
      }

      for(j = pattern->dim[1]; j < dim1; j++){
	if(bitmap_size == 0){
	  pattern->pattern[i][j] = NULL;
	}else{
	  pattern->pattern[i][j] = (guint *) g_malloc(bitmap_size);
	  memset(pattern->pattern[i][j], 0, bitmap_size);
	}
      }
    }

    pattern->dim[1] = dim1;
  }

  if(pattern->dim[2] < length){
    guint new_bitmap_size;
    
    new_bitmap_size = (guint) ceil((double) length / (double) (sizeof(guint) * 8)) * sizeof(guint);
    
    for(i = 0; i < pattern->dim[0]; i++){
      for(j = 0; j < pattern->dim[1]; j++){
	if(pattern->pattern[i][j] == NULL){
	  pattern->pattern[i][j] = (guint *) g_malloc(new_bitmap_size);
	  memset(pattern->pattern[i][j], 0, new_bitmap_size);
	}else{
	  pattern->pattern[i][j] =(guint *) g_realloc(pattern->pattern[i][j],
						      new_bitmap_size);
	  memset(pattern->pattern[i][j] + bitmap_size, 0, new_bitmap_size - bitmap_size);
	}
      }
    }

    if(pattern->note == NULL){
      pattern->note = (AgsNote **) g_malloc(length * sizeof(AgsNote *));
    }else{
      pattern->note = (AgsNote **) g_realloc(pattern->note,
					     length * sizeof(AgsNote *));
    }

    channel = NULL;
    pad = 0;
    
    g_object_get(pattern,
		 "channel", &channel,
		 NULL);

    if(channel != NULL){
      g_object_get(channel,
		   "pad", &pad,
		   NULL);
    }
    
    for(k = pattern->dim[2]; k < length; k++){
      pattern->note[k] = ags_note_new();

      g_object_set(pattern->note[k],
		   "x0", k,
		   "x1", k + 1,
		   "y", pad,
		   NULL);
    }
      

    pattern->dim[2] = length;
  }

  g_rec_mutex_unlock(pattern_mutex);  
}

gboolean
ags_pattern_is_empty(AgsPattern *pattern, guint i, guint j)
{
  guint bitmap_length;
  guint n;

  GRecMutex *pattern_mutex;

  if(!AGS_IS_PATTERN(pattern)){
    return(TRUE);
  }
  
  /* get pattern mutex */
  pattern_mutex = AGS_PATTERN_GET_OBJ_MUTEX(pattern);

  if(!(i < pattern->dim[0] ||
       j < pattern->dim[1])){
    g_rec_mutex_unlock(pattern_mutex);

    return(FALSE);
  }

  /* check */
  g_rec_mutex_lock(pattern_mutex);

  bitmap_length = (guint) ceil((double) pattern->dim[2] / (double) (sizeof(guint) * 8));

  for(n = 0; n < bitmap_length; n++){
    if(pattern->pattern[i][j][n] != 0){
      g_rec_mutex_unlock(pattern_mutex);
      
      return(FALSE);
    }
  }

  g_rec_mutex_unlock(pattern_mutex);

  return(TRUE);
}

/**
 * ags_pattern_get_bit:
 * @pattern: an #AgsPattern
 * @i: bank index 0
 * @j: bank index 1
 * @bit: the tic to check
 *
 * Check for tic to be played.
 *
 * Returns: %TRUE if tone is enabled.
 *
 * Since: 3.0.0
 */
gboolean
ags_pattern_get_bit(AgsPattern *pattern, guint i, guint j, guint bit)
{
  guint k, value;

  GRecMutex *pattern_mutex;

  if(!AGS_IS_PATTERN(pattern)){
    return(FALSE);
  }

  /* get pattern mutex */
  pattern_mutex = AGS_PATTERN_GET_OBJ_MUTEX(pattern);

  /* get bit */
  g_rec_mutex_lock(pattern_mutex);

  if(!(i < pattern->dim[0] ||
       j < pattern->dim[1] ||
       bit < pattern->dim[2])){
    g_rec_mutex_unlock(pattern_mutex);

    return(FALSE);
  }

  k = (guint) floor((double) bit / (double) (sizeof(guint) * 8));
  value = 1 << (bit % (sizeof(guint) * 8));

#ifdef AGS_DEBUG
  g_message("i,j,bit = %d,%d,%d", i, j, bit);
  g_message("k = %d; value = %x", k, value);
#endif
  
  //((1 << (bit % (sizeof(guint) *8))) & (pattern->pattern[i][j][(guint) floor((double) bit / (double) (sizeof(guint) * 8))])) != 0
  if((value & (pattern->pattern[i][j][k])) != 0){
    g_rec_mutex_unlock(pattern_mutex);
    
    return(TRUE);
  }else{
    g_rec_mutex_unlock(pattern_mutex);
    
    return(FALSE);
  }
}

/**
 * ags_pattern_toggle_bit:
 * @pattern: an #AgsPattern
 * @i: bank index 0
 * @j: bank index 1
 * @bit: the tic to toggle
 *
 * Toggle tone.
 *
 * Since: 3.0.0
 */
void
ags_pattern_toggle_bit(AgsPattern *pattern, guint i, guint j, guint bit)
{
  guint k, value;

  GRecMutex *pattern_mutex;

  if(!AGS_IS_PATTERN(pattern)){
    return;
  }

  /* get pattern mutex */
  pattern_mutex = AGS_PATTERN_GET_OBJ_MUTEX(pattern);

  /* toggle */
  g_rec_mutex_lock(pattern_mutex);

  if(!(i < pattern->dim[0] ||
       j < pattern->dim[1] ||
       bit < pattern->dim[2])){
    g_rec_mutex_unlock(pattern_mutex);

    return;
  }
  
  k = (guint) floor((double) bit / (double) (sizeof(guint) * 8));
  value = 1 << (bit % (sizeof(guint) * 8));

  if((value & (pattern->pattern[i][j][k])) != 0){
    pattern->pattern[i][j][k] &= (~value);
  }else{
    pattern->pattern[i][j][k] |= value;
  }

  g_rec_mutex_unlock(pattern_mutex);
}

/**
 * ags_pattern_get_note:
 * @pattern: an #AgsPattern
 * @bit: the tic
 *
 * Get note of @pattern.
 *
 * Returns: (transfer full): the #AgsNote at given position or %NULL
 *
 * Since: 3.3.0
 */
AgsNote*
ags_pattern_get_note(AgsPattern *pattern, guint bit)
{
  AgsNote *note;
  
  GRecMutex *pattern_mutex;

  if(!AGS_IS_PATTERN(pattern)){
    return(NULL);
  }

  note = NULL;

  /* get pattern mutex */
  pattern_mutex = AGS_PATTERN_GET_OBJ_MUTEX(pattern);

  g_rec_mutex_lock(pattern_mutex);

  if(bit < pattern->dim[2]){
    note = pattern->note[bit];
    
    g_object_ref(note);
  }
  
  g_rec_mutex_unlock(pattern_mutex);

  return(note);
}

/**
 * ags_pattern_new:
 *
 * Creates a new instance of #AgsPattern
 *
 * Returns: the new #AgsPattern
 *
 * Since: 3.0.0
 */
AgsPattern*
ags_pattern_new()
{
  AgsPattern *pattern;

  pattern = (AgsPattern *) g_object_new(AGS_TYPE_PATTERN,
					NULL);

  return(pattern);
}
