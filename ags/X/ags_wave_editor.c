/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/ags_wave_editor.h>
#include <ags/X/ags_wave_editor_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_window.h>

#include <ags/X/editor/ags_scrolled_wave_edit_box.h>
#include <ags/X/editor/ags_vwave_edit_box.h>
#include <ags/X/editor/ags_wave_edit.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <math.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_wave_editor_class_init(AgsWaveEditorClass *wave_editor);
void ags_wave_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_wave_editor_init(AgsWaveEditor *wave_editor);
void ags_wave_editor_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_wave_editor_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_wave_editor_connect(AgsConnectable *connectable);
void ags_wave_editor_disconnect(AgsConnectable *connectable);
void ags_wave_editor_finalize(GObject *gobject);

void ags_wave_editor_real_machine_changed(AgsWaveEditor *wave_editor, AgsMachine *machine);

enum{
  MACHINE_CHANGED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_SOUNDCARD,
};

static gpointer ags_wave_editor_parent_class = NULL;
static guint wave_editor_signals[LAST_SIGNAL];

/**
 * SECTION:ags_wave_editor
 * @short_description: A composite widget to edit wave
 * @title: AgsWaveEditor
 * @section_id:
 * @include: ags/X/ags_wave_editor.h
 *
 * #AgsWaveEditor is a composite widget to edit wave. You may select machines
 * or change editor tool to do wave.
 */

