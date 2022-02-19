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

#include <ags/app/ags_menu_bar.h>
#include <ags/app/ags_menu_action_callbacks.h>

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

void ags_menu_bar_class_init(AgsMenuBarClass *menu_bar);
void ags_menu_bar_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_menu_bar_init(AgsMenuBar *menu_bar);
void ags_menu_bar_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec);
void ags_menu_bar_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec);

void ags_menu_bar_connect(AgsConnectable *connectable);
void ags_menu_bar_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_menu_bar
 * @short_description: the menu bar.
 * @title: AgsMenuBar
 * @section_id:
 * @include: ags/X/ags_menu_bar.h
 *
 * #AgsMenuBar is a composite widget to be used as application's menu bar.
 */

GType
ags_menu_bar_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_menu_bar = 0;

    static const GTypeInfo ags_menu_bar_info = {
      sizeof (AgsMenuBarClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_menu_bar_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMenuBar),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_menu_bar_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_menu_bar_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_menu_bar = g_type_register_static(GTK_TYPE_MENU_BAR,
					       "AgsMenuBar", &ags_menu_bar_info,
					       0);
    
    g_type_add_interface_static(ags_type_menu_bar,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_menu_bar);
  }

  return g_define_type_id__volatile;
}

void
ags_menu_bar_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_menu_bar_connect;
  connectable->disconnect = ags_menu_bar_disconnect;
}

void
ags_menu_bar_class_init(AgsMenuBarClass *menu_bar)
{
}

void
ags_menu_bar_init(AgsMenuBar *menu_bar)
{
  GtkMenuItem *item;

  menu_bar->flags = 0;
  
  /* File */
  item = (GtkMenuItem *) gtk_menu_item_new_with_mnemonic(i18n("_File"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar, (GtkWidget*) item);

  menu_bar->file = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) menu_bar->file);

  item = (GtkMenuItem *) gtk_menu_item_new_with_mnemonic(i18n("_Open"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->file, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_mnemonic(i18n("_Save"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->file, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Save as"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->file, (GtkWidget*) item);

  gtk_menu_shell_append((GtkMenuShell*) menu_bar->file,
			(GtkWidget*) gtk_separator_menu_item_new());

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("export"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->file, (GtkWidget*) item);

  gtk_menu_shell_append((GtkMenuShell*) menu_bar->file,
			(GtkWidget*) gtk_separator_menu_item_new());

  item = (GtkMenuItem *) gtk_menu_item_new_with_mnemonic(i18n("_Quit"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->file, (GtkWidget*) item);

  /* Edit */
  item = (GtkMenuItem *) gtk_menu_item_new_with_mnemonic(i18n("_Edit"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar, (GtkWidget*) item);

  menu_bar->edit = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) menu_bar->edit);

  //  item = (GtkMenuItem *) gtk_menu_item_new_with_label(GTK_STOCK_UNDO);
  //  gtk_menu_shell_append((GtkMenuShell*) menu, (GtkWidget*) item);

  //  item = (GtkMenuItem *) gtk_menu_item_new_with_label(GTK_STOCK_REDO);
  //  gtk_menu_shell_append((GtkMenuShell*) menu, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Add"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->edit, (GtkWidget*) item);

  menu_bar->add = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) menu_bar->add);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Panel"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Mixer"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Spectrometer"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Equalizer"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Drum"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Matrix"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Synth"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("FM Synth"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Syncsynth"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("FM Syncsynth"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Hybrid Synth"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Hybrid FM Synth"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);
  
#ifdef AGS_WITH_LIBINSTPATCH
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("FPlayer"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("SF2 Synth"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);
#endif

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Sampler"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("SFZ Synth"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);
  
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Audiorec"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

#if 0  
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Desk"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);
#endif
  
  /* notation */
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Notation"));
  //  gtk_widget_set_sensitive(item,
  //			   FALSE);
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->edit, (GtkWidget*) item);
  
  /* automation */
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Automation"));
  //  gtk_widget_set_sensitive(item,
  //			   FALSE);
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->edit, (GtkWidget*) item);

  /* wave */
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Wave"));
  //  gtk_widget_set_sensitive(item,
  //			   FALSE);
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->edit, (GtkWidget*) item);

  /* wave */
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Sheet"));
  //  gtk_widget_set_sensitive(item,
  //			   FALSE);
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->edit, (GtkWidget*) item);

  /* meta data */
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->edit,
			(GtkWidget*) gtk_separator_menu_item_new());
  
  item = gtk_menu_item_new_with_label(i18n("Meta-data"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->edit, (GtkWidget*) item);
  
  /* preferences */
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->edit,
			(GtkWidget*) gtk_separator_menu_item_new());

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Preferences"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->edit, (GtkWidget*) item);

  /* MIDI */
  item = (GtkMenuItem *) gtk_menu_item_new_with_label("MIDI");
  gtk_menu_shell_append((GtkMenuShell*) menu_bar, (GtkWidget*) item);

  menu_bar->midi = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) menu_bar->midi);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("import"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->midi, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("export track"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->midi, (GtkWidget*) item);

  gtk_menu_shell_append((GtkMenuShell*) menu_bar->midi,
			(GtkWidget*) gtk_separator_menu_item_new());

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("playback"));
  gtk_widget_set_sensitive((GtkWidget *) item,
			   FALSE);
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->midi, (GtkWidget*) item);

  /* Help */
  item = (GtkMenuItem *) gtk_menu_item_new_with_mnemonic(i18n("_Help"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar, (GtkWidget*) item);

  menu_bar->help = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) menu_bar->help);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("Online help"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->help, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("About"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->help, (GtkWidget*) item);
}

