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

#ifndef __AGS_OSC_BUFFER_UTIL_H__
#define __AGS_OSC_BUFFER_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_OSC_BUFFER_UTIL         (ags_osc_buffer_util_get_type())

typedef struct _AgsOscBufferUtil AgsOscBufferUtil;

struct _AgsOscBufferUtil
{
  //empty
};

GType ags_osc_buffer_util_get_type(void);

void ags_osc_buffer_util_put_int32(guchar *buffer,
				   gint32 val);
void ags_osc_buffer_util_get_int32(guchar *buffer,
				   gint32 *val);

void ags_osc_buffer_util_put_timetag(guchar *buffer,
				     gint32 tv_secs, gint32 tv_fraction, gboolean immediately);
void ags_osc_buffer_util_get_timetag(guchar *buffer,
				     gint32 *tv_secs, gint32 *tv_fraction, gboolean *immediately);

void ags_osc_buffer_util_put_float(guchar *buffer,
				   gfloat val);
void ags_osc_buffer_util_get_float(guchar *buffer,
				   gfloat *val);

void ags_osc_buffer_util_put_string(guchar *buffer,
				    gchar *str, gsize length);
void ags_osc_buffer_util_get_string(guchar *buffer,
				    gchar **str, gsize *length);

void ags_osc_buffer_util_put_blob(guchar *buffer,
				  gint32 data_size, guchar *data);
void ags_osc_buffer_util_get_blob(guchar *buffer,
				  gint32 *data_size, guchar **data);

void ags_osc_buffer_util_put_int64(guchar *buffer,
				   gint64 val);
void ags_osc_buffer_util_get_int64(guchar *buffer,
				   gint64 *val);

void ags_osc_buffer_util_put_double(guchar *buffer,
				    gdouble val);
void ags_osc_buffer_util_get_double(guchar *buffer,
				    gdouble *val);

void ags_osc_buffer_util_put_char(guchar *buffer,
				  gchar val);
void ags_osc_buffer_util_get_char(guchar *buffer,
				  gchar *val);

void ags_osc_buffer_util_put_rgba(guchar *buffer,
				  guint8 r, guint8 g, guint8 b, guint8 a);
void ags_osc_buffer_util_get_rgba(guchar *buffer,
				  guint8 *r, guint8 *g, guint8 *b, guint8 *a);

void ags_osc_buffer_util_put_midi(guchar *buffer,
				  guint8 port, guint8 status_byte, guint8 data0, guint8 data1);
void ags_osc_buffer_util_get_midi(guchar *buffer,
				  guint8 *port, guint8 *status_byte, guint8 *data0, guint8 *data1);

void ags_osc_buffer_util_put_packet(guchar *buffer,
				    gint32 packet_size, guchar *packet);
void ags_osc_buffer_util_get_packet(guchar *buffer,
				    gint32 *packet_size, guchar **packet);

void ags_osc_buffer_util_put_packets(guchar *buffer,
				     gint32 packet_size, ...);

void ags_osc_buffer_util_put_message(guchar *buffer,
				     gchar *address_pattern, gchar *type_tag);
void ags_osc_buffer_util_get_message(guchar *buffer,
				     gchar **address_pattern, gchar **type_tag);

void ags_osc_buffer_util_put_bundle(guchar *buffer,
				    gint32 tv_secs, gint32 tv_fraction, gboolean immediately);
void ags_osc_buffer_util_get_bundle(guchar *buffer,
				    gint32 *tv_secs, gint32 *tv_fraction, gboolean *immediately);

G_END_DECLS

#endif /*__AGS_OSC_BUFFER_UTIL_H__*/
