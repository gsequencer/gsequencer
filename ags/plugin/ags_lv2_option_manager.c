/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015,2017 Joël Krähemann
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

#include <ags/plugin/ags_lv2_option_manager.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_marshal.h>

#include <stdlib.h>
#include <string.h>

void ags_lv2_option_manager_class_init(AgsLv2OptionManagerClass *lv2_option_manager);
void ags_lv2_option_manager_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_lv2_option_manager_init(AgsLv2OptionManager *lv2_option_manager);
void ags_lv2_option_manager_connect(AgsConnectable *connectable);
void ags_lv2_option_manager_disconnect(AgsConnectable *connectable);
void ags_lv2_option_manager_finalize(GObject *gobject);

void ags_lv2_option_manager_real_get_option(AgsLv2OptionManager *option_manager,
					    gpointer instance,
					    gpointer options,
					    gpointer retval);
void ags_lv2_option_manager_real_set_option(AgsLv2OptionManager *option_manager,
					    gpointer instance,
					    gpointer options,
					    gpointer retval);

void ags_lv2_option_manager_destroy_data(gpointer data);
gboolean ags_lv2_option_ressource_equal(gpointer a, gpointer b);
gboolean ags_lv2_option_ressource_finder(gpointer key, gpointer value, gpointer user_data);

/**
 * SECTION:ags_lv2_option_manager
 * @short_description: option manager
 * @title: AgsLv2OptionManager
 * @section_id:
 * @include: ags/plugin/ags_lv2_option_manager.h
 *
 * The #AgsLv2OptionManager gives you access to plugin instances global
 * configuration. And stores instance related ressources.
 */

enum{
  GET_OPTION,
  SET_OPTION,
  LAST_SIGNAL,
};

static gpointer ags_lv2_option_manager_parent_class = NULL;
static guint lv2_option_manager_signals[LAST_SIGNAL];

AgsLv2OptionManager *ags_lv2_option_manager = NULL;

GType
ags_lv2_option_manager_get_type()
{
  static GType ags_type_lv2_option_manager = 0;

  if(!ags_type_lv2_option_manager){
    const GTypeInfo ags_lv2_option_manager_info = {
      sizeof (AgsLv2OptionManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lv2_option_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLv2OptionManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_option_manager_init,
    };

    const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_lv2_option_manager_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_lv2_option_manager = g_type_register_static(G_TYPE_OBJECT,
							 "AgsLv2OptionManager",
							 &ags_lv2_option_manager_info,
							 0);

    g_type_add_interface_static(ags_type_lv2_option_manager,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return(ags_type_lv2_option_manager);
}

void
ags_lv2_option_manager_class_init(AgsLv2OptionManagerClass *lv2_option_manager)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_lv2_option_manager_parent_class = g_type_class_peek_parent(lv2_option_manager);

  /* GObject */
  gobject = (GObjectClass *) lv2_option_manager;

  gobject->finalize = ags_lv2_option_manager_finalize;

  /* AgsLv2OptionManager  */
  lv2_option_manager->get_option = ags_lv2_option_manager_real_get_option;
  lv2_option_manager->set_option = ags_lv2_option_manager_real_set_option;

  /* signals */
  /**
   * AgsLv2OptionManager::get-option:
   * @option_manager: the plugin to instantiate
   * @intstance: the LV2 plugin instance
   * @option: the LV2 option pointer
   * @retval: the location of the return value
   *
   * The ::get-option signal gets options of the manager.
   * 
   * Since: 0.7.128
   */
  lv2_option_manager_signals[GET_OPTION] =
    g_signal_new("get-option",
		 G_TYPE_FROM_CLASS(lv2_option_manager),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLv2OptionManagerClass, get_option),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__POINTER_POINTER_POINTER,
		 G_TYPE_NONE, 3,
		 G_TYPE_POINTER,
		 G_TYPE_POINTER,
		 G_TYPE_POINTER);

  /**
   * AgsLv2OptionManager::set-option:
   * @option_manager: the plugin to instantiate
   * @intstance: the LV2 plugin instance
   * @option: the LV2 option pointer
   * @retval: the location of the return value
   *
   * The ::set-option signal sets options for the manager.
   * 
   * Since: 0.7.128
   */
  lv2_option_manager_signals[SET_OPTION] =
    g_signal_new("set-option",
		 G_TYPE_FROM_CLASS(lv2_option_manager),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLv2OptionManagerClass, set_option),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__POINTER_POINTER_POINTER,
		 G_TYPE_NONE, 3,
		 G_TYPE_POINTER,
		 G_TYPE_POINTER,
		 G_TYPE_POINTER);
}

