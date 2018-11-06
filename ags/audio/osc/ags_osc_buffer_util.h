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

#ifndef __AGS_OSC_BUFFER_UTIL_H__
#define __AGS_OSC_BUFFER_UTIL_H__

#include <glib.h>
#include <glib-object.h>

void ags_osc_buffer_util_put_int32(unsigned char *buffer,
				   gint32 val);
void ags_osc_buffer_util_get_int32(unsigned char *buffer,
				   gint32 *val);

void ags_osc_buffer_util_put_timetag(unsigned char *buffer,
				     gint32 tv_secs, gint32 tv_fraction, gboolean immediately);
void ags_osc_buffer_util_get_timetag(unsigned char *buffer,
				     gint32 *tv_secs, gint32 *tv_fraction, gboolean *immediately);

void ags_osc_buffer_util_put_float(unsigned char *buffer,
				   gfloat val);
void ags_osc_buffer_util_get_float(unsigned char *buffer,
				   gfloat *val);

void ags_osc_buffer_util_put_string(unsigned char *buffer,
				    gchar *str, gsize length);
void ags_osc_buffer_util_get_string(unsigned char *buffer,
				    gchar **str, gsize *length);

void ags_osc_buffer_util_put_blob(unsigned char *buffer,
				  gint32 data_size, unsigned char *data);
void ags_osc_buffer_util_get_blob(unsigned char *buffer,
				  gint32 *data_size, unsigned char **data);

void ags_osc_buffer_util_put_packet(unsigned char *buffer,
				    gint32 packet_size, unsigned char *packet);
void ags_osc_buffer_util_get_packet(unsigned char *buffer,
				    gint32 *packet_size, unsigned char **packet);

void ags_osc_buffer_util_put_packets(unsigned char *buffer,
				     gint32 packet_size, va_list var_args);
guint ags_osc_buffer_util_check_packet_count(unsigned char *buffer);
void ags_osc_buffer_util_get_packets(unsigned char *buffer,
				     gint32 *packet_size, va_list var_args);

void ags_osc_buffer_util_put_message(unsigned char *buffer,
				     gchar *address_pattern, gchar *type_tag, va_list var_args);
guint ags_osc_buffer_util_check_message_value_count(unsigned char *buffer);
void ags_osc_buffer_util_get_message(unsigned char *buffer,
				     gchar **address_pattern, gchar **type_tag, va_list var_args);

void ags_osc_buffer_util_put_bundle(unsigned char *buffer,
				    gint32 tv_secs, gint32 tv_fraction, gboolean immediately,
				    gint32 content_size, va_list var_args);
guint ags_osc_buffer_util_check_bundle_content_count(unsigned char *buffer);
void ags_osc_buffer_util_get_bundle(unsigned char *buffer,
				    gint32 *tv_secs, gint32 *tv_fraction, gboolean *immediately,
				    gint32 content_size, va_list var_args);

#endif /*__AGS_OSC_BUFFER_UTIL_H__*/
