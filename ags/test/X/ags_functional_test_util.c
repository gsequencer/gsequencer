/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/test/X/ags_functional_test_util.h>

#include <ags/test/X/libgsequencer.h>

void
ags_functional_test_util_idle()
{
  usleep(500000);
}

GtkMenu*
ags_functional_test_util_submenu_find(GtkMenu *menu,
				      gchar *item_label)
{
  GtkMenu *submenu;

  GList *list_start, *list;
  
  gchar *str;
  
  if(!GTK_IS_MENU(menu) ||
     item_label == NULL){
    return(NULL);
  }

  list =
    list_start = gtk_container_get_children(menu);
  submenu = NULL;
  
  while(list != NULL){
    if(GTK_IS_MENU_ITEM(list->data)){
      str = NULL;
      g_object_get(list->data,
		   "label\0", &str,
		   NULL);

      if(!g_ascii_strcasecmp(str,
			     item_label)){
	g_object_get(list->data,
		     "submenu\0", &submenu,
		     NULL);

	break;
      }
    }

    list = list->next;
  }

  g_list_free(list_start);
  
  ags_functional_test_util_idle();
  
  return(submenu);
}

gboolean
ags_functional_test_util_menu_click(GtkMenu *menu,
				    gchar *item_label)
{
  GList *list_start, *list;
  
  gchar *str;

  gboolean success;
  
  if(menu == NULL ||
     item_label == NULL ||
     !GTK_IS_MENU(menu)){
    return(FALSE);
  }

  list =
    list_start = gtk_container_get_children(menu);
  success = FALSE;
  
  while(list != NULL){
    if(GTK_IS_MENU_ITEM(list->data)){
      str = NULL;
      g_object_get(list->data,
		   "label\0", &str,
		   NULL);

      if(!g_ascii_strcasecmp(str,
			     item_label)){
	gtk_menu_item_activate(list->data);
	success = TRUE;

	break;
      }
    }

    list = list->next;
  }

  g_list_free(list_start);

  ags_functional_test_util_idle();
  
  return(success);
}

gboolean
ags_functional_test_util_dialog_apply(GtkDialog *dialog)
{
  if(!GTK_IS_DIALOG(dialog)){
    return(FALSE);
  }

  gtk_dialog_response(dialog,
		      GTK_RESPONSE_APPLY);

  ags_functional_test_util_idle();
  
  return(TRUE);
}

gboolean
ags_functional_test_util_dialog_ok(GtkDialog *dialog)
{
  if(!GTK_IS_DIALOG(dialog)){
    return(FALSE);
  }

  gtk_dialog_response(dialog,
		      GTK_RESPONSE_OK);

  ags_functional_test_util_idle();
  
  return(TRUE);
}

gboolean
ags_functional_test_util_dialog_cancel(GtkDialog *dialog)
{
  if(!GTK_IS_DIALOG(dialog)){
    return(FALSE);
  }

  gtk_dialog_response(dialog,
		      GTK_RESPONSE_CANCEL);

  ags_functional_test_util_idle();
  
  return(TRUE);
}

gboolean
ags_functional_test_util_open()
{
  AgsXorgApplicationContext *xorg_application_context;

  GtkMenu *menu;
  
  gboolean success;
  
  xorg_application_context = ags_application_context_get_instance();
  menu = xorg_application_context->window->menu_bar->file;
  
  success = ags_functional_test_util_menu_click(menu,
						"open\0");
  
  ags_functional_test_util_idle();
  
  return(success); 
}

gboolean
ags_functional_test_util_save()
{
  AgsXorgApplicationContext *xorg_application_context;

  GtkMenu *menu;
  
  gboolean success;
  
  xorg_application_context = ags_application_context_get_instance();
  menu = xorg_application_context->window->menu_bar->file;
  
  success = ags_functional_test_util_menu_click(menu,
						"save\0");
  
  ags_functional_test_util_idle();
  
  return(success); 
}

gboolean
ags_functional_test_util_save_as()
{
  AgsXorgApplicationContext *xorg_application_context;

  GtkMenu *menu;
  
  gboolean success;
  
  xorg_application_context = ags_application_context_get_instance();
  menu = xorg_application_context->window->menu_bar->file;
  
  success = ags_functional_test_util_menu_click(menu,
						"save as\0");
  
  ags_functional_test_util_idle();
  
  return(success); 
}

