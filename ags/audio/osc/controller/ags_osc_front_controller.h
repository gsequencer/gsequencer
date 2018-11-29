/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#ifndef __AGS_OSC_FRONT_CONTROLLER_H__
#define __AGS_OSC_FRONT_CONTROLLER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/osc/controller/ags_osc_controller.h>

#define AGS_TYPE_OSC_FRONT_CONTROLLER                (ags_osc_front_controller_get_type())
#define AGS_OSC_FRONT_CONTROLLER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_FRONT_CONTROLLER, AgsOscFrontController))
#define AGS_OSC_FRONT_CONTROLLER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_OSC_FRONT_CONTROLLER, AgsOscFrontControllerClass))
#define AGS_IS_OSC_FRONT_CONTROLLER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSC_FRONT_CONTROLLER))
#define AGS_IS_OSC_FRONT_CONTROLLER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSC_FRONT_CONTROLLER))
#define AGS_OSC_FRONT_CONTROLLER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_OSC_FRONT_CONTROLLER, AgsOscFrontControllerClass))

#define AGS_OSC_FRONT_CONTROLLER_MESSAGE(ptr) ((AgsOscFrontControllerMessage *)(ptr))

typedef struct _AgsOscFrontController AgsOscFrontController;
typedef struct _AgsOscFrontControllerClass AgsOscFrontControllerClass;
typedef struct _AgsOscFrontControllerMessage AgsOscFrontControllerMessage;

typedef enum{
  AGS_OSC_FRONT_CONTROLLER_DELEGATE_STARTED        = 1,
  AGS_OSC_FRONT_CONTROLLER_DELEGATE_RUNNING        = 1 <<  1,
  AGS_OSC_FRONT_CONTROLLER_DELEGATE_TERMINATING    = 1 <<  2,
}AgsOscFrontControllerFlags;

struct _AgsOscFrontController
{
  AgsOscController osc_controller;

  guint flags;
  
  struct timespec *delegate_timeout;
  
  volatile gboolean do_reset;

  pthread_mutex_t *delegate_mutex;
  pthread_cond_t *delegate_cond;
  
  pthread_t *delegate_thread;

  GList *message;
};

struct _AgsOscFrontControllerClass
{
  AgsOscControllerClass osc_controller;

  void (*start_delegate)(AgsOscFrontController *osc_front_controller);
  void (*stop_delegate)(AgsOscFrontController *osc_front_controller);

  gpointer (*do_request)(AgsOscFrontController *osc_front_controller,
			 AgsOscConnection *osc_connection,
			 unsigned char *packet, guint packet_size);
};

struct _AgsOscFrontControllerMessage
{
  AgsOscConnection *osc_connection;
  
  gint32 tv_secs;
  gint32 tv_fraction;
  gboolean immediately;

  guint message_size;

  unsigned char *message;
};

GType ags_osc_front_controller_get_type();

gboolean ags_osc_front_controller_test_flags(AgsOscFrontController *osc_front_controller, guint flags);
void ags_osc_front_controller_set_flags(AgsOscFrontController *osc_front_controller, guint flags);
void ags_osc_front_controller_unset_flags(AgsOscFrontController *osc_front_controller, guint flags);

gint ags_osc_front_controller_message_sort_func(gconstpointer a,
						gconstpointer b);

AgsOscFrontControllerMessage* ags_osc_front_controller_message_alloc();
void ags_osc_front_controller_message_free(AgsOscFrontControllerMessage *message);

void ags_osc_front_controller_add_message(AgsOscFrontController *osc_front_controller,
					  AgsOscFrontControllerMessage *message);
void ags_osc_front_controller_remove_message(AgsOscFrontController *osc_front_controller,
					     AgsOscFrontControllerMessage *message);

void ags_osc_front_controller_start_delegate(AgsOscFrontController *osc_front_controller);
void ags_osc_front_controller_stop_delegate(AgsOscFrontController *osc_front_controller);

gpointer ags_osc_front_controller_do_request(AgsOscFrontController *osc_front_controller,
					     AgsOscConnection *osc_connection,
					     unsigned char *packet, guint packet_size);

AgsOscFrontController* ags_osc_front_controller_new();

#endif /*__AGS_OSC_FRONT_CONTROLLER_H__*/
