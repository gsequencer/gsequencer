/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#ifndef __AGS_TURTLE_H__
#define __AGS_TURTLE_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

G_BEGIN_DECLS

#define AGS_TYPE_TURTLE                (ags_turtle_get_type())
#define AGS_TYPE_TURTLE_FLAGS          (ags_turtle_flags_get_type())
#define AGS_TURTLE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TURTLE, AgsTurtle))
#define AGS_TURTLE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_TURTLE, AgsTurtleClass))
#define AGS_IS_TURTLE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TURTLE))
#define AGS_IS_TURTLE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TURTLE))
#define AGS_TURTLE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_TURTLE, AgsTurtleClass))

#define AGS_TURTLE_GET_OBJ_MUTEX(obj) (&(((AgsTurtle *) obj)->obj_mutex))

#define AGS_TURTLE_DEFAULT_ENCODING "en_GB.UTF-8"

#define AGS_TURTLE_DEFAULT_VERSION "6.3.2"

#define AGS_TURTLE_BOOLEAN_LITERAL_TRUE "true"
#define AGS_TURTLE_BOOLEAN_LITERAL_FALSE "false"

typedef struct _AgsTurtle AgsTurtle;
typedef struct _AgsTurtleClass AgsTurtleClass;

/**
 * AgsTurtleFlags:
 * @AGS_TURTLE_TOLOWER: make all tags and attributes lower case
 * 
 * Enum values to control the behavior or indicate internal state of #AgsTurtle by
 * enable/disable as sync_flags.
 */
typedef enum{
  AGS_TURTLE_TOLOWER    = 1,
}AgsTurtleFlags;

struct _AgsTurtle
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;
  
  gchar *filename;
  
  xmlDoc *doc;

  GHashTable *prefix_id;
};

struct _AgsTurtleClass
{
  GObjectClass gobject;
};

GType ags_turtle_get_type(void);
GType ags_turtle_flags_get_type();

/* iri, pname, label and langtag */
gchar* ags_turtle_read_iriref(gchar *offset,
			      gchar *end_ptr);
gchar* ags_turtle_read_pname_ns(gchar *offset,
				gchar *end_ptr);
gchar* ags_turtle_read_pname_ln(gchar *offset,
				gchar *end_ptr);
gchar* ags_turtle_read_blank_node_label(gchar *offset,
					gchar *end_ptr);
gchar* ags_turtle_read_langtag(gchar *offset,
			       gchar *end_ptr);

