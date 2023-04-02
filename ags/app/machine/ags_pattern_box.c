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

#include <ags/app/machine/ags_pattern_box.h>
#include <ags/app/machine/ags_pattern_box_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_line.h>

#include <gdk/gdkkeysyms.h>

#include <ags/i18n.h>

void ags_pattern_box_class_init(AgsPatternBoxClass *pattern_box);
void ags_pattern_box_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pattern_box_init(AgsPatternBox *pattern_box);
void ags_pattern_box_finalize(GObject *gobject);

void ags_pattern_box_connect(AgsConnectable *connectable);
void ags_pattern_box_disconnect(AgsConnectable *connectable);

void ags_pattern_box_realize(GtkWidget *widget);
void ags_pattern_box_show(GtkWidget *widget);

/**
 * SECTION:ags_pattern_box
 * @short_description: pattern box sequencer
 * @title: AgsPatternBox
 * @section_id:
 * @include: ags/app/machine/ags_pattern_box.h
 *
 * The #AgsPatternBox is a composite widget to act as pattern box sequencer.
 */

static gpointer ags_pattern_box_parent_class = NULL;

GHashTable *ags_pattern_box_led_queue_draw = NULL;

GType
ags_pattern_box_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_pattern_box = 0;

    static const GTypeInfo ags_pattern_box_info = {
      sizeof(AgsPatternBoxClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pattern_box_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsPatternBox),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pattern_box_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pattern_box_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_pattern_box = g_type_register_static(GTK_TYPE_GRID,
						  "AgsPatternBox", &ags_pattern_box_info,
						  0);
    
    g_type_add_interface_static(ags_type_pattern_box,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_pattern_box);
  }

  return g_define_type_id__volatile;
}

void
ags_pattern_box_class_init(AgsPatternBoxClass *pattern_box)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_pattern_box_parent_class = g_type_class_peek_parent(pattern_box);

  /* GObjectClass */
  gobject = (GObjectClass *) pattern_box;

  gobject->finalize = ags_pattern_box_finalize;

  /* GtkWidget */
  widget = (GtkWidgetClass *) pattern_box;

  widget->show = ags_pattern_box_show;
}

void
ags_pattern_box_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_pattern_box_connect;
  connectable->disconnect = ags_pattern_box_disconnect;
}

