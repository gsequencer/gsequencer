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

#include <ags/app/editor/ags_composite_edit.h>
#include <ags/app/editor/ags_composite_edit_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_composite_editor.h>

#include <ags/app/editor/ags_notation_edit.h>
#include <ags/app/editor/ags_scrolled_automation_edit_box.h>
#include <ags/app/editor/ags_scrolled_wave_edit_box.h>

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
 * @include: ags/app/ags_composite_edit.h
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
  GtkViewport *viewport;

  GtkAdjustment *adjustment;

  AgsApplicationContext *application_context;
  
  gdouble gui_scale_factor;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(composite_edit),
				 GTK_ORIENTATION_VERTICAL);
  
  composite_edit->flags = 0;
  composite_edit->connectable_flags = 0;
  composite_edit->scrollbar = 0;

  composite_edit->version = NULL;
  composite_edit->build_id = NULL;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  /* uuid */
  composite_edit->uuid = ags_uuid_alloc();
  ags_uuid_generate(composite_edit->uuid);

  /* widgets */
  composite_edit->composite_paned = (GtkPaned *) gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);

  gtk_widget_set_valign((GtkWidget *) composite_edit->composite_paned,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) composite_edit->composite_paned,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) composite_edit->composite_paned,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) composite_edit->composite_paned,
			 TRUE);

  gtk_box_append((GtkBox *) composite_edit,
		 (GtkWidget *) composite_edit->composite_paned);

  /* composite horizontal box */
  composite_edit->composite_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
							 0);

  gtk_widget_set_valign((GtkWidget *) composite_edit->composite_box,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) composite_edit->composite_box,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) composite_edit->composite_box,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) composite_edit->composite_box,
			 TRUE);

  gtk_paned_set_start_child(composite_edit->composite_paned,
			    (GtkWidget *) composite_edit->composite_box);

  /* composite grid */
  composite_edit->composite_grid = (GtkGrid *) gtk_grid_new();

  gtk_widget_set_valign((GtkWidget *) composite_edit->composite_grid,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) composite_edit->composite_grid,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) composite_edit->composite_grid,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) composite_edit->composite_grid,
			 TRUE);

  gtk_box_append(composite_edit->composite_box,
		 (GtkWidget *) composite_edit->composite_grid);
  
  /* channel selector */
  composite_edit->channel_selector = ags_notebook_new();

  gtk_widget_set_valign((GtkWidget *) composite_edit->channel_selector,
			GTK_ALIGN_START);
  gtk_widget_set_halign((GtkWidget *) composite_edit->channel_selector,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) composite_edit->channel_selector,
			 FALSE);
  gtk_widget_set_hexpand((GtkWidget *) composite_edit->channel_selector,
			 TRUE);

  gtk_grid_attach(composite_edit->composite_grid,
		  (GtkWidget *) composite_edit->channel_selector,
		  1, 0,
		  1, 1);

  /* ruler */
  composite_edit->ruler = ags_ruler_new(GTK_ORIENTATION_HORIZONTAL,
					(guint) (gui_scale_factor * AGS_RULER_DEFAULT_STEP),
					AGS_RULER_DEFAULT_FACTOR,
					AGS_RULER_DEFAULT_PRECISION,
					AGS_RULER_DEFAULT_SCALE_PRECISION);

  g_object_set(composite_edit->ruler,
	       "height-request", (gint) (gui_scale_factor * AGS_RULER_DEFAULT_HEIGHT),
	       "font-size",  (guint) (gui_scale_factor * composite_edit->ruler->font_size),
	       "large-step", (guint) (gui_scale_factor * AGS_RULER_DEFAULT_LARGE_STEP),
	       "small-step", (guint) (gui_scale_factor * AGS_RULER_DEFAULT_SMALL_STEP),
	       NULL);

  gtk_grid_attach(composite_edit->composite_grid,
		  (GtkWidget *) composite_edit->ruler,
		  1, 1,
		  1, 1);  

  /* edit vertical box */
  composite_edit->edit_mode = 0;
  composite_edit->paste_flags = 0;
  
  composite_edit->edit_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						    0);

  gtk_widget_set_valign((GtkWidget *) composite_edit->edit_box,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) composite_edit->edit_box,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) composite_edit->edit_box,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) composite_edit->edit_box,
			 TRUE);

  gtk_grid_attach(composite_edit->composite_grid,
		  (GtkWidget *) composite_edit->edit_box,
		  1, 2,
		  1, 1);

  /* edit control and edit */
  composite_edit->edit_control = NULL;
  composite_edit->edit = NULL;
  composite_edit->focused_edit = NULL;

  /* vertical scrollbar */
  composite_edit->block_vscrollbar = FALSE;

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (gui_scale_factor * AGS_COMPOSITE_EDIT_DEFAULT_SEGMENT_HEIGHT), 1.0);
  composite_edit->vscrollbar = (GtkScrollbar *) gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL,
								  adjustment);

  gtk_widget_set_valign((GtkWidget *) composite_edit->vscrollbar,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) composite_edit->vscrollbar,
			GTK_ALIGN_START);

  gtk_widget_set_vexpand((GtkWidget *) composite_edit->vscrollbar,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) composite_edit->vscrollbar,
			 FALSE);

  gtk_grid_attach(composite_edit->composite_grid,
		  (GtkWidget *) composite_edit->vscrollbar,
		  2, 2,
		  1, 1);
  
  /* horizontal scrollbar */
  composite_edit->block_hscrollbar = FALSE;

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (gui_scale_factor * AGS_COMPOSITE_EDIT_DEFAULT_SEGMENT_WIDTH), 1.0);
  composite_edit->hscrollbar = (GtkScrollbar *) gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL,
								  adjustment);

  gtk_widget_set_valign((GtkWidget *) composite_edit->hscrollbar,
			GTK_ALIGN_START);
  gtk_widget_set_halign((GtkWidget *) composite_edit->hscrollbar,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) composite_edit->hscrollbar,
			 FALSE);
  gtk_widget_set_hexpand((GtkWidget *) composite_edit->hscrollbar,
			 TRUE);

  gtk_grid_attach(composite_edit->composite_grid,
		  (GtkWidget *) composite_edit->hscrollbar,
		  1, 3,
		  1, 1);

  /* edit meta */
  composite_edit->scrolled_edit_meta = (GtkScrolledWindow *) gtk_scrolled_window_new();

  gtk_paned_set_end_child(composite_edit->composite_paned,
			  (GtkWidget *) composite_edit->scrolled_edit_meta);

  gtk_widget_set_visible(composite_edit->scrolled_edit_meta,
			 FALSE);
  
  composite_edit->edit_meta = NULL;
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
  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (composite_edit->connectable_flags)) != 0) ? TRUE: FALSE;
  
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

  composite_edit->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;

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
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (composite_edit->connectable_flags)) != 0) ? TRUE: FALSE;
  
  return(is_connected);
}

