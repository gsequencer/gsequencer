/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/app/ags_line_preset_editor.h>
#include <ags/app/ags_line_preset_editor_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_machine_editor.h>

#include <ags/i18n.h>

void ags_line_preset_editor_class_init(AgsLinePresetEditorClass *line_preset_editor);
void ags_line_preset_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_line_preset_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_line_preset_editor_init(AgsLinePresetEditor *line_preset_editor);

void ags_line_preset_editor_connect(AgsConnectable *connectable);
void ags_line_preset_editor_disconnect(AgsConnectable *connectable);

void ags_line_preset_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_line_preset_editor_apply(AgsApplicable *applicable);
void ags_line_preset_editor_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_line_preset_editor
 * @short_description: Edit line preset's aspects.
 * @title: AgsLinePresetEditor
 * @section_id:
 * @include: ags/app/ags_line_preset_editor.h
 *
 * #AgsLinePresetEditor is a composite widget to modify line preset. A line preset
 * editor should be packed by a #AgsMachineEditorLine. You may add/remove plugins with this
 * editor.
 */

GType
ags_line_preset_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_line_preset_editor = 0;

    static const GTypeInfo ags_line_preset_editor_info = {
      sizeof (AgsLinePresetEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_line_preset_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLinePresetEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_line_preset_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_line_preset_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_line_preset_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_line_preset_editor = g_type_register_static(GTK_TYPE_BOX,
							 "AgsLinePresetEditor", &ags_line_preset_editor_info,
							 0);

    g_type_add_interface_static(ags_type_line_preset_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
    
    g_type_add_interface_static(ags_type_line_preset_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_line_preset_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_line_preset_editor_class_init(AgsLinePresetEditorClass *line_preset_editor)
{
  /* empty */
}

void
ags_line_preset_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_line_preset_editor_connect;
  connectable->disconnect = ags_line_preset_editor_disconnect;
}

void
ags_line_preset_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_line_preset_editor_set_update;
  applicable->apply = ags_line_preset_editor_apply;
  applicable->reset = ags_line_preset_editor_reset;
}

void
ags_line_preset_editor_init(AgsLinePresetEditor *line_preset_editor)
{
  GtkGrid *grid;
  GtkBox *hbox;
  GtkLabel *label;

  line_preset_editor->connectable_flags = 0;
  
  gtk_orientable_set_orientation(GTK_ORIENTABLE(line_preset_editor),
				 GTK_ORIENTATION_VERTICAL);  

  gtk_box_set_spacing((GtkBox *) line_preset_editor,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  grid = (GtkGrid *) gtk_grid_new();

  gtk_widget_set_valign((GtkWidget *) grid,
			GTK_ALIGN_START);
  gtk_widget_set_halign((GtkWidget *) grid,
			GTK_ALIGN_START);

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_grid_attach((GtkGrid *) line_preset_editor,
		  (GtkWidget *) grid,
		  0, 1,
		  1, 1);

  /* format */
  label = (GtkLabel *) gtk_label_new(i18n("format"));

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  GTK_WIDGET(label),
		  0, 0,
		  1, 1);

  line_preset_editor->format = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_combo_box_text_append_text(line_preset_editor->format,
				 "8 bit");
  gtk_combo_box_text_append_text(line_preset_editor->format,
				 "16 bit");
  gtk_combo_box_text_append_text(line_preset_editor->format,
				 "24 bit");
  gtk_combo_box_text_append_text(line_preset_editor->format,
				 "32 bit");
  gtk_combo_box_text_append_text(line_preset_editor->format,
				 "64 bit");
  gtk_combo_box_text_append_text(line_preset_editor->format,
				 "floating point");
  gtk_combo_box_text_append_text(line_preset_editor->format,
				 "double precision floating point");
  gtk_combo_box_text_append_text(line_preset_editor->format,
				 "complex floating point");
  gtk_combo_box_set_active(line_preset_editor->format,
			   1);
  gtk_grid_attach(grid,
		  GTK_WIDGET(line_preset_editor->format),
		  1, 0,
		  1, 1);

  /* samplerate */
  label = (GtkLabel *) gtk_label_new(i18n("samplerate"));

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  GTK_WIDGET(label),
		  0, 1,
		  1, 1);

  line_preset_editor->samplerate = (GtkSpinButton *) gtk_spin_button_new_with_range(AGS_SOUNDCARD_MIN_SAMPLERATE, AGS_SOUNDCARD_MAX_SAMPLERATE, 1.0);
  gtk_spin_button_set_value(line_preset_editor->samplerate,
			    AGS_SOUNDCARD_DEFAULT_SAMPLERATE);
  
  gtk_widget_set_valign((GtkWidget *) line_preset_editor->samplerate,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) line_preset_editor->samplerate,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  GTK_WIDGET(line_preset_editor->samplerate),
		  1, 1,
		  1, 1);
}