void
ags_pattern_box_init(AgsPatternBox *pattern_box)
{
  GtkBox *offset;

  AgsApplicationContext *application_context;

  gdouble gui_scale_factor;
  guint i;

  application_context = ags_application_context_get_instance();
  
  g_object_set(pattern_box,
	       "can-focus", TRUE,
	       NULL);

  gtk_grid_set_column_spacing((GtkGrid *) pattern_box,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing((GtkGrid *) pattern_box,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);
			
  pattern_box->flags = 0;
  pattern_box->connectable_flags = 0;

  pattern_box->key_mask = 0;

  pattern_box->n_controls = AGS_PATTERN_BOX_N_CONTROLS;
  pattern_box->n_indices = AGS_PATTERN_BOX_N_INDICES;
  
  /* led */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
    
  pattern_box->active_led = 0;
  pattern_box->hled_array = ags_led_array_new(GTK_ORIENTATION_HORIZONTAL,
					      (guint) (gui_scale_factor * AGS_PATTERN_BOX_LED_DEFAULT_WIDTH),
					      (guint) (gui_scale_factor * AGS_PATTERN_BOX_LED_DEFAULT_HEIGHT),
					      pattern_box->n_controls);
  gtk_widget_set_size_request((GtkWidget *) pattern_box->hled_array,
			      (guint) (gui_scale_factor * pattern_box->n_controls * AGS_PATTERN_BOX_DEFAULT_PAD_WIDTH), (guint) (gui_scale_factor * AGS_PATTERN_BOX_LED_DEFAULT_HEIGHT));

  gtk_widget_set_valign((GtkWidget *) pattern_box->hled_array,
			GTK_ALIGN_CENTER);
  gtk_widget_set_vexpand((GtkWidget *) pattern_box->hled_array,
			 TRUE);
  
  gtk_grid_attach((GtkGrid *) pattern_box,
		  (GtkWidget *) pattern_box->hled_array,
		  0, 0,
		  1, 1);
  gtk_widget_show((GtkWidget *) pattern_box->hled_array);
  
  if(ags_pattern_box_led_queue_draw == NULL){
    ags_pattern_box_led_queue_draw = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							   NULL,
							   NULL);
  }

  g_hash_table_insert(ags_pattern_box_led_queue_draw,
		      pattern_box, ags_pattern_box_led_queue_draw_timeout);
  g_timeout_add((guint) floor(AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0), (GSourceFunc) ags_pattern_box_led_queue_draw_timeout, (gpointer) pattern_box);
  
  /* pattern */
  pattern_box->pad_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
						0);

  gtk_box_set_spacing(pattern_box->pad_box,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_valign((GtkWidget *) pattern_box->pad_box,
			GTK_ALIGN_CENTER);
  gtk_widget_set_vexpand((GtkWidget *) pattern_box->pad_box,
			 TRUE);

  gtk_grid_attach((GtkGrid *) pattern_box,
		   (GtkWidget *) pattern_box->pad_box,
		   0, 1,
		   1, 1);

  for(i = 0; i < pattern_box->n_controls; i++){
    GtkToggleButton *toggle_button;
    
    toggle_button = (GtkToggleButton *) gtk_toggle_button_new();

    gtk_widget_set_size_request((GtkWidget *) toggle_button,
				gui_scale_factor * AGS_PATTERN_BOX_DEFAULT_PAD_WIDTH, gui_scale_factor * AGS_PATTERN_BOX_DEFAULT_PAD_HEIGHT);

    ags_pattern_box_add_pad(pattern_box,
			    toggle_button);
  }

  /* page / offset */
  offset = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				  0);

  gtk_box_set_spacing(offset,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_grid_attach((GtkGrid *) pattern_box,
		  (GtkWidget *) offset,
		  1, 0,
		  1, 2);

  pattern_box->page_0_15 = (GtkCheckButton *) gtk_check_button_new_with_label("0 - 15");
  gtk_check_button_set_active(pattern_box->page_0_15,
			      TRUE);

  gtk_widget_set_halign((GtkWidget *) pattern_box->page_0_15,
			GTK_ALIGN_START);
  
  gtk_box_append(offset,
		 (GtkWidget *) pattern_box->page_0_15);

  pattern_box->page_16_31 = (GtkCheckButton *) gtk_check_button_new_with_label("16 - 31");
  gtk_check_button_set_group(pattern_box->page_16_31,
			     pattern_box->page_0_15);

  gtk_widget_set_halign((GtkWidget *) pattern_box->page_16_31,
			GTK_ALIGN_START);

  gtk_box_append(offset,
		 (GtkWidget *) pattern_box->page_16_31);

  pattern_box->page_32_47 = (GtkCheckButton *) gtk_check_button_new_with_label("32 - 47");
  gtk_check_button_set_group(pattern_box->page_32_47,
			     pattern_box->page_0_15);

  gtk_widget_set_halign((GtkWidget *) pattern_box->page_32_47,
			GTK_ALIGN_START);

  gtk_box_append(offset,
		 (GtkWidget *) pattern_box->page_32_47);

  pattern_box->page_48_63 = (GtkCheckButton *) gtk_check_button_new_with_label("48 - 63");
  gtk_check_button_set_group(pattern_box->page_48_63,
			     pattern_box->page_0_15);

  gtk_widget_set_halign((GtkWidget *) pattern_box->page_48_63,
			GTK_ALIGN_START);

  gtk_box_append(offset,
		 (GtkWidget *) pattern_box->page_48_63);
}

void
ags_pattern_box_finalize(GObject *gobject)
{
  g_hash_table_remove(ags_pattern_box_led_queue_draw,
		      gobject);

  G_OBJECT_CLASS(ags_pattern_box_parent_class)->finalize(gobject);
}