void
ags_composite_edit_connect(AgsConnectable *connectable)
{
  AgsCompositeEdit *composite_edit;

  GtkAdjustment *composite_adjustment;

  composite_edit = AGS_COMPOSITE_EDIT(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (composite_edit->connectable_flags)) != 0){
    return;
  }

  composite_edit->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  if(AGS_IS_NOTATION_EDIT(composite_edit->edit)){
    ags_connectable_connect(AGS_CONNECTABLE(composite_edit->edit));
  }

  if(AGS_IS_SCROLLED_AUTOMATION_EDIT_BOX(composite_edit->edit)){
    GtkAdjustment *vadjustment, *hadjustment;

    vadjustment = gtk_scrolled_window_get_vadjustment(AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_edit->edit)->scrolled_window);
    hadjustment = gtk_scrolled_window_get_hadjustment(AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_edit->edit)->scrolled_window);
    
    g_signal_connect(vadjustment, "value-changed",
		     G_CALLBACK(ags_composite_edit_vscrollbar_callback), composite_edit);
    
    g_signal_connect(hadjustment, "value-changed",
		     G_CALLBACK(ags_composite_edit_hscrollbar_callback), composite_edit);
  }

  if(AGS_IS_SCROLLED_WAVE_EDIT_BOX(composite_edit->edit)){
    GtkAdjustment *vadjustment, *hadjustment;

    vadjustment = gtk_scrolled_window_get_vadjustment(AGS_SCROLLED_WAVE_EDIT_BOX(composite_edit->edit)->scrolled_window);
    hadjustment = gtk_scrolled_window_get_hadjustment(AGS_SCROLLED_WAVE_EDIT_BOX(composite_edit->edit)->scrolled_window);
    
    g_signal_connect(vadjustment, "value-changed",
		     G_CALLBACK(ags_composite_edit_vscrollbar_callback), composite_edit);
    
    g_signal_connect(hadjustment, "value-changed",
		     G_CALLBACK(ags_composite_edit_hscrollbar_callback), composite_edit);
  }
  
  if(AGS_IS_NOTATION_EDIT(composite_edit->edit)){
    g_signal_connect(gtk_scrollbar_get_adjustment(composite_edit->vscrollbar), "value-changed",
		     G_CALLBACK(ags_composite_edit_vscrollbar_callback), composite_edit);

    g_signal_connect(gtk_scrollbar_get_adjustment(composite_edit->hscrollbar), "value-changed",
		     G_CALLBACK(ags_composite_edit_hscrollbar_callback), composite_edit);
    
    composite_adjustment = gtk_scrollbar_get_adjustment(AGS_NOTATION_EDIT(composite_edit->edit)->vscrollbar);

    g_signal_connect(composite_adjustment, "changed",
		     G_CALLBACK(ags_composite_edit_vscrollbar_changed), composite_edit);

    composite_adjustment = gtk_scrollbar_get_adjustment(AGS_NOTATION_EDIT(composite_edit->edit)->hscrollbar);

    g_signal_connect(composite_adjustment, "changed",
		     G_CALLBACK(ags_composite_edit_hscrollbar_changed), composite_edit);
  }
}

