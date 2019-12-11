/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_MESSAGE_ENVELOPE_H__
#define __AGS_MESSAGE_ENVELOPE_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

G_BEGIN_DECLS

#define AGS_TYPE_MESSAGE_ENVELOPE                (ags_message_envelope_get_type())
#define AGS_MESSAGE_ENVELOPE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MESSAGE_ENVELOPE, AgsMessageEnvelope))
#define AGS_MESSAGE_ENVELOPE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_MESSAGE_ENVELOPE, AgsMessageEnvelope))
#define AGS_IS_MESSAGE_ENVELOPE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MESSAGE_ENVELOPE))
#define AGS_IS_MESSAGE_ENVELOPE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MESSAGE_ENVELOPE))
#define AGS_MESSAGE_ENVELOPE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_MESSAGE_ENVELOPE, AgsMessageEnvelopeClass))

#define AGS_MESSAGE_ENVELOPE_GET_OBJ_MUTEX(obj) (&(((AgsMessageEnvelope *) obj)->obj_mutex))

typedef struct _AgsMessageEnvelope AgsMessageEnvelope;
typedef struct _AgsMessageEnvelopeClass AgsMessageEnvelopeClass;

struct _AgsMessageEnvelope
{
  GObject gobject;
  
  GRecMutex obj_mutex;

  GObject *sender;
  GObject *recipient;

  xmlDoc *doc;
  
  guint n_params;

  gchar **parameter_name;
  GValue *value;
};

struct _AgsMessageEnvelopeClass
{
  GObjectClass gobject; 
};

GType ags_message_envelope_get_type();

GObject* ags_message_envelope_get_sender(AgsMessageEnvelope *message_envelope);
GObject* ags_message_envelope_get_recipient(AgsMessageEnvelope *message_envelope);

xmlDoc* ags_message_envelope_get_doc(AgsMessageEnvelope *message_envelope);

void ags_message_envelope_get_parameter(AgsMessageEnvelope *message_envelope,
					guint *n_params,
					gchar ***parameter_name, GValue **value);

AgsMessageEnvelope* ags_message_envelope_new();

G_END_DECLS

#endif /*__AGS_MESSAGE_ENVELOPE_H__*/
