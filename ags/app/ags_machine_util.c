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

#include <ags/app/ags_machine_util.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

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

#include <ags/app/machine/ags_ladspa_bridge.h>
#include <ags/app/machine/ags_dssi_bridge.h>
#include <ags/app/machine/ags_lv2_bridge.h>
#include <ags/app/machine/ags_live_dssi_bridge.h>
#include <ags/app/machine/ags_live_lv2_bridge.h>

#if defined(AGS_WITH_VST3)
#include <ags/app/machine/ags_vst3_bridge.h>
#include <ags/app/machine/ags_live_vst3_bridge.h>
#endif

/**
 * SECTION:ags_machine_util
 * @short_description: machine util
 * @title: AgsMachineUtil
 * @section_id:
 * @include: ags/app/ags_machine_util.h
 *
 * Machine utility functions.
 */

/**
 * ags_machine_util_new_panel:
 * 
 * Create #AgsPanel.
 * 
 * returns: the newly instantiated #AgsPanel
 * 
 * Since: 3.3.1
 */
GtkWidget*
ags_machine_util_new_panel()
{
  AgsWindow *window;
  AgsPanel *panel;

  AgsApplicationContext *application_context;

  GObject *default_soundcard;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create panel */
  panel = ags_panel_new(G_OBJECT(default_soundcard));

  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(panel),
		     FALSE, FALSE,
		     0);

  ags_connectable_connect(AGS_CONNECTABLE(panel));

  ags_audio_set_audio_channels(AGS_MACHINE(panel)->audio,
			       2, 0);
  
  ags_audio_set_pads(AGS_MACHINE(panel)->audio,
		     AGS_TYPE_INPUT,
		     1, 0);
  ags_audio_set_pads(AGS_MACHINE(panel)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);

  gtk_widget_show_all(GTK_WIDGET(panel));

  return((GtkWidget *) panel);
}

/**
 * ags_machine_util_new_mixer:
 * 
 * Create #AgsMixer.
 * 
 * returns: the newly instantiated #AgsMixer
 * 
 * Since: 3.3.1
 */
GtkWidget*
ags_machine_util_new_mixer()
{
  AgsWindow *window;
  AgsMixer *mixer;

  AgsApplicationContext *application_context;
    
  GObject *default_soundcard;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create mixer */
  mixer = ags_mixer_new(G_OBJECT(default_soundcard));

  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(mixer),
		     FALSE, FALSE,
		     0);

  ags_connectable_connect(AGS_CONNECTABLE(mixer));

  ags_audio_set_audio_channels(AGS_MACHINE(mixer)->audio,
			       2, 0);

  ags_audio_set_pads(AGS_MACHINE(mixer)->audio,
		     AGS_TYPE_INPUT,
		     8, 0);
  ags_audio_set_pads(AGS_MACHINE(mixer)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);

  gtk_widget_show_all(GTK_WIDGET(mixer));

  return((GtkWidget *) mixer);
}

/**
 * ags_machine_util_new_spectrometer:
 * 
 * Create #AgsSpectrometer.
 * 
 * returns: the newly instantiated #AgsSpectrometer
 * 
 * Since: 3.3.1
 */
GtkWidget*
ags_machine_util_new_spectrometer()
{
  AgsWindow *window;
  AgsSpectrometer *spectrometer;

  AgsApplicationContext *application_context;
    
  GObject *default_soundcard;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create spectrometer */
  spectrometer = ags_spectrometer_new(G_OBJECT(default_soundcard));

  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(spectrometer),
		     FALSE, FALSE,
		     0);

  ags_connectable_connect(AGS_CONNECTABLE(spectrometer));

  ags_audio_set_audio_channels(AGS_MACHINE(spectrometer)->audio,
			       2, 0);

  ags_audio_set_pads(AGS_MACHINE(spectrometer)->audio,
		     AGS_TYPE_INPUT,
		     1, 0);
  ags_audio_set_pads(AGS_MACHINE(spectrometer)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);

  gtk_widget_show_all(GTK_WIDGET(spectrometer));

  return((GtkWidget *) spectrometer);
}

/**
 * ags_machine_util_new_equalizer:
 * 
 * Create #AgsEqualizer.
 * 
 * returns: the newly instantiated #AgsEqualizer
 * 
 * Since: 3.3.1
 */
GtkWidget*
ags_machine_util_new_equalizer()
{
  AgsWindow *window;
  AgsEqualizer10 *equalizer10;

  AgsApplicationContext *application_context;
    
  GObject *default_soundcard;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create equalizer10 */
  equalizer10 = ags_equalizer10_new(G_OBJECT(default_soundcard));

  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(equalizer10),
		     FALSE, FALSE,
		     0);

  ags_connectable_connect(AGS_CONNECTABLE(equalizer10));

  ags_audio_set_audio_channels(AGS_MACHINE(equalizer10)->audio,
			       2, 0);

  ags_audio_set_pads(AGS_MACHINE(equalizer10)->audio,
		     AGS_TYPE_INPUT,
		     1, 0);
  ags_audio_set_pads(AGS_MACHINE(equalizer10)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);

  gtk_widget_show_all(GTK_WIDGET(equalizer10));

  return((GtkWidget *) equalizer10);
}

/**
 * ags_machine_util_new_drum:
 * 
 * Create #AgsDrum.
 * 
 * returns: the newly instantiated #AgsDrum
 * 
 * Since: 3.3.1
 */
