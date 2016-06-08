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

#ifndef __AGS_SIMPLE_FILE_H__
#define __AGS_SIMPLE_FILE_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#define AGS_TYPE_SIMPLE_FILE                (ags_simple_file_get_type())
#define AGS_SIMPLE_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SIMPLE_FILE, AgsSimpleFile))
#define AGS_SIMPLE_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SIMPLE_FILE, AgsSimpleFileClass))
#define AGS_IS_SIMPLE_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SIMPLE_FILE))
#define AGS_IS_SIMPLE_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SIMPLE_FILE))
#define AGS_SIMPLE_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_SIMPLE_FILE, AgsSimpleFileClass))

#define AGS_SIMPLE_FILE_DEFAULT_ENCODING "UTF-8\0"
#define AGS_SIMPLE_FILE_DEFAULT_DTD "ags_simple_file.dtd\0"

#define AGS_SIMPLE_FILE_CHECKSUM_LENGTH (g_checksum_type_get_length(G_CHECKSUM_MD5))

#define AGS_SIMPLE_FILE_DEFAULT_AUDIO_FORMAT "raw\0"
#define AGS_SIMPLE_FILE_DEFAULT_AUDIO_ENCODING "base64\0"

typedef struct _AgsSimpleFile AgsSimpleFile;
typedef struct _AgsSimpleFileClass AgsSimpleFileClass;

typedef enum{
  AGS_SIMPLE_FILE_READ                    = 1,
  AGS_SIMPLE_FILE_READ_AUDIO_SIGNAL       = 1 << 1,
  AGS_SIMPLE_FILE_READ_EMBEDDED_AUDIO     = 1 << 2,
  AGS_SIMPLE_FILE_WRITE                   = 1 << 3,
  AGS_SIMPLE_FILE_WRITE_AUDIO_SIGNAL      = 1 << 4,
  AGS_SIMPLE_FILE_WRITE_EMBEDDED_AUDIO    = 1 << 5,
}AgsSimpleFileFlags;

#define AGS_SIMPLE_FILE_ERROR (ags_simple_file_error_quark())

typedef enum{
  AGS_SIMPLE_FILE_ERROR_PARSER_FAILURE,
}AgsSimpleFileError;

struct _AgsSimpleFile
{
  GObject object;

  guint flags;

  FILE *out;
  xmlChar *buffer;

  gchar *filename;
  gchar *encoding;
  gchar *dtd;

  xmlDoc *doc;
  xmlNode *root_node;

  GList *id_ref;
  GList *lookup;
  GList *launch;

  GObject *application_context;
};

struct _AgsSimpleFileClass
{
  GObjectClass object;

  void (*open)(AgsSimpleFile *simple_file);
  void (*open_from_data)(AgsSimpleFile *simple_file,
			 gchar *data, guint length);
  void (*rw_open)(AgsSimpleFile *simple_file,
		  gboolean create);

  void (*write)(AgsSimpleFile *simple_file);
  void (*write_resolve)(AgsSimpleFile *simple_file);

  void (*read)(AgsSimpleFile *simple_file);
  void (*read_resolve)(AgsSimpleFile *simple_file);
  void (*read_start)(AgsSimpleFile *simple_file);
};

GType ags_simple_file_get_type(void);

gchar* ags_simple_file_str2md5(gchar *content, guint content_length);

void ags_simple_file_add_id_ref(AgsSimpleFile *simple_file, GObject *id_ref);

GObject* ags_simple_file_find_id_ref_by_node(AgsSimpleFile *simple_file, xmlNode *node);
GList* ags_simple_file_find_id_ref_by_xpath(AgsSimpleFile *simple_file, gchar *xpath);
GList* ags_simple_file_find_id_ref_by_reference(AgsSimpleFile *simple_file, gpointer ref);

void ags_simple_file_add_lookup(AgsSimpleFile *simple_file, GObject *file_lookup);

void ags_simple_file_add_launch(AgsSimpleFile *simple_file, GObject *file_launch);

/*  */
void ags_simple_file_open(AgsSimpleFile *simple_file,
			  GError **error);
void ags_simple_file_open_from_data(AgsSimpleFile *simple_file,
				    gchar *data, guint length,
				    GError **error);
void ags_simple_file_rw_open(AgsSimpleFile *simple_file,
			     gboolean create,
			     GError **error);

void ags_simple_file_open_filename(AgsSimpleFile *simple_file,
				   gchar *filename);
void ags_simple_file_close(AgsSimpleFile *simple_file);

/*  */
void ags_simple_file_write(AgsSimpleFile *simple_file);
void ags_simple_file_write_resolve(AgsSimpleFile *simple_file);

void ags_simple_file_read(AgsSimpleFile *simple_file);
void ags_simple_file_read_resolve(AgsSimpleFile *simple_file);
void ags_simple_file_read_start(AgsSimpleFile *simple_file);

void ags_simple_file_read_config(AgsSimpleFile *simple_file, xmlNode *node, GObject **ags_config);
void ags_simple_file_write_config(AgsSimpleFile *simple_file, xmlNode *parent, GObject *ags_config);

/*  */
void ags_simple_file_read_application_context(AgsSimpleFile *simple_file, xmlNode *node, GObject **application_context);
void ags_simple_file_write_application_context(AgsSimpleFile *simple_file, xmlNode *parent, GObject *application_context);

/* */
AgsSimpleFile* ags_simple_file_new();

#endif /*__AGS_SIMPLE_FILE_H__*/
