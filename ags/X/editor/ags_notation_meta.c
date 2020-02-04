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

#include <ags/X/editor/ags_notation_meta.h>
#include <ags/X/editor/ags_notation_meta_callbacks.h>

#include <ags/X/ags_notation_editor.h>

#include <ags/i18n.h>

void ags_notation_meta_class_init(AgsNotationMetaClass *notation_meta);
void ags_notation_meta_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_notation_meta_init(AgsNotationMeta *notation_meta);

void ags_notation_meta_connect(AgsConnectable *connectable);
void ags_notation_meta_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_notation_meta
 * @short_description: notation_meta
 * @title: AgsNotationMeta
 * @section_id:
 * @include: ags/X/editor/ags_notation_meta.h
 *
 * The #AgsNotationMeta provides you information about notation editor.
 */

GType
ags_notation_meta_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_notation_meta = 0;

    static const GTypeInfo ags_notation_meta_info = {
      sizeof (AgsNotationMetaClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_notation_meta_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsNotationMeta),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_notation_meta_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_notation_meta_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_notation_meta = g_type_register_static(GTK_TYPE_VBOX,
						    "AgsNotationMeta", &ags_notation_meta_info,
						    0);
    
    g_type_add_interface_static(ags_type_notation_meta,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_notation_meta);
  }

  return g_define_type_id__volatile;
}

void
ags_notation_meta_class_init(AgsNotationMetaClass *notation_meta)
{
  /* empty */
}

void
ags_notation_meta_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_notation_meta_connect;
  connectable->disconnect = ags_notation_meta_disconnect;
}

void
ags_notation_meta_init(AgsNotationMeta *notation_meta)
{
  GtkGrid *grid;
  GtkLabel *label;

  guint i;
  
  notation_meta->flags = 0;

  grid = gtk_grid_new();
  gtk_box_pack_start((GtkBox *) notation_meta,
		     (GtkWidget *) grid,
		     FALSE,
		     TRUE,
		     0);

  /* machine type */
  i = 0;
  
  label = (GtkLabel *) gtk_label_new(i18n("machine type: "));
  g_object_set(label,
	       "halign", GTK_ALIGN_START,
	       NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, i,
		  1, 1);

  notation_meta->machine_type = (GtkLabel *) gtk_label_new("(null)");
  g_object_set(notation_meta->machine_type,
	       "halign", GTK_ALIGN_START,
	       NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) notation_meta->machine_type,
		  1, i,
		  1, 1);
  
  /* machine name */
  i++;
  
  label = (GtkLabel *) gtk_label_new(i18n("machine name: "));
  g_object_set(label,
	       "halign", GTK_ALIGN_START,
	       NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, i,
		  1, 1);

  notation_meta->machine_name = (GtkLabel *) gtk_label_new("(null)");
  g_object_set(notation_meta->machine_name,
	       "halign", GTK_ALIGN_START,
	       NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) notation_meta->machine_name,
		  1, i,
		  1, 1);

  /* audio channels */
  i++;
  
  label = (GtkLabel *) gtk_label_new(i18n("audio channels: "));
  g_object_set(label,
	       "halign", GTK_ALIGN_START,
	       NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, i,
		  1, 1);

  notation_meta->audio_channels = (GtkLabel *) gtk_label_new("-1");
  g_object_set(notation_meta->audio_channels,
	       "halign", GTK_ALIGN_START,
	       NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) notation_meta->audio_channels,
		  1, i,
		  1, 1);

  /* output pads */
  i++;
  
  label = (GtkLabel *) gtk_label_new(i18n("output pads: "));
  g_object_set(label,
	       "halign", GTK_ALIGN_START,
	       NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, i,
		  1, 1);

  notation_meta->output_pads = (GtkLabel *) gtk_label_new("-1");
  g_object_set(notation_meta->output_pads,
	       "halign", GTK_ALIGN_START,
	       NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) notation_meta->output_pads,
		  1, i,
		  1, 1);

  /* input pads */
  i++;
  
  label = (GtkLabel *) gtk_label_new(i18n("input pads: "));
  g_object_set(label,
	       "halign", GTK_ALIGN_START,
	       NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, i,
		  1, 1);

  notation_meta->input_pads = (GtkLabel *) gtk_label_new("-1");
  g_object_set(notation_meta->input_pads,
	       "halign", GTK_ALIGN_START,
	       NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) notation_meta->input_pads,
		  1, i,
		  1, 1);

  /* editor tool */
  i++;
  
  label = (GtkLabel *) gtk_label_new(i18n("editor tool: "));
  g_object_set(label,
	       "halign", GTK_ALIGN_START,
	       NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, i,
		  1, 1);

  notation_meta->editor_tool = (GtkLabel *) gtk_label_new("(null)");
  g_object_set(notation_meta->editor_tool,
	       "halign", GTK_ALIGN_START,
	       NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) notation_meta->editor_tool,
		  1, i,
		  1, 1);

  /* active audio channel */
  i++;
  
  label = (GtkLabel *) gtk_label_new(i18n("active audio channel: "));
  g_object_set(label,
	       "halign", GTK_ALIGN_START,
	       "valign", GTK_ALIGN_START,
	       NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, i,
		  1, 1);

  notation_meta->active_audio_channel = (GtkLabel *) gtk_label_new("(null)");
  g_object_set(notation_meta->active_audio_channel,
	       "halign", GTK_ALIGN_START,
	       NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) notation_meta->active_audio_channel,
		  1, i,
		  1, 1);

  /* cursor x-position */
  i++;
  
  label = (GtkLabel *) gtk_label_new(i18n("cursor x-position: "));
  g_object_set(label,
	       "halign", GTK_ALIGN_START,
	       NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, i,
		  1, 1);

  notation_meta->cursor_x_position = (GtkLabel *) gtk_label_new("-1");
  g_object_set(notation_meta->cursor_x_position,
	       "halign", GTK_ALIGN_START,
	       NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) notation_meta->cursor_x_position,
		  1, i,
		  1, 1);

  /* cursor y-position */
  i++;
  
  label = (GtkLabel *) gtk_label_new(i18n("cursor y-position: "));
  g_object_set(label,
	       "halign", GTK_ALIGN_START,
	       NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, i,
		  1, 1);

  notation_meta->cursor_y_position = (GtkLabel *) gtk_label_new("-1");
  g_object_set(notation_meta->cursor_y_position,
	       "halign", GTK_ALIGN_START,
	       NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) notation_meta->cursor_y_position,
		  1, i,
		  1, 1);

  /* current note */
  i++;
  
  label = (GtkLabel *) gtk_label_new(i18n("current note: "));
  g_object_set(label,
	       "halign", GTK_ALIGN_START,
	       "valign", GTK_ALIGN_START,
	       NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, i,
		  1, 1);

  notation_meta->current_note = (GtkLabel *) gtk_label_new("(null)");
  g_object_set(notation_meta->current_note,
	       "halign", GTK_ALIGN_START,
	       NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) notation_meta->current_note,
		  1, i,
		  1, 1);
}

