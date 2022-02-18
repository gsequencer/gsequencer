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

#include <ags/app/machine/ags_vst3_bridge_callbacks.h>

#include <ags/app/ags_window.h>
#include <ags/app/ags_effect_bridge.h>
#include <ags/app/ags_effect_bulk.h>
#include <ags/app/ags_bulk_member.h>

#include <ags/libags-vst.h>

#if defined(AGS_WITH_QUARTZ)
#import <CoreFoundation/CoreFoundation.h>
#import <Cocoa/Cocoa.h>
#endif

void
ags_vst3_bridge_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsVst3Bridge *vst3_bridge)
{
  AgsWindow *window;

  gchar *str;

  if(old_parent != NULL){
    return;
  }

  window = AGS_WINDOW(gtk_widget_get_toplevel(widget));

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_VST3_BRIDGE)->counter);

  g_object_set(AGS_MACHINE(vst3_bridge),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_VST3_BRIDGE);

  g_free(str);
}

void
ags_vst3_bridge_show_gui_callback(GtkMenuItem *item, AgsVst3Bridge *vst3_bridge)
{
  AgsVst3Plugin *vst3_plugin;

  vst3_plugin = vst3_bridge->vst3_plugin;

  if(vst3_bridge->iplug_view == NULL){
    vst3_bridge->iplug_view = ags_vst_iedit_controller_create_view(vst3_bridge->iedit_controller,
								   "editor");

    if(vst3_bridge->iplug_view != NULL){
#if defined(AGS_WITH_QUARTZ)
      AgsVstViewRect *view_rect = ags_vst_view_rect_alloc();

      ags_vst_iplug_view_check_size_constraint(vst3_bridge->iplug_view,
					       view_rect);
      
      NSWindow *ns_window = [[NSWindow alloc] init];
          
      vst3_bridge->ns_window = (gpointer) ns_window;
    
//      NSUInteger masks = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskTexturedBackground;
      //    [ns_window setStyleMask:masks];
      ns_window.styleMask |= NSWindowStyleMaskResizable;

      gint32 width = ags_vst_view_rect_get_width(view_rect);
      gint32 height = ags_vst_view_rect_get_height(view_rect);
      
      NSView *view = [[NSView alloc] initWithFrame:NSMakeRect(0.0, 0.0, (CGFloat) width, (CGFloat) height)];

      vst3_bridge->ns_view = (gpointer) view;

      view.layer.backgroundColor = [[NSColor yellowColor] CGColor];

      [ns_window setContentSize:view.frame.size];
      [ns_window setContentView:view];

      [ns_window makeKeyAndOrderFront:nil];

      ags_vst_iplug_view_attached(vst3_bridge->iplug_view,
				  vst3_bridge->ns_view,
				  "NSView");

      vst3_bridge->flags |= AGS_VST3_BRIDGE_UI_VISIBLE;
#endif
    }
  }else{
#if defined(AGS_WITH_QUARTZ)
    NSWindow *ns_window = vst3_bridge->ns_window;
    
    if((AGS_VST3_BRIDGE_UI_VISIBLE & (vst3_bridge->flags)) != 0){
      [ns_window orderOut:nil];

      vst3_bridge->flags &= (~AGS_VST3_BRIDGE_UI_VISIBLE);
    }else{
      [ns_window orderBack:nil];
      [ns_window makeKeyAndOrderFront:nil];
      
      vst3_bridge->flags |= AGS_VST3_BRIDGE_UI_VISIBLE;
    }
#endif
  }
    
  
  //TODO:JK: implement me
}

