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

#ifndef __AGS_FILE_H__
#define __AGS_FILE_H__

#include <glib.h>
#include <glib-object.h>

#include <gio/gio.h>

#include <libxml/tree.h>

G_BEGIN_DECLS

#define AGS_TYPE_FILE                (ags_file_get_type())
#define AGS_TYPE_FILE_FLAGS          (ags_file_flags_get_type())
#define AGS_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FILE, AgsFile))
#define AGS_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FILE, AgsFileClass))
#define AGS_IS_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FILE))
#define AGS_IS_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FILE))
#define AGS_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FILE, AgsFileClass))

#define AGS_FILE_GET_OBJ_MUTEX(obj) (&(((AgsFile *) obj)->obj_mutex))

#define AGS_FILE_DEFAULT_APP_ENCODING "C.UTF-8"

#define AGS_FILE_DEFAULT_ENCODING "UTF-8"
#define AGS_FILE_DEFAULT_DTD "ags_file.dtd"

#define AGS_FILE_CHECKSUM_LENGTH (32)

#define AGS_FILE_DEFAULT_AUDIO_FORMAT "raw"
#define AGS_FILE_DEFAULT_AUDIO_ENCODING "base64"

#define AGS_FILE_CHARSET_CONVERTER_MAX_STRING_LENGTH (8192)
#define AGS_FILE_CHARSET_CONVERTER_MAX_CONTENT_LENGTH (8388608) // max 8 MB

typedef struct _AgsFile AgsFile;
typedef struct _AgsFileClass AgsFileClass;

/**
 * AgsFileFlags:
 * @AGS_FILE_READ: opened file for reading
 * @AGS_FILE_READ_AUDIO_SIGNAL: read audio signal
 * @AGS_FILE_READ_EMBEDDED_AUDIO: read embedded audio data
 * @AGS_FILE_WRITE: opened file for writing
 * @AGS_FILE_WRITE_AUDIO_SIGNAL: write audio signal
 * @AGS_FILE_WRITE_EMBEDDED_AUDIO: write embedded audio data
 * 
 * Enum values to control the behavior or indicate internal state of #AgsFile by
 * enable/disable as flags.
 */
typedef enum{
  AGS_FILE_READ                    = 1,
  AGS_FILE_READ_AUDIO_SIGNAL       = 1 << 1,
  AGS_FILE_READ_EMBEDDED_AUDIO     = 1 << 2,
  AGS_FILE_WRITE                   = 1 << 3,
  AGS_FILE_WRITE_AUDIO_SIGNAL      = 1 << 4,
  AGS_FILE_WRITE_EMBEDDED_AUDIO    = 1 << 5,
}AgsFileFlags;

#define AGS_FILE_ERROR (ags_file_error_quark())

/**
 * AgsFileError:
 * @AGS_FILE_ERROR_PARSER_FAILURE: the parser failed
 * 
 * Enum values to indicated failures to used with #GError-struct.
 */
typedef enum{
  AGS_FILE_ERROR_PARSER_FAILURE,
}AgsFileError;

struct _AgsFile
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;

  gchar *app_encoding;
  
  FILE *out;
  xmlChar *buffer;

  gchar *filename;
  gchar *encoding;
  gchar *dtd;

  gchar *audio_format;
  gchar *audio_encoding;

  GCharsetConverter *read_charset_converter;
  GCharsetConverter *write_charset_converter;
  
  xmlDoc *doc;
  xmlNode *root_node;

  GList *id_refs;
  GList *lookup;
  GList *launch;

  GObject *clipboard;
  GList *property;
  GList *script;
  GObject *cluster;
  GObject *client;
  GObject *server;

  GObject *history;

  GList *embedded_audio;
  GList *file_link;
};

struct _AgsFileClass
{
  GObjectClass gobject;

  void (*open)(AgsFile *file,
	       GError **error);
  void (*open_from_data)(AgsFile *file,
			 gchar *data, guint length,
			 GError **error);
  void (*rw_open)(AgsFile *file,
		  gboolean create,
		  GError **error);

  void (*write)(AgsFile *file);
  void (*write_concurrent)(AgsFile *file);
  void (*write_resolve)(AgsFile *file);

