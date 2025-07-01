/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/app/ags_midi_cc_editor.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_midi_cc_dialog.h>

#include <ags/ags_api_config.h>

#include <ags/i18n.h>

void ags_midi_cc_editor_class_init(AgsMidiCCEditorClass *midi_cc_editor);
void ags_midi_cc_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_midi_cc_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_midi_cc_editor_init(AgsMidiCCEditor *midi_cc_editor);

gboolean ags_midi_cc_editor_is_connected(AgsConnectable *connectable);
void ags_midi_cc_editor_connect(AgsConnectable *connectable);
void ags_midi_cc_editor_disconnect(AgsConnectable *connectable);

void ags_midi_cc_editor_midi1_apply_recall(AgsMidiCCEditor *midi_cc_editor,
					   AgsRecall *recall,
					   guint midi2_channel,
					   gchar *specifier);
void ags_midi_cc_editor_midi2_apply_recall(AgsMidiCCEditor *midi_cc_editor,
					   AgsRecall *recall,
					   guint32 midi2_group,
					   guint32 midi2_channel,
					   guint32 midi2_note,
					   gchar *specifier);

void ags_midi_cc_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_midi_cc_editor_apply(AgsApplicable *applicable);
void ags_midi_cc_editor_reset(AgsApplicable *applicable);

void ags_midi_cc_editor_load_port(AgsMidiCCEditor *midi_cc_editor);

/**
 * SECTION:ags_midi_cc_editor
 * @short_description: edit MIDI control change.
 * @title: AgsMidiCCEditor
 * @section_id:
 * @include: ags/app/ags_midi_cc_editor.h
 *
 * #AgsMidiCCEditor is a composite widget to assign MIDI control change messages to a
 * specific port.
 */

static gpointer ags_midi_cc_editor_parent_class = NULL;

GType
ags_midi_cc_editor_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_midi_cc_editor = 0;

    static const GTypeInfo ags_midi_cc_editor_info = {
      sizeof (AgsMidiCCEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_midi_cc_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMidiCCEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_midi_cc_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_midi_cc_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_midi_cc_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_midi_cc_editor = g_type_register_static(GTK_TYPE_BOX,
						     "AgsMidiCCEditor", &ags_midi_cc_editor_info,
						     0);

    g_type_add_interface_static(ags_type_midi_cc_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_midi_cc_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_midi_cc_editor);
  }

  return(g_define_type_id__static);
}

void
ags_midi_cc_editor_class_init(AgsMidiCCEditorClass *midi_cc_editor)
{
  ags_midi_cc_editor_parent_class = g_type_class_peek_parent(midi_cc_editor);
}

void
ags_midi_cc_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_midi_cc_editor_is_connected;  
  connectable->connect = ags_midi_cc_editor_connect;
  connectable->disconnect = ags_midi_cc_editor_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_midi_cc_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_midi_cc_editor_set_update;
  applicable->apply = ags_midi_cc_editor_apply;
  applicable->reset = ags_midi_cc_editor_reset;
}

void
ags_midi_cc_editor_init(AgsMidiCCEditor *midi_cc_editor)
{
  GtkGrid *grid;
  GtkLabel *label;
  
#if defined(AGS_OSXAPI)
  midi_cc_editor->flags = AGS_MIDI_CC_DIALOG_SHOW_MIDI_2_0;
#else
  midi_cc_editor->flags = AGS_MIDI_CC_DIALOG_SHOW_MIDI_1_0;
#endif
  
  midi_cc_editor->connectable_flags = 0;

  midi_cc_editor->parent_midi_cc_dialog = NULL;

  midi_cc_editor->port = NULL;

  /* MIDI 1 */
  midi_cc_editor->midi1_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						     AGS_UI_PROVIDER_DEFAULT_SPACING);
  
#if defined(AGS_OSXAPI)
  gtk_widget_set_visible((GtkWidget *) midi_cc_editor->midi1_box,
			 FALSE);
#endif
  
  gtk_box_append((GtkBox *) midi_cc_editor,
		 (GtkWidget *) midi_cc_editor->midi1_box);
  
  /* grid */
  grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_box_append(midi_cc_editor->midi1_box,
		 (GtkWidget *) grid);

  /* control change */
  midi_cc_editor->midi1_control = 0;
  
  midi_cc_editor->midi1_control_label = (GtkLabel *) gtk_label_new(NULL);

  gtk_label_set_xalign(midi_cc_editor->midi1_control_label,
		       0.0);
  gtk_widget_set_size_request((GtkWidget *) midi_cc_editor->midi1_control_label,
			      240,
			      -1);
  gtk_grid_attach(grid,
		  (GtkWidget *) midi_cc_editor->midi1_control_label,
		  0, 0,
		  1, 1);

  /* port */
  midi_cc_editor->midi1_port_drop_down = (GtkDropDown *) gtk_drop_down_new(NULL, NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) midi_cc_editor->midi1_port_drop_down,
		  1, 0,
		  1, 1);

  /* midi channel */
  label = (GtkLabel *) gtk_label_new(i18n("channel"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  2, 0,
		  1, 1);

  midi_cc_editor->midi1_channel = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										   15.0,
										   1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) midi_cc_editor->midi1_channel,
		  3, 0,
		  1, 1);
  
  /* MIDI 2 */
  midi_cc_editor->midi2_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						     AGS_UI_PROVIDER_DEFAULT_SPACING);