void
ags_vst3_bridge_program_changed_callback(GtkComboBox *combo_box, AgsVst3Bridge *vst3_bridge)
{
  GtkTreeIter iter;

  GList *start_recall, *recall;

  guint port_index;
  guint program_list_id;
  guint program_index;
  
  if(gtk_combo_box_get_active_iter(combo_box,
				   &iter)){
    gtk_tree_model_get(gtk_combo_box_get_model(combo_box), &iter,
		       3, &port_index,
		       4, &program_list_id,
		       5, &program_index,
		       -1);
  
    /* play context */
    g_object_get(AGS_MACHINE(vst3_bridge)->audio,
		 "play", &start_recall,
		 NULL);
    
    recall = start_recall;
    
    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_VST3_AUDIO)) != NULL){
      ags_fx_vst3_audio_change_program(recall->data,
				       port_index,
				       program_list_id,
				       program_index);
      
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     g_object_unref);

    /* recall context */
    g_object_get(AGS_MACHINE(vst3_bridge)->audio,
		 "recall", &start_recall,
		 NULL);
    
    recall = start_recall;
    
    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_VST3_AUDIO)) != NULL){
      ags_fx_vst3_audio_change_program(recall->data,
				       port_index,
				       program_list_id,
				       program_index);
            
      /* iterate */
      recall = recall->next;
    }
    
    g_list_free_full(start_recall,
		     g_object_unref);
  }
}

AgsVstTResult
ags_vst3_bridge_perform_edit_callback(AgsVstIComponentHandler *icomponent_handler, AgsVstParamID id, AgsVstParamValue value_normalized, AgsVst3Bridge *vst3_bridge)
{
  AgsVst3Plugin *vst3_plugin;
  AgsPluginPort *plugin_port;

  GList *start_bulk_member, *bulk_member;
  
  gchar *filename, *effect;
  gchar *specifier;

  gdouble value;
  
  GRecMutex *base_plugin_mutex;

  if(vst3_bridge == NULL ||
     vst3_bridge->vst3_plugin == NULL ||
     (AGS_VST3_BRIDGE_NO_UPDATE & (vst3_bridge->flags)) != 0){
    return(-1);
  }

  vst3_plugin = vst3_bridge->vst3_plugin;

  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(vst3_plugin);

  g_rec_mutex_lock(base_plugin_mutex);
  
  plugin_port = g_hash_table_lookup(vst3_plugin->plugin_port,
				    GINT_TO_POINTER(id));  

  g_rec_mutex_unlock(base_plugin_mutex);

  if(plugin_port == NULL){
    return(-1);
  }
  
  filename = NULL;
  effect = NULL;

  specifier = NULL;
  
  g_object_get(vst3_plugin,
	       "filename", &filename,
	       "effect", &effect,
	       NULL);

  g_object_get(plugin_port,
	       "port-name", &specifier,
	       NULL);
  
  /* live instrument ports */
  value = ags_vst_iedit_controller_normalized_param_to_plain(vst3_bridge->iedit_controller,
							     id,
							     value_normalized);

  start_bulk_member = gtk_container_get_children(AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(vst3_bridge)->bridge)->bulk_input)->grid);

  bulk_member = start_bulk_member;
  
  while(bulk_member != NULL){
    if(AGS_IS_BULK_MEMBER(bulk_member->data) &&
       !g_strcmp0(AGS_BULK_MEMBER(bulk_member->data)->specifier, specifier)){
      GtkWidget *child_widget;

      gchar *block_scope;

      child_widget = ags_bulk_member_get_widget(bulk_member->data);

      if((block_scope = g_hash_table_lookup(vst3_bridge->block_control, child_widget)) == NULL ||
	 !g_strcmp0(block_scope, AGS_VST3_BRIDGE_BLOCK_CONTROL_VST3_UI) == FALSE){
	if(block_scope == NULL){
	  g_hash_table_insert(vst3_bridge->block_control,
			      child_widget,
			      AGS_VST3_BRIDGE_BLOCK_CONTROL_VST3_UI);
	}
	
	if(AGS_IS_DIAL(child_widget)){
	  ags_dial_set_value((AgsDial *) child_widget,
			     value);
	}else if(GTK_IS_SCALE(child_widget)){
	  gtk_range_set_value((GtkRange *) child_widget,
			      value);
	}else if(GTK_IS_TOGGLE_BUTTON(child_widget)){
	  gboolean active;

	  active = (value != 0.0) ? TRUE: FALSE;
	
	  gtk_toggle_button_set_active((GtkToggleButton *) child_widget,
				       active);
	}else if(GTK_IS_BUTTON(child_widget)){
	  gtk_button_clicked((GtkButton *) child_widget);
	}

	if(block_scope == NULL){
	  g_hash_table_insert(vst3_bridge->block_control,
			      child_widget,
			      NULL);
	}
      }
      
      break;
    }
    
    bulk_member = bulk_member->next;
  }

  g_list_free(start_bulk_member);
  
  g_free(filename);
  g_free(effect);

  g_free(specifier);
  
  return(0);
}

