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

#include <ags/app/ags_navigation.h>
#include <ags/app/ags_navigation_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <ags/i18n.h>

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

void ags_navigation_real_change_position(AgsNavigation *navigation,
					 gdouble tact);

/**
 * SECTION:ags_navigation
 * @short_description: control audio object's playback.
 * @title: AgsNavigation
 * @section_id:
 * @include: ags/app/ags_navigation.h
 *
 * #AgsNavigation is a composite widget to control playback of #AgsAudio objects.
 * It can start #AgsMachine in bulk mode or position the stream.
 */

enum{
  PROP_0,
  PROP_SOUNDCARD,
};

enum{
  CHANGE_POSITION,
  LAST_SIGNAL,
};

static gpointer ags_navigation_parent_class = NULL;
static guint navigation_signals[LAST_SIGNAL];

GHashTable *ags_navigation_duration_queue_draw = NULL;

GType
ags_navigation_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_navigation = 0;

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

    ags_type_navigation = g_type_register_static(GTK_TYPE_BOX,
						 "AgsNavigation", &ags_navigation_info,
						 0);

    g_type_add_interface_static(ags_type_navigation,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_navigation);
  }

  return g_define_type_id__volatile;
}

void
ags_navigation_class_init(AgsNavigationClass *navigation)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_navigation_parent_class = g_type_class_peek_parent(navigation);

  /* GObjectClass */
  gobject = (GObjectClass *) navigation;

  gobject->set_property = ags_navigation_set_property;
  gobject->get_property = ags_navigation_get_property;

  gobject->finalize = ags_navigation_finalize;

  /* properties */
  /**
   * AgsNavigation:soundcard:
   *
   * The assigned #AgsSoundcard to use as default sink.
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

  /* AgsNavigationClass */
  navigation->change_position = ags_navigation_real_change_position;

  /* signals */
  /**
   * AgsNavigation::change-position:
   * @navigation: the #AgsNavigation
   * @tact: the new position
   *
   * The ::change-position seeks the stream.
   * 
   * Since: 3.0.0
   */
  navigation_signals[CHANGE_POSITION] =
    g_signal_new("change-position",
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
  GtkBox *hbox;
  GtkLabel *label;
  
  gtk_orientable_set_orientation(GTK_ORIENTABLE(navigation),
				 GTK_ORIENTATION_VERTICAL);

  navigation->flags = AGS_NAVIGATION_BLOCK_TIC;

  navigation->soundcard = NULL;

  navigation->start_tact = 0.0;
  navigation->note_offset = 0.0;
  
  /* GtkWidget */  
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_PADDING);
  gtk_box_append((GtkBox *) navigation,
		 (GtkWidget *) hbox);

  navigation->expander = (GtkToggleButton *) gtk_toggle_button_new();

  gtk_button_set_icon_name((GtkButton *) navigation->expander,
			   "pan-down");  
  gtk_widget_set_name((GtkWidget *) navigation->expander,
		      "ags-navigation-expander");
  
  gtk_box_append(hbox,
		 (GtkWidget *) navigation->expander);
  
  label = (GtkLabel *) gtk_label_new(i18n("bpm"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  navigation->bpm = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0,
								     1000.0,
								     1.0);

  gtk_spin_button_set_value(navigation->bpm,
			    AGS_NAVIGATION_DEFAULT_BPM);

  gtk_box_append(hbox,
		 (GtkWidget *) navigation->bpm);

  navigation->current_bpm = 120.0;

  navigation->rewind = (GtkToggleButton *) g_object_new(GTK_TYPE_BUTTON,
							"icon-name", "media-skip-backward",
							"has-tooltip", TRUE,
							"tooltip-text", i18n("skip backward"),
							NULL);
  gtk_box_append(hbox,
		 (GtkWidget *) navigation->rewind);

  navigation->previous = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
						    "icon-name", "media-seek-backward",
						    "has-tooltip", TRUE,
						    "tooltip-text", i18n("seek backward"),
						    NULL);
  gtk_box_append(hbox,
		 (GtkWidget *) navigation->previous);

  navigation->play = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
						      "icon-name", "media-playback-start",
						      "has-tooltip", TRUE,
						      "tooltip-text", i18n("playback"),
						      NULL);
  gtk_box_append(hbox,
		 (GtkWidget *) navigation->play);

  navigation->stop = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
						"icon-name", "media-playback-stop",
						"has-tooltip", TRUE,
						"tooltip-text", i18n("stop"),
						NULL);
  gtk_box_append(hbox,
		 (GtkWidget *) navigation->stop);

  navigation->next = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
						"icon-name", "media-seek-forward",
						"has-tooltip", TRUE,
						"tooltip-text", i18n("seek forward"),
						NULL);
  gtk_box_append(hbox,
		 (GtkWidget *) navigation->next);

  navigation->forward = (GtkToggleButton *) g_object_new(GTK_TYPE_BUTTON,
							 "icon-name", "media-skip-forward",
							 "has-tooltip", TRUE,
							 "tooltip-text", i18n("skip forward"),
							 NULL);
  gtk_box_append(hbox,
		 (GtkWidget *) navigation->forward);

  navigation->loop = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("loop"));
  gtk_box_append(hbox,
		 (GtkWidget *) navigation->loop);

  label = (GtkLabel *) gtk_label_new("position");
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  navigation->position_time = (GtkLabel *) gtk_label_new(AGS_NAVIGATION_DURATION_ZERO);
  gtk_box_append(hbox,
		 (GtkWidget *) navigation->position_time);

  navigation->position_tact = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									       (gdouble) AGS_NAVIGATION_MAX_POSITION_TACT,
									       1.0);
  gtk_box_append(hbox,
		 (GtkWidget *) navigation->position_tact);


  label = (GtkLabel *) gtk_label_new("duration");
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  navigation->duration_time = (GtkLabel *) gtk_label_new(NULL);
  g_object_set(navigation->duration_time,
	       "label", AGS_NAVIGATION_DURATION_ZERO,
	       NULL);
  gtk_box_append(hbox,
		 (GtkWidget *) navigation->duration_time);

  if(ags_navigation_duration_queue_draw == NULL){
    ags_navigation_duration_queue_draw = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							       NULL,
							       NULL);
  }

  g_hash_table_insert(ags_navigation_duration_queue_draw,
		      navigation, ags_navigation_duration_time_queue_draw_timeout);
  
  g_timeout_add((guint) floor(AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0),
		(GSourceFunc) ags_navigation_duration_time_queue_draw_timeout,
		(gpointer) navigation);

  navigation->duration_tact = NULL;
  //  navigation->duration_tact = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, AGS_NOTATION_EDITOR_MAX_CONTROLS, 1.0);
  //  gtk_box_append(hbox, (GtkWidget *) navigation->duration_tact, FALSE, FALSE, 2);


  /* expansion */
  navigation->expansion_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					  AGS_UI_PROVIDER_DEFAULT_PADDING);
  gtk_box_append(navigation,
		 (GtkWidget *) navigation->expansion_box);

  label = (GtkLabel *) gtk_label_new(i18n("loop L"));
  gtk_box_append(navigation->expansion_box,
		 (GtkWidget *) label);

  navigation->loop_left_tact = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										65000.0,
										1.0);
  gtk_box_append(navigation->expansion_box,
		 (GtkWidget *) navigation->loop_left_tact);

  label = (GtkLabel *) gtk_label_new(i18n("loop R"));
  gtk_box_append(navigation->expansion_box,
		 (GtkWidget *) label);

  navigation->loop_right_tact = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										 65000.0,
										 1.0);
  gtk_spin_button_set_value(navigation->loop_right_tact,
			    4.0);
  gtk_box_append(navigation->expansion_box,
		 (GtkWidget *) navigation->loop_right_tact);

  navigation->scroll = NULL;
  /*
    navigation->scroll = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("auto-scroll"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(navigation->scroll),
    FALSE);
    gtk_box_append(navigation->expansion_box, (GtkWidget *) navigation->scroll, FALSE, FALSE, 2);
  */
  
  navigation->exclude_sequencer = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("exclude sequencers"));
  gtk_check_button_set_active(navigation->exclude_sequencer,
			      TRUE);
  gtk_box_append(navigation->expansion_box,
		 (GtkWidget *) navigation->exclude_sequencer);
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
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = g_value_get_object(value);

      if(navigation->soundcard == soundcard){
	return;
      }

      if(navigation->soundcard != NULL){
	g_object_unref(navigation->soundcard);
      }
      
      if(soundcard != NULL){
	//FIXME:JK: no direct callback
	g_signal_connect_after(soundcard, "stop",
			       G_CALLBACK(ags_navigation_soundcard_stop_callback), (gpointer) navigation);
	
	g_object_ref(soundcard);
      }
      
      navigation->soundcard = soundcard;
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
  case PROP_SOUNDCARD:
    g_value_set_object(value, navigation->soundcard);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_navigation_finalize(GObject *gobject)
{
  AgsNavigation *navigation;
  
  navigation = AGS_NAVIGATION(gobject);

  g_hash_table_remove(ags_navigation_duration_queue_draw,
		      navigation);
  
  /* call parent */
  G_OBJECT_CLASS(ags_navigation_parent_class)->finalize(gobject);
}

