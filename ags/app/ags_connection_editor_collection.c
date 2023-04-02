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

#include <ags/app/ags_connection_editor_collection.h>
#include <ags/app/ags_connection_editor_collection_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_connection_editor.h>

#include <ags/i18n.h>

void ags_connection_editor_collection_class_init(AgsConnectionEditorCollectionClass *connection_editor_collection);
void ags_connection_editor_collection_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_connection_editor_collection_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_connection_editor_collection_init(AgsConnectionEditorCollection *connection_editor_collection);
void ags_connection_editor_collection_set_property(GObject *gobject,
						   guint prop_id,
						   const GValue *value,
						   GParamSpec *param_spec);
void ags_connection_editor_collection_get_property(GObject *gobject,
						   guint prop_id,
						   GValue *value,
						   GParamSpec *param_spec);

void ags_connection_editor_collection_connect(AgsConnectable *connectable);
void ags_connection_editor_collection_disconnect(AgsConnectable *connectable);

void ags_connection_editor_collection_set_update(AgsApplicable *applicable, gboolean update);
void ags_connection_editor_collection_apply(AgsApplicable *applicable);
void ags_connection_editor_collection_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_connection_editor_collection
 * @short_description: Edit audio related aspects
 * @title: AgsConnectionEditorCollection
 * @section_id:
 * @include: ags/app/ags_connection_editor_collection.h
 *
 * #AgsConnectionEditorCollection is a composite widget to edit all aspects of #AgsAudio.
 * It consists of multiple child editor_collections.
 */

enum{
  PROP_0,
  PROP_CHANNEL_TYPE,
};

GType
ags_connection_editor_collection_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_connection_editor_collection = 0;

    static const GTypeInfo ags_connection_editor_collection_info = {
      sizeof (AgsConnectionEditorCollectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_connection_editor_collection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsConnectionEditorCollection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_connection_editor_collection_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_connection_editor_collection_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_connection_editor_collection_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_connection_editor_collection = g_type_register_static(GTK_TYPE_BOX,
								   "AgsConnectionEditorCollection", &ags_connection_editor_collection_info,
								   0);

    g_type_add_interface_static(ags_type_connection_editor_collection,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_connection_editor_collection,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_connection_editor_collection);
  }

  return g_define_type_id__volatile;
}

