/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/ags_navigation.h>
#include <ags/X/ags_navigation_callbacks.h>

#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/task/ags_seek_devout.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_editor.h>

#include <ags/X/editor/ags_note_edit.h>
#include <ags/X/editor/ags_pattern_edit.h>

void ags_navigation_class_init(AgsNavigationClass *navigation);
void ags_navigation_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_navigation_init(AgsNavigation *navigation);
void ags_navigation_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_navigation_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_navigation_finalize(GObject *gobject);
void ags_navigation_connect(AgsConnectable *connectable);
void ags_navigation_disconnect(AgsConnectable *connectable);
void ags_navigation_destroy(GtkObject *object);
void ags_navigation_show(GtkWidget *widget);

void ags_navigation_real_change_position(AgsNavigation *navigation,
					 gdouble tact);

/**
 * SECTION:ags_navigation
 * @short_description: control audio object's playback.
 * @title: AgsNavigation
 * @section_id:
 * @include: ags/X/ags_navigation.h
 *
 * #AgsNavigation is a composite widget to control playback of #AgsAudio objects.
 * It can start #AgsMachine in bulk mode or position the stream.
 */

enum{
  PROP_0,
  PROP_DEVOUT,
};

enum{
  CHANGE_POSITION,
  LAST_SIGNAL,
};

static gpointer ags_navigation_parent_class = NULL;
static guint navigation_signals[LAST_SIGNAL];

GType
ags_navigation_get_type(void)
{
  static GType ags_type_navigation = 0;

  if(!ags_type_navigation){
    static const GTypeInfo ags_navigation_info = {
      sizeof (AgsNavigationClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_navigation_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsNavigation),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_navigation_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_navigation_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_navigation = g_type_register_static(GTK_TYPE_VBOX,
						 "AgsNavigation\0", &ags_navigation_info,
						 0);

    g_type_add_interface_static(ags_type_navigation,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_navigation);
}

void
ags_navigation_class_init(AgsNavigationClass *navigation)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_navigation_parent_class = g_type_class_peek_parent(navigation);

  /* GObjectClass */
  gobject = (GObjectClass *) navigation;

  gobject->set_property = ags_navigation_set_property;
  gobject->get_property = ags_navigation_get_property;

  gobject->finalize = ags_navigation_finalize;

  /* properties */
  /**
   * AgsNavigation:devout:
   *
   * The assigned #AgsDevout to use as default sink.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("devout\0",
				   "assigned devout\0",
				   "The devout it is assigned with\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVOUT,
				  param_spec);

  /* AgsNavigationClass */
  navigation->change_position = ags_navigation_real_change_position;

  /* signals */
  /**
   * AgsNavigation::change-position:
   * @navigation: the #AgsNavigation
   * @tact: the new position
   *
   * The ::change-position seeks the stream.
   */
  navigation_signals[CHANGE_POSITION] =
    g_signal_new("change-position\0",
		 G_TYPE_FROM_CLASS (navigation),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsNavigationClass, change_position),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__DOUBLE,
		 G_TYPE_NONE, 1,
		 G_TYPE_DOUBLE);
}

void
ags_navigation_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_navigation_connect;
  connectable->disconnect = ags_navigation_disconnect;
}

