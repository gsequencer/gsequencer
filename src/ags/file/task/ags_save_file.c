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

#include <ags/file/task/ags_save_file.h>

#include <ags/object/ags_connectable.h>

void ags_save_file_class_init(AgsSaveFileClass *save_file);
void ags_save_file_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_save_file_init(AgsSaveFile *save_file);
void ags_save_file_connect(AgsConnectable *connectable);
void ags_save_file_disconnect(AgsConnectable *connectable);
void ags_save_file_finalize(GObject *gobject);

void ags_save_file_launch(AgsTask *task);

/**
 * SECTION:ags_save_file
 * @short_description: save file object
 * @title: AgsSaveFile
 * @section_id:
 * @include: ags/audio/task/ags_save_file.h
 *
 * The #AgsSaveFile task saves files.
 */

static gpointer ags_save_file_parent_class = NULL;
static AgsConnectableInterface *ags_save_file_parent_connectable_interface;

GType
ags_save_file_get_type()
{
  static GType ags_type_save_file = 0;

  if(!ags_type_save_file){
    static const GTypeInfo ags_save_file_info = {
      sizeof (AgsSaveFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_save_file_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSaveFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_save_file_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_save_file_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_save_file = g_type_register_static(AGS_TYPE_TASK,
						"AgsSaveFile\0",
						&ags_save_file_info,
						0);

    g_type_add_interface_static(ags_type_save_file,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_save_file);
}

void
ags_save_file_class_init(AgsSaveFileClass *save_file)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_save_file_parent_class = g_type_class_peek_parent(save_file);

  /* gobject */
  gobject = (GObjectClass *) save_file;

  gobject->finalize = ags_save_file_finalize;

  /* task */
  task = (AgsTaskClass *) save_file;

  task->launch = ags_save_file_launch;
}

void
ags_save_file_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_save_file_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_save_file_connect;
  connectable->disconnect = ags_save_file_disconnect;
}

void
ags_save_file_init(AgsSaveFile *save_file)
{
  save_file->file = NULL;
}

void
ags_save_file_connect(AgsConnectable *connectable)
{
  ags_save_file_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_save_file_disconnect(AgsConnectable *connectable)
{
  ags_save_file_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_save_file_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_save_file_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_save_file_launch(AgsTask *task)
{
  AgsSaveFile *save_file;

  save_file = AGS_SAVE_FILE(task);

  g_message("Saving to: %s", save_file->file->filename);
  ags_file_rw_open(save_file->file,
		   TRUE);
  ags_file_write(save_file->file);
  ags_file_close(save_file->file);
}

/**
 * ags_save_file_new:
 * @file: the #AgsFile
 *
 * Creates an #AgsSaveFile.
 *
 * Returns: an new #AgsSaveFile.
 *
 * Since: 0.4
 */
AgsSaveFile*
ags_save_file_new(AgsFile *file)
{
  AgsSaveFile *save_file;

  save_file = (AgsSaveFile *) g_object_new(AGS_TYPE_SAVE_FILE,
					   NULL);
  g_object_ref(file);
  save_file->file = file;

  return(save_file);
}

