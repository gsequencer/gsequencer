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

#include <ags/X/ags_composite_view.h>
#include <ags/X/ags_composite_view_callbacks.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_composite_view_class_init(AgsCompositeViewClass *composite_view);
void ags_composite_view_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_composite_view_init(AgsCompositeView *composite_view);

AgsUUID* ags_composite_view_get_uuid(AgsConnectable *connectable);
gboolean ags_composite_view_has_resource(AgsConnectable *connectable);
gboolean ags_composite_view_is_ready(AgsConnectable *connectable);
void ags_composite_view_add_to_registry(AgsConnectable *connectable);
void ags_composite_view_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_composite_view_list_resource(AgsConnectable *connectable);
xmlNode* ags_composite_view_xml_compose(AgsConnectable *connectable);
void ags_composite_view_xml_parse(AgsConnectable *connectable,
				  xmlNode *node);
gboolean ags_composite_view_is_connected(AgsConnectable *connectable);
void ags_composite_view_connect(AgsConnectable *connectable);
void ags_composite_view_disconnect(AgsConnectable *connectable);

static gpointer ags_composite_view_parent_class = NULL;

/**
 * SECTION:ags_composite_view
 * @short_description: composite view
 * @title: AgsCompositeView
 * @section_id:
 * @include: ags/X/ags_composite_view.h
 *
 * The #AgsCompositeView is a composite abstraction of edit implementation.
 */

GType
ags_composite_view_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_composite_view = 0;

    static const GTypeInfo ags_composite_view_info = {
      sizeof (AgsCompositeViewClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_composite_view_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCompositeView),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_composite_view_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_composite_view_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_composite_view = g_type_register_static(GTK_TYPE_BOX,
						     "AgsCompositeView", &ags_composite_view_info,
						     0);
    
    g_type_add_interface_static(ags_type_composite_view,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_composite_view);
  }

  return g_define_type_id__volatile;
}

void
ags_composite_view_class_init(AgsCompositeViewClass *composite_view)
{
  ags_composite_view_parent_class = g_type_class_peek_parent(composite_view);
}

void
ags_composite_view_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_composite_view_get_uuid;
  connectable->has_resource = ags_composite_view_has_resource;

  connectable->is_ready = ags_composite_view_is_ready;
  connectable->add_to_registry = ags_composite_view_add_to_registry;
  connectable->remove_from_registry = ags_composite_view_remove_from_registry;

  connectable->list_resource = ags_composite_view_list_resource;
  connectable->xml_compose = ags_composite_view_xml_compose;
  connectable->xml_parse = ags_composite_view_xml_parse;

  connectable->is_connected = ags_composite_view_is_connected;
  connectable->connect = ags_composite_view_connect;
  connectable->disconnect = ags_composite_view_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_composite_view_init(AgsCompositeView *composite_view)
{
  GtkScrolledWindow *scrolled_edit_meta;
  GtkAdjustment *adjustment;
  
  AgsApplicationContext *application_context;

  gdouble gui_scale_factor;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(composite_view),
				 GTK_ORIENTATION_VERTICAL);
  
  composite_view->flags = 0;
  composite_view->scrollbar = 0;

  composite_view->version = NULL;
  composite_view->build_id = NULL;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  /* uuid */
  composite_view->uuid = ags_uuid_alloc();
  ags_uuid_generate(composite_view->uuid);

  /* widgets */
  composite_view->edit_paned = (GtkPaned *) gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start((GtkBox *) composite_view,
		     (GtkWidget *) composite_view->edit_paned,
		     FALSE, FALSE,
		     0);
  
  composite_view->edit_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
						    0);
  gtk_paned_add1(composite_view->edit_paned,
		 composite_view->edit_box);

  composite_view->edit_grid = (GtkGrid *) gtk_grid_new();
  gtk_box_pack_start(composite_view->edit_box,
		     (GtkWidget *) composite_view->edit_grid,
		     FALSE, FALSE,
		     0);

  composite_view->channel_selector = ags_notebook_new();
  gtk_grid_attach(composite_view->edit_grid,
		  (GtkWidget *) composite_view->channel_selector,
		  1, 0,
		  1, 1);
  
  composite_view->edit_control = NULL;
  composite_view->edit = NULL;
  
  composite_view->block_vscrollbar = FALSE;

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (gui_scale_factor * AGS_COMPOSITE_VIEW_DEFAULT_SEGMENT_HEIGHT), 1.0);
  composite_view->vscrollbar = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL,
						 adjustment);

  gtk_widget_set_valign((GtkWidget *) composite_view->vscrollbar,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) composite_view->vscrollbar,
			GTK_ALIGN_FILL);

  gtk_grid_attach(composite_view->edit_grid,
		  (GtkWidget *) composite_view->vscrollbar,
		  2, 2,
		  1, 1);
  
  composite_view->block_hscrollbar = FALSE;

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (gui_scale_factor * AGS_COMPOSITE_VIEW_DEFAULT_SEGMENT_WIDTH), 1.0);
  composite_view->hscrollbar = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL,
						 adjustment);

  gtk_widget_set_valign((GtkWidget *) composite_view->hscrollbar,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) composite_view->hscrollbar,
			GTK_ALIGN_FILL);

  gtk_grid_attach(composite_view->edit_grid,
		  (GtkWidget *) composite_view->hscrollbar,
		  1, 3,
		  1, 1);		    

  scrolled_edit_meta = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL,
								     NULL);
  gtk_paned_add1(composite_view->edit_paned,
		 composite_view->scrolled_edit_meta);
  
  composite_view->edit_meta = NULL;
}