void
ags_pattern_box_connect(AgsConnectable *connectable)
{
  AgsPatternBox *pattern_box;

  GList *start_list, *list;

  if((AGS_CONNECTABLE_CONNECTED & (AGS_PATTERN_BOX(connectable)->connectable_flags)) != 0){
    return;
  }

  /* AgsPatternBox */
  pattern_box = AGS_PATTERN_BOX(connectable);

  pattern_box->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  /* connect pattern */
  list =
    start_list = ags_pattern_box_get_pad(pattern_box);

  while(list != NULL){
    g_signal_connect(G_OBJECT(list->data), "toggled",
		     G_CALLBACK(ags_pattern_box_pad_callback), (gpointer) pattern_box);

    list = list->next;
  }

  g_list_free(start_list);

  /* connect pattern offset range */
  g_signal_connect_after(G_OBJECT(pattern_box->page_0_15), "toggled",
			 G_CALLBACK(ags_pattern_box_offset_callback), (gpointer) pattern_box);

  g_signal_connect_after(G_OBJECT(pattern_box->page_16_31), "toggled",
			 G_CALLBACK(ags_pattern_box_offset_callback), (gpointer) pattern_box);

  g_signal_connect_after(G_OBJECT(pattern_box->page_32_47), "toggled",
			 G_CALLBACK(ags_pattern_box_offset_callback), (gpointer) pattern_box);

  g_signal_connect_after(G_OBJECT(pattern_box->page_48_63), "toggled",
			 G_CALLBACK(ags_pattern_box_offset_callback), (gpointer) pattern_box);
}

void
ags_pattern_box_disconnect(AgsConnectable *connectable)
{
  AgsPatternBox *pattern_box;

  GList *start_list, *list;

  if((AGS_CONNECTABLE_CONNECTED & (AGS_PATTERN_BOX(connectable)->connectable_flags)) == 0){
    return;
  }

  /* AgsPatternBox */
  pattern_box = AGS_PATTERN_BOX(connectable);

  pattern_box->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  /* connect pattern */
  list =
    start_list = ags_pattern_box_get_pad(pattern_box);

  while(list != NULL){
    g_object_disconnect(G_OBJECT(list->data),
			"any_signal::toggled",
			G_CALLBACK(ags_pattern_box_pad_callback),
			(gpointer) pattern_box,
			NULL);

    list = list->next;
  }

  g_list_free(start_list);

  /* disconnect pattern offset range */
  g_object_disconnect(G_OBJECT(pattern_box->page_0_15),
		      "any_signal::toggled",
		      G_CALLBACK(ags_pattern_box_offset_callback),
		      (gpointer) pattern_box,
		      NULL);

  g_object_disconnect(G_OBJECT(pattern_box->page_16_31),
		      "any_signal::toggled",
		      G_CALLBACK(ags_pattern_box_offset_callback),
		      (gpointer) pattern_box,
		      NULL);

  g_object_disconnect(G_OBJECT(pattern_box->page_32_47),
		      "any_signal::toggled",
		      G_CALLBACK(ags_pattern_box_offset_callback),
		      (gpointer) pattern_box,
		      NULL);
  
  g_object_disconnect(G_OBJECT(pattern_box->page_48_63),
		      "any_signal::toggled",
		      G_CALLBACK(ags_pattern_box_offset_callback),
		      (gpointer) pattern_box,
		      NULL);
}

void
ags_pattern_box_realize(GtkWidget *widget)
{  
  /* call parent */
  GTK_WIDGET_CLASS(ags_pattern_box_parent_class)->realize(widget);
}

void
ags_pattern_box_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_pattern_box_parent_class)->show(widget);

  //  ags_pattern_box_draw_matrix(AGS_PATTERN_BOX(widget));
}

/**
 * ags_pattern_box_get_pad:
 * @pattern_box: the #AgsPatternBox
 * 
 * Get pad of @pattern_box.
 * 
 * Returns: the #GList-struct containing #GtkToggleButton
 *
 * Since: 4.0.0
 */
GList*
ags_pattern_box_get_pad(AgsPatternBox *pattern_box)
{
  g_return_val_if_fail(AGS_IS_PATTERN_BOX(pattern_box), NULL);

  return(g_list_reverse(g_list_copy(pattern_box->pad)));
}

/**
 * ags_pattern_box_add_pad:
 * @pattern_box: the #AgsPattern_Box
 * @pad: the #GtkToggleButton
 * 
 * Add @pad to @pattern_box.
 * 
 * Since: 4.0.0
 */