GtkWidget*
ags_machine_util_new_drum()
{
  AgsWindow *window;
  AgsDrum *drum;

  AgsApplicationContext *application_context;
    
  GObject *default_soundcard;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create drum */
  drum = ags_drum_new(G_OBJECT(default_soundcard));

  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(drum),
		     FALSE, FALSE,
		     0);

  /* connect everything */
  ags_connectable_connect(AGS_CONNECTABLE(drum));

  /* */
  ags_audio_set_audio_channels(AGS_MACHINE(drum)->audio,
			       2, 0);

  /* AgsDrumInputPad */
  ags_audio_set_pads(AGS_MACHINE(drum)->audio,
		     AGS_TYPE_INPUT,
		     8, 0);
  ags_audio_set_pads(AGS_MACHINE(drum)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);

  /* */
  gtk_widget_show_all(GTK_WIDGET(drum));

  return((GtkWidget *) drum);
}

/**
 * ags_machine_util_new_matrix:
 * 
 * Create #AgsMatrix.
 * 
 * returns: the newly instantiated #AgsMatrix
 * 
 * Since: 3.3.1
 */
GtkWidget*
ags_machine_util_new_matrix()
{
  AgsWindow *window;
  AgsMatrix *matrix;

  AgsApplicationContext *application_context;

  GObject *default_soundcard;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create matrix */
  matrix = ags_matrix_new(G_OBJECT(default_soundcard));

  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(matrix),
		     FALSE, FALSE,
		     0);
  
  /* connect everything */
  ags_connectable_connect(AGS_CONNECTABLE(matrix));

  /* */
  ags_audio_set_audio_channels(AGS_MACHINE(matrix)->audio,
			       1, 0);

  /* AgsMatrixInputPad */
  ags_audio_set_pads(AGS_MACHINE(matrix)->audio,
		     AGS_TYPE_INPUT,
		     78, 0);
  ags_audio_set_pads(AGS_MACHINE(matrix)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);

  /* */
  gtk_widget_show_all(GTK_WIDGET(matrix));

  return((GtkWidget *) matrix);
}

/**
 * ags_machine_util_new_synth:
 * 
 * Create #AgsSynth.
 * 
 * returns: the newly instantiated #AgsSynth
 * 
 * Since: 3.3.1
 */
GtkWidget*
ags_machine_util_new_synth()
{
  AgsWindow *window;
  AgsSynth *synth;

  AgsApplicationContext *application_context;
    
  GObject *default_soundcard;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create synth */
  synth = ags_synth_new(G_OBJECT(default_soundcard));

  gtk_box_pack_start((GtkBox *) window->machines,
		     (GtkWidget *) synth,
		     FALSE, FALSE,
		     0);

  ags_connectable_connect(AGS_CONNECTABLE(synth));

  ags_audio_set_audio_channels(AGS_MACHINE(synth)->audio,
			       1, 0);

  ags_audio_set_pads(AGS_MACHINE(synth)->audio,
		     AGS_TYPE_INPUT,
		     2, 0);
  ags_audio_set_pads(AGS_MACHINE(synth)->audio,
		     AGS_TYPE_OUTPUT,
		     78, 0);

  gtk_widget_show_all((GtkWidget *) synth);

  return((GtkWidget *) synth);  
}

/**
 * ags_machine_util_new_fm_synth:
 * 
 * Create #AgsFmSynth.
 * 
 * returns: the newly instantiated #AgsFmSynth
 * 
 * Since: 3.3.1
 */
GtkWidget*
ags_machine_util_new_fm_synth()
{
  AgsWindow *window;
  AgsFMSynth *fm_synth;

  AgsApplicationContext *application_context;
    
  GObject *default_soundcard;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create fm_synth */
  fm_synth = ags_fm_synth_new(G_OBJECT(default_soundcard));

  gtk_box_pack_start((GtkBox *) window->machines,
		     (GtkWidget *) fm_synth,
		     FALSE, FALSE,
		     0);

  ags_connectable_connect(AGS_CONNECTABLE(fm_synth));

  ags_audio_set_audio_channels(AGS_MACHINE(fm_synth)->audio,
			       1, 0);

  ags_audio_set_pads(AGS_MACHINE(fm_synth)->audio,
		     AGS_TYPE_INPUT,
		     2, 0);
  ags_audio_set_pads(AGS_MACHINE(fm_synth)->audio,
		     AGS_TYPE_OUTPUT,
		     78, 0);

  gtk_widget_show_all((GtkWidget *) fm_synth);

  return((GtkWidget *) fm_synth);  
}

/**
 * ags_machine_util_new_syncsynth:
 * 
 * Create #AgsSyncsynth.
 * 
 * returns: the newly instantiated #AgsSyncsynth
 * 
 * Since: 3.3.1
 */
GtkWidget*
ags_machine_util_new_syncsynth()
{
  AgsWindow *window;
  AgsSyncsynth *syncsynth;

  AgsApplicationContext *application_context;
  
  GObject *default_soundcard;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create syncsynth */
  syncsynth = ags_syncsynth_new(G_OBJECT(default_soundcard));

  gtk_box_pack_start((GtkBox *) window->machines,
		     (GtkWidget *) syncsynth,
		     FALSE, FALSE,
		     0);

  ags_connectable_connect(AGS_CONNECTABLE(syncsynth));

  ags_audio_set_audio_channels(AGS_MACHINE(syncsynth)->audio,
			       1, 0);
  
  ags_audio_set_pads(AGS_MACHINE(syncsynth)->audio,
		     AGS_TYPE_INPUT,
		     78, 0);
  ags_audio_set_pads(AGS_MACHINE(syncsynth)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);

  gtk_widget_show_all((GtkWidget *) syncsynth);

  return((GtkWidget *) syncsynth);  
}