void
ags_notation_meta_connect(AgsConnectable *connectable)
{
  AgsNotationEditor *notation_editor;
  AgsNotationMeta *notation_meta;

  notation_meta = AGS_NOTATION_META(connectable);

  if((AGS_NOTATION_META_CONNECTED & (notation_meta->flags)) != 0){
    return;
  }

  notation_meta->flags |= AGS_NOTATION_META_CONNECTED;

  notation_editor = gtk_widget_get_ancestor(notation_meta,
					    AGS_TYPE_NOTATION_EDITOR);

  if(notation_editor != NULL){
    g_signal_connect_after(notation_editor, "machine-changed",
			   G_CALLBACK(ags_notation_meta_machine_changed_callback), notation_meta);
  }
}

void
ags_notation_meta_disconnect(AgsConnectable *connectable)
{
  AgsNotationEditor *notation_editor;
  AgsNotationMeta *notation_meta;
  
  notation_meta = AGS_NOTATION_META(connectable);

  if((AGS_NOTATION_META_CONNECTED & (notation_meta->flags)) == 0){
    return;
  }

  notation_meta->flags &= (~AGS_NOTATION_META_CONNECTED);

  notation_editor = gtk_widget_get_ancestor(notation_meta,
					    AGS_TYPE_NOTATION_EDITOR);

  if(notation_editor != NULL){
    g_object_disconnect(notation_editor,
			"any_signal::machine-changed",
			G_CALLBACK(ags_notation_meta_machine_changed_callback),
			notation_meta,
			NULL);
  }
}

/**
 * ags_notation_meta_refresh:
 * @notation_meta: the #AgsNotationMeta
 * 
 * Refresh @notation_meta.
 * 
 * Since: 3.1.0
 */
