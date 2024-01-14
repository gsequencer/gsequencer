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

#ifndef __AGS_SIMPLE_FILE_H__
#define __AGS_SIMPLE_FILE_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <unistd.h>

G_BEGIN_DECLS

#define AGS_TYPE_SIMPLE_FILE                (ags_simple_file_get_type())
#define AGS_SIMPLE_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SIMPLE_FILE, AgsSimpleFile))
#define AGS_SIMPLE_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SIMPLE_FILE, AgsSimpleFileClass))
#define AGS_IS_SIMPLE_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SIMPLE_FILE))
#define AGS_IS_SIMPLE_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SIMPLE_FILE))
#define AGS_SIMPLE_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_SIMPLE_FILE, AgsSimpleFileClass))

#define AGS_SIMPLE_FILE_DEFAULT_ENCODING "UTF-8"
#define AGS_SIMPLE_FILE_DEFAULT_DTD "ags_simple_file.dtd"

#define AGS_SIMPLE_FILE_CHECKSUM_LENGTH (g_checksum_type_get_length(G_CHECKSUM_MD5))

#define AGS_SIMPLE_FILE_DEFAULT_AUDIO_FORMAT "raw"
#define AGS_SIMPLE_FILE_DEFAULT_AUDIO_ENCODING "base64"

#define AGS_SIMPLE_FILE_ID_PROP "id"
#define AGS_SIMPLE_FILE_NAME_PROP "name"
#define AGS_SIMPLE_FILE_TYPE_PROP "type"
#define AGS_SIMPLE_FILE_VERSION_PROP "version"
#define AGS_SIMPLE_FILE_BUILD_ID_PROP "build-id"
#define AGS_SIMPLE_FILE_TRUE "true"
#define AGS_SIMPLE_FILE_FALSE "false"

typedef struct _AgsSimpleFile AgsSimpleFile;
typedef struct _AgsSimpleFileClass AgsSimpleFileClass;

typedef enum{
  AGS_SIMPLE_FILE_READ_BASIC              = 1,
  AGS_SIMPLE_FILE_READ_AUDIO_SIGNAL       = 1 << 1,
  AGS_SIMPLE_FILE_READ_EMBEDDED_AUDIO     = 1 << 2,
  AGS_SIMPLE_FILE_WRITE_BASIC             = 1 << 3,
  AGS_SIMPLE_FILE_WRITE_AUDIO_SIGNAL      = 1 << 4,
  AGS_SIMPLE_FILE_WRITE_EMBEDDED_AUDIO    = 1 << 5,
}AgsSimpleFileFlags;

#define AGS_SIMPLE_FILE_ERROR (ags_simple_file_error_quark())

typedef enum{
  AGS_SIMPLE_FILE_ERROR_PARSER_FAILURE,
}AgsSimpleFileError;

struct _AgsSimpleFile
{
  GObject gobject;

  guint flags;

  FILE *out;
  xmlChar *buffer;

  gchar *filename;
  gchar *encoding;
  gchar *dtd;

  gchar *audio_format;
  gchar *audio_encoding;

  xmlDoc *doc;
  xmlNode *root_node;

  GList *id_ref;
  GList *lookup;
  GList *launch;

  gboolean no_config;

  AgsFileUtil *file_util;
};

struct _AgsSimpleFileClass
{
  GObjectClass gobject;

  void (*open)(AgsSimpleFile *simple_file,
	       GError **error);
  void (*open_from_data)(AgsSimpleFile *simple_file,
			 gchar *data, guint length,
			 GError **error);
  void (*rw_open)(AgsSimpleFile *simple_file,
		  gboolean create,
		  GError **error);

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

void ags_simple_file_read_config(AgsSimpleFile *simple_file, xmlNode *node, AgsConfig **ags_config);

/* */
AgsSimpleFile* ags_simple_file_new();

G_END_DECLS

#endif /*__AGS_SIMPLE_FILE_H__*/