/**
 * ags_machine_util_new_fm_syncsynth:
 * 
 * Create #AgsFmSyncsynth.
 * 
 * returns: the newly instantiated #AgsFmSyncsynth
 * 
 * Since: 3.3.1
 */
GtkWidget*
ags_machine_util_new_fm_syncsynth()
{
  AgsWindow *window;
  AgsFMSyncsynth *fm_syncsynth;

  AgsApplicationContext *application_context;
  
  GObject *default_soundcard;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create fm_syncsynth */
  fm_syncsynth = ags_fm_syncsynth_new(G_OBJECT(default_soundcard));

  gtk_box_pack_start((GtkBox *) window->machines,
		     (GtkWidget *) fm_syncsynth,
		     FALSE, FALSE,
		     0);

  ags_connectable_connect(AGS_CONNECTABLE(fm_syncsynth));

  ags_audio_set_audio_channels(AGS_MACHINE(fm_syncsynth)->audio,
			       1, 0);
  
  ags_audio_set_pads(AGS_MACHINE(fm_syncsynth)->audio,
		     AGS_TYPE_INPUT,
		     78, 0);
  ags_audio_set_pads(AGS_MACHINE(fm_syncsynth)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);

  gtk_widget_show_all((GtkWidget *) fm_syncsynth);

  return((GtkWidget *) fm_syncsynth);  
}

/**
 * ags_machine_util_new_hybrid_synth:
 * 
 * Create #AgsHybridSynth.
 * 
 * returns: the newly instantiated #AgsHybridSynth
 * 
 * Since: 3.14.0
 */
GtkWidget*
ags_machine_util_new_hybrid_synth()
{
  AgsWindow *window;
  AgsHybridSynth *hybrid_synth;

  AgsApplicationContext *application_context;
  
  GObject *default_soundcard;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create hybrid_synth */
  hybrid_synth = ags_hybrid_synth_new(G_OBJECT(default_soundcard));

  gtk_box_pack_start((GtkBox *) window->machines,
		     (GtkWidget *) hybrid_synth,
		     FALSE, FALSE,
		     0);

  ags_connectable_connect(AGS_CONNECTABLE(hybrid_synth));

  ags_audio_set_audio_channels(AGS_MACHINE(hybrid_synth)->audio,
			       1, 0);
  
  ags_audio_set_pads(AGS_MACHINE(hybrid_synth)->audio,
		     AGS_TYPE_INPUT,
		     128, 0);
  ags_audio_set_pads(AGS_MACHINE(hybrid_synth)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);

  gtk_widget_show_all((GtkWidget *) hybrid_synth);

  return((GtkWidget *) hybrid_synth);  
}

/**
 * ags_machine_util_new_hybrid_fm_synth:
 * 
 * Create #AgsHybridFMSynth.
 * 
 * returns: the newly instantiated #AgsHybridFMSynth
 * 
 * Since: 3.15.0
 */
GtkWidget*
ags_machine_util_new_hybrid_fm_synth()
{
  AgsWindow *window;
  AgsHybridFMSynth *hybrid_fm_synth;

  AgsApplicationContext *application_context;
  
  GObject *default_soundcard;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create hybrid_fm_synth */
  hybrid_fm_synth = ags_hybrid_fm_synth_new(G_OBJECT(default_soundcard));

  gtk_box_pack_start((GtkBox *) window->machines,
		     (GtkWidget *) hybrid_fm_synth,
		     FALSE, FALSE,
		     0);

  ags_connectable_connect(AGS_CONNECTABLE(hybrid_fm_synth));

  ags_audio_set_audio_channels(AGS_MACHINE(hybrid_fm_synth)->audio,
			       1, 0);
  
  ags_audio_set_pads(AGS_MACHINE(hybrid_fm_synth)->audio,
		     AGS_TYPE_INPUT,
		     128, 0);
  ags_audio_set_pads(AGS_MACHINE(hybrid_fm_synth)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);

  gtk_widget_show_all((GtkWidget *) hybrid_fm_synth);

  return((GtkWidget *) hybrid_fm_synth);  
}

/**
 * ags_machine_util_new_ffplayer:
 * 
 * Create #AgsFFPlayer.
 * 
 * returns: the newly instantiated #AgsFFPlayer
 * 
 * Since: 3.3.1
 */
GtkWidget*
ags_machine_util_new_ffplayer()
{
#ifdef AGS_WITH_LIBINSTPATCH
  AgsWindow *window;
  AgsFFPlayer *ffplayer;

  AgsApplicationContext *application_context;
  
  GObject *default_soundcard;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create ffplayer */
  ffplayer = ags_ffplayer_new(G_OBJECT(default_soundcard));

  gtk_box_pack_start((GtkBox *) window->machines,
		     (GtkWidget *) ffplayer,
		     FALSE, FALSE,
		     0);

  ags_connectable_connect(AGS_CONNECTABLE(ffplayer));

  ags_audio_set_audio_channels(AGS_MACHINE(ffplayer)->audio,
			       2, 0);
  
  ags_audio_set_pads(AGS_MACHINE(ffplayer)->audio,
		     AGS_TYPE_INPUT,
		     78, 0);
  ags_audio_set_pads(AGS_MACHINE(ffplayer)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);  

  gtk_widget_show_all((GtkWidget *) ffplayer);

  return((GtkWidget *) ffplayer);
#else
  return(NULL);
#endif
}

/**
 * ags_machine_util_new_sf2_synth:
 * 
 * Create #AgsSF2Synth.
 * 
 * returns: the newly instantiated #AgsSF2Synth
 * 
 * Since: 3.3.4
 */
