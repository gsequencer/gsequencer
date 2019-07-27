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

#ifndef __AGS_OSC_BUILDER_H__
#define __AGS_OSC_BUILDER_H__

#include <glib.h>
#include <glib-object.h>
#include <gmodule.h>

#define AGS_TYPE_OSC_BUILDER                (ags_osc_builder_get_type ())
#define AGS_OSC_BUILDER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_BUILDER, AgsOscBuilder))
#define AGS_OSC_BUILDER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OSC_BUILDER, AgsOscBuilderClass))
#define AGS_IS_OSC_BUILDER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSC_BUILDER))
#define AGS_IS_OSC_BUILDER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSC_BUILDER))
#define AGS_OSC_BUILDER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_OSC_BUILDER, AgsOscBuilderClass))

#define AGS_OSC_BUILDER_GET_OBJ_MUTEX(obj) (((AgsOscBuilder *) obj)->obj_mutex)

#define AGS_OSC_BUILDER_PACKET(x) ((AgsOscBuilderPacket *)(x))
#define AGS_OSC_BUILDER_BUNDLE(x) ((AgsOscBuilderBundle *)(x))
#define AGS_OSC_BUILDER_MESSAGE(x) ((AgsOscBuilderMessage *)(x))

#define AGS_OSC_BUILDER_DEFAULT_CHUNK_SIZE (8192)
#define AGS_OSC_BUILDER_MESSAGE_DEFAULT_CHUNK_SIZE (8192)

typedef struct _AgsOscBuilder AgsOscBuilder;
typedef struct _AgsOscBuilderClass AgsOscBuilderClass;
typedef struct _AgsOscBuilderPacket AgsOscBuilderPacket;
typedef struct _AgsOscBuilderBundle AgsOscBuilderBundle;
typedef struct _AgsOscBuilderMessage AgsOscBuilderMessage;

typedef enum{
  AGS_OSC_BUILDER_ADD_VALUE            = 1,
}AgsOscBuilderFlags;

struct _AgsOscBuilder
{
  GObject gobject;

  guint flags;

  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;

  unsigned char *data;
  guint length;

  guint64 offset;

  GList *packet;

  gchar *current_type_tag;
  gchar *offset_type_tag;
};

struct _AgsOscBuilderClass
{
  GObjectClass gobject;
  
  void (*osc_putc)(AgsOscBuilder *osc_builder,
		   gint c);
  void (*on_error)(AgsOscBuilder *osc_builder,
		   GError **error);

  /* */
  void (*append_packet)(AgsOscBuilder *osc_builder);

  void (*append_bundle)(AgsOscBuilder *osc_builder,
			AgsOscBuilderBundle *parent_bundle,
			gint tv_secs, gint tv_fraction, gboolean immediately);

  void (*append_message)(AgsOscBuilder *osc_builder,
			 AgsOscBuilderBundle *parent_bundle,
			 gchar *address_pattern,
			 gchar *type_tag);

  void (*append_value)(AgsOscBuilder *osc_builder,
		       AgsOscBuilderMessage *message,
		       gint v_type,
		       GValue *value);
};

struct _AgsOscBuilderPacket
{
  guint64 offset;

  gsize packet_size;
  
  AgsOscBuilder *builder;
  
  GList *message;
  GList *bundle;
};

struct _AgsOscBuilderBundle
{
  guint64 offset;

  gsize bundle_size;
  
  gint32 tv_secs;
  gint32 tv_fraction;
  gboolean immediately;

  AgsOscBuilderPacket *packet;
  AgsOscBuilderBundle *parent_bundle;

  GList *message;
  GList *bundle;
};

struct _AgsOscBuilderMessage
{
  guint64 offset;
  
  gchar *address_pattern;
  gchar *type_tag;

  gsize data_allocated_length;
  gsize data_length;
  unsigned char *data;

  AgsOscBuilderPacket *packet;
  AgsOscBuilderBundle *parent_bundle;
};

GType ags_osc_builder_get_type(void);

pthread_mutex_t* ags_osc_builder_get_class_mutex();

AgsOscBuilderPacket* ags_osc_builder_packet_alloc(guint64 offset);
void ags_osc_builder_packet_free(AgsOscBuilderPacket *packet);

AgsOscBuilderBundle* ags_osc_builder_bundle_alloc(guint64 offset);
void ags_osc_builder_bundle_free(AgsOscBuilderBundle *bundle);

AgsOscBuilderMessage* ags_osc_builder_message_alloc(guint64 offset);
void ags_osc_builder_message_free(AgsOscBuilderMessage *message);

/*  */
void ags_osc_builder_osc_putc(AgsOscBuilder *osc_builder,
			      gint c);
void ags_osc_builder_on_error(AgsOscBuilder *osc_builder,
			      GError **error);

/* */
void ags_osc_builder_append_packet(AgsOscBuilder *osc_builder);

void ags_osc_builder_append_bundle(AgsOscBuilder *osc_builder,
				   AgsOscBuilderBundle *parent_bundle,
				   gint tv_secs, gint tv_fraction, gboolean immediately);

void ags_osc_builder_append_message(AgsOscBuilder *osc_builder,
				    AgsOscBuilderBundle *parent_bundle,
				    gchar *address_pattern,
				    gchar *type_tag);

void ags_osc_builder_append_value(AgsOscBuilder *osc_builder,
				  AgsOscBuilderMessage *message,
				  gint v_type,
				  GValue *value);

/*  */
void ags_osc_builder_build(AgsOscBuilder *osc_builder);

/*  */
AgsOscBuilder* ags_osc_builder_new();

#endif /*__AGS_OSC_BUILDER_H__*/