void
ags_lv2_option_manager_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_lv2_option_manager_connect;
  connectable->disconnect = ags_lv2_option_manager_disconnect;
}

void
ags_lv2_option_manager_init(AgsLv2OptionManager *lv2_option_manager)
{
  lv2_option_manager->ressource = g_hash_table_new_full(g_direct_hash, (GEqualFunc) ags_lv2_option_ressource_equal,
							NULL,
							(GDestroyNotify) ags_lv2_option_manager_destroy_data);
  g_hash_table_ref(lv2_option_manager->ressource);
}

void
ags_lv2_option_manager_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_lv2_option_manager_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_lv2_option_manager_finalize(GObject *gobject)
{
  AgsLv2OptionManager *lv2_option_manager;

  lv2_option_manager = AGS_LV2_OPTION_MANAGER(gobject);
  
  g_hash_table_destroy(lv2_option_manager->ressource);

  if(lv2_option_manager == ags_lv2_option_manager){
    ags_lv2_option_manager = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_lv2_option_manager_parent_class)->finalize(gobject);  
}

void
ags_lv2_option_manager_destroy_data(gpointer data)
{
  /* empty */
}

gboolean
ags_lv2_option_ressource_equal(gpointer a, gpointer b)
{
  AgsLv2OptionRessource *lv2_option_ressource, *requested_lv2_option_ressource;

  lv2_option_ressource = AGS_LV2_OPTION_RESSOURCE(a);
  requested_lv2_option_ressource = AGS_LV2_OPTION_RESSOURCE(b);
  
  if(lv2_option_ressource->instance == requested_lv2_option_ressource->instance &&
     lv2_option_ressource->option->subject == requested_lv2_option_ressource->option->subject &&
     lv2_option_ressource->option->key == requested_lv2_option_ressource->option->key){
    return(TRUE);
  }

  return(FALSE);
}

gboolean
ags_lv2_option_ressource_finder(gpointer key, gpointer value, gpointer user_data)
{
  AgsLv2OptionRessource *lv2_option_ressource, *requested_lv2_option_ressource;

  LV2_Options_Option *option;

  lv2_option_ressource = AGS_LV2_OPTION_RESSOURCE(key);
  requested_lv2_option_ressource = AGS_LV2_OPTION_RESSOURCE(user_data);
  
  if(lv2_option_ressource->instance == requested_lv2_option_ressource->instance &&
     lv2_option_ressource->option->subject == requested_lv2_option_ressource->option->subject &&
     lv2_option_ressource->option->key == requested_lv2_option_ressource->option->key){
    return(TRUE);
  }

  return(FALSE);
}

/**
 * ags_lv2_option_ressource_alloc:
 *
 * Allocate an #AgsLv2OptionRessource.
 * 
 * Returns: the newly created #AgsLv2OptionRessource-struct
 *
 * Since: 0.7.128
 */
AgsLv2OptionRessource*
ags_lv2_option_ressource_alloc()
{
  AgsLv2OptionRessource *lv2_option_ressource;

  lv2_option_ressource = (AgsLv2OptionRessource *) malloc(sizeof(AgsLv2OptionRessource));

  lv2_option_ressource->instance = NULL;
  
  lv2_option_ressource->option = (LV2_Options_Option *) malloc(sizeof(LV2_Options_Option));

  lv2_option_ressource->option->context = 0;
  lv2_option_ressource->option->subject = 0;
  lv2_option_ressource->option->key = 0;
  
  lv2_option_ressource->option->size = 0;
  lv2_option_ressource->option->type = 0;  
  lv2_option_ressource->option->value = NULL;
  
  return(lv2_option_ressource);
}

