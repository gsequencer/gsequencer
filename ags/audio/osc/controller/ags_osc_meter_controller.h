/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_OSC_METER_CONTROLLER_H__
#define __AGS_OSC_METER_CONTROLLER_H__

#include <glib.h>
#include <glib-object.h>

#include <time.h>

#include <ags/libags.h>

#include <ags/audio/ags_port.h>

#include <ags/audio/osc/controller/ags_osc_controller.h>

G_BEGIN_DECLS

#define AGS_TYPE_OSC_METER_CONTROLLER                (ags_osc_meter_controller_get_type())
#define AGS_OSC_METER_CONTROLLER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_METER_CONTROLLER, AgsOscMeterController))
#define AGS_OSC_METER_CONTROLLER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_OSC_METER_CONTROLLER, AgsOscMeterControllerClass))
#define AGS_IS_OSC_METER_CONTROLLER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSC_METER_CONTROLLER))
#define AGS_IS_OSC_METER_CONTROLLER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSC_METER_CONTROLLER))
#define AGS_OSC_METER_CONTROLLER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_OSC_METER_CONTROLLER, AgsOscMeterControllerClass))

#define AGS_OSC_METER_CONTROLLER_DEFAULT_MONITOR_TIMEOUT (1.0 / 30.0)
  
#define AGS_OSC_METER_CONTROLLER_MONITOR(ptr) ((AgsOscMeterControllerMonitor *)(ptr))

typedef struct _AgsOscMeterController AgsOscMeterController;
typedef struct _AgsOscMeterControllerClass AgsOscMeterControllerClass;
typedef struct _AgsOscMeterControllerMonitor AgsOscMeterControllerMonitor;

typedef enum{
  AGS_OSC_METER_CONTROLLER_MONITOR_STARTED        = 1,
  AGS_OSC_METER_CONTROLLER_MONITOR_RUNNING        = 1 <<  1,
  AGS_OSC_METER_CONTROLLER_MONITOR_TERMINATING    = 1 <<  2,
}AgsOscMeterControllerFlags;

struct _AgsOscMeterController
{
  AgsOscController osc_controller;

  AgsOscMeterControllerFlags flags;
  
  GList *monitor;
};

struct _AgsOscMeterControllerClass
{
  AgsOscControllerClass osc_controller;

  void (*start_monitor)(AgsOscMeterController *osc_meter_controller);
  void (*stop_monitor)(AgsOscMeterController *osc_meter_controller);
	
  gpointer (*monitor_meter)(AgsOscMeterController *osc_meter_controller,
			    AgsOscConnection *osc_connection,
			    guchar *message, guint message_size);
};

struct _AgsOscMeterControllerMonitor
{
  volatile gint ref_count;
  
  AgsOscConnection *osc_connection;

  gchar *path;
  AgsPort *port;
};

GType ags_osc_meter_controller_get_type();

gboolean ags_osc_meter_controller_test_flags(AgsOscMeterController *osc_meter_controller, AgsOscMeterControllerFlags flags);
void ags_osc_meter_controller_set_flags(AgsOscMeterController *osc_meter_controller, AgsOscMeterControllerFlags flags);
void ags_osc_meter_controller_unset_flags(AgsOscMeterController *osc_meter_controller, AgsOscMeterControllerFlags flags);

AgsOscMeterControllerMonitor* ags_osc_meter_controller_monitor_alloc();
void ags_osc_meter_controller_monitor_free(AgsOscMeterControllerMonitor *monitor);

void ags_osc_meter_controller_monitor_ref(AgsOscMeterControllerMonitor *monitor);
void ags_osc_meter_controller_monitor_unref(AgsOscMeterControllerMonitor *monitor);

GList* ags_osc_meter_controller_monitor_find_path(GList *monitor,
						  gchar *path);
GList* ags_osc_meter_controller_monitor_find_port(GList *monitor,
						  AgsPort *port);

void ags_osc_meter_controller_add_monitor(AgsOscMeterController *osc_meter_controller,
					  AgsOscMeterControllerMonitor *monitor);
void ags_osc_meter_controller_remove_monitor(AgsOscMeterController *osc_meter_controller,
					     AgsOscMeterControllerMonitor *monitor);

gboolean ags_osc_meter_controller_contains_monitor(AgsOscMeterController *osc_meter_controller,
						   AgsOscConnection *osc_connection,
						   AgsPort *port);

void ags_osc_meter_controller_start_monitor(AgsOscMeterController *osc_meter_controller);
void ags_osc_meter_controller_stop_monitor(AgsOscMeterController *osc_meter_controller);

gpointer ags_osc_meter_controller_monitor_meter(AgsOscMeterController *osc_meter_controller,
						AgsOscConnection *osc_connection,
						guchar *message, guint message_size);

AgsOscMeterController* ags_osc_meter_controller_new();

G_END_DECLS

#endif /*__AGS_OSC_METER_CONTROLLER_H__*/