void
ags_navigation_connect(AgsConnectable *connectable)
{
  AgsNavigation *navigation;

  navigation = AGS_NAVIGATION(connectable);

  if((AGS_NAVIGATION_CONNECTED & (navigation->flags)) != 0){
    return;
  }

  navigation->flags |= AGS_NAVIGATION_CONNECTED;
  
  g_signal_connect((GObject *) navigation->expander, "toggled",
		   G_CALLBACK(ags_navigation_expander_callback), (gpointer) navigation);

  g_signal_connect_after((GObject *) navigation->bpm, "value-changed",
			 G_CALLBACK(ags_navigation_bpm_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->rewind, "clicked",
		   G_CALLBACK(ags_navigation_rewind_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->previous, "clicked",
		   G_CALLBACK(ags_navigation_prev_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->play, "toggled",
		   G_CALLBACK(ags_navigation_play_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->stop, "clicked",
		   G_CALLBACK(ags_navigation_stop_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->next, "clicked",
		   G_CALLBACK(ags_navigation_next_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->forward, "clicked",
		   G_CALLBACK(ags_navigation_forward_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->loop, "toggled",
		   G_CALLBACK(ags_navigation_loop_callback), (gpointer) navigation);

  g_signal_connect_after((GObject *) navigation->position_tact, "value-changed",
			 G_CALLBACK(ags_navigation_position_tact_callback), (gpointer) navigation);

  //  g_signal_connect((GObject *) navigation->duration_tact, "value-changed",
  //		   G_CALLBACK(ags_navigation_duration_tact_callback), (gpointer) navigation);

  /* soundcard */
  //  g_signal_connect_after((GObject *) navigation->soundcard, "tic",
  //			 G_CALLBACK(ags_navigation_tic_callback), (gpointer) navigation);

  if(navigation->soundcard != NULL){
    g_signal_connect_after(navigation->soundcard, "stop",
			   G_CALLBACK(ags_navigation_soundcard_stop_callback), (gpointer) navigation);
  }
  
  /* expansion */
  g_signal_connect((GObject *) navigation->loop_left_tact, "value-changed",
		   G_CALLBACK(ags_navigation_loop_left_tact_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->loop_right_tact, "value-changed",
		   G_CALLBACK(ags_navigation_loop_right_tact_callback), (gpointer) navigation);
}

void
ags_navigation_disconnect(AgsConnectable *connectable)
{
  AgsNavigation *navigation;

  navigation = AGS_NAVIGATION(connectable);

  if((AGS_NAVIGATION_CONNECTED & (navigation->flags)) == 0){
    return;
  }

  navigation->flags &= (~AGS_NAVIGATION_CONNECTED);
  
  g_object_disconnect((GObject *) navigation->expander,
		      "any_signal::toggled",
		      G_CALLBACK(ags_navigation_expander_callback),
		      (gpointer) navigation,
		      NULL);

  g_object_disconnect((GObject *) navigation->bpm,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_navigation_bpm_callback),
		      (gpointer) navigation,
		      NULL);

  g_object_disconnect((GObject *) navigation->rewind,
		      "any_signal::clicked",
		      G_CALLBACK(ags_navigation_rewind_callback),
		      (gpointer) navigation,
		      NULL);

  g_object_disconnect((GObject *) navigation->previous,
		      "any_signal::clicked",
		      G_CALLBACK(ags_navigation_prev_callback),
		      (gpointer) navigation,
		      NULL);

  g_object_disconnect((GObject *) navigation->play,
		      "any_signal::toggled",
		      G_CALLBACK(ags_navigation_play_callback),
		      (gpointer) navigation,
		      NULL);

  g_object_disconnect((GObject *) navigation->stop,
		      "any_signal::clicked",
		      G_CALLBACK(ags_navigation_stop_callback),
		      (gpointer) navigation,
		      NULL);

  g_object_disconnect((GObject *) navigation->next,
		      "any_signal::clicked",
		      G_CALLBACK(ags_navigation_next_callback),
		      (gpointer) navigation,
		      NULL);

  g_object_disconnect((GObject *) navigation->forward,
		      "any_signal::clicked",
		      G_CALLBACK(ags_navigation_forward_callback),
		      (gpointer) navigation,
		      NULL);

  g_object_disconnect((GObject *) navigation->loop,
		      "any_signal::toggled",
		      G_CALLBACK(ags_navigation_loop_callback),
		      (gpointer) navigation,
		      NULL);

  g_object_disconnect((GObject *) navigation->position_tact,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_navigation_position_tact_callback),
		      (gpointer) navigation,
		      NULL);

  if(navigation->soundcard != NULL){
    g_object_disconnect(navigation->soundcard,
			"any_signal::stop",
			G_CALLBACK(ags_navigation_soundcard_stop_callback),
			(gpointer) navigation,
			NULL);
  }
  
  /* expansion */
  g_object_disconnect((GObject *) navigation->loop_left_tact,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_navigation_loop_left_tact_callback),
		      (gpointer) navigation,
		      NULL);

  g_object_disconnect((GObject *) navigation->loop_right_tact,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_navigation_loop_right_tact_callback),
		      (gpointer) navigation,
		      NULL);
}

void
ags_navigation_real_change_position(AgsNavigation *navigation,
				    gdouble tact_counter)
{
  AgsSeekSoundcard *seek_soundcard;

  AgsApplicationContext *application_context;

  GObject *default_soundcard;
  
  GList *start_list, *list;
  
  gchar *timestr;
  gdouble delay;
  gdouble delay_factor;
  gint64 new_offset;

  application_context = ags_application_context_get_instance();

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  /* seek soundcard */
  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(default_soundcard));
  delay_factor = ags_soundcard_get_delay_factor(AGS_SOUNDCARD(default_soundcard));
  
  new_offset = (16 * tact_counter);
  
  seek_soundcard = ags_seek_soundcard_new(default_soundcard,
					  new_offset,
					  AGS_SEEK_SET);
  
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) seek_soundcard);

  /* soundcard - start offset */
  list = 
    start_list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

  while(list != NULL){
    ags_soundcard_set_start_note_offset(AGS_SOUNDCARD(list->data),
					new_offset);

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);

  /* sequencer - start offset */
  list = 
    start_list = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));

  while(list != NULL){
    ags_sequencer_set_start_note_offset(AGS_SEQUENCER(list->data),
					new_offset);

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);

  //TODO:JK: implement me
  
  /* update */
  timestr = ags_time_get_uptime_from_offset(16.0 * tact_counter,
					    gtk_spin_button_get_value(navigation->bpm),
					    delay,
					    delay_factor);
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
 * Since: 3.0.0
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
 * @bpm: the BPM
 * @delay_factor: the delay factor
 * 
 * Convert tact unit to time.
 *
 * Returns: tact as time string
 *
 * Since: 3.0.0 
 */