void
ags_pattern_box_add_pad(AgsPatternBox *pattern_box,
			GtkToggleButton *pad)
{
  g_return_if_fail(AGS_IS_PATTERN_BOX(pattern_box));
  g_return_if_fail(GTK_IS_TOGGLE_BUTTON(pad));

  if(g_list_find(pattern_box->pad, pad) == NULL){
    pattern_box->pad = g_list_prepend(pattern_box->pad,
				      pad);
    
    gtk_box_append(pattern_box->pad_box,
		   (GtkWidget *) pad);
  }
}

/**
 * ags_pattern_box_remove_pad:
 * @pattern_box: the #AgsPattern_Box
 * @pad: the #AgsPad
 * 
 * Remove @pad from @pattern_box.
 * 
 * Since: 4.0.0
 */
void
ags_pattern_box_remove_pad(AgsPatternBox *pattern_box,
			   GtkToggleButton *pad)
{
  g_return_if_fail(AGS_IS_PATTERN_BOX(pattern_box));
  g_return_if_fail(GTK_IS_TOGGLE_BUTTON(pad));

  if(g_list_find(pattern_box->pad, pad) != NULL){
    pattern_box->pad = g_list_remove(pattern_box->pad,
				     pad);
    
    gtk_box_remove(pattern_box->pad_box,
		   (GtkWidget *) pad);
  }
}

/**
 * ags_pattern_box_set_pattern:
 * @pattern_box: the #AgsPatternBox
 *
 * Resets the pattern on @pattern_box.
 *
 * since: 2.0.0
 */
void
ags_pattern_box_set_pattern(AgsPatternBox *pattern_box)
{
  AgsMachine *machine;
  AgsLine *selected_line;

  GList *start_list, *list;
  GList *start_line, *line;

  guint index0, index1, offset;
  gboolean is_active;
  gboolean set_active;
  guint i;

  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) pattern_box,
						   AGS_TYPE_MACHINE);

  if(machine->selected_input_pad == NULL){
    return;
  }
  
  index0 = machine->bank_0;
  index1 = machine->bank_1;

  /* calculate offset / page */
  i = 0;
  
  if(gtk_check_button_get_active(pattern_box->page_0_15)){
    i = 0;
  }else if(gtk_check_button_get_active(pattern_box->page_16_31)){
    i = 1;
  }else if(gtk_check_button_get_active(pattern_box->page_32_47)){
    i = 2;
  }else if(gtk_check_button_get_active(pattern_box->page_48_63)){
    i = 3;
  }

  offset = (i * pattern_box->n_controls);

  /* get pads */
  list =
    start_list = ags_pattern_box_get_pad(pattern_box);

  /* reset */
  pattern_box->flags |= AGS_PATTERN_BOX_BLOCK_PATTERN;

  for(i = 0; i < pattern_box->n_controls; i++){
    set_active = TRUE;

    line =
      start_line = ags_pad_get_line(AGS_PAD(machine->selected_input_pad));

    while((line = ags_line_find_next_grouped(line)) != NULL){
      GList *start_pattern, *pattern;
      
      selected_line = AGS_LINE(line->data);

      g_object_get(selected_line->channel,
		   "pattern", &start_pattern,
		   NULL);

      /* check active */
      pattern = start_pattern;
      
      is_active = ags_pattern_get_bit((AgsPattern *) pattern->data, index0, index1, offset + i);

      g_list_free_full(start_pattern,
		       g_object_unref);
      
      if(!is_active){	
	set_active = FALSE;

	break;
      }

      /* iterate */
      line = line->next;
    }

    g_list_free(start_line);
    
    gtk_toggle_button_set_active((GtkToggleButton *) list->data, set_active);
    
    list = list->next;
  }

  pattern_box->flags &= (~AGS_PATTERN_BOX_BLOCK_PATTERN);

  g_list_free(start_list);
}