AgsVstTResult
ags_vst3_bridge_restart_component_callback(AgsVstIComponentHandler *icomponent_handler, guint32 flags, AgsVst3Bridge *vst3_bridge)
{
  if((AGS_VST_KPARAM_VALUES_CHANGED & (flags)) != 0){
    ags_vst3_bridge_reload_port(vst3_bridge);
  }
}

void
ags_vst3_bridge_dial_changed_callback(GtkWidget *dial, AgsVst3Bridge *vst3_bridge)
{
  AgsBulkMember *bulk_member;
  GtkAdjustment *adjustment;

  AgsVstParameterInfo *info;
  AgsVstParamID param_id;

  gchar *block_scope;
  
  gint32 port_index;
  gdouble val;
  gdouble value_normalized;

  if((AGS_VST3_BRIDGE_NO_UPDATE & (vst3_bridge->flags)) != 0 ||
     ((block_scope = g_hash_table_lookup(vst3_bridge->block_control, dial)) != NULL &&
      !g_strcmp0(block_scope, AGS_VST3_BRIDGE_BLOCK_CONTROL_BRIDGE))){
    return;
  }
  
  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(dial,
							  AGS_TYPE_BULK_MEMBER);

  g_object_get(dial,
	       "adjustment", &adjustment,
	       NULL);

  port_index = bulk_member->port_index;

  val = gtk_adjustment_get_value(adjustment);

  info = ags_vst_parameter_info_alloc();
    
  ags_vst_iedit_controller_get_parameter_info(vst3_bridge->iedit_controller,
					      port_index, info);
  
  param_id = ags_vst_parameter_info_get_param_id(info);

  value_normalized = ags_vst_iedit_controller_plain_param_to_normalized(vst3_bridge->iedit_controller,
									param_id,
									val);
  
  if(block_scope == NULL){
    g_hash_table_insert(vst3_bridge->block_control,
			dial,
			AGS_VST3_BRIDGE_BLOCK_CONTROL_BRIDGE);
  }
  
  if(vst3_bridge->iedit_controller_host_editing != NULL){
    ags_vst_iedit_controller_host_editing_begin_edit_from_host(vst3_bridge->iedit_controller_host_editing,
							       param_id);
  }
    
  ags_vst_iedit_controller_set_param_normalized(vst3_bridge->iedit_controller,
						param_id,
						value_normalized);

  if(vst3_bridge->iedit_controller_host_editing != NULL){
    ags_vst_iedit_controller_host_editing_end_edit_from_host(vst3_bridge->iedit_controller_host_editing,
							     param_id);
  }

  if(block_scope == NULL){
    g_hash_table_insert(vst3_bridge->block_control,
			dial,
			NULL);
  }
  
  ags_vst_parameter_info_free(info);
}

