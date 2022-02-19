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

#include <ags/app/ags_menu_action_callbacks.h>

#include <ags/app/ags_app_action_util.h>

#include "config.h"

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <ags/app/editor/ags_composite_edit.h>
#include <ags/app/editor/ags_composite_edit_callbacks.h>

#include <ags/app/import/ags_midi_import_wizard.h>

#include <ags/app/export/ags_midi_export_wizard.h>

#include <ags/app/machine/ags_panel.h>
#include <ags/app/machine/ags_mixer.h>
#include <ags/app/machine/ags_spectrometer.h>
#include <ags/app/machine/ags_equalizer10.h>
#include <ags/app/machine/ags_drum.h>
#include <ags/app/machine/ags_matrix.h>
#include <ags/app/machine/ags_synth.h>
#include <ags/app/machine/ags_fm_synth.h>
#include <ags/app/machine/ags_syncsynth.h>
#include <ags/app/machine/ags_fm_syncsynth.h>
#include <ags/app/machine/ags_hybrid_synth.h>
#include <ags/app/machine/ags_hybrid_fm_synth.h>

#if defined(AGS_WITH_LIBINSTPATCH)
#include <ags/app/machine/ags_ffplayer.h>
#include <ags/app/machine/ags_sf2_synth.h>
#endif

#include <ags/app/machine/ags_pitch_sampler.h>
#include <ags/app/machine/ags_sfz_synth.h>

#include <ags/app/machine/ags_audiorec.h>
#include <ags/app/machine/ags_desk.h>

#include <ags/app/machine/ags_dssi_bridge.h>
#include <ags/app/machine/ags_lv2_bridge.h>
#include <ags/app/machine/ags_live_dssi_bridge.h>
#include <ags/app/machine/ags_live_lv2_bridge.h>

#if defined(AGS_WITH_VST3)
#include <ags/app/machine/ags_vst3_bridge.h>
#include <ags/app/machine/ags_live_vst3_bridge.h>
#endif

void
ags_menu_action_open_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_open();
}

void
ags_menu_action_save_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_save();
}

void
ags_menu_action_save_as_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_save_as();
}

void
ags_menu_action_export_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_export();
}

void
ags_menu_action_quit_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_quit();
}

void
ags_menu_action_add_callback(GtkWidget *menu_item, gpointer data)
{
}

void
ags_menu_action_add_panel_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_panel();
}

void
ags_menu_action_add_mixer_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_mixer();
}

void
ags_menu_action_add_spectrometer_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_spectrometer();
}

void
ags_menu_action_add_equalizer_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_equalizer();
}

void
ags_menu_action_add_drum_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_drum();
}

void
ags_menu_action_add_matrix_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_matrix();
}

void
ags_menu_action_add_synth_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_synth();
}

void
ags_menu_action_add_fm_synth_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_fm_synth();
}

void
ags_menu_action_add_syncsynth_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_syncsynth();
}

void
ags_menu_action_add_fm_syncsynth_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_fm_syncsynth();
}

void
ags_menu_action_add_hybrid_synth_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_hybrid_synth();
}

void
ags_menu_action_add_hybrid_fm_synth_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_hybrid_fm_synth();
}

void
ags_menu_action_add_ffplayer_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_ffplayer();
}

void
ags_menu_action_add_sf2_synth_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_sf2_synth();
}

void
ags_menu_action_add_pitch_sampler_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_pitch_sampler();
}

void
ags_menu_action_add_sfz_synth_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_sfz_synth();
}

void
ags_menu_action_add_audiorec_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_audiorec();
}

void
ags_menu_action_add_desk_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_add_desk();
}

void
ags_menu_action_add_ladspa_bridge_callback(GtkWidget *menu_item, gpointer data)
{

  gchar *filename, *effect;
  
  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);

  ags_app_action_util_add_ladspa_bridge(filename, effect);
}

void
ags_menu_action_add_dssi_bridge_callback(GtkWidget *menu_item, gpointer data)
{
  gchar *filename, *effect;

  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);  

  ags_app_action_util_add_dssi_bridge(filename, effect);
}

void
ags_menu_action_add_lv2_bridge_callback(GtkWidget *menu_item, gpointer data)
{
  gchar *filename, *effect;
    
  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);

  ags_app_action_util_add_lv2_bridge(filename, effect);
}

void
ags_menu_action_add_vst3_bridge_callback(GtkWidget *menu_item, gpointer data)
{
#if defined(AGS_WITH_VST3)
  AgsVst3Bridge *vst3_bridge;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;

  gchar *filename, *effect;

  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);

  ags_app_action_util_add_vst3_bridge(filename, effect);
#endif
}

void
ags_menu_action_add_live_dssi_bridge_callback(GtkWidget *menu_item, gpointer data)
{
  gchar *filename, *effect;

  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);

  ags_app_action_util_add_live_dssi_bridge(filename, effect);
}

void
ags_menu_action_add_live_lv2_bridge_callback(GtkWidget *menu_item, gpointer data)
{
  gchar *filename, *effect;
    
  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);

  ags_app_action_util_add_live_lv2_bridge(filename, effect);
}