gboolean ags_turtle_match_iriref(gchar *offset,
				 gchar *end_ptr,
				 gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_pname_ns(gchar *offset,
				   gchar *end_ptr,
				   gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_pname_ln(gchar *offset,
				   gchar *end_ptr,
				   gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_blank_node_label(gchar *offset,
					   gchar *end_ptr,
					   gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_langtag(gchar *offset,
				  gchar *end_ptr,
				  gchar **start_offset, gchar **end_offset);

/* numbers */
gchar* ags_turtle_read_boolean(gchar *offset,
			       gchar *end_ptr);
gchar* ags_turtle_read_integer(gchar *offset,
			       gchar *end_ptr);
gchar* ags_turtle_read_decimal(gchar *offset,
			       gchar *end_ptr);
gchar* ags_turtle_read_double(gchar *offset,
			      gchar *end_ptr);
gchar* ags_turtle_read_exponent(gchar *offset,
				gchar *end_ptr);

gboolean ags_turtle_match_boolean(gchar *offset,
				  gchar *end_ptr,
				  gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_integer(gchar *offset,
				  gchar *end_ptr,
				  gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_decimal(gchar *offset,
				  gchar *end_ptr,
				  gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_double(gchar *offset,
				 gchar *end_ptr,
				 gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_exponent(gchar *offset,
				   gchar *end_ptr,
				   gchar **start_offset, gchar **end_offset);

/* literals */
gchar* ags_turtle_read_string(gchar *offset,
			      gchar *end_ptr);

gboolean ags_turtle_match_string(gchar *offset,
				 gchar *end_ptr,
				 gchar **start_offset, gchar **end_offset);

gchar* ags_turtle_read_string_literal_quote(gchar *offset,
					    gchar *end_ptr);
gchar* ags_turtle_read_string_literal_single_quote(gchar *offset,
						   gchar *end_ptr);
gchar* ags_turtle_read_string_literal_long_quote(gchar *offset,
						 gchar *end_ptr);
gchar* ags_turtle_read_string_literal_long_single_quote(gchar *offset,
							gchar *end_ptr);

gboolean ags_turtle_match_string_literal_quote(gchar *offset,
					       gchar *end_ptr,
					       gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_string_literal_single_quote(gchar *offset,
						      gchar *end_ptr,
						      gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_string_literal_long_quote(gchar *offset,
						    gchar *end_ptr,
						    gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_string_literal_long_single_quote(gchar *offset,
							   gchar *end_ptr,
							   gchar **start_offset, gchar **end_offset);

/* character ranges might return multi-byte */
gchar* ags_turtle_read_uchar(gchar *offset,
			     gchar *end_ptr);
gchar* ags_turtle_read_echar(gchar *offset,
			     gchar *end_ptr);
gchar* ags_turtle_read_ws(gchar *offset,
			  gchar *end_ptr);
gchar* ags_turtle_read_anon(gchar *offset,
			    gchar *end_ptr);
gchar* ags_turtle_read_pn_chars_base(gchar *offset,
				     gchar *end_ptr);
gchar* ags_turtle_read_pn_chars_u(gchar *offset,
				  gchar *end_ptr);
gchar* ags_turtle_read_pn_chars(gchar *offset,
				gchar *end_ptr);
gchar* ags_turtle_read_pn_prefix(gchar *offset,
				 gchar *end_ptr);
gchar* ags_turtle_read_pn_local(gchar *offset,
				gchar *end_ptr);
gchar* ags_turtle_read_plx(gchar *offset,
			   gchar *end_ptr);
gchar* ags_turtle_read_percent(gchar *offset,
			       gchar *end_ptr);
gchar* ags_turtle_read_hex(gchar *offset,
			   gchar *end_ptr);
gchar* ags_turtle_read_pn_local_esc(gchar *offset,
				    gchar *end_ptr);

gboolean ags_turtle_match_uchar(gchar *offset,
				gchar *end_ptr,
				gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_echar(gchar *offset,
				gchar *end_ptr,
				gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_ws(gchar *offset,
			     gchar *end_ptr,
			     gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_anon(gchar *offset,
			       gchar *end_ptr,
			       gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_pn_chars_base(gchar *offset,
					gchar *end_ptr,
					gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_pn_chars_u(gchar *offset,
				     gchar *end_ptr,
				     gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_pn_chars(gchar *offset,
				   gchar *end_ptr,
				   gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_pn_prefix(gchar *offset,
				    gchar *end_ptr,
				    gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_pn_local(gchar *offset,
				   gchar *end_ptr,
				   gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_plx(gchar *offset,
			      gchar *end_ptr,
			      gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_percent(gchar *offset,
				  gchar *end_ptr,
				  gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_hex(gchar *offset,
			      gchar *end_ptr,
			      gchar **start_offset, gchar **end_offset);
gboolean ags_turtle_match_pn_local_esc(gchar *offset,
				       gchar *end_ptr,
				       gchar **start_offset, gchar **end_offset);

/* XML related */
GList* ags_turtle_find_xpath(AgsTurtle *turtle,
			     gchar *xpath);
GList* ags_turtle_find_xpath_with_context_node(AgsTurtle *turtle,
					       gchar *xpath,
					       xmlNode *context_node);

gchar* ags_turtle_string_convert(AgsTurtle *turtle, gchar *str);

xmlDoc* ags_turtle_load(AgsTurtle *turtle,
			GError **error);

AgsTurtle* ags_turtle_new(gchar *filename);

G_END_DECLS

#endif /*__AGS_TURTLE_H__*/