void
ags_navigation_init(AgsNavigation *navigation)
{
  GtkHBox *hbox;
  GtkLabel *label;

  navigation->flags = AGS_NAVIGATION_BLOCK_TIC;

  navigation->devout = NULL;

  g_signal_connect_after(G_OBJECT(navigation), "parent-set\0",
			 G_CALLBACK(ags_navigation_parent_set_callback), NULL);

  navigation->start_tact = 0.0;
  
  /* GtkWidget */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) navigation, (GtkWidget *) hbox, FALSE, FALSE, 2);

  navigation->expander = (GtkToggleButton *) gtk_toggle_button_new();
  gtk_box_pack_start((GtkBox*) hbox, (GtkWidget *) navigation->expander, FALSE, FALSE, 2);
  gtk_container_add((GtkContainer *) navigation->expander,
		    (GtkWidget *) gtk_arrow_new(GTK_ARROW_RIGHT, GTK_SHADOW_NONE));

  label = (GtkLabel *) gtk_label_new("bpm\0");
  gtk_box_pack_start((GtkBox*) hbox, (GtkWidget *) label, FALSE, FALSE, 2);

  navigation->bpm = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0, 1000.0, 1.0);
  navigation->bpm->adjustment->value = 120.0;
  gtk_box_pack_start((GtkBox*) hbox, (GtkWidget *) navigation->bpm, FALSE, FALSE, 2);

  navigation->current_bpm = 120.0;

  navigation->rewind = (GtkToggleButton *) g_object_new(GTK_TYPE_BUTTON,
							"image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_MEDIA_REWIND, GTK_ICON_SIZE_LARGE_TOOLBAR),
							NULL);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->rewind, FALSE, FALSE, 0);

  navigation->previous = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
						    "image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_MEDIA_PREVIOUS, GTK_ICON_SIZE_LARGE_TOOLBAR),
						    NULL);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->previous, FALSE, FALSE, 0);

  navigation->play = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
						      "image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_LARGE_TOOLBAR),
						      NULL);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->play, FALSE, FALSE, 0);

  navigation->stop = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
						"image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_MEDIA_STOP, GTK_ICON_SIZE_LARGE_TOOLBAR),
						NULL);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->stop, FALSE, FALSE, 0);

  navigation->next = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
						"image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_MEDIA_NEXT, GTK_ICON_SIZE_LARGE_TOOLBAR),
						NULL);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->next, FALSE, FALSE, 0);

  navigation->forward = (GtkToggleButton *) g_object_new(GTK_TYPE_BUTTON,
							 "image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_MEDIA_FORWARD, GTK_ICON_SIZE_LARGE_TOOLBAR),
							 NULL);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->forward, FALSE, FALSE, 0);


  navigation->loop = (GtkCheckButton *) gtk_check_button_new_with_label("loop\0");
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->loop, FALSE, FALSE, 2);

  label = (GtkLabel *) gtk_label_new("position\0");
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) label, FALSE, FALSE, 2);

  navigation->position_time = (GtkLabel *) gtk_label_new(g_strdup("00:00.00\0"));
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->position_time, FALSE, FALSE, 2);

  navigation->position_tact = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, AGS_NOTE_EDIT_MAX_CONTROLS * 64.0, 1.0);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->position_tact, FALSE, FALSE, 2);


  label = (GtkLabel *) gtk_label_new("duration\0");
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) label, FALSE, FALSE, 2);

  navigation->duration_time = (GtkLabel *) gtk_label_new(NULL);
  g_object_set(navigation->duration_time,
	       "label\0", g_strdup("0000:00.00\0"),
	       NULL);
  gtk_widget_queue_draw((GtkWidget *) navigation->duration_time);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->duration_time, FALSE, FALSE, 2);

  navigation->duration_tact = NULL;
  //  navigation->duration_tact = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, AGS_NOTE_EDIT_MAX_CONTROLS * 64.0, 1.0);
  //  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->duration_tact, FALSE, FALSE, 2);


  /* expansion */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  //  GTK_WIDGET_SET_FLAGS((GtkWidget *) hbox, GTK_NO_SHOW_ALL);
  gtk_box_pack_start((GtkBox *) navigation, (GtkWidget *) hbox, FALSE, FALSE, 2);

  label = (GtkLabel *) gtk_label_new("loop L\0");
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) label, FALSE, FALSE, 2);

  navigation->loop_left_tact = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 65000.0, 1.0);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->loop_left_tact, FALSE, FALSE, 2);

  label = (GtkLabel *) gtk_label_new("loop R\0");
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) label, FALSE, FALSE, 2);

  navigation->loop_right_tact = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 65000.0, 1.0);
  gtk_spin_button_set_value(navigation->loop_right_tact,
			    4.0);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->loop_right_tact, FALSE, FALSE, 2);

  navigation->scroll = NULL;
  navigation->scroll = (GtkCheckButton *) gtk_check_button_new_with_label("auto-scroll\0");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(navigation->scroll),
			       FALSE);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->scroll, FALSE, FALSE, 2);

  navigation->exclude_sequencer = (GtkCheckButton *) gtk_check_button_new_with_label("exclude sequencers\0");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(navigation->exclude_sequencer),
			       TRUE);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->exclude_sequencer, FALSE, FALSE, 2);

}