gboolean
ags_functional_test_util_export_open()
{
  AgsXorgApplicationContext *xorg_application_context;

  GtkMenu *menu;
  
  gboolean success;
  
  xorg_application_context = ags_application_context_get_instance();
  menu = xorg_application_context->window->menu_bar->file;
  
  success = ags_functional_test_util_menu_click(menu,
						"export\0");
  
  ags_functional_test_util_idle();
  
  return(success); 
}

gboolean
ags_functional_test_util_export_close()
{
  AgsXorgApplicationContext *xorg_application_context;

  GtkDialog *dialog;
  
  gboolean success;
  
  xorg_application_context = ags_application_context_get_instance();
  dialog = xorg_application_context->window->export_window;
  
  success = ags_functional_test_util_dialog_close(dialog);
  
  ags_functional_test_util_idle();
  
  return(success); 
}

gboolean
ags_funcitonal_test_util_export_add()
{
  AgsXorgApplicationContext *xorg_application_context;

  AgsExportWindow *export_window;
  
  xorg_application_context = ags_application_context_get_instance();
  export_window = xorg_application_context->window->export_window;

  gtk_button_clicked(export_window->add);
  
  ags_functional_test_util_idle();
  
  return(TRUE);
}

gboolean
ags_funcitonal_test_util_export_tact(gdouble tact)
{
  AgsXorgApplicationContext *xorg_application_context;

  AgsExportWindow *export_window;
  
  xorg_application_context = ags_application_context_get_instance();
  export_window = xorg_application_context->window->export_window;

  gtk_spin_button_set_value(export_window->tact,
			    tact);
  
  ags_functional_test_util_idle();
  
  return(TRUE);
}

gboolean
ags_funcitonal_test_util_export_remove(guint nth)
{
  AgsXorgApplicationContext *xorg_application_context;

  AgsExportWindow *export_window;

  GList *list_start, *list;

  guint i;
  gboolean success;
  
  xorg_application_context = ags_application_context_get_instance();
  export_window = xorg_application_context->window->export_window;

  list_start =
    list = gtk_container_get_children(export_window->export_soundcard);
  success = FALSE;

  for(i = 0; list != NULL; i++){
    if(i == nth){
      GtkHBox *hbox;
      GtkButton *remove;
      
      GList *tmp_start;
      
      hbox = GTK_HBOX(list->data);
      tmp_start = gtk_container_get_children(hbox);

      remove = gtk_bin_get_child(tmp_start->next->data);
      g_list_free(tmp_start);

      gtk_button_clicked(remove);
      success = TRUE;
      
      break;
    }

    list = list->next;
  }

  g_list_free(list_start);
  
  ags_functional_test_util_idle();
  
  return(success);
}

gboolean
ags_funcitonal_test_util_export_set_backend(guint nth,
					    gchar *backend)
{
  AgsXorgApplicationContext *xorg_application_context;

  AgsExportWindow *export_window;

  GList *list_start, *list;

  guint i;
  gboolean success;
  
  xorg_application_context = ags_application_context_get_instance();
  export_window = xorg_application_context->window->export_window;

  list_start =
    list = gtk_container_get_children(export_window->export_soundcard);
  success = FALSE;

  for(i = 0; list != NULL; i++){
    if(i == nth){
      AgsExportSoundcard *export_soundcard;
      GtkHBox *hbox;
      GtkTreeModel *model;

      GtkTreeIter iter;
      GList *tmp_start;

      gchar *value;

      guint active;
      
      hbox = GTK_HBOX(list->data);
      tmp_start = gtk_container_get_children(hbox);

      export_soundcard = AGS_EXPORT_SOUNDCARD(tmp_start->data);
      g_list_free(tmp_start);

      model = gtk_combo_box_get_model(GTK_COMBO_BOX(export_soundcard->backend));
      active = 0;

      if(gtk_tree_model_get_iter_first(model, &iter)){
	do{
	  gtk_tree_model_get(model, &iter,
			     0, &value,
			     -1);

	  if(!g_strcmp0(backend,
			value)){
	    gtk_combo_box_set_active_iter((GtkComboBox *) export_soundcard->backend,
					  &iter);
	    success = TRUE;
	    
	    break;
	  }
	}while(gtk_tree_model_iter_next(model,
					&iter));
      }      
      
      break;
    }

    list = list->next;
  }

  g_list_free(list_start);
  
  ags_functional_test_util_idle();
  
  return(success);
}