GtkWidget*
ags_machine_util_new_sf2_synth()
{
#ifdef AGS_WITH_LIBINSTPATCH
  AgsWindow *window;
  AgsSF2Synth *sf2_synth;

  AgsApplicationContext *application_context;
  
  GObject *default_soundcard;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create pitch sampler */
  sf2_synth = ags_sf2_synth_new(G_OBJECT(default_soundcard));

  gtk_box_pack_start((GtkBox *) window->machines,
		     (GtkWidget *) sf2_synth,
		     FALSE, FALSE,
		     0);

  ags_connectable_connect(AGS_CONNECTABLE(sf2_synth));

  ags_audio_set_audio_channels(AGS_MACHINE(sf2_synth)->audio,
			       1, 0);
  
  ags_audio_set_pads(AGS_MACHINE(sf2_synth)->audio,
		     AGS_TYPE_INPUT,
		     128, 0);
  ags_audio_set_pads(AGS_MACHINE(sf2_synth)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);  

  gtk_widget_show_all((GtkWidget *) sf2_synth);

  return((GtkWidget *) sf2_synth);
#else
  return(NULL);
#endif
}

/**
 * ags_machine_util_new_pitch_sampler:
 * 
 * Create #AgsPitchSampler.
 * 
 * returns: the newly instantiated #AgsPitchSampler
 * 
 * Since: 3.3.1
 */
GtkWidget*
ags_machine_util_new_pitch_sampler()
{
  AgsWindow *window;
  AgsPitchSampler *pitch_sampler;

  AgsApplicationContext *application_context;
  
  GObject *default_soundcard;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create pitch sampler */
  pitch_sampler = ags_pitch_sampler_new(G_OBJECT(default_soundcard));

  gtk_box_pack_start((GtkBox *) window->machines,
		     (GtkWidget *) pitch_sampler,
		     FALSE, FALSE,
		     0);

  ags_connectable_connect(AGS_CONNECTABLE(pitch_sampler));

  ags_audio_set_audio_channels(AGS_MACHINE(pitch_sampler)->audio,
			       2, 0);
  
  ags_audio_set_pads(AGS_MACHINE(pitch_sampler)->audio,
		     AGS_TYPE_INPUT,
		     78, 0);
  ags_audio_set_pads(AGS_MACHINE(pitch_sampler)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);  

  gtk_widget_show_all((GtkWidget *) pitch_sampler);

  return((GtkWidget *) pitch_sampler);
}

/**
 * ags_machine_util_new_sfz_synth:
 * 
 * Create #AgsSFZSynth.
 * 
 * returns: the newly instantiated #AgsSFZSynth
 * 
 * Since: 3.3.4
 */
GtkWidget*
ags_machine_util_new_sfz_synth()
{
  AgsWindow *window;
  AgsSFZSynth *sfz_synth;

  AgsApplicationContext *application_context;
  
  GObject *default_soundcard;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create pitch sampler */
  sfz_synth = ags_sfz_synth_new(G_OBJECT(default_soundcard));

  gtk_box_pack_start((GtkBox *) window->machines,
		     (GtkWidget *) sfz_synth,
		     FALSE, FALSE,
		     0);

  ags_connectable_connect(AGS_CONNECTABLE(sfz_synth));

  ags_audio_set_audio_channels(AGS_MACHINE(sfz_synth)->audio,
			       1, 0);
  
  ags_audio_set_pads(AGS_MACHINE(sfz_synth)->audio,
		     AGS_TYPE_INPUT,
		     128, 0);
  ags_audio_set_pads(AGS_MACHINE(sfz_synth)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);  

  gtk_widget_show_all((GtkWidget *) sfz_synth);

  return((GtkWidget *) sfz_synth);
}

/**
 * ags_machine_util_new_audiorec:
 * 
 * Create #AgsAudiorec.
 * 
 * returns: the newly instantiated #AgsAudiorec
 * 
 * Since: 3.3.1
 */
GtkWidget*
ags_machine_util_new_audiorec()
{
  AgsWindow *window;
  AgsAudiorec *audiorec;

  AgsApplicationContext *application_context;
  
  GObject *default_soundcard;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create audiorec */
  audiorec = ags_audiorec_new(G_OBJECT(default_soundcard));

  gtk_box_pack_start((GtkBox *) window->machines,
		     (GtkWidget *) audiorec,
		     FALSE, FALSE,
		     0);

  ags_connectable_connect(AGS_CONNECTABLE(audiorec));

  ags_audio_set_audio_channels(AGS_MACHINE(audiorec)->audio,
			       2, 0);
  
  ags_audio_set_pads(AGS_MACHINE(audiorec)->audio,
		     AGS_TYPE_INPUT,
		     1, 0);
  ags_audio_set_pads(AGS_MACHINE(audiorec)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);  

  gtk_widget_show_all((GtkWidget *) audiorec);

  return((GtkWidget *) audiorec);
}

/**
 * ags_machine_util_new_desk:
 * 
 * Create #AgsDesk.
 * 
 * returns: the newly instantiated #AgsDesk
 * 
 * Since: 3.7.0
 */
GtkWidget*
ags_machine_util_new_desk()
{
  AgsWindow *window;
  AgsDesk *desk;

  AgsApplicationContext *application_context;
  
  GObject *default_soundcard;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create desk */
  desk = ags_desk_new(G_OBJECT(default_soundcard));

  gtk_box_pack_start((GtkBox *) window->machines,
		     (GtkWidget *) desk,
		     FALSE, FALSE,
		     0);

  ags_connectable_connect(AGS_CONNECTABLE(desk));

  ags_audio_set_audio_channels(AGS_MACHINE(desk)->audio,
			       2, 0);
  
  ags_audio_set_pads(AGS_MACHINE(desk)->audio,
		     AGS_TYPE_INPUT,
		     1, 0);
  ags_audio_set_pads(AGS_MACHINE(desk)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);  

  gtk_widget_show_all((GtkWidget *) desk);

  return((GtkWidget *) desk);
}

