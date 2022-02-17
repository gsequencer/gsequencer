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

#include <ags/GSequencer/ags_context_menu.h>
#include <ags/GSequencer/ags_menu_bar.h>
#include <ags/GSequencer/ags_menu_action_callbacks.h>

#include <ags/config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ladspa.h>
#include <lv2.h>
#include <dssi.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_context_menu_class_init(AgsContextMenuClass *context_menu);
void ags_context_menu_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_context_menu_init(AgsContextMenu *context_menu);
void ags_context_menu_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_context_menu_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_context_menu_connect(AgsConnectable *connectable);
void ags_context_menu_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_context_menu
 * @short_description: the context menu
 * @title: AgsContextMenu
 * @section_id:
 * @include: ags/X/ags_context_menu.h
 *
 * #AgsContextMenu is a composite widget to be used as application's context menu.
 */

GType
ags_context_menu_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_context_menu = 0;

    static const GTypeInfo ags_context_menu_info = {
      sizeof (AgsContextMenuClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_context_menu_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsContextMenu),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_context_menu_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_context_menu_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_context_menu = g_type_register_static(GTK_TYPE_MENU,
						   "AgsContextMenu", &ags_context_menu_info,
						   0);
    
    g_type_add_interface_static(ags_type_context_menu,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_context_menu);
  }

  return g_define_type_id__volatile;
}

void
ags_context_menu_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_context_menu_connect;
  connectable->disconnect = ags_context_menu_disconnect;
}

void
ags_context_menu_class_init(AgsContextMenuClass *context_menu)
{
}

void
ags_context_menu_init(AgsContextMenu *context_menu)
{
  GtkMenuItem *item;

  context_menu->flags = 0;
 
  item = (GtkMenuItem *) gtk_tearoff_menu_item_new();
  gtk_menu_shell_append((GtkMenuShell*) context_menu, (GtkWidget*) item);
  
  /* File */
  item = (GtkMenuItem *) gtk_menu_item_new_with_mnemonic(i18n("_File"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu, (GtkWidget*) item);

  context_menu->file = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) context_menu->file);

  item = (GtkMenuItem *) gtk_menu_item_new_with_mnemonic(i18n("_Open"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->file, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_mnemonic(i18n("_Save"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->file, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Save as"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->file, (GtkWidget*) item);

  gtk_menu_shell_append((GtkMenuShell*) context_menu->file,
			(GtkWidget*) gtk_separator_menu_item_new());

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("export"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->file, (GtkWidget*) item);

  gtk_menu_shell_append((GtkMenuShell*) context_menu->file,
			(GtkWidget*) gtk_separator_menu_item_new());

  item = (GtkMenuItem *) gtk_menu_item_new_with_mnemonic(i18n("_Quit"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->file, (GtkWidget*) item);

  /* Edit */
  item = (GtkMenuItem *) gtk_menu_item_new_with_mnemonic(i18n("_Edit"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu, (GtkWidget*) item);

  context_menu->edit = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) context_menu->edit);

  //  item = (GtkMenuItem *) gtk_menu_item_new_with_label(GTK_STOCK_UNDO);
  //  gtk_menu_shell_append((GtkMenuShell*) menu, (GtkWidget*) item);

  //  item = (GtkMenuItem *) gtk_menu_item_new_with_label(GTK_STOCK_REDO);
  //  gtk_menu_shell_append((GtkMenuShell*) menu, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Add"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->edit, (GtkWidget*) item);

  context_menu->add = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) context_menu->add);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Panel"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Mixer"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Spectrometer"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Equalizer"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Drum"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Matrix"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Synth"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("FM Synth"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Syncsynth"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("FM Syncsynth"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);
  
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Hybrid Synth"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Hybrid FM Synth"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);

#ifdef AGS_WITH_LIBINSTPATCH
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("FPlayer"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("SF2 Synth"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);
#endif

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Sampler"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("SFZ Synth"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);
  
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Audiorec"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);

#if 0
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Desk"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->add, (GtkWidget*) item);
#endif
  
  /* notation */
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Notation"));
  //  gtk_widget_set_sensitive(item,
  //			   FALSE);
  gtk_menu_shell_append((GtkMenuShell*) context_menu->edit, (GtkWidget*) item);

  /* automation */
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Automation"));
  //  gtk_widget_set_sensitive(item,
  //			   FALSE);
  gtk_menu_shell_append((GtkMenuShell*) context_menu->edit, (GtkWidget*) item);

  /* wave */
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Wave"));
  //  gtk_widget_set_sensitive(item,
  //			   FALSE);
  gtk_menu_shell_append((GtkMenuShell*) context_menu->edit, (GtkWidget*) item);

  /* sheet */
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Sheet"));
  //  gtk_widget_set_sensitive(item,
  //			   FALSE);
  gtk_menu_shell_append((GtkMenuShell*) context_menu->edit, (GtkWidget*) item);
  
  /* preferences */
  gtk_menu_shell_append((GtkMenuShell*) context_menu->edit,
			(GtkWidget*) gtk_separator_menu_item_new());

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Preferences"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->edit, (GtkWidget*) item);
  
  /* MIDI */
  item = (GtkMenuItem *) gtk_menu_item_new_with_label("MIDI");
  gtk_menu_shell_append((GtkMenuShell*) context_menu, (GtkWidget*) item);

  context_menu->midi = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) context_menu->midi);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("import"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->midi, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("export track"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->midi, (GtkWidget*) item);

  gtk_menu_shell_append((GtkMenuShell*) context_menu->midi,
			(GtkWidget*) gtk_separator_menu_item_new());

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("playback"));
  gtk_widget_set_sensitive((GtkWidget *) item,
			   FALSE);
  gtk_menu_shell_append((GtkMenuShell*) context_menu->midi, (GtkWidget*) item);

  /* Help */
  item = (GtkMenuItem *) gtk_menu_item_new_with_mnemonic(i18n("_Help"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu, (GtkWidget*) item);

  context_menu->help = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) context_menu->help);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Online help"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->help, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("About"));
  gtk_menu_shell_append((GtkMenuShell*) context_menu->help, (GtkWidget*) item);
}

