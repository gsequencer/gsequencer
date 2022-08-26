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

#include <ags/app/editor/ags_sheet_edit.h>
#include <ags/app/editor/ags_sheet_edit_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_composite_editor.h>

#include <gdk/gdkkeysyms.h>
#include <cairo.h>
#include <cairo-ps.h>
#include <pango/pango.h>

#include <math.h>

void ags_sheet_edit_class_init(AgsSheetEditClass *sheet_edit);
void ags_sheet_edit_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sheet_edit_init(AgsSheetEdit *sheet_edit);
void ags_sheet_edit_finalize(GObject *gobject);

void ags_sheet_edit_connect(AgsConnectable *connectable);
void ags_sheet_edit_disconnect(AgsConnectable *connectable);

gboolean ags_sheet_edit_auto_scroll_timeout(GtkWidget *widget);

/**
 * SECTION:ags_sheet_edit
 * @short_description: edit notes
 * @title: AgsSheetEdit
 * @section_id:
 * @include: ags/app/editor/ags_sheet_edit.h
 *
 * The #AgsSheetEdit lets you edit notes.
 */

enum{
  PROP_0,
};

static gpointer ags_sheet_edit_parent_class = NULL;

GHashTable *ags_sheet_edit_auto_scroll = NULL;

GType
ags_sheet_edit_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sheet_edit = 0;

    static const GTypeInfo ags_sheet_edit_info = {
      sizeof (AgsSheetEditClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sheet_edit_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSheetEdit),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sheet_edit_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sheet_edit_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_sheet_edit = g_type_register_static(GTK_TYPE_GRID,
						 "AgsSheetEdit", &ags_sheet_edit_info,
						 0);
    
    g_type_add_interface_static(ags_type_sheet_edit,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sheet_edit);
  }

  return g_define_type_id__volatile;
}

void
ags_sheet_edit_class_init(AgsSheetEditClass *sheet_edit)
{
  GtkWidgetClass *widget;
  
  GObjectClass *gobject;
  
  ags_sheet_edit_parent_class = g_type_class_peek_parent(sheet_edit);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(sheet_edit);

  gobject->finalize = ags_sheet_edit_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) sheet_edit;
}

void
ags_sheet_edit_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_sheet_edit_connect;
  connectable->disconnect = ags_sheet_edit_disconnect;
}

void
ags_sheet_edit_init(AgsSheetEdit *sheet_edit)
{
  GtkAdjustment *adjustment;

  AgsSheetEditScript *g_clef_script, *f_clef_script;
  
  sheet_edit->flags = 0;
  sheet_edit->connectable_flags = 0;
  sheet_edit->mode = AGS_SHEET_EDIT_NO_EDIT_MODE;

  sheet_edit->button_mask = 0;
  sheet_edit->key_mask = 0;

  sheet_edit->note_offset = 0;
  sheet_edit->note_offset_absolute = 0;

  sheet_edit->cursor_position_x = AGS_SHEET_EDIT_DEFAULT_CURSOR_POSITION_X;
  sheet_edit->cursor_position_y = AGS_SHEET_EDIT_DEFAULT_CURSOR_POSITION_Y;

  sheet_edit->selection_x0 = 0;
  sheet_edit->selection_x1 = 0;
  sheet_edit->selection_y0 = 0;
  sheet_edit->selection_y1 = 0;

  sheet_edit->paper_name = g_strdup(AGS_SHEET_EDIT_DEFAULT_PAPER_NAME);

  sheet_edit->page_orientation = GTK_PAGE_ORIENTATION_PORTRAIT;
  
  sheet_edit->notation_x0 = 0;
  sheet_edit->notation_x1 = 64;

  sheet_edit->utf8_script_line = NULL;
  sheet_edit->utf8_script_note = NULL;

  sheet_edit->ps_surface = cairo_ps_surface_create(NULL,
						   AGS_SHEET_EDIT_DEFAULT_WIDTH, AGS_SHEET_EDIT_DEFAULT_HEIGHT);
  
  sheet_edit->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_can_focus((GtkWidget *) sheet_edit->drawing_area,
			   TRUE);
  gtk_widget_set_focusable((GtkWidget *) sheet_edit->drawing_area,
			   TRUE);

  gtk_widget_set_halign(sheet_edit->drawing_area,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign(sheet_edit->drawing_area,
			GTK_ALIGN_FILL);

  gtk_widget_set_hexpand((GtkWidget *) sheet_edit->drawing_area,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) sheet_edit->drawing_area,
			 TRUE);

  gtk_grid_attach(GTK_GRID(sheet_edit),
		  (GtkWidget *) sheet_edit->drawing_area,
		  0, 0,
		  1, 1);

  sheet_edit->vscrollbar = NULL;
  sheet_edit->hscrollbar = NULL;

  sheet_edit->script = NULL;

  /* g clef */
  g_clef_script = ags_sheet_edit_script_alloc();

  g_clef_script->is_primary = TRUE;

  g_clef_script->is_grand_staff = TRUE;

  g_clef_script->clef = AGS_SHEET_EDIT_G_CLEF;
  
  ags_sheet_edit_add_script(sheet_edit,
			       g_clef_script);

  /* f clef */
  f_clef_script = ags_sheet_edit_script_alloc();

  f_clef_script->clef = AGS_SHEET_EDIT_F_CLEF;

  f_clef_script->clef_translate_y = -14.0;

  g_clef_script->companion_script = f_clef_script;
  
  ags_sheet_edit_add_script(sheet_edit,
			       f_clef_script);
  
  /* auto-scroll */
  if(ags_sheet_edit_auto_scroll == NULL){
    ags_sheet_edit_auto_scroll = g_hash_table_new_full(g_direct_hash, g_direct_equal,
						       NULL,
						       NULL);
  }

  g_hash_table_insert(ags_sheet_edit_auto_scroll,
		      sheet_edit, ags_sheet_edit_auto_scroll_timeout);
  g_timeout_add(1000 / 30, (GSourceFunc) ags_sheet_edit_auto_scroll_timeout, (gpointer) sheet_edit);
}

void
ags_sheet_edit_finalize(GObject *gobject)
{
  AgsSheetEdit *sheet_edit;
  
  sheet_edit = AGS_SHEET_EDIT(gobject);

  g_free(sheet_edit->utf8_script_line);
  g_free(sheet_edit->utf8_script_note);

  cairo_surface_destroy(sheet_edit->ps_surface);
  
  /* remove auto scroll */
  g_hash_table_remove(ags_sheet_edit_auto_scroll,
		      sheet_edit);

  /* call parent */
  G_OBJECT_CLASS(ags_sheet_edit_parent_class)->finalize(gobject);
}

