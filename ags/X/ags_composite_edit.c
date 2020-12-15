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

#include <ags/X/ags_composite_edit.h>
#include <ags/X/ags_composite_edit_callbacks.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_composite_edit_class_init(AgsCompositeEditClass *composite_edit);
void ags_composite_edit_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_composite_edit_init(AgsCompositeEdit *composite_edit);

AgsUUID* ags_composite_edit_get_uuid(AgsConnectable *connectable);
gboolean ags_composite_edit_has_resource(AgsConnectable *connectable);
gboolean ags_composite_edit_is_ready(AgsConnectable *connectable);
void ags_composite_edit_add_to_registry(AgsConnectable *connectable);
void ags_composite_edit_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_composite_edit_list_resource(AgsConnectable *connectable);
xmlNode* ags_composite_edit_xml_compose(AgsConnectable *connectable);
void ags_composite_edit_xml_parse(AgsConnectable *connectable,
				  xmlNode *node);
gboolean ags_composite_edit_is_connected(AgsConnectable *connectable);
void ags_composite_edit_connect(AgsConnectable *connectable);
void ags_composite_edit_disconnect(AgsConnectable *connectable);

static gpointer ags_composite_edit_parent_class = NULL;

/**
 * SECTION:ags_composite_edit
 * @short_description: composite edit
 * @title: AgsCompositeEdit
 * @section_id:
 * @include: ags/X/ags_composite_edit.h
 *
 * The #AgsCompositeEdit is a composite abstraction of edit implementation.
 */

GType
ags_composite_edit_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_composite_edit = 0;

    static const GTypeInfo ags_composite_edit_info = {
      sizeof (AgsCompositeEditClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_composite_edit_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCompositeEdit),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_composite_edit_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_composite_edit_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_composite_edit = g_type_register_static(GTK_TYPE_BOX,
						     "AgsCompositeEdit", &ags_composite_edit_info,
						     0);
    
    g_type_add_interface_static(ags_type_composite_edit,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_composite_edit);
  }

  return g_define_type_id__volatile;
}

void
ags_composite_edit_class_init(AgsCompositeEditClass *composite_edit)
{
  ags_composite_edit_parent_class = g_type_class_peek_parent(composite_edit);
}

void
ags_composite_edit_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_composite_edit_get_uuid;
  connectable->has_resource = ags_composite_edit_has_resource;

  connectable->is_ready = ags_composite_edit_is_ready;
  connectable->add_to_registry = ags_composite_edit_add_to_registry;
  connectable->remove_from_registry = ags_composite_edit_remove_from_registry;

  connectable->list_resource = ags_composite_edit_list_resource;
  connectable->xml_compose = ags_composite_edit_xml_compose;
  connectable->xml_parse = ags_composite_edit_xml_parse;

  connectable->is_connected = ags_composite_edit_is_connected;
  connectable->connect = ags_composite_edit_connect;
  connectable->disconnect = ags_composite_edit_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_composite_edit_init(AgsCompositeEdit *composite_edit)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(composite_edit),
				 GTK_ORIENTATION_VERTICAL);
  
  composite_edit->flags = 0;

  composite_edit->version = NULL;
  composite_edit->build_id = NULL;

  /* uuid */
  composite_edit->uuid = ags_uuid_alloc();
  ags_uuid_generate(composite_edit->uuid);

  /* widgets */
  composite_edit->edit_paned = (GtkPaned *) gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start((GtkBox *) composite_edit,
		     (GtkWidget *) composite_edit->edit_paned,
		     FALSE, FALSE,
		     0);
  
  composite_edit->edit_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
						    0);
  gtk_paned_add1(composite_edit->edit_paned,
		 composite_edit->edit_box);

  composite_edit->edit_grid = (GtkGrid *) gtk_grid_new();
  gtk_box_pack_start(composite_edit->edit_box,
		     (GtkWidget *) composite_edit->edit_grid,
		     FALSE, FALSE,
		     0);

  composite_edit->channel_selector = ags_notebook_new();
  gtk_grid_attach(composite_edit->edit_grid,
		  (GtkWidget *) composite_edit->channel_selector,
		  1, 0,
		  1, 1);
  
  composite_edit->edit_control = NULL;
  composite_edit->edit = NULL;
  
  composite_edit->block_vscrollbar = FALSE;

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (gui_scale_factor * AGS_COMPOSITE_EDIT_DEFAULT_SEGMENT_HEIGHT), 1.0);
  composite_edit->vscrollbar = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL,
						 adjustment);

  gtk_widget_set_valign((GtkWidget *) composite_edit->vscrollbar,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) composite_edit->vscrollbar,
			GTK_ALIGN_FILL);

  gtk_grid_attach(composite_edit->edit_grid,
		  (GtkWidget *) composite_edit->vscrollbar,
		  2, 2,
		  1, 1);
  
  composite_edit->block_hscrollbar = FALSE;

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (gui_scale_factor * AGS_COMPOSITE_EDIT_DEFAULT_SEGMENT_WIDTH), 1.0);
  composite_edit->hscrollbar = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL,
						 adjustment);

  gtk_widget_set_valign((GtkWidget *) composite_edit->hscrollbar,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) composite_edit->hscrollbar,
			GTK_ALIGN_FILL);

  gtk_grid_attach(composite_edit->edit_grid,
		  (GtkWidget *) composite_edit->hscrollbar,
		  1, 3,
		  1, 1);		    

  scrolled_edit_meta = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL,
								     NULL);
  gtk_paned_add1(composite_edit->edit_paned,
		 composite_edit->scrolled_edit_meta);
  
  composite_edit->edit_meta = NULL;

  //TODO:JK: implement me
}

