/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/X/ags_composite_editor.h>
#include <ags/X/ags_composite_editor_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>

#include <ags/X/editor/ags_composite_edit_callbacks.h>
#include <ags/X/editor/ags_notation_edit.h>
#include <ags/X/editor/ags_sheet_edit.h>

#include <ags/X/editor/ags_scrolled_automation_edit_box.h>
#include <ags/X/editor/ags_vautomation_edit_box.h>
#include <ags/X/editor/ags_automation_edit.h>

#include <ags/X/editor/ags_scrolled_wave_edit_box.h>
#include <ags/X/editor/ags_vwave_edit_box.h>
#include <ags/X/editor/ags_wave_edit.h>

#include <ags/X/machine/ags_panel.h>
#include <ags/X/machine/ags_mixer.h>
#include <ags/X/machine/ags_spectrometer.h>
#include <ags/X/machine/ags_equalizer10.h>
#include <ags/X/machine/ags_drum.h>
#include <ags/X/machine/ags_matrix.h>
#include <ags/X/machine/ags_synth.h>
#include <ags/X/machine/ags_fm_synth.h>
#include <ags/X/machine/ags_syncsynth.h>
#include <ags/X/machine/ags_fm_syncsynth.h>
#include <ags/X/machine/ags_hybrid_synth.h>

#ifdef AGS_WITH_LIBINSTPATCH
#include <ags/X/machine/ags_ffplayer.h>
#include <ags/X/machine/ags_sf2_synth.h>
#endif

#include <ags/X/machine/ags_pitch_sampler.h>
#include <ags/X/machine/ags_sfz_synth.h>

#include <ags/X/machine/ags_audiorec.h>
#include <ags/X/machine/ags_desk.h>

#include <ags/X/machine/ags_ladspa_bridge.h>
#include <ags/X/machine/ags_dssi_bridge.h>
#include <ags/X/machine/ags_lv2_bridge.h>
#include <ags/X/machine/ags_live_dssi_bridge.h>
#include <ags/X/machine/ags_live_lv2_bridge.h>

#if defined(AGS_WITH_VST3)
#include <ags/X/machine/ags_vst3_bridge.h>
#include <ags/X/machine/ags_live_vst3_bridge.h>
#endif

#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_composite_editor_class_init(AgsCompositeEditorClass *composite_editor);
void ags_composite_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_composite_editor_init(AgsCompositeEditor *composite_editor);

void ags_composite_editor_show_all(GtkWidget *widget);
void ags_composite_editor_show(GtkWidget *widget);

AgsUUID* ags_composite_editor_get_uuid(AgsConnectable *connectable);
gboolean ags_composite_editor_has_resource(AgsConnectable *connectable);
gboolean ags_composite_editor_is_ready(AgsConnectable *connectable);
void ags_composite_editor_add_to_registry(AgsConnectable *connectable);
void ags_composite_editor_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_composite_editor_list_resource(AgsConnectable *connectable);
xmlNode* ags_composite_editor_xml_compose(AgsConnectable *connectable);
void ags_composite_editor_xml_parse(AgsConnectable *connectable,
				    xmlNode *node);
gboolean ags_composite_editor_is_connected(AgsConnectable *connectable);
void ags_composite_editor_connect(AgsConnectable *connectable);
void ags_composite_editor_disconnect(AgsConnectable *connectable);

void ags_composite_editor_get_boundary(AgsCompositeEditor *composite_editor,
				       AgsMachine *machine,
				       AgsNotation *notation,
				       guint *lower, guint *upper);
void ags_composite_editor_invert_notation(AgsCompositeEditor *composite_editor,
					  AgsMachine *machine,
					  AgsNotation *notation,
					  guint lower, guint upper);

gint ags_composite_editor_paste_notation_all(AgsCompositeEditor *composite_editor,
					     AgsMachine *machine,
					     xmlNode *notation_node,
					     AgsTimestamp *timestamp,
					     gboolean match_channel, gboolean no_duplicates,
					     guint position_x, guint position_y,
					     gboolean paste_from_position,
					     gint *last_x);
gint ags_composite_editor_paste_notation(AgsCompositeEditor *composite_editor,
					 AgsMachine *machine,
					 xmlNode *audio_node,
					 guint position_x, guint position_y,
					 gboolean paste_from_position,
					 gint *last_x);

gint ags_composite_editor_paste_automation_all(AgsCompositeEditor *composite_editor,
					       AgsNotebook *notebook,
					       AgsMachine *machine,
					       xmlNode *automation_node,
					       AgsTimestamp *timestamp,
					       gboolean match_line, gboolean no_duplicates,
					       guint position_x, guint position_y,
					       gboolean paste_from_position,
					       gint *last_x);
gint ags_composite_editor_paste_automation(AgsCompositeEditor *composite_editor,
					   AgsNotebook *notebook,
					   AgsMachine *machine,
					   xmlNode *audio_node,
					   guint position_x, guint position_y,
					   gboolean paste_from_position,
					   gint *last_x);

gint ags_composite_editor_paste_wave_all(AgsCompositeEditor *composite_editor,
					 AgsMachine *machine,
					 AgsNotebook *notebook,
					 xmlNode *wave_node,
					 AgsTimestamp *timestamp,
					 gboolean match_line,
					 gboolean paste_from_position,
					 guint64 position_x,
					 gint64 *last_x);
gint ags_composite_editor_paste_wave(AgsCompositeEditor *composite_editor,
				     AgsMachine *machine,
				     AgsNotebook *notebook,
				     xmlNode *audio_node,
				     gboolean paste_from_position,
				     guint64 position_x,
				     guint64 relative_offset);

void ags_composite_editor_real_machine_changed(AgsCompositeEditor *composite_editor,
					       AgsMachine *machine);

enum{
  MACHINE_CHANGED,
  LAST_SIGNAL,
};

static gpointer ags_composite_editor_parent_class = NULL;
static guint composite_editor_signals[LAST_SIGNAL];

/**
 * SECTION:ags_composite_editor
 * @short_description: composite editor
 * @title: AgsCompositeEditor
 * @section_id:
 * @include: ags/X/ags_composite_editor.h
 *
 * The #AgsCompositeEditor is a composite abstraction of edit implementation.
 */

GType
ags_composite_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_composite_editor = 0;

    static const GTypeInfo ags_composite_editor_info = {
      sizeof (AgsCompositeEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_composite_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCompositeEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_composite_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_composite_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_composite_editor = g_type_register_static(GTK_TYPE_BOX,
						       "AgsCompositeEditor", &ags_composite_editor_info,
						       0);
    
    g_type_add_interface_static(ags_type_composite_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_composite_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_composite_editor_class_init(AgsCompositeEditorClass *composite_editor)
{
  GtkWidgetClass *widget;
  
  ags_composite_editor_parent_class = g_type_class_peek_parent(composite_editor);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) composite_editor;

  widget->show_all = ags_composite_editor_show_all;
  widget->show = ags_composite_editor_show;
  
  /* AgsCompositeEditorClass */
  composite_editor->machine_changed = ags_composite_editor_real_machine_changed;

  /* signals */
  /**
   * AgsCompositeEditor::machine-changed:
   * @editor: the object to change machine.
   * @machine: the #AgsMachine to set
   *
   * The ::machine-changed signal notifies about changed machine.
   * 
   * Since: 3.8.0
   */
  composite_editor_signals[MACHINE_CHANGED] =
    g_signal_new("machine-changed",
                 G_TYPE_FROM_CLASS(composite_editor),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCompositeEditorClass, machine_changed),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__OBJECT,
                 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_composite_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_composite_editor_get_uuid;
  connectable->has_resource = ags_composite_editor_has_resource;

  connectable->is_ready = ags_composite_editor_is_ready;
  connectable->add_to_registry = ags_composite_editor_add_to_registry;
  connectable->remove_from_registry = ags_composite_editor_remove_from_registry;

  connectable->list_resource = ags_composite_editor_list_resource;
  connectable->xml_compose = ags_composite_editor_xml_compose;
  connectable->xml_parse = ags_composite_editor_xml_parse;

  connectable->is_connected = ags_composite_editor_is_connected;
  connectable->connect = ags_composite_editor_connect;
  connectable->disconnect = ags_composite_editor_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_composite_editor_init(AgsCompositeEditor *composite_editor)
{
  GtkViewport *viewport;
  GtkScrolledWindow *scrolled_window;
  GtkBox *vbox;
  GtkMenu *menu;
  
  AgsApplicationContext *application_context;

  gdouble gui_scale_factor;
  gint hscrollbar_height, vscrollbar_width;
  
  static gchar *dialog[] = {
    AGS_COMPOSITE_TOOLBAR_SCOPE_COMMON,
    AGS_COMPOSITE_TOOLBAR_SCOPE_NOTATION,
    NULL,
  };

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
  
  gtk_orientable_set_orientation(GTK_ORIENTABLE(composite_editor),
				 GTK_ORIENTATION_VERTICAL);

  composite_editor->flags = 0;
  composite_editor->edit = 0;

  composite_editor->version = g_strdup(AGS_COMPOSITE_EDITOR_DEFAULT_VERSION);
  composite_editor->build_id = g_strdup(AGS_COMPOSITE_EDITOR_DEFAULT_BUILD_ID);

  /* uuid */
  composite_editor->uuid = ags_uuid_alloc();
  ags_uuid_generate(composite_editor->uuid);

  /* widgets */
  composite_editor->toolbar = ags_composite_toolbar_new();

  ags_composite_toolbar_scope_create_and_connect(composite_editor->toolbar,
						 AGS_COMPOSITE_TOOLBAR_SCOPE_NOTATION);
  
  gtk_box_pack_start((GtkBox *) composite_editor,
		     (GtkWidget *) composite_editor->toolbar,
		     FALSE, FALSE,
		     0);

  /* paned */
  composite_editor->paned = (GtkPaned *) gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);

  gtk_widget_set_valign((GtkWidget *) composite_editor->paned,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) composite_editor->paned,
			GTK_ALIGN_FILL);

  gtk_box_pack_start((GtkBox *) composite_editor,
		     (GtkWidget *) composite_editor->paned,
		     TRUE, TRUE,
		     0);

  /* machine selector */
  viewport = (GtkViewport *) gtk_viewport_new(NULL,
					      NULL);
  g_object_set(viewport,
	       "shadow-type", GTK_SHADOW_NONE,
	       NULL);

  gtk_widget_set_valign((GtkWidget *) viewport,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) viewport,
			GTK_ALIGN_FILL);
  
  gtk_paned_pack1((GtkPaned *) composite_editor->paned,
		  (GtkWidget *) viewport,
		  FALSE, TRUE);

  gtk_widget_set_valign((GtkWidget *) composite_editor->paned,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) composite_editor->paned,
			GTK_ALIGN_FILL);
  
  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(viewport),
		    GTK_WIDGET(scrolled_window));

  /* machine selector */
  composite_editor->machine_selector = g_object_new(AGS_TYPE_MACHINE_SELECTOR,
						    "homogeneous", FALSE,
						    "spacing", 0,
						    NULL);

  gtk_widget_set_valign((GtkWidget *) composite_editor->machine_selector,
			GTK_ALIGN_START);
  gtk_widget_set_halign((GtkWidget *) composite_editor->machine_selector,
			GTK_ALIGN_START);
  
  composite_editor->machine_selector->flags |= AGS_MACHINE_SELECTOR_SHOW_SHIFT_PIANO;
  composite_editor->machine_selector->edit = (AGS_MACHINE_SELECTOR_EDIT_NOTATION |
					      AGS_MACHINE_SELECTOR_EDIT_AUTOMATION |
					      AGS_MACHINE_SELECTOR_EDIT_WAVE);
  
  gtk_label_set_label(composite_editor->machine_selector->label,
		      i18n("selector"));
  
  composite_editor->machine_selector->popup = ags_machine_selector_popup_new(composite_editor->machine_selector);
  g_object_set(composite_editor->machine_selector->menu_button,
	       "menu", composite_editor->machine_selector->popup,
	       NULL);
  
  gtk_container_add((GtkContainer *) scrolled_window,
		    (GtkWidget *) composite_editor->machine_selector);

  /* selected machine */
  composite_editor->selected_machine = NULL;

  /* edit */
  composite_editor->selected_edit = NULL;

  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,
		     0);

  gtk_widget_set_valign((GtkWidget *) vbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) vbox,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) vbox,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) vbox,
			 TRUE);

  gtk_paned_pack2((GtkPaned *) composite_editor->paned,
		  (GtkWidget *) vbox,
		  TRUE, TRUE);
  
  /* edit - notation edit */
  composite_editor->notation_edit = ags_composite_edit_new();      

  ags_composite_edit_set_scrollbar(composite_editor->notation_edit,
				   AGS_COMPOSITE_EDIT_SCROLLBAR_HORIZONTAL);
  ags_composite_edit_set_scrollbar(composite_editor->notation_edit,
				   AGS_COMPOSITE_EDIT_SCROLLBAR_VERTICAL);
  
  gtk_widget_set_valign((GtkWidget *) composite_editor->notation_edit,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) composite_editor->notation_edit,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) composite_editor->notation_edit,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) composite_editor->notation_edit,
			 TRUE);

  ags_composite_edit_set_channel_selector_mode(composite_editor->notation_edit,
					       AGS_COMPOSITE_EDIT_CHANNEL_SELECTOR_AUDIO_CHANNEL);

  gtk_box_pack_start(vbox,
		     composite_editor->notation_edit,
		     TRUE, TRUE,
		     0);
  
  composite_editor->notation_edit->edit_control = ags_scrolled_piano_new();
  g_object_set(AGS_SCROLLED_PIANO(composite_editor->notation_edit->edit_control)->piano,
	       "key-width", (guint) (gui_scale_factor * AGS_PIANO_DEFAULT_KEY_WIDTH),
	       "key-height", (guint) (gui_scale_factor * AGS_PIANO_DEFAULT_KEY_HEIGHT),
	       NULL);

  gtk_widget_set_valign((GtkWidget *) composite_editor->notation_edit->edit_control,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) composite_editor->notation_edit->edit_control,
			GTK_ALIGN_FILL);

  gtk_grid_attach(composite_editor->notation_edit->composite_grid,
		  (GtkWidget *) composite_editor->notation_edit->edit_control,
		  0, 2,
		  1, 1);
  
  composite_editor->notation_edit->edit = ags_notation_edit_new();
  gtk_box_pack_start(composite_editor->notation_edit->edit_box,
		     composite_editor->notation_edit->edit,
		     TRUE, TRUE,
		     0);
  
  /* edit - sheet edit */
  composite_editor->sheet_edit = ags_composite_edit_new();

  /* edit - automation edit */
  composite_editor->automation_edit = ags_composite_edit_new();
  
  ags_composite_edit_set_scrollbar(composite_editor->automation_edit,
				   AGS_COMPOSITE_EDIT_SCROLLBAR_HORIZONTAL);
  ags_composite_edit_set_scrollbar(composite_editor->automation_edit,
				   AGS_COMPOSITE_EDIT_SCROLLBAR_VERTICAL);

  gtk_widget_get_preferred_width(composite_editor->automation_edit->vscrollbar,
				 &vscrollbar_width,
				 NULL);
  gtk_widget_get_preferred_height(composite_editor->automation_edit->hscrollbar,
				  &hscrollbar_height,
				  NULL);
  
  gtk_box_pack_start(vbox,
		     composite_editor->automation_edit,
		     TRUE, TRUE,
		     0);

  /* scale */
  composite_editor->automation_edit->edit_control = ags_scrolled_scale_box_new();

  AGS_SCROLLED_SCALE_BOX(composite_editor->automation_edit->edit_control)->viewport->remaining_height = hscrollbar_height;
  
  gtk_widget_set_hexpand((GtkWidget *) AGS_SCROLLED_SCALE_BOX(composite_editor->automation_edit->edit_control)->viewport,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) AGS_SCROLLED_SCALE_BOX(composite_editor->automation_edit->edit_control)->viewport,
			 TRUE);

  AGS_SCROLLED_SCALE_BOX(composite_editor->automation_edit->edit_control)->scale_box = (AgsScaleBox *) ags_vscale_box_new();
  g_object_set(AGS_SCROLLED_SCALE_BOX(composite_editor->automation_edit->edit_control)->scale_box,
	       "fixed-scale-width", (guint) (gui_scale_factor * AGS_SCALE_BOX_DEFAULT_FIXED_SCALE_WIDTH),
	       "fixed-scale-height", (guint) (gui_scale_factor * AGS_SCALE_BOX_DEFAULT_FIXED_SCALE_HEIGHT),
	       NULL);

  gtk_widget_set_size_request((GtkWidget *) composite_editor->automation_edit->edit_control,
  			      (gint) (gui_scale_factor * AGS_SCALE_BOX_DEFAULT_FIXED_SCALE_WIDTH),
  			      -1);

  gtk_widget_set_valign((GtkWidget *) composite_editor->automation_edit->edit_control,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) composite_editor->automation_edit->edit_control,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *)  composite_editor->automation_edit->edit_control,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *)  composite_editor->automation_edit->edit_control,
			 FALSE);

  gtk_container_add(GTK_CONTAINER(AGS_SCROLLED_SCALE_BOX(composite_editor->automation_edit->edit_control)->viewport),
		    GTK_WIDGET(AGS_SCROLLED_SCALE_BOX(composite_editor->automation_edit->edit_control)->scale_box));

  gtk_grid_attach(composite_editor->automation_edit->composite_grid,
		  (GtkWidget *) composite_editor->automation_edit->edit_control,
		  0, 2,
		  1, 1);

  /* automation edit */
  composite_editor->automation_edit->edit = ags_scrolled_automation_edit_box_new();

  //  AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_editor->automation_edit->edit)->viewport->remaining_width = vscrollbar_width;
  AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_editor->automation_edit->edit)->viewport->remaining_height = hscrollbar_height;

  gtk_widget_set_hexpand((GtkWidget *) AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_editor->automation_edit->edit)->viewport,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_editor->automation_edit->edit)->viewport,
			 TRUE);

  g_signal_connect(AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_editor->automation_edit->edit)->viewport->vadjustment, "changed",
		   G_CALLBACK(ags_composite_editor_edit_viewport_vadjustment_changed_callback), composite_editor);
  
  g_signal_connect(gtk_range_get_adjustment(composite_editor->automation_edit->hscrollbar), "value-changed",
		   G_CALLBACK(ags_composite_editor_edit_hadjustment_value_changed_callback), composite_editor);
  
  g_signal_connect(gtk_range_get_adjustment(composite_editor->automation_edit->vscrollbar), "value-changed",
		   G_CALLBACK(ags_composite_editor_edit_vadjustment_value_changed_callback), composite_editor);
  
  AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_editor->automation_edit->edit)->automation_edit_box = (AgsAutomationEditBox *) ags_vautomation_edit_box_new();
  g_object_set(AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_editor->automation_edit->edit)->automation_edit_box,
	       "fixed-edit-height", (guint) (gui_scale_factor * AGS_SCALE_BOX_DEFAULT_FIXED_SCALE_HEIGHT),
	       NULL);
  
  gtk_widget_set_size_request((GtkWidget *) composite_editor->automation_edit->edit,
  			      -1,
  			      -1);

  gtk_widget_set_valign((GtkWidget *) composite_editor->automation_edit->edit,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) composite_editor->automation_edit->edit,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) composite_editor->automation_edit->edit,
  			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) composite_editor->automation_edit->edit,
  			 TRUE);
  
  gtk_container_add(AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_editor->automation_edit->edit)->viewport,
		    GTK_WIDGET(AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_editor->automation_edit->edit)->automation_edit_box));
    
  gtk_box_pack_start(composite_editor->automation_edit->edit_box,
		     composite_editor->automation_edit->edit,
		     TRUE, TRUE,
		     0);

  /* edit - wave edit */
  composite_editor->wave_edit = ags_composite_edit_new();

  ags_composite_edit_set_scrollbar(composite_editor->wave_edit,
				   AGS_COMPOSITE_EDIT_SCROLLBAR_HORIZONTAL);
  ags_composite_edit_set_scrollbar(composite_editor->wave_edit,
				   AGS_COMPOSITE_EDIT_SCROLLBAR_VERTICAL);

  gtk_widget_get_preferred_width(composite_editor->wave_edit->vscrollbar,
				 &vscrollbar_width,
				 NULL);
  gtk_widget_get_preferred_height(composite_editor->wave_edit->hscrollbar,
				  &hscrollbar_height,
				  NULL);
  
  ags_composite_edit_set_channel_selector_mode(composite_editor->wave_edit,
					       AGS_COMPOSITE_EDIT_CHANNEL_SELECTOR_AUDIO_CHANNEL);

  gtk_box_pack_start(vbox,
		     composite_editor->wave_edit,
		     TRUE, TRUE,
		     0);
  
  /* level */
  composite_editor->wave_edit->edit_control = ags_scrolled_level_box_new();
  
  AGS_SCROLLED_LEVEL_BOX(composite_editor->wave_edit->edit_control)->viewport->remaining_height = hscrollbar_height;
  
  gtk_widget_set_hexpand((GtkWidget *) AGS_SCROLLED_LEVEL_BOX(composite_editor->wave_edit->edit_control)->viewport,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) AGS_SCROLLED_LEVEL_BOX(composite_editor->wave_edit->edit_control)->viewport,
			 TRUE);

  AGS_SCROLLED_LEVEL_BOX(composite_editor->wave_edit->edit_control)->level_box = (AgsLevelBox *) ags_vlevel_box_new();
  g_object_set(AGS_SCROLLED_LEVEL_BOX(composite_editor->wave_edit->edit_control)->level_box,
	       "fixed-level-width", (guint) (gui_scale_factor * AGS_LEVEL_BOX_DEFAULT_FIXED_LEVEL_WIDTH),
	       "fixed-level-height", (guint) (gui_scale_factor * AGS_LEVEL_BOX_DEFAULT_FIXED_LEVEL_HEIGHT),
	       NULL);

  gtk_widget_set_size_request((GtkWidget *) composite_editor->wave_edit->edit_control,
  			      (gint) (gui_scale_factor * AGS_LEVEL_BOX_DEFAULT_FIXED_LEVEL_WIDTH),
  			      -1);

  gtk_widget_set_valign((GtkWidget *) composite_editor->wave_edit->edit_control,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) composite_editor->wave_edit->edit_control,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *)  composite_editor->wave_edit->edit_control,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *)  composite_editor->wave_edit->edit_control,
			 FALSE);

  gtk_container_add(GTK_CONTAINER(AGS_SCROLLED_LEVEL_BOX(composite_editor->wave_edit->edit_control)->viewport),
		    GTK_WIDGET(AGS_SCROLLED_LEVEL_BOX(composite_editor->wave_edit->edit_control)->level_box));

  gtk_grid_attach(composite_editor->wave_edit->composite_grid,
		  (GtkWidget *) composite_editor->wave_edit->edit_control,
		  0, 2,
		  1, 1);
  
  /* wave edit */
  composite_editor->wave_edit->edit = ags_scrolled_wave_edit_box_new();

  //  AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->viewport->remaining_width = vscrollbar_width;
  AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->viewport->remaining_height = hscrollbar_height;

  gtk_widget_set_hexpand((GtkWidget *) AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->viewport,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->viewport,
			 TRUE);

  g_signal_connect(AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->viewport->vadjustment, "changed",
		   G_CALLBACK(ags_composite_editor_edit_viewport_vadjustment_changed_callback), (gpointer) composite_editor);
  
  g_signal_connect(gtk_range_get_adjustment(composite_editor->wave_edit->hscrollbar), "value-changed",
		   G_CALLBACK(ags_composite_editor_edit_hadjustment_value_changed_callback), composite_editor);
  
  g_signal_connect(gtk_range_get_adjustment(composite_editor->wave_edit->vscrollbar), "value-changed",
		   G_CALLBACK(ags_composite_editor_edit_vadjustment_value_changed_callback), (gpointer) composite_editor);
  
  AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->wave_edit_box = (AgsWaveEditBox *) ags_vwave_edit_box_new();
  g_object_set(AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->wave_edit_box,
	       "fixed-edit-height", (guint) (gui_scale_factor * AGS_LEVEL_BOX_DEFAULT_FIXED_LEVEL_HEIGHT),
	       NULL);
  
  gtk_widget_set_size_request((GtkWidget *) composite_editor->wave_edit->edit,
  			      -1,
  			      -1);

  gtk_widget_set_valign((GtkWidget *) composite_editor->wave_edit->edit,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) composite_editor->wave_edit->edit,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) composite_editor->wave_edit->edit,
  			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) composite_editor->wave_edit->edit,
  			 TRUE);
  
  gtk_container_add(AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->viewport,
		    GTK_WIDGET(AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->wave_edit_box));
    
  gtk_box_pack_start(composite_editor->wave_edit->edit_box,
		     composite_editor->wave_edit->edit,
		     TRUE, TRUE,
		     0);

  gtk_adjustment_changed(AGS_SCROLLED_SCALE_BOX(composite_editor->automation_edit->edit_control)->viewport->vadjustment);
  gtk_adjustment_changed(AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_editor->automation_edit->edit)->viewport->vadjustment);

  gtk_adjustment_changed(AGS_SCROLLED_SCALE_BOX(composite_editor->automation_edit->edit_control)->viewport->vadjustment);
  gtk_adjustment_changed(AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_editor->automation_edit->edit)->viewport->vadjustment);

  gtk_adjustment_changed(AGS_SCROLLED_LEVEL_BOX(composite_editor->wave_edit->edit_control)->viewport->vadjustment);
  gtk_adjustment_changed(AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->viewport->vadjustment);
}