void
ags_vst3_bridge_scale_changed_callback(GtkWidget *scale, AgsVst3Bridge *vst3_bridge)
{
  AgsBulkMember *bulk_member;
  GtkAdjustment *adjustment;

  AgsVstParameterInfo *info;
  AgsVstParamID param_id;  

  gchar *block_scope;

  gint32 port_index;
  gdouble val;
  gdouble value_normalized;
  
  if((AGS_VST3_BRIDGE_NO_UPDATE & (vst3_bridge->flags)) != 0 ||
     ((block_scope = g_hash_table_lookup(vst3_bridge->block_control, scale)) != NULL &&
      !g_strcmp0(block_scope, AGS_VST3_BRIDGE_BLOCK_CONTROL_BRIDGE))){
    return;
  }

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(scale,
							  AGS_TYPE_BULK_MEMBER);

  g_object_get(scale,
	       "adjustment", &adjustment,
	       NULL);

  port_index = bulk_member->port_index;

  val = gtk_adjustment_get_value(adjustment);
  

  info = ags_vst_parameter_info_alloc();
    
  ags_vst_iedit_controller_get_parameter_info(vst3_bridge->iedit_controller,
					      port_index, info);
  
  param_id = ags_vst_parameter_info_get_param_id(info);

  value_normalized = ags_vst_iedit_controller_plain_param_to_normalized(vst3_bridge->iedit_controller,
									param_id,
									val);
  
  if(block_scope == NULL){
    g_hash_table_insert(vst3_bridge->block_control,
			scale,
			AGS_VST3_BRIDGE_BLOCK_CONTROL_BRIDGE);
  }
  
  if(vst3_bridge->iedit_controller_host_editing != NULL){
    ags_vst_iedit_controller_host_editing_begin_edit_from_host(vst3_bridge->iedit_controller_host_editing,
							       param_id);
  }
    
  ags_vst_iedit_controller_set_param_normalized(vst3_bridge->iedit_controller,
						param_id,
						value_normalized);

  if(vst3_bridge->iedit_controller_host_editing != NULL){
    ags_vst_iedit_controller_host_editing_end_edit_from_host(vst3_bridge->iedit_controller_host_editing,
							     param_id);
  }
  
  if(block_scope == NULL){
    g_hash_table_insert(vst3_bridge->block_control,
			scale,
			NULL);
  }
  
  ags_vst_parameter_info_free(info);
}


void
ags_vst3_bridge_spin_button_changed_callback(GtkWidget *spin_button, AgsVst3Bridge *vst3_bridge)
{
  AgsBulkMember *bulk_member;
  GtkAdjustment *adjustment;

  AgsVstParameterInfo *info;
  AgsVstParamID param_id;  

  gchar *block_scope;

  gint32 port_index;
  gdouble val;
  gdouble value_normalized;

  if((AGS_VST3_BRIDGE_NO_UPDATE & (vst3_bridge->flags)) != 0 ||
     ((block_scope = g_hash_table_lookup(vst3_bridge->block_control, spin_button)) != NULL &&
      !g_strcmp0(block_scope, AGS_VST3_BRIDGE_BLOCK_CONTROL_BRIDGE))){
    return;
  }

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(spin_button,
							  AGS_TYPE_BULK_MEMBER);

  g_object_get(spin_button,
	       "adjustment", &adjustment,
	       NULL);

  port_index = bulk_member->port_index;

  val = gtk_adjustment_get_value(adjustment);

  info = ags_vst_parameter_info_alloc();
    
  ags_vst_iedit_controller_get_parameter_info(vst3_bridge->iedit_controller,
					      port_index, info);
  
  param_id = ags_vst_parameter_info_get_param_id(info);

  value_normalized = ags_vst_iedit_controller_plain_param_to_normalized(vst3_bridge->iedit_controller,
									param_id,
									val);
  
  if(block_scope == NULL){
    g_hash_table_insert(vst3_bridge->block_control,
			spin_button,
			AGS_VST3_BRIDGE_BLOCK_CONTROL_BRIDGE);
  }
  
  if(vst3_bridge->iedit_controller_host_editing != NULL){
    ags_vst_iedit_controller_host_editing_begin_edit_from_host(vst3_bridge->iedit_controller_host_editing,
							       param_id);
  }
    
  ags_vst_iedit_controller_set_param_normalized(vst3_bridge->iedit_controller,
						param_id,
						value_normalized);

  if(vst3_bridge->iedit_controller_host_editing != NULL){
    ags_vst_iedit_controller_host_editing_end_edit_from_host(vst3_bridge->iedit_controller_host_editing,
							     param_id);
  }
    
  if(block_scope == NULL){
    g_hash_table_insert(vst3_bridge->block_control,
			spin_button,
			NULL);
  }
  
  ags_vst_parameter_info_free(info);
}

