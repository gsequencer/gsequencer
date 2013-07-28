#include <ags/X/ags_property_editor.h>
#include <ags/X/ags_property_editor_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

void ags_property_editor_class_init(AgsPropertyEditorClass *property_editor);
void ags_property_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_property_editor_init(AgsPropertyEditor *property_editor);
void ags_property_editor_connect(AgsConnectable *connectable);
void ags_property_editor_disconnect(AgsConnectable *connectable);
void ags_property_editor_destroy(GtkObject *object);
void ags_property_editor_show(GtkWidget *widget);

GType
ags_property_editor_get_type(void)
{
  static GType ags_type_property_editor = 0;

  if(!ags_type_property_editor){
    static const GTypeInfo ags_property_editor_info = {
      sizeof (AgsPropertyEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_property_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPropertyEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_property_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_property_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_property_editor = g_type_register_static(GTK_TYPE_VBOX,
						      "AgsPropertyEditor\0", &ags_property_editor_info,
						      0);
    
    g_type_add_interface_static(ags_type_property_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_property_editor);
}

void
ags_property_editor_class_init(AgsPropertyEditorClass *property_editor)
{
}

void
ags_property_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_property_editor_connect;
  connectable->disconnect = ags_property_editor_disconnect;
}

void
ags_property_editor_init(AgsPropertyEditor *property_editor)
{
  property_editor->flags = 0;

  property_editor->enabled = (GtkCheckButton *) gtk_check_button_new_with_label("enabled\0");
  gtk_box_pack_start(GTK_BOX(property_editor),
		     GTK_WIDGET(property_editor->enabled),
		     FALSE, FALSE,
		     0);
}

void
ags_property_editor_connect(AgsConnectable *connectable)
{
  AgsPropertyEditor *property_editor;
  GList *pad_list;

  /* AgsPropertyEditor */
  property_editor = AGS_PROPERTY_EDITOR(connectable);

  g_signal_connect_after(G_OBJECT(property_editor->enabled), "toggled\0",
			 G_CALLBACK(ags_property_editor_enable_callback), property_editor);
}

void
ags_property_editor_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_property_editor_destroy(GtkObject *object)
{
  AgsPropertyEditor *property_editor;

  property_editor = (AgsPropertyEditor *) object;
}

void
ags_property_editor_show(GtkWidget *widget)
{
  AgsPropertyEditor *property_editor = (AgsPropertyEditor *) widget;
}

AgsPropertyEditor*
ags_property_editor_new()
{
  AgsPropertyEditor *property_editor;

  property_editor = (AgsPropertyEditor *) g_object_new(AGS_TYPE_PROPERTY_EDITOR,
						       NULL);
  
  return(property_editor);
}
