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

#include <ags/app/machine/ags_desk_pad.h>
#include <ags/app/machine/ags_desk_pad_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <ags/app/machine/ags_desk.h>

#include <math.h>

#include <ags/i18n.h>

void ags_desk_pad_class_init(AgsDeskPadClass *desk_pad);
void ags_desk_pad_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_desk_pad_init(AgsDeskPad *desk_pad);
static void ags_desk_pad_finalize(GObject *gobject);

void ags_desk_pad_connect(AgsConnectable *connectable);
void ags_desk_pad_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_desk_pad
 * @short_description: desk sequencer input pad
 * @title: AgsDeskPad
 * @section_id:
 * @include: ags/app/machine/ags_desk_pad.h
 *
 * The #AgsDeskPad is a composite widget to act as desk sequencer input pad.
 */

static gpointer ags_desk_pad_parent_class = NULL;
static AgsConnectableInterface *ags_desk_pad_parent_connectable_interface;

GType
ags_desk_pad_get_type()
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_desk_pad = 0;

    static const GTypeInfo ags_desk_pad_info = {
      sizeof(AgsDeskPadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_desk_pad_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDeskPad),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_desk_pad_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_desk_pad_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_desk_pad = g_type_register_static(GTK_TYPE_BOX,
					       "AgsDeskPad", &ags_desk_pad_info,
					       0);

    g_type_add_interface_static(ags_type_desk_pad,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_desk_pad);
  }

  return g_define_type_id__volatile;
}

void
ags_desk_pad_class_init(AgsDeskPadClass *desk_pad)
{
  GObjectClass *gobject;
  
  ags_desk_pad_parent_class = g_type_class_peek_parent(desk_pad);

  /*  */
  gobject = (GObjectClass *) desk_pad;

  gobject->finalize = ags_desk_pad_finalize;
}

void
ags_desk_pad_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_desk_pad_connect;
  connectable->disconnect = ags_desk_pad_disconnect;
}