void
ags_context_menu_connect(AgsConnectable *connectable)
{
  AgsContextMenu *context_menu;

  GList *list0, *list1, *list2, *list3, *list4;
  GList *list0_start, *list1_start, *list2_start, *list3_start, *list4_start;

  context_menu = AGS_CONTEXT_MENU(connectable);

  if((AGS_CONTEXT_MENU_CONNECTED & (context_menu->flags)) != 0){
    return;
  }

  context_menu->flags |= AGS_CONTEXT_MENU_CONNECTED;
  
  /* File */
  list0 =
    list0_start = gtk_container_get_children((GtkContainer *) context_menu);
  list0 = list0->next;

  list1_start = 
    list1 = gtk_container_get_children ((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list0->data));

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_open_callback), (gpointer) context_menu);
  list1 = list1->next;

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_save_callback), (gpointer) context_menu);
  list1 = list1->next;

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_save_as_callback), context_menu);
  list1 = list1->next->next;

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_export_callback), context_menu);
  list1 = list1->next->next;

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_quit_callback), (gpointer) context_menu);

  g_list_free(list1_start);

  /* Edit */
  list0 = list0->next;
  list1_start = 
    list1 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list0->data));

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_add_callback), (gpointer) context_menu);

  list2_start = 
    list2 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list1->data));
  list1 = list1->next;

  /* machines */
  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_panel_callback), (gpointer) context_menu);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_mixer_callback), (gpointer) context_menu);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_spectrometer_callback), (gpointer) context_menu);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_equalizer_callback), (gpointer) context_menu);
  list2 = list2->next;
  
  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_drum_callback), (gpointer) context_menu);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_matrix_callback), (gpointer) context_menu);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_synth_callback), (gpointer) context_menu);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_fm_synth_callback), (gpointer) context_menu);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_syncsynth_callback), (gpointer) context_menu);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_fm_syncsynth_callback), (gpointer) context_menu);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_hybrid_synth_callback), (gpointer) context_menu);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_hybrid_fm_synth_callback), (gpointer) context_menu);
  list2 = list2->next;

#ifdef AGS_WITH_LIBINSTPATCH
  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_ffplayer_callback), (gpointer) context_menu);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_sf2_synth_callback), (gpointer) context_menu);
  list2 = list2->next;
#endif

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_pitch_sampler_callback), (gpointer) context_menu);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_sfz_synth_callback), (gpointer) context_menu);
  list2 = list2->next;
  
  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_audiorec_callback), (gpointer) context_menu);
  list2 = list2->next;

#if 0
  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_desk_callback), (gpointer) context_menu);
  list2 = list2->next;
#endif
  
  /* ladspa */
  list3_start = 
    list3 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list2->data));
  list2 = list2->next;

  while(list3 != NULL){
    g_signal_connect(G_OBJECT(list3->data), "activate",
		     G_CALLBACK(ags_menu_action_add_ladspa_bridge_callback), (gpointer) context_menu);
    
    list3 = list3->next;
  }
  
  g_list_free(list3_start);
  
  /* dssi */
  list3_start = 
    list3 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list2->data));
  list2 = list2->next;

  while(list3 != NULL){
    g_signal_connect(G_OBJECT(list3->data), "activate",
		     G_CALLBACK(ags_menu_action_add_dssi_bridge_callback), (gpointer) context_menu);

    list3 = list3->next;
  }
  
  g_list_free(list3_start);
  
  /* lv2 */
  list3_start = 
    list3 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list2->data));
  list2 = list2->next;
  
  while(list3 != NULL){
    g_signal_connect(G_OBJECT(list3->data), "activate",
		     G_CALLBACK(ags_menu_action_add_lv2_bridge_callback), (gpointer) context_menu);
    
    list3 = list3->next;
  }

  g_list_free(list3_start);
  
  /* vst3 */