/**
 * ags_lv2_option_manager_ressource_insert:
 * @lv2_option_manager: the #AgsLv2OptionManager
 * @lv2_option_ressource: the #AgsOptionRessource-struct as key
 * @data: the data
 *
 * Inserts a data into hash associated with @lv2_option_ressource.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 0.7.128
 */
gboolean
ags_lv2_option_manager_ressource_insert(AgsLv2OptionManager *lv2_option_manager,
					AgsLv2OptionRessource *lv2_option_ressource, gpointer data)
{
  if(!AGS_IS_LV2_OPTION_MANAGER(lv2_option_manager) ||
     lv2_option_ressource == NULL ||
     data == NULL){
    return(FALSE);
  }

  g_hash_table_insert(lv2_option_manager->ressource,
		      (gpointer) lv2_option_ressource, data);

  return(TRUE);
}

/**
 * ags_lv2_option_manager_ressource_remove:
 * @lv2_option_manager: the #AgsLv2OptionManager
 * @lv2_option_ressource: the struct to remove
 * 
 * Removes an entry associated with @lv2_option_ressource.
 *
 * Returns: %TRUE as successfully removed, otherwise %FALSE
 *
 * Since: 0.7.128
 */
gboolean
ags_lv2_option_manager_ressource_remove(AgsLv2OptionManager *lv2_option_manager,
					AgsLv2OptionRessource *lv2_option_ressource)
{
  gpointer data;

  if(!AGS_IS_LV2_OPTION_MANAGER(lv2_option_manager) ||
     lv2_option_ressource == NULL){
    return(FALSE);
  }

  g_hash_table_remove(lv2_option_manager->ressource,
		      lv2_option_ressource);
  
  return(TRUE);
}

/**
 * ags_lv2_option_manager_ressource_lookup:
 * @lv2_option_manager: the #AgsLv2OptionManager
 * @lv2_option_ressource: the #AgsLv2OptionRessource to lookup
 *
 * Lookup a ressource associated with @lv2_option_ressource in
 * @lv2_option_manager.
 *
 * Returns: the pointer on success, else NULL
 *
 * Since: 0.7.128
 */
gpointer
ags_lv2_option_manager_ressource_lookup(AgsLv2OptionManager *lv2_option_manager,
					AgsLv2OptionRessource *lv2_option_ressource)
{
  gpointer data;

  if(!AGS_IS_LV2_OPTION_MANAGER(lv2_option_manager) ||
     lv2_option_ressource == NULL){
    return(NULL);
  }
  
  data = (gpointer) g_hash_table_lookup(lv2_option_manager->ressource,
					lv2_option_ressource);
  
  return(data);
}

void
ags_lv2_option_manager_real_get_option(AgsLv2OptionManager *lv2_option_manager,
				       gpointer instance,
				       gpointer option,
				       gpointer retval)
{
  AgsLv2OptionRessource *lv2_option_ressource;
  
  gpointer data;
  gpointer key_ptr, value_ptr;

  uint32_t *ret;
  
  /* initial set to success */
  ret = (uint32_t *) retval;
  
  if(ret != NULL){
    *ret = 0;
  }
  
  /* check option to be non NULL */
  if(option == NULL){
    if(ret != NULL){
      *ret |= (LV2_OPTIONS_ERR_BAD_SUBJECT |
		  LV2_OPTIONS_ERR_BAD_KEY);
    }

    return;
  }

  /* get option */
  if(AGS_LV2_OPTIONS_OPTION(option)->context == LV2_OPTIONS_RESOURCE){
    key_ptr = NULL;
    value_ptr = NULL;

    lv2_option_ressource = (AgsLv2OptionRessource *) malloc(sizeof(AgsLv2OptionRessource));
    
    lv2_option_ressource->instance = (LV2_Handle) instance;
    lv2_option_ressource->option = (LV2_Options_Option *) option;

    //    g_message("%x %x %x", lv2_option_manager, option, instance);
    
    if(g_hash_table_lookup_extended(lv2_option_manager->ressource,
				    lv2_option_ressource,
				    &key_ptr, &value_ptr)){
      /* set requested fields */
      AGS_LV2_OPTIONS_OPTION(option)->type = AGS_LV2_OPTION_RESSOURCE(key_ptr)->option->type;
      AGS_LV2_OPTIONS_OPTION(option)->size = AGS_LV2_OPTION_RESSOURCE(key_ptr)->option->size;
      AGS_LV2_OPTIONS_OPTION(option)->value = value_ptr;
    }else{      
      /* do error reporting */
      if(ret != NULL){
	*ret |= (LV2_OPTIONS_ERR_BAD_SUBJECT |
		 LV2_OPTIONS_ERR_BAD_KEY);
      }
    }
  }
}

