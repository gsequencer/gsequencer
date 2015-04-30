/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/editor/ags_file_selection.h>
#include <ags/X/editor/ags_file_selection_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/X/ags_window.h>

#include <ags/X/editor/ags_inline_player.h>
#include <ags/X/editor/ags_sf2_chooser.h>

void ags_file_selection_class_init(AgsFileSelectionClass *file_selection);
void ags_file_selection_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_file_selection_init(AgsFileSelection *file_selection);
void ags_file_selection_connect(AgsConnectable *connectable);
void ags_file_selection_disconnect(AgsConnectable *connectable);
static void ags_file_selection_finalize(GObject *gobject);
void ags_file_selection_show(GtkWidget *widget);

void ags_file_selection_real_add_entry(AgsFileSelection *file_selection, GtkWidget *widget);
void ags_file_selection_real_remove_entry(AgsFileSelection *file_selection, GtkWidget *widget);
void ags_file_selection_real_completed(AgsFileSelection *file_selection);

enum{
  ADD_ENTRY,
  REMOVE_ENTRY,
  COMPLETED,
  LAST_SIGNAL,
};

static gpointer ags_file_selection_parent_class = NULL;

static guint file_selection_signals[LAST_SIGNAL];

GType
ags_file_selection_get_type(void)
{
  static GType ags_type_file_selection = 0;

  if(!ags_type_file_selection){
    static const GTypeInfo ags_file_selection_info = {
      sizeof (AgsFileSelectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_file_selection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFileSelection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_file_selection_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_file_selection_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_file_selection = g_type_register_static(GTK_TYPE_VBOX,
						     "AgsFileSelection\0", &ags_file_selection_info,
						     0);
    
    g_type_add_interface_static(ags_type_file_selection,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_file_selection);
}

void
ags_file_selection_class_init(AgsFileSelectionClass *file_selection)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_file_selection_parent_class = g_type_class_peek_parent(file_selection);

  /* GtkObjectClass */
  gobject = (GObjectClass *) file_selection;

  gobject->finalize = ags_file_selection_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) file_selection;

  widget->show = ags_file_selection_show;

  /* AgsFileSelectionClass */
  file_selection->add_entry = ags_file_selection_real_add_entry;
  file_selection->remove_entry = ags_file_selection_real_remove_entry;
  file_selection->completed = NULL;

  file_selection_signals[ADD_ENTRY] = 
    g_signal_new("add_entry\0",
		 G_TYPE_FROM_CLASS(file_selection),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileSelectionClass, add_entry),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  file_selection_signals[REMOVE_ENTRY] = 
    g_signal_new("remove_entry\0",
		 G_TYPE_FROM_CLASS(file_selection),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileSelectionClass, remove_entry),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  file_selection_signals[COMPLETED] = 
    g_signal_new("completed\0",
		 G_TYPE_FROM_CLASS(file_selection),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileSelectionClass, completed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_file_selection_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_file_selection_connect;
  connectable->disconnect = ags_file_selection_disconnect;
}

void
ags_file_selection_init(AgsFileSelection *file_selection)
{
  GtkHBox *hbox;
  GtkLabel *label;

  file_selection->flags = 0;

  file_selection->directory = NULL;

  file_selection->entry_count = 0;
  file_selection->entry = NULL;

  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_end(GTK_BOX(file_selection),
		   GTK_WIDGET(hbox),
		   FALSE, FALSE,
		   0);

  label = (GtkLabel *) gtk_label_new("Choosed: \0");
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  file_selection->chosed = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
						     "label\0", g_strdup_printf("%d\0", 0),
						     NULL);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(file_selection->chosed),
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new("/\0");
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  file_selection->selected = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
						       "label\0", g_strdup_printf("%d\0", 0),
						       NULL);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(file_selection->selected),
		     FALSE, FALSE,
		     0);
}

void
ags_file_selection_connect(AgsConnectable *connectable)
{
  AgsFileSelection *file_selection;

  /* AgsFileSelection */
  file_selection = AGS_FILE_SELECTION(connectable);
}