/**
 * ags_machine_util_new_ladspa_bridge:
 * @filename: the filename
 * @effect: the effect
 * 
 * Create #AgsLadspaBridge.
 * 
 * returns: the newly instantiated #AgsLadspaBridge
 * 
 * Since: 3.3.1
 */
GtkWidget*
ags_machine_util_new_ladspa_bridge(gchar *filename, gchar *effect)
{
  AgsWindow *window;
  AgsLadspaBridge *ladspa_bridge;

  AgsApplicationContext *application_context;

  GObject *default_soundcard;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create ladspa bridge */
  ladspa_bridge = ags_ladspa_bridge_new(G_OBJECT(default_soundcard),
					filename,
					effect);
  
  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(ladspa_bridge),
		     FALSE, FALSE, 0);

  /* connect everything */
  ags_connectable_connect(AGS_CONNECTABLE(ladspa_bridge));

  /* */
  ags_audio_set_audio_channels(AGS_MACHINE(ladspa_bridge)->audio,
			       2, 0);

  /*  */
  ags_audio_set_pads(AGS_MACHINE(ladspa_bridge)->audio,
		     AGS_TYPE_INPUT,
		     1, 0);
  ags_audio_set_pads(AGS_MACHINE(ladspa_bridge)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);

  /* */
  gtk_widget_show_all(GTK_WIDGET(ladspa_bridge));

  return((GtkWidget *) ladspa_bridge);
}

/**
 * ags_machine_util_new_dssi_bridge:
 * @filename: the filename
 * @effect: the effect
 * 
 * Create #AgsDssiBridge.
 * 
 * returns: the newly instantiated #AgsDssiBridge
 * 
 * Since: 3.3.1
 */
GtkWidget*
ags_machine_util_new_dssi_bridge(gchar *filename, gchar *effect)
{
  AgsWindow *window;
  AgsDssiBridge *dssi_bridge;

  AgsApplicationContext *application_context;

  GObject *default_soundcard;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create dssi bridge */
  dssi_bridge = ags_dssi_bridge_new(G_OBJECT(default_soundcard),
				    filename,
				    effect);
  
  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(dssi_bridge),
		     FALSE, FALSE, 0);

  /* connect everything */
  ags_connectable_connect(AGS_CONNECTABLE(dssi_bridge));
  
  /*  */
  ags_audio_set_audio_channels(AGS_MACHINE(dssi_bridge)->audio,
			       2, 0);

  ags_audio_set_pads(AGS_MACHINE(dssi_bridge)->audio,
		     AGS_TYPE_INPUT,
		     128, 0);
  ags_audio_set_pads(AGS_MACHINE(dssi_bridge)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);

  /*  */
  ags_dssi_bridge_load(dssi_bridge);

  /* */
  gtk_widget_show_all(GTK_WIDGET(dssi_bridge));

  return((GtkWidget *) dssi_bridge);
}

/**
 * ags_machine_util_new_vst3_bridge:
 * @filename: the filename
 * @effect: the effect
 * 
 * Create #AgsVst3Bridge.
 * 
 * returns: the newly instantiated #AgsVst3Bridge
 * 
 * Since: 3.10.5
 */
GtkWidget*
ags_machine_util_new_vst3_bridge(gchar *filename, gchar *effect)
{
#if defined(AGS_WITH_VST3)
  AgsWindow *window;
  AgsVst3Bridge *vst3_bridge;

  AgsApplicationContext *application_context;

  AgsVst3Plugin *vst3_plugin;

  GObject *default_soundcard;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create vst3 bridge */
  vst3_plugin = ags_vst3_manager_find_vst3_plugin(ags_vst3_manager_get_instance(),
						  filename, effect);

  if(vst3_plugin == NULL){
    return(NULL);
  }

  vst3_bridge = ags_vst3_bridge_new(G_OBJECT(default_soundcard),
				    filename,
				    effect);
  
  if(vst3_plugin != NULL &&
     ags_base_plugin_test_flags(vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    ags_audio_set_flags(AGS_MACHINE(vst3_bridge)->audio, (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
							  AGS_AUDIO_INPUT_HAS_RECYCLING |
							  AGS_AUDIO_SYNC |
							  AGS_AUDIO_ASYNC));
    ags_audio_set_ability_flags(AGS_MACHINE(vst3_bridge)->audio, (AGS_SOUND_ABILITY_NOTATION));
    ags_audio_set_behaviour_flags(AGS_MACHINE(vst3_bridge)->audio, (AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT |
								    AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING));
    
    g_object_set(AGS_MACHINE(vst3_bridge)->audio,
		 "max-input-pads", 128,
		 "audio-start-mapping", 0,
		 "audio-end-mapping", 128,
		 "midi-start-mapping", 0,
		 "midi-end-mapping", 128,
		 NULL);
    
    AGS_MACHINE(vst3_bridge)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
					AGS_MACHINE_REVERSE_NOTATION);

    ags_machine_popup_add_connection_options((AgsMachine *) vst3_bridge,
					     (AGS_MACHINE_POPUP_MIDI_DIALOG));

    ags_machine_popup_add_edit_options((AgsMachine *) vst3_bridge,
				       (AGS_MACHINE_POPUP_ENVELOPE));
  }

  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(vst3_bridge),
		     FALSE, FALSE, 0);

  /* connect everything */
  ags_connectable_connect(AGS_CONNECTABLE(vst3_bridge));
  
  /*  */
  ags_audio_set_audio_channels(AGS_MACHINE(vst3_bridge)->audio,
			       2, 0);

  if(vst3_plugin != NULL){
    if(!ags_base_plugin_test_flags(vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
      ags_audio_set_pads(AGS_MACHINE(vst3_bridge)->audio,
			 AGS_TYPE_INPUT,
			 1, 0);
    }else{
      ags_audio_set_pads(AGS_MACHINE(vst3_bridge)->audio,
			 AGS_TYPE_INPUT,
			 128, 0);
    }
  }

  ags_audio_set_pads(AGS_MACHINE(vst3_bridge)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);

  /*  */
  ags_vst3_bridge_load(vst3_bridge);

  /* */
  gtk_widget_show_all(GTK_WIDGET(vst3_bridge));

  return((GtkWidget *) vst3_bridge);
#else
  return(NULL);
#endif
}

