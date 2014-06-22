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

#ifndef __AGS_MACHINE_H__
#define __AGS_MACHINE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_audio.h>

#define AGS_TYPE_MACHINE                (ags_machine_get_type())
#define AGS_MACHINE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MACHINE, AgsMachine))
#define AGS_MACHINE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MACHINE, AgsMachineClass))
#define AGS_IS_MACHINE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MACHINE))
#define AGS_IS_MACHINE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_MACHINE))
#define AGS_MACHINE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MACHINE, AgsMachineClass))

#define AGS_MACHINE_DEFAULT_VERSION "0.4.0\0"
#define AGS_MACHINE_DEFAULT_BUILD_ID "CEST 22-06-2014 03:07\0"

typedef struct _AgsMachine AgsMachine;
typedef struct _AgsMachineClass AgsMachineClass;

typedef enum{
  AGS_MACHINE_SOLO              = 1,
  AGS_MACHINE_IS_EFFECT         = 1 <<  1,
  AGS_MACHINE_IS_SEQUENCER      = 1 <<  2,
  AGS_MACHINE_IS_SYNTHESIZER    = 1 <<  3,
  AGS_MACHINE_TAKES_FILE_INPUT  = 1 <<  4,
  AGS_MACHINE_MAPPED_RECALL     = 1 <<  5,
  AGS_MACHINE_PREMAPPED_RECALL  = 1 <<  5,
}AgsMachineFlags;

typedef enum{
  AGS_MACHINE_ACCEPT_WAV          = 1,
  AGS_MACHINE_ACCEPT_OGG          = 1 <<  1,
  AGS_MACHINE_ACCEPT_SOUNDFONT2   = 1 <<  2,
}AgsMachineFileInputFlags;

struct _AgsMachine
{
  GtkHandleBox handle_box;

  GObject *ags_main;

  char *name;

  gchar *version;
  gchar *build_id;

  guint flags;
  guint file_input_flags;

  AgsAudio *audio;

  GType output_pad_type;
  GType output_line_type;
  GtkContainer *output;

  GType input_pad_type;
  GType input_line_type;
  GtkContainer *input;

  GList *port;

  GtkMenu *popup;
  GtkDialog *properties;
  GtkDialog *rename;
};

struct _AgsMachineClass
{
  GtkHandleBoxClass handle_box;

  void (*add_default_recalls)(AgsMachine *machine);
};

GType ags_machine_get_type(void);

void ags_machine_add_default_recalls(AgsMachine *machine);

GtkListStore* ags_machine_get_possible_links(AgsMachine *machine);

AgsMachine* ags_machine_find_by_name(GList *list, char *name);

void ags_machine_find_port(AgsMachine *machine);

GtkFileChooserDialog* ags_machine_file_chooser_dialog_new(AgsMachine *machine);

void ags_machine_open_files(AgsMachine *machine,
			    GSList *filenames,
			    gboolean overwrite_channels,
			    gboolean create_channels);

AgsMachine* ags_machine_new(GObject *devout);

#endif /*__AGS_MACHINE_H__*/
