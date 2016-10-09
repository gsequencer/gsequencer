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

#include <ags/server/controller/ags_controller.h>

void ags_controller_class_init(AgsControllerClass *controller);
void ags_controller_init(AgsController *controller);
void ags_controller_finalize(GObject *gobject);

/**
 * SECTION:ags_controller
 * @short_description: authentication by XML file
 * @title: AgsController
 * @section_id:
 * @include: ags/server/controller/ags_controller.h
 *
 * The #AgsController is a base object to implement controllers.
 */

static gpointer ags_controller_parent_class = NULL;

GType
ags_controller_get_type()
{
  static GType ags_type_controller = 0;

  if(!ags_type_controller){
    static const GTypeInfo ags_controller_info = {
      sizeof (AgsControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_controller_init,
    };
    
    ags_type_controller = g_type_register_static(G_TYPE_OBJECT,
						 "AgsController\0",
						 &ags_controller_info,
						 0);
  }

  return (ags_type_controller);
}

void
ags_controller_class_init(AgsControllerClass *controller)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_controller_parent_class = g_type_class_peek_parent(controller);

  /* GObjectClass */
  gobject = (GObjectClass *) controller;

  gobject->finalize = ags_controller_finalize;
}

void
ags_controller_init(AgsController *controller)
{
  controller->server = NULL;
}

void
ags_controller_finalize(GObject *gobject)
{
  AgsController *controller;

  controller = AGS_CONTROLLER(gobject);

  G_OBJECT_CLASS(ags_controller_parent_class)->finalize(gobject);
}

AgsController*
ags_controller_new()
{
  AgsController *controller;

  controller = (AgsController *) g_object_new(AGS_TYPE_CONTROLLER,
					      NULL);

  return(controller);
}