void
ags_connection_editor_collection_class_init(AgsConnectionEditorCollectionClass *connection_editor_collection)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  /* GObjectClass */
  gobject = (GObjectClass *) connection_editor_collection;

  gobject->set_property = ags_connection_editor_collection_set_property;
  gobject->get_property = ags_connection_editor_collection_get_property;

  /* properties */
  /**
   * AgsConnectionEditorCollection:channel-type:
   *
   * The channel type.
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_gtype("channel-type",
				  i18n_pspec("channel type"),
				  i18n_pspec("The channel type"),
				  AGS_TYPE_CHANNEL,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL_TYPE,
				  param_spec);
}

void
ags_connection_editor_collection_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_connection_editor_collection_connect;
  connectable->disconnect = ags_connection_editor_collection_disconnect;
}

void
ags_connection_editor_collection_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_connection_editor_collection_set_update;
  applicable->apply = ags_connection_editor_collection_apply;
  applicable->reset = ags_connection_editor_collection_reset;
}

void
ags_connection_editor_collection_init(AgsConnectionEditorCollection *connection_editor_collection)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(connection_editor_collection),
				 GTK_ORIENTATION_VERTICAL);
  
  connection_editor_collection->connectable_flags = 0;

  connection_editor_collection->channel_type = G_TYPE_NONE;

  connection_editor_collection->parent_connection_editor = NULL;
  
  connection_editor_collection->enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));

  gtk_widget_set_halign((GtkWidget *) connection_editor_collection->enabled,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) connection_editor_collection->enabled,
			GTK_ALIGN_START);
  
  gtk_box_append((GtkBox *) connection_editor_collection,
		 (GtkWidget *) connection_editor_collection->enabled);

  connection_editor_collection->bulk = NULL;

  connection_editor_collection->bulk_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
								  AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append((GtkBox *) connection_editor_collection,
		 (GtkWidget *) connection_editor_collection->bulk_box);

  connection_editor_collection->add_bulk = (GtkButton *) gtk_button_new();
  gtk_button_set_icon_name(connection_editor_collection->add_bulk,
			   "list-add-symbolic");

  gtk_widget_set_halign((GtkWidget *) connection_editor_collection->add_bulk,
			GTK_ALIGN_END);
  gtk_widget_set_valign((GtkWidget *) connection_editor_collection->add_bulk,
			GTK_ALIGN_START);

  gtk_box_append((GtkBox *) connection_editor_collection,
		 (GtkWidget *) connection_editor_collection->add_bulk);
}

void
ags_connection_editor_collection_set_property(GObject *gobject,
					      guint prop_id,
					      const GValue *value,
					      GParamSpec *param_spec)
{
  AgsConnectionEditorCollection *connection_editor_collection;

  connection_editor_collection = AGS_CONNECTION_EDITOR_COLLECTION(gobject);

  switch(prop_id){
  case PROP_CHANNEL_TYPE:
    {
      connection_editor_collection->channel_type = g_value_get_gtype(value);  
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_connection_editor_collection_get_property(GObject *gobject,
					      guint prop_id,
					      GValue *value,
					      GParamSpec *param_spec)
{
  AgsConnectionEditorCollection *connection_editor_collection;

  connection_editor_collection = AGS_CONNECTION_EDITOR_COLLECTION(gobject);

  switch(prop_id){
  case PROP_CHANNEL_TYPE:
    {
      g_value_set_gtype(value, connection_editor_collection->channel_type);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_connection_editor_collection_connect(AgsConnectable *connectable)
{
  AgsConnectionEditorCollection *connection_editor_collection;

  GList *start_bulk, *bulk;

  connection_editor_collection = AGS_CONNECTION_EDITOR_COLLECTION(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (connection_editor_collection->connectable_flags)) != 0){
    return;
  }

  connection_editor_collection->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_signal_connect(connection_editor_collection->add_bulk, "clicked",
		   G_CALLBACK(ags_connection_editor_collection_add_bulk_callback), connection_editor_collection);
  
  bulk =
    start_bulk = ags_connection_editor_collection_get_bulk(connection_editor_collection);

  while(bulk != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(bulk->data));

    /* iterate */
    bulk = bulk->next;
  }

  g_list_free(start_bulk);
}

void
ags_connection_editor_collection_disconnect(AgsConnectable *connectable)
{
  AgsConnectionEditorCollection *connection_editor_collection;

  GList *start_bulk, *bulk;

  connection_editor_collection = AGS_CONNECTION_EDITOR_COLLECTION(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (connection_editor_collection->connectable_flags)) == 0){
    return;
  }
  
  connection_editor_collection->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_object_disconnect(connection_editor_collection->add_bulk,
		      "any_signal::clicked",
		      G_CALLBACK(ags_connection_editor_collection_add_bulk_callback),
		      connection_editor_collection,
		      NULL);

  bulk =
    start_bulk = ags_connection_editor_collection_get_bulk(connection_editor_collection);

  while(bulk != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(bulk->data));

    /* iterate */
    bulk = bulk->next;
  }

  g_list_free(start_bulk);
}

void
ags_connection_editor_collection_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsConnectionEditorCollection *connection_editor_collection;

  GList *start_bulk, *bulk;

  connection_editor_collection = AGS_CONNECTION_EDITOR_COLLECTION(applicable);

  bulk =
    start_bulk = ags_connection_editor_collection_get_bulk(connection_editor_collection);

  while(bulk != NULL){
    ags_applicable_set_update(AGS_APPLICABLE(bulk->data),
			      update);

    /* iterate */
    bulk = bulk->next;
  }

  g_list_free(start_bulk);
}

