#include "ags_channel_link_collection_editor.h"
#include "ags_channel_link_collection_editor_callbacks.h"

#include "../object/ags_connectable.h"
#include "../object/ags_applicable.h"

void ags_channel_link_collection_editor_class_init(AgsChannelLinkCollectionEditorClass *channel_link_collection_editor);
void ags_channel_link_collection_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_channel_link_collection_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_channel_link_collection_editor_init(AgsChannelLinkCollectionEditor *channel_link_collection_editor);
void ags_channel_link_collection_editor_connect(AgsConnectable *connectable);
void ags_channel_link_collection_editor_disconnect(AgsConnectable *connectable);
void ags_channel_link_collection_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_channel_link_collection_editor_apply(AgsApplicable *applicable);
void ags_channel_link_collection_editor_reset(AgsApplicable *applicable);
void ags_channel_link_collection_editor_destroy(GtkObject *object);
void ags_channel_link_collection_editor_show(GtkWidget *widget);

GType
ags_channel_link_collection_editor_get_type(void)
{
  static GType ags_type_channel_link_collection_editor = 0;

  if(!ags_type_channel_link_collection_editor){
    static const GTypeInfo ags_channel_link_collection_editor_info = {
      sizeof (AgsChannelLinkCollectionEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_channel_link_collection_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsChannelLinkCollectionEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_channel_link_collection_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_channel_link_collection_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_channel_link_collection_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_channel_link_collection_editor = g_type_register_static(GTK_TYPE_TABLE,
								     "AgsChannelLinkCollectionEditor\0",
								     &ags_channel_link_collection_editor_info,
								     0);
    
    g_type_add_interface_static(ags_type_channel_link_collection_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_channel_link_collection_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }

  return(ags_type_channel_link_collection_editor);
}

void
ags_channel_link_collection_editor_class_init(AgsChannelLinkCollectionEditorClass *channel_link_collection_editor)
{
}

void
ags_channel_link_collection_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_channel_link_collection_editor_connect;
  connectable->disconnect = ags_channel_link_collection_editor_disconnect;
}

void
ags_channel_link_collection_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_channel_link_collection_editor_set_update;
  applicable->apply = ags_channel_link_collection_editor_apply;
  applicable->reset = ags_channel_link_collection_editor_reset;
}

void
ags_channel_link_collection_editor_init(AgsChannelLinkCollectionEditor *channel_link_collection_editor)
{
  GtkAlignment *alignment;
  GtkLabel *label;
  GtkCellRenderer *cell_renderer;
  GtkListStore *model;
  GtkTreeIter iter;

  g_signal_connect_after(GTK_WIDGET(channel_link_collection_editor), "parent_set\0",
			 G_CALLBACK(ags_channel_link_collection_editor_parent_set_callback), channel_link_collection_editor);

  gtk_table_resize(GTK_TABLE(channel_link_collection_editor),
		   4, 2);
  gtk_table_set_row_spacings(GTK_TABLE(channel_link_collection_editor),
			     4);
  gtk_table_set_col_spacings(GTK_TABLE(channel_link_collection_editor),
			     2);

  /* link */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(channel_link_collection_editor),
		   GTK_WIDGET(alignment),
		   0, 1,
		   0, 1,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  label = (GtkLabel *) gtk_label_new("link\0");
  gtk_container_add(GTK_CONTAINER(alignment), GTK_WIDGET(label));


  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(channel_link_collection_editor),
		   GTK_WIDGET(alignment),
		   1, 2,
		   0, 1,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  channel_link_collection_editor->link = (GtkComboBox *) gtk_combo_box_new();
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(channel_link_collection_editor->link));
		    
  model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
		    
  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, "NULL\0",
		     1, NULL,
		     -1);
  
  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(channel_link_collection_editor->link),
			     cell_renderer,
			     FALSE); 
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(channel_link_collection_editor->link),
				 cell_renderer,
				 "text", 0,
				 NULL);

  gtk_combo_box_set_model(channel_link_collection_editor->link,
			  GTK_TREE_MODEL(model));

  /* first line */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(channel_link_collection_editor),
		   GTK_WIDGET(alignment),
		   0, 1,
		   1, 2,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  label = (GtkLabel *) gtk_label_new("first line\0");
  gtk_container_add(GTK_CONTAINER(alignment), GTK_WIDGET(label));

  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(channel_link_collection_editor),
		   GTK_WIDGET(alignment),
		   1, 2,
		   1, 2,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);
  
  channel_link_collection_editor->first_line = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(channel_link_collection_editor->first_line));

  /* first link line */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(channel_link_collection_editor),
		   GTK_WIDGET(alignment),
		   0, 1,
		   2, 3,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,

		   0, 0);

  label = (GtkLabel *) gtk_label_new("first link line\0");
  gtk_container_add(GTK_CONTAINER(alignment), GTK_WIDGET(label));

  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(channel_link_collection_editor),
		   GTK_WIDGET(alignment),
		   1, 2,
		   2, 3,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);
  
  channel_link_collection_editor->first_link = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(channel_link_collection_editor->first_link));

  /* count */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(channel_link_collection_editor),
		   GTK_WIDGET(alignment),
		   0, 1,
		   3, 4,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  label = (GtkLabel *) gtk_label_new("count\0");
  gtk_container_add(GTK_CONTAINER(alignment), GTK_WIDGET(label));

  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(channel_link_collection_editor),
		   GTK_WIDGET(alignment),
		   1, 2,
		   3, 4,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);
  
  channel_link_collection_editor->count = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(channel_link_collection_editor->count));
}

void
ags_channel_link_collection_editor_connect(AgsConnectable *connectable)
{
  AgsChannelLinkCollectionEditor *channel_link_collection_editor;
  GList *pad_list;

  /* AgsChannelLinkCollectionEditor */
  channel_link_collection_editor = AGS_CHANNEL_LINK_COLLECTION_EDITOR(connectable);
}

void
ags_channel_link_collection_editor_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_channel_link_collection_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsChannelLinkCollectionEditor *channel_link_collection_editor;

  channel_link_collection_editor = AGS_CHANNEL_LINK_COLLECTION_EDITOR(applicable);

}

void
ags_channel_link_collection_editor_apply(AgsApplicable *applicable)
{
  AgsChannelLinkCollectionEditor *channel_link_collection_editor;

  channel_link_collection_editor = AGS_CHANNEL_LINK_COLLECTION_EDITOR(applicable);
}

void
ags_channel_link_collection_editor_reset(AgsApplicable *applicable)
{
  AgsChannelLinkCollectionEditor *channel_link_collection_editor;

  channel_link_collection_editor = AGS_CHANNEL_LINK_COLLECTION_EDITOR(applicable);

}

void
ags_channel_link_collection_editor_destroy(GtkObject *object)
{
  AgsChannelLinkCollectionEditor *channel_link_collection_editor;

  channel_link_collection_editor = (AgsChannelLinkCollectionEditor *) object;
}

void
ags_channel_link_collection_editor_show(GtkWidget *widget)
{
  AgsChannelLinkCollectionEditor *channel_link_collection_editor = (AgsChannelLinkCollectionEditor *) widget;
}

AgsChannelLinkCollectionEditor*
ags_channel_link_collection_editor_new()
{
  AgsChannelLinkCollectionEditor *channel_link_collection_editor;

  channel_link_collection_editor = (AgsChannelLinkCollectionEditor *) g_object_new(AGS_TYPE_CHANNEL_LINK_COLLECTION_EDITOR,
										   NULL);
  
  return(channel_link_collection_editor);
}