void
ags_composite_editor_show_all(GtkWidget *widget)
{
  AgsCompositeEditor *composite_editor;

  composite_editor = (AgsCompositeEditor *) widget;
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_composite_editor_parent_class)->show_all(widget);

  gtk_widget_hide(composite_editor->automation_edit);
  gtk_widget_hide(composite_editor->wave_edit);
}

void
ags_composite_editor_show(GtkWidget *widget)
{
  AgsCompositeEditor *composite_editor;

  composite_editor = (AgsCompositeEditor *) widget;
  
  /* call parent */ 
  GTK_WIDGET_CLASS(ags_composite_editor_parent_class)->show(widget);

  gtk_widget_hide(composite_editor->automation_edit);
  gtk_widget_hide(composite_editor->wave_edit);
}

AgsUUID*
ags_composite_editor_get_uuid(AgsConnectable *connectable)
{
  AgsCompositeEditor *composite_editor;
  
  AgsUUID *ptr;

  composite_editor = AGS_COMPOSITE_EDITOR(connectable);

  ptr = composite_editor->uuid;

  return(ptr);
}

gboolean
ags_composite_editor_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_composite_editor_is_ready(AgsConnectable *connectable)
{
  AgsCompositeEditor *composite_editor;
  
  gboolean is_ready;

  composite_editor = AGS_COMPOSITE_EDITOR(connectable);

  /* check is added */
  is_ready = ags_composite_editor_test_flags(composite_editor, AGS_COMPOSITE_EDITOR_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_composite_editor_add_to_registry(AgsConnectable *connectable)
{
  AgsCompositeEditor *composite_editor;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  composite_editor = AGS_COMPOSITE_EDITOR(connectable);

  ags_composite_editor_set_flags(composite_editor, AGS_COMPOSITE_EDITOR_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);

    entry->id = composite_editor->uuid;
    g_value_set_object(entry->entry,
		       (gpointer) composite_editor);
    
    ags_registry_add_entry(registry,
			   entry);
  }
}

void
ags_composite_editor_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_composite_editor_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_composite_editor_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_composite_editor_xml_parse(AgsConnectable *connectable,
			       xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_composite_editor_is_connected(AgsConnectable *connectable)
{
  AgsCompositeEditor *composite_editor;
  
  gboolean is_connected;

  composite_editor = AGS_COMPOSITE_EDITOR(connectable);

  /* check is connected */
  is_connected = ags_composite_editor_test_flags(composite_editor, AGS_COMPOSITE_EDITOR_CONNECTED);
  
  return(is_connected);
}

void
ags_composite_editor_connect(AgsConnectable *connectable)
{
  AgsCompositeEditor *composite_editor;
  
  composite_editor = AGS_COMPOSITE_EDITOR(connectable);

  if((AGS_COMPOSITE_EDITOR_CONNECTED & (composite_editor->flags)) != 0){
    return;
  }

  composite_editor->flags |= AGS_COMPOSITE_EDITOR_CONNECTED;

  ags_connectable_connect(AGS_CONNECTABLE(composite_editor->toolbar));
  
  ags_connectable_connect(AGS_CONNECTABLE(composite_editor->notation_edit));
  ags_connectable_connect(AGS_CONNECTABLE(composite_editor->sheet_edit));
  ags_connectable_connect(AGS_CONNECTABLE(composite_editor->automation_edit));
  ags_connectable_connect(AGS_CONNECTABLE(composite_editor->wave_edit));

  g_signal_connect(composite_editor->machine_selector, "changed",
		   G_CALLBACK(ags_composite_editor_machine_selector_changed_callback), composite_editor);
  
  //TODO:JK: implement me
}

void
ags_composite_editor_disconnect(AgsConnectable *connectable)
{
  AgsCompositeEditor *composite_editor;
  
  composite_editor = AGS_COMPOSITE_EDITOR(connectable);

  if((AGS_COMPOSITE_EDITOR_CONNECTED & (composite_editor->flags)) == 0){
    return;
  }

  composite_editor->flags &= (~AGS_COMPOSITE_EDITOR_CONNECTED);

  ags_connectable_disconnect(AGS_CONNECTABLE(composite_editor->toolbar));
  
  ags_connectable_disconnect(AGS_CONNECTABLE(composite_editor->notation_edit));
  ags_connectable_disconnect(AGS_CONNECTABLE(composite_editor->sheet_edit));
  ags_connectable_disconnect(AGS_CONNECTABLE(composite_editor->automation_edit));
  ags_connectable_disconnect(AGS_CONNECTABLE(composite_editor->wave_edit));

  //TODO:JK: implement me
}

/**
 * ags_composite_editor_test_flags:
 * @composite_editor: the #AgsCompositeEditor
 * @flags: the flags
 *
 * Test @flags to be set on @composite_editor.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.8.0
 */
gboolean
ags_composite_editor_test_flags(AgsCompositeEditor *composite_editor, guint flags)
{
  gboolean retval;  
  
  if(!AGS_IS_COMPOSITE_EDITOR(composite_editor)){
    return(FALSE);
  }
    
  /* test */
  retval = (flags & (composite_editor->flags)) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_composite_editor_set_flags:
 * @composite_editor: the #AgsCompositeEditor
 * @flags: see enum AgsCompositeEditorFlags
 *
 * Enable a feature of #AgsCompositeEditor.
 *
 * Since: 3.8.0
 */
void
ags_composite_editor_set_flags(AgsCompositeEditor *composite_editor, guint flags)
{
  if(!AGS_IS_COMPOSITE_EDITOR(composite_editor)){
    return;
  }

  /* set flags */
  composite_editor->flags |= flags;
}
    
/**
 * ags_composite_editor_unset_flags:
 * @composite_editor: the #AgsCompositeEditor
 * @flags: see enum AgsCompositeEditorFlags
 *
 * Disable a feature of AgsCompositeEditor.
 *
 * Since: 3.8.0
 */
void
ags_composite_editor_unset_flags(AgsCompositeEditor *composite_editor, guint flags)
{  
  if(!AGS_IS_COMPOSITE_EDITOR(composite_editor)){
    return;
  }

  /* unset flags */
  composite_editor->flags &= (~flags);
}

void
ags_composite_editor_real_machine_changed(AgsCompositeEditor *composite_editor,
					  AgsMachine *machine)
{
  AgsMachine *old_machine;
  
  AgsApplicationContext *application_context;

  GList *tab;
  GList *start_list, *list;

  gdouble gui_scale_factor;
  guint length;
  guint audio_channels;
  guint input_lines;
  guint i;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
  
  /* disconnect set pads - old */
  old_machine = composite_editor->selected_machine;

  if(old_machine == machine){
    return;
  }
  
  if(old_machine != NULL){
    g_object_disconnect(old_machine,
			"any_signal::resize-audio-channels",
			G_CALLBACK(ags_composite_editor_resize_audio_channels_callback),
			(gpointer) composite_editor,
			"any_signal::resize-pads",
			G_CALLBACK(ags_composite_editor_resize_pads_callback),
			(gpointer) composite_editor,
			NULL);
  }
  
  /* selected machine */
  composite_editor->selected_machine = machine;
  
  /* notation edit notebook - remove tabs */
  length = g_list_length(composite_editor->notation_edit->channel_selector->tab);
  
  for(i = 0; i < length; i++){
    ags_notebook_remove_tab(composite_editor->notation_edit->channel_selector,
			    0);
  }
  
  /* automation edit notebook - remove tabs */
  length = g_list_length(composite_editor->automation_edit->channel_selector->tab);
  
  for(i = 0; i < length; i++){
    ags_notebook_remove_tab(composite_editor->automation_edit->channel_selector,
			    0);
  }
  
  /* wave edit notebook - remove tabs */
  length = g_list_length(composite_editor->wave_edit->channel_selector->tab);
  
  for(i = 0; i < length; i++){
    ags_notebook_remove_tab(composite_editor->wave_edit->channel_selector,
			    0);
  }
  
  /* check pattern mode */
  if(AGS_IS_DRUM(machine) ||
     AGS_IS_MATRIX(machine)){
    ags_composite_edit_set_edit_mode(composite_editor->notation_edit,
				     AGS_COMPOSITE_EDIT_PATTERN_MODE);
  }else{
    ags_composite_edit_set_edit_mode(composite_editor->notation_edit,
				     AGS_COMPOSITE_EDIT_NOTATION_MODE);
  }
  
  /* notebook - add tabs */
  audio_channels = 2;
  input_lines = 2;
  
  if(machine != NULL){
    g_object_get(machine->audio,
		 "audio-channels", &audio_channels,
		 "input-lines", &input_lines,
		 NULL);
    
    for(i = 0; i < audio_channels; i++){
      ags_notebook_insert_tab(composite_editor->notation_edit->channel_selector,
			      i);

      tab = composite_editor->notation_edit->channel_selector->tab;
      gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(tab->data)->toggle,
				   TRUE);
      gtk_widget_show(AGS_NOTEBOOK_TAB(tab->data)->toggle);
    }
    
    for(i = 0; i < audio_channels; i++){
      ags_notebook_insert_tab(composite_editor->wave_edit->channel_selector,
			      i);

      tab = composite_editor->wave_edit->channel_selector->tab;
      gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(tab->data)->toggle,
				   TRUE);
      gtk_widget_show(AGS_NOTEBOOK_TAB(tab->data)->toggle);
    }
  }

  /* piano */
  if(machine != NULL){
    guint channel_count;

    /* get channel count */
    if(ags_audio_test_behaviour_flags(machine->audio, AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT)){
      g_object_get(machine->audio,
		   "input-pads", &channel_count,
		   NULL);
    }else{
      g_object_get(machine->audio,
		   "output-pads", &channel_count,
		   NULL);
    }

    /* apply channel count */
    AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->key_count = channel_count;

    g_object_set(AGS_SCROLLED_PIANO(composite_editor->notation_edit->edit_control)->piano,
		 "key-count", channel_count,
		 NULL);

    if(machine->base_note != NULL){
      g_object_set(AGS_SCROLLED_PIANO(composite_editor->notation_edit->edit_control)->piano,
		   "base-note", machine->base_note,
		   "base-key-code", machine->base_key_code,
		   NULL);
    }
  }else{
    /* apply default */
    AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->key_count = AGS_NOTATION_EDIT_DEFAULT_KEY_COUNT;
    
    g_object_set(AGS_SCROLLED_PIANO(composite_editor->notation_edit->edit_control)->piano,
		 "key-count", AGS_PIANO_DEFAULT_KEY_COUNT,
		 NULL);
  }

  /* automation edit */
  list = 
    start_list = gtk_container_get_children(AGS_SCROLLED_SCALE_BOX(composite_editor->automation_edit->edit_control)->scale_box);
  
  while(list != NULL){
    gtk_widget_destroy(list->data);

    list = list->next;
  }

  g_list_free(start_list);
  
  list = 
    start_list = gtk_container_get_children(AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_editor->automation_edit->edit)->automation_edit_box);
  
  while(list != NULL){
    gtk_widget_destroy(list->data);

    list = list->next;
  }

  g_list_free(start_list);

  /* wave edit */
  list = 
    start_list = gtk_container_get_children(AGS_SCROLLED_LEVEL_BOX(composite_editor->wave_edit->edit_control)->level_box);
  
  while(list != NULL){
    gtk_widget_destroy(list->data);

    list = list->next;
  }

  g_list_free(start_list);
  
  list = 
    start_list = gtk_container_get_children(AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->wave_edit_box);
  
  while(list != NULL){
    gtk_widget_destroy(list->data);

    list = list->next;
  }

  g_list_free(start_list);
  
  if(AGS_IS_DRUM(machine) ||
     AGS_IS_MATRIX(machine) ||
     AGS_IS_SYNCSYNTH(machine) ||
     AGS_IS_FM_SYNCSYNTH(machine) ||
     AGS_IS_HYBRID_SYNTH(machine) ||
#ifdef AGS_WITH_LIBINSTPATCH
     AGS_IS_FFPLAYER(machine) ||
     AGS_IS_SF2_SYNTH(machine) ||
#endif
     AGS_IS_PITCH_SAMPLER(machine) ||
     AGS_IS_SFZ_SYNTH(machine) ||
     AGS_IS_DSSI_BRIDGE(machine) ||
     AGS_IS_LIVE_DSSI_BRIDGE(machine) ||
     (AGS_IS_LV2_BRIDGE(machine) && (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0) ||
     AGS_IS_LIVE_LV2_BRIDGE(machine)
#if defined(AGS_WITH_VST3)
     || (AGS_IS_VST3_BRIDGE(machine) && (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0) ||
     AGS_IS_LIVE_VST3_BRIDGE(machine)
#endif
     ){
    ags_composite_toolbar_scope_create_and_connect(composite_editor->toolbar,
						   AGS_COMPOSITE_TOOLBAR_SCOPE_NOTATION);
    
    composite_editor->selected_edit = composite_editor->notation_edit;

    composite_editor->notation_edit->paste_flags = (AGS_COMPOSITE_EDIT_PASTE_MATCH_AUDIO_CHANNEL |
						    AGS_COMPOSITE_EDIT_PASTE_NO_DUPLICATES);
    
    gtk_widget_show_all(composite_editor->notation_edit);
    gtk_widget_hide(composite_editor->sheet_edit);
    gtk_widget_hide(composite_editor->automation_edit);
    gtk_widget_hide(composite_editor->wave_edit);
    
    gtk_widget_show(composite_editor->machine_selector->shift_piano);
  }else if(AGS_IS_PANEL(machine) ||
	   AGS_IS_MIXER(machine) ||
	   AGS_IS_SPECTROMETER(machine) ||
	   AGS_IS_EQUALIZER10(machine) ||
	   AGS_IS_LADSPA_BRIDGE(machine) ||
	   AGS_IS_DESK(machine) ||
	   (AGS_IS_LV2_BRIDGE(machine) && (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) == 0)
#if defined(AGS_WITH_VST3)
	   || (AGS_IS_VST3_BRIDGE(machine) && (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) == 0)
#endif
    ){
    GList *start_automation_port, *automation_port;
    
    ags_composite_toolbar_scope_create_and_connect(composite_editor->toolbar,
						   AGS_COMPOSITE_TOOLBAR_SCOPE_AUTOMATION);
    
    composite_editor->selected_edit = composite_editor->automation_edit;

    composite_editor->automation_edit->paste_flags = (AGS_COMPOSITE_EDIT_PASTE_MATCH_LINE |
						      AGS_COMPOSITE_EDIT_PASTE_NO_DUPLICATES);

    start_automation_port = g_list_copy(machine->enabled_automation_port);

    automation_port =
      start_automation_port = g_list_reverse(start_automation_port);

    while(automation_port != NULL){
      ags_composite_editor_add_automation_port(composite_editor,
					       AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type,
					       AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->control_name);

      automation_port = automation_port->next;
    }

    g_list_free(start_automation_port);

    gtk_widget_hide(composite_editor->notation_edit);
    gtk_widget_hide(composite_editor->sheet_edit);
    gtk_widget_show_all(composite_editor->automation_edit);
    gtk_widget_hide(composite_editor->wave_edit);
    
    gtk_widget_hide(composite_editor->machine_selector->shift_piano);
  }else if(AGS_IS_AUDIOREC(machine)){
    ags_composite_toolbar_scope_create_and_connect(composite_editor->toolbar,
						   AGS_COMPOSITE_TOOLBAR_SCOPE_WAVE);

    for(i = 0; i < input_lines; i++){
      AgsWaveEdit *wave_edit;
      AgsLevel *level;

      /* level */
      level = ags_level_new();
      g_object_set(level,
		   "level-width", (guint) (gui_scale_factor * AGS_LEVEL_DEFAULT_LEVEL_WIDTH),
		   "level-height", (guint) (gui_scale_factor * AGS_LEVEL_DEFAULT_LEVEL_HEIGHT),
		   NULL);
      gtk_box_pack_start(GTK_BOX(AGS_SCROLLED_LEVEL_BOX(composite_editor->wave_edit->edit_control)->level_box),
			 GTK_WIDGET(level),
			 FALSE, TRUE,
			 AGS_WAVE_EDIT_DEFAULT_PADDING);
	
      gtk_widget_show(GTK_WIDGET(level));

      /* wave edit */
      wave_edit = ags_wave_edit_new(i);
      gtk_box_pack_start(GTK_BOX(AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->wave_edit_box),
			 GTK_WIDGET(wave_edit),
			 FALSE, FALSE,
			 AGS_WAVE_EDIT_DEFAULT_PADDING);

      ags_connectable_connect(AGS_CONNECTABLE(wave_edit));
      gtk_widget_show(GTK_WIDGET(wave_edit));

      g_signal_connect(gtk_range_get_adjustment((GtkRange *) wave_edit->hscrollbar), "changed",
		       G_CALLBACK(ags_composite_editor_wave_edit_hadjustment_changed_callback), (gpointer) composite_editor);
      
      g_signal_connect(gtk_range_get_adjustment(wave_edit->hscrollbar), "changed",
		       G_CALLBACK(ags_composite_edit_hscrollbar_changed), composite_editor->wave_edit);
      
      g_signal_connect((GObject *) wave_edit->hscrollbar, "value-changed",
		       G_CALLBACK(ags_composite_editor_wave_edit_hscrollbar_value_changed), (gpointer) composite_editor);
    }
    
    composite_editor->selected_edit = composite_editor->wave_edit;
    
    composite_editor->wave_edit->paste_flags = (AGS_COMPOSITE_EDIT_PASTE_MATCH_LINE |
						AGS_COMPOSITE_EDIT_PASTE_NO_DUPLICATES);

    gtk_widget_hide(composite_editor->notation_edit);
    gtk_widget_hide(composite_editor->sheet_edit);
    gtk_widget_hide(composite_editor->automation_edit);
    gtk_widget_show_all(composite_editor->wave_edit);

    gtk_widget_show_all(composite_editor->wave_edit->edit_control);
    gtk_widget_show_all(composite_editor->wave_edit->edit);
    
    gtk_widget_hide(composite_editor->machine_selector->shift_piano);
  }else if(machine == NULL){
    ags_composite_toolbar_scope_create_and_connect(composite_editor->toolbar,
						   AGS_COMPOSITE_TOOLBAR_SCOPE_NOTATION);

    composite_editor->selected_edit = composite_editor->notation_edit;

    gtk_widget_show_all(composite_editor->notation_edit);
    gtk_widget_hide(composite_editor->sheet_edit);
    gtk_widget_hide(composite_editor->automation_edit);
    gtk_widget_hide(composite_editor->wave_edit);
    
    gtk_widget_show(composite_editor->machine_selector->shift_piano);
  }
  
  gtk_widget_queue_resize((GtkWidget *) AGS_SCROLLED_PIANO(composite_editor->notation_edit->edit_control)->piano);
  gtk_widget_queue_resize((GtkWidget *) composite_editor->notation_edit->edit_control);

  gtk_widget_queue_draw((GtkWidget *) AGS_SCROLLED_PIANO(composite_editor->notation_edit->edit_control)->piano);
  gtk_widget_queue_draw((GtkWidget *) composite_editor->notation_edit->edit_control);
  
  /* reset scrollbars */
  ags_notation_edit_reset_vscrollbar(composite_editor->notation_edit->edit);
  ags_notation_edit_reset_hscrollbar(composite_editor->notation_edit->edit);

  if(AGS_IS_AUDIOREC(machine)){
    list = 
      start_list = gtk_container_get_children(AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->wave_edit_box);

    while(list != NULL){
      ags_wave_edit_reset_vscrollbar(list->data);
      ags_wave_edit_reset_hscrollbar(list->data);

      list = list->next;
    }

    g_list_free(start_list);
  }
  
  /* redraw */
  gtk_widget_queue_draw((GtkWidget *) composite_editor->notation_edit->edit);

  /* connect set-pads - new */
  if(machine != NULL){
    g_signal_connect_after(machine, "resize-audio-channels",
			   G_CALLBACK(ags_composite_editor_resize_audio_channels_callback), composite_editor);

    g_signal_connect_after(machine, "resize-pads",
			   G_CALLBACK(ags_composite_editor_resize_pads_callback), composite_editor);
  }  
}

/**
 * ags_composite_editor_machine_changed:
 * @composite_editor: the #AgsCompositeEditor
 * @machine: the new #AgsMachine
 *
 * Is emitted as machine changed of @composite_editor.
 *
 * Since: 3.8.0
 */
void
ags_composite_editor_machine_changed(AgsCompositeEditor *composite_editor,
				     AgsMachine *machine)
{
  g_return_if_fail(AGS_IS_COMPOSITE_EDITOR(composite_editor));

  g_object_ref((GObject *) composite_editor);
  g_signal_emit((GObject *) composite_editor,
		composite_editor_signals[MACHINE_CHANGED], 0,
		machine);
  g_object_unref((GObject *) composite_editor);
}

/**
 * ags_composite_editor_add_automation_port:
 * @composite_editor: the #AgsCompositeEditor
 * @channel_type: either G_TYPE_NONE, AGS_TYPE_OUTPUT or AGS_TYPE_INPUT
 * @control_name: the control name
 *
 * Add automation port @control_name to @composite_editor.
 *
 * Since: 3.12.2
 */
void
ags_composite_editor_add_automation_port(AgsCompositeEditor *composite_editor,
					 GType channel_type,
					 gchar *control_name)
{
  AgsMachine *machine;
  AgsScale *scale;
  AgsAutomationEdit *automation_edit;

  AgsChannel *start_channel, *channel;
  AgsPluginPort *plugin_port;

  AgsApplicationContext *application_context;
  
  GList *start_automation, *automation;
  GList *start_port, *port;

  gdouble gui_scale_factor;

  gdouble upper, lower;
  gdouble default_value;

  GValue *upper_value;
  GValue *lower_value;
  GValue *value;
  
  if(!AGS_IS_COMPOSITE_EDITOR(composite_editor)){
    return;
  }

  machine = composite_editor->selected_machine;

  if(machine == NULL){
    return;
  }
  
  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  start_port = NULL;

  if(channel_type == G_TYPE_NONE){
    GList *current_start_port;

    start_port = 
      current_start_port = ags_audio_collect_all_audio_ports_by_specifier_and_context(machine->audio,
										      control_name,
										      TRUE);
    
    current_start_port = ags_audio_collect_all_audio_ports_by_specifier_and_context(machine->audio,
										    control_name,
										    FALSE);

    if(start_port != NULL){
      start_port = g_list_concat(start_port,
				 current_start_port);
    }else{
      start_port = current_start_port;
    }
  }else if(channel_type == AGS_TYPE_OUTPUT){
    GList *current_start_port;

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
      
      current_start_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(channel,
											  control_name,
											  TRUE);
      if(start_port != NULL){
	start_port = g_list_concat(start_port,
				   current_start_port);
      }else{
	start_port = current_start_port;
      }
      
      current_start_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(channel,
											  control_name,
											  FALSE);

      if(start_port != NULL){
	start_port = g_list_concat(start_port,
				   current_start_port);
      }else{
	start_port = current_start_port;
      }

      /* iterate */
      next = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next;
    }

    /* unref */
    if(start_channel != NULL){
      g_object_unref(start_channel);
    }
  }else if(channel_type == AGS_TYPE_INPUT){
    GList *current_start_port;

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
      
      current_start_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(channel,
											  control_name,
											  TRUE);
      if(start_port != NULL){
	start_port = g_list_concat(start_port,
				   current_start_port);
      }else{
	start_port = current_start_port;
      }
      
      current_start_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(channel,
											  control_name,
											  FALSE);

      if(start_port != NULL){
	start_port = g_list_concat(start_port,
				   current_start_port);
      }else{
	start_port = current_start_port;
      }

      /* iterate */
      next = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next;
    }

    /* unref */
    if(start_channel != NULL){
      g_object_unref(start_channel);
    }
  }
  
  start_automation = NULL;
  
  g_object_get(machine->audio,
	       "automation", &start_automation,
	       NULL);

  automation = start_automation;
  
  /* scale */
  scale = ags_scale_new();
  g_object_set(scale,
	       "scale-width", (guint) (gui_scale_factor * AGS_SCALE_DEFAULT_SCALE_WIDTH),
	       "scale-height", (guint) (gui_scale_factor * AGS_SCALE_DEFAULT_SCALE_HEIGHT),
	       NULL);
  
  /* get some fields */
  plugin_port = NULL;

  upper_value = NULL;
  lower_value = NULL;

  value = NULL;
  
  if(start_port != NULL){
    g_object_get(start_port->data,
		 "plugin-port", &plugin_port,
		 NULL);   
  }

  if(plugin_port != NULL){
    g_object_get(plugin_port,
		 "upper-value", &upper_value,
		 "lower-value", &lower_value,
		 "default-value", &value,
		 NULL);

    if(G_VALUE_HOLDS_DOUBLE(value)){
      upper = g_value_get_double(upper_value);
      lower = g_value_get_double(lower_value);
	  
      default_value = g_value_get_double(value);
    }else{
      upper = g_value_get_float(upper_value);
      lower = g_value_get_float(lower_value);
	  
      default_value = g_value_get_float(value);
    }

    g_object_unref(plugin_port);
  }else{
    upper = 1.0;
    lower = 0.0;
      
    default_value = 1.0;
  }
  
  /* apply scale */
  g_object_set(scale,
	       "control-name", control_name,
	       "upper", upper,
	       "lower", lower,
	       "default-value", default_value,
	       NULL);
  
  gtk_box_pack_start((GtkBox *) AGS_SCROLLED_SCALE_BOX(composite_editor->automation_edit->edit_control)->scale_box,
		     (GtkWidget *) scale,
		     FALSE, FALSE,
		     AGS_AUTOMATION_EDIT_DEFAULT_PADDING);
  
  gtk_widget_show((GtkWidget *) scale);

  /* automation edit */
  automation_edit = ags_automation_edit_new();

  g_object_set(automation_edit,
	       "channel-type", channel_type,
	       "control-specifier", control_name,
	       "control-name", control_name,
	       "upper", upper,
	       "lower", lower,
	       "default-value", default_value,
	       NULL);

  if(plugin_port != NULL &&
     ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_LOGARITHMIC)){
    automation_edit->flags |= AGS_AUTOMATION_EDIT_LOGARITHMIC;
  }
  
  if(plugin_port != NULL &&
     ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_INTEGER)){
    automation_edit->flags |= AGS_AUTOMATION_EDIT_INTEGER;
  }
  
  if(plugin_port != NULL &&
     ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_TOGGLED)){
    automation_edit->flags |= AGS_AUTOMATION_EDIT_TOGGLED;
  }
  
  gtk_box_pack_start((GtkBox *) AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_editor->automation_edit->edit)->automation_edit_box,
		     (GtkWidget *) automation_edit,
		     FALSE, FALSE,
		     AGS_AUTOMATION_EDIT_DEFAULT_PADDING);

  ags_connectable_connect(AGS_CONNECTABLE(automation_edit));
  gtk_widget_show((GtkWidget *) automation_edit);

  g_signal_connect(gtk_range_get_adjustment((GtkRange *) automation_edit->hscrollbar), "changed",
		   G_CALLBACK(ags_composite_editor_automation_edit_hadjustment_changed_callback), (gpointer) composite_editor);
  
  g_signal_connect(gtk_range_get_adjustment(automation_edit->hscrollbar), "changed",
		   G_CALLBACK(ags_composite_edit_hscrollbar_changed), composite_editor->automation_edit);
  
  g_signal_connect((GObject *) automation_edit->hscrollbar, "value-changed",
		   G_CALLBACK(ags_composite_editor_automation_edit_hscrollbar_value_changed), (gpointer) composite_editor);
  
  
  /* unset bypass */
  ags_audio_add_automation_port(machine->audio, control_name);
    
  automation = start_automation;
    
  while((automation = ags_automation_find_channel_type_with_control_name(automation,
									 channel_type, control_name)) != NULL){
    ags_automation_unset_flags(automation->data,
			       AGS_AUTOMATION_BYPASS);

    automation = automation->next;
  }

  ags_automation_edit_reset_vscrollbar(composite_editor->automation_edit->edit);
  ags_automation_edit_reset_hscrollbar(composite_editor->automation_edit->edit);
  
  g_list_free_full(start_automation,
		   g_object_unref);

  g_list_free_full(start_port,
		   g_object_unref);
}

