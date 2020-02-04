/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/X/ags_notation_editor.h>
#include <ags/X/ags_notation_editor_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>

#include <ags/X/machine/ags_drum.h>
#include <ags/X/machine/ags_matrix.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <math.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_notation_editor_class_init(AgsNotationEditorClass *notation_editor);
void ags_notation_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_notation_editor_init(AgsNotationEditor *notation_editor);
void ags_notation_editor_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_notation_editor_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_notation_editor_finalize(GObject *gobject);

void ags_notation_editor_connect(AgsConnectable *connectable);
void ags_notation_editor_disconnect(AgsConnectable *connectable);

void ags_notation_editor_show(GtkWidget *widget);
void ags_notation_editor_show_all(GtkWidget *widget);

void ags_notation_editor_real_machine_changed(AgsNotationEditor *notation_editor,
					      AgsMachine *machine);

gint ags_notation_editor_paste_notation_all(AgsNotationEditor *notation_editor,
					    AgsMachine *machine,
					    xmlNode *notation_node,
					    AgsTimestamp *timestamp,
					    gboolean match_channel, gboolean no_duplicates,
					    guint position_x, guint position_y,
					    gboolean paste_from_position,
					    gint *last_x);
gint ags_notation_editor_paste_notation(AgsNotationEditor *notation_editor,
					AgsMachine *machine,
					xmlNode *audio_node,
					guint position_x, guint position_y,
					gboolean paste_from_position,
					gint *last_x);

void ags_notation_editor_get_boundary(AgsNotationEditor *notation_editor,
				      AgsMachine *machine,
				      AgsNotation *notation,
				      guint *lower, guint *upper);
void ags_notation_editor_invert_notation(AgsNotationEditor *notation_editor,
					 AgsMachine *machine,
					 AgsNotation *notation,
					 guint lower, guint upper);

enum{
  MACHINE_CHANGED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_SOUNDCARD,
};

static gpointer ags_notation_editor_parent_class = NULL;
static guint notation_editor_signals[LAST_SIGNAL];

/**
 * SECTION:ags_notation_editor
 * @short_description: A composite widget to edit notation
 * @title: AgsNotationEditor
 * @section_id:
 * @include: ags/X/ags_notation_editor.h
 *
 * #AgsNotationEditor is a composite widget to edit notation. You may select machines
 * or change editor tool to do notation.
 */