void
ags_desk_pad_init(AgsDeskPad *desk_pad)
{
  GtkBox *hbox;
  GtkBox *control_vbox;
  GtkBox *control_hbox;
  GtkBox *playlist_vbox;
  GtkBox *playlist_button_hbox;

  GtkTreeViewColumn *playlist_filename_column;
  GtkTreeViewColumn *playlist_selected_column;

  GtkCellRenderer *cell_renderer;

  GtkListStore *playlist_model;

  AgsApplicationContext *application_context;

  gdouble gui_scale_factor;      

  gtk_orientable_set_orientation(GTK_ORIENTABLE(desk_pad),
				 GTK_ORIENTATION_VERTICAL);
  
  application_context = ags_application_context_get_instance();

  desk_pad->flags = 0;
  
  desk_pad->name = NULL;
  desk_pad->xml_type = "ags-desk-input-pad";

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  /* position */
  desk_pad->position_time = (GtkLabel *) gtk_label_new("00:00.000");
  gtk_box_append((GtkBox *) desk_pad,
		 (GtkWidget *) desk_pad->position_time);
  
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_append((GtkBox *) desk_pad,
		 (GtkWidget *) hbox);
  
  desk_pad->position = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,
							     0.0, 1.0, 0.001);
  gtk_box_append(hbox,
		 (GtkWidget *) desk_pad->position);
  
  /* filename */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_append((GtkBox *) desk_pad,
		 (GtkWidget *) hbox);

  /* play */
  desk_pad->play = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
						    "icon-name", "media-playback-start",
						    NULL);
  gtk_box_append(hbox,
		 (GtkWidget *) desk_pad->play);

  desk_pad->filename = (GtkEntry *) gtk_entry_new();
  gtk_box_append(hbox,
		 (GtkWidget *) desk_pad->filename);
  
  desk_pad->grab_filename = (GtkButton *) gtk_button_new_with_label(i18n("grab"));
  gtk_box_append(hbox,
		 (GtkWidget *) desk_pad->grab_filename);

  /* controls/playlist */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_append((GtkBox *) desk_pad,
		 (GtkWidget *) hbox);
  
  /* controls */
  control_vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
					0);
  gtk_box_append(hbox,
		 (GtkWidget *) control_vbox);

  control_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					0);
  gtk_box_append(control_vbox,
		 (GtkWidget *) control_hbox);

  /* indicator */
  desk_pad->indicator = (AgsIndicator *) ags_indicator_new(GTK_ORIENTATION_VERTICAL,
							   gui_scale_factor * AGS_INDICATOR_DEFAULT_SEGMENT_WIDTH,
							   gui_scale_factor * AGS_INDICATOR_DEFAULT_SEGMENT_HEIGHT);

  g_object_set(desk_pad->indicator,
	       "segment-padding", (guint) (gui_scale_factor * AGS_INDICATOR_DEFAULT_SEGMENT_PADDING),
	       NULL);

  gtk_box_append(control_hbox,
		 (GtkWidget *) desk_pad->indicator);

  /* volume */
  desk_pad->volume = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
							   0.0, 2.0, 0.1);
  gtk_range_set_inverted((GtkRange *) desk_pad->volume,
			 TRUE);
  gtk_range_set_value((GtkRange *) desk_pad->volume,
		      1.0);
  gtk_widget_set_size_request((GtkWidget *) desk_pad->volume,
			      gui_scale_factor * 16, gui_scale_factor * 100);
  gtk_box_append(control_hbox,
		 (GtkWidget *) desk_pad->volume);

  /* playlist */
  playlist_vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
					 0);
  gtk_box_append(hbox,
		 (GtkWidget *) playlist_vbox);

  playlist_model = gtk_list_store_new(2,
				      G_TYPE_STRING,
				      G_TYPE_BOOLEAN);
  
  desk_pad->playlist = (GtkTreeView *) gtk_tree_view_new_with_model(GTK_TREE_MODEL(playlist_model));
  gtk_widget_set_size_request((GtkWidget *) desk_pad->playlist,
			      240, 240);
  gtk_box_append(playlist_vbox,
		 (GtkWidget *) desk_pad->playlist);
  
  cell_renderer = gtk_cell_renderer_text_new();

  playlist_filename_column = gtk_tree_view_column_new_with_attributes(i18n("filename"),
								      cell_renderer,
								      "text", 0,
								      NULL);
  gtk_tree_view_append_column(desk_pad->playlist,
			      playlist_filename_column);

  cell_renderer = gtk_cell_renderer_toggle_new();

  playlist_selected_column = gtk_tree_view_column_new_with_attributes(i18n("selected"),
								      cell_renderer,
								      "active", 1,
								      NULL);
  gtk_tree_view_append_column(desk_pad->playlist,
			      playlist_selected_column);
  
  playlist_button_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
						0);
  gtk_box_append(playlist_vbox,
		 (GtkWidget *) playlist_button_hbox);

  desk_pad->move_up = (GtkButton *) gtk_button_new_from_icon_name("go-up");
  gtk_box_append(playlist_button_hbox,
		 (GtkWidget *) desk_pad->move_up);

  desk_pad->move_down = (GtkButton *) gtk_button_new_from_icon_name("go-down");
  gtk_box_append(playlist_button_hbox,
		 (GtkWidget *) desk_pad->move_down);

  desk_pad->add = (GtkButton *) gtk_button_new_from_icon_name("list-add");
  gtk_box_append(playlist_button_hbox,
		 (GtkWidget *) desk_pad->add);
  
  desk_pad->remove = (GtkButton *) gtk_button_new_from_icon_name("list-remove");
  gtk_box_append(playlist_button_hbox,
		 (GtkWidget *) desk_pad->remove);
}

static void
ags_desk_pad_finalize(GObject *gobject)
{
  AgsDeskPad *desk_pad;

  desk_pad = AGS_DESK_PAD(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_desk_pad_parent_class)->finalize(gobject);
}