/**
 * ags_composite_editor_remove_automation_port:
 * @composite_editor: the #AgsCompositeEditor
 * @channel_type: either G_TYPE_NONE, AGS_TYPE_OUTPUT or AGS_TYPE_INPUT
 * @control_name: the control name
 *
 * Add automation port @control_name to @composite_editor.
 *
 * Since: 3.12.2
 */
void
ags_composite_editor_remove_automation_port(AgsCompositeEditor *composite_editor,
					    GType channel_type,
					    gchar *control_name)
{
  AgsMachine *machine;

  GList *start_automation, *automation;
  GList *start_list, *list;
  
  gint nth;
  gboolean success;
  
  if(!AGS_IS_COMPOSITE_EDITOR(composite_editor)){
    return;
  }

  machine = composite_editor->selected_machine;

  if(machine == NULL){
    return;
  }

  start_automation = NULL;

  g_object_get(machine->audio,
	       "automation", &start_automation,
	       NULL);
  
  nth = -1;
  success = FALSE;

  list = 
    start_list = gtk_container_get_children((GtkContainer *) AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_editor->automation_edit->edit)->automation_edit_box);

  while(list != NULL){
    nth++;
      
    if(AGS_AUTOMATION_EDIT(list->data)->channel_type == channel_type &&
       !g_strcmp0(AGS_AUTOMATION_EDIT(list->data)->control_name,
		  control_name)){
      gtk_widget_destroy(list->data);

      success = TRUE;

      break;
    }
	
    list = list->next;
  }

  g_list_free(start_list);

  if(success){
    start_list = gtk_container_get_children((GtkContainer *) AGS_SCROLLED_SCALE_BOX(composite_editor->automation_edit->edit_control)->scale_box);

    list = g_list_nth(start_list,
		      nth);

    gtk_widget_destroy(list->data);
      
    g_list_free(start_list);
  }
    
  /* set bypass */
  ags_audio_remove_automation_port(machine->audio, control_name);

  automation = start_automation;
    
  while((automation = ags_automation_find_channel_type_with_control_name(automation,
									 channel_type, control_name)) != NULL){
    ags_automation_set_flags(automation->data,
			     AGS_AUTOMATION_BYPASS);

    automation = automation->next;
  }
    
  g_list_free_full(start_automation,
		   g_object_unref);
}