/**
 * ags_lv2_option_manager_get_option:
 * @lv2_option_manager: the #AgsLv2OptionManager
 * @instance: the instance
 * @option: the option
 * @retval: return value for #LV2_Options_Status-enum
 * 
 * Get option.
 * 
 * Since: 0.7.128
 */
void
ags_lv2_option_manager_get_option(AgsLv2OptionManager *lv2_option_manager,
				  gpointer instance,
				  gpointer option,
				  gpointer retval)
{
  g_return_if_fail(AGS_IS_LV2_OPTION_MANAGER(lv2_option_manager));
  g_object_ref(G_OBJECT(lv2_option_manager));
  g_signal_emit(G_OBJECT(lv2_option_manager),
		lv2_option_manager_signals[GET_OPTION], 0,
		instance,
		option,
		retval);
  g_object_unref(G_OBJECT(lv2_option_manager));
}

void
ags_lv2_option_manager_real_set_option(AgsLv2OptionManager *lv2_option_manager,
				       gpointer instance,
				       gpointer option,
				       gpointer retval)
{
  gpointer data;
  gpointer key_ptr, value_ptr;

  uint32_t *ret;
  
  /* initial set to success */
  ret = (uint32_t *) retval;

  if(ret != NULL){
    *ret = 0;
  }

  /* check option to be non NULL */
  if(option == NULL){
    *ret |= (LV2_OPTIONS_ERR_BAD_SUBJECT |
		LV2_OPTIONS_ERR_BAD_KEY);
    
    return;
  }

  /* set option */
  if(AGS_LV2_OPTIONS_OPTION(option)->context == LV2_OPTIONS_RESOURCE){
    key_ptr = NULL;
    value_ptr = NULL;
      
    if(g_hash_table_lookup_extended(lv2_option_manager->ressource,
				    option,
				    &key_ptr, &value_ptr)){
      /* set fields */
      AGS_LV2_OPTION_RESSOURCE(key_ptr)->option->type = AGS_LV2_OPTIONS_OPTION(option)->type;
      AGS_LV2_OPTION_RESSOURCE(key_ptr)->option->size = AGS_LV2_OPTIONS_OPTION(option)->size;
      AGS_LV2_OPTION_RESSOURCE(key_ptr)->option->value = AGS_LV2_OPTIONS_OPTION(option)->value;
    }else{
      AgsLv2OptionRessource *lv2_option_ressource;
      
      /* allocate new */
      lv2_option_ressource = ags_lv2_option_ressource_alloc();

      /* set fields */
      lv2_option_ressource->instance = instance;
      
      lv2_option_ressource->option->context = AGS_LV2_OPTIONS_OPTION(option)->context;
      lv2_option_ressource->option->subject = AGS_LV2_OPTIONS_OPTION(option)->subject;
      lv2_option_ressource->option->key = AGS_LV2_OPTIONS_OPTION(option)->key;

      lv2_option_ressource->option->type = AGS_LV2_OPTIONS_OPTION(option)->type;
      lv2_option_ressource->option->size = AGS_LV2_OPTIONS_OPTION(option)->size;
      lv2_option_ressource->option->value = AGS_LV2_OPTIONS_OPTION(option)->value;

      ags_lv2_option_manager_ressource_insert(lv2_option_manager,
					      lv2_option_ressource, (gpointer) AGS_LV2_OPTIONS_OPTION(option)->value);
    }
  }
}

