#include "ags_channel_resize_editor.h"

#include "../object/ags_connectable.h"
#include "../object/ags_applicable.h"

void ags_channel_resize_editor_class_init(AgsChannelResizeEditorClass *channel_resize_editor);
void ags_channel_resize_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_channel_resize_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_channel_resize_editor_init(AgsChannelResizeEditor *channel_resize_editor);
void ags_channel_resize_editor_connect(AgsConnectable *connectable);
void ags_channel_resize_editor_disconnect(AgsConnectable *connectable);
void ags_channel_resize_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_channel_resize_editor_apply(AgsApplicable *applicable);
void ags_channel_resize_editor_reset(AgsApplicable *applicable);
void ags_channel_resize_editor_destroy(GtkObject *object);
void ags_channel_resize_editor_show(GtkWidget *widget);

GType
ags_channel_resize_editor_get_type(void)
{
  static GType ags_type_channel_resize_editor = 0;

  if(!ags_type_channel_resize_editor){
    static const GTypeInfo ags_channel_resize_editor_info = {
      sizeof (AgsChannelResizeEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_channel_resize_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsChannelResizeEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_channel_resize_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_channel_resize_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_channel_resize_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_channel_resize_editor = g_type_register_static(AGS_TYPE_PROPERTY_EDITOR,
							    "AgsChannelResizeEditor\0",
							    &ags_channel_resize_editor_info,
							    0);
    
    g_type_add_interface_static(ags_type_channel_resize_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_channel_resize_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }

  return(ags_type_channel_resize_editor);
}

void
ags_channel_resize_editor_class_init(AgsChannelResizeEditorClass *channel_resize_editor)
{
}

void
ags_channel_resize_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_channel_resize_editor_connect;
  connectable->disconnect = ags_channel_resize_editor_disconnect;
}

void
ags_channel_resize_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_channel_resize_editor_set_update;
  applicable->apply = ags_channel_resize_editor_apply;
  applicable->reset = ags_channel_resize_editor_reset;
}

void
ags_channel_resize_editor_init(AgsChannelResizeEditor *channel_resize_editor)
{
  GtkTable *table;
  GtkAlignment *alignment;
  GtkLabel *label;

  table = (GtkTable *) gtk_table_new(3, 2, FALSE);
  gtk_box_pack_start(GTK_BOX(channel_resize_editor),
		     GTK_WIDGET(table),
		     FALSE, FALSE,
		     0);

  /* audio channels */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(table,
		   GTK_WIDGET(alignment),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  label = (GtkLabel *) gtk_label_new("audio channels\0");
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(label));

  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(table,
		   GTK_WIDGET(alignment),
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  channel_resize_editor->audio_channels = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(channel_resize_editor->audio_channels));

  /* output pads */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(table,
		   GTK_WIDGET(alignment),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  label = (GtkLabel *) gtk_label_new("outputs\0");
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(label));

  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(table,
		   GTK_WIDGET(alignment),
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  channel_resize_editor->output_pads = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(channel_resize_editor->output_pads));

  /* input pads */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(table,
		   GTK_WIDGET(alignment),
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  label = (GtkLabel *) gtk_label_new("inputs\0");
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(label));

  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(table,
		   GTK_WIDGET(alignment),
		   1, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  channel_resize_editor->input_pads = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(channel_resize_editor->input_pads));
}

void
ags_channel_resize_editor_connect(AgsConnectable *connectable)
{
  AgsChannelResizeEditor *channel_resize_editor;
  GList *pad_list;

  /* AgsChannelResizeEditor */
  channel_resize_editor = AGS_CHANNEL_RESIZE_EDITOR(connectable);
}

void
ags_channel_resize_editor_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_channel_resize_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsChannelResizeEditor *channel_resize_editor;

  channel_resize_editor = AGS_CHANNEL_RESIZE_EDITOR(applicable);
}

void
ags_channel_resize_editor_apply(AgsApplicable *applicable)
{
  AgsChannelResizeEditor *channel_resize_editor;

  channel_resize_editor = AGS_CHANNEL_RESIZE_EDITOR(applicable);
}

void
ags_channel_resize_editor_reset(AgsApplicable *applicable)
{
  AgsChannelResizeEditor *channel_resize_editor;

  channel_resize_editor = AGS_CHANNEL_RESIZE_EDITOR(applicable);
}

void
ags_channel_resize_editor_destroy(GtkObject *object)
{
  AgsChannelResizeEditor *channel_resize_editor;

  channel_resize_editor = (AgsChannelResizeEditor *) object;
}

void
ags_channel_resize_editor_show(GtkWidget *widget)
{
  AgsChannelResizeEditor *channel_resize_editor = (AgsChannelResizeEditor *) widget;
}

AgsChannelResizeEditor*
ags_channel_resize_editor_new()
{
  AgsChannelResizeEditor *channel_resize_editor;

  channel_resize_editor = (AgsChannelResizeEditor *) g_object_new(AGS_TYPE_CHANNEL_RESIZE_EDITOR,
								  NULL);

  return(channel_resize_editor);
}