/**
 * ags_composite_editor_select_all:
 * @composite_editor: the #AgsCompositeEditor
 *
 * Paste to @composite_editor from clipboard.
 * 
 * Since: 3.12.0
 */
void
ags_composite_editor_select_all(AgsCompositeEditor *composite_editor)
{
  AgsMachine *machine;
      
  if(!AGS_IS_COMPOSITE_EDITOR(composite_editor)){
    return;
  }

  machine = composite_editor->selected_machine;

  if(machine == NULL){
    return;
  }

  if(composite_editor->selected_edit == composite_editor->notation_edit){  
    GList *start_notation, *notation;

    guint audio_channel;
    gint i;

    /* get notation */
    g_object_get(machine->audio,
		 "notation", &start_notation,
		 NULL);

    /* select all matching channel selector */
    i = 0;
    
    while((i = ags_notebook_next_active_tab(composite_editor->notation_edit->channel_selector,
					    i)) != -1){
      notation = start_notation;
      
      while(notation != NULL){
	g_object_get(notation->data,
		     "audio-channel", &audio_channel,
		     NULL);

	if(i != audio_channel){	
	  notation = notation->next;

	  continue;
	}
	
	ags_notation_add_all_to_selection(AGS_NOTATION(notation->data));
	
	notation = notation->next;
      }

      /* iterate */
      i++;
    }

    gtk_widget_queue_draw(composite_editor->notation_edit->edit);
    
    g_list_free_full(start_notation,
		     (GDestroyNotify) g_object_unref);
  }else if(composite_editor->selected_edit == composite_editor->sheet_edit){
    //TODO:JK: implement me
  }else if(composite_editor->selected_edit == composite_editor->automation_edit){
    AgsNotebook *notebook;
  
    GList *start_automation, *automation;

    gint i;
    
    notebook = composite_editor->automation_edit->channel_selector;

    start_automation = NULL;

    g_object_get(machine->audio,
		 "automation", &start_automation,
		 NULL);

     i = 0;
    
    while(notebook == NULL ||
	  (i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){
      automation = start_automation;
      
      while(automation != NULL){
	GType channel_type;
	
	gchar *control_name;

	guint line;
	
	line = 0;
	channel_type = G_TYPE_NONE;

	control_name = NULL;
	
	g_object_get(automation->data,
		     "line", &line,
		     "channel-type", &channel_type,
		     "control-name", &control_name,
		     NULL);

	if(i != line ||
	   channel_type != AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type ||
	   (!g_strcmp0(control_name,
		       AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name)) != TRUE){
	  automation = automation->next;

	  continue;
	}
	
	ags_automation_add_all_to_selection(AGS_AUTOMATION(automation->data));
	
	automation = automation->next;
      }

      if(notebook == NULL){
	break;
      }

      i++;
    }

    gtk_widget_queue_draw(AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit));
    
    g_list_free_full(start_automation,
		     g_object_unref);
  }else if(composite_editor->selected_edit == composite_editor->wave_edit){
    AgsNotebook *notebook;
  
    GList *start_wave, *wave;

    guint samplerate;
    guint64 relative_offset;
    gint i;
    
    notebook = composite_editor->wave_edit->channel_selector;

    g_object_get(machine->audio,
		 "wave", &start_wave,
		 "samplerate", &samplerate,
		 NULL);

    relative_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * samplerate;

    i = 0;
    
    while(notebook == NULL ||
	  (i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){
      wave = start_wave;
      
      while(wave != NULL){
	guint line;
	
	line = 0;
	
	g_object_get(wave->data,
		     "line", &line,
		     NULL);

	if(i != line){	
	  wave = wave->next;

	  continue;
	}

	ags_wave_add_all_to_selection(AGS_WAVE(wave->data));
	
	wave = wave->next;
      }

      i++;
    }

    gtk_widget_queue_draw(composite_editor->wave_edit->focused_edit);
    
    g_list_free_full(start_wave,
		     g_object_unref);
  }  
}
  
gint
ags_composite_editor_paste_notation_all(AgsCompositeEditor *composite_editor,
					AgsMachine *machine,
					xmlNode *notation_node,
					AgsTimestamp *timestamp,
					gboolean match_channel, gboolean no_duplicates,
					guint position_x, guint position_y,
					gboolean paste_from_position,
					gint *last_x)
{    
  AgsNotation *current_notation;
		
  GList *start_notation, *notation;
    
  gint first_x;
  guint current_x;
  gint i;

  first_x = -1;
    
  /*  */
  i = 0;
		
  while((i = ags_notebook_next_active_tab(composite_editor->notation_edit->channel_selector,
					  i)) != -1){
    g_object_get(machine->audio,
		 "notation", &start_notation,
		 NULL);
      
    notation = ags_notation_find_near_timestamp(start_notation, i,
						     timestamp);

    if(notation == NULL){
      current_notation = ags_notation_new((GObject *) machine->audio,
				  i);
      current_notation->timestamp->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;
	
      ags_audio_add_notation(machine->audio,
			     (GObject *) current_notation);
    }else{
      current_notation = AGS_NOTATION(notation->data);
    }
		  
    g_list_free_full(start_notation,
		     (GDestroyNotify) g_object_unref);

    if(paste_from_position){
      xmlNode *child;

      guint x_boundary;
	  
      ags_notation_insert_from_clipboard_extended(current_notation,
						  notation_node,
						  TRUE, position_x,
						  TRUE, position_y,
						  match_channel, no_duplicates);
		    
      /* get boundaries */
      child = notation_node->children;
      current_x = 0;
	  
      while(child != NULL){
	if(child->type == XML_ELEMENT_NODE){
	  if(!xmlStrncmp(child->name,
			 BAD_CAST "note",
			 5)){
	    guint tmp;

	    tmp = g_ascii_strtoull(xmlGetProp(child,
					      BAD_CAST "x1"),
				   NULL,
				   10);

	    if(tmp > current_x){
	      current_x = tmp;
	    }
	  }
	}

	child = child->next;
      }

      x_boundary = g_ascii_strtoull(xmlGetProp(notation_node,
					       BAD_CAST "x_boundary"),
				    NULL,
				    10);


      if(first_x == -1 || x_boundary < first_x){
	first_x = x_boundary;
      }
	  
      if(position_x > x_boundary){
	current_x += (position_x - x_boundary);
      }else{
	current_x -= (x_boundary - position_x);
      }
	  
      if(current_x > last_x[0]){
	last_x[0] = current_x;
      }	
    }else{
      xmlNode *child;

      ags_notation_insert_from_clipboard(current_notation,
					 notation_node,
					 FALSE, 0,
					 FALSE, 0);

      /* get boundaries */
      child = notation_node->children;
      current_x = 0;
	  
      while(child != NULL){
	if(child->type == XML_ELEMENT_NODE){
	  if(!xmlStrncmp(child->name,
			 BAD_CAST "note",
			 5)){
	    guint tmp;

	    tmp = g_ascii_strtoull(xmlGetProp(child,
					      BAD_CAST "x1"),
				   NULL,
				   10);

	    if(tmp > current_x){
	      current_x = tmp;
	    }
	  }
	}

	child = child->next;
      }

      if(current_x > last_x[0]){
	last_x[0] = current_x;
      }
    }

    /* iterate */
    i++;
  }

  return(first_x);
}

gint
ags_composite_editor_paste_notation(AgsCompositeEditor *composite_editor,
				    AgsMachine *machine,
				    xmlNode *audio_node,
				    guint position_x, guint position_y,
				    gboolean paste_from_position,
				    gint *last_x)
{
  AgsTimestamp *timestamp;

  xmlNode *notation_list_node, *notation_node;
  xmlNode *timestamp_node;

  gint first_x;
  gboolean match_channel, no_duplicates;

  first_x = -1;
    
  match_channel = ((AGS_COMPOSITE_EDIT_PASTE_MATCH_AUDIO_CHANNEL & (composite_editor->notation_edit->paste_flags)) != 0) ? TRUE: FALSE;
  no_duplicates = ((AGS_COMPOSITE_EDIT_PASTE_NO_DUPLICATES & (composite_editor->notation_edit->paste_flags)) != 0) ? TRUE: FALSE;

  /* timestamp */
  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;
    
  ags_timestamp_set_ags_offset(timestamp,
			       0);

  /* paste notation */
  notation_list_node = audio_node->children;

  while(notation_list_node != NULL){
    if(notation_list_node->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(notation_list_node->name,
		     BAD_CAST "notation-list",
		     14)){
	notation_node = notation_list_node->children;
	  
	while(notation_node != NULL){
	  if(notation_node->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(notation_node->name,
			   BAD_CAST "notation",
			   9)){
	      guint64 offset;
		
	      timestamp_node = notation_node->children;
	      offset = 0;
	  
	      while(timestamp_node != NULL){
		if(timestamp_node->type == XML_ELEMENT_NODE){
		  if(!xmlStrncmp(timestamp_node->name,
				 BAD_CAST "timestamp",
				 10)){
		    offset = g_ascii_strtoull((gchar *) xmlGetProp(timestamp_node,
								   BAD_CAST "offset"),
					      NULL,
					      10);
		      
		    break;
		  }
		}

		timestamp_node = timestamp_node->next;
	      }     

	      /* 1st attempt */
	      timestamp->timer.ags_offset.offset = (guint64) AGS_NOTATION_DEFAULT_OFFSET * floor((double) position_x / (double) AGS_NOTATION_DEFAULT_OFFSET);
		
	      first_x = ags_composite_editor_paste_notation_all(composite_editor,
								machine,
								notation_node,
								timestamp,
								match_channel, no_duplicates,
								position_x, position_y,
								paste_from_position,
								last_x);

	      /* 2nd attempt */
	      timestamp->timer.ags_offset.offset += AGS_NOTATION_DEFAULT_OFFSET;

	      ags_composite_editor_paste_notation_all(composite_editor,
						      machine,
						      notation_node,
						      timestamp,
						      match_channel, no_duplicates,
						      position_x, position_y,
						      paste_from_position,
						      last_x);
	    }
	  }

	  notation_node = notation_node->next;
	}	  
      }
    }

    notation_list_node = notation_list_node->next;
  }    

  g_object_unref(timestamp);

  return(first_x);
}

