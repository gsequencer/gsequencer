#include "ags_property_collection_editor.h"
#include "ags_property_collection_editor_callbacks.h"

#include "../object/ags_connectable.h"

void ags_property_collection_editor_class_init(AgsPropertyCollectionEditorClass *property_collection_editor);
void ags_property_collection_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_property_collection_editor_init(AgsPropertyCollectionEditor *property_collection_editor);
void ags_property_collection_editor_connect(AgsConnectable *connectable);
void ags_property_collection_editor_disconnect(AgsConnectable *connectable);
void ags_property_collection_editor_destroy(GtkObject *object);
void ags_property_collection_editor_show(GtkWidget *widget);

GType
ags_property_collection_editor_get_type(void)
{
  static GType ags_type_property_collection_editor = 0;

  if(!ags_type_property_collection_editor){
    static const GTypeInfo ags_property_collection_editor_info = {
      sizeof (AgsPropertyCollectionEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_property_collection_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPropertyCollectionEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_property_collection_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_property_collection_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_property_collection_editor = g_type_register_static(AGS_TYPE_PROPERTY_EDITOR,
								 "AgsPropertyCollectionEditor\0", &ags_property_collection_editor_info,
								 0);
    
    g_type_add_interface_static(ags_type_property_collection_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_property_collection_editor);
}

void
ags_property_collection_editor_class_init(AgsPropertyCollectionEditorClass *property_collection_editor)
{
}

void
ags_property_collection_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_property_collection_editor_connect;
  connectable->disconnect = ags_property_collection_editor_disconnect;
}

void
ags_property_collection_editor_init(AgsPropertyCollectionEditor *property_collection_editor)
{
  GtkAlignment *alignment;
  
  property_collection_editor->child = (GtkVBox *) gtk_vbox_new(FALSE, 16);
  gtk_box_pack_start(GTK_BOX(property_collection_editor),
		     GTK_WIDGET(property_collection_editor->child),
		     TRUE, TRUE,
		     0);

  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.0,
						 0.0, 0.0);
  gtk_box_pack_start(GTK_BOX(property_collection_editor),
		     GTK_WIDGET(alignment),
		     FALSE, FALSE,
		     0);

  property_collection_editor->add_collection = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(property_collection_editor->add_collection));
}

void
ags_property_collection_editor_connect(AgsConnectable *connectable)
{
  AgsPropertyCollectionEditor *property_collection_editor;
  GList *pad_list;

  /* AgsPropertyCollectionEditor */
  property_collection_editor = AGS_PROPERTY_COLLECTION_EDITOR(connectable);

  g_signal_connect(G_OBJECT(property_collection_editor->add_collection), "clicked\0",
		   G_CALLBACK(ags_property_collection_editor_add_collection_callback), property_collection_editor);
}

void
ags_property_collection_editor_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_property_collection_editor_destroy(GtkObject *object)
{
  AgsPropertyCollectionEditor *property_collection_editor;

  property_collection_editor = (AgsPropertyCollectionEditor *) object;
}

void
ags_property_collection_editor_show(GtkWidget *widget)
{
  AgsPropertyCollectionEditor *property_collection_editor = (AgsPropertyCollectionEditor *) widget;
}

AgsPropertyCollectionEditor*
ags_property_collection_editor_new(GType child_type)
{
  AgsPropertyCollectionEditor *property_collection_editor;

  property_collection_editor = (AgsPropertyCollectionEditor *) g_object_new(AGS_TYPE_PROPERTY_COLLECTION_EDITOR,
									    NULL);

  property_collection_editor->child_type = child_type;
  
  return(property_collection_editor);
}