gchar*
ags_navigation_tact_to_time_string(gdouble tact,
				   gdouble bpm,
				   gdouble delay_factor)
{
  gdouble delay_min, delay_sec, delay_msec;
  gchar *timestr;
  gdouble tact_redux;
  guint min, sec, msec;

  delay_min = bpm / delay_factor;
  delay_sec = delay_min / 60.0;
  delay_msec = delay_sec / 1000.0;

  tact_redux = (tact + (tact / 16.0)) * 16.0;

  min = (guint) floor(tact_redux / delay_min);

  if(min > 0){
    tact_redux = tact_redux - (min * delay_min);
  }

  sec = (guint) floor(tact_redux / delay_sec);

  if(sec > 0){
    tact_redux = tact_redux - (sec * delay_sec);
  }

  msec = (guint) floor(tact_redux / delay_msec);

  timestr = g_strdup_printf("%.4d:%.2d.%.3d", min, sec, msec);

  return(timestr);
}

/**
 * ags_navigation_update_time_string:
 * @tact: the new position
 * @bpm: the BPM
 * @delay_factor: the delay factor
 * @time_string: the pointer location to set
 *
 * Updates time as string.
 *
 * Since: 3.0.0 
 */
void
ags_navigation_update_time_string(double tact,
				  gdouble bpm,
				  gdouble delay_factor,
				  gchar *time_string)
{
  gdouble delay_min, delay_sec, delay_msec;
  gdouble tact_redux;
  guint min, sec, msec;

  delay_min = bpm * (60.0 / bpm) * (60.0 / bpm) * delay_factor;
  delay_sec = delay_min / 60.0;
  delay_msec = delay_sec / 1000.0;

  tact_redux = 1.0 / 16.0;

  min = (guint) floor(tact_redux / delay_min);

  if(min > 0){
    tact_redux = tact_redux - (min * delay_min);
  }

  sec = (guint) floor(tact_redux / delay_sec);

  if(sec > 0){
    tact_redux = tact_redux - (sec * delay_sec);
  }

  msec = (guint) floor(tact_redux / delay_msec);

  sprintf(time_string, "%.4d:%.2d.%.3d", min, sec, msec);
}