gint
ags_composite_editor_paste_automation_all(AgsCompositeEditor *composite_editor,
					  AgsNotebook *notebook,
					  AgsMachine *machine,
					  xmlNode *automation_node,
					  AgsTimestamp *timestamp,
					  gboolean match_line, gboolean no_duplicates,
					  guint position_x, guint position_y,
					  gboolean paste_from_position,
					  gint *last_x)
{    
  AgsChannel *start_output, *start_input;
  AgsChannel *nth_channel;
  AgsAutomation *automation;
  
  GList *start_list_automation, *list_automation;
    
  guint first_x;
  guint current_x;
  gint i;

  first_x = -1;

  /* get some fields */
  start_output = NULL;
  start_input = NULL;

  g_object_get(machine->audio,
	       "output", &start_output,
	       "input", &start_input,
	       NULL);
    
  /*  */
  i = 0;
		
  while(notebook == NULL ||
	(i = ags_notebook_next_active_tab(notebook,
					  i)) != -1){		  
    g_object_get(machine->audio,
		 "automation", &start_list_automation,
		 NULL);
      
    list_automation = ags_automation_find_near_timestamp_extended(start_list_automation, i,
								  AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type, AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name,
								  timestamp);

    if(list_automation == NULL){
      GList *start_play_port, *play_port;
      GList *start_recall_port, *recall_port;
	
      play_port = NULL;
      recall_port = NULL;

      if(AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type == AGS_TYPE_OUTPUT){
	nth_channel = ags_channel_nth(start_output,
				      i);
	  
	play_port =
	  start_play_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(nth_channel,
											   AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name,
											   TRUE);

	recall_port =
	  start_recall_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(nth_channel,
											     AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name,
											     FALSE);

	if(nth_channel != NULL){
	  g_object_unref(nth_channel);
	}
      }else if(AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type == AGS_TYPE_INPUT){
	nth_channel = ags_channel_nth(start_input,
				      i);

	play_port =
	  start_play_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(nth_channel,
											   AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name,
											   TRUE);

	recall_port =
	  start_recall_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(nth_channel,
											     AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name,
											     FALSE);

	if(nth_channel != NULL){
	  g_object_unref(nth_channel);
	}
      }else{
	play_port =
	  start_play_port = ags_audio_collect_all_audio_ports_by_specifier_and_context(machine->audio,
										       AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name,
										       TRUE);
	
	recall_port =
	  start_recall_port = ags_audio_collect_all_audio_ports_by_specifier_and_context(machine->audio,
											 AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name,
											 FALSE);
      }

      /* play port */
      while(play_port != NULL){
	AgsPort *current_port;
	
	current_port = play_port->data;

	automation = ags_automation_new(G_OBJECT(machine->audio),
					i,
					AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type,
					AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name);
	automation->timestamp->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;

	g_object_set(automation,
		     "port", current_port,
		     NULL);
	
	/* add to audio */
	ags_audio_add_automation(machine->audio,
				 (GObject *) automation);

	/* add to port */
	ags_port_add_automation(current_port,
				(GObject *) automation);
	  
	/* iterate */
	play_port = play_port->next;
      }

      g_list_free_full(start_play_port,
		       g_object_unref);
      
      /* recall port */
      if(recall_port != NULL){
	AgsPort *current_port;
	
	current_port = recall_port->data;

	automation = ags_automation_new(G_OBJECT(machine->audio),
					i,
					AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type,
					AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name);
	automation->timestamp->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;

	g_object_set(automation,
		     "port", current_port,
		     NULL);
	
	/* add to audio */
	ags_audio_add_automation(machine->audio,
				 (GObject *) automation);

	/* add to port */
	ags_port_add_automation(current_port,
				(GObject *) automation);
	  
	/* iterate */
	recall_port = recall_port->next;
      }

      g_list_free_full(start_recall_port,
		       g_object_unref);
    }else{
      automation = AGS_AUTOMATION(list_automation->data);
    }

    g_list_free_full(start_list_automation,
		     g_object_unref);
      
    g_object_get(machine->audio,
		 "automation", &start_list_automation,
		 NULL);

    list_automation = start_list_automation;
      
    while((list_automation = ags_automation_find_near_timestamp_extended(list_automation, i,
									 AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type, AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name,
									 timestamp)) != NULL){
      automation = list_automation->data;
	
      if(paste_from_position){
	xmlNode *child;

	guint x_boundary;
	  
	ags_automation_insert_from_clipboard_extended(automation,
						      automation_node,
						      TRUE, position_x,
						      TRUE, position_y,
						      match_line, no_duplicates);

	/* get boundaries */
	child = automation_node->children;
	current_x = 0;
	  
	while(child != NULL){
	  if(child->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(child->name,
			   BAD_CAST "note",
			   5)){
	      guint tmp;

	      tmp = g_ascii_strtoull((gchar *) xmlGetProp(child,
							  BAD_CAST "x"),
				     NULL,
				     10);

	      if(tmp > current_x){
		current_x = tmp;
	      }
	    }
	  }

	  child = child->next;
	}

	x_boundary = g_ascii_strtoull((gchar *) xmlGetProp(automation_node,
							   BAD_CAST "x_boundary"),
				      NULL,
				      10);


	if(first_x == -1 || x_boundary < first_x){
	  first_x = x_boundary;
	}
	  
	if(position_x > x_boundary){
	  current_x += (position_x - x_boundary);
	}else{
	  current_x -= (x_boundary - position_x);
	}
	  
	if(current_x > last_x[0]){
	  last_x[0] = current_x;
	}	
      }else{
	xmlNode *child;

	ags_automation_insert_from_clipboard(automation,
					     automation_node,
					     FALSE, 0,
					     FALSE, 0);

	/* get boundaries */
	child = automation_node->children;
	current_x = 0;
	  
	while(child != NULL){
	  if(child->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(child->name,
			   BAD_CAST "note",
			   5)){
	      guint tmp;

	      tmp = g_ascii_strtoull((gchar *) xmlGetProp(child,
							  BAD_CAST "x"),
				     NULL,
				     10);

	      if(tmp > current_x){
		current_x = tmp;
	      }
	    }
	  }

	  child = child->next;
	}

	if(current_x > last_x[0]){
	  last_x[0] = current_x;
	}
      }

      list_automation = list_automation->next;
    }

    g_list_free_full(start_list_automation,
		     g_object_unref);
      
    if(notebook == NULL){
      break;
    }
		  
    i++;
  }

  /* unref */
  if(start_output != NULL){
    g_object_unref(start_output);
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(first_x);
}
  
gint
ags_composite_editor_paste_automation(AgsCompositeEditor *composite_editor,
				      AgsNotebook *notebook,
				      AgsMachine *machine,
				      xmlNode *audio_node,
				      guint position_x, guint position_y,
				      gboolean paste_from_position,
				      gint *last_x)
{
  AgsTimestamp *timestamp;

  xmlNode *automation_list_node, *automation_node;
  xmlNode *timestamp_node;

  guint first_x;
  gboolean match_line, no_duplicates;

  first_x = -1;
    
  match_line = ((AGS_COMPOSITE_EDIT_PASTE_MATCH_LINE & (composite_editor->automation_edit->paste_flags)) != 0) ? TRUE: FALSE;
  no_duplicates = ((AGS_COMPOSITE_EDIT_PASTE_NO_DUPLICATES & (composite_editor->automation_edit->paste_flags)) != 0) ? TRUE: FALSE;
    
  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;
    
  timestamp->timer.ags_offset.offset = 0;
    
  /* paste automation */
  automation_list_node = audio_node->children;

  while(automation_list_node != NULL){
    if(automation_list_node->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(automation_list_node->name,
		     BAD_CAST "automation-list",
		     14)){
	automation_node = automation_list_node->children;
	  
	while(automation_node != NULL){
	  if(automation_node->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(automation_node->name,
			   BAD_CAST "automation",
			   9)){
	      guint64 offset;
		
	      timestamp_node = automation_node->children;
	      offset = 0;
	  
	      while(timestamp_node != NULL){
		if(timestamp_node->type == XML_ELEMENT_NODE){
		  if(!xmlStrncmp(timestamp_node->name,
				 BAD_CAST "timestamp",
				 10)){
		    offset = g_ascii_strtoull((gchar *) xmlGetProp(timestamp_node,
								   BAD_CAST "offset"),
					      NULL,
					      10);
		      
		    break;
		  }
		}

		timestamp_node = timestamp_node->next;
	      }     
		
	      /* 1st attempt */
	      timestamp->timer.ags_offset.offset = (guint64) AGS_AUTOMATION_DEFAULT_OFFSET * floor((double) position_x / (double) AGS_AUTOMATION_DEFAULT_OFFSET);
		
	      first_x = ags_composite_editor_paste_automation_all(composite_editor,
								  notebook,
								  machine,
								  automation_node,
								  timestamp,
								  match_line, no_duplicates,
								  position_x, position_y,
								  paste_from_position,
								  last_x);

	      /* 2nd attempt */
	      timestamp->timer.ags_offset.offset += AGS_AUTOMATION_DEFAULT_OFFSET;

	      ags_composite_editor_paste_automation_all(composite_editor,
							notebook,
							machine,
							automation_node,
							timestamp,
							match_line, no_duplicates,
							position_x, position_y,
							paste_from_position,
							last_x);
		
	    }
	  }

	  automation_node = automation_node->next;
	}	  
      }
    }

    automation_list_node = automation_list_node->next;
  }    

  g_object_unref(timestamp);

  return(first_x);
}

gint
ags_composite_editor_paste_wave_all(AgsCompositeEditor *composite_editor,
				    AgsMachine *machine,
				    AgsNotebook *notebook,
				    xmlNode *wave_node,
				    AgsTimestamp *timestamp,
				    gboolean match_line,
				    gboolean paste_from_position,
				    guint64 position_x,
				    gint64 *last_x)
{    
  AgsWave *wave;
		
  GList *start_list_wave, *list_wave;
    
  guint64 first_x;
  guint64 current_x;
  gint i;

  first_x = -1;

  /*  */
  i = 0;
		
  while((i = ags_notebook_next_active_tab(notebook,
					  i)) != -1){		  
    g_object_get(machine->audio,
		 "wave", &start_list_wave,
		 NULL);
      
    list_wave = ags_wave_find_near_timestamp(start_list_wave, i,
					     timestamp);

    if(list_wave == NULL){
      wave = ags_wave_new((GObject *) machine->audio,
			  i);
      wave->timestamp->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;
	
      /* add to audio */
      ags_audio_add_wave(machine->audio,
			 (GObject *) wave);
    }else{
      wave = AGS_WAVE(list_wave->data);
    }

    if(paste_from_position){
      xmlNode *child;

      guint64 x_boundary;
	  
      ags_wave_insert_from_clipboard_extended(wave,
					      wave_node,
					      TRUE, position_x,
					      0.0, 0,
					      match_line, FALSE);

      /* get boundaries */
      child = wave_node->children;
      current_x = 0;
	  
      while(child != NULL){
	if(child->type == XML_ELEMENT_NODE){
	  if(!xmlStrncmp(child->name,
			 BAD_CAST "buffer",
			 5)){
	    guint64 tmp;

	    tmp = g_ascii_strtoull(xmlGetProp(child,
					      BAD_CAST "x"),
				   NULL,
				   10);

	    if(tmp > current_x){
	      current_x = tmp;
	    }
	  }
	}

	child = child->next;
      }

      x_boundary = g_ascii_strtoull(xmlGetProp(wave_node,
					       BAD_CAST "x-boundary"),
				    NULL,
				    10);


      if(first_x == -1 || x_boundary < first_x){
	first_x = x_boundary;
      }
	  
      if(position_x > x_boundary){
	current_x += (position_x - x_boundary);
      }else{
	current_x -= (x_boundary - position_x);
      }
	  
      if(current_x > last_x[0]){
	last_x[0] = current_x;
      }	
    }else{
      xmlNode *child;

      ags_wave_insert_from_clipboard_extended(wave,
					      wave_node,
					      FALSE, 0,
					      0.0, 0,
					      match_line, FALSE);

      /* get boundaries */
      child = wave_node->children;
      current_x = 0;
	  
      while(child != NULL){
	if(child->type == XML_ELEMENT_NODE){
	  if(!xmlStrncmp(child->name,
			 BAD_CAST "buffer",
			 5)){
	    guint64 tmp;

	    tmp = g_ascii_strtoull(xmlGetProp(child,
					      BAD_CAST "x"),
				   NULL,
				   10);

	    if(tmp > current_x){
	      current_x = tmp;
	    }
	  }
	}

	child = child->next;
      }

      if(current_x > last_x[0]){
	last_x[0] = current_x;
      }
    }

    g_list_free_full(start_list_wave,
		     g_object_unref);
      
    i++;
  }

  return(first_x);
}
  
gint
ags_composite_editor_paste_wave(AgsCompositeEditor *composite_editor,
				AgsMachine *machine,
				AgsNotebook *notebook,
				xmlNode *audio_node,
				gboolean paste_from_position,
				guint64 position_x,
				guint64 relative_offset)
{
  AgsTimestamp *timestamp;

  xmlNode *wave_list_node, *wave_node;
  xmlNode *timestamp_node;

  gint64 first_x, last_x;
  gboolean match_line;

  first_x = -1;
  
  match_line = ((AGS_COMPOSITE_EDIT_PASTE_MATCH_LINE & (composite_editor->wave_edit->paste_flags)) != 0) ? TRUE: FALSE;
    
  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;
    
  timestamp->timer.ags_offset.offset = 0;
    
  /* paste wave */
  wave_list_node = audio_node->children;

  while(wave_list_node != NULL){
    if(wave_list_node->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(wave_list_node->name,
		     BAD_CAST "wave-list",
		     14)){
	wave_node = wave_list_node->children;
	  
	while(wave_node != NULL){
	  if(wave_node->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(wave_node->name,
			   BAD_CAST "wave",
			   9)){
	      guint64 offset;
		
	      timestamp_node = wave_node->children;
	      offset = 0;
	  
	      while(timestamp_node != NULL){
		if(timestamp_node->type == XML_ELEMENT_NODE){
		  if(!xmlStrncmp(timestamp_node->name,
				 BAD_CAST "timestamp",
				 10)){
		    offset = g_ascii_strtoull(xmlGetProp(timestamp_node,
							 BAD_CAST "offset"),
					      NULL,
					      10);
		      
		    break;
		  }
		}

		timestamp_node = timestamp_node->next;
	      }     
		
	      /* 1st attempt */
	      timestamp->timer.ags_offset.offset = (guint64) relative_offset * floor((double) position_x / (double) relative_offset);
		
	      first_x = ags_composite_editor_paste_wave_all(composite_editor,
							    machine,
							    notebook,
							    wave_node,
							    timestamp,
							    match_line,
							    paste_from_position,
							    position_x,
							    &last_x);

	      /* 2nd attempt */
	      timestamp->timer.ags_offset.offset += relative_offset;

	      ags_composite_editor_paste_wave_all(composite_editor,
						  machine,
						  notebook,
						  wave_node,
						  timestamp,
						  match_line,
						  paste_from_position,
						  position_x,
						  &last_x);
		
	    }
	  }

	  wave_node = wave_node->next;
	}	  
      }
    }

    wave_list_node = wave_list_node->next;
  }    

  g_object_unref(timestamp);

  return(first_x);
}