gboolean
ags_funcitonal_test_util_export_set_device(guint nth,
					   gchar *device)
{
  AgsXorgApplicationContext *xorg_application_context;

  AgsExportWindow *export_window;

  GList *list_start, *list;

  guint i;
  gboolean success;
  
  xorg_application_context = ags_application_context_get_instance();
  export_window = xorg_application_context->window->export_window;

  list_start =
    list = gtk_container_get_children(export_window->export_soundcard);
  success = FALSE;

  for(i = 0; list != NULL; i++){
    if(i == nth){
      AgsExportSoundcard *export_soundcard;
      GtkHBox *hbox;
      GtkTreeModel *model;

      GtkTreeIter iter;
      GList *tmp_start;

      gchar *value;

      guint active;
      
      hbox = GTK_HBOX(list->data);
      tmp_start = gtk_container_get_children(hbox);

      export_soundcard = AGS_EXPORT_SOUNDCARD(tmp_start->data);
      g_list_free(tmp_start);

      model = gtk_combo_box_get_model(GTK_COMBO_BOX(export_soundcard->card));
      active = 0;

      if(gtk_tree_model_get_iter_first(model, &iter)){
	do{
	  gtk_tree_model_get(model, &iter,
			     0, &value,
			     -1);

	  if(!g_strcmp0(device,
			value)){
	    gtk_combo_box_set_active_iter((GtkComboBox *) export_soundcard->card,
					  &iter);
	    success = TRUE;
	    
	    break;
	  }
	}while(gtk_tree_model_iter_next(model,
					&iter));
      }      
      
      break;
    }

    list = list->next;
  }

  g_list_free(list_start);
  
  ags_functional_test_util_idle();
  
  return(success);
}

gboolean
ags_funcitonal_test_util_export_set_filename(guint nth,
					     gchar *filename)
{
  AgsXorgApplicationContext *xorg_application_context;

  AgsExportWindow *export_window;

  GList *list_start, *list;

  guint i;
  gboolean success;
  
  xorg_application_context = ags_application_context_get_instance();
  export_window = xorg_application_context->window->export_window;

  list_start =
    list = gtk_container_get_children(export_window->export_soundcard);
  success = FALSE;

  for(i = 0; list != NULL; i++){
    if(i == nth){
      AgsExportSoundcard *export_soundcard;
      GtkHBox *hbox;

      GList *tmp_start;
      
      hbox = GTK_HBOX(list->data);
      tmp_start = gtk_container_get_children(hbox);

      export_soundcard = AGS_EXPORT_SOUNDCARD(tmp_start->data);
      g_list_free(tmp_start);

      gtk_entry_set_text(export_soundcard->filename,
			 filename);
      
      success = TRUE;
      
      break;
    }
    
    list = list->next;
  }

  g_list_free(list_start);
  
  ags_functional_test_util_idle();
  
  return(success);
}

gboolean
ags_funcitonal_test_util_export_open(guint nth)
{
  //TODO:JK: 
}

gboolean
ags_funcitonal_test_util_export_set_format(guint nth,
					   gchar *format)
{
  //TODO:JK: 
}

gboolean
ags_funcitonal_test_util_export_do_export(guint nth,
					  gchar *format)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_quit()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_add_machine(gchar *submenu,
				     gchar *machine_name)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMenuBar *menu_bar;
  
  GtkMenu *add_menu;

  gboolean success;
  
  if(machine_name == NULL){
    return(FALSE);
  }

  xorg_application_context = ags_application_context_get_instance();
  menu_bar = xorg_application_context->window->menu_bar;
  
  add_menu = ags_functional_test_util_submenu_find(menu_bar->edit,
						   GTK_STOCK_ADD);

  success = FALSE;
  
  if(submenu == NULL){
    success = ags_functional_test_util_menu_click(add_menu,
						  machine_name);
  }else{
    GtkMenu *bridge_menu;
    
    bridge_menu = ags_functional_test_util_submenu_find(add_menu,
							submenu);

    success = ags_functional_test_util_menu_click(bridge_menu,
						  machine_name);    
  }

  ags_functional_test_util_idle();
  
  return(success);
}