#if defined(AGS_WITH_VST3)
  list3_start = 
    list3 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list2->data));
#endif

  list2 = list2->next;

#if defined(AGS_WITH_VST3)
  while(list3 != NULL){
    g_signal_connect(G_OBJECT(list3->data), "activate",
		     G_CALLBACK(ags_menu_action_add_vst3_bridge_callback), (gpointer) context_menu);

    list3 = list3->next;
  }
  
  g_list_free(list3_start);
#endif
  
  /* live! */
  list3 =
    list3_start = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list2->data));
  
  /* dssi */
  list4_start = 
    list4 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list3->data));
  list3 = list3->next;

  while(list4 != NULL){
    g_signal_connect(G_OBJECT(list4->data), "activate",
		     G_CALLBACK(ags_menu_action_add_live_dssi_bridge_callback), (gpointer) context_menu);

    list4 = list4->next;
  }
  
  g_list_free(list4_start);
  
  /* lv2 */
  list4_start = 
    list4 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list3->data));
  list3 = list3->next;
  
  while(list4 != NULL){
    g_signal_connect(G_OBJECT(list4->data), "activate",
		     G_CALLBACK(ags_menu_action_add_live_lv2_bridge_callback), (gpointer) context_menu);
    
    list4 = list4->next;
  }

  g_list_free(list4_start);

  /* vst3 */
#if defined(AGS_WITH_VST3)
  list4_start = 
    list4 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list3->data));
  list3 = list3->next;

  while(list4 != NULL){
    g_signal_connect(G_OBJECT(list4->data), "activate",
		     G_CALLBACK(ags_menu_action_add_live_vst3_bridge_callback), (gpointer) context_menu);

    list4 = list4->next;
  }
  
  g_list_free(list4_start);
#endif
  
  g_list_free(list3_start);
  g_list_free(list2_start);

  /* notation, automation, wave, sheet and preferences */
  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_notation_callback), (gpointer) context_menu);
  list1 = list1->next;

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_automation_callback), (gpointer) context_menu);
  list1 = list1->next;

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_wave_callback), (gpointer) context_menu);
  list1 = list1->next;

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_sheet_callback), (gpointer) context_menu);
  list1 = list1->next;
  list1 = list1->next;

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_preferences_callback), (gpointer) context_menu);

  g_list_free(list1_start);

  /* MIDI */
  list0 = list0->next;
  list1_start = 
    list1 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list0->data));

  g_signal_connect(G_OBJECT(list1->data), "activate",
		   G_CALLBACK(ags_menu_action_midi_import_callback), (gpointer) context_menu);
  list1 = list1->next;

  g_signal_connect(G_OBJECT(list1->data), "activate",
		   G_CALLBACK(ags_menu_action_midi_export_track_callback), (gpointer) context_menu);
  list1 = list1->next;
  list1 = list1->next;

  g_signal_connect(G_OBJECT(list1->data), "activate",
		   G_CALLBACK(ags_menu_action_midi_playback_callback), (gpointer) context_menu);
  
  g_list_free(list1_start);
  
  /* Help */
  list0 = list0->next;
  list1_start = 
    list1 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list0->data));

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_online_help_callback), (gpointer) context_menu);

  list1 = list1->next;

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_about_callback), (gpointer) context_menu);
  
  g_list_free(list1_start);

  g_list_free(list0_start);
}

void
ags_context_menu_disconnect(AgsConnectable *connectable)
{
  AgsContextMenu *context_menu;
  
  context_menu = AGS_CONTEXT_MENU(connectable);

  if((AGS_CONTEXT_MENU_CONNECTED & (context_menu->flags)) == 0){
    return;
  }

  context_menu->flags &= (~AGS_CONTEXT_MENU_CONNECTED);

  /* empty */
}

/**
 * ags_context_menu_new:
 *
 * Creates an #AgsContextMenu
 *
 * Returns: a new #AgsContextMenu
 *
 * Since: 3.0.0
 */
AgsContextMenu*
ags_context_menu_new()
{
  AgsContextMenu *context_menu;

  context_menu = (AgsContextMenu *) g_object_new(AGS_TYPE_CONTEXT_MENU,
						 NULL);

  return(context_menu);
}