/**
 * ags_composite_editor_paste:
 * @composite_editor: the #AgsCompositeEditor
 *
 * Paste to @composite_editor from clipboard.
 * 
 * Since: 3.8.0
 */
void
ags_composite_editor_paste(AgsCompositeEditor *composite_editor)
{
  AgsMachine *machine;

  AgsApplicationContext *application_context;
  
  if(!AGS_IS_COMPOSITE_EDITOR(composite_editor)){
    return;
  }

  machine = composite_editor->selected_machine;

  if(machine == NULL){
    return;
  }

  application_context = ags_application_context_get_instance();

  if(composite_editor->selected_edit == composite_editor->notation_edit){
    xmlDoc *clipboard;
    xmlNode *audio_node;
  
    gchar *buffer;
    
    guint position_x, position_y;
    gint first_x, last_x;
    gboolean paste_from_position;

    /* get clipboard */
    buffer = gtk_clipboard_wait_for_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
    
    if(buffer == NULL){
      return;
    }

    position_x = 0;
    position_y = 0;

    /* get position */
    if(composite_editor->toolbar->selected_tool == composite_editor->toolbar->position){
      last_x = 0;
      paste_from_position = TRUE;

      position_x = AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->cursor_position_x;
      position_y = AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->cursor_position_y;
      
#ifdef DEBUG
      printf("pasting at position: [%u,%u]\n", position_x, position_y);
#endif
    }else{
      paste_from_position = FALSE;
    }

    /* get xml tree */
    clipboard = xmlReadMemory(buffer, strlen(buffer),
			      NULL, "UTF-8",
			      0);
    audio_node = xmlDocGetRootElement(clipboard);

    first_x = -1;
    
    /* iterate xml tree */
    while(audio_node != NULL){
      if(audio_node->type == XML_ELEMENT_NODE){
	if(!xmlStrncmp(audio_node->name,
		       BAD_CAST "audio",
		       6)){
	  first_x = ags_composite_editor_paste_notation(composite_editor,
							machine,
							audio_node,
							position_x, position_y,
							paste_from_position,
							&last_x);
	
	  break;
	}
      }
      
      audio_node = audio_node->next;
    }

    if(first_x == -1){
      first_x = 0;
    }
    
    gtk_widget_queue_draw(composite_editor->notation_edit);
    
    xmlFreeDoc(clipboard); 

    if(paste_from_position){
      gint big_step, small_step;

      //TODO:JK: implement me
      big_step = (guint) ceil((double) last_x / 16.0) * 16.0 + (AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->cursor_position_x % (guint) 16);
      small_step = (guint) big_step - 16;
	
      if(small_step < last_x){
	AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->cursor_position_x = big_step;
      }else{
	AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->cursor_position_x = small_step;
      }
    }
  }else if(composite_editor->selected_edit == composite_editor->sheet_edit){
    //TODO:JK: implement me
  }else if(composite_editor->selected_edit == composite_editor->automation_edit){
    AgsAutomationEdit *automation_edit;
    AgsNotebook *notebook;

    xmlDoc *clipboard;
    xmlNode *audio_node;
  
    GList *start_list, *list;
    
    gchar *buffer;

    guint position_x, position_y;
    gint first_x, last_x;
    gboolean paste_from_position;

    automation_edit = NULL;
    notebook = composite_editor->automation_edit->channel_selector;

    position_x = 0;
    position_y = 0;
      
    list =
      start_list = gtk_container_get_children(AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_editor->automation_edit->edit)->automation_edit_box);

    while(list != NULL){
      if(!g_strcmp0(AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name,
		    AGS_AUTOMATION_EDIT(list->data)->control_name) &&
	 AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type == AGS_AUTOMATION_EDIT(list->data)->channel_type){
	automation_edit = list->data;
	  
	break;
      }
	
      list = list->next;
    }

    g_list_free(start_list);  

    /* get clipboard */
    buffer = gtk_clipboard_wait_for_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
    
    if(buffer == NULL){
      return;
    }

    /* get position */
    if(composite_editor->toolbar->selected_tool == composite_editor->toolbar->position){
      last_x = 0;
      paste_from_position = TRUE;

      position_x = AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->cursor_position_x;
      position_y = AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->cursor_position_y;
      
#ifdef DEBUG
      printf("pasting at position: [%u,%u]\n", position_x, position_y);
#endif
    }else{
      paste_from_position = FALSE;
    }

    /* get xml tree */
    clipboard = xmlReadMemory(buffer, strlen(buffer),
			      NULL, "UTF-8",
			      0);
    audio_node = xmlDocGetRootElement(clipboard);

    first_x = -1;
    
    /* iterate xml tree */
    while(audio_node != NULL){
      if(audio_node->type == XML_ELEMENT_NODE){
	if(!xmlStrncmp(BAD_CAST "audio",
		       audio_node->name,
		       6)){
	  first_x = ags_composite_editor_paste_automation(composite_editor,
							  notebook,
							  machine,
							  audio_node,
							  position_x, position_y,
							  paste_from_position,
							  &last_x);
	  
	  break;
	}
      }
      
      audio_node = audio_node->next;
    }

    if(first_x == -1){
      first_x = 0;
    }
    
    gtk_widget_queue_draw(composite_editor->automation_edit->focused_edit);
    
    xmlFreeDoc(clipboard); 

    if(paste_from_position){
      gint big_step, small_step;

      //TODO:JK: implement me
      big_step = (guint) ceil((double) last_x / 16.0) * 16.0 + (AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->cursor_position_x % (guint) 16);
      small_step = (guint) big_step - 16;
	
      if(small_step < last_x){
	AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->cursor_position_x = big_step;
      }else{
	AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->cursor_position_x = small_step;
      }
    }
  }else if(composite_editor->selected_edit == composite_editor->wave_edit){
    AgsWindow *window;
    AgsWaveEdit *wave_edit;
    AgsNotebook *notebook;
  
    AgsWave *wave;

    GObject *soundcard;

    xmlDoc *clipboard;
    xmlNode *audio_node;
    xmlNode *wave_node;
  
    gchar *buffer;

    gdouble bpm;
    guint samplerate;
    guint64 relative_offset;
    guint64 position_x;
    gint64 first_x, last_x;
    double zoom, zoom_factor;
    gdouble delay_factor;
    gboolean paste_from_position;

    window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

    wave_edit = composite_editor->wave_edit->focused_edit;
    
    notebook = composite_editor->wave_edit->channel_selector;

    bpm = gtk_spin_button_get_value(window->navigation->bpm);
    
    /* zoom */
    zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) composite_editor->toolbar->zoom) - 2.0);
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) composite_editor->toolbar->zoom));

    /* check all active tabs */
    soundcard = NULL;

    samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
    
    g_object_get(machine->audio,
		 "output-soundcard", &soundcard,
		 "samplerate", &samplerate,
		 NULL);
    
    delay_factor = ags_soundcard_get_delay_factor(AGS_SOUNDCARD(soundcard));

    relative_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * samplerate;

    /* get clipboard */
    buffer = gtk_clipboard_wait_for_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
    
    if(buffer == NULL){
      if(soundcard != NULL){
	g_object_unref(soundcard);
      }
      
      return;
    }

    /* get position */
    position_x = 0;
    
    if(composite_editor->toolbar->selected_tool == composite_editor->toolbar->position){
      last_x = 0;
      paste_from_position = TRUE;

      position_x = 15.0 * delay_factor * AGS_WAVE_EDIT(composite_editor->wave_edit->focused_edit)->cursor_position_x * samplerate / (16.0 * bpm);
      
#ifdef DEBUG
      printf("pasting at position: [%u]\n", position_x);
#endif
    }else{
      paste_from_position = FALSE;
    }

    /* get xml tree */
    clipboard = xmlReadMemory(buffer, strlen(buffer),
			      NULL, "UTF-8",
			      XML_PARSE_HUGE);
    audio_node = xmlDocGetRootElement(clipboard);

    first_x = -1;
    
    /* iterate xml tree */
    while(audio_node != NULL){
      if(audio_node->type == XML_ELEMENT_NODE){
	if(!xmlStrncmp(BAD_CAST "audio", audio_node->name, 6)){
	  wave_node = audio_node->children;
	
	  //	  g_message("paste");
	  first_x = ags_composite_editor_paste_wave(composite_editor,
						    machine,
						    notebook,
						    audio_node,
						    paste_from_position,
						    position_x,
						    relative_offset);
	
	  break;
	}
      }
      
      audio_node = audio_node->next;
    }

    if(first_x == -1){
      first_x = 0;
    }
    
    gtk_widget_queue_draw(composite_editor->wave_edit->focused_edit);
    
    xmlFreeDoc(clipboard); 

    if(paste_from_position){
      gint big_step, small_step;

      //TODO:JK: implement me
    }

    if(soundcard != NULL){
      g_object_unref(soundcard);
    }
  }
}

/**
 * ags_composite_editor_copy:
 * @composite_editor: the #AgsCompositeEditor
 * 
 * Copy to clipboard from @composite_editor.
 * 
 * Since: 3.8.0
 */
void
ags_composite_editor_copy(AgsCompositeEditor *composite_editor)
{
  AgsMachine *machine;
  
  if(!AGS_IS_COMPOSITE_EDITOR(composite_editor)){
    return;
  }

  machine = composite_editor->selected_machine;
  
  if(machine == NULL){
    return;
  }
  
  if(composite_editor->selected_edit == composite_editor->notation_edit){
    xmlDoc *clipboard;
    xmlNode *audio_node, *notation_list_node, *notation_node;

    GList *start_notation, *notation;

    xmlChar *buffer;

    int buffer_size;
    gint i;
    
    /* create document */
    clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);
    
    /* create root node */
    audio_node = xmlNewNode(NULL,
			    BAD_CAST "audio");
    xmlDocSetRootElement(clipboard, audio_node);
    
    /* create notation list node */
    notation_list_node = xmlNewNode(NULL,
				    BAD_CAST "notation-list");
    xmlAddChild(audio_node,
		notation_list_node);
    
    g_object_get(machine->audio,
		 "notation", &start_notation,
		 NULL);

    i = 0;

    while((i = ags_notebook_next_active_tab(composite_editor->notation_edit->channel_selector,
					    i)) != -1){
      notation = start_notation;

      /* copy */
      while(notation != NULL){
	guint audio_channel;
	
	g_object_get(notation->data,
		     "audio-channel", &audio_channel,
		     NULL);

	if(i != audio_channel){	
	  notation = notation->next;

	  continue;
	}

	notation_node = ags_notation_copy_selection(AGS_NOTATION(notation->data));
	xmlAddChild(notation_list_node,
		    notation_node);
	
	notation = notation->next;
      }

      /* iterate */
      i++;
    }

    g_list_free_full(start_notation,
		     g_object_unref);
    
    /* write to clipboard */
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &buffer_size, "UTF-8", TRUE);
    gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
			   (gchar *) buffer, buffer_size);
    gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));

    xmlFreeDoc(clipboard);
  }else if(composite_editor->selected_edit == composite_editor->sheet_edit){
    //TODO:JK: implement me
  }else if(composite_editor->selected_edit == composite_editor->automation_edit){
    AgsNotebook *notebook;
  
    xmlDoc *clipboard;
    xmlNode *audio_node, *automation_list_node, *automation_node;

    GList *start_automation, *automation;

    xmlChar *buffer;

    int size;
    gint i;
    
    notebook = composite_editor->automation_edit->channel_selector;

    /* create document */
    clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);

    /* create root node */
    audio_node = xmlNewNode(NULL,
			    BAD_CAST "audio");
    xmlDocSetRootElement(clipboard, audio_node);

    automation_list_node = xmlNewNode(NULL,
				    BAD_CAST "automation-list");
    xmlAddChild(audio_node,
		automation_list_node);

    /* create automation nodes */
    g_object_get(machine->audio,
		 "automation", &start_automation,
		 NULL);
    
    i = 0;

    while(notebook == NULL ||
	  (i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){
      automation = start_automation;

      /* copy */
      while((automation = ags_automation_find_near_timestamp_extended(automation, i,
								      AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type, AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name,
								      NULL)) != NULL){
	automation_node = ags_automation_copy_selection(AGS_AUTOMATION(automation->data));
	xmlAddChild(automation_list_node,
		    automation_node);
	
	automation = automation->next;
      }

      if(notebook == NULL){
	break;
      }
      
      i++;
    }
    
    g_list_free_full(start_automation,
		     g_object_unref);
          
    /* write to clipboard */
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8", TRUE);
    gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
			   (gchar *) buffer, size);
    gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));

    xmlFreeDoc(clipboard);
  }else if(composite_editor->selected_edit == composite_editor->wave_edit){
    AgsNotebook *notebook;
  
    xmlDoc *clipboard;
    xmlNode *audio_node, *wave_list_node, *wave_node;

    GList *start_wave, *wave;

    xmlChar *buffer;

    int size;
    guint line;
    gint i;
  
    notebook = composite_editor->wave_edit->channel_selector;

    /* create document */
    clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);

    /* create root node */
    audio_node = xmlNewNode(NULL,
			    BAD_CAST "audio");
    xmlDocSetRootElement(clipboard, audio_node);

    wave_list_node = xmlNewNode(NULL,
				BAD_CAST "wave-list");
    xmlAddChild(audio_node,
		wave_list_node);

    /* create wave nodes */
    g_object_get(machine->audio,
		 "wave", &start_wave,
		 NULL);
    
    i = 0;

    while((i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){
      wave = start_wave;

      /* copy */
      while(wave != NULL){
	g_object_get(wave->data,
		     "line", &line,
		     NULL);

	if(i != line){	
	  wave = wave->next;

	  continue;
	}

	//	g_message("copy %d", i);
	wave_node = ags_wave_copy_selection(AGS_WAVE(wave->data));
	xmlAddChild(wave_list_node,
		    wave_node);
	
	wave = wave->next;
      }

      i++;
    }

    g_list_free_full(start_wave,
		     g_object_unref);
    
    /* write to clipboard */
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8", TRUE);
    gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
			   (gchar *) buffer, size);
    gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));

    xmlFreeDoc(clipboard);
  }
}

/**
 * ags_composite_editor_cut:
 * @composite_editor: the #AgsCompositeEditor
 * 
 * Copy to clipboard from @composite_editor.
 * 
 * Since: 3.8.0
 */
