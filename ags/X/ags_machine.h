/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_MACHINE_H__
#define __AGS_MACHINE_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#define AGS_TYPE_MACHINE                (ags_machine_get_type())
#define AGS_MACHINE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MACHINE, AgsMachine))
#define AGS_MACHINE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MACHINE, AgsMachineClass))
#define AGS_IS_MACHINE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MACHINE))
#define AGS_IS_MACHINE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_MACHINE))
#define AGS_MACHINE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MACHINE, AgsMachineClass))

#define AGS_MACHINE_AUTOMATION_PORT(ptr) ((AgsMachineAutomationPort *)(ptr))

#define AGS_MACHINE_DEFAULT_VERSION "2.1.60"
#define AGS_MACHINE_DEFAULT_BUILD_ID "Wed Feb 20 18:38:17 UTC 2019"

typedef struct _AgsMachine AgsMachine;
typedef struct _AgsMachineClass AgsMachineClass;
typedef struct _AgsMachineAutomationPort AgsMachineAutomationPort;

typedef enum{
  AGS_MACHINE_SOLO                    = 1,
  AGS_MACHINE_IS_EFFECT               = 1 <<  1,
  AGS_MACHINE_IS_SEQUENCER            = 1 <<  2,
  AGS_MACHINE_IS_SYNTHESIZER          = 1 <<  3,
  AGS_MACHINE_IS_WAVE_PLAYER          = 1 <<  4,
  AGS_MACHINE_TAKES_FILE_INPUT        = 1 <<  5,
  AGS_MACHINE_MAPPED_RECALL           = 1 <<  6,
  AGS_MACHINE_PREMAPPED_RECALL        = 1 <<  7,
  AGS_MACHINE_BLOCK_PLAY              = 1 <<  8,
  AGS_MACHINE_BLOCK_STOP              = 1 <<  9,
  AGS_MACHINE_BLOCK_STOP_CALLBACK     = 1 << 10,
  AGS_MACHINE_CONNECTED               = 1 << 11,
  AGS_MACHINE_REVERSE_NOTATION        = 1 << 12,
  AGS_MACHINE_STICKY_CONTROLS         = 1 << 13,
}AgsMachineFlags;

typedef enum{
  AGS_MACHINE_ACCEPT_WAV          = 1,
  AGS_MACHINE_ACCEPT_OGG          = 1 <<  1,
  AGS_MACHINE_ACCEPT_SOUNDFONT2   = 1 <<  2,
  AGS_MACHINE_ACCEPT_SFZ          = 1 <<  3,
}AgsMachineFileInputFlags;

typedef enum{
  AGS_MACHINE_MONO                  = 1,
  AGS_MACHINE_DISABLE_LINE_MEMBER   = 1 <<  1,
  AGS_MACHINE_DISABLE_BULK_MEMBER   = 1 <<  2,
}AgsMachineMappingFlags;

typedef enum{
  AGS_MACHINE_POPUP_COPY_PATTERN          = 1,
  AGS_MACHINE_POPUP_PASTE_PATTERN         = 1 <<  1,
  AGS_MACHINE_POPUP_ENVELOPE              = 1 <<  2,
}AgsMachineEditOptions;

typedef enum{
  AGS_MACHINE_POPUP_CONNECTION_EDITOR         = 1,
  AGS_MACHINE_SHOW_AUDIO_OUTPUT_CONNECTION    = 1 <<  1,
  AGS_MACHINE_SHOW_AUDIO_INPUT_CONNECTION     = 1 <<  2,
  AGS_MACHINE_POPUP_MIDI_DIALOG               = 1 <<  3,
  AGS_MACHINE_SHOW_MIDI_INPUT                 = 1 <<  4,
  AGS_MACHINE_SHOW_MIDI_OUTPUT                = 1 <<  5,
}AgsMachineConnectionOptions;

struct _AgsMachine
{
  GtkHandleBox handle_box;

  guint flags;
  guint file_input_flags;
  guint mapping_flags;
  guint connection_flags;

  char *machine_name;

  gchar *version;
  gchar *build_id;

  guint samplerate;
  guint buffer_size;
  guint format;
    
  guint bank_0;
  guint bank_1;

  AgsAudio *audio;
  
  GtkToggleButton *play;

  GType output_pad_type;
  GType output_line_type;
  GtkContainer *output;