void
ags_navigation_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsNavigation *navigation;

  navigation = AGS_NAVIGATION(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    {
      AgsDevout *devout;

      devout = (AgsDevout *) g_value_get_object(value);

      if(navigation->devout == devout)
	return;

      if(devout != NULL)
	g_object_ref(devout);

      navigation->devout = devout;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_navigation_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsNavigation *navigation;

  navigation = AGS_NAVIGATION(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    g_value_set_object(value, navigation->devout);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_navigation_connect(AgsConnectable *connectable)
{
  AgsNavigation *navigation;

  navigation = AGS_NAVIGATION(connectable);

  g_signal_connect((GObject *) navigation, "destroy\0",
		   G_CALLBACK(ags_navigation_destroy_callback), (gpointer) navigation);

  g_signal_connect_after((GObject *) navigation, "show\0",
			 G_CALLBACK(ags_navigation_show_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->expander, "clicked\0",
		   G_CALLBACK(ags_navigation_expander_callback), (gpointer) navigation);

  g_signal_connect_after((GObject *) navigation->bpm, "value-changed\0",
			 G_CALLBACK(ags_navigation_bpm_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->rewind, "clicked\0",
		   G_CALLBACK(ags_navigation_rewind_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->previous, "clicked\0",
		   G_CALLBACK(ags_navigation_prev_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->play, "clicked\0",
		   G_CALLBACK(ags_navigation_play_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->stop, "clicked\0",
		   G_CALLBACK(ags_navigation_stop_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->next, "clicked\0",
		   G_CALLBACK(ags_navigation_next_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->forward, "clicked\0",
		   G_CALLBACK(ags_navigation_forward_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->loop, "clicked\0",
		   G_CALLBACK(ags_navigation_loop_callback), (gpointer) navigation);

  g_signal_connect_after((GObject *) navigation->position_tact, "value-changed\0",
			 G_CALLBACK(ags_navigation_position_tact_callback), (gpointer) navigation);

  //  g_signal_connect((GObject *) navigation->duration_tact, "value-changed\0",
  //		   G_CALLBACK(ags_navigation_duration_tact_callback), (gpointer) navigation);

  /* devout */
  g_signal_connect_after((GObject *) navigation->devout, "tic\0",
  			 G_CALLBACK(ags_navigation_tic_callback), (gpointer) navigation);

  g_signal_connect_after((GObject *) navigation->devout, "stop\0",
  			 G_CALLBACK(ags_navigation_devout_stop_callback), (gpointer) navigation);

  /* expansion */
  g_signal_connect((GObject *) navigation->loop_left_tact, "value-changed\0",
		   G_CALLBACK(ags_navigation_loop_left_tact_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->loop_right_tact, "value-changed\0",
		   G_CALLBACK(ags_navigation_loop_right_tact_callback), (gpointer) navigation);
}

void
ags_navigation_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_navigation_finalize(GObject *gobject)
{
  /* empty */
}

void
ags_navigation_destroy(GtkObject *object)
{
  /* empty */
}

void
ags_navigation_show(GtkWidget *widget)
{
  GList *list;

  /* empty */
  //  list = gtk_container_get_children((GtkContainer *) widget);
  //  GTK_WIDGET_UNSET_FLAGS((GtkWidget *) list->next->data, GTK_NO_SHOW_ALL);
}

void
ags_navigation_real_change_position(AgsNavigation *navigation,
				    gdouble tact_counter)
{
  AgsWindow *window;
  AgsEditor *editor;
  AgsSeekDevout *seek_devout;
  gchar *timestr;
  double tact_factor, zoom_factor;
  double tact;
  gdouble delay;
  guint steps;
  gboolean move_forward;
  
  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));
  editor = window->editor;

  /* seek devout */
  delay = window->devout->delay[window->devout->tic_counter];

  tact = window->devout->tact_counter - navigation->start_tact;
  
  if(tact < tact_counter){
    steps = (guint) ((tact_counter - tact) * delay);
    move_forward = TRUE;
  }else{
    steps = (guint) ((navigation->start_tact - tact) * delay);
    move_forward = FALSE;
  }
  
  seek_devout = ags_seek_devout_new(window->devout,
				    steps,
				    move_forward);

  ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(window->ags_main)->main_loop)->task_thread),
			      AGS_TASK(seek_devout));

  /* update GUI */
  zoom_factor = 0.25;

  tact_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) editor->toolbar->zoom));
  tact = exp2((double) gtk_combo_box_get_active((GtkComboBox *) editor->toolbar->zoom) - 2.0);

  if(AGS_IS_NOTE_EDIT(editor->current_edit_widget)){
    gtk_adjustment_set_value(GTK_RANGE(AGS_NOTE_EDIT(editor->current_edit_widget)->hscrollbar)->adjustment,
			     tact_counter * AGS_NOTE_EDIT(editor->current_edit_widget)->control_current.control_width * (16.0 / tact_factor));
  }else if(AGS_IS_PATTERN_EDIT(editor->current_edit_widget)){
    gtk_adjustment_set_value(GTK_RANGE(AGS_PATTERN_EDIT(editor->current_edit_widget)->hscrollbar)->adjustment,
			     tact_counter * AGS_PATTERN_EDIT(editor->current_edit_widget)->control_current.control_width * (16.0 / tact_factor));
  }
  
  timestr = ags_navigation_tact_to_time_string(tact_counter,
					       navigation->bpm->adjustment->value);
  gtk_label_set_text(navigation->position_time, timestr);
  
  g_free(timestr);
}

/**
 * ags_navigation_change_position:
 * @navigation: the #AgsNavigation
 * @tact: the new position
 * 
 * Change tact position of editor. The scrollbar is adjustet
 * and its playback position seeked.
 *
 * Since: 0.4
 */
void
ags_navigation_change_position(AgsNavigation *navigation,
			       gdouble tact)
{
  g_return_if_fail(AGS_IS_NAVIGATION(navigation));

  g_object_ref(G_OBJECT(navigation));
  g_signal_emit(G_OBJECT(navigation),
		navigation_signals[CHANGE_POSITION], 0,
		tact);
  g_object_unref(G_OBJECT(navigation));
}

/**
 * ags_navigation_tact_to_time_string:
 * @tact: the new position
 * 
 * Convert tact unit to time.
 *
 * Returns: tact as time string
 *
 * Since: 0.4 
 */
gchar*
ags_navigation_tact_to_time_string(gdouble tact,
				   gdouble bpm)
{
  static gdouble delay_min, delay_sec, delay_hsec;
  static gboolean initialized = FALSE;
  gchar *timestr;
  gdouble tact_redux;
  guint min, sec, hsec;

  if(!initialized){
    delay_min = bpm;
    delay_sec = delay_min / 60.0;
    delay_hsec = delay_sec / 100.0;

    initialized = TRUE;
  }

  tact_redux = tact;

  min = (guint) floor(tact_redux / delay_min);

  if(min > 0){
    tact_redux = tact_redux - (min * delay_min);
  }

  sec = (guint) floor(tact_redux / delay_sec);

  if(sec > 0){
    tact_redux = tact_redux - (sec * delay_sec);
  }

  hsec = (guint) floor(tact_redux / delay_hsec);

  timestr = g_strdup_printf("%.4d:%.2d.%.2d\0", min, sec, hsec);

  return(timestr);
}

/**
 * ags_navigation_update_time_string:
 * @tact: the new position
 * @time_str: the pointer location to set
 *
 * Updates time as string.
 *
 * Since: 0.4 
 */
void
ags_navigation_update_time_string(double tact,
				  gchar *time_string)
{
  static gdouble delay_min, delay_sec, delay_hsec;
  static gboolean initialized = FALSE;
  gchar *timestr;
  gdouble tact_redux;
  guint min, sec, hsec;

  if(!initialized){
    delay_min = AGS_DEVOUT_DEFAULT_BPM;
    delay_sec = delay_min / 60.0;
    delay_hsec = delay_sec / 100.0;

    initialized = TRUE;
  }

  tact_redux = 1.0 / 16.0;

  min = (guint) floor(tact_redux / delay_min);

  if(min > 0){
    tact_redux = tact_redux - (min * delay_min);
  }

  sec = (guint) floor(tact_redux / delay_sec);

  if(sec > 0){
    tact_redux = tact_redux - (sec * delay_sec);
  }

  hsec = (guint) floor(tact_redux / delay_hsec);

  sprintf(time_string, "%.4d:%.2d.%.2d\0", min, sec, hsec);
}

/**
 * ags_navigation_set_seeking_sensitive_new:
 * @navigation: the #AgsNavigation
 * @enabled: if %TRUE then sensitive, otherwise insensitive.
 *
 * Enables/Disables the #AgsNavigation to control the tree.
 *
 * Since: 0.4
 */
void
ags_navigation_set_seeking_sensitive(AgsNavigation *navigation,
				     gboolean enabled)
{
  gtk_widget_set_sensitive((GtkWidget *) navigation->rewind,
			   enabled);
  gtk_widget_set_sensitive((GtkWidget *) navigation->previous,
			   enabled);
  gtk_widget_set_sensitive((GtkWidget *) navigation->play,
			   enabled);
  gtk_widget_set_sensitive((GtkWidget *) navigation->stop,
			   enabled);
  gtk_widget_set_sensitive((GtkWidget *) navigation->next,
			   enabled);
  gtk_widget_set_sensitive((GtkWidget *) navigation->forward,
			   enabled);
}

/**
 * ags_navigation_new:
 *
 * Creates an #AgsNavigation to control the tree.
 *
 * Returns: a new #AgsNavigation
 *
 * Since: 0.4
 */
AgsNavigation*
ags_navigation_new()
{
  AgsNavigation *navigation;

  navigation = (AgsNavigation *) g_object_new(AGS_TYPE_NAVIGATION, NULL);

  return(navigation);
}