void
ags_menu_bar_connect(AgsConnectable *connectable)
{
  AgsMenuBar *menu_bar;

  GList *list0, *list1, *list2, *list3, *list4;
  GList *list0_start, *list1_start, *list2_start, *list3_start, *list4_start;

  menu_bar = AGS_MENU_BAR(connectable);

  if((AGS_MENU_BAR_CONNECTED & (menu_bar->flags)) != 0){
    return;
  }

  menu_bar->flags |= AGS_MENU_BAR_CONNECTED;
  
  /* File */
  list0 =
    list0_start = gtk_container_get_children(GTK_CONTAINER(menu_bar));

  list1_start = 
    list1 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list0->data));

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_open_callback), (gpointer) menu_bar);
  list1 = list1->next;

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_save_callback), (gpointer) menu_bar);
  list1 = list1->next;

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_save_as_callback), menu_bar);
  list1 = list1->next->next;

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_export_callback), menu_bar);
  list1 = list1->next->next;

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_quit_callback), (gpointer) menu_bar);

  g_list_free(list1_start);

  /* Edit */
  list0 = list0->next;
  list1_start = 
    list1 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list0->data));

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_add_callback), (gpointer) menu_bar);

  list2_start = 
    list2 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list1->data));
  list1 = list1->next;

  /* machines */
  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_panel_callback), (gpointer) menu_bar);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_mixer_callback), (gpointer) menu_bar);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_spectrometer_callback), (gpointer) menu_bar);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_equalizer_callback), (gpointer) menu_bar);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_drum_callback), (gpointer) menu_bar);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_matrix_callback), (gpointer) menu_bar);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_synth_callback), (gpointer) menu_bar);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_fm_synth_callback), (gpointer) menu_bar);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_syncsynth_callback), (gpointer) menu_bar);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_fm_syncsynth_callback), (gpointer) menu_bar);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_hybrid_synth_callback), (gpointer) menu_bar);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_hybrid_fm_synth_callback), (gpointer) menu_bar);
  list2 = list2->next;

#ifdef AGS_WITH_LIBINSTPATCH
  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_ffplayer_callback), (gpointer) menu_bar);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_sf2_synth_callback), (gpointer) menu_bar);
  list2 = list2->next;
#endif

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_pitch_sampler_callback), (gpointer) menu_bar);
  list2 = list2->next;

  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_sfz_synth_callback), (gpointer) menu_bar);
  list2 = list2->next;
  
  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_audiorec_callback), (gpointer) menu_bar);
  list2 = list2->next;

