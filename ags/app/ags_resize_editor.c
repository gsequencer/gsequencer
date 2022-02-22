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

#include <ags/app/ags_resize_editor.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_machine_editor.h>

#include <ags/i18n.h>

void ags_resize_editor_class_init(AgsResizeEditorClass *resize_editor);
void ags_resize_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_resize_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_resize_editor_init(AgsResizeEditor *resize_editor);

void ags_resize_editor_connect(AgsConnectable *connectable);
void ags_resize_editor_disconnect(AgsConnectable *connectable);

void ags_resize_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_resize_editor_apply(AgsApplicable *applicable);
void ags_resize_editor_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_resize_editor
 * @short_description: Resize audio channels or pads. 
 * @title: AgsResizeEditor
 * @section_id:
 * @include: ags/app/ags_resize_editor.h
 *
 * #AgsResizeEditor is a composite widget to resize. A resize editor 
 * should be packed by a #AgsMachineEditor.
 */

static gpointer ags_resize_editor_parent_class = NULL;
AgsConnectableInterface *ags_resize_editor_parent_connectable_interface;

GType
ags_resize_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_resize_editor = 0;

    static const GTypeInfo ags_resize_editor_info = {
      sizeof (AgsResizeEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_resize_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsResizeEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_resize_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_resize_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_resize_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_resize_editor = g_type_register_static(AGS_TYPE_PROPERTY_EDITOR,
						    "AgsResizeEditor",
						    &ags_resize_editor_info,
						    0);
    
    g_type_add_interface_static(ags_type_resize_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_resize_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_resize_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_resize_editor_class_init(AgsResizeEditorClass *resize_editor)
{
  ags_resize_editor_parent_class = g_type_class_peek_parent(resize_editor);
}

void
ags_resize_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_resize_editor_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_resize_editor_connect;
  connectable->disconnect = ags_resize_editor_disconnect;
}

void
ags_resize_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_resize_editor_set_update;
  applicable->apply = ags_resize_editor_apply;
  applicable->reset = ags_resize_editor_reset;
}

void
ags_resize_editor_init(AgsResizeEditor *resize_editor)
{
  GtkGrid *grid;
  GtkLabel *label;

  grid = (GtkGrid *) gtk_grid_new();
  gtk_box_pack_start(GTK_BOX(resize_editor),
		     GTK_WIDGET(grid),
		     FALSE, FALSE,
		     0);

  /* audio channels */
  label = (GtkLabel *) gtk_label_new(i18n("audio channels count"));

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

  resize_editor->audio_channels = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1024.0, 1.0);

  gtk_widget_set_valign((GtkWidget *) resize_editor->audio_channels,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) resize_editor->audio_channels,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  GTK_WIDGET(resize_editor->audio_channels),
		  1, 0,
		  1, 1);

  /* output pads */
  label = (GtkLabel *) gtk_label_new(i18n("output pads count"));

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

  resize_editor->output_pads = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1024.0, 1.0);

  gtk_widget_set_valign((GtkWidget *) resize_editor->output_pads,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) resize_editor->output_pads,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		   GTK_WIDGET(resize_editor->output_pads),
		   1, 1,
		   1, 1);

  /* input pads */
  label = (GtkLabel *) gtk_label_new(i18n("input pads count"));

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		   GTK_WIDGET(label),
		   0, 2,
		   1, 1);

  resize_editor->input_pads = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1024.0, 1.0);

  gtk_widget_set_valign((GtkWidget *) resize_editor->input_pads,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) resize_editor->input_pads,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  GTK_WIDGET(resize_editor->input_pads),
		  1, 2,
		  1, 1);
}

void
ags_resize_editor_connect(AgsConnectable *connectable)
{
  ags_resize_editor_parent_connectable_interface->connect(connectable);
}

void
ags_resize_editor_disconnect(AgsConnectable *connectable)
{
  ags_resize_editor_parent_connectable_interface->disconnect(connectable);
}

void
ags_resize_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_resize_editor_apply(AgsApplicable *applicable)
{
  AgsMachineEditor *machine_editor;
  AgsResizeEditor *resize_editor;

  AgsAudio *audio;
  AgsResizeAudio *resize_audio;
  
  AgsApplicationContext *application_context;

  guint audio_channels;
  guint output_pads, input_pads;
  
  resize_editor = AGS_RESIZE_EDITOR(applicable);

  if((AGS_PROPERTY_EDITOR_ENABLED & (AGS_PROPERTY_EDITOR(resize_editor)->flags)) == 0){
    return;
  }
  
  machine_editor = AGS_MACHINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(resize_editor),
							      AGS_TYPE_MACHINE_EDITOR));
  
  application_context = ags_application_context_get_instance();

  /* get audio */
  audio = machine_editor->machine->audio;

  /* create task */
  audio_channels = gtk_spin_button_get_value_as_int(resize_editor->audio_channels);
  
  output_pads = gtk_spin_button_get_value_as_int(resize_editor->output_pads);
  input_pads = gtk_spin_button_get_value_as_int(resize_editor->input_pads);
  
  resize_audio = ags_resize_audio_new(audio,
				      output_pads,
				      input_pads,
				      audio_channels);
      
  /* append AgsResizeAudio */
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) resize_audio);
}

void
ags_resize_editor_reset(AgsApplicable *applicable)
{
  AgsMachineEditor *machine_editor;
  AgsResizeEditor *resize_editor;

  AgsAudio *audio;

  guint audio_channels;
  guint min_audio_channels, max_audio_channels;
  guint output_pads, input_pads;
  guint min_output_pads, max_output_pads, min_input_pads, max_input_pads;

  resize_editor = AGS_RESIZE_EDITOR(applicable);

  machine_editor = AGS_MACHINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(resize_editor),
							      AGS_TYPE_MACHINE_EDITOR));

  audio = machine_editor->machine->audio;
  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       "min-audio-channels", &min_audio_channels,
	       "max-audio-channels", &max_audio_channels,
	       "output-pads", &output_pads,
	       "input-pads", &input_pads,
	       "min-output-pads", &min_output_pads,
	       "max-output-pads", &max_output_pads,
	       "min-input-pads", &min_input_pads,
	       "max-input-pads", &max_input_pads,
	       NULL);
  
  /* reset - audio channels */
  gtk_spin_button_set_range(resize_editor->audio_channels,
			    (gdouble) min_audio_channels, (gdouble) max_audio_channels);

  gtk_spin_button_set_value(resize_editor->audio_channels,
			    (gdouble) audio_channels);

  /* reset - output/input pads */
  gtk_spin_button_set_range(resize_editor->output_pads,
			   (gdouble) min_output_pads, (gdouble) max_output_pads);
  
  gtk_spin_button_set_range(resize_editor->input_pads,
			   (gdouble) min_input_pads, (gdouble) max_input_pads);

  gtk_spin_button_set_value(resize_editor->output_pads,
			    (gdouble) output_pads);

  gtk_spin_button_set_value(resize_editor->input_pads,
			    (gdouble) input_pads);
}

/**
 * ags_resize_editor_new:
 *
 * Create a new instance of #AgsResizeEditor
 *
 * Returns: the new #AgsResizeEditor
 *
 * Since: 3.0.0
 */
AgsResizeEditor*
ags_resize_editor_new()
{
  AgsResizeEditor *resize_editor;

  resize_editor = (AgsResizeEditor *) g_object_new(AGS_TYPE_RESIZE_EDITOR,
						   NULL);
  
  return(resize_editor);
}
