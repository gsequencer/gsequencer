/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_OSC_CONFIG_CONTROLLER_H__
#define __AGS_OSC_CONFIG_CONTROLLER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/osc/controller/ags_osc_controller.h>

G_BEGIN_DECLS

#define AGS_TYPE_OSC_CONFIG_CONTROLLER                (ags_osc_config_controller_get_type())
#define AGS_OSC_CONFIG_CONTROLLER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_CONFIG_CONTROLLER, AgsOscConfigController))
#define AGS_OSC_CONFIG_CONTROLLER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_OSC_CONFIG_CONTROLLER, AgsOscConfigControllerClass))
#define AGS_IS_OSC_CONFIG_CONTROLLER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSC_CONFIG_CONTROLLER))
#define AGS_IS_OSC_CONFIG_CONTROLLER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSC_CONFIG_CONTROLLER))
#define AGS_OSC_CONFIG_CONTROLLER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_OSC_CONFIG_CONTROLLER, AgsOscConfigControllerClass))

typedef struct _AgsOscConfigController AgsOscConfigController;
typedef struct _AgsOscConfigControllerClass AgsOscConfigControllerClass;

struct _AgsOscConfigController
{
  AgsOscController osc_controller;
};

struct _AgsOscConfigControllerClass
{
  AgsOscControllerClass osc_controller;

  gpointer (*apply_config)(AgsOscConfigController *osc_config_controller,
			    AgsOscConnection *osc_connection,
			    unsigned char *message, guint message_size);
};

GType ags_osc_config_controller_get_type();

gpointer ags_osc_config_controller_apply_config(AgsOscConfigController *osc_config_controller,
						AgsOscConnection *osc_connection,
						unsigned char *message, guint message_size);

AgsOscConfigController* ags_osc_config_controller_new();

G_END_DECLS

#endif /*__AGS_OSC_CONFIG_CONTROLLER_H__*/