void
ags_composite_edit_disconnect(AgsConnectable *connectable)
{
  AgsCompositeEdit *composite_edit;
  
  composite_edit = AGS_COMPOSITE_EDIT(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (composite_edit->connectable_flags)) == 0){
    return;
  }

  composite_edit->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  ags_connectable_disconnect(AGS_CONNECTABLE(composite_edit->edit));

  if(AGS_IS_SCROLLED_AUTOMATION_EDIT_BOX(composite_edit->edit)){
    GtkAdjustment *vadjustment, *hadjustment;

    vadjustment = gtk_scrolled_window_get_vadjustment(AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_edit->edit)->scrolled_window);
    hadjustment = gtk_scrolled_window_get_hadjustment(AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_edit->edit)->scrolled_window);
    
    g_object_disconnect(vadjustment,
			"any_signal::value-changed",
			G_CALLBACK(ags_composite_edit_vscrollbar_callback),
			composite_edit,
			NULL);
    
    g_object_disconnect(hadjustment,
			"any_signal::value-changed",
			G_CALLBACK(ags_composite_edit_hscrollbar_callback),
			composite_edit,
			NULL);
  }

  if(AGS_IS_SCROLLED_WAVE_EDIT_BOX(composite_edit->edit)){
    GtkAdjustment *vadjustment, *hadjustment;

    vadjustment = gtk_scrolled_window_get_vadjustment(AGS_SCROLLED_WAVE_EDIT_BOX(composite_edit->edit)->scrolled_window);
    hadjustment = gtk_scrolled_window_get_hadjustment(AGS_SCROLLED_WAVE_EDIT_BOX(composite_edit->edit)->scrolled_window);
    
    g_object_disconnect(vadjustment,
			"any_signal::value-changed",
			G_CALLBACK(ags_composite_edit_vscrollbar_callback),
			composite_edit,
			NULL);
    
    g_object_disconnect(hadjustment,
			"any_signal::value-changed",
			G_CALLBACK(ags_composite_edit_hscrollbar_callback),
			composite_edit,
			NULL);
  }

  if(AGS_IS_NOTATION_EDIT(composite_edit->edit)){
    GtkAdjustment *composite_adjustment;
      
    g_object_disconnect(composite_edit->vscrollbar,
			"any_signal::value-changed",
			G_CALLBACK(ags_composite_edit_vscrollbar_callback),
			composite_edit,
			NULL);

    g_object_disconnect(composite_edit->hscrollbar,
			"any_signal::value-changed",
			G_CALLBACK(ags_composite_edit_hscrollbar_callback),
			composite_edit,
			NULL);
    
    composite_adjustment = gtk_scrollbar_get_adjustment(AGS_NOTATION_EDIT(composite_edit->edit)->vscrollbar);

    g_object_disconnect(composite_adjustment,
			"any_signal::changed",
			G_CALLBACK(ags_composite_edit_vscrollbar_changed),
			composite_edit,
			NULL);

    composite_adjustment = gtk_scrollbar_get_adjustment(AGS_NOTATION_EDIT(composite_edit->edit)->hscrollbar);

    g_object_disconnect(composite_adjustment,
			"any_signal::changed",
			G_CALLBACK(ags_composite_edit_hscrollbar_changed),
			composite_edit,
			NULL);
  }
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
 * ags_composite_edit_test_scrollbar:
 * @composite_edit: the #AgsCompositeEdit
 * @scrollbar: the scrollbar
 *
 * Test @scrollbar to be set on @composite_edit.
 * 
 * Returns: %TRUE if scrollbar are set, else %FALSE
 *
 * Since: 3.12.0
 */