GType
ags_notation_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_notation_editor = 0;

    static const GTypeInfo ags_notation_editor_info = {
      sizeof (AgsNotationEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_notation_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsNotationEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_notation_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_notation_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_notation_editor = g_type_register_static(GTK_TYPE_VBOX,
						      "AgsNotationEditor", &ags_notation_editor_info,
						      0);
    
    g_type_add_interface_static(ags_type_notation_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_notation_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_notation_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_notation_editor_connect;
  connectable->disconnect = ags_notation_editor_disconnect;
}

void
ags_notation_editor_class_init(AgsNotationEditorClass *notation_editor)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  
  GParamSpec *param_spec;

  ags_notation_editor_parent_class = g_type_class_peek_parent(notation_editor);

  /* GObjectClass */
  gobject = (GObjectClass *) notation_editor;

  gobject->set_property = ags_notation_editor_set_property;
  gobject->get_property = ags_notation_editor_get_property;

  gobject->finalize = ags_notation_editor_finalize;
  
  /* properties */
  /**
   * AgsNotationEditor:soundcard:
   *
   * The assigned #AgsSoundcard acting as default sink.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("assigned soundcard"),
				   i18n_pspec("The soundcard it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) notation_editor;
  
  widget->show = ags_notation_editor_show;
  widget->show_all = ags_notation_editor_show_all;
  
  /* AgsEditorClass */
  notation_editor->machine_changed = ags_notation_editor_real_machine_changed;

  /* signals */
  /**
   * AgsEditor::machine-changed:
   * @editor: the object to change machine.
   * @machine: the #AgsMachine to set
   *
   * The ::machine-changed signal notifies about changed machine.
   * 
   * Since: 3.0.0
   */
  notation_editor_signals[MACHINE_CHANGED] =
    g_signal_new("machine-changed",
                 G_TYPE_FROM_CLASS(notation_editor),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsNotationEditorClass, machine_changed),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__OBJECT,
                 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_notation_editor_init(AgsNotationEditor *notation_editor)
{
  GtkViewport *viewport;
  GtkScrolledWindow *scrolled_window;
  GtkTable *table;

  AgsConfig *config;

  gchar *str;
  
  gdouble gui_scale_factor;

  notation_editor->flags = (AGS_NOTATION_EDITOR_PASTE_MATCH_AUDIO_CHANNEL |
			    AGS_NOTATION_EDITOR_PASTE_NO_DUPLICATES);

  notation_editor->version = AGS_NOTATION_EDITOR_DEFAULT_VERSION;
  notation_editor->build_id = AGS_NOTATION_EDITOR_DEFAULT_BUILD_ID;

  config = ags_config_get_instance();

  /* scale factor */
  gui_scale_factor = 1.0;
  
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "gui-scale");

  if(str != NULL){
    gui_scale_factor = g_ascii_strtod(str,
				      NULL);

    g_free(str);
  }

  /* offset */
  notation_editor->tact_counter = 0;
  notation_editor->current_tact = 0.0;

  /* active keys */
  notation_editor->active_key = NULL;
  notation_editor->active_key_count = 0;
  
  /* soundcard */
  notation_editor->soundcard = NULL;

  /* notation toolbar */
  notation_editor->notation_toolbar = ags_notation_toolbar_new();
  gtk_box_pack_start((GtkBox *) notation_editor,
		     (GtkWidget *) notation_editor->notation_toolbar,
		     FALSE, FALSE,
		     0);

  /* paned */
  notation_editor->paned = (GtkHPaned *) gtk_hpaned_new();
  gtk_box_pack_start((GtkBox *) notation_editor,
		     (GtkWidget *) notation_editor->paned,
		     TRUE, TRUE, 0);

  /* machine selector */
  viewport = (GtkViewport *) gtk_viewport_new(NULL,
					      NULL);
  g_object_set(viewport,
	       "shadow-type", GTK_SHADOW_NONE,
	       NULL);
  gtk_paned_pack1((GtkPaned *) notation_editor->paned,
		  (GtkWidget *) viewport,
		  FALSE, TRUE);

  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(viewport),
		    GTK_WIDGET(scrolled_window));

  notation_editor->machine_selector = g_object_new(AGS_TYPE_MACHINE_SELECTOR,
						   "homogeneous", FALSE,
						   "spacing", 0,
						   NULL);
  notation_editor->machine_selector->flags |= (AGS_MACHINE_SELECTOR_SHOW_REVERSE_MAPPING |
					       AGS_MACHINE_SELECTOR_SHOW_SHIFT_PIANO |
					       AGS_MACHINE_SELECTOR_NOTATION);
  gtk_label_set_label(notation_editor->machine_selector->label,
		      i18n("notation"));
  
  notation_editor->machine_selector->popup = ags_machine_selector_popup_new(notation_editor->machine_selector);
  g_object_set(notation_editor->machine_selector->menu_button,
	       "menu", notation_editor->machine_selector->popup,
	       NULL);
  
  gtk_scrolled_window_add_with_viewport(scrolled_window,
					(GtkWidget *) notation_editor->machine_selector);

  /* selected machine */
  notation_editor->selected_machine = NULL;

  /* table */
  viewport = (GtkViewport *) gtk_viewport_new(NULL,
					      NULL);
  g_object_set(viewport,
	       "shadow-type", GTK_SHADOW_NONE,
	       NULL);
  gtk_paned_pack2((GtkPaned *) notation_editor->paned,
		  (GtkWidget *) viewport,
		  TRUE, TRUE);

  table = (GtkTable *) gtk_table_new(3, 2,
				     FALSE);
  gtk_container_add(GTK_CONTAINER(viewport),
		    GTK_WIDGET(table));
  
  /* notebook */
  notation_editor->notebook = g_object_new(AGS_TYPE_NOTEBOOK,
					   "homogeneous", FALSE,
					   "spacing", 0,
					   "prefix", i18n("channel"),
					   NULL);
  gtk_table_attach(table,
		   (GtkWidget *) notation_editor->notebook,
		   0, 3,
		   0, 1,
		   GTK_FILL|GTK_EXPAND, GTK_FILL,
		   0, 0);

  /* scrolled piano */
  notation_editor->scrolled_piano = ags_scrolled_piano_new();
  g_object_set(notation_editor->scrolled_piano,
	       "margin-top", (gint) (gui_scale_factor * AGS_RULER_DEFAULT_HEIGHT),
	       NULL);
  g_object_set(notation_editor->scrolled_piano->piano,
	       "key-width", (guint) (gui_scale_factor * AGS_PIANO_DEFAULT_KEY_WIDTH),
	       "key-height", (guint) (gui_scale_factor * AGS_PIANO_DEFAULT_KEY_HEIGHT),
	       NULL);
  gtk_table_attach(table,
		   (GtkWidget *) notation_editor->scrolled_piano,
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  /* notation edit */
  notation_editor->notation_edit = ags_notation_edit_new();
  gtk_table_attach(table,
		   (GtkWidget *) notation_editor->notation_edit,
		   1, 2,
		   1, 2,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  /* notation meta */
  notation_editor->notation_meta = ags_notation_meta_new();
  g_object_set(notation_editor->notation_meta,
	       "valign", GTK_ALIGN_START,
	       NULL);  
  gtk_table_attach(table,
		   (GtkWidget *) notation_editor->notation_meta,
		   2, 3,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_notation_editor_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsNotationEditor *notation_editor;

  notation_editor = AGS_NOTATION_EDITOR(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = g_value_get_object(value);

      if(notation_editor->soundcard == soundcard){
	return;
      }

      if(notation_editor->soundcard != NULL){
	g_object_unref(notation_editor->soundcard);
      }
      
      if(soundcard != NULL){
	g_object_ref(soundcard);
      }
      
      notation_editor->soundcard = soundcard;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_notation_editor_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsNotationEditor *notation_editor;

  notation_editor = AGS_NOTATION_EDITOR(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, notation_editor->soundcard);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_notation_editor_finalize(GObject *gobject)
{
  AgsNotationEditor *notation_editor;

  notation_editor = AGS_NOTATION_EDITOR(gobject);

  if(notation_editor->soundcard != NULL){
    g_object_unref(notation_editor->soundcard);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_notation_editor_parent_class)->finalize(gobject);
}

void
ags_notation_editor_connect(AgsConnectable *connectable)
{
  AgsNotationEditor *notation_editor;

  notation_editor = AGS_NOTATION_EDITOR(connectable);

  if((AGS_NOTATION_EDITOR_CONNECTED & (notation_editor->flags)) != 0){
    return;
  }

  notation_editor->flags |= AGS_NOTATION_EDITOR_CONNECTED;  
  
  g_signal_connect((GObject *) notation_editor->machine_selector, "changed",
		   G_CALLBACK(ags_notation_editor_machine_changed_callback), (gpointer) notation_editor);

  g_signal_connect((GObject *) notation_editor->scrolled_piano->piano, "key-pressed",
		   G_CALLBACK(ags_notation_editor_piano_key_pressed_callback), (gpointer) notation_editor);

  g_signal_connect((GObject *) notation_editor->scrolled_piano->piano, "key-released",
		   G_CALLBACK(ags_notation_editor_piano_key_released_callback), (gpointer) notation_editor);

  
  /* toolbar */
  ags_connectable_connect(AGS_CONNECTABLE(notation_editor->notation_toolbar));

  /* machine selector */
  ags_connectable_connect(AGS_CONNECTABLE(notation_editor->machine_selector));

  /* notation edit */
  ags_connectable_connect(AGS_CONNECTABLE(notation_editor->notation_edit));

  /* notation meta */
  ags_connectable_connect(AGS_CONNECTABLE(notation_editor->notation_meta));
}

void
ags_notation_editor_disconnect(AgsConnectable *connectable)
{
  AgsNotationEditor *notation_editor;

  notation_editor = AGS_NOTATION_EDITOR(connectable);

  if((AGS_NOTATION_EDITOR_CONNECTED & (notation_editor->flags)) == 0){
    return;
  }

  notation_editor->flags &= (~AGS_NOTATION_EDITOR_CONNECTED);

  g_object_disconnect((GObject *) notation_editor->machine_selector,
		      "any_signal::changed",
		      G_CALLBACK(ags_notation_editor_machine_changed_callback),
		      (gpointer) notation_editor,
		      NULL);

  g_object_disconnect((GObject *) notation_editor->scrolled_piano->piano,
		      "any_signal::key-pressed",
		      G_CALLBACK(ags_notation_editor_piano_key_pressed_callback),
		      (gpointer) notation_editor,
		      "any_signal::key-released",
		      G_CALLBACK(ags_notation_editor_piano_key_released_callback),
		      (gpointer) notation_editor,
		      NULL);

  /* notation toolbar */
  ags_connectable_disconnect(AGS_CONNECTABLE(notation_editor->notation_toolbar)); 

  /* machine selector */
  ags_connectable_disconnect(AGS_CONNECTABLE(notation_editor->machine_selector));

  /* notation edit */
  ags_connectable_disconnect(AGS_CONNECTABLE(notation_editor->notation_edit));

  /* notation meta */
  ags_connectable_disconnect(AGS_CONNECTABLE(notation_editor->notation_meta));
}

void
ags_notation_editor_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_notation_editor_parent_class)->show(widget);

  gtk_widget_hide(AGS_NOTATION_EDITOR(widget)->notation_meta);
}

void
ags_notation_editor_show_all(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_notation_editor_parent_class)->show_all(widget);

  gtk_widget_hide(AGS_NOTATION_EDITOR(widget)->notation_meta);
}

void
ags_notation_editor_real_machine_changed(AgsNotationEditor *notation_editor,
					 AgsMachine *machine)
{
  AgsMachine *old_machine;

  GList *tab;

  guint length;
  guint audio_channels;
  guint i;

  /* disconnect set pads - old */
  old_machine = notation_editor->selected_machine;

  if(old_machine != NULL){
    g_object_disconnect(old_machine,
			"any_signal::resize-audio-channels",
			G_CALLBACK(ags_notation_editor_resize_audio_channels_callback),
			(gpointer) notation_editor,
			"any_signal::resize-pads",
			G_CALLBACK(ags_notation_editor_resize_pads_callback),
			(gpointer) notation_editor,
			NULL);
  }
  
  /* notebook - remove tabs */
  length = g_list_length(notation_editor->notebook->tab);
  
  for(i = 0; i < length; i++){
    ags_notebook_remove_tab(notation_editor->notebook,
			    0);
  }

  /* check pattern mode */
  if(AGS_IS_DRUM(machine) ||
     AGS_IS_MATRIX(machine)){
    notation_editor->flags |= AGS_NOTATION_EDITOR_PATTERN_MODE;
  }else{
    notation_editor->flags &= (~AGS_NOTATION_EDITOR_PATTERN_MODE);
  }
  
  /* notebook - add tabs */
  if(machine != NULL){
    g_object_get(machine->audio,
		 "audio-channels", &audio_channels,
		 NULL);
    
    for(i = 0; i < audio_channels; i++){
      ags_notebook_insert_tab(notation_editor->notebook,
			      i);

      tab = notation_editor->notebook->tab;
      gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(tab->data)->toggle,
				   TRUE);
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
    g_object_set(notation_editor->scrolled_piano->piano,
		 "key-count", channel_count,
		 NULL);
  }else{
    /* apply default */
    g_object_set(notation_editor->scrolled_piano->piano,
		 "key-count", AGS_PIANO_DEFAULT_KEY_COUNT,
		 NULL);
  }
  
  gtk_widget_queue_draw((GtkWidget *) notation_editor->scrolled_piano->piano);
  gtk_widget_queue_draw((GtkWidget *) notation_editor->scrolled_piano);
  
  /* selected machine */
  notation_editor->selected_machine = machine;

  /* reset scrollbars */
  ags_notation_edit_reset_vscrollbar(notation_editor->notation_edit);
  ags_notation_edit_reset_hscrollbar(notation_editor->notation_edit);

  /* redraw */
  gtk_widget_queue_draw((GtkWidget *) notation_editor->notation_edit);

  /* connect set-pads - new */
  if(machine != NULL){
    g_signal_connect_after(machine, "resize-audio-channels",
			   G_CALLBACK(ags_notation_editor_resize_audio_channels_callback), notation_editor);

    g_signal_connect_after(machine, "resize-pads",
			   G_CALLBACK(ags_notation_editor_resize_pads_callback), notation_editor);
  }  
}

/**
 * ags_notation_editor_machine_changed:
 * @notation_editor: the #AgsNotationEditor
 * @machine: the new #AgsMachine
 *
 * Is emitted as machine changed of notation_editor.
 *
 * Since: 3.0.0
 */
void
ags_notation_editor_machine_changed(AgsNotationEditor *notation_editor,
				    AgsMachine *machine)
{
  g_return_if_fail(AGS_IS_NOTATION_EDITOR(notation_editor));

  g_object_ref((GObject *) notation_editor);
  g_signal_emit((GObject *) notation_editor,
		notation_editor_signals[MACHINE_CHANGED], 0,
		machine);
  g_object_unref((GObject *) notation_editor);
}

/**
 * ags_notation_editor_add_note:
 * @notation_editor: the #AgsNotationEditor
 * @note: the #AgsNote to add
 * 
 * Add note.
 *
 * Since: 3.0.0
 */
void
ags_notation_editor_add_note(AgsNotationEditor *notation_editor,
			     AgsNote *note)
{
  AgsMachine *machine;
  
  AgsNotation *notation;

  AgsTimestamp *timestamp;

  GList *start_list_notation, *list_notation;

  guint x0;
  gint i;
  
  if(!AGS_IS_NOTATION_EDITOR(notation_editor) ||
     !AGS_IS_NOTE(note)){
    return;
  }

  if(notation_editor->selected_machine != NULL){
    machine = notation_editor->selected_machine;

    /* check all active tabs */
    timestamp = ags_timestamp_new();

    timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
    timestamp->flags |= AGS_TIMESTAMP_OFFSET;

    g_object_get(note,
		 "x0", &x0,
		 NULL);
    
    timestamp->timer.ags_offset.offset = (guint64) AGS_NOTATION_DEFAULT_OFFSET * floor((double) x0 / (double) AGS_NOTATION_DEFAULT_OFFSET);

    i = 0;

    while((i = ags_notebook_next_active_tab(notation_editor->notebook,
					    i)) != -1){
      AgsNote *new_note;

      g_object_get(machine->audio,
		   "notation", &start_list_notation,
		   NULL);
      
      list_notation = ags_notation_find_near_timestamp(start_list_notation, i,
						       timestamp);

      if(list_notation != NULL){
	notation = list_notation->data;
      }else{
	notation = ags_notation_new((GObject *) machine->audio,
				    i);
	AGS_TIMESTAMP(notation->timestamp)->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;
	
	ags_audio_add_notation(machine->audio,
			       (GObject *) notation);
      }

      new_note = ags_note_duplicate(note);
      ags_notation_add_note(notation,
			    new_note,
			    FALSE);

      g_list_free_full(start_list_notation,
		       g_object_unref);

      /* iterate */
      i++;
    }

    g_object_unref(timestamp);
    
    gtk_widget_queue_draw((GtkWidget *) notation_editor->notation_edit);
  }
}

/**
 * ags_notation_editor_delete_note:
 * @notation_editor: the #AgsNotationEditor
 * @x: point x
 * @y: point y
 * 
 * Delete note.
 *
 * Since: 3.0.0
 */
void
ags_notation_editor_delete_note(AgsNotationEditor *notation_editor,
				guint x, guint y)
{
  AgsMachine *machine;
  
  AgsNotation *notation;

  AgsTimestamp *timestamp;

  GList *start_list_notation, *list_notation;

  gint i;

  if(!AGS_IS_NOTATION_EDITOR(notation_editor)){
    return;
  }
  
  if(notation_editor->selected_machine != NULL){
    machine = notation_editor->selected_machine;
  
    /* check all active tabs */
    timestamp = ags_timestamp_new();

    timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
    timestamp->flags |= AGS_TIMESTAMP_OFFSET;
    
    timestamp->timer.ags_offset.offset = (guint64) AGS_NOTATION_DEFAULT_OFFSET * floor((double) x / (double) AGS_NOTATION_DEFAULT_OFFSET);

    i = 0;

    while((i = ags_notebook_next_active_tab(notation_editor->notebook,
					    i)) != -1){
      AgsNote *new_note;

      g_object_get(machine->audio,
		   "notation", &start_list_notation,
		   NULL);
      
      list_notation = ags_notation_find_near_timestamp(start_list_notation, i,
						       timestamp);
      
      if(list_notation != NULL){
	notation = list_notation->data;
	ags_notation_remove_note_at_position(notation,
					     x, y);
      }

      g_list_free_full(start_list_notation,
		       g_object_unref);
       
      /* iterate */
      i++;
    }

    g_object_unref(timestamp);
    
    gtk_widget_queue_draw((GtkWidget *) notation_editor->notation_edit);
  }
}

/**
 * ags_notation_editor_select_region:
 * @notation_editor: the #AgsNotationEditor
 * @x0: point x0
 * @y0: point y0
 * @x1: point x1
 * @y1: point y1
 * 
 * Select region.
 *
 * Since: 3.0.0
 */
void
ags_notation_editor_select_region(AgsNotationEditor *notation_editor,
				  guint x0, guint y0,
				  guint x1, guint y1)
{
  AgsMachine *machine;
  
  AgsNotation *notation;

  AgsTimestamp *timestamp;

  GList *start_list_notation, *list_notation;

  gint i;

  if(!AGS_IS_NOTATION_EDITOR(notation_editor)){
    return;
  }
  
  if(notation_editor->selected_machine != NULL){
    machine = notation_editor->selected_machine;
  
    /* swap values if needed */
    if(x0 > x1){
      guint tmp;

      tmp = x0;
      
      x0 = x1;
      x1 = tmp;
    }

    if(y0 > y1){
      guint tmp;

      tmp = y0;
      
      y0 = y1;
      y1 = tmp;
    }

    /* check all active tabs */
    timestamp = ags_timestamp_new();

    timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
    timestamp->flags |= AGS_TIMESTAMP_OFFSET;
    
    i = 0;

    g_object_get(machine->audio,
		 "notation", &start_list_notation,
		 NULL);
    
    while((i = ags_notebook_next_active_tab(notation_editor->notebook,
					    i)) != -1){      
      list_notation = start_list_notation;
      
      timestamp->timer.ags_offset.offset = AGS_NOTATION_DEFAULT_OFFSET * floor(x0 / AGS_NOTATION_DEFAULT_OFFSET);
      
      while((list_notation = ags_notation_find_near_timestamp(list_notation, i,
							      timestamp)) != NULL &&
	    timestamp->timer.ags_offset.offset < (AGS_NOTATION_DEFAULT_OFFSET * floor(x1 / AGS_NOTATION_DEFAULT_OFFSET)) + AGS_NOTATION_DEFAULT_OFFSET){
	ags_notation_add_region_to_selection(list_notation->data,
					     x0, y0,
					     x1, y1,
					     TRUE);
	
	/* iterate */
	timestamp->timer.ags_offset.offset += AGS_NOTATION_DEFAULT_OFFSET;
	
	list_notation = list_notation->next;
      }

      /* iterate */
      i++;
    }

    g_list_free_full(start_list_notation,
		     g_object_unref);
    
    gtk_widget_queue_draw((GtkWidget *) notation_editor->notation_edit);
  }
}

/**
 * ags_notation_editor_do_feedback:
 * @notation_editor: the #AgsNotationEditor
 * 
 * Do playback feedback.
 *
 * Since: 3.0.0
 */
void
ags_notation_editor_do_feedback(AgsNotationEditor *notation_editor)
{
  if(!AGS_IS_NOTATION_EDITOR(notation_editor)){
    return;
  }
  
  if(notation_editor->selected_machine != NULL){
    AgsNotationEdit *notation_edit;
    AgsMachine *machine;
  
    AgsChannel *start_output, *start_input;
    AgsChannel *channel, *nth_channel, *nth_pad;
    AgsPlayback *playback;
    AgsNotation *notation;
    
    AgsTimestamp *timestamp;
    
    GList *start_list_notation, *list_notation;
    
    guint output_pads, input_pads;
    gint i;

    notation_edit = notation_editor->notation_edit;
    machine = notation_editor->selected_machine;
  
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

    while((i = ags_notebook_next_active_tab(notation_editor->notebook,
					    i)) != -1){
      AgsNote *current_note;
      AgsNote *play_note;
      
      list_notation = start_list_notation;
      list_notation = ags_notation_find_near_timestamp(list_notation, i,
						       timestamp);

      if(list_notation != NULL){
	notation = list_notation->data;
      }else{
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
}

/**
 * ags_notation_editor_start_play_key:
 * @notation_editor: the #AgsNotationEditor
 * @key_code: the key to play
 * 
 * Start play @key_code.
 *
 * Since: 3.0.0
 */
void
ags_notation_editor_start_play_key(AgsNotationEditor *notation_editor,
				   gint key_code)
{
  if(!AGS_IS_NOTATION_EDITOR(notation_editor)){
    return;
  }

  if(notation_editor->selected_machine != NULL){
    AgsMachine *machine;

    AgsChannel *start_output, *start_input;
    AgsChannel *channel, *nth_channel, *nth_pad;
    AgsPlayback *playback;

    GObject *output_soundcard;
    
    guint output_pads, input_pads;
    guint audio_channels;
    guint note_offset;
    guint y;
    gint i;

    machine = notation_editor->selected_machine;

    i = 0;

    g_object_get(machine->audio,
		 "output-soundcard", &output_soundcard,
		 "output", &start_output,
		 "output-pads", &output_pads,
		 "input", &start_input,
		 "input-pads", &input_pads,
		 "audio-channels", &audio_channels,
		 NULL);

    for(i = 0; i < audio_channels; i++){
      AgsNote *play_note;

      //TODO:JK: improve me
      y = key_code;

#if 0      
      if(ags_audio_test_behaviour_flags(machine->audio, AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_OUTPUT)){
	nth_channel = ags_channel_nth(start_output,
				      i);
      }else{
	nth_channel = ags_channel_nth(start_input,
				      i);
      }
      
      if(ags_audio_test_behaviour_flags(machine->audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
	nth_pad = ags_channel_pad_nth(nth_channel,
				      (ags_audio_test_behaviour_flags(machine->audio, AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_OUTPUT) ? output_pads: input_pads) - y - 1);
      }else{
	nth_pad = ags_channel_pad_nth(nth_channel,
				      y);
      }
#else
      nth_channel = ags_channel_nth(start_input,
				    i);
      
      if(ags_audio_test_behaviour_flags(machine->audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
	nth_pad = ags_channel_pad_nth(nth_channel,
				      (input_pads) - y - 1);
      }else{
	nth_pad = ags_channel_pad_nth(nth_channel,
				      y);
      }
#endif
      
      if(nth_pad != NULL){
	g_object_get(nth_pad,
		     "playback", &playback,
		     NULL);

	g_object_get(playback,
		     "play-note", &play_note,
		     NULL);

	note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(output_soundcard));

	ags_note_set_flags(play_note, AGS_NOTE_FEED);
	g_object_set(play_note,
		     "x0", note_offset,
		     "x1", note_offset + 1,
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

    /* unref */
    if(start_output != NULL){
      g_object_unref(start_output);
    }

    if(start_input != NULL){
      g_object_unref(start_input);
    }
  }
}

/**
 * ags_notation_editor_stop_play_key:
 * @notation_editor: the #AgsNotationEditor
 * @key_code: the key to stop
 * 
 * Stop play @key_code.
 *
 * Since: 3.0.0
 */
void
ags_notation_editor_stop_play_key(AgsNotationEditor *notation_editor,
				  gint key_code)
{
  if(!AGS_IS_NOTATION_EDITOR(notation_editor)){
    return;
  }

  if(notation_editor->selected_machine != NULL){
    AgsMachine *machine;

    AgsChannel *start_output, *start_input;
    AgsChannel *channel, *nth_channel, *nth_pad;
    AgsPlayback *playback;
    
    guint output_pads, input_pads;
    guint audio_channels;
    guint y;
    gint i;

    machine = notation_editor->selected_machine;

    i = 0;

    g_object_get(machine->audio,
		 "output", &start_output,
		 "output-pads", &output_pads,
		 "input", &start_input,
		 "input-pads", &input_pads,
		 "audio-channels", &audio_channels,
		 NULL);

    for(i = 0; i < audio_channels; i++){
      AgsNote *play_note;

      //TODO:JK: improve me
      y = key_code;
      
#if 0      
      if(ags_audio_test_behaviour_flags(machine->audio, AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_OUTPUT)){
	nth_channel = ags_channel_nth(start_output,
				      i);
      }else{
	nth_channel = ags_channel_nth(start_input,
				      i);
      }

      if(ags_audio_test_behaviour_flags(machine->audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
	nth_pad = ags_channel_pad_nth(nth_channel,
				      (ags_audio_test_behaviour_flags(machine->audio, AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_OUTPUT) ? output_pads: input_pads) - y - 1);
      }else{
	nth_pad = ags_channel_pad_nth(nth_channel,
				      y);
      }
#else
      nth_channel = ags_channel_nth(start_input,
				    i);

      if(ags_audio_test_behaviour_flags(machine->audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
	nth_pad = ags_channel_pad_nth(nth_channel,
				      (input_pads) - y - 1);
      }else{
	nth_pad = ags_channel_pad_nth(nth_channel,
				      y);
      }
#endif

      if(nth_pad != NULL){
	g_object_get(nth_pad,
		     "playback", &playback,
		     NULL);

	g_object_get(playback,
		     "play-note", &play_note,
		     NULL);

	ags_note_unset_flags(play_note, AGS_NOTE_FEED);
	  
	ags_machine_playback_set_active(machine,
					playback,
					FALSE);
	  
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

    /* unref */
    if(start_output != NULL){
      g_object_unref(start_output);
    }

    if(start_input != NULL){
      g_object_unref(start_input);
    }
  }
}

/**
 * ags_notation_editor_select_all:
 * @notation_editor: the #AgsNotationEditor
 *
 * Is emitted as machine changed of notation_editor.
 *
 * Since: 3.0.0
 */
void
ags_notation_editor_select_all(AgsNotationEditor *notation_editor)
{
  AgsMachine *machine;
      
  GList *start_list_notation, *list_notation;

  guint audio_channel;
  gint i;

  if(!AGS_IS_NOTATION_EDITOR(notation_editor)){
    return;
  }
  
  if(notation_editor->selected_machine != NULL){
    machine = notation_editor->selected_machine;

    /* check all active tabs */
    g_object_get(machine->audio,
		 "notation", &start_list_notation,
		 NULL);

    i = 0;
    
    while((i = ags_notebook_next_active_tab(notation_editor->notebook,
					    i)) != -1){
      list_notation = start_list_notation;
      
      while(list_notation != NULL){
	g_object_get(list_notation->data,
		     "audio-channel", &audio_channel,
		     NULL);

	if(i != audio_channel){	
	  list_notation = list_notation->next;

	  continue;
	}
	
	ags_notation_add_all_to_selection(AGS_NOTATION(list_notation->data));
	
	list_notation = list_notation->next;
      }

      /* iterate */
      i++;
    }

    g_list_free_full(start_list_notation,
		     g_object_unref);
    
    gtk_widget_queue_draw((GtkWidget *) notation_editor->notation_edit);
  }
}

gint
ags_notation_editor_paste_notation_all(AgsNotationEditor *notation_editor,
				       AgsMachine *machine,
				       xmlNode *notation_node,
				       AgsTimestamp *timestamp,
				       gboolean match_channel, gboolean no_duplicates,
				       guint position_x, guint position_y,
				       gboolean paste_from_position,
				       gint *last_x)
{    
  AgsNotation *notation;
		
  GList *start_list_notation, *list_notation;
    
  gint first_x;
  guint current_x;
  gint i;

  first_x = -1;
    
  /*  */
  i = 0;
		
  while((i = ags_notebook_next_active_tab(notation_editor->notebook,
					  i)) != -1){
    g_object_get(machine->audio,
		 "notation", &start_list_notation,
		 NULL);
      
    list_notation = ags_notation_find_near_timestamp(start_list_notation, i,
						     timestamp);

    if(list_notation == NULL){
      notation = ags_notation_new((GObject *) machine->audio,
				  i);
      notation->timestamp->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;
	
      ags_audio_add_notation(machine->audio,
			     (GObject *) notation);
    }else{
      notation = AGS_NOTATION(list_notation->data);
    }
		  
    g_list_free_full(start_list_notation,
		     g_object_unref);

    if(paste_from_position){
      xmlNode *child;

      guint x_boundary;
	  
      ags_notation_insert_from_clipboard_extended(notation,
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
			 "note",
			 5)){
	    guint tmp;

	    tmp = g_ascii_strtoull(xmlGetProp(child,
					      "x1"),
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
					       "x_boundary"),
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

      ags_notation_insert_from_clipboard(notation,
					 notation_node,
					 FALSE, 0,
					 FALSE, 0);

      /* get boundaries */
      child = notation_node->children;
      current_x = 0;
	  
      while(child != NULL){
	if(child->type == XML_ELEMENT_NODE){
	  if(!xmlStrncmp(child->name,
			 "note",
			 5)){
	    guint tmp;

	    tmp = g_ascii_strtoull(xmlGetProp(child,
					      "x1"),
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
ags_notation_editor_paste_notation(AgsNotationEditor *notation_editor,
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

  match_channel = ((AGS_NOTATION_EDITOR_PASTE_MATCH_AUDIO_CHANNEL & (notation_editor->flags)) != 0) ? TRUE: FALSE;
  no_duplicates = ((AGS_NOTATION_EDITOR_PASTE_NO_DUPLICATES & (notation_editor->flags)) != 0) ? TRUE: FALSE;

  /* timestamp */
  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;
    
  timestamp->timer.ags_offset.offset = 0;
    
  /* paste notation */
  notation_list_node = audio_node->children;

  while(notation_list_node != NULL){
    if(notation_list_node->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(notation_list_node->name,
		     "notation-list",
		     14)){
	notation_node = notation_list_node->children;
	  
	while(notation_node != NULL){
	  if(notation_node->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(notation_node->name,
			   "notation",
			   9)){
	      guint64 offset;
		
	      timestamp_node = notation_node->children;
	      offset = 0;
	  
	      while(timestamp_node != NULL){
		if(timestamp_node->type == XML_ELEMENT_NODE){
		  if(!xmlStrncmp(timestamp_node->name,
				 "timestamp",
				 10)){
		    offset = g_ascii_strtoull(xmlGetProp(timestamp_node,
							 "offset"),
					      NULL,
					      10);
		      
		    break;
		  }
		}

		timestamp_node = timestamp_node->next;
	      }     

	      /* 1st attempt */
	      timestamp->timer.ags_offset.offset = (guint64) AGS_NOTATION_DEFAULT_OFFSET * floor((double) position_x / (double) AGS_NOTATION_DEFAULT_OFFSET);
		
	      first_x = ags_notation_editor_paste_notation_all(notation_editor,
							       machine,
							       notation_node,
							       timestamp,
							       match_channel, no_duplicates,
							       position_x, position_y,
							       paste_from_position,
							       last_x);

	      /* 2nd attempt */
	      timestamp->timer.ags_offset.offset += AGS_NOTATION_DEFAULT_OFFSET;

	      ags_notation_editor_paste_notation_all(notation_editor,
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

/**
 * ags_notation_editor_paste:
 * @notation_editor: the #AgsNotationEditor
 *
 * Is emitted as machine changed of notation_editor.
 *
 * Since: 3.0.0
 */
void
ags_notation_editor_paste(AgsNotationEditor *notation_editor)
{
  AgsMachine *machine;
  AgsNotationEdit *notation_edit;
  
  AgsNotation *notation;

  xmlDoc *clipboard;
  xmlNode *audio_node;
  xmlNode *notation_node;
  
  gchar *buffer;

  guint position_x, position_y;
  gint first_x, last_x;
  gboolean paste_from_position;

  if(!AGS_IS_NOTATION_EDITOR(notation_editor)){
    return;
  }
  
  if((machine = notation_editor->selected_machine) != NULL){
    notation_edit = notation_editor->notation_edit;

    /* get clipboard */
    buffer = gtk_clipboard_wait_for_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
    
    if(buffer == NULL){
      return;
    }

    position_x = 0;
    position_y = 0;

    /* get position */
    if(notation_editor->notation_toolbar->selected_edit_mode == notation_editor->notation_toolbar->position){
      last_x = 0;
      paste_from_position = TRUE;

      position_x = notation_editor->notation_edit->cursor_position_x;
      position_y = notation_editor->notation_edit->cursor_position_y;
      
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
	if(!xmlStrncmp("audio", audio_node->name, 6)){
	  notation_node = audio_node->children;
	  
	  first_x = ags_notation_editor_paste_notation(notation_editor,
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
    
    xmlFreeDoc(clipboard); 

    if(paste_from_position){
      gint big_step, small_step;

      //TODO:JK: implement me
      big_step = (guint) ceil((double) last_x / 16.0) * 16.0 + (notation_edit->cursor_position_x % (guint) 16);
      small_step = (guint) big_step - 16;
	
      if(small_step < last_x){
	notation_editor->notation_edit->cursor_position_x = big_step;
      }else{
	notation_editor->notation_edit->cursor_position_x = small_step;
      }
    }

    gtk_widget_queue_draw((GtkWidget *) notation_editor->notation_edit);
  }
}

/**
 * ags_notation_editor_copy:
 * @notation_editor: the #AgsNotationEditor
 *
 * Is emitted as machine changed of notation_editor.
 *
 * Since: 3.0.0
 */
void
ags_notation_editor_copy(AgsNotationEditor *notation_editor)
{
  AgsMachine *machine;
  
  AgsNotation *notation;

  xmlDoc *clipboard;
  xmlNode *audio_node, *notation_list_node, *notation_node;

  GList *start_list_notation, *list_notation;

  xmlChar *buffer;

  int size;
  guint audio_channel;
  gint i;

  if(!AGS_IS_NOTATION_EDITOR(notation_editor)){
    return;
  }
  
  if(notation_editor->selected_machine != NULL){
    machine = notation_editor->selected_machine;

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
		 "notation", &start_list_notation,
		 NULL);

    i = 0;

    while((i = ags_notebook_next_active_tab(notation_editor->notebook,
					    i)) != -1){
      list_notation = start_list_notation;

      /* copy */
      while(list_notation != NULL){
	g_object_get(list_notation->data,
		     "audio-channel", &audio_channel,
		     NULL);

	if(i != audio_channel){	
	  list_notation = list_notation->next;

	  continue;
	}

	notation_node = ags_notation_copy_selection(AGS_NOTATION(list_notation->data));
	xmlAddChild(notation_list_node,
		    notation_node);
	
	list_notation = list_notation->next;
      }

      /* iterate */
      i++;
    }

    g_list_free_full(start_list_notation,
		     g_object_unref);
    
    /* write to clipboard */
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8", TRUE);
    gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
			   buffer, size);
    gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));

    xmlFreeDoc(clipboard);
  }
}

/**
 * ags_notation_editor_cut:
 * @notation_editor: the #AgsNotationEditor
 *
 * Is emitted as machine changed of notation_editor.
 *
 * Since: 3.0.0
 */
void
ags_notation_editor_cut(AgsNotationEditor *notation_editor)
{
  AgsMachine *machine;
  
  AgsNotation *notation;

  xmlDoc *clipboard;
  xmlNode *audio_node;
  xmlNode *notation_list_node, *notation_node;

  GList *start_list_notation, *list_notation;

  xmlChar *buffer;

  int size;
  guint audio_channel;
  gint i;

  if(!AGS_IS_NOTATION_EDITOR(notation_editor)){
    return;
  }
  
  if(notation_editor->selected_machine != NULL){
    machine = notation_editor->selected_machine;

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
		 "notation", &start_list_notation,
		 NULL);

    i = 0;
    
    while((i = ags_notebook_next_active_tab(notation_editor->notebook,
					    i)) != -1){
      list_notation = start_list_notation;

      /* cut */
      while(list_notation != NULL){
	g_object_get(list_notation->data,
		     "audio-channel", &audio_channel,
		     NULL);

	if(i != audio_channel){	
	  list_notation = list_notation->next;

	  continue;
	}

	notation_node = ags_notation_cut_selection(AGS_NOTATION(list_notation->data));
	xmlAddChild(notation_list_node,
		    notation_node);
	
	list_notation = list_notation->next;
      }

      /* iterate */
      i++;
    }

    g_list_free_full(start_list_notation,
		     g_object_unref);

    gtk_widget_queue_draw((GtkWidget *) notation_editor->notation_edit);

    /* write to clipboard */
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8", TRUE);
    gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
			   buffer, size);
    gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));

    xmlFreeDoc(clipboard);
  }
}


void
ags_notation_editor_get_boundary(AgsNotationEditor *notation_editor,
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
ags_notation_editor_invert_notation(AgsNotationEditor *notation_editor,
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
 * ags_notation_editor_invert:
 * @notation_editor: the #AgsNotationEditor
 *
 * Invert all notation of @notation_editor's selected machine.
 *
 * Since: 3.0.0
 */
void
ags_notation_editor_invert(AgsNotationEditor *notation_editor)
{
  AgsMachine *machine;
  
  AgsNotation *notation;

  GList *start_list_notation, *list_notation;

  int size;
  guint audio_channel;
  gint i;
  
  if(!AGS_IS_NOTATION_EDITOR(notation_editor)){
    return;
  }
  
  if(notation_editor->selected_machine != NULL){
    guint lower, upper;

    /* create notation nodes */
    machine = notation_editor->selected_machine;

    g_object_get(machine->audio,
		 "notation", &start_list_notation,
		 NULL);
    
    i = 0;
    
    while((i = ags_notebook_next_active_tab(notation_editor->notebook,
					    i)) != -1){
      /* get boundary */
      list_notation = start_list_notation;

      lower = G_MAXUINT;
      upper = 0;

      while(list_notation != NULL){
	g_object_get(list_notation->data,
		     "audio-channel", &audio_channel,
		     NULL);

	if(i != audio_channel){	
	  list_notation = list_notation->next;

	  continue;
	}

	ags_notation_editor_get_boundary(notation_editor,
					 machine,
					 AGS_NOTATION(list_notation->data),
					 &lower, &upper);

	list_notation = list_notation->next;
      }
      
      /* invert */
      list_notation = start_list_notation;
      
      while(list_notation != NULL){
	g_object_get(list_notation->data,
		     "audio-channel", &audio_channel,
		     NULL);

	if(i != audio_channel){	
	  list_notation = list_notation->next;

	  continue;
	}

	ags_notation_editor_invert_notation(notation_editor,
					    machine,
					    AGS_NOTATION(list_notation->data),
					    lower, upper);

	list_notation = list_notation->next;
      }
      
      i++;
    }

    g_list_free_full(start_list_notation,
		     g_object_unref);
    
    gtk_widget_queue_draw((GtkWidget *) notation_editor->notation_edit);
  }
}

/**
 * ags_notation_editor_new:
 *
 * Creates the #AgsNotationEditor
 *
 * Returns: a new #AgsNotationEditor
 *
 * Since: 3.0.0
 */
AgsNotationEditor*
ags_notation_editor_new()
{
  AgsNotationEditor *notation_editor;

  notation_editor = (AgsNotationEditor *) g_object_new(AGS_TYPE_NOTATION_EDITOR,
						       NULL);

  return(notation_editor);
}
