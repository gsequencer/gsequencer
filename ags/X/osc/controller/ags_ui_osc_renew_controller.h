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

#ifndef __AGS_UI_OSC_RENEW_CONTROLLER_H__
#define __AGS_UI_OSC_RENEW_CONTROLLER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#define AGS_TYPE_UI_OSC_RENEW_CONTROLLER                (ags_ui_osc_renew_controller_get_type())
#define AGS_UI_OSC_RENEW_CONTROLLER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_UI_OSC_RENEW_CONTROLLER, AgsUiOscRenewController))
#define AGS_UI_OSC_RENEW_CONTROLLER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_UI_OSC_RENEW_CONTROLLER, AgsUiOscRenewControllerClass))
#define AGS_IS_UI_OSC_RENEW_CONTROLLER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_UI_OSC_RENEW_CONTROLLER))
#define AGS_IS_UI_OSC_RENEW_CONTROLLER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_UI_OSC_RENEW_CONTROLLER))
#define AGS_UI_OSC_RENEW_CONTROLLER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_UI_OSC_RENEW_CONTROLLER, AgsUiOscRenewControllerClass))

#define AGS_UI_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH (12)

typedef struct _AgsUiOscRenewController AgsUiOscRenewController;
typedef struct _AgsUiOscRenewControllerClass AgsUiOscRenewControllerClass;

struct _AgsUiOscRenewController
{
  AgsOscController osc_controller;
};

struct _AgsUiOscRenewControllerClass
{
  AgsOscControllerClass osc_controller;

  gpointer (*set_data)(AgsUiOscRenewController *ui_osc_renew_controller,
		       AgsOscConnection *osc_connection,
		       unsigned char *message, guint message_size);
};

GType ags_ui_osc_renew_controller_get_type();

gpointer ags_ui_osc_renew_controller_set_data(AgsUiOscRenewController *ui_osc_renew_controller,
					      AgsOscConnection *osc_connection,
					      unsigned char *message, guint message_size);

void ags_ui_osc_renew_controller_add_monitor(AgsUiOscRenewController *ui_osc_renew_controller);
void ags_ui_osc_renew_controller_remove_monitor(AgsUiOscRenewController *ui_osc_renew_controller);

gboolean ags_ui_osc_renew_controller_message_monitor_timeout(AgsUiOscRenewController *ui_osc_renew_controller);

AgsUiOscRenewController* ags_ui_osc_renew_controller_new();

#endif /*__AGS_UI_OSC_RENEW_CONTROLLER_H__*/