AgsUUID*
ags_composite_edit_get_uuid(AgsConnectable *connectable)
{
  AgsCompositeEdit *composite_edit;
  
  AgsUUID *ptr;

  composite_edit = AGS_COMPOSITE_EDIT(connectable);

  ptr = composite_edit->uuid;

  return(ptr);
}

gboolean
ags_composite_edit_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_composite_edit_is_ready(AgsConnectable *connectable)
{
  AgsCompositeEdit *composite_edit;
  
  gboolean is_ready;

  composite_edit = AGS_COMPOSITE_EDIT(connectable);

  /* check is added */
  is_ready = ags_composite_edit_test_flags(composite_edit, AGS_COMPOSITE_EDIT_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_composite_edit_add_to_registry(AgsConnectable *connectable)
{
  AgsCompositeEdit *composite_edit;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  composite_edit = AGS_COMPOSITE_EDIT(connectable);

  ags_composite_edit_set_flags(composite_edit, AGS_COMPOSITE_EDIT_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);

    entry->id = composite_edit->uuid;
    g_value_set_object(entry->entry,
		       (gpointer) composite_edit);
    
    ags_registry_add_entry(registry,
			   entry);
  }
}

void
ags_composite_edit_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_composite_edit_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_composite_edit_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_composite_edit_xml_parse(AgsConnectable *connectable,
			     xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_composite_edit_is_connected(AgsConnectable *connectable)
{
  AgsCompositeEdit *composite_edit;
  
  gboolean is_connected;

  composite_edit = AGS_COMPOSITE_EDIT(connectable);

  /* check is connected */
  is_connected = ags_composite_edit_test_flags(composite_edit, AGS_COMPOSITE_EDIT_CONNECTED);
  
  return(is_connected);
}

void
ags_composite_edit_connect(AgsConnectable *connectable)
{
  AgsCompositeEdit *composite_edit;
  
  composite_edit = AGS_COMPOSITE_EDIT(connectable);

  if((AGS_COMPOSITE_EDIT_CONNECTED & (composite_edit->flags)) != 0){
    return;
  }

  composite_edit->flags |= AGS_COMPOSITE_EDIT_CONNECTED;

  //TODO:JK: implement me
}

void
ags_composite_edit_disconnect(AgsConnectable *connectable)
{
  AgsCompositeEdit *composite_edit;
  
  composite_edit = AGS_COMPOSITE_EDIT(connectable);

  if((AGS_COMPOSITE_EDIT_CONNECTED & (composite_edit->flags)) == 0){
    return;
  }

  composite_edit->flags &= (~AGS_COMPOSITE_EDIT_CONNECTED);

  //TODO:JK: implement me
}

/**
 * ags_composite_edit_test_flags:
 * @composite_edit: the #AgsCompositeEdit
 * @flags: the flags
 *
 * Test @flags to be set on @composite_edit.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.8.0
 */
gboolean
ags_composite_edit_test_flags(AgsCompositeEdit *composite_edit, guint flags)
{
  gboolean retval;  
  
  if(!AGS_IS_COMPOSITE_EDIT(composite_edit)){
    return(FALSE);
  }
    
  /* test */
  retval = (flags & (composite_edit->flags)) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_composite_edit_set_flags:
 * @composite_edit: the #AgsCompositeEdit
 * @flags: see enum AgsCompositeEditFlags
 *
 * Enable a feature of #AgsCompositeEdit.
 *
 * Since: 3.8.0
 */
void
ags_composite_edit_set_flags(AgsCompositeEdit *composite_edit, guint flags)
{
  if(!AGS_IS_COMPOSITE_EDIT(composite_edit)){
    return;
  }

  /* set flags */
  composite_edit->flags |= flags;
}
    
/**
 * ags_composite_edit_unset_flags:
 * @composite_edit: the #AgsCompositeEdit
 * @flags: see enum AgsCompositeEditFlags
 *
 * Disable a feature of AgsCompositeEdit.
 *
 * Since: 3.8.0
 */
void
ags_composite_edit_unset_flags(AgsCompositeEdit *composite_edit, guint flags)
{  
  if(!AGS_IS_COMPOSITE_EDIT(composite_edit)){
    return;
  }

  /* unset flags */
  composite_edit->flags &= (~flags);
}

/**
 * ags_composite_edit_new:
 *
 * Create a new #AgsCompositeEdit.
 *
 * Returns: a new #AgsCompositeEdit
 *
 * Since: 3.8.0
 */
AgsCompositeEdit*
ags_composite_edit_new()
{
  AgsCompositeEdit *composite_edit;

  composite_edit = (AgsCompositeEdit *) g_object_new(AGS_TYPE_COMPOSITE_EDIT,
						     NULL);

  return(composite_edit);
}