AgsUUID*
ags_composite_view_get_uuid(AgsConnectable *connectable)
{
  AgsCompositeView *composite_view;
  
  AgsUUID *ptr;

  composite_view = AGS_COMPOSITE_VIEW(connectable);

  ptr = composite_view->uuid;

  return(ptr);
}

gboolean
ags_composite_view_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_composite_view_is_ready(AgsConnectable *connectable)
{
  AgsCompositeView *composite_view;
  
  gboolean is_ready;

  composite_view = AGS_COMPOSITE_VIEW(connectable);

  /* check is added */
  is_ready = ags_composite_view_test_flags(composite_view, AGS_COMPOSITE_VIEW_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_composite_view_add_to_registry(AgsConnectable *connectable)
{
  AgsCompositeView *composite_view;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  composite_view = AGS_COMPOSITE_VIEW(connectable);

  ags_composite_view_set_flags(composite_view, AGS_COMPOSITE_VIEW_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);

    entry->id = composite_view->uuid;
    g_value_set_object(entry->entry,
		       (gpointer) composite_view);
    
    ags_registry_add_entry(registry,
			   entry);
  }
}

void
ags_composite_view_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_composite_view_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_composite_view_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_composite_view_xml_parse(AgsConnectable *connectable,
			     xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_composite_view_is_connected(AgsConnectable *connectable)
{
  AgsCompositeView *composite_view;
  
  gboolean is_connected;

  composite_view = AGS_COMPOSITE_VIEW(connectable);

  /* check is connected */
  is_connected = ags_composite_view_test_flags(composite_view, AGS_COMPOSITE_VIEW_CONNECTED);
  
  return(is_connected);
}

void
ags_composite_view_connect(AgsConnectable *connectable)
{
  AgsCompositeView *composite_view;
  
  composite_view = AGS_COMPOSITE_VIEW(connectable);

  if((AGS_COMPOSITE_VIEW_CONNECTED & (composite_view->flags)) != 0){
    return;
  }

  composite_view->flags |= AGS_COMPOSITE_VIEW_CONNECTED;

  //TODO:JK: implement me
}

void
ags_composite_view_disconnect(AgsConnectable *connectable)
{
  AgsCompositeView *composite_view;
  
  composite_view = AGS_COMPOSITE_VIEW(connectable);

  if((AGS_COMPOSITE_VIEW_CONNECTED & (composite_view->flags)) == 0){
    return;
  }

  composite_view->flags &= (~AGS_COMPOSITE_VIEW_CONNECTED);

  //TODO:JK: implement me
}

/**
 * ags_composite_view_test_flags:
 * @composite_view: the #AgsCompositeView
 * @flags: the flags
 *
 * Test @flags to be set on @composite_view.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.8.0
 */
gboolean
ags_composite_view_test_flags(AgsCompositeView *composite_view, guint flags)
{
  gboolean retval;  
  
  if(!AGS_IS_COMPOSITE_VIEW(composite_view)){
    return(FALSE);
  }
    
  /* test */
  retval = (flags & (composite_view->flags)) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_composite_view_set_flags:
 * @composite_view: the #AgsCompositeView
 * @flags: see enum AgsCompositeViewFlags
 *
 * Enable a feature of #AgsCompositeView.
 *
 * Since: 3.8.0
 */
void
ags_composite_view_set_flags(AgsCompositeView *composite_view, guint flags)
{
  if(!AGS_IS_COMPOSITE_VIEW(composite_view)){
    return;
  }

  /* set flags */
  composite_view->flags |= flags;
}
    
/**
 * ags_composite_view_unset_flags:
 * @composite_view: the #AgsCompositeView
 * @flags: see enum AgsCompositeViewFlags
 *
 * Disable a feature of AgsCompositeView.
 *
 * Since: 3.8.0
 */
void
ags_composite_view_unset_flags(AgsCompositeView *composite_view, guint flags)
{  
  if(!AGS_IS_COMPOSITE_VIEW(composite_view)){
    return;
  }

  /* unset flags */
  composite_view->flags &= (~flags);
}

/**
 * ags_composite_view_new:
 *
 * Create a new #AgsCompositeView.
 *
 * Returns: a new #AgsCompositeView
 *
 * Since: 3.8.0
 */
AgsCompositeView*
ags_composite_view_new()
{
  AgsCompositeView *composite_view;

  composite_view = (AgsCompositeView *) g_object_new(AGS_TYPE_COMPOSITE_VIEW,
						     NULL);

  return(composite_view);
}
