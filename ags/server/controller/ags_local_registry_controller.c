/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/server/controller/ags_local_registry_controller.h>

#include <ags/object/ags_marshal.h>

#include <ags/server/security/ags_authentication_manager.h>

void ags_local_registry_controller_class_init(AgsLocalRegistryControllerClass *local_registry_controller);
void ags_local_registry_controller_init(AgsLocalRegistryController *local_registry_controller);
void ags_local_registry_controller_finalize(GObject *gobject);

gpointer ags_local_registry_controller_real_entry_bulk(AgsLocalRegistryController *local_registry_controller);

/**
 * SECTION:ags_local_registry_controller
 * @short_description: local registry controller
 * @title: AgsLocalRegistryController
 * @section_id:
 * @include: ags/server/controller/ags_local_registry_controller.h
 *
 * The #AgsLocalRegistryController is a controller.
 */

enum{
  CREATE_ENTRY_BULK,
  LAST_SIGNAL,
};

static gpointer ags_local_registry_controller_parent_class = NULL;
static guint local_registry_controller_signals[LAST_SIGNAL];

GType
ags_local_registry_controller_get_type()
{
  static GType ags_type_local_registry_controller = 0;

  if(!ags_type_local_registry_controller){
    static const GTypeInfo ags_local_registry_controller_info = {
      sizeof (AgsLocalRegistryControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_local_registry_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLocalRegistryController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_local_registry_controller_init,
    };
    
    ags_type_local_registry_controller = g_type_register_static(G_TYPE_OBJECT,
								"AgsLocalRegistryController",
								&ags_local_registry_controller_info,
								0);
  }

  return (ags_type_local_registry_controller);
}

void
ags_local_registry_controller_class_init(AgsLocalRegistryControllerClass *local_registry_controller)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_local_registry_controller_parent_class = g_type_class_peek_parent(local_registry_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) local_registry_controller;

  gobject->finalize = ags_local_registry_controller_finalize;

  /* AgsLocalRegistryController */
  local_registry_controller->entry_bulk = ags_local_registry_controller_real_entry_bulk;

  /* signals */
  /**
   * AgsLocalRegistryController::entry-bulk:
   * @local_registry_controller: the #AgsLocalRegistryController
   *
   * The ::entry-bulk signal is used to retrieve all registry entries.
   *
   * Returns: the response
   * 
   * Since: 1.0.0
   */
  local_registry_controller_signals[ENTRY_BULK] =
    g_signal_new("entry-bulk",
		 G_TYPE_FROM_CLASS(local_registry_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLocalRegistryControllerClass, entry_bulk),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);
}

void
ags_local_registry_controller_init(AgsLocalRegistryController *local_registry_controller)
{
  //TODO:JK: implement me
}

void
ags_local_registry_controller_finalize(GObject *gobject)
{
  AgsLocalRegistryController *local_registry_controller;

  local_registry_controller = AGS_LOCAL_REGISTRY_CONTROLLER(gobject);

  G_OBJECT_CLASS(ags_local_registry_controller_parent_class)->finalize(gobject);
}

gpointer
ags_local_registry_controller_real_entry_bulk(AgsLocalRegistryController *local_registry_controller)
{
}

gpointer
ags_local_registry_controller_entry_bulk(AgsLocalRegistryController *local_registry_controller)
{
}

AgsLocalRegistryController*
ags_local_registry_controller_new()
{
  AgsLocalRegistryController *local_registry_controller;

  local_registry_controller = (AgsLocalRegistryController *) g_object_new(AGS_TYPE_LOCAL_REGISTRY_CONTROLLER,
									  NULL);

  return(local_registry_controller);
}