/**
 * ags_pattern_box_led_queue_draw_timeout:
 * @pattern_box: the #AgsPatternBox
 *
 * Queue draw led.
 *
 * Returns: %TRUE if continue timeout, otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_pattern_box_led_queue_draw_timeout(AgsPatternBox *pattern_box)
{
  if(g_hash_table_lookup(ags_pattern_box_led_queue_draw,
			 pattern_box) != NULL){
    AgsMachine *machine;

    AgsAudio *audio;
    AgsRecallID *recall_id;
    
    AgsFxPatternAudio *play_fx_pattern_audio;
    AgsFxPatternAudioProcessor *play_fx_pattern_audio_processor;

    GList *start_list, *list;
    GList *start_recall, *recall;
    
    guint64 active_led_new;
    gboolean success;
    
    GRecMutex *play_fx_pattern_audio_processor_mutex;
    
    machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) pattern_box,
						     AGS_TYPE_MACHINE);

    if(machine == NULL){
      return(TRUE);
    }
    
    audio = machine->audio;
    
    /* get some recalls */
    recall_id = NULL;
    g_object_get(audio,
		 "recall-id", &start_list,
		 NULL);

    list = start_list;

    success = FALSE;
    
    while(list != NULL && !success){
      AgsRecyclingContext *parent_recycling_context;
      AgsRecyclingContext *current_recycling_context;

      parent_recycling_context = NULL;
      current_recycling_context = NULL;
      
      g_object_get(list->data,
		   "recycling-context", &current_recycling_context,
		   NULL);
      
      if(current_recycling_context != NULL){
	g_object_get(current_recycling_context,
		     "parent", &parent_recycling_context,
		     NULL);

	if(parent_recycling_context == NULL &&
	   ags_recall_id_check_sound_scope(list->data, AGS_SOUND_SCOPE_SEQUENCER)){
	  recall_id = list->data;

	  g_object_get(audio,
		       "play", &start_recall,
		       NULL);

	  play_fx_pattern_audio = NULL;
	  play_fx_pattern_audio_processor = NULL;
    
	  recall = ags_recall_find_type(start_recall,
					AGS_TYPE_FX_PATTERN_AUDIO);
    
	  if(recall != NULL){
	    play_fx_pattern_audio = AGS_FX_PATTERN_AUDIO(recall->data);
	  }
    
	  recall = ags_recall_find_type_with_recycling_context(start_recall,
							       AGS_TYPE_FX_PATTERN_AUDIO_PROCESSOR,
							       (GObject *) current_recycling_context);
    
	  if(recall != NULL){
	    play_fx_pattern_audio_processor = AGS_FX_PATTERN_AUDIO_PROCESSOR(recall->data);
	  }


	  g_list_free_full(start_recall,
			   g_object_unref);

	  if(play_fx_pattern_audio == NULL ||
	     play_fx_pattern_audio_processor == NULL){
	    recall_id = NULL;
	  }else{
	    success = TRUE;
	  }
	}
      }

      if(parent_recycling_context != NULL){
	g_object_unref(parent_recycling_context);
      }

      if(current_recycling_context != NULL){
	g_object_unref(current_recycling_context);
      }

      list = list->next;
    }

    g_list_free_full(start_list,
		     g_object_unref);
    
    if(recall_id == NULL){      
      return(TRUE);
    }
    
    /* active led */
    play_fx_pattern_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(play_fx_pattern_audio_processor);

    g_rec_mutex_lock(play_fx_pattern_audio_processor_mutex);

    active_led_new = play_fx_pattern_audio_processor->offset_counter;
    
    g_rec_mutex_unlock(play_fx_pattern_audio_processor_mutex);

    pattern_box->active_led = (guint) (active_led_new % pattern_box->n_controls);

    ags_led_array_unset_all((AgsLedArray *) pattern_box->hled_array);
    ags_led_array_set_nth((AgsLedArray *) pattern_box->hled_array,
			  pattern_box->active_led);
        
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_pattern_box_new:
 *
 * Create a new instance of #AgsPatternBox
 *
 * Returns: the new #AgsPatternBox
 *
 * Since: 3.0.0
 */
AgsPatternBox*
ags_pattern_box_new()
{
  AgsPatternBox *pattern_box;

  pattern_box = (AgsPatternBox *) g_object_new(AGS_TYPE_PATTERN_BOX,
					       NULL);

  return(pattern_box);
}
