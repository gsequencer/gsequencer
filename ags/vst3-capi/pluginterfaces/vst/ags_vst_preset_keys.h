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

#ifndef __AGS_VST_PRESET_KEYS_H__
#define __AGS_VST_PRESET_KEYS_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_types.h>

#ifdef __cplusplus
extern "C" {
#endif

  /* preset attributes */
  extern const AgsVstCString ags_vst_preset_attributes_kplug_in_name;
  extern const AgsVstCString ags_vst_preset_attributes_kplug_in_category;
  extern const AgsVstCString ags_vst_preset_attributes_kinstrument;
  extern const AgsVstCString ags_vst_preset_attributes_kstyle;
  extern const AgsVstCString ags_vst_preset_attributes_kcharacter;
  extern const AgsVstCString ags_vst_preset_attributes_kstate_type;
  extern const AgsVstCString ags_vst_preset_attributes_kfile_path_string_type;
  extern const AgsVstCString ags_vst_preset_attributes_kname;
  extern const AgsVstCString ags_vst_preset_attributes_kfile_name;

  /* state type */
  extern const AgsVstCString ags_vst_state_type_kproject;
  extern const AgsVstCString ags_vst_state_type_kdefault;

  /* musical instrument */
  extern const AgsVstCString ags_vst_musical_instrument_kaccordion;
  extern const AgsVstCString ags_vst_musical_instrument_kaccordion_accordion;
  extern const AgsVstCString ags_vst_musical_instrument_kaccordion_harmonica;
  extern const AgsVstCString ags_vst_musical_instrument_kaccordion_other;
 
  extern const AgsVstCString ags_vst_musical_instrument_kbass;
  extern const AgsVstCString ags_vst_musical_instrument_kbass_a_bass;
  extern const AgsVstCString ags_vst_musical_instrument_kbass_e_bass;
  extern const AgsVstCString ags_vst_musical_instrument_kbass_synth_bass;
  extern const AgsVstCString ags_vst_musical_instrument_kbass_other;

  extern const AgsVstCString ags_vst_musical_instrument_kbrass;
  extern const AgsVstCString ags_vst_musical_instrument_kbrass_french_horn;
  extern const AgsVstCString ags_vst_musical_instrument_kbrass_trumpet;
  extern const AgsVstCString ags_vst_musical_instrument_kbrass_trombone;
  extern const AgsVstCString ags_vst_musical_instrument_kbrass_tuba;
  extern const AgsVstCString ags_vst_musical_instrument_kbrass_section;
  extern const AgsVstCString ags_vst_musical_instrument_kbrass_synth;
  extern const AgsVstCString ags_vst_musical_instrument_kbrass_other;

  extern const AgsVstCString ags_vst_musical_instrument_kchromatic_perc;
  extern const AgsVstCString ags_vst_musical_instrument_kchromatic_perc_bell;
  extern const AgsVstCString ags_vst_musical_instrument_kchromatic_perc_mallett;
  extern const AgsVstCString ags_vst_musical_instrument_kchromatic_perc_wood;
  extern const AgsVstCString ags_vst_musical_instrument_kchromatic_percPercussion;
  extern const AgsVstCString ags_vst_musical_instrument_kchromatic_perc_timpani;
  extern const AgsVstCString ags_vst_musical_instrument_kchromatic_perc_other;

  extern const AgsVstCString ags_vst_musical_instrument_kdrum_perc;
  extern const AgsVstCString ags_vst_musical_instrument_kdrum_perc_drumset_g_m;
  extern const AgsVstCString ags_vst_musical_instrument_kdrum_perc_drumset;
  extern const AgsVstCString ags_vst_musical_instrument_kdrum_perc_drum_menues;
  extern const AgsVstCString ags_vst_musical_instrument_kdrum_perc_beats;
  extern const AgsVstCString ags_vst_musical_instrument_kdrum_percPercussion;
  extern const AgsVstCString ags_vst_musical_instrument_kdrum_perc_kick_drum;
  extern const AgsVstCString ags_vst_musical_instrument_kdrum_perc_snare_drum;
  extern const AgsVstCString ags_vst_musical_instrument_kdrum_perc_toms;
  extern const AgsVstCString ags_vst_musical_instrument_kdrum_perc_hi_hats;
  extern const AgsVstCString ags_vst_musical_instrument_kdrum_perc_cymbals;
  extern const AgsVstCString ags_vst_musical_instrument_kdrum_perc_other;

  extern const AgsVstCString ags_vst_musical_instrument_kethnic;
  extern const AgsVstCString ags_vst_musical_instrument_kethnic_asian;
  extern const AgsVstCString ags_vst_musical_instrument_kethnic_african;
  extern const AgsVstCString ags_vst_musical_instrument_kethnic_european;
  extern const AgsVstCString ags_vst_musical_instrument_kethnic_latin;
  extern const AgsVstCString ags_vst_musical_instrument_kethnic_american;
  extern const AgsVstCString ags_vst_musical_instrument_kethnic_alien;
  extern const AgsVstCString ags_vst_musical_instrument_kethnic_other;

  extern const AgsVstCString ags_vst_musical_instrument_kguitar;
  extern const AgsVstCString ags_vst_musical_instrument_kguitar_a_guitar;
  extern const AgsVstCString ags_vst_musical_instrument_kguitar_e_guitar;
  extern const AgsVstCString ags_vst_musical_instrument_kguitar_harp;
  extern const AgsVstCString ags_vst_musical_instrument_kguitar_ethnic;
  extern const AgsVstCString ags_vst_musical_instrument_kguitar_other;

  extern const AgsVstCString ags_vst_musical_instrument_kkeyboard;
  extern const AgsVstCString ags_vst_musical_instrument_kkeyboard_clavi;
  extern const AgsVstCString ags_vst_musical_instrument_kkeyboard_e_piano;
  extern const AgsVstCString ags_vst_musical_instrument_kkeyboard_harpsichord;
  extern const AgsVstCString ags_vst_musical_instrument_kkeyboard_other; 

  extern const AgsVstCString ags_vst_musical_instrument_kmusical_fx;
  extern const AgsVstCString ags_vst_musical_instrument_kmusical_fx_hits_stabs;
  extern const AgsVstCString ags_vst_musical_instrument_kmusical_fx_motion;
  extern const AgsVstCString ags_vst_musical_instrument_kmusical_fx_sweeps;
  extern const AgsVstCString ags_vst_musical_instrument_kmusical_fx_beeps_blips;
  extern const AgsVstCString ags_vst_musical_instrument_kmusical_fx_scratches;
  extern const AgsVstCString ags_vst_musical_instrument_kmusical_fx_other;
 
  extern const AgsVstCString ags_vst_musical_instrument_korgan;
  extern const AgsVstCString ags_vst_musical_instrument_korgan_electric;
  extern const AgsVstCString ags_vst_musical_instrument_korgan_pipe;
  extern const AgsVstCString ags_vst_musical_instrument_korgan_other;

  extern const AgsVstCString ags_vst_musical_instrument_kpiano;
  extern const AgsVstCString ags_vst_musical_instrument_kpiano_a_piano;
  extern const AgsVstCString ags_vst_musical_instrument_kpiano_e_grand;
  extern const AgsVstCString ags_vst_musical_instrument_kpiano_other;

  extern const AgsVstCString ags_vst_musical_instrument_ksound_fx;
  extern const AgsVstCString ags_vst_musical_instrument_ksound_fx_nature;
  extern const AgsVstCString ags_vst_musical_instrument_ksound_fx_mechanical;
  extern const AgsVstCString ags_vst_musical_instrument_ksound_fx_synthetic;
  extern const AgsVstCString ags_vst_musical_instrument_ksound_fx_other;

  extern const AgsVstCString ags_vst_musical_instrument_kstrings;
  extern const AgsVstCString ags_vst_musical_instrument_kstrings_violin;
  extern const AgsVstCString ags_vst_musical_instrument_kstrings_viola;
  extern const AgsVstCString ags_vst_musical_instrument_kstrings_cello;
  extern const AgsVstCString ags_vst_musical_instrument_kstrings_bass;
  extern const AgsVstCString ags_vst_musical_instrument_kstrings_section;
  extern const AgsVstCString ags_vst_musical_instrument_kstrings_synth;
  extern const AgsVstCString ags_vst_musical_instrument_kstrings_other;

  extern const AgsVstCString ags_vst_musical_instrument_ksynth_lead;
  extern const AgsVstCString ags_vst_musical_instrument_ksynth_lead_analog;
  extern const AgsVstCString ags_vst_musical_instrument_ksynth_lead_digital;
  extern const AgsVstCString ags_vst_musical_instrument_ksynth_lead_arpeggio;
  extern const AgsVstCString ags_vst_musical_instrument_ksynth_lead_other;

  extern const AgsVstCString ags_vst_musical_instrument_ksynth_pad;
  extern const AgsVstCString ags_vst_musical_instrument_ksynth_padSynth_choir;
  extern const AgsVstCString ags_vst_musical_instrument_ksynth_pad_analog;
  extern const AgsVstCString ags_vst_musical_instrument_ksynth_pad_digital;
  extern const AgsVstCString ags_vst_musical_instrument_ksynth_pad_motion;
  extern const AgsVstCString ags_vst_musical_instrument_ksynth_pad_other;

  extern const AgsVstCString ags_vst_musical_instrument_ksynth_comp;
  extern const AgsVstCString ags_vst_musical_instrument_ksynth_comp_analog;
  extern const AgsVstCString ags_vst_musical_instrument_ksynth_comp_digital;
  extern const AgsVstCString ags_vst_musical_instrument_ksynth_comp_other;

  extern const AgsVstCString ags_vst_musical_instrument_kvocal;
  extern const AgsVstCString ags_vst_musical_instrument_kvocal_lead_vocal;
  extern const AgsVstCString ags_vst_musical_instrument_kvocal_adlibs;
  extern const AgsVstCString ags_vst_musical_instrument_kvocal_choir;
  extern const AgsVstCString ags_vst_musical_instrument_kvocal_solo;
  extern const AgsVstCString ags_vst_musical_instrument_kvocal_fx;
  extern const AgsVstCString ags_vst_musical_instrument_kvocal_spoken;
  extern const AgsVstCString ags_vst_musical_instrument_kvocal_other;

  extern const AgsVstCString ags_vst_musical_instrument_kwoodwinds;
  extern const AgsVstCString ags_vst_musical_instrument_kwoodwinds_ethnic;
  extern const AgsVstCString ags_vst_musical_instrument_kwoodwinds_flute;
  extern const AgsVstCString ags_vst_musical_instrument_kwoodwinds_oboe;
  extern const AgsVstCString ags_vst_musical_instrument_kwoodwinds_engl_horn;
  extern const AgsVstCString ags_vst_musical_instrument_kwoodwinds_clarinet;
  extern const AgsVstCString ags_vst_musical_instrument_kwoodwinds_saxophone;
  extern const AgsVstCString ags_vst_musical_instrument_kwoodwinds_bassoon;
  extern const AgsVstCString ags_vst_musical_instrument_kwoodwinds_other;

  /* musical style */
  extern const AgsVstCString ags_vst_musical_style_kalternative_indie;
  extern const AgsVstCString ags_vst_musical_style_kalternative_indie_goth_rock;
  extern const AgsVstCString ags_vst_musical_style_kalternative_indie_grunge;
  extern const AgsVstCString ags_vst_musical_style_kalternative_indie_new_wave;
  extern const AgsVstCString ags_vst_musical_style_kalternative_indie_punk;
  extern const AgsVstCString ags_vst_musical_style_kalternative_indie_college_rock;
  extern const AgsVstCString ags_vst_musical_style_kalternative_indie_dark_wave;
  extern const AgsVstCString ags_vst_musical_style_kalternative_indie_hardcore;

  extern const AgsVstCString ags_vst_musical_style_kambient_chill_out;
  extern const AgsVstCString ags_vst_musical_style_kambient_chill_out_new_age_meditation;
  extern const AgsVstCString ags_vst_musical_style_kambient_chill_out_dark_ambient;
  extern const AgsVstCString ags_vst_musical_style_kambient_chill_out_downtempo;
  extern const AgsVstCString ags_vst_musical_style_kambient_chill_out_lounge;

  extern const AgsVstCString ags_vst_musical_style_kblues;
  extern const AgsVstCString ags_vst_musical_style_kblues_acoustic_blues;
  extern const AgsVstCString ags_vst_musical_style_kblues_country_blues;
  extern const AgsVstCString ags_vst_musical_style_kblues_electric_blues;
  extern const AgsVstCString ags_vst_musical_style_kblues_chicago_blues;

  extern const AgsVstCString ags_vst_musical_style_kclassical;
  extern const AgsVstCString ags_vst_musical_style_kclassical_baroque;
  extern const AgsVstCString ags_vst_musical_style_kclassical_chamber_music;
  extern const AgsVstCString ags_vst_musical_style_kclassical_medieval;
  extern const AgsVstCString ags_vst_musical_style_kclassical_modern_composition;
  extern const AgsVstCString ags_vst_musical_style_kclassical_opera;
  extern const AgsVstCString ags_vst_musical_style_kclassical_gregorian;
  extern const AgsVstCString ags_vst_musical_style_kclassical_renaissance;
  extern const AgsVstCString ags_vst_musical_style_kclassical_classic;
  extern const AgsVstCString ags_vst_musical_style_kclassical_romantic;
  extern const AgsVstCString ags_vst_musical_style_kclassical_soundtrack;

  extern const AgsVstCString ags_vst_musical_style_kcountry;
  extern const AgsVstCString ags_vst_musical_style_kcountry_country_western;
  extern const AgsVstCString ags_vst_musical_style_kcountry_honky_tonk;
  extern const AgsVstCString ags_vst_musical_style_kcountry_urban_cowboy;
  extern const AgsVstCString ags_vst_musical_style_kcountry_bluegrass;
  extern const AgsVstCString ags_vst_musical_style_kcountry_americana;
  extern const AgsVstCString ags_vst_musical_style_kcountry_squaredance;
  extern const AgsVstCString ags_vst_musical_style_kcountry_north_american_folk;

  extern const AgsVstCString ags_vst_musical_style_kelectronica_dance;
  extern const AgsVstCString ags_vst_musical_style_kelectronica_dance_minimal;
  extern const AgsVstCString ags_vst_musical_style_kelectronica_dance_classic_house;
  extern const AgsVstCString ags_vst_musical_style_kelectronica_dance_elektro_house;
  extern const AgsVstCString ags_vst_musical_style_kelectronica_dance_funky_house;
  extern const AgsVstCString ags_vst_musical_style_kelectronica_dance_industrial;
  extern const AgsVstCString ags_vst_musical_style_kelectronica_dance_electronic_body_music;
  extern const AgsVstCString ags_vst_musical_style_kelectronica_dance_trip_hop;
  extern const AgsVstCString ags_vst_musical_style_kelectronica_dance_techno;
  extern const AgsVstCString ags_vst_musical_style_kelectronica_dance_drum_n_bass_jungle;
  extern const AgsVstCString ags_vst_musical_style_kelectronica_dance_elektro;
  extern const AgsVstCString ags_vst_musical_style_kelectronica_dance_trance;
  extern const AgsVstCString ags_vst_musical_style_kelectronica_dance_dub;
  extern const AgsVstCString ags_vst_musical_style_kelectronica_dance_big_beats;

  extern const AgsVstCString ags_vst_musical_style_kexperimental;
  extern const AgsVstCString ags_vst_musical_style_kexperimental_new_music;
  extern const AgsVstCString ags_vst_musical_style_kexperimental_free_improvisation;
  extern const AgsVstCString ags_vst_musical_style_kexperimental_electronic_art_music;
  extern const AgsVstCString ags_vst_musical_style_kexperimental_noise;

  extern const AgsVstCString ags_vst_musical_style_kjazz;
  extern const AgsVstCString ags_vst_musical_style_kjazz_new_orleans_jazz;
  extern const AgsVstCString ags_vst_musical_style_kjazz_traditional_jazz;
  extern const AgsVstCString ags_vst_musical_style_kjazz_oldtime_jazz_dixiland;
  extern const AgsVstCString ags_vst_musical_style_kjazz_fusion;
  extern const AgsVstCString ags_vst_musical_style_kjazz_avantgarde;
  extern const AgsVstCString ags_vst_musical_style_kjazz_latin_jazz;
  extern const AgsVstCString ags_vst_musical_style_kjazz_free_jazz;
  extern const AgsVstCString ags_vst_musical_style_kjazz_ragtime;
  
  extern const AgsVstCString ags_vst_musical_style_kpop;
  extern const AgsVstCString ags_vst_musical_style_kpop_britpop;
  extern const AgsVstCString ags_vst_musical_style_kpop_rock;
  extern const AgsVstCString ags_vst_musical_style_kpop_teen_pop;
  extern const AgsVstCString ags_vst_musical_style_kpop_chart_dance;
  extern const AgsVstCString ags_vst_musical_style_kpop80s_pop;
  extern const AgsVstCString ags_vst_musical_style_kpop_dancehall;
  extern const AgsVstCString ags_vst_musical_style_kpop_disco;

  extern const AgsVstCString ags_vst_musical_style_krock_metal;
  extern const AgsVstCString ags_vst_musical_style_krock_metal_blues_rock;
  extern const AgsVstCString ags_vst_musical_style_krock_metal_classic_rock;
  extern const AgsVstCString ags_vst_musical_style_krock_metal_hard_rock;
  extern const AgsVstCString ags_vst_musical_style_krock_metal_rock_roll;
  extern const AgsVstCString ags_vst_musical_style_krock_metal_singer_songwriter;
  extern const AgsVstCString ags_vst_musical_style_krock_metal_heavy_metal;
  extern const AgsVstCString ags_vst_musical_style_krock_metal_death_black_metal;
  extern const AgsVstCString ags_vst_musical_style_krock_metal_nu_metal;
  extern const AgsVstCString ags_vst_musical_style_krock_metal_reggae;
  extern const AgsVstCString ags_vst_musical_style_krock_metal_ballad;
  extern const AgsVstCString ags_vst_musical_style_krock_metal_alternative_rock;
  extern const AgsVstCString ags_vst_musical_style_krock_metal_rockabilly;
  extern const AgsVstCString ags_vst_musical_style_krock_metal_thrash_metal;
  extern const AgsVstCString ags_vst_musical_style_krock_metal_progressive_rock;

  extern const AgsVstCString ags_vst_musical_style_kurban_hip_hop_r_b;
  extern const AgsVstCString ags_vst_musical_style_kurban_hip_hop_r_b_classic;
  extern const AgsVstCString ags_vst_musical_style_kurban_hip_hop_r_b_modern;
  extern const AgsVstCString ags_vst_musical_style_kurban_hip_hop_r_b_pop;
  extern const AgsVstCString ags_vst_musical_style_kurban_hip_hop_r_b_west_coast_hip_hop;
  extern const AgsVstCString ags_vst_musical_style_kurban_hip_hop_r_b_east_coast_hip_hop;
  extern const AgsVstCString ags_vst_musical_style_kurban_hip_hop_r_b_rap_hip_hop;
  extern const AgsVstCString ags_vst_musical_style_kurban_hip_hop_r_b_soul;
  extern const AgsVstCString ags_vst_musical_style_kurban_hip_hop_r_b_funk;

  extern const AgsVstCString ags_vst_musical_style_kworld_ethnic;
  extern const AgsVstCString ags_vst_musical_style_kworld_ethnic_africa;
  extern const AgsVstCString ags_vst_musical_style_kworld_ethnic_asia;
  extern const AgsVstCString ags_vst_musical_style_kworld_ethnic_celtic;
  extern const AgsVstCString ags_vst_musical_style_kworld_ethnic_europe;
  extern const AgsVstCString ags_vst_musical_style_kworld_ethnic_klezmer;
  extern const AgsVstCString ags_vst_musical_style_kworld_ethnic_scandinavia;
  extern const AgsVstCString ags_vst_musical_style_kworld_ethnic_eastern_europe;
  extern const AgsVstCString ags_vst_musical_style_kworld_ethnic_india_oriental;
  extern const AgsVstCString ags_vst_musical_style_kworld_ethnic_north_america;
  extern const AgsVstCString ags_vst_musical_style_kworld_ethnic_south_america;
  extern const AgsVstCString ags_vst_musical_style_kworld_ethnic_australia;

  /* musical character */
  extern const AgsVstCString ags_vst_musical_character_kmono;
  extern const AgsVstCString ags_vst_musical_character_kpoly;

  extern const AgsVstCString ags_vst_musical_character_ksplit;
  extern const AgsVstCString ags_vst_musical_character_klayer;

  extern const AgsVstCString ags_vst_musical_character_kglide;
  extern const AgsVstCString ags_vst_musical_character_kglissando;

  extern const AgsVstCString ags_vst_musical_character_kmajor;
  extern const AgsVstCString ags_vst_musical_character_kminor;

  extern const AgsVstCString ags_vst_musical_character_ksingle;
  extern const AgsVstCString ags_vst_musical_character_kensemble;

  extern const AgsVstCString ags_vst_musical_character_kacoustic;
  extern const AgsVstCString ags_vst_musical_character_kelectric;
  extern const AgsVstCString ags_vst_musical_character_kanalog;
  extern const AgsVstCString ags_vst_musical_character_kdigital;

  extern const AgsVstCString ags_vst_musical_character_kvintage;
  extern const AgsVstCString ags_vst_musical_character_kmodern;

  extern const AgsVstCString ags_vst_musical_character_kold;
  extern const AgsVstCString ags_vst_musical_character_knew;

  extern const AgsVstCString ags_vst_musical_character_kclean;
  extern const AgsVstCString ags_vst_musical_character_kdistorted;

  extern const AgsVstCString ags_vst_musical_character_kdry;
  extern const AgsVstCString ags_vst_musical_character_kprocessed;

  extern const AgsVstCString ags_vst_musical_character_kharmonic;
  extern const AgsVstCString ags_vst_musical_character_kdissonant;

  extern const AgsVstCString ags_vst_musical_character_kclear;
  extern const AgsVstCString ags_vst_musical_character_knoisy;

  extern const AgsVstCString ags_vst_musical_character_kthin;
  extern const AgsVstCString ags_vst_musical_character_krich;

  extern const AgsVstCString ags_vst_musical_character_kdark;
  extern const AgsVstCString ags_vst_musical_character_kbright;

  extern const AgsVstCString ags_vst_musical_character_kcold;
  extern const AgsVstCString ags_vst_musical_character_kwarm;

  extern const AgsVstCString ags_vst_musical_character_kmetallic;
  extern const AgsVstCString ags_vst_musical_character_kwooden;

  extern const AgsVstCString ags_vst_musical_character_kglass;
  extern const AgsVstCString ags_vst_musical_character_kplastic;

  extern const AgsVstCString ags_vst_musical_character_kpercussive;
  extern const AgsVstCString ags_vst_musical_character_ksoft;

  extern const AgsVstCString ags_vst_musical_character_kfast;
  extern const AgsVstCString ags_vst_musical_character_kslow;

  extern const AgsVstCString ags_vst_musical_character_kshort;
  extern const AgsVstCString ags_vst_musical_character_klong;

  extern const AgsVstCString ags_vst_musical_character_kattack;
  extern const AgsVstCString ags_vst_musical_character_krelease;

  extern const AgsVstCString ags_vst_musical_character_kdecay;
  extern const AgsVstCString ags_vst_musical_character_ksustain;

  extern const AgsVstCString ags_vst_musical_character_kfast_attack;
  extern const AgsVstCString ags_vst_musical_character_kslow_attack;

  extern const AgsVstCString ags_vst_musical_character_kshort_release;
  extern const AgsVstCString ags_vst_musical_character_klong_release;
  extern const AgsVstCString ags_vst_musical_character_kstatic;
  extern const AgsVstCString ags_vst_musical_character_kmoving;

  extern const AgsVstCString ags_vst_musical_character_kloop;
  extern const AgsVstCString ags_vst_musical_character_kone_shot;
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_PRESET_KEYS_H__*/