/**
 * ags_machine_util_new_lv2_bridge:
 * @filename: the filename
 * @effect: the effect
 * 
 * Create #AgsLv2Bridge.
 * 
 * returns: the newly instantiated #AgsLv2Bridge
 * 
 * Since: 3.3.1
 */
GtkWidget*
ags_machine_util_new_lv2_bridge(gchar *filename, gchar *effect)
{
  AgsWindow *window;
  AgsLv2Bridge *lv2_bridge;

  AgsApplicationContext *application_context;

  AgsLv2Plugin *lv2_plugin;

  GObject *default_soundcard;

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  if(filename != NULL &&
     effect != NULL){
    AgsTurtle *manifest;
    AgsTurtleManager *turtle_manager;
    
    gchar *path;
    gchar *manifest_filename;

    turtle_manager = ags_turtle_manager_get_instance();
    
    path = g_path_get_dirname(filename);

    manifest_filename = g_strdup_printf("%s%c%s",
					path,
					G_DIR_SEPARATOR,
					"manifest.ttl");

    manifest = (AgsTurtle *) ags_turtle_manager_find(turtle_manager,
						     manifest_filename);

    if(manifest == NULL){
      AgsLv2TurtleParser *lv2_turtle_parser;
	
      AgsTurtle **turtle;

      guint n_turtle;

      if(!g_file_test(manifest_filename,
		      G_FILE_TEST_EXISTS)){
	return(NULL);
      }

      g_message("new turtle [Manifest] - %s", manifest_filename);
	
      manifest = ags_turtle_new(manifest_filename);
      ags_turtle_load(manifest,
		      NULL);
      ags_turtle_manager_add(turtle_manager,
			     (GObject *) manifest);

      lv2_turtle_parser = ags_lv2_turtle_parser_new(manifest);

      n_turtle = 1;
      turtle = (AgsTurtle **) malloc(2 * sizeof(AgsTurtle *));

      turtle[0] = manifest;
      turtle[1] = NULL;
	
      ags_lv2_turtle_parser_parse(lv2_turtle_parser,
				  turtle, n_turtle);
    
      g_object_run_dispose((GObject *) lv2_turtle_parser);
      g_object_unref(lv2_turtle_parser);
	
      g_object_unref(manifest);
	
      free(turtle);
    }
    
    g_free(manifest_filename);
  }
  
  /* create lv2 bridge */    
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       filename, effect);

  if(lv2_plugin == NULL){
    return(NULL);
  }
  
  lv2_bridge = ags_lv2_bridge_new(G_OBJECT(default_soundcard),
				  filename,
				  effect);
  
  if(lv2_plugin != NULL &&
     (AGS_LV2_PLUGIN_IS_SYNTHESIZER & (lv2_plugin->flags)) != 0){
    ags_audio_set_flags(AGS_MACHINE(lv2_bridge)->audio, (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
							 AGS_AUDIO_INPUT_HAS_RECYCLING |
							 AGS_AUDIO_SYNC |
							 AGS_AUDIO_ASYNC));
    ags_audio_set_ability_flags(AGS_MACHINE(lv2_bridge)->audio, (AGS_SOUND_ABILITY_NOTATION));
    ags_audio_set_behaviour_flags(AGS_MACHINE(lv2_bridge)->audio, (AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT |
								   AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING));
    
    g_object_set(AGS_MACHINE(lv2_bridge)->audio,
		 "max-input-pads", 128,
		 "audio-start-mapping", 0,
		 "audio-end-mapping", 128,
		 "midi-start-mapping", 0,
		 "midi-end-mapping", 128,
		 NULL);
    
    AGS_MACHINE(lv2_bridge)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
				       AGS_MACHINE_REVERSE_NOTATION);

    ags_machine_popup_add_connection_options((AgsMachine *) lv2_bridge,
					     (AGS_MACHINE_POPUP_MIDI_DIALOG));

    ags_machine_popup_add_edit_options((AgsMachine *) lv2_bridge,
				       (AGS_MACHINE_POPUP_ENVELOPE));
  }

  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(lv2_bridge),
		     FALSE, FALSE, 0);

  /* connect everything */
  ags_connectable_connect(AGS_CONNECTABLE(lv2_bridge));
  
  /*  */
  ags_audio_set_audio_channels(AGS_MACHINE(lv2_bridge)->audio,
			       2, 0);

  /*  */
  if(lv2_plugin != NULL){
    if((AGS_LV2_PLUGIN_IS_SYNTHESIZER & (lv2_plugin->flags)) == 0){
      ags_audio_set_pads(AGS_MACHINE(lv2_bridge)->audio,
			 AGS_TYPE_INPUT,
			 1, 0);
    }else{
      ags_audio_set_pads(AGS_MACHINE(lv2_bridge)->audio,
			 AGS_TYPE_INPUT,
			 128, 0);
    }
  }
  
  ags_audio_set_pads(AGS_MACHINE(lv2_bridge)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);

  /*  */
  ags_lv2_bridge_load(lv2_bridge);

  /* */
  gtk_widget_show_all(GTK_WIDGET(lv2_bridge));

  return((GtkWidget *) lv2_bridge);
}