gchar*
ags_navigation_relative_tact_to_time_string(gchar *timestr,
					    gdouble delay,
					    gdouble bpm,
					    gdouble delay_factor)
{
  guint min, sec, msec;
  guint prev_min, prev_sec, prev_msec;

  gdouble sec_value;

  sscanf(timestr, "%d:%d.%d", &prev_min, &prev_sec, &prev_msec);
  sec_value = prev_min * 60.0;
  sec_value += prev_sec;
  sec_value += (1.0 / (16.0 * delay_factor) * (60.0 / bpm) + (1.0 / delay)) / 2.0;
  
  if(prev_msec != 0){
    sec_value += (prev_msec / 1000.0);
  }
  
  //  sec_value += (1.0 / delay);

  min = (guint) floor(sec_value / 60.0);

  sec = sec_value - 60 * min;

  msec = (sec_value - sec - min * 60) * 1000;
  
  timestr = g_strdup_printf("%.4d:%.2d.%.3d", min, sec, msec);
  
  return(timestr);
}

gchar*
ags_navigation_absolute_tact_to_time_string(gdouble tact,
					    gdouble bpm,
					    gdouble delay_factor)
{
  gchar *timestr;

  gdouble delay_min, delay_sec, delay_msec;
  gdouble tact_redux;
  guint min, sec, msec;

  /* calculate delays */
  delay_sec = ((bpm / delay_factor) / 60.0);
  delay_min = delay_sec * 60.0;
  delay_msec = delay_sec / 1000.0;

  /* translate to time string */
  tact_redux = tact;

  min = (guint) floor(tact_redux / delay_min);

  if(min > 0){
    tact_redux = tact_redux - (min * delay_min);
  }

  sec = (guint) floor(tact_redux / delay_sec);

  if(sec > 0){
    tact_redux = tact_redux - (sec * delay_sec);
  }

  msec = (guint) floor(tact_redux / delay_msec);

  timestr = g_strdup_printf("%.4d:%.2d.%.3d", min, sec, msec);

  return(timestr);
}