#if 0
  g_signal_connect(G_OBJECT (list2->data), "activate",
		   G_CALLBACK (ags_menu_action_add_desk_callback), (gpointer) menu_bar);
  list2 = list2->next;
#endif
  
  /* ladspa */
  list3_start = 
    list3 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list2->data));
  list2 = list2->next;

  while(list3 != NULL){
    g_signal_connect(G_OBJECT(list3->data), "activate",
		     G_CALLBACK(ags_menu_action_add_ladspa_bridge_callback), (gpointer) menu_bar);
    
    list3 = list3->next;
  }
  
  g_list_free(list3_start);
  
  /* dssi */
  list3_start = 
    list3 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list2->data));
  list2 = list2->next;

  while(list3 != NULL){
    g_signal_connect(G_OBJECT(list3->data), "activate",
		     G_CALLBACK(ags_menu_action_add_dssi_bridge_callback), (gpointer) menu_bar);

    list3 = list3->next;
  }
  
  g_list_free(list3_start);
  
  /* lv2 */
  list3_start = 
    list3 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list2->data));
  list2 = list2->next;
  
  while(list3 != NULL){
    g_signal_connect(G_OBJECT(list3->data), "activate",
		     G_CALLBACK(ags_menu_action_add_lv2_bridge_callback), (gpointer) menu_bar);
    
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
		     G_CALLBACK(ags_menu_action_add_vst3_bridge_callback), (gpointer) menu_bar);

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
		     G_CALLBACK(ags_menu_action_add_live_dssi_bridge_callback), (gpointer) menu_bar);

    list4 = list4->next;
  }
  
  g_list_free(list4_start);
  
  /* lv2 */
  list4_start = 
    list4 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list3->data));
  list3 = list3->next;
  
  while(list4 != NULL){
    g_signal_connect(G_OBJECT(list4->data), "activate",
		     G_CALLBACK(ags_menu_action_add_live_lv2_bridge_callback), (gpointer) menu_bar);
    
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
		     G_CALLBACK(ags_menu_action_add_live_vst3_bridge_callback), (gpointer) menu_bar);

    list4 = list4->next;
  }
  
  g_list_free(list4_start);
#endif
  
  g_list_free(list3_start);
  g_list_free(list2_start);

  /* notation, automation, wave, sheet and preferences */
  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_notation_callback), (gpointer) menu_bar);
  list1 = list1->next;
  
  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_automation_callback), (gpointer) menu_bar);
  list1 = list1->next;

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_wave_callback), (gpointer) menu_bar);
  list1 = list1->next;  

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_sheet_callback), (gpointer) menu_bar);
  list1 = list1->next;  
  list1 = list1->next;

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK(ags_menu_action_meta_data_callback), (gpointer) menu_bar);
  list1 = list1->next;  
  list1 = list1->next;

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_preferences_callback), (gpointer) menu_bar);

  g_list_free(list1_start);

  /* MIDI */
  list0 = list0->next;
  list1_start = 
    list1 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list0->data));

  g_signal_connect(G_OBJECT(list1->data), "activate",
		   G_CALLBACK(ags_menu_action_midi_import_callback), (gpointer) menu_bar);
  list1 = list1->next;

  g_signal_connect(G_OBJECT(list1->data), "activate",
		   G_CALLBACK(ags_menu_action_midi_export_track_callback), (gpointer) menu_bar);
  list1 = list1->next;
  list1 = list1->next;

  g_signal_connect(G_OBJECT(list1->data), "activate",
		   G_CALLBACK(ags_menu_action_midi_playback_callback), (gpointer) menu_bar);
  
  g_list_free(list1_start);
  
  /* Help */
  list0 = list0->next;
  list1_start = 
    list1 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list0->data));

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_online_help_callback), (gpointer) menu_bar);

  list1 = list1->next;

  g_signal_connect(G_OBJECT (list1->data), "activate",
		   G_CALLBACK (ags_menu_action_about_callback), (gpointer) menu_bar);

  g_list_free(list1_start);

  g_list_free(list0_start);
}

