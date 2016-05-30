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

#ifndef __AGS_TURTLE_H__
#define __AGS_TURTLE_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#define AGS_TYPE_TURTLE                (ags_turtle_get_type())
#define AGS_TURTLE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TURTLE, AgsTurtle))
#define AGS_TURTLE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_TURTLE, AgsTurtleClass))
#define AGS_IS_TURTLE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TURTLE))
#define AGS_IS_TURTLE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TURTLE))
#define AGS_TURTLE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_TURTLE, AgsTurtleClass))

#define AGS_TURTLE_DEFAULT_ENCODING "UTF-8\0"

#define AGS_TURTLE_DEFAULT_VERSION "0.7.3\0"

typedef struct _AgsTurtle AgsTurtle;
typedef struct _AgsTurtleClass AgsTurtleClass;

typedef enum{
  AGS_TURTLE_READ_SUBJECT    = 1,
  AGS_TURTLE_READ_VERB       = 1 << 1,
  AGS_TURTLE_READ_OBJECT     = 1 << 2,
  AGS_TURTLE_TRIPLE_END      = 1 << 3,
}AgsTurtleFlags;

struct _AgsTurtle
{
  GObject object;

  guint flags;
  
  gchar *filename;
  
  xmlDoc *doc;
};

struct _AgsTurtleClass
{
  GObjectClass object;
};

GType ags_turtle_get_type(void);

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

/* literals */
gchar* ags_turtle_read_string(gchar *offset,
			      gchar *end_ptr);

gchar* ags_turtle_read_string_literal_quote(gchar *offset,
					    gchar *end_ptr);
gchar* ags_turtle_read_string_literal_single_quote(gchar *offset,
						   gchar *end_ptr);
gchar* ags_turtle_read_string_literal_long_quote(gchar *offset,
						 gchar *end_ptr);
gchar* ags_turtle_read_string_literal_long_single_quote(gchar *offset,
							gchar *end_ptr);

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

/* XML related */
GList* ags_turtle_find_xpath(AgsTurtle *turtle,
			     gchar *xpath);
GList* ags_turtle_find_xpath_with_context_node(AgsTurtle *turtle,
					       gchar *xpath,
					       xmlNode *context_node);

xmlDoc* ags_turtle_load(AgsTurtle *turtle,
			GError **error);

AgsTurtle* ags_turtle_new(gchar *filename);

#endif /*__AGS_TURTLE_H__*/