void
ags_sheet_edit_connect(AgsConnectable *connectable)
{
  AgsSheetEdit *sheet_edit;

  sheet_edit = AGS_SHEET_EDIT(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (sheet_edit->connectable_flags)) != 0){
    return;
  }

  sheet_edit->connectable_flags |= AGS_CONNECTABLE_CONNECTED;  
  
  /* drawing area */
  gtk_drawing_area_set_draw_func(sheet_edit->drawing_area,
				 ags_sheet_edit_draw_callback,
				 sheet_edit,
				 NULL);

  g_signal_connect_after((GObject *) sheet_edit->drawing_area, "resize",
			 G_CALLBACK(ags_sheet_edit_drawing_area_resize_callback), (gpointer) sheet_edit);

  //TODO:JK: implement me
}

void
ags_sheet_edit_disconnect(AgsConnectable *connectable)
{
  AgsSheetEdit *sheet_edit;

  sheet_edit = AGS_SHEET_EDIT(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (sheet_edit->connectable_flags)) == 0){
    return;
  }

  sheet_edit->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  /* drawing area */
  gtk_drawing_area_set_draw_func(sheet_edit->drawing_area,
				 NULL,
				 NULL,
				 NULL);
  
  g_object_disconnect((GObject *) sheet_edit->drawing_area,
		      "any_signal::resize",
		      G_CALLBACK(ags_sheet_edit_drawing_area_resize_callback),
		      (gpointer) sheet_edit,
		      NULL);

  //TODO:JK: implement me
}

gboolean
ags_sheet_edit_auto_scroll_timeout(GtkWidget *widget)
{
  if(g_hash_table_lookup(ags_sheet_edit_auto_scroll,
			 widget) != NULL){
    AgsCompositeEditor *composite_editor;
    AgsSheetEdit *sheet_edit;

    GObject *output_soundcard;
    
    double x;
    
    sheet_edit = AGS_SHEET_EDIT(widget);

    if((AGS_SHEET_EDIT_AUTO_SCROLL & (sheet_edit->flags)) == 0){
      return(TRUE);
    }
    
    composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) sheet_edit,
								      AGS_TYPE_COMPOSITE_EDITOR);
    
    if(composite_editor->selected_machine == NULL){
      return(TRUE);
    }

    //TODO:JK: implement me
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