  GtkWidget *selected_output_pad;
  
  GType input_pad_type;
  GType input_line_type;
  GtkContainer *input;
  
  GtkWidget *selected_input_pad;

  GtkContainer *bridge;

  GList *port;
  GList *enabled_automation_port;

  GtkMenuToolButton *menu_tool_button;
  GtkMenu *popup;
  
  GtkDialog *properties;
  GtkDialog *rename;
  GtkDialog *rename_audio;
  GtkDialog *reposition_audio;
  GtkDialog *connection_editor;
  GtkDialog *midi_dialog;
  GtkDialog *envelope_dialog;
  GtkDialog *envelope_info;
  
  GObject *application_context;
};

struct _AgsMachineClass
{
  GtkHandleBoxClass handle_box;

  void (*samplerate_changed)(AgsMachine *machine,
			     guint samplerate, guint old_samplerate);
  void (*buffer_size_changed)(AgsMachine *machine,
			      guint buffer_size, guint old_buffer_size);
  void (*format_changed)(AgsMachine *machine,
			 guint format, guint old_format);
  
  void (*resize_audio_channels)(AgsMachine *machine,
				guint new_size, guint old_size);
  void (*resize_pads)(AgsMachine *machine,
		      GType channel_type,
		      guint new_size, guint old_size);
  
  void (*map_recall)(AgsMachine *machine);
  GList* (*find_port)(AgsMachine *machine);

  void (*stop)(AgsMachine *machine,
	       GList *recall_id, gint sound_scope);
};

struct _AgsMachineAutomationPort
{
  GType channel_type;
  gchar *control_name;
};

GType ags_machine_get_type(void);

AgsMachineAutomationPort* ags_machine_automation_port_alloc(GType channel_type, gchar *control_name);
void ags_machine_automation_port_free(AgsMachineAutomationPort *automation_port);

GList* ags_machine_automation_port_find_channel_type_with_control_name(GList *list,
								       GType channel_type, gchar *control_name);

void ags_machine_samplerate_changed(AgsMachine *machine,
				    guint samplerate, guint old_samplerate);
void ags_machine_buffer_size_changed(AgsMachine *machine,
				     guint buffer_size, guint old_buffer_size);
void ags_machine_format_changed(AgsMachine *machine,
				guint format, guint old_format);

void ags_machine_resize_audio_channels(AgsMachine *machine,
				       guint new_size, guint old_size);
void ags_machine_resize_pads(AgsMachine *machine,
			     GType channel_type,
			     guint new_size, guint old_size);

void ags_machine_map_recall(AgsMachine *machine);
GList* ags_machine_find_port(AgsMachine *machine);

void ags_machine_stop(AgsMachine *machine,
		      GList *recall_id, gint sound_scope);

void ags_machine_add_default_recalls(AgsMachine *machine) G_DEPRECATED_FOR(ags_machine_map_recall);

AgsMachine* ags_machine_find_by_name(GList *list, char *name);

void ags_machine_set_run(AgsMachine *machine,
			 gboolean run);
void ags_machine_set_run_extended(AgsMachine *machine,
				  gboolean run,
				  gboolean sequencer, gboolean notation, gboolean wave, gboolean midi);

GtkListStore* ags_machine_get_possible_links(AgsMachine *machine);
GtkListStore* ags_machine_get_possible_audio_output_connections(AgsMachine *machine);
GtkListStore* ags_machine_get_possible_audio_input_connections(AgsMachine *machine);

GtkFileChooserDialog* ags_machine_file_chooser_dialog_new(AgsMachine *machine);

void ags_machine_open_files(AgsMachine *machine,
			    GSList *filenames,
			    gboolean overwrite_channels,
			    gboolean create_channels);

void ags_machine_copy_pattern(AgsMachine *machine);

void ags_machine_popup_add_edit_options(AgsMachine *machine, guint edit_options);
void ags_machine_popup_add_connection_options(AgsMachine *machine, guint connection_options);

gboolean ags_machine_message_monitor_timeout(AgsMachine *machine);

gboolean ags_machine_generic_output_message_monitor_timeout(AgsMachine *machine);
gboolean ags_machine_generic_input_message_monitor_timeout(AgsMachine *machine);

AgsMachine* ags_machine_new(GObject *soundcard);

#endif /*__AGS_MACHINE_H__*/