void
ags_composite_editor_cut(AgsCompositeEditor *composite_editor)
{
  AgsMachine *machine;
  
  if(!AGS_IS_COMPOSITE_EDITOR(composite_editor)){
    return;
  }

  machine = composite_editor->selected_machine;

  if(machine == NULL){
    return;
  }

  if(composite_editor->selected_edit == composite_editor->notation_edit){
    AgsNotebook *notebook;
    
    xmlDoc *clipboard;
    xmlNode *audio_node;
    xmlNode *notation_list_node, *notation_node;

    GList *start_notation, *notation;

    xmlChar *buffer;

    int buffer_size;
    guint audio_channel;
    gint i;

    notebook = composite_editor->notation_edit->channel_selector;
    
    /* create document */
    clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);

    /* create root node */
    audio_node = xmlNewNode(NULL,
			    BAD_CAST "audio");
    xmlDocSetRootElement(clipboard, audio_node);

    notation_list_node = xmlNewNode(NULL,
				    BAD_CAST "notation-list");
    xmlAddChild(audio_node,
		notation_list_node);

    /* create notation nodes */
    g_object_get(machine->audio,
		 "notation", &start_notation,
		 NULL);

    i = 0;
    
    while((i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){
      notation = start_notation;

      /* cut */
      while(notation != NULL){
	g_object_get(notation->data,
		     "audio-channel", &audio_channel,
		     NULL);

	if(i != audio_channel){	
	  notation = notation->next;

	  continue;
	}

	notation_node = ags_notation_cut_selection(AGS_NOTATION(notation->data));
	xmlAddChild(notation_list_node,
		    notation_node);
	
	notation = notation->next;
      }

      /* iterate */
      i++;
    }

    g_list_free_full(start_notation,
		     g_object_unref);

    /* write to clipboard */
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &buffer_size, "UTF-8", TRUE);
    gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
			   (gchar *) buffer, buffer_size);
    gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));

    xmlFreeDoc(clipboard);
    
    gtk_widget_queue_draw(composite_editor->notation_edit);
  }else if(composite_editor->selected_edit == composite_editor->sheet_edit){
    //TODO:JK: implement me
  }else if(composite_editor->selected_edit == composite_editor->automation_edit){    
    AgsNotebook *notebook;
  
    xmlDoc *clipboard;
    xmlNode *audio_node;
    xmlNode *automation_list_node, *automation_node;

    GList *start_automation, *automation;

    xmlChar *buffer;
    int size;
    gint i;

    notebook = composite_editor->automation_edit->channel_selector;

    /* create document */
    clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);

    /* create root node */
    audio_node = xmlNewNode(NULL,
			    BAD_CAST "audio");
    xmlDocSetRootElement(clipboard, audio_node);

    automation_list_node = xmlNewNode(NULL,
				      BAD_CAST "automation-list");
    xmlAddChild(audio_node,
		automation_list_node);

    /* create automation nodes */
    g_object_get(machine->audio,
		 "automation", &start_automation,
		 NULL);
    
    i = 0;
    
    while(notebook == NULL ||
	  (i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){
      automation = start_automation;

      /* cut */
      while((automation = ags_automation_find_near_timestamp_extended(automation, i,
								      AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type, AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name,
								      NULL)) != NULL){
	automation_node = ags_automation_cut_selection(AGS_AUTOMATION(automation->data));
	xmlAddChild(automation_list_node,
		    automation_node);
	
	automation = automation->next;
      }
      
      if(notebook == NULL){
	break;
      }

      i++;
    }
    
    g_list_free_full(start_automation,
		     g_object_unref);

    /* write to clipboard */
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8", TRUE);
    gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
			   (gchar *) buffer, size);
    gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));

    xmlFreeDoc(clipboard);
    
    gtk_widget_queue_draw(composite_editor->automation_edit->focused_edit);
  }else if(composite_editor->selected_edit == composite_editor->wave_edit){
    AgsNotebook *notebook;
  
    xmlDoc *clipboard;
    xmlNode *audio_node;
    xmlNode *wave_list_node, *wave_node;

    GList *start_wave, *wave;

    xmlChar *buffer;
  
    int size;
    guint line;
    gint i;

    notebook = composite_editor->wave_edit->channel_selector;

    /* create document */
    clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);

    /* create root node */
    audio_node = xmlNewNode(NULL,
			    BAD_CAST "audio");
    xmlDocSetRootElement(clipboard, audio_node);

    wave_list_node = xmlNewNode(NULL,
				BAD_CAST "wave-list");
    xmlAddChild(audio_node,
		wave_list_node);

    /* create wave nodes */
    g_object_get(machine->audio,
		 "wave", &start_wave,
		 NULL);
    
    i = 0;
    
    while((i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){
      wave = start_wave;

      /* cut */
      while(wave != NULL){
	g_object_get(wave->data,
		     "line", &line,
		     NULL);

	if(i != line){	
	  wave = wave->next;

	  continue;
	}

	//	g_message("cut %d", i);
	wave_node = ags_wave_cut_selection(AGS_WAVE(wave->data));
	xmlAddChild(wave_list_node,
		    wave_node);
	
	wave = wave->next;
      }

      i++;
    }

    g_list_free_full(start_wave,
		     g_object_unref);

    /* write to clipboard */
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8", TRUE);
    gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
			   (gchar *) buffer, size);
    gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));

    xmlFreeDoc(clipboard);
    
    gtk_widget_queue_draw(composite_editor->wave_edit->focused_edit);
  }
}

void
ags_composite_editor_get_boundary(AgsCompositeEditor *composite_editor,
				  AgsMachine *machine,
				  AgsNotation *notation,
				  guint *lower, guint *upper)
{
  GList *start_note, *note;

  guint note_y;

  g_object_get(notation,
	       "note", &start_note,
	       NULL);
    
  /* retrieve upper and lower */
  note = start_note;
    
  while(note != NULL){
    g_object_get(note->data,
		 "y", &note_y,
		 NULL);
      
    if(note_y < lower[0]){
      lower[0] = note_y;
    }

    if(note_y > upper[0]){
      upper[0] = note_y;
    }
      
    note = note->next;
  }

  g_list_free_full(start_note,
		   g_object_unref);
}

void
ags_composite_editor_invert_notation(AgsCompositeEditor *composite_editor,
				     AgsMachine *machine,
				     AgsNotation *notation,
				     guint lower, guint upper)
{
  GList *start_note, *note;

  guint note_y;
    
  g_object_get(notation,
	       "note", &start_note,
	       NULL);

  /* invert */
  note = start_note;

  while(note != NULL){
    g_object_get(note->data,
		 "y", &note_y,
		 NULL);
      
    if((gdouble) note_y < (gdouble) (upper - lower) / 2.0){
      g_object_set(note->data,
		   "y", (upper - (note_y - lower)),
		   NULL);
    }else if((gdouble) note_y > (gdouble) (upper - lower) / 2.0){
      g_object_set(note->data,
		   "y", (lower + (upper - AGS_NOTE(note->data)->y)),
		   NULL);
    }
      
    note = note->next;
  }

  g_list_free_full(start_note,
		   g_object_unref);
}

/**
 * ags_composite_editor_invert:
 * @composite_editor: the #AgsCompositeEditor
 * 
 * Invert selected region of @composite_editor.
 * 
 * Since: 3.8.0
 */
void
ags_composite_editor_invert(AgsCompositeEditor *composite_editor)
{
  AgsMachine *machine;
  
  if(!AGS_IS_COMPOSITE_EDITOR(composite_editor)){
    return;
  }

  machine = composite_editor->selected_machine;

  if(machine == NULL){
    return;
  }

  if(composite_editor->selected_edit == composite_editor->notation_edit){
    AgsNotebook *notebook;
    
    GList *start_notation, *notation;

    guint audio_channel;
    guint lower, upper;
    gint i;
    
    notebook = composite_editor->notation_edit->channel_selector;
    
    g_object_get(machine->audio,
		 "notation", &start_notation,
		 NULL);
    
    i = 0;
    
    while((i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){
      /* get boundary */
      notation = start_notation;

      lower = G_MAXUINT;
      upper = 0;

      while(notation != NULL){
	g_object_get(notation->data,
		     "audio-channel", &audio_channel,
		     NULL);

	if(i != audio_channel){	
	  notation = notation->next;

	  continue;
	}

	ags_composite_editor_get_boundary(composite_editor,
					  machine,
					  AGS_NOTATION(notation->data),
					  &lower, &upper);

	notation = notation->next;
      }
      
      /* invert */
      notation = start_notation;
      
      while(notation != NULL){
	g_object_get(notation->data,
		     "audio-channel", &audio_channel,
		     NULL);

	if(i != audio_channel){	
	  notation = notation->next;

	  continue;
	}

	ags_composite_editor_invert_notation(composite_editor,
					     machine,
					     AGS_NOTATION(notation->data),
					     lower, upper);

	notation = notation->next;
      }
      
      i++;
    }
    
    gtk_widget_queue_draw(composite_editor->notation_edit);
    
    g_list_free_full(start_notation,
		     g_object_unref);
  }else if(composite_editor->selected_edit == composite_editor->sheet_edit){
    //TODO:JK: implement me
  }
}

/**
 * ags_composite_editor_do_feedback:
 * @composite_editor: the #AgsCompositeEditor
 * 
 * Do feedback of @composite_editor.
 * 
 * Since: 3.12.0
 */
void
ags_composite_editor_do_feedback(AgsCompositeEditor *composite_editor)
{
  AgsMachine *machine;
  AgsNotationEdit *notation_edit;
  
  AgsChannel *start_output, *start_input;
  AgsChannel *nth_channel, *nth_pad;
  AgsPlayback *playback;
    
  AgsTimestamp *timestamp;
    
  GList *start_list_notation, *list_notation;
    
  guint output_pads, input_pads;
  gint i;
  
  if(!AGS_IS_COMPOSITE_EDITOR(composite_editor)){
    return;
  }

  machine = composite_editor->selected_machine;

  if(machine == NULL){
    return;
  }

  notation_edit = (AgsNotationEdit *) composite_editor->notation_edit->edit;
  
  /* check all active tabs */
  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;
    
  timestamp->timer.ags_offset.offset = AGS_NOTATION_DEFAULT_OFFSET * floor(notation_edit->cursor_position_x / AGS_NOTATION_DEFAULT_OFFSET);

  i = 0;

  g_object_get(machine->audio,
	       "output", &start_output,
	       "output-pads", &output_pads,
	       "input", &start_input,
	       "input-pads", &input_pads,
	       "notation", &start_list_notation,
	       NULL);

  while((i = ags_notebook_next_active_tab(composite_editor->notation_edit->channel_selector,
					  i)) != -1){
    AgsNote *current_note;
    AgsNote *play_note;
      
    list_notation = start_list_notation;
    list_notation = ags_notation_find_near_timestamp(list_notation, i,
						     timestamp);

    if(list_notation == NULL){
      i++;
	
      continue;
    }

    current_note = ags_notation_find_point(list_notation->data,
					   notation_edit->cursor_position_x, notation_edit->cursor_position_y,
					   FALSE);

    if(current_note != NULL){
      if(ags_audio_test_behaviour_flags(machine->audio, AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_OUTPUT)){
	nth_channel = ags_channel_nth(start_output,
				      i);
      }else{
	nth_channel = ags_channel_nth(start_input,
				      i);
      }
	
      if(ags_audio_test_behaviour_flags(machine->audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
	nth_pad = ags_channel_pad_nth(nth_channel,
				      (ags_audio_test_behaviour_flags(machine->audio, AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_OUTPUT) ? output_pads: input_pads) - notation_edit->cursor_position_y - 1);
      }else{
	nth_pad = ags_channel_pad_nth(nth_channel,
				      notation_edit->cursor_position_y);
      }

      if(nth_pad != NULL){
	guint x0, x1;
	  
	g_object_get(nth_pad,
		     "playback", &playback,
		     NULL);

	g_object_get(playback,
		     "play-note", &play_note,
		     NULL);

	g_object_get(current_note,
		     "x0", &x0,
		     "x1", &x1,
		     NULL);
	  
	g_object_set(play_note,
		     "x0", 0,
		     "x1", x1 - x0,
		     NULL);
	  
	ags_machine_playback_set_active(machine,
					playback,
					TRUE);
	  
	g_object_unref(playback);

	g_object_unref(play_note);
      }

      /* unref */
      if(nth_channel != NULL){
	g_object_unref(nth_channel);
      }

      if(nth_pad != NULL){
	g_object_unref(nth_pad);
      }
    }

    /* iterate */
    i++;
  }

  g_list_free_full(start_list_notation,
		   g_object_unref);

  /* unref */
  if(start_output != NULL){
    g_object_unref(start_output);
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

/**
 * ags_composite_editor_add_note:
 * @composite_editor: the #AgsCompositeEditor
 * @note: the #AgsNote
 * 
 * Add @note to @composite_editor.
 * 
 * Since: 3.12.0
 */
void
ags_composite_editor_add_note(AgsCompositeEditor *composite_editor,
			      AgsNote *note)
  
{  
  AgsMachine *machine;
  
  if(!AGS_IS_COMPOSITE_EDITOR(composite_editor) ||
     !AGS_IS_NOTE(note)){
    return;
  }

  machine = composite_editor->selected_machine;

  if(machine == NULL){
    return;
  }

  if(composite_editor->selected_edit == composite_editor->notation_edit){
    AgsTimestamp *timestamp;

    GList *start_notation, *notation;

    guint x0;
    gint i;

    /* check all active tabs */
    timestamp = ags_timestamp_new();

    timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
    timestamp->flags |= AGS_TIMESTAMP_OFFSET;

    g_object_get(note,
		 "x0", &x0,
		 NULL);
    
    ags_timestamp_set_ags_offset(timestamp,
				 (guint64) AGS_NOTATION_DEFAULT_OFFSET * floor((double) x0 / (double) AGS_NOTATION_DEFAULT_OFFSET));

    i = 0;

    while((i = ags_notebook_next_active_tab(composite_editor->notation_edit->channel_selector,
					    i)) != -1){
      AgsNotation *current_notation;
      AgsNote *new_note;

      g_object_get(machine->audio,
		   "notation", &start_notation,
		   NULL);
      
      notation = ags_notation_find_near_timestamp(start_notation, i,
						  timestamp);

      if(notation != NULL){
	current_notation = notation->data;
      }else{
	current_notation = ags_notation_new((GObject *) machine->audio,
					    i);

	ags_timestamp_set_ags_offset(current_notation->timestamp,
				     timestamp->timer.ags_offset.offset);
	
	ags_audio_add_notation(machine->audio,
			       (GObject *) current_notation);
      }

      new_note = ags_note_duplicate(note);
      ags_notation_add_note(current_notation,
			    new_note,
			    FALSE);

      g_list_free_full(start_notation,
		       g_object_unref);

      /* iterate */
      i++;
    }
    
    gtk_widget_queue_draw(composite_editor->notation_edit);

    g_object_unref(timestamp);
  }else if(composite_editor->selected_edit == composite_editor->sheet_edit){
    //TODO:JK: implement me
  }
}

/**
 * ags_composite_editor_delete_note:
 * @composite_editor: the #AgsCompositeEditor
 * @x: the x position
 * @y: the y position
 * 
 * Delete note at position @x|@y of @composite_editor.
 * 
 * Since: 3.12.0
 */
void
ags_composite_editor_delete_note(AgsCompositeEditor *composite_editor,
				 guint x, guint y)
{
  AgsMachine *machine;
  
  if(!AGS_IS_COMPOSITE_EDITOR(composite_editor)){
    return;
  }

  machine = composite_editor->selected_machine;

  if(machine == NULL){
    return;
  }

  if(composite_editor->selected_edit == composite_editor->notation_edit){
    AgsTimestamp *timestamp;

    GList *start_notation, *notation;

    gint i;

    timestamp = ags_timestamp_new();

    timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
    timestamp->flags |= AGS_TIMESTAMP_OFFSET;
        
    ags_timestamp_set_ags_offset(timestamp,
				 (guint64) AGS_NOTATION_DEFAULT_OFFSET * floor((double) x / (double) AGS_NOTATION_DEFAULT_OFFSET));

    i = 0;

    while((i = ags_notebook_next_active_tab(composite_editor->notation_edit->channel_selector,
					    i)) != -1){
      g_object_get(machine->audio,
		   "notation", &start_notation,
		   NULL);
      
      notation = ags_notation_find_near_timestamp(start_notation, i,
						  timestamp);
      
      if(notation != NULL){
	ags_notation_remove_note_at_position(notation->data,
					     x, y);
      }

      g_list_free_full(start_notation,
		       g_object_unref);
       
      /* iterate */
      i++;
    }
    
    gtk_widget_queue_draw(composite_editor->notation_edit);

    g_object_unref(timestamp);
  }else if(composite_editor->selected_edit == composite_editor->sheet_edit){
    //TODO:JK: implement me
  }
}

/**
 * ags_composite_editor_add_acceleration:
 * @composite_editor: the #AgsCompositeEditor
 * @acceleration: the #AgsAcceleration
 * 
 * Add @acceleration to @composite_editor.
 * 
 * Since: 3.12.2
 */
void
ags_composite_editor_add_acceleration(AgsCompositeEditor *composite_editor,
				      AgsAcceleration *acceleration)
{
  AgsMachine *machine;
 
  AgsNotebook *notebook;
 
  AgsChannel *start_output, *start_input;
  AgsChannel *nth_channel;
  AgsAutomation *automation;
  
  AgsTimestamp *timestamp;

  gint i;
  
  if(!AGS_IS_COMPOSITE_EDITOR(composite_editor) ||
     !AGS_IS_ACCELERATION(acceleration)){
    return;
  }

  machine = composite_editor->selected_machine;

  if(machine == NULL){
    return;
  }
  
  /* get some fields */
  start_output = NULL;
  start_input = NULL;

  g_object_get(machine->audio,
	       "output", &start_output,
	       "input", &start_input,
	       NULL);

  notebook = composite_editor->automation_edit->channel_selector;

  /* check all active tabs */
  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;
    
  timestamp->timer.ags_offset.offset = AGS_AUTOMATION_DEFAULT_OFFSET * floor(acceleration->x / AGS_AUTOMATION_DEFAULT_OFFSET);

  i = 0;

  while(notebook == NULL ||
	(i = ags_notebook_next_active_tab(notebook,
					  i)) != -1){      
    AgsAcceleration *new_acceleration;

    GList *start_play_port, *play_port;
    GList *start_recall_port, *recall_port;
    GList *start_list, *list;

    play_port = NULL;
    recall_port = NULL;
      
    if(AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type == AGS_TYPE_OUTPUT){
      nth_channel = ags_channel_nth(start_output,
				    i);

      play_port =
	start_play_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(nth_channel,
											 AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name,
											 TRUE);

      recall_port =
	start_recall_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(nth_channel,
											   AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name,
											   FALSE);

      if(nth_channel != NULL){
	g_object_unref(nth_channel);
      }
    }else if(AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type == AGS_TYPE_INPUT){
      nth_channel = ags_channel_nth(start_input,
				    i);

      play_port =
	start_play_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(nth_channel,
											 AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name,
											 TRUE);

      recall_port =
	start_recall_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(nth_channel,
											   AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name,
											   FALSE);

      if(nth_channel != NULL){
	g_object_unref(nth_channel);
      }
    }else{
      play_port =
	start_play_port = ags_audio_collect_all_audio_ports_by_specifier_and_context(machine->audio,
										     AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name,
										     TRUE);
	
      recall_port =
	start_recall_port = ags_audio_collect_all_audio_ports_by_specifier_and_context(machine->audio,
										       AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name,
										       FALSE);
    }

    /* play port */
    while(play_port != NULL){
      AgsPort *current_port;
	
      current_port = play_port->data;

      g_object_get(current_port,
		   "automation", &start_list,
		   NULL);
	
      list = ags_automation_find_near_timestamp(start_list, i,
						timestamp);
	
      if(list == NULL){
	automation = ags_automation_new(G_OBJECT(machine->audio),
					i,
					AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type, AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name);
	automation->timestamp->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;

	g_object_set(automation,
		     "port", current_port,
		     NULL);

	/* add to audio */
	ags_audio_add_automation(machine->audio,
				 (GObject *) automation);

	/* add to port */
	ags_port_add_automation(current_port,
				(GObject *) automation);
      }else{
	automation = list->data;
      }
	
      new_acceleration = ags_acceleration_duplicate(acceleration);
      ags_automation_add_acceleration(automation,
				      new_acceleration,
				      FALSE);

      g_list_free_full(start_list,
		       g_object_unref);
	
      /* iterate */
      play_port = play_port->next;
    }

    g_list_free_full(start_play_port,
		     g_object_unref);
      
    /* recall port */
    if(recall_port != NULL){
      AgsPort *current_port;
	
      current_port = recall_port->data;

      g_object_get(current_port,
		   "automation", &start_list,
		   NULL);

      list = ags_automation_find_near_timestamp(start_list, i,
						timestamp);
	
      if(list == NULL){
	automation = ags_automation_new(G_OBJECT(machine->audio),
					i,
					AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type, AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name);
	automation->timestamp->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;
	g_object_set(automation,
		     "port", current_port,
		     NULL);
	  
	/* add to audio */
	ags_audio_add_automation(machine->audio,
				 (GObject *) automation);

	/* add to port */
	ags_port_add_automation(current_port,
				(GObject *) automation);
      }else{
	automation = list->data;
      }
	
      new_acceleration = ags_acceleration_duplicate(acceleration);
      ags_automation_add_acceleration(automation,
				      new_acceleration,
				      FALSE);

      g_list_free_full(start_list,
		       g_object_unref);
	
      /* iterate */
      recall_port = recall_port->next;
    }      

    g_list_free_full(start_recall_port,
		     g_object_unref);

    if(notebook == NULL){
      break;
    }
      
    i++;
  }    
  
  gtk_widget_queue_draw(composite_editor->automation_edit->focused_edit);
  
  /* unref */
  if(start_output != NULL){
    g_object_unref(start_output);
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }

  g_object_unref(timestamp);
    
  gtk_widget_queue_draw(GTK_WIDGET(AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)));
}

/**
 * ags_composite_editor_delete_acceleration:
 * @composite_editor: the #AgsCompositeEditor
 * @x: the x offset
 * @y: the y value
 * 
 * Delete acceleration at position @x and @y from @composite_editor.
 * 
 * Since: 3.12.2
 */
void
ags_composite_editor_delete_acceleration(AgsCompositeEditor *composite_editor,
					 guint x, gdouble y)
{
  AgsMachine *machine;
  AgsNotebook *notebook;

  GtkAdjustment *automation_edit_vscrollbar_adjustment;

  AgsAutomation *current_automation;

  AgsTimestamp *timestamp;

  GtkAllocation automation_edit_allocation;

  GList *start_automation, *automation;

  gdouble c_range;
  guint g_range;
  guint scan_x;
  gdouble scan_y;
  gboolean success;
  gint i, j, j_step, j_stop;
  
  if(!AGS_IS_COMPOSITE_EDITOR(composite_editor)){
    return;
  }

  machine = composite_editor->selected_machine;

  if(machine == NULL){
    return;
  }

  notebook = composite_editor->automation_edit->channel_selector;

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->flags)) != 0){
    c_range = exp(AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->upper) - exp(AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->lower);
  }else{
    c_range = AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->upper - AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->lower;
  }

  gtk_widget_get_allocation(GTK_WIDGET(AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->drawing_area),
			    &automation_edit_allocation);
    
  automation_edit_vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->vscrollbar));
    
  g_range = gtk_adjustment_get_upper(automation_edit_vscrollbar_adjustment) + automation_edit_allocation.height;
    
  /* get some fields */
  g_object_get(machine->audio,
	       "automation", &start_automation,
	       NULL);

  /* check all active tabs */
  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;
    
  timestamp->timer.ags_offset.offset = AGS_AUTOMATION_DEFAULT_OFFSET * floor(x / AGS_AUTOMATION_DEFAULT_OFFSET);

  i = 0;

  while(notebook == NULL ||
	(i = ags_notebook_next_active_tab(notebook,
					  i)) != -1){
    automation = start_automation;
      
    while((automation = ags_automation_find_near_timestamp_extended(automation, i,
								    AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type, AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name,
								    timestamp)) != NULL){
      
      if(automation != NULL){
	current_automation = automation->data;
      }else{
	if(notebook == NULL){
	  break;
	}

	automation = automation->next;
	  
	i++;
	
	continue;
      }

      success = FALSE;
      
      j = 0;
      j_step = 1;
      j_stop = 4;

      while(!success &&
	    exp2(j_step) <= AGS_AUTOMATION_EDIT_DEFAULT_SCAN_WIDTH){
	scan_x = (-1 * j_step + floor(j / (2 * j_step)));

	if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->flags)) != 0){
	  scan_y = log((-1 * j_step + floor(j % (2 * j_step))) / g_range) * c_range;	
	}else{
	  scan_y = ((-1 * j_step + floor(j % (2 * j_step))) / g_range) * c_range;	
	}

	success = ags_automation_remove_acceleration_at_position(current_automation,
								 x - scan_x, y - scan_y);
	
	j++;
	
	if(j >= j_stop){
	  j_step++;
	  j_stop = exp2(j_step + 1);
	}
      }

	
      automation = automation->next;
    }
      
    if(notebook == NULL){
      break;
    }

    i++;
  }

  gtk_widget_queue_draw(composite_editor->automation_edit->focused_edit);
  
  g_list_free_full(start_automation,
		   g_object_unref);
}

