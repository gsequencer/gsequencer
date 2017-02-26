/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/server/controller/ags_local_factory_controller.h>

#include <ags/object/ags_marshal.h>

#include <ags/server/security/ags_authentication_manager.h>

void ags_local_factory_controller_class_init(AgsLocalFactoryControllerClass *local_factory_controller);
void ags_local_factory_controller_init(AgsLocalFactoryController *local_factory_controller);
void ags_local_factory_controller_finalize(GObject *gobject);

/**
 * SECTION:ags_local_factory_controller
 * @short_description: local factory controller
 * @title: AgsLocalFactoryController
 * @section_id:
 * @include: ags/server/controller/ags_local_factory_controller.h
 *
 * The #AgsLocalFactoryController is a controller.
 */

enum{
  CREATE_INSTANCE,
  LAST_SIGNAL,
};

static gpointer ags_local_factory_controller_parent_class = NULL;
static guint local_factory_controller_signals[LAST_SIGNAL];

GType
ags_local_factory_controller_get_type()
{
  static GType ags_type_local_factory_controller = 0;

  if(!ags_type_local_factory_controller){
    static const GTypeInfo ags_local_factory_controller_info = {
      sizeof (AgsLocalFactoryControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_local_factory_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLocalFactoryController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_local_factory_controller_init,
    };
    
    ags_type_local_factory_controller = g_type_register_static(G_TYPE_OBJECT,
							       "AgsLocalFactoryController\0",
							       &ags_local_factory_controller_info,
							       0);
  }

  return (ags_type_local_factory_controller);
}

void
ags_local_factory_controller_class_init(AgsLocalFactoryControllerClass *local_factory_controller)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_local_factory_controller_parent_class = g_type_class_peek_parent(local_factory_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) local_factory_controller;

  gobject->finalize = ags_local_factory_controller_finalize;

  /* AgsLocalFactoryController */
  local_factory_controller->create_instance = ags_local_factory_controller_real_create_instance;

  /* signals */
  local_factory_controller_signals[CREATE_INSTANCE] =
    g_signal_new("create-instance\0",
		 G_TYPE_FROM_CLASS(local_factory_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLocalFactoryControllerClass, create_instance),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__ULONG_POINTER_UINT,
		 G_TYPE_POINTER, 3,
		 G_TYPE_ULONG,
		 G_TYPE_POINTER,
		 G_TYPE_UINT);
}

void
ags_local_factory_controller_init(AgsLocalFactoryController *local_factory_controller)
{
  //TODO:JK: implement me
}

void
ags_local_factory_controller_finalize(GObject *gobject)
{
  AgsLocalFactoryController *local_factory_controller;

  local_factory_controller = AGS_LOCAL_FACTORY_CONTROLLER(gobject);

  G_OBJECT_CLASS(ags_local_factory_controller_parent_class)->finalize(gobject);
}

AgsLocalFactoryController*
ags_local_factory_controller_new()
{
  AgsLocalFactoryController *local_factory_controller;

  local_factory_controller = (AgsLocalFactoryController *) g_object_new(AGS_TYPE_LOCAL_FACTORY_CONTROLLER,
									NULL);

  return(local_factory_controller);
}
