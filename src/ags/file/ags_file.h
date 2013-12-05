/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_FILE_H__
#define __AGS_FILE_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#define AGS_TYPE_FILE                (ags_file_get_type())
#define AGS_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FILE, AgsFile))
#define AGS_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FILE, AgsFileClass))
#define AGS_IS_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FILE))
#define AGS_IS_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FILE))
#define AGS_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FILE, AgsFileClass))

#define AGS_FILE_DEFAULT_ENCODING "UTF-8\0" //"ISO-8859-15"
#define AGS_FILE_DEFAULT_DTD "ags_file.dtd\0"
#define AGS_FILE_DEFAULT_NS NULL
#define AGS_FILE_DEFAULT_PREFIX NULL

#define AGS_FILE_CHECKSUM_LENGTH (g_checksum_type_get_length(G_CHECKSUM_MD5))

#define AGS_FILE_DEFAULT_AUDIO_FORMAT "raw\0"
#define AGS_FILE_DEFAULT_AUDIO_ENCODING "base64\0"

typedef struct _AgsFile AgsFile;
typedef struct _AgsFileClass AgsFileClass;

typedef enum{
  AGS_FILE_READ                    = 1,
  AGS_FILE_READ_AUDIO_SIGNAL       = 1 << 1,
  AGS_FILE_READ_EMBEDDED_AUDIO     = 1 << 2,
  AGS_FILE_WRITE                   = 1 << 3,
  AGS_FILE_WRITE_AUDIO_SIGNAL      = 1 << 4,
  AGS_FILE_WRITE_EMBEDDED_AUDIO    = 1 << 5,
}AgsFileFlags;

struct _AgsFile
{
  GObject object;

  guint flags;

  gchar *filename;
  gchar *encoding;
  gchar *dtd;

  gchar *audio_format;
  gchar *audio_encoding;

  xmlDoc *doc;

  GList *id_refs;
  GList *lookup;
  GList *launch;

  GObject *ags_main;

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
  GObjectClass object;

  void (*write)(AgsFile *file);
  void (*write_resolve)(AgsFile *file);

  void (*read)(AgsFile *file);
  void (*read_resolve)(AgsFile *file);
  void (*read_start)(AgsFile *file);
};

GType ags_file_get_type(void);

gchar* ags_file_str2md5(gchar *content, guint strlen);

void ags_file_add_id_ref(AgsFile *file, GObject *id_ref);

GObject* ags_file_find_id_ref_by_node(AgsFile *file, xmlNode *node);
GObject* ags_file_find_id_ref_by_xpath(AgsFile *file, gchar *xpath);
GObject* ags_file_find_id_ref_by_reference(AgsFile *file, gpointer ref);

void ags_file_add_lookup(AgsFile *file, GObject *id_ref);

void ags_file_write(AgsFile *file);
void ags_file_write_resolve(AgsFile *file);

void ags_file_read(AgsFile *file);
void ags_file_read_resolve(AgsFile *file);
void ags_file_read_start(AgsFile *file);

/*  */
void ags_file_read_server(AgsFile *file, xmlNode *node, GObject **server);
void ags_file_write_server(AgsFile *file, xmlNode *parent, GObject *server);

/*  */
void ags_file_read_main(AgsFile *file, xmlNode *node, GObject **main);
void ags_file_write_main(AgsFile *file, xmlNode *parent, GObject *main);

/* */
AgsFile* ags_file_new();

#endif /*__AGS_FILE_H__*/