#if !defined(AGS_OSXAPI)
  gtk_widget_set_visible((GtkWidget *) midi_cc_editor->midi2_box,
			 FALSE);
#endif

  gtk_box_append((GtkBox *) midi_cc_editor,
		 (GtkWidget *) midi_cc_editor->midi2_box);
  
  /* grid */
  grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_box_append(midi_cc_editor->midi2_box,
		 (GtkWidget *) grid);

  /* control change */
  midi_cc_editor->midi2_control = 0;
  
  midi_cc_editor->midi2_control_label = (GtkLabel *) gtk_label_new(NULL);

  gtk_label_set_xalign(midi_cc_editor->midi2_control_label,
		       0.0);
  gtk_widget_set_size_request((GtkWidget *) midi_cc_editor->midi2_control_label,
			      240,
			      -1);
  gtk_grid_attach(grid,
		  (GtkWidget *) midi_cc_editor->midi2_control_label,
		  0, 0,
		  1, 1);

  /* port */
  midi_cc_editor->midi2_port_drop_down = (GtkDropDown *) gtk_drop_down_new(NULL, NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) midi_cc_editor->midi2_port_drop_down,
		  1, 0,
		  1, 1);

  /* midi group */
  label = (GtkLabel *) gtk_label_new(i18n("group"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  2, 0,
		  1, 1);

  midi_cc_editor->midi2_group = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										 15.0,
										 1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) midi_cc_editor->midi2_group,
		  3, 0,
		  1, 1);

  /* midi channel */
  label = (GtkLabel *) gtk_label_new(i18n("channel"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  4, 0,
		  1, 1);

  midi_cc_editor->midi2_channel = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										   15.0,
										   1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) midi_cc_editor->midi2_channel,
		  5, 0,
		  1, 1);

  /* midi note */
  label = (GtkLabel *) gtk_label_new(i18n("note"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  6, 0,
		  1, 1);

  midi_cc_editor->midi2_note = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										127.0,
										1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) midi_cc_editor->midi2_note,
		  7, 0,
		  1, 1);
}

gboolean
ags_midi_cc_editor_is_connected(AgsConnectable *connectable)
{
  AgsMidiCCEditor *midi_cc_editor;

  gboolean retval;

  midi_cc_editor = AGS_MIDI_CC_EDITOR(connectable);

  retval = ((AGS_CONNECTABLE_CONNECTED & (midi_cc_editor->connectable_flags)) != 0) ? TRUE: FALSE;

  return(retval);
}

void
ags_midi_cc_editor_connect(AgsConnectable *connectable)
{
  AgsMidiCCEditor *midi_cc_editor;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }
  
  midi_cc_editor = AGS_MIDI_CC_EDITOR(connectable);

  midi_cc_editor->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  //TODO:JK: implement me
}

void
ags_midi_cc_editor_disconnect(AgsConnectable *connectable)
{
  AgsMidiCCEditor *midi_cc_editor;
  
  if(!ags_connectable_is_connected(connectable)){
    return;
  }
  
  midi_cc_editor = AGS_MIDI_CC_EDITOR(connectable);
  
  midi_cc_editor->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  //TODO:JK: implement me
}

