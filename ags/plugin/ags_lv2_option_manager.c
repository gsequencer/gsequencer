/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

void ags_lv2_option_manager_class_init(AgsLv2OptionManagerClass *lv2_option_manager);
void ags_lv2_option_manager_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_lv2_option_manager_init(AgsLv2OptionManager *lv2_option_manager);
void ags_lv2_option_manager_connect(AgsConnectable *connectable);
void ags_lv2_option_manager_disconnect(AgsConnectable *connectable);
void ags_lv2_option_manager_finalize(GObject *gobject);

void ags_lv2_option_manager_real_get_option(AgsLv2OptionManager *option_manager,
					    LV2_Handle instance,
					    LV2_Options_Option* options,
					    GError **error);
void ags_lv2_option_manager_real_set_option(AgsLv2OptionManager *option_manager,
					    LV2_Handle instance,
					    LV2_Options_Option* options,
					    GError **error);

void ags_lv2_option_manager_destroy_data(gpointer data);

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
							 "AgsLv2OptionManager\0",
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
  lv2_option_manager->get_option = ags_lv2_option_manager_get_option;
  lv2_option_manager->set_option = ags_lv2_option_manager_set_option;
}

GQuark
ags_lv2_option_manager_error_quark()
{
  return(g_quark_from_static_string("ags-lv2-option-manager-error-quark\0"));
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
  lv2_option_manager->ressource = g_hash_table_new_full(g_direct_hash, g_direct_equal,
						     NULL,
						     (GDestroyNotify) ags_lv2_option_manager_destroy_data);
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
}

void
ags_lv2_option_manager_destroy_data(gpointer data)
{
  /* empty */
}

/**
 * ags_lv2_option_manager_insert:
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
ags_lv2_option_manager_insert(AgsLv2OptionManager *lv2_option_manager,
			      AgsLv2OptionRessource *lv2_option_ressource, gpointer data)
{
  if(lv2_option_manager == NULL ||
     lv2_option_ressource == NULL ||
     data == NULL){
    return(FALSE);
  }

  g_hash_table_insert(lv2_option_manager->ressource,
		      lv2_option_ressource, data);

  return(TRUE);
}

/**
 * ags_lv2_option_manager_remove:
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
ags_lv2_option_manager_remove(AgsLv2OptionManager *lv2_option_manager,
			      AgsLv2OptionRessource *lv2_option_ressource)
{
  gpointer data;

  data = g_hash_table_lookup(lv2_option_manager->ressource,
			     lv2_option_ressource);

  if(data != NULL){
    g_hash_table_remove(lv2_option_manager->ressource,
			data);
  }
  
  return(TRUE);
}

/**
 * ags_lv2_option_manager_lookup:
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
ags_lv2_option_manager_lookup(AgsLv2OptionManager *lv2_option_manager,
			      AgsLv2OptionRessource *lv2_option_ressource)
{
  gpointer data;
  
  data = (gpointer) g_hash_table_lookup(lv2_option_manager->ressource,
					lv2_option_ressource);
  
  return(data);
}

void
ags_lv2_option_manager_real_get_option(AgsLv2OptionManager *lv2_option_manager,
				       LV2_Handle instance,
				       LV2_Options_Option* options,
				       GError **error)
{
  //TODO:JK: implement me
}

/**
 * ags_lv2_option_manager_get_option:
 * @lv2_option_manager: the #AgsLv2OptionManager
 * @instance: the instance
 * @options: the options
 * @error: the #GError-struct
 * 
 * Get option.
 * 
 * Since: 0.7.128
 */
void
ags_lv2_option_manager_get_option(AgsLv2OptionManager *lv2_option_manager,
				  LV2_Handle instance,
				  LV2_Options_Option* options,
				  GError **error)
{
  g_return_if_fail(AGS_IS_LV2_OPTION_MANAGER(lv2_option_manager));
  g_object_ref(G_OBJECT(lv2_option_manager));
  g_signal_emit(G_OBJECT(lv2_option_manager),
		lv2_option_manager_signals[GET_OPTION], 0,
		instance,
		options,
		error);
  g_object_unref(G_OBJECT(lv2_option_manager));
}

void
ags_lv2_option_manager_real_set_option(AgsLv2OptionManager *lv2_option_manager,
				       LV2_Handle instance,
				       LV2_Options_Option* options,
				       GError **error)
{
  //TODO:JK: implement me
}

/**
 * ags_lv2_option_manager_set_option:
 * @lv2_option_manager: the #AgsLv2OptionManager
 * @instance: the instance
 * @options: the options
 * @error: the #GError-struct
 * 
 * Set option.
 * 
 * Since: 0.7.128
 */
void
ags_lv2_option_manager_set_option(AgsLv2OptionManager *lv2_option_manager,
				  LV2_Handle instance,
				  LV2_Options_Option* options,
				  GError **error)
{
  g_return_if_fail(AGS_IS_LV2_OPTION_MANAGER(lv2_option_manager));
  g_object_ref(G_OBJECT(lv2_option_manager));
  g_signal_emit(G_OBJECT(lv2_option_manager),
		lv2_option_manager_signals[SET_OPTION], 0,
		instance,
		options,
		error);
  g_object_unref(G_OBJECT(lv2_option_manager));
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
  if(ags_lv2_option_manager == NULL){
    ags_lv2_option_manager = ags_lv2_option_manager_new();
    
    //    ags_lv2_option_manager_load_default(ags_lv2_option_manager);
  }

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