void
ags_menu_bar_disconnect(AgsConnectable *connectable)
{
  AgsMenuBar *menu_bar;
  
  menu_bar = AGS_MENU_BAR(connectable);

  if((AGS_MENU_BAR_CONNECTED & (menu_bar->flags)) == 0){
    return;
  }

  menu_bar->flags &= (~AGS_MENU_BAR_CONNECTED);

  /* empty */
}

GtkMenu*
ags_zoom_menu_new()
{
  GtkMenu *menu;
  GtkMenuItem *item;
  GtkLabel *label;

  menu = (GtkMenu *) gtk_menu_new();

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new("16:1");
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new("8:1");
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new("4:1");
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new("2:1");
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new("1:1");
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);


  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new("1:2");
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new("1:4");
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new("1:8");
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new("1:16");
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  return(menu);
}

GtkMenu*
ags_tact_menu_new()
{
  GtkMenu *menu;
  GtkMenuItem *item;
  GtkLabel *label;

  menu = (GtkMenu *) gtk_menu_new();

  //TODO:JK: uncomment me if tact implemented
  /*
  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new("16/1");
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new("8/1");
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new("4/1");
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new("2/1");
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);


  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new("1:1");
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);


  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new("1/2");
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new("1/4");
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new("1/8");
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new("1/16");
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);
  */

  return(menu);
}

/**
 * ags_zoom_combo_box_new:
 *
 * Creates an #GtkComboBox to select zoom.
 *
 * Returns: a new #GtkComboBox
 *
 * Since: 3.0.0
 */
GtkComboBox*
ags_zoom_combo_box_new()
{
  GtkComboBoxText *combo_box;

  combo_box = (GtkComboBoxText *) gtk_combo_box_text_new();

  //  gtk_combo_box_text_append_text(combo_box,
  //				 "16:1");
  //  gtk_combo_box_text_append_text(combo_box,
  //				 "8:1");
  gtk_combo_box_text_append_text(combo_box,
				 "4:1");
  gtk_combo_box_text_append_text(combo_box,
				 "2:1");
  gtk_combo_box_text_append_text(combo_box,
				 "1:1");
  gtk_combo_box_text_append_text(combo_box,
				 "1:2");
  gtk_combo_box_text_append_text(combo_box,
				 "1:4");
  gtk_combo_box_text_append_text(combo_box,
				 "1:8");
  gtk_combo_box_text_append_text(combo_box,
				 "1:16");

  return((GtkComboBox *) combo_box);
}

/**
 * ags_zoom_combo_box_new:
 *
 * Creates an #GtkComboBox to select tact
 *
 * Returns: a new #GtkComboBox
 *
 * Since: 3.0.0
 */
GtkComboBox*
ags_tact_combo_box_new()
{
  GtkComboBoxText *combo_box;

  combo_box = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(combo_box,
				 "16/1");
  gtk_combo_box_text_append_text(combo_box,
				 "8/1");
  gtk_combo_box_text_append_text(combo_box,
				 "4/1");
  gtk_combo_box_text_append_text(combo_box,
				 "2/1");
  gtk_combo_box_text_append_text(combo_box,
				 "1/1");
  gtk_combo_box_text_append_text(combo_box,
				 "1/2");
  gtk_combo_box_text_append_text(combo_box,
				 "1/4");
  gtk_combo_box_text_append_text(combo_box,
				 "1/8");
  gtk_combo_box_text_append_text(combo_box,
				 "1/16");

  return((GtkComboBox *) combo_box);
}