void
ags_connection_editor_collection_apply(AgsApplicable *applicable)
{
  AgsConnectionEditorCollection *connection_editor_collection;

  GList *start_bulk, *bulk;

  connection_editor_collection = AGS_CONNECTION_EDITOR_COLLECTION(applicable);

  if(!gtk_check_button_get_active(connection_editor_collection->enabled)){
    return;
  }

  bulk =
    start_bulk = ags_connection_editor_collection_get_bulk(connection_editor_collection);

  while(bulk != NULL){
    ags_applicable_apply(AGS_APPLICABLE(bulk->data));

    /* iterate */
    bulk = bulk->next;
  }

  g_list_free(start_bulk);
}

void
ags_connection_editor_collection_reset(AgsApplicable *applicable)
{
  AgsMachine *machine;
  AgsConnectionEditor *connection_editor;
  AgsConnectionEditorCollection *connection_editor_collection;

  GList *start_dialog_model, *dialog_model;
  GList *start_bulk, *bulk;

  connection_editor_collection = AGS_CONNECTION_EDITOR_COLLECTION(applicable);

  connection_editor = (AgsConnectionEditor *) gtk_widget_get_ancestor((GtkWidget *) connection_editor_collection,
								      AGS_TYPE_CONNECTION_EDITOR);

  if(connection_editor == NULL){
    return;
  }
  
  machine = connection_editor->machine;

  bulk =
    start_bulk = ags_connection_editor_collection_get_bulk(connection_editor_collection);

  while(bulk != NULL){
    ags_connection_editor_collection_remove_bulk(connection_editor_collection,
						 bulk->data);

    /* iterate */
    bulk = bulk->next;
  }

  g_list_free(start_bulk);

  dialog_model =
    start_dialog_model = ags_machine_get_dialog_model(machine);

  while(dialog_model != NULL){
    AgsConnectionEditorBulk *bulk;

    xmlNode *node;

    node = dialog_model->data;
    
    if(!g_strcmp0(node->name,
		  "ags-connection-editor-bulk")){
      xmlChar *direction;

      direction = xmlGetProp(node,
			     BAD_CAST "direction");

      if(g_type_is_a(connection_editor_collection->channel_type, AGS_TYPE_OUTPUT) &&
	 !g_strcmp0(direction, "output")){
	bulk = ags_connection_editor_bulk_new();

	if((AGS_CONNECTION_EDITOR_SHOW_SOUNDCARD_OUTPUT & (connection_editor->flags)) != 0){
	  gtk_widget_set_visible((GtkWidget *) bulk->output_grid,
				 TRUE);
	}

	if((AGS_CONNECTION_EDITOR_SHOW_SOUNDCARD_INPUT & (connection_editor->flags)) != 0){
	  gtk_widget_set_visible((GtkWidget *) bulk->input_grid,
				 TRUE);
	}

	ags_connection_editor_collection_add_bulk(connection_editor_collection,
						  bulk);

	ags_connectable_connect(AGS_CONNECTABLE(bulk));
      }else if(g_type_is_a(connection_editor_collection->channel_type, AGS_TYPE_INPUT) &&
	       !g_strcmp0(direction, "input")){
	bulk = ags_connection_editor_bulk_new();

	if((AGS_CONNECTION_EDITOR_SHOW_SOUNDCARD_OUTPUT & (connection_editor->flags)) != 0){
	  gtk_widget_set_visible((GtkWidget *) bulk->output_grid,
				 TRUE);
	}

	if((AGS_CONNECTION_EDITOR_SHOW_SOUNDCARD_INPUT & (connection_editor->flags)) != 0){
	  gtk_widget_set_visible((GtkWidget *) bulk->input_grid,
				 TRUE);
	}

	ags_connection_editor_collection_add_bulk(connection_editor_collection,
						  bulk);

	ags_connectable_connect(AGS_CONNECTABLE(bulk));
      }
    }
    
    /* iterate */
    dialog_model = dialog_model->next;
  }

  /* reset */
  bulk =
    start_bulk = ags_connection_editor_collection_get_bulk(connection_editor_collection);

  while(bulk != NULL){
    ags_applicable_reset(AGS_APPLICABLE(bulk->data));

    /* iterate */
    bulk = bulk->next;
  }

  g_list_free(start_bulk);
}