/**
 * ags_composite_editor_select_region:
 * @composite_editor: the #AgsCompositeEditor
 * @x0: the x0 position
 * @y0: the y0 position
 * @x1: the x1 position
 * @y1: the y1 position
 * 
 * Selecte region of @composite_editor.
 * 
 * Since: 3.12.0
 */
void
ags_composite_editor_select_region(AgsCompositeEditor *composite_editor,
				   guint x0, guint y0, 
				   guint x1, guint y1)
{
  AgsMachine *machine;
  
  AgsApplicationContext *application_context;
  
  if(!AGS_IS_COMPOSITE_EDITOR(composite_editor)){
    return;
  }

  machine = composite_editor->selected_machine;

  if(machine == NULL){
    return;
  }

  application_context = ags_application_context_get_instance();
  
  if(composite_editor->selected_edit == composite_editor->notation_edit){
    AgsTimestamp *timestamp;

    GList *start_notation, *notation;

    gint i;

    timestamp = ags_timestamp_new();

    timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
    timestamp->flags |= AGS_TIMESTAMP_OFFSET;

    i = 0;

    g_object_get(machine->audio,
		 "notation", &start_notation,
		 NULL);
    
    while((i = ags_notebook_next_active_tab(composite_editor->notation_edit->channel_selector,
					    i)) != -1){      
      notation = start_notation;
      
      ags_timestamp_set_ags_offset(timestamp,
				   (guint64) AGS_NOTATION_DEFAULT_OFFSET * floor((double) x0 / (double) AGS_NOTATION_DEFAULT_OFFSET));

      while((notation = ags_notation_find_near_timestamp(notation, i,
							 timestamp)) != NULL &&
	    ags_timestamp_get_ags_offset(timestamp) < (guint64) (AGS_NOTATION_DEFAULT_OFFSET * floor((double) x1 / (double) AGS_NOTATION_DEFAULT_OFFSET)) + AGS_NOTATION_DEFAULT_OFFSET){
	ags_notation_add_region_to_selection(notation->data,
					     x0, y0,
					     x1, y1,
					     TRUE);
	
	/* iterate */
	timestamp->timer.ags_offset.offset += AGS_NOTATION_DEFAULT_OFFSET;
	
	notation = notation->next;
      }

      /* iterate */
      i++;
    }

    gtk_widget_queue_draw(composite_editor->notation_edit);
  
    g_list_free_full(start_notation,
		     (GDestroyNotify) g_object_unref);
  }else if(composite_editor->selected_edit == composite_editor->sheet_edit){
    //TODO:JK: implement me
  }else if(composite_editor->selected_edit == composite_editor->automation_edit){
    AgsNotebook *notebook;
  
    AgsTimestamp *timestamp;

    GList *start_automation, *automation;

    gint i;

    notebook = composite_editor->automation_edit->channel_selector;

     /* swap values if needed */
    if(x0 > x1){
      guint tmp;

      tmp = x0;
      
      x0 = x1;
      x1 = tmp;
    }

    if(y0 > y1){
      gdouble tmp;

      tmp = y0;
      
      y0 = y1;
      y1 = tmp;
    }

    /* check all active tabs */
    g_object_get(machine->audio,
		 "automation", &start_automation,
		 NULL);

    timestamp = ags_timestamp_new();

    timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
    timestamp->flags |= AGS_TIMESTAMP_OFFSET;
    
    i = 0;

    while(notebook == NULL ||
	  (i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){      
      automation = start_automation;
      
      timestamp->timer.ags_offset.offset = AGS_AUTOMATION_DEFAULT_OFFSET * floor(x0 / AGS_AUTOMATION_DEFAULT_OFFSET);

      while(timestamp->timer.ags_offset.offset < (AGS_AUTOMATION_DEFAULT_OFFSET * floor(x1 / AGS_AUTOMATION_DEFAULT_OFFSET)) + AGS_AUTOMATION_DEFAULT_OFFSET){
	while((automation = ags_automation_find_near_timestamp_extended(automation, i,
									AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type, AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->control_name,
									timestamp)) != NULL){
	  ags_automation_add_region_to_selection(automation->data,
						 x0, y0,
						 x1, y1,
						 TRUE);
	  
	  /* iterate */    
	  automation = automation->next;
	}

	timestamp->timer.ags_offset.offset += AGS_AUTOMATION_DEFAULT_OFFSET;
      }
      
      if(notebook == NULL){
	break;
      }
      
      i++;
    }
    
    gtk_widget_queue_draw(composite_editor->automation_edit->focused_edit);
  
    g_list_free_full(start_automation,
		     g_object_unref);
  }else if(composite_editor->selected_edit == composite_editor->wave_edit){
    AgsWindow *window;
    AgsNotebook *notebook;

    AgsTimestamp *timestamp;

    GObject *soundcard;

    GList *start_wave_edit, *wave_edit;
    GList *start_wave, *wave;

    gdouble bpm;
    guint samplerate;
    guint buffer_size;
    guint64 relative_offset;
    guint64 x0_offset, x1_offset;
    double zoom, zoom_factor;
    gdouble delay_factor;
    gint i;

    window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
    
    notebook = composite_editor->wave_edit->channel_selector;

    bpm = gtk_spin_button_get_value(window->navigation->bpm);

    /* swap values if needed */
    if(x0 > x1){
      guint tmp;

      tmp = x0;
      
      x0 = x1;
      x1 = tmp;
    }

    if(y0 > y1){
      gdouble tmp;

      tmp = y0;
      
      y0 = y1;
      y1 = tmp;
    }
    
    /* zoom */
    zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) composite_editor->toolbar->zoom) - 2.0);
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) composite_editor->toolbar->zoom));
    
    /* check all active tabs */
    soundcard = NULL;
    
    start_wave = NULL;

    samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
    buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
    
    g_object_get(machine->audio,
		 "output-soundcard", &soundcard,
		 "wave", &start_wave,
		 "samplerate", &samplerate,
		 "buffer-size", &buffer_size,
		 NULL);
    
    delay_factor = ags_soundcard_get_delay_factor(AGS_SOUNDCARD(soundcard));

    relative_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * samplerate;
    
    x0_offset = (x0 / 64.0) * delay_factor / (bpm / 60.0) * samplerate;
    x1_offset = (x1 / 64.0) * delay_factor / (bpm / 60.0) * samplerate;

    //TODO:JK: improve me
    x0_offset = buffer_size * floor(x0_offset / buffer_size);
    x1_offset = buffer_size * ceil(x1_offset / buffer_size);
    
    timestamp = ags_timestamp_new();

    timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
    timestamp->flags |= AGS_TIMESTAMP_OFFSET;

    start_wave_edit = gtk_container_get_children(AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->wave_edit_box);
    i = 0;

    while((i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){
      wave_edit = g_list_nth(start_wave_edit,
			     i);
      
      wave = start_wave;
      
      timestamp->timer.ags_offset.offset = relative_offset * floor(x0 / relative_offset);

      while(timestamp->timer.ags_offset.offset < (relative_offset * floor(x1 / relative_offset)) + relative_offset){
	while((wave = ags_wave_find_near_timestamp(wave, i,
							timestamp)) != NULL){
	  ags_wave_add_region_to_selection(wave->data,
					   x0_offset,
					   x1_offset,
					   TRUE);
	  
	  /* iterate */
	  wave = wave->next;
	}

	timestamp->timer.ags_offset.offset += relative_offset;
      }

      /* queue draw */
      gtk_widget_queue_draw(GTK_WIDGET(wave_edit->data));
      
      i++;
    }
    
    gtk_widget_queue_draw(composite_editor->wave_edit->focused_edit);
  
    if(soundcard != NULL){
      g_object_unref(soundcard);
    }

    g_list_free(start_wave_edit);
    g_list_free_full(start_wave,
		     g_object_unref);
  }
}

/**
 * ags_composite_editor_new:
 *
 * Create a new #AgsCompositeEditor.
 *
 * Returns: a new #AgsCompositeEditor
 *
 * Since: 3.8.0
 */
AgsCompositeEditor*
ags_composite_editor_new()
{
  AgsCompositeEditor *composite_editor;

  composite_editor = (AgsCompositeEditor *) g_object_new(AGS_TYPE_COMPOSITE_EDITOR,
							 NULL);

  return(composite_editor);
}