gboolean
ags_composite_edit_test_scrollbar(AgsCompositeEdit *composite_edit, guint scrollbar)
{
  gboolean retval;  
  
  if(!AGS_IS_COMPOSITE_EDIT(composite_edit)){
    return(FALSE);
  }
    
  /* test */
  retval = (scrollbar & (composite_edit->scrollbar)) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_composite_edit_set_scrollbar:
 * @composite_edit: the #AgsCompositeEdit
 * @scrollbar: see enum AgsCompositeEditScrollbar
 *
 * Enable a feature of #AgsCompositeEdit.
 *
 * Since: 3.12.0
 */
void
ags_composite_edit_set_scrollbar(AgsCompositeEdit *composite_edit, guint scrollbar)
{
  if(!AGS_IS_COMPOSITE_EDIT(composite_edit)){
    return;
  }

  if((AGS_COMPOSITE_EDIT_SCROLLBAR_HORIZONTAL & (scrollbar)) != 0){
    gtk_widget_show(composite_edit->hscrollbar);
  }else{
    if((AGS_COMPOSITE_EDIT_SCROLLBAR_HORIZONTAL & (composite_edit->scrollbar)) == 0){
      gtk_widget_hide(composite_edit->hscrollbar);
    }
  }

  if((AGS_COMPOSITE_EDIT_SCROLLBAR_VERTICAL & (scrollbar)) != 0){
    gtk_widget_show(composite_edit->vscrollbar);
  }else{
    if((AGS_COMPOSITE_EDIT_SCROLLBAR_VERTICAL & (composite_edit->scrollbar)) == 0){
      gtk_widget_hide(composite_edit->vscrollbar);
    }
  }
  
  /* set scrollbar */
  composite_edit->scrollbar |= scrollbar;
}
    
/**
 * ags_composite_edit_unset_scrollbar:
 * @composite_edit: the #AgsCompositeEdit
 * @scrollbar: see enum AgsCompositeEditScrollbar
 *
 * Disable a feature of AgsCompositeEdit.
 *
 * Since: 3.12.0
 */
void
ags_composite_edit_unset_scrollbar(AgsCompositeEdit *composite_edit, guint scrollbar)
{  
  if(!AGS_IS_COMPOSITE_EDIT(composite_edit)){
    return;
  }

  if((AGS_COMPOSITE_EDIT_SCROLLBAR_HORIZONTAL & (scrollbar)) != 0){
    gtk_widget_hide(composite_edit->hscrollbar);
  }else{
    if((AGS_COMPOSITE_EDIT_SCROLLBAR_HORIZONTAL & (composite_edit->scrollbar)) != 0){
      gtk_widget_show(composite_edit->hscrollbar);
    }
  }

  if((AGS_COMPOSITE_EDIT_SCROLLBAR_VERTICAL & (scrollbar)) != 0){
    gtk_widget_hide(composite_edit->vscrollbar);
  }else{
    if((AGS_COMPOSITE_EDIT_SCROLLBAR_VERTICAL & (composite_edit->scrollbar)) != 0){
      gtk_widget_show(composite_edit->vscrollbar);
    }
  }

  /* unset scrollbar */
  composite_edit->scrollbar &= (~scrollbar);
}

/**
 * ags_composite_edit_set_channel_selector_mode:
 * @composite_edit: the #AgsCompositeEdit
 * @channel_selector_mode: see enum AgsCompositeEditChannelSelectorMode
 *
 * Enable a feature of #AgsCompositeEdit.
 *
 * Since: 3.12.0
 */
void
ags_composite_edit_set_channel_selector_mode(AgsCompositeEdit *composite_edit, guint channel_selector_mode)
{
  AgsCompositeEditor *composite_editor;

  GList *start_tab, *tab;
  
  guint length;
  guint i;
  
  if(!AGS_IS_COMPOSITE_EDIT(composite_edit)){
    return;
  }

  composite_editor = gtk_widget_get_ancestor(composite_edit,
					     AGS_TYPE_COMPOSITE_EDITOR);
  
  /* channel selector - remove tabs */
  tab =
    start_tab = ags_notebook_get_tab(composite_edit->channel_selector);
  
  for(i = 0; i < length; i++){
    ags_notebook_remove_tab(composite_edit->channel_selector,
			    tab->data);

    tab = tab->next;
  }

  g_list_free(start_tab);

  if((AGS_COMPOSITE_EDIT_CHANNEL_SELECTOR_AUDIO_CHANNEL & (channel_selector_mode)) != 0){
    if(composite_editor != NULL &&
       composite_editor->selected_machine != NULL){
      guint audio_channels;
      
      g_object_get(composite_editor->selected_machine->audio,
		   "audio-channels", &audio_channels,
		   NULL);
    
      for(i = 0; i < audio_channels; i++){
	gchar *str;

	str = g_strdup_printf("channel %d",
			      i);

	ags_notebook_add_tab(composite_edit->channel_selector,
			     gtk_toggle_button_new_with_label(str));

	g_free(str);
      }
    }
  }else if((AGS_COMPOSITE_EDIT_CHANNEL_SELECTOR_OUTPUT_LINE & (channel_selector_mode)) != 0){
    if(composite_editor != NULL &&
       composite_editor->selected_machine != NULL){
      guint output_lines;
      
      g_object_get(composite_editor->selected_machine->audio,
		   "output-lines", &output_lines,
		   NULL);
    
      for(i = 0; i < output_lines; i++){
	gchar *str;

	str = g_strdup_printf("line %d",
			      i);

	ags_notebook_add_tab(composite_edit->channel_selector,
			     gtk_toggle_button_new_with_label(str));

	g_free(str);
      }
    }
  }else if((AGS_COMPOSITE_EDIT_CHANNEL_SELECTOR_INPUT_LINE & (channel_selector_mode)) != 0){
    if(composite_editor != NULL &&
       composite_editor->selected_machine != NULL){
      guint input_lines;
      
      g_object_get(composite_editor->selected_machine->audio,
		   "input-lines", &input_lines,
		   NULL);
    
      for(i = 0; i < input_lines; i++){
	gchar *str;

	str = g_strdup_printf("line %d",
			      i);

	ags_notebook_add_tab(composite_edit->channel_selector,
			     gtk_toggle_button_new_with_label(str));

	g_free(str);
      }
    }
  }
  
  /* set channel selector mode */
  composite_edit->channel_selector_mode = channel_selector_mode;
}

/**
 * ags_composite_edit_set_edit_mode:
 * @composite_edit: the #AgsCompositeEdit
 * @edit_mode: see enum AgsCompositeEditEditMode
 *
 * Enable a feature of #AgsCompositeEdit.
 *
 * Since: 3.12.0
 */
void
ags_composite_edit_set_edit_mode(AgsCompositeEdit *composite_edit, guint edit_mode)
{
  if(!AGS_IS_COMPOSITE_EDIT(composite_edit)){
    return;
  }

  composite_edit->edit_mode = edit_mode;
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
