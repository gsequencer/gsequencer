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

#ifndef __AGS_REMOTE_CHANNEL_H__
#define __AGS_REMOTE_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_audio_signal.h>

#define AGS_TYPE_REMOTE_CHANNEL                (ags_remote_channel_get_type())
#define AGS_REMOTE_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_REMOTE_CHANNEL, AgsRemoteChannel))
#define AGS_REMOTE_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_REMOTE_CHANNEL, AgsRemoteChannel))
#define AGS_IS_REMOTE_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_REMOTE_CHANNEL))
#define AGS_IS_REMOTE_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_REMOTE_CHANNEL))
#define AGS_REMOTE_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_REMOTE_CHANNEL, AgsRemoteChannelClass))

typedef struct _AgsRemoteChannel AgsRemoteChannel;
typedef struct _AgsRemoteChannelClass AgsRemoteChannelClass;

typedef enum{
  AGS_REMOTE_CHANNEL_KEEP_ALIVE         =  1,
}AgsRemoteChannelFlags;

typedef enum{
  AGS_REMOTE_CHANNEL_RPC                =  1,
  AGS_REMOTE_CHANNEL_XMLRPC             =  1 <<  1,
  AGS_REMOTE_CHANNEL_REST_HTTP          =  1 <<  2,
}AgsRemoteChannelProtocolFlags;

typedef enum{
  AGS_REMOTE_CHANNEL_SERVER_TIMEOUT,
  AGS_REMOTE_CHANNEL_SERVER_DIED,
}AgsRemoteChannelError;

struct _AgsRemoteChannel
{
  GObject object;

  guint flags;
  guint protocol_flags;

  gchar *data_encoding;
  gchar *data_compression;
  
  gchar *character_encoding;
  
  gchar *uuid;
  gchar *uri;

  GObject *server;
  GObject *credentials;

  GObject *client;
  GList *data_package;

  GList *audio_signal;
};

struct _AgsRemoteChannelClass
{
  GObjectClass object;

  GObject* (*connect)(AgsRemoteChannel *remote_channel);

  guint (*transfer_data)(AgsRemoteChannel *remote_channel, gchar *uuid,
			 char *data, guint buffer_length,
			 GError *error);
  guint (*recieve_data)(AgsRemoteChannel *remote_channel, gchar *uuid,
			char *data, guint buffer_length,
			GError *error);
  
  void (*put_package)(AgsRemoteChannel *remote_channel,
		      GObject *data_package);
  void (*get_package)(AgsRemoteChannel *remote_channel,
		      GObject *data_package);

  void (*delete_package)(AgsRemoteChannel *remote_channel,
			 GObject *data_package);
  void (*post_package)(AgsRemoteChannel *remote_channel,
		       GObject *data_package);

  void (*add_audio_signal)(AgsRemoteChannel *remote_channel,
			   AgsAudioSignal *audio_signal);
  void (*remove_audio_signal)(AgsRemoteChannel *remote_channel,
			      AgsAudioSignal *audio_signal);
};

GType ags_remote_channel_get_type();

GQuark ags_remote_channel_error_quark();

AgsRemoteChannel* ags_remote_channel_new(GObject *application_context);

#endif /*__AGS_REMOTE_CHANNEL_H__*/