/**
 * ags_machine_util_new_live_dssi_bridge:
 * @filename: the filename
 * @effect: the effect
 * 
 * Create #AgsLiveDssiBridge.
 * 
 * returns: the newly instantiated #AgsLiveDssiBridge
 * 
 * Since: 3.3.1
 */
GtkWidget*
ags_machine_util_new_live_dssi_bridge(gchar *filename, gchar *effect)
{
  AgsWindow *window;
  AgsLiveDssiBridge *live_dssi_bridge;

  AgsApplicationContext *application_context;

  GObject *default_soundcard;

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create live dssi bridge */
  live_dssi_bridge = ags_live_dssi_bridge_new(G_OBJECT(default_soundcard),
					      filename,
					      effect);
  
  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(live_dssi_bridge),
		     FALSE, FALSE, 0);
  
  /* connect everything */
  ags_connectable_connect(AGS_CONNECTABLE(live_dssi_bridge));

  /* */
  ags_audio_set_audio_channels(AGS_MACHINE(live_dssi_bridge)->audio,
			       2, 0);

  /*  */
  ags_audio_set_pads(AGS_MACHINE(live_dssi_bridge)->audio,
		     AGS_TYPE_INPUT,
		     128, 0);
  ags_audio_set_pads(AGS_MACHINE(live_dssi_bridge)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);

  /*  */
  ags_live_dssi_bridge_load(live_dssi_bridge);

  /* */
  gtk_widget_show_all(GTK_WIDGET(live_dssi_bridge));

  return((GtkWidget *) live_dssi_bridge);
}

/**
 * ags_machine_util_new_live_lv2_bridge:
 * @filename: the filename
 * @effect: the effect
 * 
 * Create #AgsLiveLv2Bridge.
 * 
 * returns: the newly instantiated #AgsLiveLv2Bridge
 * 
 * Since: 3.3.1
 */
GtkWidget*
ags_machine_util_new_live_lv2_bridge(gchar *filename, gchar *effect)
{
  AgsWindow *window;
  AgsLiveLv2Bridge *live_lv2_bridge;

  AgsApplicationContext *application_context;

  AgsLv2Plugin *lv2_plugin;

  GObject *default_soundcard;
    
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create live lv2 bridge */    
  if(filename != NULL &&
     effect != NULL){
    AgsTurtle *manifest;
    AgsTurtleManager *turtle_manager;
    
    gchar *path;
    gchar *manifest_filename;

    turtle_manager = ags_turtle_manager_get_instance();
    
    path = g_path_get_dirname(filename);

    manifest_filename = g_strdup_printf("%s%c%s",
					path,
					G_DIR_SEPARATOR,
					"manifest.ttl");

    manifest = (AgsTurtle *) ags_turtle_manager_find(turtle_manager,
						     manifest_filename);

    if(manifest == NULL){
      AgsLv2TurtleParser *lv2_turtle_parser;
	
      AgsTurtle **turtle;

      guint n_turtle;

      if(!g_file_test(manifest_filename,
		      G_FILE_TEST_EXISTS)){
	return(NULL);
      }

      g_message("new turtle [Manifest] - %s", manifest_filename);
	
      manifest = ags_turtle_new(manifest_filename);
      ags_turtle_load(manifest,
		      NULL);
      ags_turtle_manager_add(turtle_manager,
			     (GObject *) manifest);

      lv2_turtle_parser = ags_lv2_turtle_parser_new(manifest);

      n_turtle = 1;
      turtle = (AgsTurtle **) malloc(2 * sizeof(AgsTurtle *));

      turtle[0] = manifest;
      turtle[1] = NULL;
	
      ags_lv2_turtle_parser_parse(lv2_turtle_parser,
				  turtle, n_turtle);
    
      g_object_run_dispose((GObject *) lv2_turtle_parser);
      g_object_unref(lv2_turtle_parser);
	
      g_object_unref(manifest);
	
      free(turtle);
    }
    
    g_free(manifest_filename);
  }

  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       filename, effect);

  if(lv2_plugin == NULL){
    return(NULL);
  }
  
  live_lv2_bridge = ags_live_lv2_bridge_new(G_OBJECT(default_soundcard),
					    filename,
					    effect);
  
  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(live_lv2_bridge),
		     FALSE, FALSE, 0);

  /* connect everything */
  ags_connectable_connect(AGS_CONNECTABLE(live_lv2_bridge));
  
  /*  */
  ags_audio_set_audio_channels(AGS_MACHINE(live_lv2_bridge)->audio,
			       2, 0);

  /*  */
  if(lv2_plugin != NULL){
    if((AGS_LV2_PLUGIN_IS_SYNTHESIZER & (lv2_plugin->flags)) == 0){
      ags_audio_set_pads(AGS_MACHINE(live_lv2_bridge)->audio,
			 AGS_TYPE_INPUT,
			 1, 0);
    }else{
      ags_audio_set_pads(AGS_MACHINE(live_lv2_bridge)->audio,
			 AGS_TYPE_INPUT,
			 128, 0);
    }
  }
  
  ags_audio_set_pads(AGS_MACHINE(live_lv2_bridge)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);

  /*  */
  ags_live_lv2_bridge_load(live_lv2_bridge);

  /* */
  gtk_widget_show_all(GTK_WIDGET(live_lv2_bridge));

  return((GtkWidget *) live_lv2_bridge);
}