void
ags_line_preset_editor_connect(AgsConnectable *connectable)
{
  AgsLinePresetEditor *line_preset_editor;

  line_preset_editor = AGS_LINE_PRESET_EDITOR(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (line_preset_editor->connectable_flags)) != 0){
    return;
  }

  line_preset_editor->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
}

void
ags_line_preset_editor_disconnect(AgsConnectable *connectable)
{
  AgsLinePresetEditor *line_preset_editor;

  line_preset_editor = AGS_LINE_PRESET_EDITOR(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (line_preset_editor->connectable_flags)) == 0){
    return;
  }

  line_preset_editor->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
}

void
ags_line_preset_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_line_preset_editor_apply(AgsApplicable *applicable)
{
  /* empty */
}

void
ags_line_preset_editor_reset(AgsApplicable *applicable)
{
  AgsMachineEditorLine *machine_editor_line;
  AgsLinePresetEditor *line_preset_editor;

  AgsSoundcardFormat format;
  guint samplerate;

  line_preset_editor = AGS_LINE_PRESET_EDITOR(applicable);
  
  machine_editor_line = (AgsMachineEditorLine *) gtk_widget_get_ancestor((GtkWidget *) line_preset_editor,
									 AGS_TYPE_MACHINE_EDITOR_LINE);

  if(AGS_IS_INPUT(machine_editor_line->channel) &&
     AGS_INPUT(machine_editor_line->channel)->file_link != NULL){
    AgsAudioFile *audio_file;
    
    gchar *filename;

    filename = ags_file_link_get_filename(AGS_INPUT(machine_editor_line->channel)->file_link);

    audio_file = ags_audio_file_manager_find_audio_file(ags_audio_file_manager_get_instance(),
							filename);

    g_free(filename);

    if(audio_file != NULL){
      GObject *sound_resource;
      
      guint file_samplerate;
      AgsSoundcardFormat file_format;
      gint position;
      
      sound_resource = ags_audio_file_get_sound_resource(audio_file);
      ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sound_resource),
				     NULL,
				     &file_samplerate,
				     NULL,
				     &file_format);

      /*  */
      position = 1;
      
      switch(file_format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
	position = 0;
	break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
	position = 1;
	break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
	position = 2;
	break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
	position = 3;
	break;
      case AGS_SOUNDCARD_SIGNED_64_BIT:
	position = 4;
	break;
      case AGS_SOUNDCARD_FLOAT:
	position = 5;
	break;
      case AGS_SOUNDCARD_DOUBLE:
	position = 6;
	break;
      case AGS_SOUNDCARD_COMPLEX:
	position = 7;
	break;
      }
  
      gtk_combo_box_set_active(line_preset_editor->format,
			       position);

      gtk_spin_button_set_value(line_preset_editor->samplerate,
				(gdouble) file_samplerate);
    }
  }
}

/**
 * ags_line_preset_editor_new:
 *
 * Create a new instance of #AgsLinePresetEditor
 *
 * Returns: the new #AgsLinePresetEditor
 *
 * Since: 5.3.0
 */
AgsLinePresetEditor*
ags_line_preset_editor_new()
{
  AgsLinePresetEditor *line_preset_editor;

  line_preset_editor = (AgsLinePresetEditor *) g_object_new(AGS_TYPE_LINE_PRESET_EDITOR,
							    NULL);

  return(line_preset_editor);
}