void
ags_menu_action_add_live_vst3_bridge_callback(GtkWidget *menu_item, gpointer data)
{
#if defined(AGS_WITH_VST3)
  gchar *filename, *effect;

  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);

  ags_app_action_util_add_live_vst3_bridge(filename, effect);
#endif
}

void
ags_menu_action_notation_callback(GtkWidget *menu_item, gpointer data)
{
  AgsApplicationContext *application_context;
  AgsWindow *window;

  gboolean use_composite_editor;
  
  application_context = ags_application_context_get_instance();
  
  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  if(use_composite_editor){
    AgsCompositeEditor *composite_editor;
    AgsMachine *machine;
    
    composite_editor = window->composite_editor;

    machine = composite_editor->selected_machine;
    
    if(AGS_IS_DRUM(machine) ||
       AGS_IS_MATRIX(machine) ||
       AGS_IS_SYNCSYNTH(machine) ||
       AGS_IS_FM_SYNCSYNTH(machine) ||
       AGS_IS_HYBRID_SYNTH(machine) ||
       AGS_IS_HYBRID_FM_SYNTH(machine) ||
#ifdef AGS_WITH_LIBINSTPATCH
       AGS_IS_FFPLAYER(machine) ||
       AGS_IS_SF2_SYNTH(machine) ||
#endif
       AGS_IS_PITCH_SAMPLER(machine) ||
       AGS_IS_SFZ_SYNTH(machine) ||
       AGS_IS_DSSI_BRIDGE(machine) ||
       AGS_IS_LIVE_DSSI_BRIDGE(machine) ||
       (AGS_IS_LV2_BRIDGE(machine) && (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0) ||
       AGS_IS_LIVE_LV2_BRIDGE(machine)
#if defined(AGS_WITH_VST3)
       || (AGS_IS_VST3_BRIDGE(machine) && (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0) ||
       AGS_IS_LIVE_VST3_BRIDGE(machine)
#endif
       ){
      AgsCompositeEdit *composite_edit, *prev_composite_edit;
      
      ags_composite_toolbar_scope_create_and_connect(composite_editor->toolbar,
						     AGS_COMPOSITE_TOOLBAR_SCOPE_NOTATION);

      prev_composite_edit = composite_editor->selected_edit;
      
      composite_edit = 
	composite_editor->selected_edit = composite_editor->notation_edit;
      
      gtk_widget_show_all(composite_editor->notation_edit);
      gtk_widget_hide(composite_editor->sheet_edit);
      gtk_widget_hide(composite_editor->automation_edit);
      gtk_widget_hide(composite_editor->wave_edit);
      
      /* shift piano */
      composite_editor->machine_selector->flags |= AGS_MACHINE_SELECTOR_SHOW_SHIFT_PIANO;
      
      gtk_widget_show(composite_editor->machine_selector->shift_piano);
    }
  }
}

void
ags_menu_action_automation_callback(GtkWidget *menu_item, gpointer data)
{
  AgsApplicationContext *application_context;
  AgsWindow *window;

  gboolean use_composite_editor;
  
  application_context = ags_application_context_get_instance();
  
  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  if(use_composite_editor){
    AgsCompositeEditor *composite_editor;
    AgsCompositeEdit *composite_edit, *prev_composite_edit;
    AgsMachine *machine;

    composite_editor = window->composite_editor;

    machine = composite_editor->selected_machine;
    
    ags_composite_toolbar_scope_create_and_connect(composite_editor->toolbar,
						   AGS_COMPOSITE_TOOLBAR_SCOPE_AUTOMATION);

    prev_composite_edit = composite_editor->selected_edit;      
    
    composite_edit = 
      composite_editor->selected_edit = composite_editor->automation_edit;
    
    gtk_widget_hide(composite_editor->notation_edit);
    gtk_widget_hide(composite_editor->sheet_edit);
    gtk_widget_show_all(composite_editor->automation_edit);
    gtk_widget_hide(composite_editor->wave_edit);
    
    /* shift piano */
    composite_editor->machine_selector->flags &= (~AGS_MACHINE_SELECTOR_SHOW_SHIFT_PIANO);

    gtk_widget_hide(composite_editor->machine_selector->shift_piano);
  }else{  
    gtk_widget_show_all((GtkWidget *) window->automation_window);
  }
}

void
ags_menu_action_wave_callback(GtkWidget *menu_item, gpointer data)
{
  AgsApplicationContext *application_context;
  AgsWindow *window;

  gboolean use_composite_editor;
  
  application_context = ags_application_context_get_instance();
  
  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  if(use_composite_editor){
    AgsCompositeEditor *composite_editor;
    AgsCompositeEdit *composite_edit, *prev_composite_edit;
    AgsMachine *machine;

    composite_editor = window->composite_editor;

    prev_composite_edit = composite_editor->selected_edit;      
    
    composite_edit = composite_editor->wave_edit;

    machine = composite_editor->selected_machine;

    if(AGS_IS_AUDIOREC(machine)){
      GtkAdjustment *adjustment;

      GList *start_wave_edit;

      gdouble lower, upper;
      gdouble page_increment, step_increment;
      gdouble page_size;
      gdouble value;
      
      ags_composite_toolbar_scope_create_and_connect(composite_editor->toolbar,
						     AGS_COMPOSITE_TOOLBAR_SCOPE_WAVE);

      composite_editor->selected_edit = composite_editor->wave_edit;
      
      gtk_widget_hide(composite_editor->notation_edit);
      gtk_widget_hide(composite_editor->sheet_edit);
      gtk_widget_hide(composite_editor->automation_edit);
      gtk_widget_show_all(composite_editor->wave_edit);

      /* shift piano */
      composite_editor->machine_selector->flags &= (~AGS_MACHINE_SELECTOR_SHOW_SHIFT_PIANO);
      
      gtk_widget_hide(composite_editor->machine_selector->shift_piano);

      start_wave_edit = gtk_container_get_children(GTK_CONTAINER(AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->wave_edit_box));
      
      if(start_wave_edit != NULL){
	adjustment = gtk_range_get_adjustment(AGS_WAVE_EDIT(start_wave_edit->data)->hscrollbar);
	
	g_object_get(adjustment,
		     "lower", &lower,
		     "upper", &upper,
		     "page-increment", &page_increment,
		     "step-increment", &step_increment,
		     "page-size", &page_size,
		     "value", &value,
		     NULL);

	g_object_set(gtk_range_get_adjustment((GtkRange *) composite_editor->wave_edit->hscrollbar),
		     "lower", lower,
		     "upper", upper,
		     "page-increment", page_increment,
		     "step-increment", step_increment,
		     "page-size", page_size,
		     "value", value,
		     NULL);

	g_list_free(start_wave_edit);
      }
    }
  }else{  
    gtk_widget_show_all((GtkWidget *) window->wave_window);
  }
}

void
ags_menu_action_sheet_callback(GtkWidget *menu_item, gpointer data)
{
  AgsApplicationContext *application_context;
  AgsWindow *window;

  gboolean use_composite_editor;
  
  application_context = ags_application_context_get_instance();
  
  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  if(use_composite_editor){
    AgsCompositeEditor *composite_editor;
    AgsMachine *machine;
    
    composite_editor = window->composite_editor;

    machine = composite_editor->selected_machine;
    
    if(AGS_IS_DRUM(machine) ||
       AGS_IS_MATRIX(machine) ||
       AGS_IS_SYNCSYNTH(machine) ||
       AGS_IS_FM_SYNCSYNTH(machine) ||
       AGS_IS_HYBRID_SYNTH(machine) ||
       AGS_IS_HYBRID_FM_SYNTH(machine) ||
#ifdef AGS_WITH_LIBINSTPATCH
       AGS_IS_FFPLAYER(machine) ||
       AGS_IS_SF2_SYNTH(machine) ||
#endif
       AGS_IS_PITCH_SAMPLER(machine) ||
       AGS_IS_SFZ_SYNTH(machine) ||
       AGS_IS_DSSI_BRIDGE(machine) ||
       AGS_IS_LIVE_DSSI_BRIDGE(machine) ||
       (AGS_IS_LV2_BRIDGE(machine) && (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0) ||
       AGS_IS_LIVE_LV2_BRIDGE(machine)
#if defined(AGS_WITH_VST3)
       || (AGS_IS_VST3_BRIDGE(machine) && (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0) ||
       AGS_IS_LIVE_VST3_BRIDGE(machine)
#endif
       ){
      AgsCompositeEdit *composite_edit, *prev_composite_edit;
      
      ags_composite_toolbar_scope_create_and_connect(composite_editor->toolbar,
						     AGS_COMPOSITE_TOOLBAR_SCOPE_SHEET);

      prev_composite_edit = composite_editor->selected_edit;
      
      composite_edit = 
	composite_editor->selected_edit = composite_editor->notation_edit;
      
      gtk_widget_hide(composite_editor->notation_edit);
      gtk_widget_show_all(composite_editor->sheet_edit);
      gtk_widget_hide(composite_editor->automation_edit);
      gtk_widget_hide(composite_editor->wave_edit);
      
      /* shift piano */
      composite_editor->machine_selector->flags |= AGS_MACHINE_SELECTOR_SHOW_SHIFT_PIANO;
      
      gtk_widget_show(composite_editor->machine_selector->shift_piano);
    }
  }
}

void
ags_menu_action_meta_data_callback(GtkWidget *menu_item, gpointer data)
{
  //TODO:JK: implement me
}

void
ags_menu_action_preferences_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_preferences();
}

void
ags_menu_action_midi_import_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_smf_import();
}

void
ags_menu_action_midi_export_track_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_smf_export();
}

void
ags_menu_action_midi_playback_callback(GtkWidget *menu_item, gpointer data)
{
  //TODO:JK: implement me
}

void
ags_menu_action_online_help_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_help();
}

void
ags_menu_action_about_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_about();
}