/**
 * ags_machine_util_new_live_vst3_bridge:
 * @filename: the filename
 * @effect: the effect
 * 
 * Create #AgsLiveVst3Bridge.
 * 
 * returns: the newly instantiated #AgsLiveVst3Bridge
 * 
 * Since: 3.10.5
 */
GtkWidget*
ags_machine_util_new_live_vst3_bridge(gchar *filename, gchar *effect)
{
#if defined(AGS_WITH_VST3)
  AgsWindow *window;
  AgsLiveVst3Bridge *live_vst3_bridge;

  AgsApplicationContext *application_context;

  AgsVst3Plugin *vst3_plugin;

  GObject *default_soundcard;

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create live vst3 bridge */  
  vst3_plugin = ags_vst3_manager_find_vst3_plugin(ags_vst3_manager_get_instance(),
						  filename, effect);

  if(vst3_plugin == NULL){
    return(NULL);
  }

  live_vst3_bridge = ags_live_vst3_bridge_new(G_OBJECT(default_soundcard),
					      filename,
					      effect);
  
  live_vst3_bridge->vst3_plugin = vst3_plugin;
  
  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(live_vst3_bridge),
		     FALSE, FALSE, 0);
  
  /* connect everything */
  ags_connectable_connect(AGS_CONNECTABLE(live_vst3_bridge));

  /* */
  ags_audio_set_audio_channels(AGS_MACHINE(live_vst3_bridge)->audio,
			       2, 0);

  /*  */
  if(vst3_plugin != NULL){
    if(!ags_base_plugin_test_flags(vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
      ags_audio_set_pads(AGS_MACHINE(live_vst3_bridge)->audio,
			 AGS_TYPE_INPUT,
			 1, 0);
    }else{
      ags_audio_set_pads(AGS_MACHINE(live_vst3_bridge)->audio,
			 AGS_TYPE_INPUT,
			 128, 0);
    }
  }

  ags_audio_set_pads(AGS_MACHINE(live_vst3_bridge)->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);

  /*  */
  ags_live_vst3_bridge_load(live_vst3_bridge);

  /* */
  gtk_widget_show_all(GTK_WIDGET(live_vst3_bridge));

  return((GtkWidget *) live_vst3_bridge);
#else
  return(NULL);
#endif
}

/**
 * ags_machine_util_new_by_type_name:
 * @machine_type_name: the machine type name
 * @filename: the filename
 * @effect: the effect
 * 
 * Create #AgsLiveLv2Bridge.
 * 
 * returns: the newly instantiated #Machine implementation
 * 
 * Since: 3.3.1
 */
GtkWidget*
ags_machine_util_new_by_type_name(gchar *machine_type_name,
				  gchar *filename, gchar *effect)
{
  GtkWidget *machine;

  machine = NULL;

  if(!g_ascii_strncasecmp(machine_type_name,
			  "AgsPanel",
			  9)){
    machine = ags_machine_util_new_panel();
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsMixer",
				9)){
    machine = ags_machine_util_new_mixer();
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsSpectrometer",
				16)){
    machine = ags_machine_util_new_spectrometer();
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsEqualizer10",
				15)){
    machine = ags_machine_util_new_equalizer();
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsDrum",
				8)){
    machine = ags_machine_util_new_drum();
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsMatrix",
				10)){
    machine = ags_machine_util_new_matrix();
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsSynth",
				9)){
    machine = ags_machine_util_new_synth();
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsFmSynth",
				11)){
    machine = ags_machine_util_new_fm_synth();
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsSyncsynth",
				13)){
    machine = ags_machine_util_new_syncsynth();
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsFmSyncsynth",
				15)){
    machine = ags_machine_util_new_fm_syncsynth();
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsHybridSynth",
				16)){
    machine = ags_machine_util_new_hybrid_synth();
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsHybridFMSynth",
				16)){
    machine = ags_machine_util_new_hybrid_fm_synth();
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsFFPlayer",
				11)){
    machine = ags_machine_util_new_ffplayer();
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsSF2Synth",
				12)){
    machine = ags_machine_util_new_sf2_synth();
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsPitchSampler",
				16)){
    machine = ags_machine_util_new_pitch_sampler();
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsSFZSynth",
				12)){
    machine = ags_machine_util_new_sfz_synth();
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsAudiorec",
				12)){
    machine = ags_machine_util_new_audiorec();
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsDesk",
				8)){
    machine = ags_machine_util_new_desk();
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsLadspaBridge",
				16)){
    machine = ags_machine_util_new_ladspa_bridge(filename,
						 effect);
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsDssiBridge",
				14)){
    machine = ags_machine_util_new_dssi_bridge(filename,
					       effect);
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsLv2Bridge",
				13)){
    machine = ags_machine_util_new_lv2_bridge(filename,
					      effect);
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsVst3Bridge",
				14)){
    machine = ags_machine_util_new_vst3_bridge(filename,
					       effect);
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsLiveDssiBridge",
				18)){
    machine = ags_machine_util_new_live_dssi_bridge(filename,
						    effect);
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsLiveLv2Bridge",
				17)){
    machine = ags_machine_util_new_live_lv2_bridge(filename,
						   effect);
  }else if(!g_ascii_strncasecmp(machine_type_name,
				"AgsLiveVst3Bridge",
				18)){
    machine = ags_machine_util_new_live_vst3_bridge(filename,
						    effect);
  }

  return(machine);
}