/**
 * ags_connection_editor_collection_get_bulk:
 * @connection_editor_collection: the #AgsConnectionEditorCollection
 * 
 * Get bulk.
 * 
 * Returns: the #GList-struct containig #AgsConnectionEditorBulk
 * 
 * Since: 4.0.0
 */
GList*
ags_connection_editor_collection_get_bulk(AgsConnectionEditorCollection *connection_editor_collection)
{
  g_return_val_if_fail(AGS_IS_CONNECTION_EDITOR_COLLECTION(connection_editor_collection), NULL);

  return(g_list_reverse(g_list_copy(connection_editor_collection->bulk)));
}

/**
 * ags_connection_editor_collection_add_bulk:
 * @connection_editor_collection: the #AgsConnectionEditorCollection
 * @bulk: the #AgsConnectionEditorBulk
 * 
 * Add @bulk to @connection_editor_collection.
 * 
 * Since: 4.0.0
 */
void
ags_connection_editor_collection_add_bulk(AgsConnectionEditorCollection *connection_editor_collection,
					  AgsConnectionEditorBulk *bulk)
{
  g_return_if_fail(AGS_IS_CONNECTION_EDITOR_COLLECTION(connection_editor_collection));
  g_return_if_fail(AGS_IS_CONNECTION_EDITOR_BULK(bulk));

  if(g_list_find(connection_editor_collection->bulk, bulk) == NULL){
    connection_editor_collection->bulk = g_list_prepend(connection_editor_collection->bulk,
							bulk);
    
    gtk_box_append(connection_editor_collection->bulk_box,
		   (GtkWidget *) bulk);
  }
}

/**
 * ags_connection_editor_collection_remove_bulk:
 * @connection_editor_collection: the #AgsConnectionEditorCollection
 * @bulk: the #AgsConnectionEditorBulk
 * 
 * Remove @bulk from @connection_editor_collection.
 * 
 * Since: 4.0.0
 */
void
ags_connection_editor_collection_remove_bulk(AgsConnectionEditorCollection *connection_editor_collection,
					     AgsConnectionEditorBulk *bulk)
{
  g_return_if_fail(AGS_IS_CONNECTION_EDITOR_COLLECTION(connection_editor_collection));
  g_return_if_fail(AGS_IS_CONNECTION_EDITOR_BULK(bulk));

  if(g_list_find(connection_editor_collection->bulk, bulk) != NULL){
    connection_editor_collection->bulk = g_list_remove(connection_editor_collection->bulk,
						       bulk);
    
    gtk_box_remove(connection_editor_collection->bulk_box,
		   (GtkWidget *) bulk);
  }
}

/**
 * ags_connection_editor_collection_new:
 * @channel_type: the channel type
 *
 * Creates an #AgsConnectionEditorCollection
 *
 * Returns: a new #AgsConnectionEditorCollection
 *
 * Since: 4.0.0
 */
AgsConnectionEditorCollection*
ags_connection_editor_collection_new(GType channel_type)
{
  AgsConnectionEditorCollection *connection_editor_collection;

  connection_editor_collection = (AgsConnectionEditorCollection *) g_object_new(AGS_TYPE_CONNECTION_EDITOR_COLLECTION,
										"channel-type", channel_type,
										NULL);

  return(connection_editor_collection);
}