  void (*read)(AgsFile *file);
  void (*read_resolve)(AgsFile *file);
  void (*read_start)(AgsFile *file);
};

GType ags_file_get_type(void);
GType ags_file_flags_get_type();

void ags_file_set_filename(AgsFile *file,
			   gchar *filename);
gchar* ags_file_get_filename(AgsFile *file);

void ags_file_set_encoding(AgsFile *file,
			   gchar *encoding);
gchar* ags_file_get_encoding(AgsFile *file);

void ags_file_set_audio_format(AgsFile *file,
			       gchar *audio_format);
gchar* ags_file_get_audio_format(AgsFile *file);

void ags_file_set_audio_encoding(AgsFile *file,
				 gchar *audio_encoding);
gchar* ags_file_get_audio_encoding(AgsFile *file);

void ags_file_set_xml_doc(AgsFile *file,
			  xmlDoc *xml_doc);
xmlDoc* ags_file_get_xml_doc(AgsFile *file);

gchar* ags_file_str2md5(gchar *content, guint content_length);

void ags_file_add_id_ref(AgsFile *file, GObject *id_ref);

GObject* ags_file_find_id_ref_by_node(AgsFile *file, xmlNode *node);
GObject* ags_file_find_id_ref_by_xpath(AgsFile *file, gchar *xpath);
GObject* ags_file_find_id_ref_by_reference(AgsFile *file, gpointer ref);

void ags_file_add_lookup(AgsFile *file, GObject *file_lookup);

void ags_file_add_launch(AgsFile *file, GObject *file_launch);

/* tree manipulation */
xmlDoc* ags_file_xml_new_doc(AgsFile *file,
			     gchar *version);

xmlNode* ags_file_xml_new_node(AgsFile *file,
			       xmlNs *xml_namespace,
			       gchar *node_name);
xmlNode* ags_file_xml_get_root_element(AgsFile *file,
				       xmlDoc *doc);
void ags_file_xml_set_root_element(AgsFile *file,
				   xmlDoc *doc,
				   xmlNode *root_node);
void ags_file_xml_add_child(AgsFile *file,
			    xmlNode *parent,
			    xmlNode *child);

gchar* ags_file_xml_get_node_name(AgsFile *file,
				  xmlNode *node);

xmlNode* ags_file_xml_get_node_parent(AgsFile *file,
				      xmlNode *node);
xmlNode* ags_file_xml_get_node_next(AgsFile *file,
				    xmlNode *node);
xmlNode* ags_file_xml_get_node_children(AgsFile *file,
					xmlNode *node);

gchar* ags_file_xml_get_prop(AgsFile *file,
			     xmlNode *node,
			     gchar *prop_name);
void ags_file_xml_set_prop(AgsFile *file,
			   xmlNode *node,
			   gchar *prop_name,
			   gchar *prop_value);

gchar* ags_file_xml_get_content(AgsFile *file,
				xmlNode *node);
void ags_file_xml_set_content(AgsFile *file,
			      xmlNode *node,
			      gchar *content,
			      gboolean is_cdata);

/*  */
void ags_file_open(AgsFile *file,
		   GError **error);
void ags_file_open_from_data(AgsFile *file,
			     gchar *data, guint length,
			     GError **error);
void ags_file_rw_open(AgsFile *file,
		      gboolean create,
		      GError **error);

void ags_file_open_filename(AgsFile *file,
			    gchar *filename);
void ags_file_close(AgsFile *file);

/*  */
void ags_file_write(AgsFile *file);
void ags_file_write_concurrent(AgsFile *file);
void ags_file_write_resolve(AgsFile *file);

void ags_file_read(AgsFile *file);
void ags_file_read_resolve(AgsFile *file);
void ags_file_read_start(AgsFile *file);

void ags_file_read_config(AgsFile *file, xmlNode *node, GObject **ags_config);
void ags_file_write_config(AgsFile *file, xmlNode *parent, GObject *ags_config);

/*  */
void ags_file_read_application_context(AgsFile *file, xmlNode *node, GObject **application_context);
void ags_file_write_application_context(AgsFile *file, xmlNode *parent, GObject *application_context);

/* */
AgsFile* ags_file_new();

G_END_DECLS

#endif /*__AGS_FILE_H__*/
