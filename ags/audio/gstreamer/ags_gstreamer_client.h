/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_GSTREAMER_CLIENT_H__
#define __AGS_GSTREAMER_CLIENT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/ags_api_config.h>

#if defined(AGS_WITH_GSTREAMER)
#include <gst/gst.h>
#endif

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_GSTREAMER_CLIENT                (ags_gstreamer_client_get_type())
#define AGS_GSTREAMER_CLIENT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_GSTREAMER_CLIENT, AgsGstreamerClient))
#define AGS_GSTREAMER_CLIENT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_GSTREAMER_CLIENT, AgsGstreamerClient))
#define AGS_IS_GSTREAMER_CLIENT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_GSTREAMER_CLIENT))
#define AGS_IS_GSTREAMER_CLIENT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_GSTREAMER_CLIENT))
#define AGS_GSTREAMER_CLIENT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_GSTREAMER_CLIENT, AgsGstreamerClientClass))

#define AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(obj) (&(((AgsGstreamerClient *) obj)->obj_mutex))

typedef struct _AgsGstreamerClient AgsGstreamerClient;
typedef struct _AgsGstreamerClientClass AgsGstreamerClientClass;

/**
 * AgsGstreamerClientFlags:
 * @AGS_GSTREAMER_CLIENT_ACTIVATED: the client was activated
 * @AGS_GSTREAMER_CLIENT_READY: the client is ready
 * 
 * Enum values to control the behavior or indicate internal state of #AgsGstreamerClient by
 * enable/disable as flags.
 */
typedef enum{
  AGS_GSTREAMER_CLIENT_ACTIVATED          = 1 <<  2,
  AGS_GSTREAMER_CLIENT_READY              = 1 <<  3,
}AgsGstreamerClientFlags;

struct _AgsGstreamerClient
{
  GObject gobject;

  guint flags;
  guint connectable_flags;
  
  GRecMutex obj_mutex;
  
  GObject *gstreamer_server;

  AgsUUID *uuid;

#if defined(AGS_WITH_GSTREAMER)
  GstElement *pipeline;
#else
  gpointer pipeline;
#endif
  
  gchar *client_uuid;
  gchar *client_name;

  GList *device;
  GList *port;
};

struct _AgsGstreamerClientClass
{
  GObjectClass gobject;
};

GType ags_gstreamer_client_get_type();
GType ags_gstreamer_client_flags_get_type();

gboolean ags_gstreamer_client_test_flags(AgsGstreamerClient *gstreamer_client, guint flags);
void ags_gstreamer_client_set_flags(AgsGstreamerClient *gstreamer_client, guint flags);
void ags_gstreamer_client_unset_flags(AgsGstreamerClient *gstreamer_client, guint flags);

GList* ags_gstreamer_client_find_uuid(GList *gstreamer_client,
				      gchar *client_uuid);
GList* ags_gstreamer_client_find(GList *gstreamer_client,
				 gchar *client_name);

void ags_gstreamer_client_open(AgsGstreamerClient *gstreamer_client,
			       gchar *client_name);
void ags_gstreamer_client_close(AgsGstreamerClient *gstreamer_client);

void ags_gstreamer_client_add_device(AgsGstreamerClient *gstreamer_client,
				     GObject *gstreamer_device);
void ags_gstreamer_client_remove_device(AgsGstreamerClient *gstreamer_client,
					GObject *gstreamer_device);

void ags_gstreamer_client_add_port(AgsGstreamerClient *gstreamer_client,
				   GObject *gstreamer_port);
void ags_gstreamer_client_remove_port(AgsGstreamerClient *gstreamer_client,
				      GObject *gstreamer_port);

void ags_gstreamer_client_activate(AgsGstreamerClient *gstreamer_client);
void ags_gstreamer_client_deactivate(AgsGstreamerClient *gstreamer_client);

AgsGstreamerClient* ags_gstreamer_client_new(GObject *gstreamer_server);

G_END_DECLS

#endif /*__AGS_GSTREAMER_CLIENT_H__*/