void
ags_desk_pad_connect(AgsConnectable *connectable)
{
  AgsDeskPad *desk_pad;

  if((AGS_DESK_PAD_CONNECTED & (AGS_DESK_PAD(connectable)->flags)) != 0){
    return;
  }

  desk_pad = AGS_DESK_PAD(connectable);

  desk_pad->flags |= AGS_DESK_PAD_CONNECTED;

  g_signal_connect(desk_pad->play, "clicked",
		   G_CALLBACK(ags_desk_pad_play_callback), desk_pad);

  g_signal_connect(desk_pad->grab_filename, "clicked",
		   G_CALLBACK(ags_desk_pad_grab_filename_callback), desk_pad);

  g_signal_connect(desk_pad->volume, "value-changed",
		   G_CALLBACK(ags_desk_pad_volume_callback), desk_pad);

  g_signal_connect(desk_pad->move_up, "clicked",
		   G_CALLBACK(ags_desk_pad_move_up_callback), desk_pad);

  g_signal_connect(desk_pad->move_down, "clicked",
		   G_CALLBACK(ags_desk_pad_move_down_callback), desk_pad);

  g_signal_connect(desk_pad->add, "clicked",
		   G_CALLBACK(ags_desk_pad_add_callback), desk_pad);

  g_signal_connect(desk_pad->remove, "clicked",
		   G_CALLBACK(ags_desk_pad_remove_callback), desk_pad);
}

void
ags_desk_pad_disconnect(AgsConnectable *connectable)
{
  AgsDeskPad *desk_pad;

  if((AGS_DESK_PAD_CONNECTED & (AGS_DESK_PAD(connectable)->flags)) == 0){
    return;
  }

  desk_pad = AGS_DESK_PAD(connectable);

  desk_pad->flags &= (~AGS_DESK_PAD_CONNECTED);

  g_object_disconnect(desk_pad->play,
		      "any_signal::clicked",
		      G_CALLBACK(ags_desk_pad_play_callback),
		      desk_pad,
		      NULL);

  g_object_disconnect(desk_pad->grab_filename,
		      "any_signal::clicked",
		      G_CALLBACK(ags_desk_pad_grab_filename_callback),
		      desk_pad,
		      NULL);

  g_object_disconnect(desk_pad->volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_desk_pad_volume_callback),
		      desk_pad,
		      NULL);

  g_object_disconnect(desk_pad->move_up,
		      "any_signal::clicked",
		      G_CALLBACK(ags_desk_pad_move_up_callback),
		      desk_pad,
		      NULL);

  g_object_disconnect(desk_pad->move_down,
		      "any_signal::clicked",
		      G_CALLBACK(ags_desk_pad_move_down_callback),
		      desk_pad,
		      NULL);

  g_object_disconnect(desk_pad->add,
		      "any_signal::clicked",
		      G_CALLBACK(ags_desk_pad_add_callback),
		      desk_pad,
		      NULL);

  g_object_disconnect(desk_pad->remove,
		      "any_signal::clicked",
		      G_CALLBACK(ags_desk_pad_remove_callback),
		      desk_pad,
		      NULL);
}

/**
 * ags_desk_pad_add_filename:
 * @desk_pad: the #AgsDeskPad
 * @filename: the filename
 *
 * Add @filename to @desk_pad.
 *
 * Since: 3.7.0
 */
void
ags_desk_pad_add_filename(AgsDeskPad *desk_pad,
			  gchar *filename)
{
  GtkTreeModel *playlist_model;
  
  GtkTreeIter iter;
  
  if(!AGS_IS_DESK_PAD(desk_pad) ||
     filename == NULL){
    return;
  }

  playlist_model = gtk_tree_view_get_model(desk_pad->playlist);

  gtk_list_store_append(GTK_LIST_STORE(playlist_model), &iter);
  gtk_list_store_set(GTK_LIST_STORE(playlist_model),
		     &iter,
		     0, filename,
		     1, FALSE,
		     -1);
}

/**
 * ags_desk_pad_remove_nth_filename:
 * @desk_pad: the #AgsDeskPad
 * @nth_filename: the nth filename
 *
 * Remove @nth_filename from @desk_pad.
 *
 * Since: 3.7.0
 */
void
ags_desk_pad_remove_nth_filename(AgsDeskPad *desk_pad,
				 guint nth_filename)
{
  if(!AGS_IS_DESK_PAD(desk_pad)){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_desk_pad_new:
 *
 * Creates an #AgsDeskPad
 *
 * Returns: a new #AgsDeskPad
 *
 * Since: 3.0.0
 */
AgsDeskPad*
ags_desk_pad_new()
{
  AgsDeskPad *desk_pad;

  desk_pad = (AgsDeskPad *) g_object_new(AGS_TYPE_DESK_PAD,
					 NULL);

  return(desk_pad);
}