void
ags_file_selection_disconnect(AgsConnectable *connectable)
{
  /* implement me */
}

static void
ags_file_selection_finalize(GObject *gobject)
{
  AgsFileSelection *file_selection;

  file_selection = (AgsFileSelection *) gobject;

  /* free directory string */
  if(file_selection->directory != NULL)
    free(file_selection->directory);

  /* free entries */
  ags_list_free_and_free_link(file_selection->entry);

  /* call finalize of parent class */
  G_OBJECT_CLASS(ags_file_selection_parent_class)->finalize(gobject);
}

void
ags_file_selection_show(GtkWidget *widget)
{
  AgsFileSelection *file_selection;

  file_selection = (AgsFileSelection *) widget;

  GTK_WIDGET_CLASS(ags_file_selection_parent_class)->show(widget);
}

/**
 * ags_file_selection_set_entry:
 * @file_selection an #AgsFileSelection
 * @entry a #GList
 *
 * Add new entries to the #AgsFileSelection.
 */
void
ags_file_selection_set_entry(AgsFileSelection *file_selection, GList *entry_list)
{
  AgsFileSelectionEntry *entry;

  auto GtkTable* ags_file_selection_set_entry_new_entry();

  GtkTable* ags_file_selection_set_entry_new_entry(){
    GtkTable *table;
    GtkTable *range_table;
    GtkLabel *label;
    GtkButton *remove;
    AgsInlinePlayer *inline_player;

    table = (GtkTable *) gtk_table_new(3, 3, FALSE);

    /* the range buttons */
    range_table = (GtkTable *) gtk_table_new(2, 3, FALSE);
    gtk_table_attach(table,
		     GTK_WIDGET(range_table),
		     1, 3,
		     0, 1,
		     GTK_FILL, GTK_FILL,
		     0, 0);

    label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				      "label\0", "nth sample: \0",
				      "xalign\0", 0.0,
				      NULL);
    gtk_table_attach(range_table,
		     GTK_WIDGET(label),
		     0, 1,
		     0, 1,
		     GTK_FILL, GTK_FILL,
		     0, 0);

    label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				      "label\0", "nth channel: \0",
				      "xalign\0", 0.0,
				      NULL);
    gtk_table_attach(range_table,
		     GTK_WIDGET(label),
		     0, 1,
		     1, 2,
		     GTK_FILL, GTK_FILL,
		     0, 0);

    label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				      "label\0", "count: \0",
				      "xalign\0", 0.0,
				      NULL);
    gtk_table_attach(range_table,
		     GTK_WIDGET(label),
		     0, 1,
		     2, 3,
		     GTK_FILL, GTK_FILL,
		     0, 0);

    entry->nth_sample = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 65534.0, 1.0);
    gtk_table_attach(range_table,
		     GTK_WIDGET(entry->nth_sample),
		     1, 2,
		     0, 1,
		     GTK_FILL, GTK_FILL,
		     0, 0);

    entry->nth_channel = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 65534.0, 1.0);
    gtk_table_attach(range_table,
		     GTK_WIDGET(entry->nth_channel),
		     1, 2,
		     1, 2,
		     GTK_FILL, GTK_FILL,
		     0, 0);

    entry->count = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 65534.0, 1.0);
    gtk_table_attach(range_table,
		     GTK_WIDGET(entry->count),
		     1, 2,
		     2, 3,
		     GTK_FILL, GTK_FILL,
		     0, 0);

    /* the remove button */
    remove = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
    gtk_table_attach(table,
		     GTK_WIDGET(remove),
		     2, 3,
		     1, 2,
		     GTK_FILL, GTK_FILL,
		     0, 0);

    /* and it's callback */
    g_signal_connect(G_OBJECT(remove), "clicked\0",
		     G_CALLBACK(ags_file_selection_remove_callback), table);

    /* create an AgsInlinePlayer */
    inline_player = ags_inline_player_new();
    gtk_table_attach(table,
		     GTK_WIDGET(inline_player),
		     0, 1,
		     2, 3,
		     GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		     0, 0);    

    return(table);
  }

  while(entry_list != NULL){
    if(g_str_has_suffix(AGS_FILE_SELECTION_ENTRY(entry_list->data)->filename, ".sf2\0")){
      entry = AGS_FILE_SELECTION_ENTRY(entry_list->data);

      entry->table = ags_file_selection_set_entry_new_entry();

      entry->entry = (GtkWidget *) ags_sf2_chooser_new();
      gtk_table_attach(entry->table,
		       GTK_WIDGET(entry->entry),
		       0, 1,
		       0, 2,
		       GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		       0, 0);

      ags_sf2_chooser_open(AGS_SF2_CHOOSER(entry->entry),
			   entry->filename);

      ags_file_selection_add_entry(file_selection, (GtkWidget *) entry->table);

      ags_connectable_connect(AGS_CONNECTABLE(entry->entry));
    }

    entry_list = entry_list->next;
  }
}