GtkMenu*
ags_ladspa_bridge_menu_new()
{
  GtkMenu *menu;
  GtkMenuItem *item;

  AgsLadspaManager *ladspa_manager;

  GList *list, *start_list;

  GRecMutex *ladspa_manager_mutex;

  menu = (GtkMenu *) gtk_menu_new();

  ladspa_manager = ags_ladspa_manager_get_instance();

  /* get ladspa manager mutex */
  ladspa_manager_mutex = AGS_LADSPA_MANAGER_GET_OBJ_MUTEX(ladspa_manager);

  /* get plugin */
  g_rec_mutex_lock(ladspa_manager_mutex);
  
  list =
    start_list = g_list_copy_deep(ladspa_manager->ladspa_plugin,
				  (GCopyFunc) g_object_ref,
				  NULL);

  g_rec_mutex_unlock(ladspa_manager_mutex);
  
  start_list = ags_base_plugin_sort(start_list);
  g_list_free(list);
 
  list = start_list;

  while(list != NULL){
    gchar *filename, *effect;
    
    /* get filename and effect */
    g_object_get(list->data,
		 "filename", &filename,
		 "effect", &effect,
		 NULL);
      
    /* create item */
    item = (GtkMenuItem *) gtk_menu_item_new_with_label(effect);

    g_object_set_data((GObject *) item,
		      AGS_MENU_ITEM_FILENAME_KEY, filename);
    g_object_set_data((GObject *) item,
		      AGS_MENU_ITEM_EFFECT_KEY, effect);
    
    gtk_menu_shell_append((GtkMenuShell *) menu,
			  (GtkWidget *) item);

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
  
  return(menu);
}

GtkMenu*
ags_dssi_bridge_menu_new()
{
  GtkMenu *menu;
  GtkMenuItem *item;

  AgsDssiManager *dssi_manager;

  GList *list, *start_list;

  GRecMutex *dssi_manager_mutex;

  menu = (GtkMenu *) gtk_menu_new();

  dssi_manager = ags_dssi_manager_get_instance();

  /* get dssi manager mutex */
  dssi_manager_mutex = AGS_DSSI_MANAGER_GET_OBJ_MUTEX(dssi_manager);

  /* get plugin */
  g_rec_mutex_lock(dssi_manager_mutex);
  
  list =
    start_list = g_list_copy_deep(dssi_manager->dssi_plugin,
				  (GCopyFunc) g_object_ref,
				  NULL);

  g_rec_mutex_unlock(dssi_manager_mutex);
  
  start_list = ags_base_plugin_sort(start_list);
  g_list_free(list);
 
  list = start_list;

  while(list != NULL){
    gchar *filename, *effect;
    
    /* get filename and effect */
    g_object_get(list->data,
		 "filename", &filename,
		 "effect", &effect,
		 NULL);
      
    /* create item */
    item = (GtkMenuItem *) gtk_menu_item_new_with_label(effect);

    g_object_set_data((GObject *) item,
		      AGS_MENU_ITEM_FILENAME_KEY, filename);
    g_object_set_data((GObject *) item,
		      AGS_MENU_ITEM_EFFECT_KEY, effect);

    gtk_menu_shell_append((GtkMenuShell *) menu,
			  (GtkWidget *) item);

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
  
  return(menu);
}

GtkMenu*
ags_lv2_bridge_menu_new()
{
  GtkMenu *menu;
  GtkMenuItem *item;

  AgsLv2Manager *lv2_manager;

  guint length;
  guint i;
  
  GRecMutex *lv2_manager_mutex;
  
  menu = (GtkMenu *) gtk_menu_new();

  lv2_manager = ags_lv2_manager_get_instance();

  /* get lv2 manager mutex */
  lv2_manager_mutex = AGS_LV2_MANAGER_GET_OBJ_MUTEX(lv2_manager);

  /* get plugin */
  g_rec_mutex_lock(lv2_manager_mutex);

  if(lv2_manager->quick_scan_plugin_filename != NULL){
    length = g_strv_length(lv2_manager->quick_scan_plugin_filename);

    for(i = 0; i < length; i++){
      gchar *filename, *effect;
    
      /* get filename and effect */
      filename = lv2_manager->quick_scan_plugin_filename[i];
      effect = lv2_manager->quick_scan_plugin_effect[i];
      
      /* create item */
      if(filename != NULL &&
	 effect != NULL){
	item = (GtkMenuItem *) gtk_menu_item_new_with_label(effect);
    
	g_object_set_data((GObject *) item,
			  AGS_MENU_ITEM_FILENAME_KEY, filename);
	g_object_set_data((GObject *) item,
			  AGS_MENU_ITEM_EFFECT_KEY, effect);
    
	gtk_menu_shell_append((GtkMenuShell *) menu,
			      (GtkWidget *) item);
      }
    }
  }
  
  if(lv2_manager->quick_scan_instrument_filename != NULL){
    length = g_strv_length(lv2_manager->quick_scan_instrument_filename);
  
    for(i = 0; i < length; i++){
      gchar *filename, *effect;
    
      /* get filename and effect */
      filename = lv2_manager->quick_scan_instrument_filename[i];
      effect = lv2_manager->quick_scan_instrument_effect[i];
    
      /* create item */
      if(filename != NULL &&
	 effect != NULL){
	item = (GtkMenuItem *) gtk_menu_item_new_with_label(effect);
    
	g_object_set_data((GObject *) item,
			  AGS_MENU_ITEM_FILENAME_KEY, filename);
	g_object_set_data((GObject *) item,
			  AGS_MENU_ITEM_EFFECT_KEY, effect);
    
	gtk_menu_shell_append((GtkMenuShell *) menu,
			      (GtkWidget *) item);
      }
    }
  }

  g_rec_mutex_unlock(lv2_manager_mutex);
  
  return(menu);
}

GtkMenu*
ags_vst3_bridge_menu_new()
{
#if defined(AGS_WITH_VST3)
  GtkMenu *menu;
  GtkMenuItem *item;

  AgsVst3Manager *vst3_manager;

  GList *list, *start_list;

  GRecMutex *vst3_manager_mutex;

  menu = (GtkMenu *) gtk_menu_new();

  vst3_manager = ags_vst3_manager_get_instance();

  /* get vst3 manager mutex */
  vst3_manager_mutex = AGS_VST3_MANAGER_GET_OBJ_MUTEX(vst3_manager);

  /* get plugin */
  g_rec_mutex_lock(vst3_manager_mutex);
  
  list =
    start_list = g_list_copy_deep(vst3_manager->vst3_plugin,
				  (GCopyFunc) g_object_ref,
				  NULL);

  g_rec_mutex_unlock(vst3_manager_mutex);
  
  start_list = ags_base_plugin_sort(start_list);
  g_list_free(list);
 
  list = start_list;

  while(list != NULL){
    gchar *filename, *effect;
    
    /* get filename and effect */
    g_object_get(list->data,
		 "filename", &filename,
		 "effect", &effect,
		 NULL);
      
    /* create item */
    item = (GtkMenuItem *) gtk_menu_item_new_with_label(effect);

    g_object_set_data((GObject *) item,
		      AGS_MENU_ITEM_FILENAME_KEY, filename);
    g_object_set_data((GObject *) item,
		      AGS_MENU_ITEM_EFFECT_KEY, effect);

    gtk_menu_shell_append((GtkMenuShell *) menu,
			  (GtkWidget *) item);

    list = list->next;
  }
  
  g_list_free_full(start_list,
		   g_object_unref);
  
  return(menu);
#else
  return(NULL);
#endif
}

GtkMenu*
ags_live_dssi_bridge_menu_new()
{
  GtkMenu *menu;
  GtkMenuItem *item;

  AgsDssiManager *dssi_manager;

  GList *list, *start_list;

  GRecMutex *dssi_manager_mutex;
  
  menu = (GtkMenu *) gtk_menu_new();

  dssi_manager = ags_dssi_manager_get_instance();

  /* get dssi manager mutex */
  dssi_manager_mutex = AGS_DSSI_MANAGER_GET_OBJ_MUTEX(dssi_manager);

  /* get plugin */
  g_rec_mutex_lock(dssi_manager_mutex);
  
  list =
    start_list = g_list_copy_deep(dssi_manager->dssi_plugin,
				  (GCopyFunc) g_object_ref,
				  NULL);
  
  g_rec_mutex_unlock(dssi_manager_mutex);
  
  start_list = ags_base_plugin_sort(start_list);
  g_list_free(list);
 
  list = start_list;

  while(list != NULL){
    gchar *filename, *effect;

    /* get filename and effect */
    g_object_get(list->data,
		 "filename", &filename,
		 "effect", &effect,
		 NULL);
      
    /* create item */
    item = (GtkMenuItem *) gtk_menu_item_new_with_label(effect);
    
    g_object_set_data((GObject *) item,
		      AGS_MENU_ITEM_FILENAME_KEY, filename);
    g_object_set_data((GObject *) item,
		      AGS_MENU_ITEM_EFFECT_KEY, effect);
    
    gtk_menu_shell_append((GtkMenuShell *) menu,
			  (GtkWidget *) item);

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
  
  return(menu);
}

GtkMenu*
ags_live_lv2_bridge_menu_new()
{
  GtkMenu *menu;
  GtkMenuItem *item;

  AgsLv2Manager *lv2_manager;

  guint length;
  guint i;

  GRecMutex *lv2_manager_mutex;
  
  menu = (GtkMenu *) gtk_menu_new();

  lv2_manager = ags_lv2_manager_get_instance();

  /* get lv2 manager mutex */
  lv2_manager_mutex = AGS_LV2_MANAGER_GET_OBJ_MUTEX(lv2_manager);

  /* get plugin */
  g_rec_mutex_lock(lv2_manager_mutex);

  if(lv2_manager->quick_scan_instrument_filename != NULL){
    length = g_strv_length(lv2_manager->quick_scan_instrument_filename);

    for(i = 0; i < length; i++){
      gchar *filename, *effect;
    
      /* get filename and effect */
      filename = lv2_manager->quick_scan_instrument_filename[i];
      effect = lv2_manager->quick_scan_instrument_effect[i];
    
      /* create item */
      if(filename != NULL &&
	 effect != NULL){
	item = (GtkMenuItem *) gtk_menu_item_new_with_label(effect);
    
	g_object_set_data((GObject *) item,
			  AGS_MENU_ITEM_FILENAME_KEY, filename);
	g_object_set_data((GObject *) item,
			  AGS_MENU_ITEM_EFFECT_KEY, effect);
    
	gtk_menu_shell_append((GtkMenuShell *) menu,
			      (GtkWidget *) item);
      }
    }
  }
  
  g_rec_mutex_unlock(lv2_manager_mutex);
    
  return(menu);
}

GtkMenu*
ags_live_vst3_bridge_menu_new()
{
#if defined(AGS_WITH_VST3)
  GtkMenu *menu;
  GtkMenuItem *item;

  AgsVst3Manager *vst3_manager;

  GList *list, *start_list;

  GRecMutex *vst3_manager_mutex;
  
  menu = (GtkMenu *) gtk_menu_new();

  vst3_manager = ags_vst3_manager_get_instance();

  /* get vst3 manager mutex */
  vst3_manager_mutex = AGS_VST3_MANAGER_GET_OBJ_MUTEX(vst3_manager);

  /* get plugin */
  g_rec_mutex_lock(vst3_manager_mutex);
  
  list =
    start_list = g_list_copy_deep(vst3_manager->vst3_plugin,
				  (GCopyFunc) g_object_ref,
				  NULL);
  
  g_rec_mutex_unlock(vst3_manager_mutex);
  
  start_list = ags_base_plugin_sort(start_list);
  g_list_free(list);
 
  list = start_list;

  while(list != NULL){
    gchar *filename, *effect;

    if(ags_base_plugin_test_flags(list->data, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
      /* get filename and effect */
      g_object_get(list->data,
		   "filename", &filename,
		   "effect", &effect,
		   NULL);
      
      /* create item */
      item = (GtkMenuItem *) gtk_menu_item_new_with_label(effect);
    
      g_object_set_data((GObject *) item,
			AGS_MENU_ITEM_FILENAME_KEY, filename);
      g_object_set_data((GObject *) item,
			AGS_MENU_ITEM_EFFECT_KEY, effect);
    
      gtk_menu_shell_append((GtkMenuShell *) menu,
			    (GtkWidget *) item);
    }
    
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
  
  return(menu);
#else
  return(NULL);
#endif
}

/**
 * ags_menu_bar_new:
 *
 * Creates an #AgsMenuBar
 *
 * Returns: a new #AgsMenuBar
 *
 * Since: 3.0.0
 */
AgsMenuBar*
ags_menu_bar_new()
{
  AgsMenuBar *menu_bar;

  menu_bar = (AgsMenuBar *) g_object_new(AGS_TYPE_MENU_BAR,
					 NULL);

  return(menu_bar);
}