GType
ags_wave_editor_get_type(void)
{
  static GType ags_type_wave_editor = 0;

  if(!ags_type_wave_editor){
    static const GTypeInfo ags_wave_editor_info = {
      sizeof (AgsWaveEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_wave_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsWaveEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_wave_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_wave_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_wave_editor = g_type_register_static(GTK_TYPE_VBOX,
						  "AgsWaveEditor", &ags_wave_editor_info,
						  0);
    
    g_type_add_interface_static(ags_type_wave_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_wave_editor);
}

void
ags_wave_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_wave_editor_connect;
  connectable->disconnect = ags_wave_editor_disconnect;
}

void
ags_wave_editor_class_init(AgsWaveEditorClass *wave_editor)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_wave_editor_parent_class = g_type_class_peek_parent(wave_editor);

  /* GObjectClass */
  gobject = (GObjectClass *) wave_editor;

  gobject->set_property = ags_wave_editor_set_property;
  gobject->get_property = ags_wave_editor_get_property;

  gobject->finalize = ags_wave_editor_finalize;
  
  /* properties */
  /**
   * AgsWaveEditor:soundcard:
   *
   * The assigned #AgsSoundcard acting as default sink.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("assigned soundcard"),
				   i18n_pspec("The soundcard it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /* AgsEditorClass */
  wave_editor->machine_changed = ags_wave_editor_real_machine_changed;

  /* signals */
  /**
   * AgsEditor::machine-changed:
   * @editor: the object to change machine.
   * @machine: the #AgsMachine to set
   *
   * The ::machine-changed signal notifies about changed machine.
   * 
   * Since: 2.0.0
   */
  wave_editor_signals[MACHINE_CHANGED] =
    g_signal_new("machine-changed",
                 G_TYPE_FROM_CLASS(wave_editor),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsWaveEditorClass, machine_changed),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__OBJECT,
                 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_wave_editor_init(AgsWaveEditor *wave_editor)
{
  GtkViewport *viewport;
  GtkScrolledWindow *scrolled_window;  
  GtkTable *table;
  
  GtkAdjustment *adjustment;

  g_signal_connect_after((GObject *) wave_editor, "parent-set",
			 G_CALLBACK(ags_wave_editor_parent_set_callback), wave_editor);

  wave_editor->flags = 0;

  wave_editor->version = AGS_WAVE_EDITOR_DEFAULT_VERSION;
  wave_editor->build_id = AGS_WAVE_EDITOR_DEFAULT_BUILD_ID;

  /* offset */
  wave_editor->tact_counter = 0;
  wave_editor->current_tact = 0.0;

  /* soundcard */
  wave_editor->soundcard = NULL;

  wave_editor->wave_toolbar = ags_wave_toolbar_new();
  gtk_box_pack_start((GtkBox *) wave_editor,
		     (GtkWidget *) wave_editor->wave_toolbar,
		     FALSE, FALSE, 0);

  wave_editor->paned = (GtkHPaned *) gtk_hpaned_new();
  gtk_box_pack_start((GtkBox *) wave_editor,
		     (GtkWidget *) wave_editor->paned,
		     TRUE, TRUE, 0);

  /* machine selector */
  viewport = gtk_viewport_new(NULL,
			      NULL);
  g_object_set(viewport,
	       "shadow-type", GTK_SHADOW_NONE,
	       NULL);
  gtk_paned_pack1((GtkPaned *) wave_editor->paned,
		  (GtkWidget *) viewport,
		  FALSE, TRUE);

  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(viewport,
		    scrolled_window);

  wave_editor->machine_selector = g_object_new(AGS_TYPE_MACHINE_SELECTOR,
					       "homogeneous", FALSE,
					       "spacing", 0,
					       NULL);
  wave_editor->machine_selector->flags |= (AGS_MACHINE_SELECTOR_WAVE);
  gtk_label_set_label(wave_editor->machine_selector->label,
		      i18n("wave"));
  
  wave_editor->machine_selector->popup = ags_machine_selector_popup_new(wave_editor->machine_selector);
  g_object_set(wave_editor->machine_selector->menu_button,
	       "menu", wave_editor->machine_selector->popup,
	       NULL);
  
  gtk_scrolled_window_add_with_viewport(scrolled_window, (GtkWidget *) wave_editor->machine_selector);

  /* selected machine */
  wave_editor->selected_machine = NULL;

  /* table */
  viewport = gtk_viewport_new(NULL,
			      NULL);
  g_object_set(viewport,
	       "shadow-type", GTK_SHADOW_NONE,
	       NULL);
  gtk_paned_pack2((GtkPaned *) wave_editor->paned,
		  (GtkWidget *) viewport,
		  TRUE, TRUE);

  table = (GtkTable *) gtk_table_new(4, 3, FALSE);
  gtk_container_add(viewport,
		    table);

  /* scrollbars */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, AGS_WAVE_EDIT_DEFAULT_CONTROL_HEIGHT, 1.0);
  wave_editor->vscrollbar = gtk_vscrollbar_new(adjustment);
  gtk_table_attach(table,
		   (GtkWidget *) wave_editor->vscrollbar,
		   2, 3,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, AGS_WAVE_EDIT_DEFAULT_CONTROL_WIDTH, 1.0);
  wave_editor->hscrollbar = gtk_hscrollbar_new(adjustment);
  gtk_table_attach(table,
		   (GtkWidget *) wave_editor->hscrollbar,
		   1, 2,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  /* notebook */
  wave_editor->notebook = g_object_new(AGS_TYPE_NOTEBOOK,
				       "homogeneous", FALSE,
				       "spacing", 0,
				       "prefix", i18n("line"),
				       NULL);
  gtk_table_attach(table,
		   (GtkWidget *) wave_editor->notebook,
		   0, 3,
		   0, 1,
		   GTK_FILL | GTK_EXPAND, GTK_FILL,
		   0, 0);

  /* ruler */
  wave_editor->ruler = ags_ruler_new();
  gtk_table_attach(table,
		   (GtkWidget *) wave_editor->ruler,
		   1, 2,
		   1, 2,
		   GTK_FILL | GTK_EXPAND, GTK_FILL,
		   0, 0);


  /* level */
  wave_editor->scrolled_level_box = ags_scrolled_level_box_new();

  wave_editor->scrolled_level_box->level_box = ags_vlevel_box_new();
  gtk_container_add(wave_editor->scrolled_level_box->viewport,
		    wave_editor->scrolled_level_box->level_box);

  gtk_table_attach(table,
		   (GtkWidget *) wave_editor->scrolled_level_box,
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* wave edit */
  wave_editor->scrolled_wave_edit_box = ags_scrolled_wave_edit_box_new();

  wave_editor->scrolled_wave_edit_box->wave_edit_box = ags_vwave_edit_box_new();
  gtk_container_add(wave_editor->scrolled_wave_edit_box->viewport,
		    wave_editor->scrolled_wave_edit_box->wave_edit_box);

  gtk_table_attach(table,
		   (GtkWidget *) wave_editor->scrolled_wave_edit_box,
		   1, 2,
		   2, 3,
		   GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND,
		   0, 0);

  gtk_widget_set_events(GTK_WIDGET(wave_editor->scrolled_wave_edit_box->viewport), GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			| GDK_CONTROL_MASK);
}

void
ags_wave_editor_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsWaveEditor *wave_editor;

  wave_editor = AGS_WAVE_EDITOR(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = g_value_get_object(value);

      if(wave_editor->soundcard == soundcard){
	return;
      }

      if(wave_editor->soundcard != NULL){
	g_object_unref(wave_editor->soundcard);
      }
      
      if(soundcard != NULL){
	g_object_ref(soundcard);
      }
      
      wave_editor->soundcard = soundcard;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wave_editor_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsWaveEditor *wave_editor;

  wave_editor = AGS_WAVE_EDITOR(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, wave_editor->soundcard);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wave_editor_connect(AgsConnectable *connectable)
{
  AgsWaveEditor *wave_editor;

  wave_editor = AGS_WAVE_EDITOR(connectable);

  if((AGS_WAVE_EDITOR_CONNECTED & (wave_editor->flags)) != 0){
    return;
  }

  wave_editor->flags |= AGS_WAVE_EDITOR_CONNECTED;
  
  /*  */
  g_signal_connect((GObject *) wave_editor->machine_selector, "changed",
		   G_CALLBACK(ags_wave_editor_machine_changed_callback), (gpointer) wave_editor);

  /*  */
  ags_connectable_connect(AGS_CONNECTABLE(wave_editor->wave_toolbar));
  ags_connectable_connect(AGS_CONNECTABLE(wave_editor->machine_selector));
}

void
ags_wave_editor_disconnect(AgsConnectable *connectable)
{
  AgsWaveEditor *wave_editor;

  wave_editor = AGS_WAVE_EDITOR(connectable);

  ags_connectable_disconnect(AGS_CONNECTABLE(wave_editor->wave_toolbar)); 
  ags_connectable_disconnect(AGS_CONNECTABLE(wave_editor->machine_selector));
}

void
ags_wave_editor_finalize(GObject *gobject)
{
  AgsWaveEditor *wave_editor;

  wave_editor = AGS_WAVE_EDITOR(gobject);
  
  G_OBJECT_CLASS(ags_wave_editor_parent_class)->finalize(gobject);
}

void
ags_wave_editor_real_machine_changed(AgsWaveEditor *wave_editor, AgsMachine *machine)
{
  AgsMachine *machine_old;

  AgsMutexManager *mutex_manager;

  GList *list, *list_start;
  GList *child;

  guint output_lines, input_lines;
  guint pads;
  guint i;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  
  if(wave_editor->selected_machine == machine){
    return;
  }

  machine_old = wave_editor->selected_machine;
  wave_editor->selected_machine = machine;
  
  if(machine == NULL){
    return;
  }

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) machine->audio);
  
  pthread_mutex_unlock(application_mutex);

  /* get audio properties */
  pthread_mutex_lock(audio_mutex);

  output_lines = machine->audio->output_lines;
  input_lines = machine->audio->input_lines;
  
  pthread_mutex_unlock(audio_mutex);

  //TODO:JK: implement me
}
 
/**
 * ags_wave_editor_machine_changed:
 * @wave_editor: an #AgsWaveEditor
 * @machine: the new #AgsMachine
 *
 * Is emitted as machine changed of wave editor.
 *
 * Since: 2.0.0
 */
void
ags_wave_editor_machine_changed(AgsWaveEditor *wave_editor, AgsMachine *machine)
{
  g_return_if_fail(AGS_IS_WAVE_EDITOR(wave_editor));

  g_object_ref((GObject *) wave_editor);
  g_signal_emit((GObject *) wave_editor,
		wave_editor_signals[MACHINE_CHANGED], 0,
		machine);
  g_object_unref((GObject *) wave_editor);
}

void
ags_wave_editor_select_all(AgsWaveEditor *wave_editor)
{
  //TODO:JK: implement me
}

void
ags_wave_editor_paste(AgsWaveEditor *wave_editor)
{
  //TODO:JK: implement me
}

void
ags_wave_editor_copy(AgsWaveEditor *wave_editor)
{
  //TODO:JK: implement me
}

void
ags_wave_editor_cut(AgsWaveEditor *wave_editor)
{
  //TODO:JK: implement me
}

void
ags_wave_editor_invert(AgsWaveEditor *wave_editor)
{
  g_message("ags_wave_editor_invert() - not implemented");
}

/**
 * ags_wave_editor_new:
 *
 * Create a new #AgsWaveEditor.
 *
 * Returns: a new #AgsWaveEditor
 *
 * Since: 2.0.0
 */
AgsWaveEditor*
ags_wave_editor_new()
{
  AgsWaveEditor *wave_editor;

  wave_editor = (AgsWaveEditor *) g_object_new(AGS_TYPE_WAVE_EDITOR,
					       NULL);

  return(wave_editor);
}
