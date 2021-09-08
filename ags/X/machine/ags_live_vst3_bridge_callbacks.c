/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/X/machine/ags_live_vst3_bridge_callbacks.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_bulk.h>
#include <ags/X/ags_bulk_member.h>

#include <ags/libags-vst.h>

#if defined(AGS_WITH_QUARTZ)
#import <CoreFoundation/CoreFoundation.h>
#import <Cocoa/Cocoa.h>
#endif

void
ags_live_vst3_bridge_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsLiveVst3Bridge *live_vst3_bridge)
{
  AgsWindow *window;

  gchar *str;

  if(old_parent != NULL){
    return;
  }

  window = AGS_WINDOW(gtk_widget_get_toplevel(widget));

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_LIVE_VST3_BRIDGE)->counter);

  g_object_set(AGS_MACHINE(live_vst3_bridge),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_LIVE_VST3_BRIDGE);

  g_free(str);
}

void
ags_live_vst3_bridge_show_gui_callback(GtkMenuItem *item, AgsLiveVst3Bridge *live_vst3_bridge)
{
  AgsVst3Plugin *vst3_plugin;

  vst3_plugin = live_vst3_bridge->vst3_plugin;

  if(live_vst3_bridge->iplug_view == NULL){
    g_message("create view");
    
    live_vst3_bridge->iplug_view = ags_vst_iedit_controller_create_view(live_vst3_bridge->iedit_controller,
									"editor");

    if(live_vst3_bridge->iplug_view != NULL){
#if defined(AGS_WITH_QUARTZ)
      AgsVstViewRect *view_rect = ags_vst_view_rect_alloc();

      ags_vst_iplug_view_check_size_constraint(live_vst3_bridge->iplug_view,
					       view_rect);
      
      NSWindow *ns_window = [[NSWindow alloc] init];
    
      live_vst3_bridge->ns_window = (gpointer) ns_window;
    
      NSUInteger masks = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskTexturedBackground;
      [ns_window setStyleMask:masks];

      gint32 width = ags_vst_view_rect_get_width(view_rect);
      gint32 height = ags_vst_view_rect_get_height(view_rect);

      g_message("%d %d", width, height);
      
      NSView *view = [[NSView alloc] initWithFrame:NSMakeRect(0.0, 0.0, (CGFloat) width, (CGFloat) height)];

      live_vst3_bridge->ns_view = (gpointer) view;

      view.layer.backgroundColor = [[NSColor yellowColor] CGColor];

      [ns_window.contentView addSubview:view];

      NSRect rect;

      rect.size.width = width;
      rect.size.height = height;

      [ns_window setFrame:rect display:YES];

      ags_vst_iplug_view_attached(live_vst3_bridge->iplug_view,
				  live_vst3_bridge->ns_view,
				  "NSView");
#endif
    }
  }
  
  //TODO:JK: implement me
}

void
ags_live_vst3_bridge_program_changed_callback(GtkComboBox *combo_box, AgsLiveVst3Bridge *live_vst3_bridge)
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
    g_object_get(AGS_MACHINE(live_vst3_bridge)->audio,
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
    g_object_get(AGS_MACHINE(live_vst3_bridge)->audio,
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
