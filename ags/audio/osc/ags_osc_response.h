/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#ifndef __AGS_OSC_RESPONSE_H__
#define __AGS_OSC_RESPONSE_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_OSC_RESPONSE                (ags_osc_response_get_type ())
#define AGS_OSC_RESPONSE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_RESPONSE, AgsOscResponse))
#define AGS_OSC_RESPONSE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OSC_RESPONSE, AgsOscResponseClass))
#define AGS_IS_OSC_RESPONSE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSC_RESPONSE))
#define AGS_IS_OSC_RESPONSE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSC_RESPONSE))
#define AGS_OSC_RESPONSE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_OSC_RESPONSE, AgsOscResponseClass))

#define AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE "server failure"
#define AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST "malformed request"
#define AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_PATH "unknow path"
#define AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_ARGUMENT "unknown argument"
#define AGS_OSC_RESPONSE_ERROR_MESSAGE_MISSING_INDEX "missing index"
#define AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED "chunk size exceeded"

#define AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE (131072)

typedef struct _AgsOscResponse AgsOscResponse;
typedef struct _AgsOscResponseClass AgsOscResponseClass;

typedef enum{
  AGS_OSC_RESPONSE_EMPTY     = 1,
  AGS_OSC_RESPONSE_ERROR     = 1 <<  1,
}AgsOscResponseFlags;

struct _AgsOscResponse
{
  GObject gobject;

  guint flags;

  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;

  guchar *packet;
  guint packet_size;

  gchar *error_message;
};

struct _AgsOscResponseClass
{
  GObjectClass gobject;
};

GType ags_osc_response_get_type(void);

pthread_mutex_t* ags_osc_response_get_class_mutex();

gboolean ags_osc_response_test_flags(AgsOscResponse *osc_response, guint flags);
void ags_osc_response_set_flags(AgsOscResponse *osc_response, guint flags);
void ags_osc_response_unset_flags(AgsOscResponse *osc_response, guint flags);

AgsOscResponse* ags_osc_response_new();

#endif /*__AGS_OSC_RESPONSE_H__*/