void
ags_vst3_bridge_check_button_clicked_callback(GtkWidget *check_button, AgsVst3Bridge *vst3_bridge)
{
  AgsBulkMember *bulk_member;

  AgsVstParameterInfo *info;
  AgsVstParamID param_id;  

  gchar *block_scope;

  gboolean is_active;
  gint32 port_index;
  gdouble val;
  gdouble value_normalized;

  if((AGS_VST3_BRIDGE_NO_UPDATE & (vst3_bridge->flags)) != 0 ||
     ((block_scope = g_hash_table_lookup(vst3_bridge->block_control, check_button)) != NULL &&
      !g_strcmp0(block_scope, AGS_VST3_BRIDGE_BLOCK_CONTROL_BRIDGE))){
    return;
  }

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(check_button,
							  AGS_TYPE_BULK_MEMBER);

  is_active = gtk_toggle_button_get_active((GtkToggleButton *) check_button);
  
  port_index = bulk_member->port_index;

  val = is_active ? 1.0: 0.0;

  info = ags_vst_parameter_info_alloc();
    
  ags_vst_iedit_controller_get_parameter_info(vst3_bridge->iedit_controller,
					      port_index, info);
  
  param_id = ags_vst_parameter_info_get_param_id(info);

  value_normalized = ags_vst_iedit_controller_plain_param_to_normalized(vst3_bridge->iedit_controller,
									param_id,
									val);
  
  if(block_scope == NULL){
    g_hash_table_insert(vst3_bridge->block_control,
			check_button,
			AGS_VST3_BRIDGE_BLOCK_CONTROL_BRIDGE);
  }
  
  if(vst3_bridge->iedit_controller_host_editing != NULL){
    ags_vst_iedit_controller_host_editing_begin_edit_from_host(vst3_bridge->iedit_controller_host_editing,
							       param_id);
  }
    
  ags_vst_iedit_controller_set_param_normalized(vst3_bridge->iedit_controller,
						param_id,
						value_normalized);

  if(vst3_bridge->iedit_controller_host_editing != NULL){
    ags_vst_iedit_controller_host_editing_end_edit_from_host(vst3_bridge->iedit_controller_host_editing,
							     param_id);
  }
  
  if(block_scope == NULL){
    g_hash_table_insert(vst3_bridge->block_control,
			check_button,
			NULL);
  }
  
  ags_vst_parameter_info_free(info);
}

