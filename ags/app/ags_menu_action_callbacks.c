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
#include <ags/app/ags_machine_util.h>

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

#ifdef AGS_WITH_LIBINSTPATCH
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

static GMutex locale_mutex;

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
static char *locale_env;
#else
static locale_t c_locale;
#endif

static gboolean locale_initialized = FALSE;

#if defined(AGS_WITH_VST3)
void ags_menu_action_add_vst3_bridge_add_audio_callback(AgsTask *task,
							AgsVst3Bridge *vst3_bridge);

void ags_menu_action_add_live_vst3_bridge_add_audio_callback(AgsTask *task,
							     AgsLiveVst3Bridge *live_vst3_bridge);
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
  AgsPanel *panel;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();

  /* create panel */
  panel = (AgsPanel *) ags_machine_util_new_panel();
  
  add_audio = ags_add_audio_new(AGS_MACHINE(panel)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_add_mixer_callback(GtkWidget *menu_item, gpointer data)
{
  AgsMixer *mixer;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
  
  /* create mixer */
  mixer = (AgsMixer *) ags_machine_util_new_mixer();

  add_audio = ags_add_audio_new(AGS_MACHINE(mixer)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_add_spectrometer_callback(GtkWidget *menu_item, gpointer data)
{
  AgsSpectrometer *spectrometer;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
    
  application_context = ags_application_context_get_instance();

  /* create spectrometer */
  spectrometer = (AgsSpectrometer *) ags_machine_util_new_spectrometer();

  add_audio = ags_add_audio_new(AGS_MACHINE(spectrometer)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_add_equalizer_callback(GtkWidget *menu_item, gpointer data)
{
  AgsEqualizer10 *equalizer10;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
    
  application_context = ags_application_context_get_instance();

  /* create equalizer10 */
  equalizer10 = (AgsEqualizer10 *) ags_machine_util_new_equalizer();

  add_audio = ags_add_audio_new(AGS_MACHINE(equalizer10)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_add_drum_callback(GtkWidget *menu_item, gpointer data)
{
  AgsDrum *drum;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
    
  application_context = ags_application_context_get_instance();

  /* create drum */
  drum = (AgsDrum *) ags_machine_util_new_drum();
  
  add_audio = ags_add_audio_new(AGS_MACHINE(drum)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_add_matrix_callback(GtkWidget *menu_item, gpointer data)
{
  AgsMatrix *matrix;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  /* create matrix */
  matrix = (AgsMatrix *) ags_machine_util_new_matrix();

  add_audio = ags_add_audio_new(AGS_MACHINE(matrix)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_add_synth_callback(GtkWidget *menu_item, gpointer data)
{
  AgsSynth *synth;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
    
  application_context = ags_application_context_get_instance();

  /* create synth */
  synth = (AgsSynth *) ags_machine_util_new_synth();

  add_audio = ags_add_audio_new(AGS_MACHINE(synth)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_add_fm_synth_callback(GtkWidget *menu_item, gpointer data)
{
  AgsFMSynth *fm_synth;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
    
  application_context = ags_application_context_get_instance();

  /* create fm synth */
  fm_synth = (AgsFMSynth *) ags_machine_util_new_fm_synth();

  add_audio = ags_add_audio_new(AGS_MACHINE(fm_synth)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_add_syncsynth_callback(GtkWidget *menu_item, gpointer data)
{
  AgsSyncsynth *syncsynth;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();

  /* create syncsynth */
  syncsynth = (AgsSyncsynth *) ags_machine_util_new_syncsynth();

  add_audio = ags_add_audio_new(AGS_MACHINE(syncsynth)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_add_fm_syncsynth_callback(GtkWidget *menu_item, gpointer data)
{
  AgsFMSyncsynth *fm_syncsynth;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();

  /* create fm syncsynth */
  fm_syncsynth = (AgsFMSyncsynth *) ags_machine_util_new_fm_syncsynth();

  add_audio = ags_add_audio_new(AGS_MACHINE(fm_syncsynth)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_add_hybrid_synth_callback(GtkWidget *menu_item, gpointer data)
{
  AgsHybridSynth *hybrid_synth;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();

  /* create hybrid synth */
  hybrid_synth = (AgsHybridSynth *) ags_machine_util_new_hybrid_synth();

  add_audio = ags_add_audio_new(AGS_MACHINE(hybrid_synth)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_add_hybrid_fm_synth_callback(GtkWidget *menu_item, gpointer data)
{
  AgsHybridFMSynth *hybrid_fm_synth;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();

  /* create FM hybrid synth */
  hybrid_fm_synth = (AgsHybridFMSynth *) ags_machine_util_new_hybrid_fm_synth();

  add_audio = ags_add_audio_new(AGS_MACHINE(hybrid_fm_synth)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

#ifdef AGS_WITH_LIBINSTPATCH
void
ags_menu_action_add_ffplayer_callback(GtkWidget *menu_item, gpointer data)
{
  AgsFFPlayer *ffplayer;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
  
  /* create ffplayer */
  ffplayer = (AgsFFPlayer *) ags_machine_util_new_ffplayer();

  add_audio = ags_add_audio_new(AGS_MACHINE(ffplayer)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_add_sf2_synth_callback(GtkWidget *menu_item, gpointer data)
{
  AgsSF2Synth *sf2_synth;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
  
  /* create SF2 synth */
  sf2_synth = (AgsSF2Synth *) ags_machine_util_new_sf2_synth();
  
  add_audio = ags_add_audio_new(AGS_MACHINE(sf2_synth)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}
#endif

void
ags_menu_action_add_pitch_sampler_callback(GtkWidget *menu_item, gpointer data)
{
  AgsPitchSampler *pitch_sampler;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
  
  /* create pitch sampler */
  pitch_sampler = (AgsPitchSampler *) ags_machine_util_new_pitch_sampler();
  
  add_audio = ags_add_audio_new(AGS_MACHINE(pitch_sampler)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_add_sfz_synth_callback(GtkWidget *menu_item, gpointer data)
{
  AgsSFZSynth *sfz_synth;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
  
  /* create SFZ synth */
  sfz_synth = (AgsSFZSynth *) ags_machine_util_new_sfz_synth();
  
  add_audio = ags_add_audio_new(AGS_MACHINE(sfz_synth)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_add_audiorec_callback(GtkWidget *menu_item, gpointer data)
{
  AgsAudiorec *audiorec;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
  
  /* create audiorec */
  audiorec = (AgsAudiorec *) ags_machine_util_new_audiorec();

  add_audio = ags_add_audio_new(AGS_MACHINE(audiorec)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_add_desk_callback(GtkWidget *menu_item, gpointer data)
{
  AgsDesk *desk;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
  
  /* create desk */
  desk = (AgsDesk *) ags_machine_util_new_desk();

  add_audio = ags_add_audio_new(AGS_MACHINE(desk)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
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
  AgsLv2Bridge *lv2_bridge;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  gchar *filename, *effect;
    
  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);
  
  application_context = ags_application_context_get_instance();

  /* create lv2 bridge */
  lv2_bridge = (AgsLv2Bridge *) ags_machine_util_new_lv2_bridge(filename, effect);
  
  add_audio = ags_add_audio_new(AGS_MACHINE(lv2_bridge)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

#if defined(AGS_WITH_VST3)
void
ags_menu_action_add_vst3_bridge_add_audio_callback(AgsTask *task,
						   AgsVst3Bridge *vst3_bridge)
{
  AgsAudio *audio;
  AgsChannel *start_output, *output;
  AgsPlaybackDomain *playback_domain;

  AgsInstantiateVst3Plugin *instantiate_vst3_plugin;

  AgsVst3Plugin *vst3_plugin;

  AgsAudioThread *audio_thread;
  AgsChannelThread *channel_thread;
  
  GList *start_playback, *playback;	
  GList *start_play, *start_recall;
  GList *start_list, *list;

  guint audio_channels;

  audio = AGS_MACHINE(vst3_bridge)->audio;
  
  vst3_plugin = vst3_bridge->vst3_plugin;

  start_output = NULL;
  
  start_play = NULL;
  start_recall = NULL;

  playback_domain = NULL;

  audio_channels = 0;
  
  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       "output", &start_output,
	       "play", &start_play,
	       "recall", &start_recall,
	       "playback-domain", &playback_domain,
	       NULL);

  list = 
    start_list = g_list_concat(start_play,
			       start_recall);

  start_playback = NULL;
	
  g_object_get(playback_domain,
	       "output-playback", &start_playback,
	       NULL);

  while(list != NULL){
    if(AGS_IS_FX_VST3_AUDIO(list->data)){
      AgsTaskLauncher *task_launcher;      

      if(ags_base_plugin_test_flags((AgsBasePlugin *) vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
	/*  */
	if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_PLAYBACK)){
	  audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							      AGS_SOUND_SCOPE_PLAYBACK);

	  task_launcher = ags_audio_thread_get_task_launcher(audio_thread);
	
	  instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								     AGS_SOUND_SCOPE_PLAYBACK,
								     -1,
								     FALSE);
	  ags_task_launcher_add_task(task_launcher,
				     instantiate_vst3_plugin);

	  g_object_unref(audio_thread);

	  g_object_unref(task_launcher);
	}
	
	/*  */
	if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_SEQUENCER)){
	  audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							      AGS_SOUND_SCOPE_SEQUENCER);

	  task_launcher = ags_audio_thread_get_task_launcher(audio_thread);
	
	  instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								     AGS_SOUND_SCOPE_SEQUENCER,
								     -1,
								     FALSE);
	  ags_task_launcher_add_task(task_launcher,
				     instantiate_vst3_plugin);

	  g_object_unref(audio_thread);

	  g_object_unref(task_launcher);
	}
	
	/*  */
	if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_NOTATION)){
	  audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							      AGS_SOUND_SCOPE_NOTATION);

	  task_launcher = ags_audio_thread_get_task_launcher(audio_thread);
	
	  instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								     AGS_SOUND_SCOPE_NOTATION,
								     -1,
								     FALSE);
	  ags_task_launcher_add_task(task_launcher,
				     instantiate_vst3_plugin);

	  g_object_unref(audio_thread);

	  g_object_unref(task_launcher);
	}
      }else{
	guint i;

	for(i = 0; i < audio_channels; i++){
	  playback = start_playback;

	  output = ags_channel_nth(start_output,
				   i);
	  
	  while(playback != NULL){
	    AgsChannel *channel;
	    
	    gboolean success;

	    channel = NULL;
	    
	    g_object_get(playback->data,
			 "channel", &channel,
			 NULL);

	    success = FALSE;

	    if(channel == output){
	      success = TRUE;
	    }

	    g_object_unref(channel);
	    
	    if(success){
	      break;
	    }

	    playback = playback->next;
	  }
	  
	  /*  */
	  if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_PLAYBACK)){
	    channel_thread = ags_playback_get_channel_thread(playback->data,
							     AGS_SOUND_SCOPE_PLAYBACK);
	
	    task_launcher = ags_channel_thread_get_task_launcher(channel_thread);
	
	    instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								       AGS_SOUND_SCOPE_PLAYBACK,
								       -1,
								       FALSE);
	    ags_task_launcher_add_task(task_launcher,
				       instantiate_vst3_plugin);

	    g_object_unref(channel_thread);

	    g_object_unref(task_launcher);
	  }
	  
	  /*  */
	  if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_SEQUENCER)){
	    channel_thread = ags_playback_get_channel_thread(playback->data,
							     AGS_SOUND_SCOPE_SEQUENCER);

	    task_launcher = ags_channel_thread_get_task_launcher(channel_thread);

	    instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								       AGS_SOUND_SCOPE_SEQUENCER,
								       -1,
								       FALSE);
	    ags_task_launcher_add_task(task_launcher,
				       instantiate_vst3_plugin);

	    g_object_unref(channel_thread);

	    g_object_unref(task_launcher);
	  }
	  
	  /*  */
	  if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_NOTATION)){
	    channel_thread = ags_playback_get_channel_thread(playback->data,
							     AGS_SOUND_SCOPE_NOTATION);

	    task_launcher = ags_channel_thread_get_task_launcher(channel_thread);

	    instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								       AGS_SOUND_SCOPE_NOTATION,
								       -1,
								       FALSE);
	    ags_task_launcher_add_task(task_launcher,
				       instantiate_vst3_plugin);

	    g_object_unref(channel_thread);

	    g_object_unref(task_launcher);
	  }
	  
	  g_object_unref(output);
	}	
      }
    }
    
    list = list->next;
  }
    
  if(playback_domain != NULL){
    g_object_unref(playback_domain);
  }

  g_list_free_full(start_playback,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(start_list,
		   (GDestroyNotify) g_object_unref);
}
#endif

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
  
  application_context = ags_application_context_get_instance();
  
  /* create vst3 bridge */
  vst3_bridge = (AgsVst3Bridge *) ags_machine_util_new_vst3_bridge(filename, effect);
  
  add_audio = ags_add_audio_new(AGS_MACHINE(vst3_bridge)->audio);

  g_signal_connect_after(add_audio, "launch",
			 G_CALLBACK(ags_menu_action_add_vst3_bridge_add_audio_callback), vst3_bridge);

  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
#endif
}

void
ags_menu_action_add_live_dssi_bridge_callback(GtkWidget *menu_item, gpointer data)
{
  AgsLiveDssiBridge *live_dssi_bridge;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  gchar *filename, *effect;

  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);
  
  application_context = ags_application_context_get_instance();
  
  /* create live dssi bridge */
  live_dssi_bridge = (AgsLiveDssiBridge *) ags_machine_util_new_live_dssi_bridge(filename, effect);
  
  add_audio = ags_add_audio_new(AGS_MACHINE(live_dssi_bridge)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_add_live_lv2_bridge_callback(GtkWidget *menu_item, gpointer data)
{
  AgsLiveLv2Bridge *live_lv2_bridge;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;

  gchar *filename, *effect;
    
  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);
  
  application_context = ags_application_context_get_instance();
  
  /* create live lv2 bridge */
  live_lv2_bridge = (AgsLiveLv2Bridge *) ags_machine_util_new_live_lv2_bridge(filename, effect);
    
  add_audio = ags_add_audio_new(AGS_MACHINE(live_lv2_bridge)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

#if defined(AGS_WITH_VST3)
void
ags_menu_action_add_live_vst3_bridge_add_audio_callback(AgsTask *task,
							AgsLiveVst3Bridge *live_vst3_bridge)
{
  AgsAudio *audio;
  AgsChannel *start_output, *output;
  AgsPlaybackDomain *playback_domain;

  AgsInstantiateVst3Plugin *instantiate_vst3_plugin;

  AgsVst3Plugin *vst3_plugin;

  AgsAudioThread *audio_thread;
  AgsChannelThread *channel_thread;
  
  GList *start_playback, *playback;	
  GList *start_play, *start_recall;
  GList *start_list, *list;

  guint audio_channels;

  audio = AGS_MACHINE(live_vst3_bridge)->audio;
  
  vst3_plugin = live_vst3_bridge->vst3_plugin;

  start_output = NULL;
  
  start_play = NULL;
  start_recall = NULL;

  playback_domain = NULL;

  audio_channels = 0;
  
  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       "output", &start_output,
	       "play", &start_play,
	       "recall", &start_recall,
	       "playback-domain", &playback_domain,
	       NULL);

  list = 
    start_list = g_list_concat(start_play,
			       start_recall);

  start_playback = NULL;
	
  g_object_get(playback_domain,
	       "output-playback", &start_playback,
	       NULL);

  while(list != NULL){
    if(AGS_IS_FX_VST3_AUDIO(list->data)){
      AgsTaskLauncher *task_launcher;      

      if(ags_base_plugin_test_flags((AgsBasePlugin *) vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
	/*  */
	if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_PLAYBACK)){
	  audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							      AGS_SOUND_SCOPE_PLAYBACK);

	  task_launcher = ags_audio_thread_get_task_launcher(audio_thread);
	
	  instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								     AGS_SOUND_SCOPE_PLAYBACK,
								     -1,
								     FALSE);
	  ags_task_launcher_add_task(task_launcher,
				     instantiate_vst3_plugin);

	  g_object_unref(audio_thread);

	  g_object_unref(task_launcher);
	}
	
	/*  */
	if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_SEQUENCER)){
	  audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							      AGS_SOUND_SCOPE_SEQUENCER);

	  task_launcher = ags_audio_thread_get_task_launcher(audio_thread);
	
	  instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								     AGS_SOUND_SCOPE_SEQUENCER,
								     -1,
								     FALSE);
	  ags_task_launcher_add_task(task_launcher,
				     instantiate_vst3_plugin);

	  g_object_unref(audio_thread);

	  g_object_unref(task_launcher);
	}
	
	/*  */
	if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_NOTATION)){
	  audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							      AGS_SOUND_SCOPE_NOTATION);

	  task_launcher = ags_audio_thread_get_task_launcher(audio_thread);
	
	  instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								     AGS_SOUND_SCOPE_NOTATION,
								     -1,
								     FALSE);
	  ags_task_launcher_add_task(task_launcher,
				     instantiate_vst3_plugin);

	  g_object_unref(audio_thread);

	  g_object_unref(task_launcher);
	}
      }
    }
    
    list = list->next;
  }
    
  if(playback_domain != NULL){
    g_object_unref(playback_domain);
  }

  g_list_free_full(start_playback,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(start_list,
		   (GDestroyNotify) g_object_unref);
}
#endif

void
ags_menu_action_add_live_vst3_bridge_callback(GtkWidget *menu_item, gpointer data)
{
#if defined(AGS_WITH_VST3)
  AgsLiveVst3Bridge *live_vst3_bridge;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  gchar *filename, *effect;

  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);
  
  application_context = ags_application_context_get_instance();
  
  /* create live vst3 bridge */
  live_vst3_bridge = (AgsLiveVst3Bridge *) ags_machine_util_new_live_vst3_bridge(filename, effect);
  
  add_audio = ags_add_audio_new(AGS_MACHINE(live_vst3_bridge)->audio);

  g_signal_connect_after(add_audio, "launch",
			 G_CALLBACK(ags_menu_action_add_live_vst3_bridge_add_audio_callback), live_vst3_bridge);
  
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
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
ags_menu_action_preferences_callback(GtkWidget *menu_item, gpointer data)
{
  ags_app_action_util_preferences();
}

void
ags_menu_action_midi_import_callback(GtkWidget *menu_item, gpointer data)
{
  AgsApplicationContext *application_context;
  AgsWindow *window;

  application_context = ags_application_context_get_instance();
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  if(window->midi_import_wizard != NULL){
    return;
  }

  window->midi_import_wizard = (GtkWidget *) ags_midi_import_wizard_new();

  ags_connectable_connect(AGS_CONNECTABLE(window->midi_import_wizard));
  ags_applicable_reset(AGS_APPLICABLE(window->midi_import_wizard));

  gtk_widget_show_all(GTK_WIDGET(window->midi_import_wizard));
}

void
ags_menu_action_midi_export_track_callback(GtkWidget *menu_item, gpointer data)
{
  AgsApplicationContext *application_context;
  AgsWindow *window;

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  if(window->midi_export_wizard != NULL){
    return;
  }

  window->midi_export_wizard = (GtkWidget *) ags_midi_export_wizard_new((GtkWidget *) window);

  ags_connectable_connect(AGS_CONNECTABLE(window->midi_export_wizard));
  ags_applicable_reset(AGS_APPLICABLE(window->midi_export_wizard));

  gtk_widget_show_all(GTK_WIDGET(window->midi_export_wizard));
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