void
ags_notation_meta_refresh(AgsNotationMeta *notation_meta)
{
  AgsNotationEditor *notation_editor;
  
  if(!AGS_IS_NOTATION_META(notation_meta)){
    return;
  }
  
  notation_editor = gtk_widget_get_ancestor(notation_meta,
					    AGS_TYPE_NOTATION_EDITOR);

  if(notation_editor == NULL){
    return;
  }

  if(notation_editor->selected_machine == NULL){
    gtk_label_set_label(notation_meta->machine_type,
			"(null)"); 

    gtk_label_set_label(notation_meta->machine_name,
			"(null)"); 

    gtk_label_set_label(notation_meta->audio_channels,
			"-1");

    gtk_label_set_label(notation_meta->output_pads,
			"-1"); 

    gtk_label_set_label(notation_meta->input_pads,
			"-1"); 

    gtk_label_set_label(notation_meta->editor_tool,
			"(null)");

    gtk_label_set_label(notation_meta->active_audio_channel,
			"(null)"); 

    gtk_label_set_label(notation_meta->cursor_x_position,
			"-1"); 

    gtk_label_set_label(notation_meta->cursor_y_position,
			"-1"); 

    gtk_label_set_label(notation_meta->current_note,
			"(null)"); 
  }else{
    AgsTimestamp *timestamp;
    
    GList *start_notation, *notation;
    GList *start_note, *note;
    
    gchar *str;
    
    guint audio_channels;
    guint output_pads, input_pads;
    gint active_start, active_end;
    gint position;
    guint x0, y0;
    guint x1, y1;
    guint i;
    
    gtk_label_set_label(notation_meta->machine_type,
			G_OBJECT_TYPE_NAME(notation_editor->selected_machine)); 

    gtk_label_set_label(notation_meta->machine_name,
			notation_editor->selected_machine->machine_name); 

    g_object_get(notation_editor->selected_machine->audio,
		 "audio-channels", &audio_channels,
		 "output-pads", &output_pads,
		 "input-pads", &input_pads,
		 NULL);
    
    /* audio channels */
    str = g_strdup_printf("%u", audio_channels);
    gtk_label_set_label(notation_meta->audio_channels,
			str); 

    g_free(str);
    
    /* output pads */
    str = g_strdup_printf("%u", output_pads);
    gtk_label_set_label(notation_meta->output_pads,
			str);

    g_free(str);

    /* input pads */
    str = g_strdup_printf("%u", input_pads);
    gtk_label_set_label(notation_meta->input_pads,
			str);

    g_free(str);

    str = NULL;

    if(notation_editor->notation_toolbar->selected_edit_mode == notation_editor->notation_toolbar->position){
      str = i18n("position");
    }else if(notation_editor->notation_toolbar->selected_edit_mode == notation_editor->notation_toolbar->edit){
      str = i18n("edit");
    }else if(notation_editor->notation_toolbar->selected_edit_mode == notation_editor->notation_toolbar->clear){
      str = i18n("clear");
    }else if(notation_editor->notation_toolbar->selected_edit_mode == notation_editor->notation_toolbar->select){
      str = i18n("select");
    }

    if(str != NULL){
      gtk_label_set_label(notation_meta->editor_tool,
			  str);
    }else{
      gtk_label_set_label(notation_meta->editor_tool,
			  "(null)");
    }
    
    /* active audio channels */
    str = NULL;
    
    active_start = -1;
    active_end = -1;

    position = 0;
    
    for(; (position = ags_notebook_next_active_tab(notation_editor->notebook, position)) != -1; position++){
      if(active_start == -1){
	active_start = position;
	active_end = position;
      }else{
	if(position == active_end + 1){
	  active_end = position;
	}else{
	  if(str == NULL){
	    if(active_start == active_end){
	      str = g_strdup_printf("%d",
				    active_start);
	    }else{
	      str = g_strdup_printf("%d-%d",
				    active_start,
				    active_end);
	    }
	  }else{
	    if(active_start == active_end){
	      gchar *tmp;
	    
	      tmp = g_strdup_printf("%s, %d",
				    str,
				    active_start);

	      g_free(str);

	      str = tmp;
	    }else{
	      gchar *tmp;
	    
	      tmp = g_strdup_printf("%s, %d-%d",
				    str,
				    active_start,
				    active_end);

	      g_free(str);

	      str = tmp;
	    }
	  }

	  active_start = position;
	  active_end = position;
	}
      }
    }

    if(active_start == -1){
      gtk_label_set_label(notation_meta->active_audio_channel,
			  "(null)"); 
    }else{
      if(str == NULL){
	if(active_start == active_end){
	  str = g_strdup_printf("[%d]",
				active_start);
	}else{
	  gchar *tmp;
	    
	  tmp = g_strdup_printf("[%d-%d]",
				active_start,
				active_end);

	  g_free(str);

	  str = tmp;
	}
      }else{
	if(active_start == active_end){
	  gchar *tmp;
	    
	  tmp = g_strdup_printf("[%s, %d]",
				str,
				active_start);

	  g_free(str);

	  str = tmp;
	}else{
	  gchar *tmp;
	    
	  tmp = g_strdup_printf("[%s, %d-%d]",
				str,
				active_start,
				active_end);

	  g_free(str);

	  str = tmp;
	}
      }

      gtk_label_set_label(notation_meta->active_audio_channel,
			  str);

      g_free(str);
    }

    /* cursor position x */
    str = g_strdup_printf("%u", notation_editor->notation_edit->cursor_position_x);
    gtk_label_set_label(notation_meta->cursor_x_position,
			str);

    g_free(str);

    /* cursor position y */
    str = g_strdup_printf("%u", notation_editor->notation_edit->cursor_position_y);
    gtk_label_set_label(notation_meta->cursor_y_position,
			str);

    g_free(str);

    /* current note */
    g_object_get(notation_editor->selected_machine->audio,
		 "notation", &start_notation,
		 NULL);

    timestamp = ags_timestamp_new();

    timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
    timestamp->flags |= AGS_TIMESTAMP_OFFSET;
    
    x0 = notation_editor->notation_edit->cursor_position_x;
    y0 = notation_editor->notation_edit->cursor_position_y;

    x1 = x0 + exp2(6.0 - (double) gtk_combo_box_get_active(notation_editor->notation_toolbar->zoom));
    
    y1 = y0 + 1;

    str = NULL;
    
    for(i = 0; i < audio_channels;){
      gchar *current_str;

      timestamp->timer.ags_offset.offset = AGS_NOTATION_DEFAULT_OFFSET * floor(x0 / AGS_NOTATION_DEFAULT_OFFSET);
      
      position = i;
      position = ags_notebook_next_active_tab(notation_editor->notebook, position);

      if(position == -1){
	break;
      }

      i = position;

    ags_notation_meta_refresh_CURRENT_NOTE_TIMESTAMP_NO2:
      notation = start_notation;

      current_str = NULL;
      
      while((notation = ags_notation_find_near_timestamp(notation, i,
							 timestamp)) != NULL){
	start_note = ags_notation_find_region(notation->data,
					      x0, y0,
					      x1, y1,
					      FALSE);

	note = start_note;

	while(note != NULL){
	  guint x0, x1;
	  guint y;

	  g_object_get(note->data,
		       "x0", &x0,
		       "x1", &x1,
		       "y", &y,
		       NULL);

	  if(current_str == NULL){
	    current_str = g_strdup_printf("%u-%u",
					  x0,
					  x1);
	  }else{
	    gchar *tmp;

	    tmp = g_strdup_printf("%s, %u-%u",
				  current_str,
				  x0,
				  x1);

	    g_free(current_str);

	    current_str = tmp;
	  }

	  note = note->next;
	}
	
	notation = notation->next;
      }
    
      if(timestamp->timer.ags_offset.offset < AGS_NOTATION_DEFAULT_OFFSET * floor(x1 / AGS_NOTATION_DEFAULT_OFFSET)){
	timestamp->timer.ags_offset.offset = AGS_NOTATION_DEFAULT_OFFSET * floor(x1 / AGS_NOTATION_DEFAULT_OFFSET);
	goto ags_notation_meta_refresh_CURRENT_NOTE_TIMESTAMP_NO2;
      }
      
      if(current_str == NULL){
	if(str == NULL){
	  str = g_strdup_printf("@audio_channel[%d] -> (null)",
				i);
	}else{
	  gchar *tmp;
	  
	  tmp = g_strdup_printf("%s,\n  @audio_channel[%d] -> (null)",
				str,
				i);

	  g_free(str);

	  str = tmp;
	}
      }else{
	if(str == NULL){
	  str = g_strdup_printf("@audio_channel[%d] -> {%s}",
				i,
				current_str);
	}else{
	  gchar *tmp;
	  
	  tmp = g_strdup_printf("%s,\n  @audio_channel[%d] -> {%s}",
				str,
				i,
				current_str);

	  g_free(str);

	  str = tmp;
	}

	g_free(current_str);
      }
      
      i++;
    }

    g_list_free_full(start_notation,
		     (GDestroyNotify) g_object_unref);

    if(str == NULL){
      gtk_label_set_label(notation_meta->current_note,
			  "(null)");
    }else{
      gchar *tmp;
      
      tmp = g_strdup_printf("[%s]", str);
      gtk_label_set_label(notation_meta->current_note,
			  tmp);

      g_free(str);
      g_free(tmp);
    }
    
    g_object_unref(timestamp);
  }
}

/**
 * ags_notation_meta_new:
 *
 * Create a new #AgsNotationMeta.
 *
 * Returns: a new #AgsNotationMeta
 *
 * Since: 3.1.0
 */
AgsNotationMeta*
ags_notation_meta_new()
{
  AgsNotationMeta *notation_meta;

  notation_meta = (AgsNotationMeta *) g_object_new(AGS_TYPE_NOTATION_META,
						   NULL);

  return(notation_meta);
}