void
ags_vst3_bridge_toggle_button_clicked_callback(GtkWidget *toggle_button, AgsVst3Bridge *vst3_bridge)
{
  AgsBulkMember *bulk_member;

  AgsVstParameterInfo *info;
  AgsVstParamID param_id;  

  gchar *block_scope;

  gboolean is_active;
  gint32 port_index;
  gdouble val;
  gdouble value_normalized;

  if((AGS_VST3_BRIDGE_NO_UPDATE & (vst3_bridge->flags)) != 0 ||
     ((block_scope = g_hash_table_lookup(vst3_bridge->block_control, toggle_button)) != NULL &&
      !g_strcmp0(block_scope, AGS_VST3_BRIDGE_BLOCK_CONTROL_BRIDGE))){
    return;
  }

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(toggle_button,
							  AGS_TYPE_BULK_MEMBER);

  is_active = gtk_toggle_button_get_active((GtkToggleButton *) toggle_button);
  
  port_index = bulk_member->port_index;
  val = is_active ? 1.0: 0.0;

  info = ags_vst_parameter_info_alloc();
    
  ags_vst_iedit_controller_get_parameter_info(vst3_bridge->iedit_controller,
					      port_index, info);
  
  param_id = ags_vst_parameter_info_get_param_id(info);

  value_normalized = ags_vst_iedit_controller_plain_param_to_normalized(vst3_bridge->iedit_controller,
									param_id,
									val);
  
  if(block_scope == NULL){
    g_hash_table_insert(vst3_bridge->block_control,
			toggle_button,
			AGS_VST3_BRIDGE_BLOCK_CONTROL_BRIDGE);
  }
  
  if(vst3_bridge->iedit_controller_host_editing != NULL){
    ags_vst_iedit_controller_host_editing_begin_edit_from_host(vst3_bridge->iedit_controller_host_editing,
							       param_id);
  }
    
  ags_vst_iedit_controller_set_param_normalized(vst3_bridge->iedit_controller,
						param_id,
						value_normalized);

  if(vst3_bridge->iedit_controller_host_editing != NULL){
    ags_vst_iedit_controller_host_editing_end_edit_from_host(vst3_bridge->iedit_controller_host_editing,
							     param_id);
  }
  
  if(block_scope == NULL){
    g_hash_table_insert(vst3_bridge->block_control,
			toggle_button,
			NULL);
  }
  
  ags_vst_parameter_info_free(info);
}

void
ags_vst3_bridge_button_clicked_callback(GtkWidget *button, AgsVst3Bridge *vst3_bridge)
{
  AgsBulkMember *bulk_member;

  AgsVstParameterInfo *info;
  AgsVstParamID param_id;  

  gchar *block_scope;

  gint32 port_index;
  gdouble val;
  gdouble value_normalized;

  if((AGS_VST3_BRIDGE_NO_UPDATE & (vst3_bridge->flags)) != 0 ||
     ((block_scope = g_hash_table_lookup(vst3_bridge->block_control, button)) != NULL &&
      !g_strcmp0(block_scope, AGS_VST3_BRIDGE_BLOCK_CONTROL_BRIDGE))){
    return;
  }

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(button,
							  AGS_TYPE_BULK_MEMBER);

  port_index = bulk_member->port_index;

  val = 0.0;

  info = ags_vst_parameter_info_alloc();
    
  ags_vst_iedit_controller_get_parameter_info(vst3_bridge->iedit_controller,
					      port_index, info);
  
  param_id = ags_vst_parameter_info_get_param_id(info);

  value_normalized = ags_vst_iedit_controller_plain_param_to_normalized(vst3_bridge->iedit_controller,
									param_id,
									val);
  
  if(block_scope == NULL){
    g_hash_table_insert(vst3_bridge->block_control,
			button,
			AGS_VST3_BRIDGE_BLOCK_CONTROL_BRIDGE);
  }
  
  if(vst3_bridge->iedit_controller_host_editing != NULL){
    ags_vst_iedit_controller_host_editing_begin_edit_from_host(vst3_bridge->iedit_controller_host_editing,
							       param_id);
  }
    
  ags_vst_iedit_controller_set_param_normalized(vst3_bridge->iedit_controller,
						param_id,
						value_normalized);

  if(vst3_bridge->iedit_controller_host_editing != NULL){
    ags_vst_iedit_controller_host_editing_end_edit_from_host(vst3_bridge->iedit_controller_host_editing,
							     param_id);
  }
  
  if(block_scope == NULL){
    g_hash_table_insert(vst3_bridge->block_control,
			button,
			NULL);
  }
  
  ags_vst_parameter_info_free(info);
}