/**
 * ags_lv2_option_manager_set_option:
 * @lv2_option_manager: the #AgsLv2OptionManager
 * @instance: the instance
 * @option: the option
 * @retval: return value for #LV2_Options_Status-enum
 * 
 * Set option.
 * 
 * Since: 0.7.128
 */
void
ags_lv2_option_manager_set_option(AgsLv2OptionManager *lv2_option_manager,
				  gpointer instance,
				  gpointer option,
				  gpointer retval)
{
  g_return_if_fail(AGS_IS_LV2_OPTION_MANAGER(lv2_option_manager));
  g_object_ref(G_OBJECT(lv2_option_manager));
  g_signal_emit(G_OBJECT(lv2_option_manager),
		lv2_option_manager_signals[SET_OPTION], 0,
		instance,
		option,
		retval);
  g_object_unref(G_OBJECT(lv2_option_manager));
}

/**
 * ags_lv2_option_manager_lv2_options_get:
 * @instance: the lv2 instance
 * @options: the LV2_Options
 * 
 * The LV2 options interface's get method.
 * 
 * Since: 0.7.128
 */
uint32_t
ags_lv2_option_manager_lv2_options_get(LV2_Handle instance,
				       LV2_Options_Option* options)
{
  guint retval, tmpval;
  guint i;
  
  if(options == NULL){
    return(LV2_OPTIONS_ERR_BAD_SUBJECT |
	   LV2_OPTIONS_ERR_BAD_KEY);
  }

  retval = 0;
  
  for(i = 0; ; i++){
    if(options[i].subject == 0 &&
       options[i].key == 0 &&
       options[i].type == 0 &&
       options[i].size == 0 &&
       options[i].value == NULL){
      break;
    }

    ags_lv2_option_manager_get_option(ags_lv2_option_manager_get_instance(),
				      instance,
				      &(options[i]),
				      &tmpval);

    retval |= tmpval;
  }

  return(retval);
}

/**
 * ags_lv2_option_manager_lv2_options_set:
 * @instance: the lv2 instance
 * @options: the LV2_Options
 * 
 * The LV2 options interface's set method.
 * 
 * Since: 0.7.128
 */
uint32_t
ags_lv2_option_manager_lv2_options_set(LV2_Handle instance,
				       LV2_Options_Option* options)
{
  guint retval, tmpval;
  guint i;
  
  if(options == NULL){
    return(LV2_OPTIONS_ERR_BAD_SUBJECT |
	   LV2_OPTIONS_ERR_BAD_KEY);
  }

  retval = 0;
  
  for(i = 0; ; i++){
    if(options[i].subject == 0 &&
       options[i].key == 0 &&
       options[i].type == 0 &&
       options[i].size == 0 &&
       options[i].value == NULL){
      break;
    }

    ags_lv2_option_manager_set_option(ags_lv2_option_manager_get_instance(),
				      instance,
				      &(options[i]),
				      &tmpval);

    retval |= tmpval;
  }

  return(retval);
}

/**
 * ags_lv2_option_manager_get_instance:
 * 
 * Singleton function to optain the id manager instance.
 *
 * Returns: an instance of #AgsLv2OptionManager
 *
 * Since: 0.7.128
 */
AgsLv2OptionManager*
ags_lv2_option_manager_get_instance()
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&mutex);

  if(ags_lv2_option_manager == NULL){
    ags_lv2_option_manager = ags_lv2_option_manager_new();
    
    //    ags_lv2_option_manager_load_default(ags_lv2_option_manager);
  }

  pthread_mutex_unlock(&mutex);

  return(ags_lv2_option_manager);
}

/**
 * ags_lv2_option_manager_new:
 *
 * Instantiate a id manager. 
 *
 * Returns: a new #AgsLv2OptionManager
 *
 * Since: 0.7.128
 */
AgsLv2OptionManager*
ags_lv2_option_manager_new()
{
  AgsLv2OptionManager *lv2_option_manager;

  lv2_option_manager = (AgsLv2OptionManager *) g_object_new(AGS_TYPE_LV2_OPTION_MANAGER,
							    NULL);

  return(lv2_option_manager);
}