#if 0
void
ags_sheet_edit_draw_notation(AgsSheetEdit *sheet_edit, cairo_t *cr)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *toolbar;
  AgsMachine *selected_machine;

  AgsNotebook *notebook;

  GtkSettings *settings;
  
  AgsTimestamp *timestamp;
  AgsTimestamp *current_timestamp;    

  AgsApplicationContext *application_context;

  GList *start_list_notation, *list_notation;

  gchar *font_name;
  
  gdouble page_width, page_height;
  gdouble offset;
  guint x0, x1;
  guint prev_note_x0, prev_note_x1;
  guint audio_channels;
  guint input_pads;
  guint midi_start_mapping;
  guint audio_channel;
  guint i;
  guint offset_counter;
  gboolean is_next_rest;
  
  const gchar* note_strv[] = {
    "𝅝",
    "𝅗𝅥",
    "𝅘𝅥",
    "𝅘𝅥𝅮",
    "𝅘𝅥𝅯",
    NULL,
  };
  
  application_context = ags_application_context_get_instance();

  settings = gtk_settings_get_default();

  font_name = NULL;
  
  g_object_get(settings,
	       "gtk-font-name", &font_name,
	       NULL);

  /*  */
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) sheet_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);
    
  notebook = composite_editor->sheet_edit->channel_selector;
    
  toolbar = composite_editor->toolbar;

  selected_machine = composite_editor->selected_machine;

  if(sheet_edit->page_orientation == GTK_PAGE_ORIENTATION_PORTRAIT){
    page_width = AGS_SHEET_EDIT_DEFAULT_WIDTH;
    page_height = AGS_SHEET_EDIT_DEFAULT_HEIGHT;
  }else{
    page_width = AGS_SHEET_EDIT_DEFAULT_HEIGHT;
    page_height = AGS_SHEET_EDIT_DEFAULT_WIDTH;
  }  

  offset = 0.0;  

  /* clear with white color */
  cairo_set_source_rgba(cr,
			1.0,
			1.0,
			1.0,
			1.0);
  cairo_rectangle(cr,
		  (gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING, (gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING,
		  (double) page_width, (double) page_height);
  cairo_fill(cr);

  ags_sheet_edit_draw_script(sheet_edit, cr,
				AGS_SHEET_EDIT_G_CLEF,
				7,
				TRUE,
				0,
				(gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT, (gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP,
				page_width - (AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_RIGHT), 4.0 * AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT);

  ags_sheet_edit_draw_script(sheet_edit, cr,
				AGS_SHEET_EDIT_G_CLEF,
				7,
				FALSE,
				0,
				(gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT, (gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP + 4.0 * AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT + AGS_SHEET_EDIT_DEFAULT_TABLATUR_SPACING,
				page_width - (AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_RIGHT), 4.0 * AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT);


  ags_sheet_edit_draw_script(sheet_edit, cr,
				AGS_SHEET_EDIT_F_CLEF,
				7,
				TRUE,
				0,
				(gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT, (gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP + 2.0 * (4.0 * AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT + AGS_SHEET_EDIT_DEFAULT_TABLATUR_SPACING),
				page_width - (AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_RIGHT), 4.0 * AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT);

  ags_sheet_edit_draw_script(sheet_edit, cr,
				AGS_SHEET_EDIT_F_CLEF,
				7,
				FALSE,
				0,
				(gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT, (gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP + 3.0 * (4.0 * AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT + AGS_SHEET_EDIT_DEFAULT_TABLATUR_SPACING),
				page_width - (AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_RIGHT), 4.0 * AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT);

  /* test */
  x0 = 0;

  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  start_list_notation = ags_audio_get_notation(selected_machine->audio);

  audio_channels = ags_audio_get_audio_channels(selected_machine->audio);

  input_pads = ags_audio_get_input_pads(selected_machine->audio);

  midi_start_mapping = ags_audio_get_midi_start_mapping(selected_machine->audio);
  
  timestamp->timer.ags_offset.offset = (guint64) AGS_NOTATION_DEFAULT_OFFSET * floor((double) x0 / (double) AGS_NOTATION_DEFAULT_OFFSET);

  offset = 100.0;

  offset_counter = 0;

  prev_note_x0 = 0;
  prev_note_x1 = 0;

  is_next_rest = FALSE;
  
  for(i = 0; i < audio_channels; i++){
    list_notation = ags_notation_find_near_timestamp(start_list_notation, i,
						     timestamp);
    
    while(list_notation != NULL){
      AgsNotation *notation;

      GList *start_list_note, *list_note;

      notation = AGS_NOTATION(list_notation->data);

      g_object_get(notation,
		   "audio-channel", &audio_channel,
		   "timestamp", &current_timestamp,
		   NULL);

      if(ags_timestamp_get_ags_offset(current_timestamp) > AGS_NOTATION_DEFAULT_OFFSET * floor((double) x1 / (double) AGS_NOTATION_DEFAULT_OFFSET) + AGS_NOTATION_DEFAULT_OFFSET){
	break;
      }

      if(ags_timestamp_get_ags_offset(current_timestamp) + AGS_NOTATION_DEFAULT_OFFSET < x0){
	list_notation = list_notation->next;

	continue;
      }
      
      if(i != audio_channel){
	list_notation = list_notation->next;

	continue;
      }

      start_list_note = NULL;

      g_object_get(notation,
		   "note", &start_list_note,
		   NULL);
      
      list_note = start_list_note;

      while(list_note != NULL){
	AgsNote *note, *note_next;

	PangoLayout *layout;
	PangoFontDescription *desc;
	
	PangoRectangle ink_rect, logical_rect;

	GList *list;

	gchar *note_str;
	gchar *rest_str;
	
	guint note_x0, note_x1;
	guint note_y;
	guint note_next_x0, note_next_x1;
	guint note_next_pos_x0, note_next_pos_x1;
	gboolean success;
	
	note = AGS_NOTE(list_note->data);

	note_next = NULL;

	if(list_note->next != NULL){
	  note_next = AGS_NOTE(list_note->next->data);
	}
	
	note_x0 = 0;
	note_x1 = 0;

	note_y = 0;
	
	g_object_get(note,
		     "x0", &note_x0,
		     "x1", &note_x1,
		     "y", &note_y,
		     NULL);
	
	note_next_x0 = 0;
	note_next_x1 = 0;

	if(note_next != NULL){
	  g_object_get(note_next,
		       "x0", &note_next_x0,
		       "x1", &note_next_x1,
		       NULL);
	}

	note_next_pos_x0 = 0;
	note_next_pos_x1 = 0;

	list = list_note->next;
		    
	while(list != NULL){
	  note_next_pos_x0 = 0;
	  note_next_pos_x1 = 0;
		      
	  g_object_get(list->data,
		       "x0", &note_next_pos_x0,
		       "x1", &note_next_pos_x1,
		       NULL);

	  if(note_next_pos_x0 > note_x0){
	    break;
	  }
		      
	  list = list->next;
	}

	if(list == NULL){
	  note_next_pos_x0 = 0;
	  note_next_pos_x1 = 0;
	}
	
	note_str = NULL;
	rest_str = NULL;

	success = FALSE;

	if(!is_next_rest){
	  if((note_next_pos_x0 != note_next_pos_x1 &&
	      note_next_pos_x0 - note_x0 <= 16)){
	    if(note_x1 <= (guint) (16.0 * floor(note_x0 / 16.0) + 16.0)){
	      if((gint) log2((gdouble) (note_next_pos_x0 - note_x0)) >= 0 &&
		 (gint) log2((gdouble) (note_next_pos_x0 - note_x0)) < 5){
		success = TRUE;
	  
		note_str = note_strv[4 - (gint) log2((gdouble) (note_next_pos_x0 - note_x0))];

		if(note_x0 != note_next_x0){
		  offset_counter += (note_next_pos_x0 - note_x0);
		}
	      }
	    }else{
	    }
	  }else if((note_next_pos_x0 != note_next_pos_x1 &&
		    note_next_pos_x0 - note_x0 > 16)){
	    if(note_x1 <= (guint) (16.0 * floor(note_x0 / 16.0) + 16.0)){
	      if((gint) log2((gdouble) ((guint) (16.0 * floor(note_x0 / 16.0) + 16.0) - note_x0)) >= 0 &&
		 (gint) log2((gdouble) ((guint) (16.0 * floor(note_x0 / 16.0) + 16.0) - note_x0)) < 5){
		success = TRUE;
		is_next_rest = TRUE;
	  
		note_str = note_strv[4 - (gint) log2((gdouble) ((guint) (16.0 * floor(note_x0 / 16.0) + 16.0) - note_x0))];

		if(note_x0 != note_next_x0){
		  offset_counter += ((guint) (16.0 * floor(note_x0 / 16.0) + 16.0) - note_x0);
		}
	      }
	    }else{
	    }	  
	  }else if(note_next_pos_x0 == note_next_pos_x1 &&
		   note_x1 - note_x0 <= 16){
	    if(note_x1 <= (guint) (16.0 * floor(note_x0 / 16.0) + 16.0)){
	      if((gint) log2((gdouble) (note_x1 - note_x0)) >= 0 &&
		 (gint) log2((gdouble) (note_x1 - note_x0)) < 5){
		success = TRUE;
	  
		note_str = note_strv[4 - (gint) log2((gdouble) (note_x1 - note_x0))];

		offset_counter += (note_x1 - note_x0);
	      }
	    }else{
	    }
	  }
	}else{
	  is_next_rest = FALSE;
	}
	
	if(!success){
	  if(prev_note_x0 != prev_note_x1){
	    if(offset_counter % 16 == 0 && offset_counter + 16 <= note_x0){
	      rest_str = "𝄻";

	      offset_counter += 16;
	    }else if(offset_counter % 8 == 0 && offset_counter + 8 <= note_x0 && prev_note_x0 + 8 <= offset_counter){
	      rest_str = "𝄼";

	      offset_counter += 8;
	    }else if(offset_counter % 4 == 0 && offset_counter + 4 <= note_x0 && prev_note_x0 + 4 <= offset_counter){
	      rest_str = "𝄽";

	      offset_counter += 4;
	    }else if(offset_counter % 2 == 0 && offset_counter + 2 <= note_x0 && prev_note_x0 + 2 <= offset_counter){
	      rest_str = "𝄾";

	      offset_counter += 2;
	    }else if(offset_counter + 1 == note_x0 && prev_note_x0 + 1 == offset_counter){
	      rest_str = "𝄿";

	      offset_counter += 1;
	    }
	  }else{
	    if(offset_counter % 16 == 0 && offset_counter + 16 <= note_x0){
	      rest_str = "𝄻";

	      offset_counter += 16;
	    }else if(offset_counter % 8 == 0 && offset_counter + 8 <= note_x0){
	      rest_str = "𝄼";

	      offset_counter += 8;
	    }else if(offset_counter % 4 == 0 && offset_counter + 4 <= note_x0){
	      rest_str = "𝄽";

	      offset_counter += 4;
	    }else if(offset_counter % 2 == 0 && offset_counter + 2 <= note_x0){
	      rest_str = "𝄾";

	      offset_counter += 2;
	    }else if(offset_counter + 1 == note_x0){
	      rest_str = "𝄿";

	      offset_counter += 1;
	    }
	  }
	}
	
	if(note_str != NULL){
	  layout = pango_cairo_create_layout(cr);
	  pango_layout_set_text(layout,
				note_str,
				-1);
	  desc = pango_font_description_from_string(font_name);
	  pango_font_description_set_size(desc,
					  AGS_SHEET_EDIT_DEFAULT_KEY_FONT_SIZE * PANGO_SCALE);
	  pango_layout_set_font_description(layout,
					    desc);
	  pango_font_description_free(desc);    

	  pango_layout_get_extents(layout,
				   &ink_rect,
				   &logical_rect);

	  //TODO:JK: improve me
	  cairo_move_to(cr,
			(gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + offset,
			(gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP + (((gdouble) note_y - 62.0) * ((gdouble) AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT / 2.0)) - (((gdouble) AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT * 4.0) / 8.0) - 1.0);
	  
	  pango_cairo_show_layout(cr,
				  layout);
    
	  if(note_x0 < note_next_x0 ||
	     note_next_x0 == note_next_x1){
	    offset += 2.5 * (logical_rect.width / PANGO_SCALE);

	    if(offset_counter % 16 == 0){
	      cairo_move_to(cr,
			    (gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + offset, (gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP);
	      cairo_line_to(cr,
			    (gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + offset, (gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP + ((gdouble) AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT * 4.0));
	      cairo_stroke(cr);	      

	      offset += 2.5 * (logical_rect.width / PANGO_SCALE);
	    }
	  }
	    
	  g_object_unref(layout);
	}

	if(rest_str != NULL){
	  layout = pango_cairo_create_layout(cr);
	  pango_layout_set_text(layout,
				rest_str,
				-1);
	  desc = pango_font_description_from_string(font_name);
	  pango_font_description_set_size(desc,
					  AGS_SHEET_EDIT_DEFAULT_REST_FONT_SIZE * PANGO_SCALE);
	  pango_layout_set_font_description(layout,
					    desc);
	  pango_font_description_free(desc);    

	  pango_layout_get_extents(layout,
				   &ink_rect,
				   &logical_rect);

	  //TODO:JK: improve me
	  cairo_move_to(cr,
			(gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + offset,
			(gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP + (1.0 * ((gdouble) AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT / 2.0)) - (((gdouble) AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT * 4.0) / 8.0) - 1.0);
	  
	  pango_cairo_show_layout(cr,
				  layout);
    
	  offset += 2.5 * (logical_rect.width / PANGO_SCALE);

	  if(offset_counter % 16 == 0){	      
	    cairo_move_to(cr,
			  (gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + offset, (gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP);
	    cairo_line_to(cr,
			  (gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + offset, (gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP + ((gdouble) AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT * 4.0));
	    cairo_stroke(cr);	      

	    offset += 2.5 * (logical_rect.width / PANGO_SCALE);
	  }
	    
	  g_object_unref(layout);
	}
	
	if(success){
	  list_note = list_note->next;
	}
      }

      g_list_free_full(start_list_note,
		       g_object_unref);
      
      list_notation = list_notation->next;
    }
  }
}
#endif

AgsSheetEditScript*
ags_sheet_edit_script_alloc()
{
  AgsSheetEditScript *sheet_edit_script;

  sheet_edit_script = (AgsSheetEditScript *) g_new0(AgsSheetEditScript,
							  1);

  sheet_edit_script->is_primary = FALSE;

  sheet_edit_script->notation_x_start = 0;
  sheet_edit_script->notation_x_end = 16;
  
  sheet_edit_script->font_size = AGS_SHEET_EDIT_DEFAULT_FONT_SIZE;

  sheet_edit_script->margin_top = 15.0;
  sheet_edit_script->margin_bottom = 25.0;
  sheet_edit_script->margin_left = 36.5;
  sheet_edit_script->margin_right = 10.0;

  sheet_edit_script->is_grand_staff = FALSE;

  sheet_edit_script->grand_staff_brace_translate_x = -24.0;
  sheet_edit_script->grand_staff_brace_translate_y = -22.0;
  sheet_edit_script->grand_staff_brace_translate_z = 0.0;

  sheet_edit_script->grand_staff_brace_font_size = AGS_SHEET_EDIT_DEFAULT_GRAND_BRACE_STAFF_FONT_SIZE;

  sheet_edit_script->clef = AGS_SHEET_EDIT_G_CLEF;

  sheet_edit_script->clef_translate_x = 0.0;
  sheet_edit_script->clef_translate_y = -12.0;
  sheet_edit_script->clef_translate_z = 0.0;

  sheet_edit_script->clef_font_size = AGS_SHEET_EDIT_DEFAULT_CLEF_FONT_SIZE;
  
  sheet_edit_script->staff_extends_top = 0;
  sheet_edit_script->staff_extends_bottom = 0;

  sheet_edit_script->staff_x0 = 0.0;
  sheet_edit_script->staff_y0 = 0.0;

  sheet_edit_script->staff_width = AGS_SHEET_EDIT_DEFAULT_WIDTH - (sheet_edit_script->margin_left + sheet_edit_script->margin_right);
  sheet_edit_script->staff_height = ((4.0 + (gdouble) sheet_edit_script->staff_extends_top + (gdouble) sheet_edit_script->staff_extends_bottom) * sheet_edit_script->font_size);

  sheet_edit_script->is_minor = FALSE;
  sheet_edit_script->sharp_flat = 0;

  sheet_edit_script->sharp_translate_x = 0.0;
  sheet_edit_script->sharp_translate_y = 0.0;
  sheet_edit_script->sharp_translate_z = 0.0;

  sheet_edit_script->flat_translate_x = 0.0;
  sheet_edit_script->flat_translate_y = -4.0;
  sheet_edit_script->flat_translate_z = 0.0;

  sheet_edit_script->sharp_flat_font_size = AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_FONT_SIZE;

  sheet_edit_script->key_translate_x = 0.0;
  sheet_edit_script->key_translate_y = 6.0;
  sheet_edit_script->key_translate_z = 0.0;

  sheet_edit_script->reverse_key_translate_x = 0.0;
  sheet_edit_script->reverse_key_translate_y = -3.0;
  sheet_edit_script->reverse_key_translate_z = 0.0;

  sheet_edit_script->key_font_size = AGS_SHEET_EDIT_DEFAULT_KEY_FONT_SIZE;

  sheet_edit_script->rest_translate_x = 0.0;
  sheet_edit_script->rest_translate_y = 0.0;
  sheet_edit_script->rest_translate_z = 0.0;

  sheet_edit_script->rest_font_size = AGS_SHEET_EDIT_DEFAULT_REST_FONT_SIZE;
  
  sheet_edit_script->companion_script = NULL;
  
  return(sheet_edit_script);
}

void
ags_sheet_edit_script_free(AgsSheetEditScript *sheet_edit_script)
{
  g_return_if_fail(sheet_edit_script != NULL);

  g_free(sheet_edit_script);
}

GList*
ags_sheet_edit_get_script(AgsSheetEdit *sheet_edit)
{
  g_return_val_if_fail(AGS_IS_SHEET_EDIT(sheet_edit), NULL);

  return(g_list_reverse(g_list_copy(sheet_edit->script)));
}

void
ags_sheet_edit_add_script(AgsSheetEdit *sheet_edit,
			     AgsSheetEditScript *sheet_edit_script)
{
  g_return_if_fail(AGS_IS_SHEET_EDIT(sheet_edit));
  g_return_if_fail(sheet_edit_script != NULL);

  if(g_list_find(sheet_edit->script, sheet_edit_script) == NULL){
    sheet_edit->script = g_list_prepend(sheet_edit->script,
					   sheet_edit_script);
  }
}

void
ags_sheet_edit_remove_script(AgsSheetEdit *sheet_edit,
				AgsSheetEditScript *sheet_edit_script)
{
  g_return_if_fail(AGS_IS_SHEET_EDIT(sheet_edit));
  g_return_if_fail(sheet_edit_script != NULL);

  if(g_list_find(sheet_edit->script, sheet_edit_script) != NULL){
    sheet_edit->script = g_list_remove(sheet_edit->script,
					  sheet_edit_script);
  }
}

void
ags_sheet_edit_draw_staff(AgsSheetEdit *sheet_edit, cairo_t *cr,
			  AgsSheetEditScript *sheet_edit_script,
			  gdouble x0, gdouble y0,
			  gdouble width, gdouble height,
			  gdouble font_size)
{
  GtkSettings *settings;

  AgsSheetEditScript *current;
  
  gchar *font_name;

  gdouble first_line_x0, first_line_y0;
  gdouble current_y0;
  guint i;

  const gchar *grand_staff_brace_str = "𝄔";
  
  g_return_if_fail(AGS_IS_SHEET_EDIT(sheet_edit));
  g_return_if_fail(sheet_edit_script != NULL);

  settings = gtk_settings_get_default();

  font_name = NULL;
  
  g_object_get(settings,
	       "gtk-font-name", &font_name,
	       NULL);

  first_line_x0 = x0 + sheet_edit_script->margin_left;
  first_line_y0 = y0;

  current = sheet_edit_script;

  current_y0 = first_line_y0;
  
  cairo_set_line_width(cr, 0.866);

  do{
    current_y0 += current->margin_top + (current->staff_extends_top * (current->font_size / 2.0));
  
    for(i = 0; i < 5; i++){
      cairo_move_to(cr,
		    first_line_x0, current_y0 + ((gdouble) i * (current->font_size / 2.0)));
      cairo_line_to(cr,
		    width - (current->margin_left + current->margin_right), current_y0 + ((gdouble) i * (current->font_size / 2.0)));
      cairo_stroke(cr);
    }
    
    current_y0 += (4.0 * (current->font_size / 2.0) + current->margin_bottom + (current->staff_extends_bottom * (current->font_size / 2.0)));
  }while((current = current->companion_script) != NULL);

  if(sheet_edit_script->is_grand_staff){
    PangoLayout *layout;
    PangoFontDescription *desc;
 
    PangoRectangle ink_rect, logical_rect;

    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout,
			  grand_staff_brace_str,
			  -1);
    desc = pango_font_description_from_string(font_name);
    pango_font_description_set_size(desc,
				    (current_y0 - first_line_y0) * (12.5 / AGS_SHEET_EDIT_DEFAULT_GRAND_BRACE_STAFF_FONT_SIZE) * PANGO_SCALE);
    pango_layout_set_font_description(layout,
				      desc);
    pango_font_description_free(desc);    

    pango_layout_get_extents(layout,
			     &ink_rect,
			     &logical_rect);
    
    cairo_move_to(cr,
		  first_line_x0 + sheet_edit_script->grand_staff_brace_translate_x, first_line_y0 + sheet_edit_script->grand_staff_brace_translate_y);
    
    pango_cairo_show_layout(cr,
    			    layout);
  }
}

void
ags_sheet_edit_draw_clef(AgsSheetEdit *sheet_edit, cairo_t *cr,
			 AgsSheetEditScript *sheet_edit_script,
			 gdouble x0, gdouble y0,
			 gdouble clef_font_size)
{
  GtkSettings *settings;

  AgsSheetEditScript *current;

  gchar *font_name;
  
  gdouble first_line_x0, first_line_y0;
  gdouble current_y0;
  guint i;

  g_return_if_fail(AGS_IS_SHEET_EDIT(sheet_edit));
  g_return_if_fail(sheet_edit_script != NULL);

  settings = gtk_settings_get_default();

  font_name = NULL;
  
  g_object_get(settings,
	       "gtk-font-name", &font_name,
	       NULL);

  first_line_x0 = x0 + sheet_edit_script->margin_left;
  first_line_y0 = y0;

  current = sheet_edit_script;

  current_y0 = first_line_y0;
  
  cairo_set_line_width(cr, 1.0);

  do{
    PangoLayout *layout;
    PangoFontDescription *desc;

    PangoRectangle ink_rect, logical_rect;

    gchar *clef_str;

    const gchar *g_clef_str = "𝄞";
    const gchar *f_clef_str = "𝄢";
    
    current_y0 += current->margin_top + (current->staff_extends_top * (current->font_size / 2.0));

    clef_str = NULL;

    switch(current->clef){
    case AGS_SHEET_EDIT_G_CLEF:
      {
	clef_str = g_clef_str;
      }
      break;
    case AGS_SHEET_EDIT_F_CLEF:
      {
	clef_str = f_clef_str;
      }
      break;
    }
    
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout,
			  clef_str,
			  -1);
    desc = pango_font_description_from_string(font_name);
    pango_font_description_set_size(desc,
				    current->clef_font_size * PANGO_SCALE);
    pango_layout_set_font_description(layout,
				      desc);
    pango_font_description_free(desc);    

    pango_layout_get_extents(layout,
			     &ink_rect,
			     &logical_rect);

    cairo_move_to(cr,
		  first_line_x0 + current->clef_translate_x, current_y0 + ((gdouble) i * (current->font_size / 2.0)) + current->clef_translate_y);

    pango_cairo_show_layout(cr,
    			    layout);
    
    current_y0 += (4.0 * (current->font_size / 2.0) + current->margin_bottom + (current->staff_extends_bottom * (current->font_size / 2.0)));
  }while((current = current->companion_script) != NULL);
}

void
ags_sheet_edit_draw_sharp_flat(AgsSheetEdit *sheet_edit, cairo_t *cr,
			       AgsSheetEditScript *sheet_edit_script,
			       gdouble x0, gdouble y0,
			       gdouble sharp_flat_font_size)
{
  GtkSettings *settings;

  AgsSheetEditScript *current;

  gchar *font_name;
  
  gdouble first_line_x0, first_line_y0;
  gdouble current_y0;
  guint i;

  g_return_if_fail(AGS_IS_SHEET_EDIT(sheet_edit));
  g_return_if_fail(sheet_edit_script != NULL);

  settings = gtk_settings_get_default();

  font_name = NULL;
  
  g_object_get(settings,
	       "gtk-font-name", &font_name,
	       NULL);

  first_line_x0 = x0 + sheet_edit_script->margin_left;
  first_line_y0 = y0;

  current = sheet_edit_script;

  current_y0 = first_line_y0;
  
  cairo_set_line_width(cr, 1.0);

  do{
    PangoLayout *layout;
    PangoFontDescription *desc;

    PangoRectangle ink_rect, logical_rect;

    const gchar *sharp_str = "♯";
    const gchar *flat_str = "♭";
    
    current_y0 += current->margin_top + (current->staff_extends_top * (current->font_size / 2.0));
    
    if(current->clef == AGS_SHEET_EDIT_G_CLEF){
      if(!(current->is_minor)){
	layout = pango_cairo_create_layout(cr);
	pango_layout_set_text(layout,
			      sharp_str,
			      -1);
	desc = pango_font_description_from_string(font_name);
	pango_font_description_set_size(desc,
					current->sharp_flat_font_size * PANGO_SCALE);
	pango_layout_set_font_description(layout,
					  desc);
	pango_font_description_free(desc);    

	pango_layout_get_extents(layout,
				 &ink_rect,
				 &logical_rect);
	
	switch(current->sharp_flat){
	case 7:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (7.0 * (logical_rect.width / PANGO_SCALE * 0.75)) + current->sharp_translate_x,
			current_y0 + (0.5 * (current->font_size / 2.0)) + current->sharp_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	case 6:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (6.0 * (logical_rect.width / PANGO_SCALE * 0.75)) + current->sharp_translate_x,
			current_y0 + (-1.0 * (current->font_size / 2.0)) + current->sharp_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	case 5:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (5.0 * (logical_rect.width / PANGO_SCALE * 0.75)) + current->sharp_translate_x,
			current_y0 + (1.0 * (current->font_size / 2.0)) + current->sharp_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	case 4:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (4.0 * (logical_rect.width / PANGO_SCALE * 0.75)) + current->sharp_translate_x,
			current_y0 + (-0.5 * (current->font_size / 2.0)) + current->sharp_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	case 3:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (3.0 * (logical_rect.width / PANGO_SCALE * 0.75)) + current->sharp_translate_x,
			current_y0 + (-2.0 * (current->font_size / 2.0)) + current->sharp_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	case 2:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (2.0 * (logical_rect.width / PANGO_SCALE * 0.75)) + current->sharp_translate_x,
			current_y0 + current->sharp_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	case 1:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (logical_rect.width / PANGO_SCALE * 0.75) + current->sharp_translate_x,
			current_y0 + (-1.5 * (current->font_size / 2.0)) + current->sharp_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	}
      }else{
	layout = pango_cairo_create_layout(cr);
	pango_layout_set_text(layout,
			      flat_str,
			      -1);
	desc = pango_font_description_from_string(font_name);
	pango_font_description_set_size(desc,
					AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_FONT_SIZE * PANGO_SCALE);
	pango_layout_set_font_description(layout,
					  desc);
	pango_font_description_free(desc);    

	pango_layout_get_extents(layout,
				 &ink_rect,
				 &logical_rect);

	switch(current->sharp_flat){
	case 7:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (7.0 * (logical_rect.width / PANGO_SCALE * 0.9)) + current->flat_translate_x,
			current_y0 + (2.0 * (current->font_size / 2.0)) + current->flat_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	case 6:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (6.0 * (logical_rect.width / PANGO_SCALE * 0.9)) + current->flat_translate_x,
			current_y0 + current->flat_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	case 5:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (5.0 * (logical_rect.width / PANGO_SCALE * 0.9)) + current->flat_translate_x,
			current_y0 + (1.5 * (current->font_size / 2.0)) + current->flat_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	case 4:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (4.0 * (logical_rect.width / PANGO_SCALE * 0.9)) + current->flat_translate_x,
			current_y0 + (-0.5 * (current->font_size / 2.0)) + current->flat_translate_y);
	  
	  pango_cairo_show_layout(cr,
				  layout);
	case 3:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (3.0 * (logical_rect.width / PANGO_SCALE * 0.9)) + current->flat_translate_x,
			current_y0 + (1.0 * (current->font_size / 2.0)) + current->flat_translate_y);
	
	  pango_cairo_show_layout(cr,
				  layout);
	case 2:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (2.0 * (logical_rect.width / PANGO_SCALE * 0.9)) + current->flat_translate_x,
			current_y0 + (-1.0 * (current->font_size / 2.0)) + current->flat_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	case 1:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (logical_rect.width / PANGO_SCALE * 0.9) + current->flat_translate_x,
			current_y0 + (0.5 * (current->font_size / 2.0)) + current->flat_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	}
      }    
    }else if(current->clef == AGS_SHEET_EDIT_F_CLEF){
      if(!current->is_minor){
	layout = pango_cairo_create_layout(cr);
	pango_layout_set_text(layout,
			      sharp_str,
			      -1);
	desc = pango_font_description_from_string(font_name);
	pango_font_description_set_size(desc,
					current->sharp_flat_font_size * PANGO_SCALE);
	pango_layout_set_font_description(layout,
					  desc);
	pango_font_description_free(desc);    

	pango_layout_get_extents(layout,
				 &ink_rect,
				 &logical_rect);
	
	switch(current->sharp_flat){
	case 7:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (7.0 * (logical_rect.width / PANGO_SCALE * 0.75)) + current->sharp_translate_x,
			current_y0 + (1.5 * (current->font_size / 2.0)) + current->sharp_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	case 6:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (6.0 * (logical_rect.width / PANGO_SCALE * 0.75)) + current->sharp_translate_x,
			current_y0 + current->sharp_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	case 5:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (5.0 * (logical_rect.width / PANGO_SCALE * 0.75)) + current->sharp_translate_x,
			current_y0 + (2.0 * (current->font_size / 2.0)) + current->sharp_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	case 4:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (4.0 * (logical_rect.width / PANGO_SCALE * 0.75)) + current->sharp_translate_x,
			current_y0 + (0.5 * (current->font_size / 2.0)) + current->sharp_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	case 3:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (3.0 * (logical_rect.width / PANGO_SCALE * 0.75)) + current->sharp_translate_x,
			current_y0 + (-1.0 * (current->font_size / 2.0)) + current->sharp_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	case 2:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (2.0 * (logical_rect.width / PANGO_SCALE * 0.75)) + current->sharp_translate_x,
			current_y0 + (1.0 * (current->font_size / 2.0)) + current->sharp_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	case 1:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (logical_rect.width / PANGO_SCALE * 0.75) + current->sharp_translate_x,
			current_y0 + (-0.5 * (current->font_size / 2.0)) + current->sharp_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	}
      }else{
	layout = pango_cairo_create_layout(cr);
	pango_layout_set_text(layout,
			      flat_str,
			      -1);
	desc = pango_font_description_from_string(font_name);
	pango_font_description_set_size(desc,
					AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_FONT_SIZE * PANGO_SCALE);
	pango_layout_set_font_description(layout,
					  desc);
	pango_font_description_free(desc);    

	pango_layout_get_extents(layout,
				 &ink_rect,
				 &logical_rect);

	switch(current->sharp_flat){
	case 7:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (7.0 * (logical_rect.width / PANGO_SCALE * 0.9)) + current->flat_translate_x,
			current_y0 + (3.0 * (current->font_size / 2.0)) + current->flat_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	case 6:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (6.0 * (logical_rect.width / PANGO_SCALE * 0.9)) + current->flat_translate_x,
			current_y0 + (1.0 * (current->font_size / 2.0)) + current->flat_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	case 5:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (5.0 * (logical_rect.width / PANGO_SCALE * 0.9)) + current->flat_translate_x,
			current_y0 + (2.5 * (current->font_size / 2.0)) + current->flat_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	case 4:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (4.0 * (logical_rect.width / PANGO_SCALE * 0.9)) + current->flat_translate_x,
			current_y0 + (0.5 * (current->font_size / 2.0)) + current->flat_translate_y);
	  
	  pango_cairo_show_layout(cr,
				  layout);
	case 3:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (3.0 * (logical_rect.width / PANGO_SCALE * 0.9)) + current->flat_translate_x,
			current_y0 + (2.0 * (current->font_size / 2.0)) + current->flat_translate_y);
	
	  pango_cairo_show_layout(cr,
				  layout);
	case 2:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (2.0 * (logical_rect.width / PANGO_SCALE * 0.9)) + current->flat_translate_x,
			current_y0 + current->flat_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	case 1:
	  cairo_move_to(cr,
			first_line_x0 + AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 + (logical_rect.width / PANGO_SCALE * 0.9) + current->flat_translate_x,
			current_y0 + (1.5 * (current->font_size / 2.0)) + current->flat_translate_y);

	  pango_cairo_show_layout(cr,
				  layout);
	}  
      }
    }
    
    current_y0 += (4.0 * (current->font_size / 2.0) + current->margin_bottom + (current->staff_extends_bottom * (current->font_size / 2.0)));
  }while((current = current->companion_script) != NULL);
}

void
ags_sheet_edit_draw_note(AgsSheetEdit *sheet_edit, cairo_t *cr,
			 AgsSheetEditScript *sheet_edit_script,
			 AgsNotation *notation,
			 AgsNote *note,
			 gdouble x0, gdouble y0,
			 gdouble key_font_size)
{
  AgsNote *note_next;

  GtkSettings *settings;

  PangoLayout *layout;
  PangoFontDescription *desc;
	
  PangoRectangle ink_rect, logical_rect;
  
  GList *start_list, *list;

  gchar *font_name;
  gchar *note_str;

  gdouble page_width, page_height;
  guint key_center;
  guint note_x0, note_x1;
  guint note_y;
  guint note_next_x0, note_next_x1;
  guint note_next_pos_x0, note_next_pos_x1;

  const gchar* note_strv[] = {
    "𝅝",
    "𝅗𝅥",
    "𝅘𝅥",
    "𝅘𝅥𝅮",
    "𝅘𝅥𝅯",
    NULL,
  };

  g_return_if_fail(AGS_IS_SHEET_EDIT(sheet_edit));
  g_return_if_fail(sheet_edit_script != NULL);

  settings = gtk_settings_get_default();

  font_name = NULL;
  
  g_object_get(settings,
	       "gtk-font-name", &font_name,
	       NULL);

  start_list = ags_notation_get_note(notation);

  list = g_list_find(start_list,
		     note);
  
  note_next = NULL;

  if(list != NULL && list->next != NULL){
    note_next = AGS_NOTE(list->next->data);
  }
  
  note_x0 = 0;
  note_x1 = 0;

  note_y = 0;
	
  g_object_get(note,
	       "x0", &note_x0,
	       "x1", &note_x1,
	       "y", &note_y,
	       NULL);

  note_next_x0 = 0;
  note_next_x1 = 0;

  if(note_next != NULL){
    g_object_get(note_next,
		 "x0", &note_next_x0,
		 "x1", &note_next_x1,
		 NULL);
  }

  note_next_pos_x0 = 0;
  note_next_pos_x1 = 0;

  while(list != NULL){
    note_next_pos_x0 = 0;
    note_next_pos_x1 = 0;
		      
    g_object_get(list->data,
		 "x0", &note_next_pos_x0,
		 "x1", &note_next_pos_x1,
		 NULL);

    if(note_next_pos_x0 > note_x0){
      break;
    }
		      
    list = list->next;
  }
  
  if(list == NULL){
    note_next_pos_x0 = (16 * (guint) floor(note_x0 / 16.0) + 16);
    note_next_pos_x1 = (16 * (guint) floor(note_x0 / 16.0) + 16);
  }

  note_str = NULL;

  if(16 * (guint) floor(note_x0 / 16.0) + 16 >= note_next_pos_x0){
    if((gint) log2((gdouble) ((16 * (guint) floor(note_x0 / 16.0) + 16) - note_x0)) >= 0 &&
       (gint) log2((gdouble) ((16 * (guint) floor(note_x0 / 16.0) + 16) - note_x0)) < 5){
      note_str = note_strv[4 - (gint) log2((gdouble) (note_next_pos_x0 - note_x0))];
    }else{
      g_message("out of index");
    }
  }else{
    if((gint) log2((gdouble) ((16 * (guint) floor(note_x0 / 16.0) + 16) - note_x0)) >= 0 &&
       (gint) log2((gdouble) ((16 * (guint) floor(note_x0 / 16.0) + 16) - note_x0)) < 5){
      note_str = note_strv[4 - (gint) log2((gdouble) ((16 * (guint) floor(note_x0 / 16.0) + 16) - note_x0))];
    }else{
      g_message("out of index");
    }
  }

  if(sheet_edit->page_orientation == GTK_PAGE_ORIENTATION_PORTRAIT){
    page_width = AGS_SHEET_EDIT_DEFAULT_WIDTH;
    page_height = AGS_SHEET_EDIT_DEFAULT_HEIGHT;
  }else{
    page_width = AGS_SHEET_EDIT_DEFAULT_HEIGHT;
    page_height = AGS_SHEET_EDIT_DEFAULT_WIDTH;
  }

  key_center = 60;

  if(sheet_edit_script->clef == AGS_SHEET_EDIT_G_CLEF){
    key_center = 62;
  }
  
  if(note_str != NULL){
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout,
			  note_str,
			  -1);
    desc = pango_font_description_from_string(font_name);
    pango_font_description_set_size(desc,
				    AGS_SHEET_EDIT_DEFAULT_KEY_FONT_SIZE * PANGO_SCALE);
    pango_layout_set_font_description(layout,
				      desc);
    pango_font_description_free(desc);    

    pango_layout_get_extents(layout,
			     &ink_rect,
			     &logical_rect);

    //TODO:jk: check + 2 f clef
    if(note_y + 2 < key_center){
      cairo_save(cr);

      cairo_translate(cr,
		      (page_width / 2.0) + (logical_rect.width / PANGO_SCALE), (page_height / 2.0) + (logical_rect.height / PANGO_SCALE));
      cairo_rotate(cr,
		   2.0 * M_PI * 0.5);
      
      cairo_move_to(cr,
		    (page_width / 2.0) - (x0 + sheet_edit_script->reverse_key_translate_x),
		    (page_height / 2.0) + y0 + (((gdouble) note_y - (gdouble) key_center) * (sheet_edit_script->font_size / 4.0)) + sheet_edit_script->reverse_key_translate_y);

      pango_cairo_update_layout(cr,
				layout);
    }else{
      cairo_move_to(cr,
		    x0 + sheet_edit_script->key_translate_x,
		    y0 + (((gdouble) note_y - (gdouble) key_center) * (sheet_edit_script->font_size / 4.0)) + sheet_edit_script->key_translate_y);
    }
        
    pango_cairo_show_layout(cr,
			    layout);

    if(note_y + 2 < key_center){
      cairo_restore(cr);
    }
	    
    g_object_unref(layout);
  }

  g_list_free_full(start_list,
		   (GDestroyNotify) g_object_unref);
}

void
ags_sheet_edit_draw_rest(AgsSheetEdit *sheet_edit, cairo_t *cr,
			 AgsSheetEditScript *sheet_edit_script,
			 guint rest_x0, guint rest_x1,
			 gdouble x0, gdouble y0,
			 gdouble rest_font_size)
{
  gchar *rest_str;

  const gchar* rest_strv[] = {
    "𝄻",
    "𝄼",
    "𝄽",
    "𝄾",
    "𝄿",
    NULL,
  };

  g_return_if_fail(AGS_IS_SHEET_EDIT(sheet_edit));
  g_return_if_fail(sheet_edit_script != NULL);

  //TODO:JK: implement me
}

void
ags_sheet_edit_draw_notation(AgsSheetEdit *sheet_edit, cairo_t *cr,
			     AgsSheetEditScript *sheet_edit_script,
			     AgsNotation *notation,
			     guint notation_x0, guint notation_x1,
			     gdouble x0, gdouble y0,
			     gdouble key_font_size)
{
  AgsNote *note;

  GList *start_list, *list;

  guint note_x0;    
  guint prev_note_x0;
  guint i;
  guint offset;

  g_return_if_fail(AGS_IS_SHEET_EDIT(sheet_edit));
  g_return_if_fail(sheet_edit_script != NULL);

  start_list = ags_notation_get_note(notation);

  list = start_list;

  prev_note_x0 = 0;
  
  offset = 0;
  
  while(list != NULL){
    note = AGS_NOTE(list->data);
    
    note_x0 = 0;
	
    g_object_get(note,
		 "x0", &note_x0,
		 NULL);

    if(note_x0 > notation_x1){
      break;
    }

    if(note_x0 >= notation_x0){
      ags_sheet_edit_draw_note(sheet_edit, cr,
			       sheet_edit_script,
			       notation,
			       note,
			       x0 + sheet_edit_script->margin_left + AGS_SHEET_EDIT_DEFAULT_KEY_X0 + offset, y0,
			       key_font_size);

      if(start_list->prev == NULL ||
	 prev_note_x0 != note_x0){
	offset += AGS_SHEET_EDIT_DEFAULT_KEY_PADDING;
      }

      prev_note_x0 = note_x0;
    }

    list = list->next;
  }

  g_list_free_full(start_list,
		   (GDestroyNotify) g_object_unref);
}

void
ags_sheet_edit_draw(AgsSheetEdit *sheet_edit, cairo_t *cr)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *toolbar;
  AgsMachine *selected_machine;

  GtkStyleContext *style_context;

  AgsApplicationContext *application_context;

  GList *start_script, *script;
  GList *start_notation, *notation;
  
  gint width, height;
  gdouble page_width, page_height;
  gdouble y0;
  
  application_context = ags_application_context_get_instance();

  style_context = gtk_widget_get_style_context((GtkWidget *) sheet_edit);

  /*  */
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) sheet_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);
    
  toolbar = composite_editor->toolbar;

  selected_machine = composite_editor->selected_machine;

  width = gtk_widget_get_width(sheet_edit);
  height = gtk_widget_get_height(sheet_edit);

  if(sheet_edit->page_orientation == GTK_PAGE_ORIENTATION_PORTRAIT){
    page_width = AGS_SHEET_EDIT_DEFAULT_WIDTH;
    page_height = AGS_SHEET_EDIT_DEFAULT_HEIGHT;
  }else{
    page_width = AGS_SHEET_EDIT_DEFAULT_HEIGHT;
    page_height = AGS_SHEET_EDIT_DEFAULT_WIDTH;
  }

  gtk_render_background(style_context,
			cr,
			0.0, 0.0,
			(gdouble) width, (gdouble) height);

  notation = 
    start_notation = ags_audio_get_notation(selected_machine->audio);

  script =
    start_script = ags_sheet_edit_get_script(sheet_edit);

  y0 = 0.0;
  
  while(script != NULL){
    if(AGS_SHEET_EDIT_SCRIPT(script->data)->is_primary){
      AgsSheetEditScript *current;
      
      ags_sheet_edit_draw_staff(sheet_edit, cr,
				script->data,
				0.0, y0,
				page_width, page_height - y0,
				AGS_SHEET_EDIT_DEFAULT_FONT_SIZE);
      ags_sheet_edit_draw_clef(sheet_edit, cr,
			       script->data,
			       0.0, y0,
			       AGS_SHEET_EDIT_DEFAULT_CLEF_FONT_SIZE);
      ags_sheet_edit_draw_sharp_flat(sheet_edit, cr,
				     script->data,
				     0.0, y0,
				     AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_FONT_SIZE);

      if(notation != NULL){
	ags_sheet_edit_draw_notation(sheet_edit, cr,
				     script->data,
				     notation->data,
				     0, 16,
				     0.0, y0,
				     AGS_SHEET_EDIT_DEFAULT_KEY_FONT_SIZE);
      }
      
      current = script->data;

      do{
	y0 += (4.0 * (current->font_size / 2.0) + current->staff_extends_top * (current->font_size / 2.0) + current->staff_extends_bottom * (current->font_size / 2.0) + current->margin_top + current->margin_bottom);
      }while((current = current->companion_script) != NULL);
    }

    script = script->next;
  }

  g_list_free(start_script);
  
  g_list_free_full(start_notation,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_sheet_edit_new:
 *
 * Create a new #AgsSheetEdit.
 *
 * Returns: a new #AgsSheetEdit
 * 
 * Since: 3.0.0
 */
AgsSheetEdit*
ags_sheet_edit_new()
{
  AgsSheetEdit *sheet_edit;

  sheet_edit = (AgsSheetEdit *) g_object_new(AGS_TYPE_SHEET_EDIT,
					     NULL);

  return(sheet_edit);
}