void
ags_midi_cc_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_midi_cc_editor_midi1_apply_recall(AgsMidiCCEditor *midi_cc_editor,
				      AgsRecall *recall,
				      guint midi1_channel,
				      gchar *specifier)
{
  GHashTable *midi1_cc_to_port_specifier;
  GList *start_port, *port;

  gchar *local_specifier;

  GRecMutex *recall_mutex;

  g_return_if_fail(AGS_IS_MIDI_CC_EDITOR(midi_cc_editor));
  g_return_if_fail(AGS_IS_RECALL(recall));
  g_return_if_fail(midi1_channel < 16);

  if(specifier == NULL){
    return;
  }
  
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  midi1_cc_to_port_specifier = ags_recall_get_midi1_cc_to_port_specifier(recall);

  local_specifier = g_strdup(specifier);

  g_rec_mutex_lock(recall_mutex);

  switch(midi_cc_editor->midi1_control){
  case AGS_RECALL_MIDI1_BANK_SELECT:
    {
      g_hash_table_insert(midi1_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI1_CC_BANK_SELECT(midi1_channel)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI1_MODULATION_WHEEL:
    {
      g_hash_table_insert(midi1_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI1_CC_MODULATION_WHEEL(midi1_channel)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI1_BREATH_CONTROLLER:
    {
      g_hash_table_insert(midi1_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI1_CC_BREATH_CONTROLLER(midi1_channel)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI1_FOOT_CONTROLLER:
    {
      g_hash_table_insert(midi1_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI1_CC_FOOT_CONTROLLER(midi1_channel)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI1_PORTAMENTO_TIME:
    {
      g_hash_table_insert(midi1_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI1_CC_PORTAMENTO_TIME(midi1_channel)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI1_CHANNEL_VOLUME:
    {
      g_hash_table_insert(midi1_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI1_CC_CHANNEL_VOLUME(midi1_channel)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI1_BALANCE:
    {
      g_hash_table_insert(midi1_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI1_CC_BALANCE(midi1_channel)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI1_PAN:
    {
      g_hash_table_insert(midi1_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI1_CC_PAN(midi1_channel)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI1_EXPRESSION_CONTROLLER:
    {
      g_hash_table_insert(midi1_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI1_CC_EXPRESSION_CONTROLLER(midi1_channel)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI1_EFFECT_CONTROL_1:
    {
      g_hash_table_insert(midi1_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI1_CC_EFFECT_CONTROL_1(midi1_channel)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI1_EFFECT_CONTROL_2:
    {
      g_hash_table_insert(midi1_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI1_CC_EFFECT_CONTROL_2(midi1_channel)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI1_GENERAL_PURPOSE_CONTROLLER_1:
    {
      g_hash_table_insert(midi1_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI1_CC_GENERAL_PURPOSE_CONTROLLER_1(midi1_channel)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI1_GENERAL_PURPOSE_CONTROLLER_2:
    {
      g_hash_table_insert(midi1_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI1_CC_GENERAL_PURPOSE_CONTROLLER_2(midi1_channel)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI1_GENERAL_PURPOSE_CONTROLLER_3:
    {
      g_hash_table_insert(midi1_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI1_CC_GENERAL_PURPOSE_CONTROLLER_3(midi1_channel)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI1_GENERAL_PURPOSE_CONTROLLER_4:
    {
      g_hash_table_insert(midi1_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI1_CC_GENERAL_PURPOSE_CONTROLLER_4(midi1_channel)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI1_CHANGE_PROGRAM:
    {
      g_hash_table_insert(midi1_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI1_CC_CHANGE_PROGRAM(midi1_channel)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI1_CHANGE_PRESSURE:
    {
      g_hash_table_insert(midi1_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI1_CC_CHANGE_PRESSURE(midi1_channel)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI1_PITCH_BEND:
    {
      g_hash_table_insert(midi1_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI1_CC_PITCH_BEND(midi1_channel)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI1_SONG_POSITION:
    {
      g_hash_table_insert(midi1_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI1_CC_SONG_POSITION(midi1_channel)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI1_SONG_SELECT:
    {
      g_hash_table_insert(midi1_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI1_CC_SONG_SELECT(midi1_channel)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI1_TUNE_REQUEST:
    {
      g_hash_table_insert(midi1_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI1_CC_TUNE_REQUEST(midi1_channel)),
			  local_specifier);
    }
    break;
  }

  g_rec_mutex_unlock(recall_mutex);

  g_hash_table_unref(midi1_cc_to_port_specifier);
}

void
ags_midi_cc_editor_midi2_apply_recall(AgsMidiCCEditor *midi_cc_editor,
				      AgsRecall *recall,
				      guint32 midi2_group,
				      guint32 midi2_channel,
				      guint32 midi2_note,
				      gchar *specifier)
{
  GHashTable *midi2_cc_to_port_specifier;
  GList *start_port, *port;

  gchar *local_specifier;

  GRecMutex *recall_mutex;

  g_return_if_fail(AGS_IS_MIDI_CC_EDITOR(midi_cc_editor));
  g_return_if_fail(AGS_IS_RECALL(recall));
  g_return_if_fail(midi2_group < 16);
  g_return_if_fail(midi2_channel < 16);
  g_return_if_fail(midi2_note < 128);

  if(specifier == NULL){
    return;
  }

  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  midi2_cc_to_port_specifier = ags_recall_get_midi2_cc_to_port_specifier(recall);

  local_specifier = g_strdup(specifier);

  g_rec_mutex_lock(recall_mutex);
  
  switch((AgsUmpWord) midi_cc_editor->midi2_control){
  case AGS_RECALL_MIDI2_MIDI1_BANK_SELECT:
    {
      g_hash_table_insert(midi2_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI2_CC_MIDI1_BANK_SELECT(midi2_group, midi2_channel, midi2_note)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI2_MIDI1_MODULATION_WHEEL:
    {
      g_hash_table_insert(midi2_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI2_CC_MIDI1_MODULATION_WHEEL(midi2_group, midi2_channel, midi2_note)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI2_MIDI1_BREATH_CONTROLLER:
    {
      g_hash_table_insert(midi2_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI2_CC_MIDI1_BREATH_CONTROLLER(midi2_group, midi2_channel, midi2_note)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI2_MIDI1_FOOT_CONTROLLER:
    {
      g_hash_table_insert(midi2_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI2_CC_MIDI1_FOOT_CONTROLLER(midi2_group, midi2_channel, midi2_note)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI2_MIDI1_PORTAMENTO_TIME:
    {      
      g_hash_table_insert(midi2_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI2_CC_MIDI1_PORTAMENTO_TIME(midi2_group, midi2_channel, midi2_note)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI2_MIDI1_CHANNEL_VOLUME:
    {
      g_hash_table_insert(midi2_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI2_CC_MIDI1_CHANNEL_VOLUME(midi2_group, midi2_channel, midi2_note)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI2_MIDI1_BALANCE:
    {
      g_hash_table_insert(midi2_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI2_CC_MIDI1_BALANCE(midi2_group, midi2_channel, midi2_note)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI2_MIDI1_PAN:
    {
      g_hash_table_insert(midi2_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI2_CC_MIDI1_PAN(midi2_group, midi2_channel, midi2_note)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI2_MIDI1_EXPRESSION_CONTROLLER:
    {
      g_hash_table_insert(midi2_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI2_CC_MIDI1_EXPRESSION_CONTROLLER(midi2_group, midi2_channel, midi2_note)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI2_MIDI1_EFFECT_CONTROL_1:
    {
      g_hash_table_insert(midi2_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI2_CC_MIDI1_EFFECT_CONTROL_1(midi2_group, midi2_channel, midi2_note)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI2_MIDI1_EFFECT_CONTROL_2:
    {
      g_hash_table_insert(midi2_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI2_CC_MIDI1_EFFECT_CONTROL_2(midi2_group, midi2_channel, midi2_note)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI2_MIDI1_GENERAL_PURPOSE_CONTROLLER_1:
    {
      g_hash_table_insert(midi2_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI2_CC_MIDI1_GENERAL_PURPOSE_CONTROLLER_1(midi2_group, midi2_channel, midi2_note)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI2_MIDI1_GENERAL_PURPOSE_CONTROLLER_2:
    {
      g_hash_table_insert(midi2_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI2_CC_MIDI1_GENERAL_PURPOSE_CONTROLLER_2(midi2_group, midi2_channel, midi2_note)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI2_MIDI1_GENERAL_PURPOSE_CONTROLLER_3:
    {
      g_hash_table_insert(midi2_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI2_CC_MIDI1_GENERAL_PURPOSE_CONTROLLER_3(midi2_group, midi2_channel, midi2_note)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI2_MIDI1_GENERAL_PURPOSE_CONTROLLER_4:
    {
      g_hash_table_insert(midi2_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI2_CC_MIDI1_GENERAL_PURPOSE_CONTROLLER_4(midi2_group, midi2_channel, midi2_note)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI2_MIDI1_CHANGE_PROGRAM:
    {
      g_hash_table_insert(midi2_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI2_CC_MIDI1_CHANGE_PROGRAM(midi2_group, midi2_channel, midi2_note)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI2_MIDI1_CHANGE_PRESSURE:
    {
      g_hash_table_insert(midi2_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI2_CC_MIDI1_CHANGE_PRESSURE(midi2_group, midi2_channel, midi2_note)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI2_MIDI1_PITCH_BEND:
    {
      g_hash_table_insert(midi2_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI2_CC_MIDI1_PITCH_BEND(midi2_group, midi2_channel, midi2_note)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI2_CHANGE_PROGRAM:
    {
      g_hash_table_insert(midi2_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI2_CC_CHANGE_PROGRAM(midi2_group, midi2_channel, midi2_note)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI2_CHANGE_PRESSURE:
    {
      g_hash_table_insert(midi2_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI2_CC_CHANGE_PRESSURE(midi2_group, midi2_channel, midi2_note)),
			  local_specifier);
    }
    break;
  case AGS_RECALL_MIDI2_PITCH_BEND:
    {
      g_hash_table_insert(midi2_cc_to_port_specifier,
			  GUINT_TO_POINTER((guint) AGS_RECALL_MIDI2_CC_PITCH_BEND(midi2_group, midi2_channel, midi2_note)),
			  local_specifier);
    }
    break;
  }

  g_rec_mutex_unlock(recall_mutex);

  g_hash_table_unref(midi2_cc_to_port_specifier);
}

void
ags_midi_cc_editor_apply(AgsApplicable *applicable)
{
  AgsMachine *machine;
  AgsMidiCCDialog *midi_cc_dialog;
  AgsMidiCCEditor *midi_cc_editor;

  AgsRecallAudio *recall_audio;
  AgsRecallContainer *play_container, *recall_container;
  
  GObject *gobject;

  GList *start_recall, *recall;
  GList *start_recall_channel, *recall_channel;
  GList *start_port, *port;
  
  gchar *specifier;

  guint midi1_channel;

  guint32 midi2_group;
  guint32 midi2_channel;
  guint32 midi2_note;

  GRecMutex *recall_mutex;
  
  midi_cc_editor = AGS_MIDI_CC_EDITOR(applicable);

  midi_cc_dialog = (AgsMidiCCDialog *) midi_cc_editor->parent_midi_cc_dialog;

  if(midi_cc_dialog == NULL){
    return;
  }

  machine = midi_cc_dialog->machine;

  if(machine == NULL){
    return;
  }

  play_container = NULL;
  recall_container = NULL;
  
  /* port */
  specifier = NULL;

  if(gtk_drop_down_get_selected(midi_cc_editor->midi2_port_drop_down) > 0){
    gobject = gtk_drop_down_get_selected_item(midi_cc_editor->midi2_port_drop_down);
    
    specifier = gtk_string_object_get_string((GtkStringObject *) gobject);
  }

  midi1_channel = (guint) gtk_spin_button_get_value_as_int(midi_cc_editor->midi1_channel);

  midi2_group = (guint32) gtk_spin_button_get_value_as_int(midi_cc_editor->midi2_group);
  midi2_channel = (guint32) gtk_spin_button_get_value_as_int(midi_cc_editor->midi2_channel);
  midi2_note = (guint32) gtk_spin_button_get_value_as_int(midi_cc_editor->midi2_note);
  
  /* recall */
  recall = 
    start_recall = ags_audio_get_play(machine->audio);

  while(recall != NULL){
    gboolean success;
    
    port =
      start_port = ags_recall_get_port(recall->data);

    success = FALSE;
    
    if((port = ags_port_find_specifier(port, specifier)) != NULL){
      play_container = (AgsRecallContainer *) ags_recall_get_recall_container(recall->data);

      success = TRUE;
    }

    g_list_free_full(start_port,
		     (GDestroyNotify) g_object_unref);
    
    if(success){
      break;
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* recall */
  recall = 
    start_recall = ags_audio_get_recall(machine->audio);

  while(recall != NULL){
    gboolean success;
    
    port =
      start_port = ags_recall_get_port(recall->data);

    success = FALSE;
    
    if((port = ags_port_find_specifier(port, specifier)) != NULL){
      recall_container = (AgsRecallContainer *) ags_recall_get_recall_container(recall->data);

      success = TRUE;
    }

    g_list_free_full(start_port,
		     (GDestroyNotify) g_object_unref);
    
    if(success){
      break;
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* apply to recall audio - play container */
  recall_audio = (AgsRecallAudio *) ags_recall_container_get_recall_audio(play_container);

  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_audio);

  if(recall_audio != NULL){
    start_port = ags_recall_get_port((AgsRecall *) recall_audio);

    if(ags_port_find_specifier(start_port, specifier) != NULL){
      /* MIDI 1 */
      if(ags_midi_cc_editor_test_flags(midi_cc_editor, AGS_MIDI_CC_EDITOR_SHOW_MIDI_1_0)){
	GHashTable *midi1_cc_to_port_specifier;

	midi1_cc_to_port_specifier = ags_recall_get_midi1_cc_to_port_specifier((AgsRecall *) recall_audio);

	/* apply */
	if(midi1_cc_to_port_specifier != NULL){
	  ags_midi_cc_editor_midi1_apply_recall(midi_cc_editor,
						(AgsRecall *) recall_audio,
						midi1_channel,
						specifier);

	  g_hash_table_unref(midi1_cc_to_port_specifier);
	}
      }
      
      /* MIDI 2 */
      if(ags_midi_cc_editor_test_flags(midi_cc_editor, AGS_MIDI_CC_EDITOR_SHOW_MIDI_2_0)){
	GHashTable *midi2_cc_to_port_specifier;

	midi2_cc_to_port_specifier = ags_recall_get_midi2_cc_to_port_specifier((AgsRecall *) recall_audio);

	/* apply */
	if(midi2_cc_to_port_specifier != NULL){
	  ags_midi_cc_editor_midi2_apply_recall(midi_cc_editor,
						(AgsRecall *) recall_audio,
						midi2_group,
						midi2_channel,
						midi2_note,
						specifier);

	  g_hash_table_unref(midi2_cc_to_port_specifier);
	}
      }
    }

    g_list_free_full(start_port,
		     (GDestroyNotify) g_object_unref);
  }

  /* apply to recall audio - recall container */
  recall_audio = (AgsRecallAudio *) ags_recall_container_get_recall_audio(recall_container);

  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_audio);
  
  if(recall_audio != NULL){
    start_port = ags_recall_get_port((AgsRecall *) recall_audio);

    if(ags_port_find_specifier(start_port, specifier) != NULL){
      /* MIDI 1 */
      if(ags_midi_cc_editor_test_flags(midi_cc_editor, AGS_MIDI_CC_EDITOR_SHOW_MIDI_1_0)){
	GHashTable *midi1_cc_to_port_specifier;

	midi1_cc_to_port_specifier = ags_recall_get_midi1_cc_to_port_specifier((AgsRecall *) recall_audio);

	/* apply */
	if(midi1_cc_to_port_specifier != NULL){
	  ags_midi_cc_editor_midi1_apply_recall(midi_cc_editor,
						(AgsRecall *) recall_audio,
						midi1_channel,
						specifier);

	  g_hash_table_unref(midi1_cc_to_port_specifier);
	}
      }

      /* MIDI 2 */
      if(ags_midi_cc_editor_test_flags(midi_cc_editor, AGS_MIDI_CC_EDITOR_SHOW_MIDI_2_0)){
	GHashTable *midi2_cc_to_port_specifier;

	midi2_cc_to_port_specifier = ags_recall_get_midi2_cc_to_port_specifier((AgsRecall *) recall_audio);

	/* apply */
	if(midi2_cc_to_port_specifier != NULL){
	  ags_midi_cc_editor_midi2_apply_recall(midi_cc_editor,
						(AgsRecall *) recall_audio,
						midi2_group,
						midi2_channel,
						midi2_note,
						specifier);

	  g_hash_table_unref(midi2_cc_to_port_specifier);
	}
      }
    }

    g_list_free_full(start_port,
		     (GDestroyNotify) g_object_unref);
  }

  /* apply to recall channel - play container */
  recall_channel =
    start_recall_channel = ags_recall_container_get_recall_channel(play_container);

  while(recall_channel != NULL){
    start_port = ags_recall_get_port((AgsRecall *) recall_channel->data);

    if(ags_port_find_specifier(start_port, specifier) != NULL){
      /* MIDI 1 */
      if(ags_midi_cc_editor_test_flags(midi_cc_editor, AGS_MIDI_CC_EDITOR_SHOW_MIDI_1_0)){
	GHashTable *midi1_cc_to_port_specifier;

	midi1_cc_to_port_specifier = ags_recall_get_midi1_cc_to_port_specifier((AgsRecall *) recall_audio);

	/* apply */
	if(midi1_cc_to_port_specifier != NULL){
	  ags_midi_cc_editor_midi1_apply_recall(midi_cc_editor,
						(AgsRecall *) recall_channel->data,
						midi1_channel,
						specifier);

	  g_hash_table_unref(midi1_cc_to_port_specifier);
	}
      }

      /* MIDI 2 */
      if(ags_midi_cc_editor_test_flags(midi_cc_editor, AGS_MIDI_CC_EDITOR_SHOW_MIDI_2_0)){
	GHashTable *midi2_cc_to_port_specifier;

	midi2_cc_to_port_specifier = ags_recall_get_midi2_cc_to_port_specifier((AgsRecall *) recall_channel->data);

	/* apply */
	if(midi2_cc_to_port_specifier != NULL){
	  ags_midi_cc_editor_midi2_apply_recall(midi_cc_editor,
						(AgsRecall *) recall_channel->data,
						midi2_group,
						midi2_channel,
						midi2_note,
						specifier);

	  g_hash_table_unref(midi2_cc_to_port_specifier);
	}
      }
    }

    g_list_free_full(start_port,
		     (GDestroyNotify) g_object_unref);
    
    /* iterate */
    recall_channel = recall_channel->next;
  }
  
  /* apply to recall channel - recall container */
  recall_channel =
    start_recall_channel = ags_recall_container_get_recall_channel(recall_container);

  while(recall_channel != NULL){
    start_port = ags_recall_get_port((AgsRecall *) recall_channel->data);

    if(ags_port_find_specifier(start_port, specifier) != NULL){
      if(ags_midi_cc_editor_test_flags(midi_cc_editor, AGS_MIDI_CC_EDITOR_SHOW_MIDI_1_0)){
	GHashTable *midi1_cc_to_port_specifier;

	midi1_cc_to_port_specifier = ags_recall_get_midi1_cc_to_port_specifier((AgsRecall *) recall_audio);

	if(midi1_cc_to_port_specifier != NULL){
	  /* remove all */
	  g_rec_mutex_lock(recall_mutex);
  
	  g_hash_table_remove_all(midi1_cc_to_port_specifier);

	  g_rec_mutex_unlock(recall_mutex);

	  /* apply */
	  ags_midi_cc_editor_midi1_apply_recall(midi_cc_editor,
						(AgsRecall *) recall_channel->data,
						midi1_channel,
						specifier);

	  g_hash_table_unref(midi1_cc_to_port_specifier);
	}
      }

      /* MIDI 2 */
      if(ags_midi_cc_editor_test_flags(midi_cc_editor, AGS_MIDI_CC_EDITOR_SHOW_MIDI_2_0)){
	GHashTable *midi2_cc_to_port_specifier;

	midi2_cc_to_port_specifier = ags_recall_get_midi2_cc_to_port_specifier((AgsRecall *) recall_channel->data);

	if(midi2_cc_to_port_specifier != NULL){
	  /* remove all */
	  g_rec_mutex_lock(recall_mutex);
  
	  g_hash_table_remove_all(midi2_cc_to_port_specifier);

	  g_rec_mutex_unlock(recall_mutex);

	  /* apply */
	  ags_midi_cc_editor_midi2_apply_recall(midi_cc_editor,
						(AgsRecall *) recall_channel->data,
						midi2_group,
						midi2_channel,
						midi2_note,
						specifier);

	  g_hash_table_unref(midi2_cc_to_port_specifier);
	}
      }
    }

    g_list_free_full(start_port,
		     (GDestroyNotify) g_object_unref);
    
    /* iterate */
    recall_channel = recall_channel->next;
  }
}

void
ags_midi_cc_editor_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_cc_editor_test_flags:
 * @midi_cc_editor: the #AgsMidiCCEditor
 * @flags: the flags
 *
 * Test @flags of @midi_cc_editor.
 * 
 * Returns: %TRUE if @flags is set, otherwise %FALSE
 *
 * Since: 8.0.0
 */
gboolean
ags_midi_cc_editor_test_flags(AgsMidiCCEditor *midi_cc_editor,
			      AgsMidiCCEditorFlags flags)
{
  guint retval;
  
  g_return_val_if_fail(AGS_IS_MIDI_CC_EDITOR(midi_cc_editor), FALSE);

  retval = (((flags &(midi_cc_editor->flags))) != 0) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_midi_cc_editor_set_flags:
 * @midi_cc_editor: the #AgsMidiCCEditor
 * @flags: the flags
 *
 * Set @flags of @midi_cc_editor.
 * 
 * Since: 8.0.0
 */
void
ags_midi_cc_editor_set_flags(AgsMidiCCEditor *midi_cc_editor,
			     AgsMidiCCEditorFlags flags)
{
  g_return_if_fail(AGS_IS_MIDI_CC_EDITOR(midi_cc_editor));

  //TODO:JK: implement me
  
  midi_cc_editor->flags |= flags;
}

/**
 * ags_midi_cc_editor_unset_flags:
 * @midi_cc_editor: the #AgsMidiCCEditor
 * @flags: the flags
 *
 * Unset @flags of @midi_cc_editor.
 * 
 * Since: 8.0.0
 */
void
ags_midi_cc_editor_unset_flags(AgsMidiCCEditor *midi_cc_editor,
			       AgsMidiCCEditorFlags flags)
{
  g_return_if_fail(AGS_IS_MIDI_CC_EDITOR(midi_cc_editor));

  //TODO:JK: implement me
  
  midi_cc_editor->flags &= (~flags);
}

void
ags_midi_cc_editor_load_port(AgsMidiCCEditor *midi_cc_editor)
{
  AgsMachine *machine;

  AgsChannel *start_channel;
  AgsChannel *channel;
  
  GList *start_port, *port;
  
  gchar **collected_specifier;

  guint length;
  
  g_return_if_fail(AGS_IS_MIDI_CC_EDITOR(midi_cc_editor));
  g_return_if_fail(midi_cc_editor->parent_midi_cc_dialog != NULL);

  machine = AGS_MIDI_CC_DIALOG(midi_cc_editor->parent_midi_cc_dialog)->machine;
    
  /* add NULL to collected specifier */
  collected_specifier = (gchar **) g_malloc(2 * sizeof(gchar*));

  collected_specifier[0] = g_strdup("NULL");
  collected_specifier[1] = NULL;

  length = 2;

  /* audio */
  port =
    start_port = ags_audio_collect_all_audio_ports(machine->audio);

  while(port != NULL){
    AgsPluginPort *plugin_port;

    gchar *specifier;

    gboolean contains_control_name;

    specifier = NULL;
    plugin_port = NULL;
    
    g_object_get(port->data,
		 "specifier", &specifier,
		 "plugin-port", &plugin_port,
		 NULL);

    if(specifier == NULL){
      if(plugin_port != NULL){
	g_object_unref(plugin_port);
      }
      
      /* iterate */
      port = port->next;

      continue;
    }
    
#ifdef HAVE_GLIB_2_44
    contains_control_name = g_strv_contains((const gchar * const *) collected_specifier,
					    specifier);
#else
    contains_control_name = ags_strv_contains(collected_specifier,
					      specifier);
#endif

    if(plugin_port != NULL &&
       !contains_control_name){    
      /* add to collected specifier */
      collected_specifier = (gchar **) g_realloc(collected_specifier,
						 (length + 1) * sizeof(gchar *));
      collected_specifier[length - 1] = g_strdup(specifier);
      collected_specifier[length] = NULL;

      length++;
    }

    g_free(specifier);
    
    if(plugin_port != NULL){
      g_object_unref(plugin_port);
    }
    
    /* iterate */
    port = port->next;
  }

  g_list_free_full(start_port,
		   g_object_unref);

  /* output */
  start_channel = NULL;
  
  g_object_get(machine->audio,
	       "output", &start_channel,
	       NULL);

  channel = start_channel;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;
    
    /* output */
    port =
      start_port = ags_channel_collect_all_channel_ports(channel);

    while(port != NULL){
      AgsPluginPort *plugin_port;

      gchar *specifier;

      gboolean contains_control_name;

      specifier = NULL;
      plugin_port = NULL;
    
      g_object_get(port->data,
		   "specifier", &specifier,
		   "plugin-port", &plugin_port,
		   NULL);

      if(specifier == NULL){
	if(plugin_port != NULL){
	  g_object_unref(plugin_port);
	}
      
	/* iterate */
	port = port->next;

	continue;
      }
      
      contains_control_name = g_strv_contains((const gchar * const *) collected_specifier,
					      specifier);

      if(plugin_port != NULL &&
	 !contains_control_name){
	/* add to collected specifier */
	collected_specifier = (gchar **) g_realloc(collected_specifier,
						   (length + 1) * sizeof(gchar *));
	collected_specifier[length - 1] = g_strdup(specifier);
	collected_specifier[length] = NULL;

	length++;
      }

      g_free(specifier);
      
      if(plugin_port != NULL){
	g_object_unref(plugin_port);
      }
    
      /* iterate */
      port = port->next;
    }

    g_list_free_full(start_port,
		     g_object_unref);
    
    /* iterate */
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  /* unref */
  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
  
  /* input */
  start_channel = NULL;
  
  g_object_get(machine->audio,
	       "input", &start_channel,
	       NULL);

  channel = start_channel;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;
    
    /* input */
    port =
      start_port = ags_channel_collect_all_channel_ports(channel);

    while(port != NULL){
      AgsPluginPort *plugin_port;

      gchar *specifier;

      gboolean contains_control_name;

      specifier = NULL;
      plugin_port = NULL;
    
      g_object_get(port->data,
		   "specifier", &specifier,
		   "plugin-port", &plugin_port,
		   NULL);

      if(specifier == NULL){
	if(plugin_port != NULL){
	  g_object_unref(plugin_port);
	}
      
	/* iterate */
	port = port->next;

	continue;
      }
      
      contains_control_name = g_strv_contains((const gchar * const *) collected_specifier,
					      specifier);

      if(plugin_port != NULL &&
	 !contains_control_name){
	/* add to collected specifier */
	collected_specifier = (gchar **) g_realloc(collected_specifier,
						   (length + 1) * sizeof(gchar *));
	collected_specifier[length - 1] = g_strdup(specifier);
	collected_specifier[length] = NULL;

	length++;
      }

      g_free(specifier);
      
      if(plugin_port != NULL){
	g_object_unref(plugin_port);
      }
    
      /* iterate */
      port = port->next;
    }

    g_list_free_full(start_port,
		     g_object_unref);
    
    /* iterate */
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  /* unref */
  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
  
  if(midi_cc_editor->port != NULL){
    g_strfreev(midi_cc_editor->port);
  }
  
  midi_cc_editor->port = collected_specifier;
}

/**
 * ags_midi_cc_editor_midi1_load_port:
 * @midi_cc_editor: the #AgsMidiCCEditor
 *
 * MIDI 1 load port.
 * 
 * Since: 8.0.0
 */
void
ags_midi_cc_editor_midi1_load_port(AgsMidiCCEditor *midi_cc_editor)
{
  AgsMachine *machine;

  g_return_if_fail(AGS_IS_MIDI_CC_EDITOR(midi_cc_editor));

  machine = AGS_MIDI_CC_DIALOG(midi_cc_editor->parent_midi_cc_dialog)->machine;

  if(machine == NULL){
    gtk_drop_down_set_model(midi_cc_editor->midi1_port_drop_down,
			    NULL);
    
    return;
  }

  ags_midi_cc_editor_load_port(midi_cc_editor);
  
  gtk_drop_down_set_model(midi_cc_editor->midi1_port_drop_down,
			  G_LIST_MODEL(gtk_string_list_new((const gchar * const *) midi_cc_editor->port)));

}

/**
 * ags_midi_cc_editor_midi2_load_port:
 * @midi_cc_editor: the #AgsMidiCCEditor
 *
 * MIDI 2 load port.
 * 
 * Since: 8.0.0
 */
void
ags_midi_cc_editor_midi2_load_port(AgsMidiCCEditor *midi_cc_editor)
{
  AgsMachine *machine;

  g_return_if_fail(AGS_IS_MIDI_CC_EDITOR(midi_cc_editor));

  machine = AGS_MIDI_CC_DIALOG(midi_cc_editor->parent_midi_cc_dialog)->machine;

  if(machine == NULL){
    gtk_drop_down_set_model(midi_cc_editor->midi2_port_drop_down,
			    NULL);
    
    return;
  }

  ags_midi_cc_editor_load_port(midi_cc_editor);
  
  gtk_drop_down_set_model(midi_cc_editor->midi2_port_drop_down,
			  G_LIST_MODEL(gtk_string_list_new((const gchar * const *) midi_cc_editor->port)));
}

/**
 * ags_midi_cc_editor_find_midi1_control:
 * @midi_cc_editor: (transfer none): the #GList-struct containing #AgsMidiCCEditor
 * @midi1_control: the MIDI 1 control
 *
 * Find @control in @midi_cc_editor.
 * 
 * Returns: on success the matching #GList-struct otherwise %NULL
 * 
 * Since: 8.0.0
 */
GList*
ags_midi_cc_editor_find_midi1_control(GList *midi_cc_editor,
				      gchar *midi1_control)
{
  g_return_val_if_fail(midi_cc_editor != NULL, NULL);
  g_return_val_if_fail(midi1_control != NULL, NULL);

  while(midi_cc_editor != NULL){
    gchar *str;

    gboolean success;
    
    str = gtk_label_get_text(AGS_MIDI_CC_EDITOR(midi_cc_editor->data)->midi1_control_label);

    success = FALSE;
    
    if(!g_ascii_strcasecmp(str, midi1_control)){
      success = TRUE;
    }

    if(success){
      break;
    }
    
    midi_cc_editor = midi_cc_editor->next;
  }
  
  return(midi_cc_editor);
}

/**
 * ags_midi_cc_editor_find_midi2_control:
 * @midi_cc_editor: (transfer none): the #GList-struct containing #AgsMidiCCEditor
 * @midi2_control: the MIDI 2 control
 *
 * Find @control in @midi_cc_editor.
 * 
 * Returns: on success the matching #GList-struct otherwise %NULL
 * 
 * Since: 8.0.0
 */
GList*
ags_midi_cc_editor_find_midi2_control(GList *midi_cc_editor,
				      gchar *midi2_control)
{
  g_return_val_if_fail(midi_cc_editor != NULL, NULL);
  g_return_val_if_fail(midi2_control != NULL, NULL);

  while(midi_cc_editor != NULL){
    gchar *str;

    gboolean success;
    
    str = gtk_label_get_text(AGS_MIDI_CC_EDITOR(midi_cc_editor->data)->midi2_control_label);

    success = FALSE;
    
    if(!g_ascii_strcasecmp(str, midi2_control)){
      success = TRUE;
    }

    if(success){
      break;
    }
    
    midi_cc_editor = midi_cc_editor->next;
  }
  
  return(midi_cc_editor);
}

/**
 * ags_midi_cc_editor_new:
 *
 * Create a new instance of #AgsMidiCCEditor
 *
 * Returns: the new #AgsMidiCCEditor
 *
 * Since: 6.9.2
 */
AgsMidiCCEditor*
ags_midi_cc_editor_new()
{
  AgsMidiCCEditor *midi_cc_editor;

  midi_cc_editor = (AgsMidiCCEditor *) g_object_new(AGS_TYPE_MIDI_CC_EDITOR,
						    NULL);

  return(midi_cc_editor);
}
