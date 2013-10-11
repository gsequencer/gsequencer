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

#include <gtk/gtk.h>

#include <libxml/tree.h>

#define AGS_TYPE_FILE                (ags_file_get_type())
#define AGS_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FILE, AgsFile))
#define AGS_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FILE, AgsFileClass))
#define AGS_IS_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FILE))
#define AGS_IS_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FILE))
#define AGS_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FILE, AgsFileClass))

#define AGS_FILE_RESOLVE(f)          ((AgsFileResolve)(f))

#define AGS_FILE_DEFAULT_NS NULL

typedef struct _AgsFile AgsFile;
typedef struct _AgsFileClass AgsFileClass;

typedef struct _AgsFileIdRef AgsFileIdRef;
typedef struct _AgsFileIdConsumer AgsFileIdConsumer;

typedef void (*AgsFileResolve)(void);

typedef enum{
  AGS_FILE_READ,
  AGS_FILE_WRITE,
}AgsFileFlags;

struct _AgsFile
{
  GObject object;

  guint flags;

  char *name;
  char *encoding;
  char *dtd;
  xmlDocPtr doc;
  xmlNodePtr current;

  GList *id_refs;
  GList *id_consumers;

  GObject *clipboard;
  GList *property;
  GList *script;
  GObject *cluster;
  GObject *client;
  GObject *server;
  GObject *main;
  GList *embedded_audio;
  GList *file_link_history;
  GObject *history;
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

struct _AgsFileIdRef
{
  gchar *id;
  gpointer ref;
};

struct _AgsFileIdConsumer
{
  gchar *id;
  gpointer consumer;

  void (*resolve)(AgsFile *file, gpointer consumer, gchar *id);
};

GType ags_file_get_type(void);

AgsFileIdRef* ags_file_id_ref_alloc(gchar *id, gpointer ref);
AgsFileIdRefConsumer* ags_file_id_ref_consumer_alloc(gchar *id, gpointer consumer, AgsFileResolve *resolve);

void ags_file_add_id_ref(AgsFile *file, AgsFileIdRef *ref);
void ags_file_add_id_consumer(AgsFile *file, AgsFileIdRef *ref);

void ags_file_write(AgsFile *file);
void ags_file_write_resolve(AgsFile *file);

void ags_file_read(AgsFile *file);
void ags_file_read_resolve(AgsFile *file);
void ags_file_read_start(AgsFile *file);

/*  */
void ags_file_read_server(AgsFile *file, xmlNode *node, AgsServer **server);
void ags_file_write_server(AgsFile *file, xmlNode *parent, AgsServer *server);

/*  */
void ags_file_read_main(AgsFile *file, xmlNode *node, AgsMain **main);
void ags_file_write_main(AgsFile *file, xmlNode *parent, AgsMain *main);

/*  */
void ags_file_read_embedded_audio(AgsFile *file, xmlNode *node, AgsEmbeddedAudio **embedded_audio);
void ags_file_write_embedded_audio(AgsFile *file, xmlNode *parent, AgsEmbeddedAudio *embedded_audio);

void ags_file_read_embedded_audio_list(AgsFile *file, xmlNode *node, GList **embedded_audio);
void ags_file_write_embedded_audio_list(AgsFile *file, xmlNode *parent, GList *embedded_audio);

/*  */
void ags_file_read_file_link(AgsFile *file, xmlNode *node, AgsEmbeddedAudio **file_link);
void ags_file_write_file_link(AgsFile *file, xmlNode *parent, AgsEmbeddedAudio *file_link);

void ags_file_read_file_link_list(AgsFile *file, xmlNode *node, GList **file_link);
void ags_file_write_file_link_list(AgsFile *file, xmlNode *parent, GList *file_link);

/* */
AgsFile* ags_file_new();

#endif /*__AGS_FILE_H__*/