gboolean
ags_functional_test_util_automation_open()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_close()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_preferences_open()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_preferences_close()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_about_open()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_about_close()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_expand()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_bpm(gdouble bpm)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_rewind()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_prev()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_play()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_stop()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_next()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_forward()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_loop()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_position(gdouble position)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_loop_left(gdouble loop_left)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_loop_right(gdouble loop_right)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_navigation_exclude_sequencers()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_toolbar_cursor_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_toolbar_edit_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_toolbar_delete_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_toolbar_select_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_toolbar_invert_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_toolbar_paste_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_toolbar_copy_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_toolbar_cut_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_toolbar_zoom(guint nth_zoom)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_selector_select(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_selection_select(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_selection_remove_index()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_selection_add_index()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_selection_link_index()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_selection_reverse_mapping()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_selection_shift_piano(guint nth_shift)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_pattern_edit_delete_point(guint nth_index,
						   guint x,
						   guint y)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_pattern_edit_add_point(guint nth_index,
						guint x,
						guint y)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_pattern_edit_select_region(guint nth_index,
						    guint x0, guint x1,
						    guint y0, guint y1)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_note_edit_delete_point(guint nth_index,
						guint x,
						guint y)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_note_edit_add_point(guint nth_index,
					     guint x0, guint x1,
					     guint y)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_note_edit_select_region(guint nth_index,
						 guint x0, guint x1,
						 guint y0, guint y1)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_toolbar_cursor_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_toolbar_edit_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_toolbar_delete_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_toolbar_select_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_toolbar_paste_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_toolbar_copy_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_toolbar_cut_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_toolbar_zoom(guint nth_zoom)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_edit_audio_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_edit_output_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_edit_input_click()
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_edit_delete_point(guint nth_index,
						      guint x,
						      guint y)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_edit_add_point(guint nth_index,
						   guint x,
						   guint y)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_automation_edit_select_region(guint nth_index,
						       guint x0, guint x1,
						       guint y0, guint y1)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_move_up(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_move_down(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_hide(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_show(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_destroy(guint nth_machine)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsMachine *machine;

  GList *list_start, *list;
  
  gboolean success;
  
  xorg_application_context = ags_application_context_get_instance();

  /* retrieve machine */
  list_start = gtk_container_get_children(xorg_application_context->window->machines);
  list = g_list_nth(list_start,
		    nth_machine);

  if(list != NULL &&
     AGS_IS_MACHINE(list->data)){
    machine = list->data;
  }else{
    return(FALSE);
  }
  
  g_list_free(list_start);

  /* activate destroy */
  success = ags_functional_test_util_menu_click(machine->popup,
						"destroy\0");

  ags_functional_test_util_idle();
  
  return(success);
}

gboolean
ags_functional_test_util_machine_rename_open(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_rename_close(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_rename_set_name(guint nth_machine,
						 gchar *name)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_open(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_close(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_click_tab(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_click_enable(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_link_set(guint nth_machine,
						     guint pad, guint audio_channel,
						     gchar *link_name, guint link_nth_line)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_link_open(guint nth_machine,
						      guint pad, guint audio_channel)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_effect_add(guint nth_machine,
						       guint pad, guint audio_channel)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_effect_remove(guint nth_machine,
							  guint pad, guint audio_channel,
							  guint nth)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_effect_plugin_type(guint nth_machine,
							       guint pad, guint audio_channel,
							       gchar *plugin_type)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_ladspa_filename(guint nth_machine,
							    guint pad, guint audio_channel,
							    gchar *filename)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_ladspa_effect(guint nth_machine,
							  guint pad, guint audio_channel,
							  gchar *effect)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_lv2_filename(guint nth_machine,
							 guint pad, guint audio_channel,
							 gchar *filename)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_lv2_effect(guint nth_machine,
						       guint pad, guint audio_channel,
						       gchar *effect)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_bulk_add(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_bulk_remove(guint nth_machine,
							guint nth_bulk)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_bulk_link(guint nth_machine,
						      guint nth_bulk,
						      gchar *link)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_bulk_first_line(guint nth_machine,
							    guint nth_bulk,
							    guint first_line)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_bulk_link_line(guint nth_machine,
							   guint nth_bulk,
							   guint first_link_line)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_bulk_count(guint nth_machine,
						       guint nth_bulk,
						       guint count)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_resize_audio_channels(guint nth_machine,
								  guint audio_channels)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_resize_inputs(guint nth_machine,
							  guint inputs)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_properties_resize_outputs(guint nth_machine,
							   guint outputs)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_edit_copy(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_line_member_set_toggled(gchar *specifier,
						 gboolean is_toggled)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_line_member_set_value(gchar *specifier,
					       gdouble value)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_audio_connection_click_tab(guint nth_machine,
							    guint nth_tab)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_audio_connection_click_enable(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_audio_connection_set_device(guint nth_machine,
							     guint pad, guint audio_channel,
							     gchar *device)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_audio_connection_set_line(guint nth_machine,
							   uint pad, guint audio_channel,
							   guint line)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_audio_connection_bulk_add(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_audio_connection_bulk_remove(guint nth_machine,
							      guint nth_bulk)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_audio_connection_bulk_first_line(guint nth_machine,
								  guint nth_bulk,
								  guint first_line)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_audio_connection_bulk_count(guint nth_machine,
							     guint nth_bulk,
							     guint count)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_audio_connection_bulk_device(guint nth_machine,
							      guint nth_bulk,
							      gchar *device)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_midi_connection_midi_channel(guint nth_machine,
							      guint midi_channel)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_midi_connection_audio_start_mapping(guint nth_machine,
								     guint audio_start_mapping)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_midi_connection_audio_end_mapping(guint nth_machine,
								   guint audio_end_mapping)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_midi_connection_midi_start_mapping(guint nth_machine,
								    guint midi_start_mapping)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_midi_connection_midi_end_mapping(guint nth_machine,
								  guint midi_end_mapping)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_machine_midi_connection_midi_device(guint nth_machine,
							     gchar *device)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_pad_group(guint nth_machine,
				   guint nth_pad)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_pad_mute(guint nth_machine,
				  guint nth_pad)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_pad_solo(guint nth_machine,
				  guint nth_pad)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_pad_play(guint nth_machine,
				  guint nth_pad)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_expander(guint nth_machine,
				  guint nth_pad, guint nth_audio_channel)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_effect_bulk_add(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_effect_bulk_remove(guint nth_machine,
					    guint nth_bulk_effect)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_effect_bulk_ladspa_filename(guint nth_machine,
						     gchar *filename)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_effect_bulk_ladspa_effect(guint nth_machine,
						   gchar *effect)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_effect_bulk_lv2_filename(guint nth_machine,
						  gchar *filename)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_effect_bulk_lv2_effect(guint nth_machine,
						gchar *effect)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_panel_mute_line(guint nth_machine,
					 guint nth_pad, guint nth_audio_channel)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_mixer_input_line_volume(guint nth_machine,
						 guint nth_pad, guint nth_audio_channel,
						 gdouble volume)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_open(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_run(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_loop(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_bank_0(guint nth_machine,
				     guint bank_0)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_bank_1(guint nth_machine,
				     guint bank_1)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_length(guint nth_machine,
				     guint length)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_index(guint nth_machine,
				    guint nth_radio)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_pad(guint nth_machine,
				  guint nth_pad)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_pad_open(guint nth_machine,
				       guint nth_pad)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_pad_edit(guint nth_machine,
				       guint nth_pad)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_input_line_volume(guint nth_machine,
						guint nth_pad, guint nth_audio_channel,
						gdouble volume)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_matrix_run(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_matrix_loop(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_matrix_bank_1(guint nth_machine,
				       guint bank_1)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_matrix_length(guint nth_machine,
				       guint length)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_matrix_gutter(guint nth_machine,
				       guint nth_gutter)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_drum_cell(guint nth_machine,
				   guint x, guint y)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_synth_auto_update(guint nth_machine)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_synth_update(guint nth_machine)
{
  //TODO:JK: 
}

gboolean ags_functional_test_util_oscillator_wave(guint nth_machine,
						  guint nth_oscillator,
						  gchar *wave)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_oscillator_phase(guint nth_machine,
					  guint nth_oscillator,
					  gdouble phase)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_oscillator_attack(guint nth_machine,
					   guint nth_oscillator,
					   gdouble attack)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_oscillator_frequency(guint nth_machine,
					      guint nth_oscillator,
					      gdouble frequency)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_oscillator_length(guint nth_machine,
					   guint nth_oscillator,
					   gdouble length)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_oscillator_volume(guint nth_machine,
					   guint nth_oscillator,
					   gdouble volume)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_ffplayer_open(guint nth_machine)
{
  //TODO:JK: 
}

gboolean ags_functional_test_util_ffplayer_preset(guint nth_machine,
						  gchar *preset)
{
  //TODO:JK: 
}

gboolean
ags_functional_test_util_ffplayer_instrument(guint nth_machine,
					     gchar *instrument)
{
  //TODO:JK: 
}