/**
 * ags_navigation_set_seeking_sensitive_new:
 * @navigation: the #AgsNavigation
 * @enabled: if %TRUE then sensitive, otherwise insensitive.
 *
 * Enables/Disables the #AgsNavigation to control the tree.
 *
 * Since: 3.0.0
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

gboolean
ags_navigation_duration_time_queue_draw_timeout(GtkWidget *widget)
{
  if(g_hash_table_lookup(ags_navigation_duration_queue_draw,
			 widget) != NULL){      
    AgsNavigation *navigation;

    AgsApplicationContext *application_context;

    GObject *default_soundcard;

    gchar *str;

    navigation = AGS_NAVIGATION(widget);

    application_context = ags_application_context_get_instance();

    default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

    if(default_soundcard != NULL){
      str = ags_soundcard_get_uptime(AGS_SOUNDCARD(default_soundcard));
    
      g_object_set(navigation->duration_time,
		   "label", str,
		   NULL);
      g_free(str);
  
      gtk_widget_queue_draw((GtkWidget *) navigation->duration_time);
    }
  
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_navigation_new:
 *
 * Create a new instance of #AgsNavigation to control the tree.
 *
 * Returns: the new #AgsNavigation
 *
 * Since: 3.0.0
 */
AgsNavigation*
ags_navigation_new()
{
  AgsNavigation *navigation;

  navigation = (AgsNavigation *) g_object_new(AGS_TYPE_NAVIGATION, NULL);

  return(navigation);
}