void
ags_file_selection_add_entry(AgsFileSelection *file_selection, GtkWidget *widget)
{
  g_return_if_fail(AGS_IS_FILE_SELECTION(file_selection));

  g_object_ref((GObject *) file_selection);
  g_signal_emit(G_OBJECT(file_selection),
		file_selection_signals[ADD_ENTRY], 0,
		widget);
  g_object_unref((GObject *) file_selection);
}

void
ags_file_selection_real_add_entry(AgsFileSelection *file_selection, GtkWidget *widget)
{
  file_selection->entry_count++;
  gtk_label_set_text(file_selection->selected,
		     g_strdup_printf("%d\0", file_selection->entry_count));

  gtk_box_pack_start(GTK_BOX(file_selection),
		     widget,
		     FALSE,
		     FALSE,
		     0);
}

void
ags_file_selection_remove_entry(AgsFileSelection *file_selection, GtkWidget *widget)
{
  g_return_if_fail(AGS_IS_FILE_SELECTION(file_selection));

  g_object_ref((GObject *) file_selection);
  g_signal_emit(G_OBJECT(file_selection),
		file_selection_signals[REMOVE_ENTRY], 0,
		widget);
  g_object_unref((GObject *) file_selection);
}

void
ags_file_selection_real_remove_entry(AgsFileSelection *file_selection, GtkWidget *widget)
{
  file_selection->entry_count--;
  gtk_label_set_text(file_selection->selected,
		     g_strdup_printf("%d\0", file_selection->entry_count));

  gtk_widget_destroy(widget);
}

gboolean
ags_file_selection_contains_file(AgsFileSelection *file_selection, gchar *filename)
{
  GList *list;

  list = file_selection->entry;

  while(list != NULL){
    if(!g_strcmp0(list->data, filename))
      return(TRUE);
  }
  
  return(FALSE);
}

void
ags_file_selection_completed(AgsFileSelection *file_selection)
{
  g_return_if_fail(AGS_IS_FILE_SELECTION(file_selection));

  g_object_ref((GObject *) file_selection);
  g_signal_emit(G_OBJECT(file_selection),
		file_selection_signals[COMPLETED], 0);
  g_object_unref((GObject *) file_selection);
}

AgsFileSelectionEntry*
ags_file_selection_entry_alloc()
{
  AgsFileSelectionEntry *entry;
  
  entry = (AgsFileSelectionEntry *) malloc(sizeof(AgsFileSelectionEntry));
 
  entry->table = NULL;
  entry->entry = NULL;
 
  entry->chosed = FALSE;
  entry->filename = NULL;

  entry->nth_sample = NULL;
  entry->nth_channel = NULL;
  entry->count = NULL;

  return(entry);
}

AgsFileSelection*
ags_file_selection_new()
{
  AgsFileSelection *file_selection;

  file_selection = (AgsFileSelection *) g_object_new(AGS_TYPE_FILE_SELECTION,
						     NULL);

  return(file_selection);
}
