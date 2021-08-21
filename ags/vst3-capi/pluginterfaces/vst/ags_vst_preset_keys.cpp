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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_preset_keys.h>

extern "C" {

  /* preset attributes */
  const AgsVstCString ags_vst_preset_attributes_kplug_in_name            = "PlugInName";
  const AgsVstCString ags_vst_preset_attributes_kplug_in_category        = "PlugInCategory";
  const AgsVstCString ags_vst_preset_attributes_kinstrument              = "MusicalInstrument";
  const AgsVstCString ags_vst_preset_attributes_kstyle                   = "MusicalStyle";
  const AgsVstCString ags_vst_preset_attributes_kcharacter               = "MusicalCharacter";
  const AgsVstCString ags_vst_preset_attributes_kstate_type              = "StateType";
  const AgsVstCString ags_vst_preset_attributes_kfile_path_string_type   = "FilePathString";
  const AgsVstCString ags_vst_preset_attributes_kname                    = "Name";
  const AgsVstCString ags_vst_preset_attributes_kfile_name               = "FileName";

  /* state type */
  const AgsVstCString ags_vst_state_type_kproject = "Project";
  const AgsVstCString ags_vst_state_type_kdefault = "Default";

  /* musical instrument */
  const AgsVstCString ags_vst_musical_instrument_kaccordion                          = "Accordion";
  const AgsVstCString ags_vst_musical_instrument_kaccordion_accordion                = "Accordion|Accordion";
  const AgsVstCString ags_vst_musical_instrument_kaccordion_harmonica                = "Accordion|Harmonica";
  const AgsVstCString ags_vst_musical_instrument_kaccordion_other                    = "Accordion|Other";
 
  const AgsVstCString ags_vst_musical_instrument_kbass                               = "Bass";
  const AgsVstCString ags_vst_musical_instrument_kbass_a_bass                        = "Bass|A. Bass";
  const AgsVstCString ags_vst_musical_instrument_kbass_e_bass                        = "Bass|E. Bass";
  const AgsVstCString ags_vst_musical_instrument_kbass_synth_bass                    = "Bass|Synth Bass";
  const AgsVstCString ags_vst_musical_instrument_kbass_other                         = "Bass|Other";

  const AgsVstCString ags_vst_musical_instrument_kbrass                              = "Brass";
  const AgsVstCString ags_vst_musical_instrument_kbrass_french_horn                  = "Brass|French Horn";
  const AgsVstCString ags_vst_musical_instrument_kbrass_trumpet                      = "Brass|Trumpet";
  const AgsVstCString ags_vst_musical_instrument_kbrass_trombone                     = "Brass|Trombone";
  const AgsVstCString ags_vst_musical_instrument_kbrass_tuba                         = "Brass|Tuba";
  const AgsVstCString ags_vst_musical_instrument_kbrass_section                      = "Brass|Section";
  const AgsVstCString ags_vst_musical_instrument_kbrass_synth                        = "Brass|Synth";
  const AgsVstCString ags_vst_musical_instrument_kbrass_other                        = "Brass|Other";

  const AgsVstCString ags_vst_musical_instrument_kchromatic_perc                     = "Chromatic Perc";
  const AgsVstCString ags_vst_musical_instrument_kchromatic_perc_bell                = "Chromatic Perc|Bell";
  const AgsVstCString ags_vst_musical_instrument_kchromatic_perc_mallett             = "Chromatic Perc|Mallett";
  const AgsVstCString ags_vst_musical_instrument_kchromatic_perc_wood                = "Chromatic Perc|Wood";
  const AgsVstCString ags_vst_musical_instrument_kchromatic_percPercussion           = "Chromatic Perc|Percussion";
  const AgsVstCString ags_vst_musical_instrument_kchromatic_perc_timpani             = "Chromatic Perc|Timpani";
  const AgsVstCString ags_vst_musical_instrument_kchromatic_perc_other               = "Chromatic Perc|Other";

  const AgsVstCString ags_vst_musical_instrument_kdrum_perc                          = "Drum&Perc";
  const AgsVstCString ags_vst_musical_instrument_kdrum_perc_drumset_g_m              = "Drum&Perc|Drumset GM";
  const AgsVstCString ags_vst_musical_instrument_kdrum_perc_drumset                  = "Drum&Perc|Drumset";
  const AgsVstCString ags_vst_musical_instrument_kdrum_perc_drum_menues              = "Drum&Perc|Drum Menues";
  const AgsVstCString ags_vst_musical_instrument_kdrum_perc_beats                    = "Drum&Perc|Beats";
  const AgsVstCString ags_vst_musical_instrument_kdrum_percPercussion                = "Drum&_perc|Percussion";
  const AgsVstCString ags_vst_musical_instrument_kdrum_perc_kick_drum                = "Drum&Perc|Kick Drum";
  const AgsVstCString ags_vst_musical_instrument_kdrum_perc_snare_drum               = "Drum&Perc|Snare Drum";
  const AgsVstCString ags_vst_musical_instrument_kdrum_perc_toms                     = "Drum&Perc|Toms";
  const AgsVstCString ags_vst_musical_instrument_kdrum_perc_hi_hats                  = "Drum&Perc|HiHats";
  const AgsVstCString ags_vst_musical_instrument_kdrum_perc_cymbals                  = "Drum&Perc|Cymbals";
  const AgsVstCString ags_vst_musical_instrument_kdrum_perc_other                    = "Drum&Perc|Other";

  const AgsVstCString ags_vst_musical_instrument_kethnic                             = "Ethnic";
  const AgsVstCString ags_vst_musical_instrument_kethnic_asian                       = "Ethnic|Asian";
  const AgsVstCString ags_vst_musical_instrument_kethnic_african                     = "Ethnic|African";
  const AgsVstCString ags_vst_musical_instrument_kethnic_european                    = "Ethnic|European";
  const AgsVstCString ags_vst_musical_instrument_kethnic_latin                       = "Ethnic|Latin";
  const AgsVstCString ags_vst_musical_instrument_kethnic_american                    = "Ethnic|American";
  const AgsVstCString ags_vst_musical_instrument_kethnic_alien                       = "Ethnic|Alien";
  const AgsVstCString ags_vst_musical_instrument_kethnic_other                       = "Ethnic|Other";

  const AgsVstCString ags_vst_musical_instrument_kguitar                             = "Guitar/Plucked";
  const AgsVstCString ags_vst_musical_instrument_kguitar_a_guitar                    = "Guitar/Plucked|_a. Guitar";
  const AgsVstCString ags_vst_musical_instrument_kguitar_e_guitar                    = "Guitar/Plucked|_e. Guitar";
  const AgsVstCString ags_vst_musical_instrument_kguitar_harp                        = "Guitar/Plucked|_harp";
  const AgsVstCString ags_vst_musical_instrument_kguitar_ethnic                      = "Guitar/Plucked|Ethnic";
  const AgsVstCString ags_vst_musical_instrument_kguitar_other                       = "Guitar/Plucked|Other";

  const AgsVstCString ags_vst_musical_instrument_kkeyboard                           = "Keyboard";
  const AgsVstCString ags_vst_musical_instrument_kkeyboard_clavi                     = "Keyboard|Clavi";
  const AgsVstCString ags_vst_musical_instrument_kkeyboard_e_piano                   = "Keyboard|E. Piano";
  const AgsVstCString ags_vst_musical_instrument_kkeyboard_harpsichord               = "Keyboard|Harpsichord";
  const AgsVstCString ags_vst_musical_instrument_kkeyboard_other                     = "Keyboard|Other"; 

  const AgsVstCString ags_vst_musical_instrument_kmusical_fx                         = "Musical FX";
  const AgsVstCString ags_vst_musical_instrument_kmusical_fx_hits_stabs              = "Musical FX|Hits&Stabs";
  const AgsVstCString ags_vst_musical_instrument_kmusical_fx_motion                  = "Musical FX|Motion";
  const AgsVstCString ags_vst_musical_instrument_kmusical_fx_sweeps                  = "Musical FX|Sweeps";
  const AgsVstCString ags_vst_musical_instrument_kmusical_fx_beeps_blips             = "Musical FX|Beeps&Blips";
  const AgsVstCString ags_vst_musical_instrument_kmusical_fx_scratches               = "Musical FX|Scratches";
  const AgsVstCString ags_vst_musical_instrument_kmusical_fx_other                   = "Musical FX|Other";
 
  const AgsVstCString ags_vst_musical_instrument_korgan                              = "Organ";
  const AgsVstCString ags_vst_musical_instrument_korgan_electric                     = "Organ|Electric";
  const AgsVstCString ags_vst_musical_instrument_korgan_pipe                         = "Organ|Pipe";
  const AgsVstCString ags_vst_musical_instrument_korgan_other                        = "Organ|Other";

  const AgsVstCString ags_vst_musical_instrument_kpiano                              = "Piano";
  const AgsVstCString ags_vst_musical_instrument_kpiano_a_piano                      = "Piano|A. Piano";
  const AgsVstCString ags_vst_musical_instrument_kpiano_e_grand                      = "Piano|E. Grand";
  const AgsVstCString ags_vst_musical_instrument_kpiano_other                        = "Piano|Other";

  const AgsVstCString ags_vst_musical_instrument_ksound_fx                           = "Sound FX";
  const AgsVstCString ags_vst_musical_instrument_ksound_fx_nature                    = "Sound FX|Nature";
  const AgsVstCString ags_vst_musical_instrument_ksound_fx_mechanical                = "Sound FX|Mechanical";
  const AgsVstCString ags_vst_musical_instrument_ksound_fx_synthetic                 = "Sound FX|Synthetic";
  const AgsVstCString ags_vst_musical_instrument_ksound_fx_other                     = "Sound FX|Other";

  const AgsVstCString ags_vst_musical_instrument_kstrings                            = "Strings";
  const AgsVstCString ags_vst_musical_instrument_kstrings_violin                     = "Strings|Violin";
  const AgsVstCString ags_vst_musical_instrument_kstrings_viola                      = "Strings|Viola";
  const AgsVstCString ags_vst_musical_instrument_kstrings_cello                      = "Strings|Cello";
  const AgsVstCString ags_vst_musical_instrument_kstrings_bass                       = "Strings|Bass";
  const AgsVstCString ags_vst_musical_instrument_kstrings_section                    = "Strings|Section";
  const AgsVstCString ags_vst_musical_instrument_kstrings_synth                      = "Strings|Synth";
  const AgsVstCString ags_vst_musical_instrument_kstrings_other                      = "Strings|Other";

  const AgsVstCString ags_vst_musical_instrument_ksynth_lead                         = "Synth Lead";
  const AgsVstCString ags_vst_musical_instrument_ksynth_lead_analog                  = "Synth Lead|Analog";
  const AgsVstCString ags_vst_musical_instrument_ksynth_lead_digital                 = "Synth Lead|Digital";
  const AgsVstCString ags_vst_musical_instrument_ksynth_lead_arpeggio                = "Synth Lead|Arpeggio";
  const AgsVstCString ags_vst_musical_instrument_ksynth_lead_other                   = "Synth Lead|Other";

  const AgsVstCString ags_vst_musical_instrument_ksynth_pad                          = "Synth Pad";
  const AgsVstCString ags_vst_musical_instrument_ksynth_padSynth_choir               = "Synth Pad|Synth Choir";
  const AgsVstCString ags_vst_musical_instrument_ksynth_pad_analog                   = "Synth Pad|Analog";
  const AgsVstCString ags_vst_musical_instrument_ksynth_pad_digital                  = "Synth Pad|Digital";
  const AgsVstCString ags_vst_musical_instrument_ksynth_pad_motion                   = "Synth Pad|Motion";
  const AgsVstCString ags_vst_musical_instrument_ksynth_pad_other                    = "Synth Pad|Other";

  const AgsVstCString ags_vst_musical_instrument_ksynth_comp                         = "Synth Comp";
  const AgsVstCString ags_vst_musical_instrument_ksynth_comp_analog                  = "Synth Comp|Analog";
  const AgsVstCString ags_vst_musical_instrument_ksynth_comp_digital                 = "Synth Comp|Digital";
  const AgsVstCString ags_vst_musical_instrument_ksynth_comp_other                   = "Synth Comp|Other";

  const AgsVstCString ags_vst_musical_instrument_kvocal                              = "Vocal";
  const AgsVstCString ags_vst_musical_instrument_kvocal_lead_vocal                   = "Vocal|Lead Vocal";
  const AgsVstCString ags_vst_musical_instrument_kvocal_adlibs                       = "Vocal|Adlibs";
  const AgsVstCString ags_vst_musical_instrument_kvocal_choir                        = "Vocal|Choir";
  const AgsVstCString ags_vst_musical_instrument_kvocal_solo                         = "Vocal|Solo";
  const AgsVstCString ags_vst_musical_instrument_kvocal_fx                           = "Vocal|FX";
  const AgsVstCString ags_vst_musical_instrument_kvocal_spoken                       = "Vocal|Spoken";
  const AgsVstCString ags_vst_musical_instrument_kvocal_other                        = "Vocal|Other";

  const AgsVstCString ags_vst_musical_instrument_kwoodwinds                          = "Woodwinds";
  const AgsVstCString ags_vst_musical_instrument_kwoodwinds_ethnic                   = "Woodwinds|Ethnic";
  const AgsVstCString ags_vst_musical_instrument_kwoodwinds_flute                    = "Woodwinds|Flute";
  const AgsVstCString ags_vst_musical_instrument_kwoodwinds_oboe                     = "Woodwinds|Oboe";
  const AgsVstCString ags_vst_musical_instrument_kwoodwinds_engl_horn                = "Woodwinds|Engl. Horn";
  const AgsVstCString ags_vst_musical_instrument_kwoodwinds_clarinet                 = "Woodwinds|Carinet";
  const AgsVstCString ags_vst_musical_instrument_kwoodwinds_saxophone                = "Woodwinds|Saxophone";
  const AgsVstCString ags_vst_musical_instrument_kwoodwinds_bassoon                  = "Woodwinds|Bassoon";
  const AgsVstCString ags_vst_musical_instrument_kwoodwinds_other                    = "Woodwinds|Other";

  /* musical style */
  const AgsVstCString ags_vst_musical_style_kalternative_indie                                  = "Alternative/Indie";
  const AgsVstCString ags_vst_musical_style_kalternative_indie_goth_rock                        = "Alternative/Indie|Goth Rock";
  const AgsVstCString ags_vst_musical_style_kalternative_indie_grunge                           = "Alternative/Indie|Grunge";
  const AgsVstCString ags_vst_musical_style_kalternative_indie_new_wave                         = "Alternative/Indie|New Wave";
  const AgsVstCString ags_vst_musical_style_kalternative_indie_punk                             = "Alternative/Indie|Punk";
  const AgsVstCString ags_vst_musical_style_kalternative_indie_college_rock                     = "Alternative/Indie|College Rock";
  const AgsVstCString ags_vst_musical_style_kalternative_indie_dark_wave                        = "Alternative/Indie|Dark Wave";
  const AgsVstCString ags_vst_musical_style_kalternative_indie_hardcore                         = "Alternative/Indie|Hardcore";

  const AgsVstCString ags_vst_musical_style_kambient_chill_out                                  = "Ambient/ChillOut";
  const AgsVstCString ags_vst_musical_style_kambient_chill_out_new_age_meditation               = "Ambient/ChillOut|New Age/Meditation";
  const AgsVstCString ags_vst_musical_style_kambient_chill_out_dark_ambient                     = "Ambient/ChillOut|Dark Ambient";
  const AgsVstCString ags_vst_musical_style_kambient_chill_out_downtempo                        = "Ambient/ChillOut|Downtempo";
  const AgsVstCString ags_vst_musical_style_kambient_chill_out_lounge                           = "Ambient/ChillOut|Lounge";

  const AgsVstCString ags_vst_musical_style_kblues                                              = "Blues";
  const AgsVstCString ags_vst_musical_style_kblues_acoustic_blues                               = "Blues|Acoustic Blues";
  const AgsVstCString ags_vst_musical_style_kblues_country_blues                                = "Blues|Country Blues";
  const AgsVstCString ags_vst_musical_style_kblues_electric_blues                               = "Blues|Electric Blues";
  const AgsVstCString ags_vst_musical_style_kblues_chicago_blues                                = "Blues|Chicago Blues";

  const AgsVstCString ags_vst_musical_style_kclassical                                          = "Classical";
  const AgsVstCString ags_vst_musical_style_kclassical_baroque                                  = "Classical|Baroque";
  const AgsVstCString ags_vst_musical_style_kclassical_chamber_music                            = "Classical|Chamber Music";
  const AgsVstCString ags_vst_musical_style_kclassical_medieval                                 = "Classical|Medieval";
  const AgsVstCString ags_vst_musical_style_kclassical_modern_composition                       = "Classical|Modern Composition";
  const AgsVstCString ags_vst_musical_style_kclassical_opera                                    = "Classical|Opera";
  const AgsVstCString ags_vst_musical_style_kclassical_gregorian                                = "Classical|Gregorian";
  const AgsVstCString ags_vst_musical_style_kclassical_renaissance                              = "Classical|Renaissance";
  const AgsVstCString ags_vst_musical_style_kclassical_classic                                  = "Classical|Classic";
  const AgsVstCString ags_vst_musical_style_kclassical_romantic                                 = "Classical|Romantic";
  const AgsVstCString ags_vst_musical_style_kclassical_soundtrack                               = "Classical|Soundtrack";

  const AgsVstCString ags_vst_musical_style_kcountry                                            = "Country";
  const AgsVstCString ags_vst_musical_style_kcountry_country_western                            = "Country|Country/Western";
  const AgsVstCString ags_vst_musical_style_kcountry_honky_tonk                                 = "Country|Honky Tonk";
  const AgsVstCString ags_vst_musical_style_kcountry_urban_cowboy                               = "Country|Urban Cowboy";
  const AgsVstCString ags_vst_musical_style_kcountry_bluegrass                                  = "Country|Bluegrass";
  const AgsVstCString ags_vst_musical_style_kcountry_americana                                  = "Country|Americana";
  const AgsVstCString ags_vst_musical_style_kcountry_squaredance                                = "Country|Squaredance";
  const AgsVstCString ags_vst_musical_style_kcountry_north_american_folk                        = "Country|North American Folk";

  const AgsVstCString ags_vst_musical_style_kelectronica_dance                                 = "Electronica/Dance";
  const AgsVstCString ags_vst_musical_style_kelectronica_dance_minimal                         = "Electronica/Dance|Minimal";
  const AgsVstCString ags_vst_musical_style_kelectronica_dance_classic_house                   = "Electronica/Dance|Classic House";
  const AgsVstCString ags_vst_musical_style_kelectronica_dance_elektro_house                   = "Electronica/Dance|_elektro House";
  const AgsVstCString ags_vst_musical_style_kelectronica_dance_funky_house                     = "Electronica/Dance|_funky House";
  const AgsVstCString ags_vst_musical_style_kelectronica_dance_industrial                      = "Electronica/Dance|Industrial";
  const AgsVstCString ags_vst_musical_style_kelectronica_dance_electronic_body_music           = "Electronica/Dance|Electronic Body Music";
  const AgsVstCString ags_vst_musical_style_kelectronica_dance_trip_hop                        = "Electronica/Dance|Trip Hop";
  const AgsVstCString ags_vst_musical_style_kelectronica_dance_techno                          = "Electronica/Dance|Techno";
  const AgsVstCString ags_vst_musical_style_kelectronica_dance_drum_n_bass_jungle              = "Electronica/Dance|Drum'n'Bass/Jungle";
  const AgsVstCString ags_vst_musical_style_kelectronica_dance_elektro                         = "Electronica/Dance|Elektro";
  const AgsVstCString ags_vst_musical_style_kelectronica_dance_trance                          = "Electronica/Dance|Trance";
  const AgsVstCString ags_vst_musical_style_kelectronica_dance_dub                             = "Electronica/Dance|Dub";
  const AgsVstCString ags_vst_musical_style_kelectronica_dance_big_beats                       = "Electronica/Dance|Big Beats";

  const AgsVstCString ags_vst_musical_style_kexperimental                                      = "Experimental";
  const AgsVstCString ags_vst_musical_style_kexperimental_new_music                            = "Experimental|New Music";
  const AgsVstCString ags_vst_musical_style_kexperimental_free_improvisation                   = "Experimental|Free Improvisation";
  const AgsVstCString ags_vst_musical_style_kexperimental_electronic_art_music                 = "Experimental|Electronic Art Music";
  const AgsVstCString ags_vst_musical_style_kexperimental_noise                                = "Experimental|Noise";

  const AgsVstCString ags_vst_musical_style_kjazz                                              = "Jazz";
  const AgsVstCString ags_vst_musical_style_kjazz_new_orleans_jazz                             = "Jazz|New Orleans Jazz";
  const AgsVstCString ags_vst_musical_style_kjazz_traditional_jazz                             = "Jazz|Traditional Jazz";
  const AgsVstCString ags_vst_musical_style_kjazz_oldtime_jazz_dixiland                        = "Jazz|_oldtime Jazz/Dixiland";
  const AgsVstCString ags_vst_musical_style_kjazz_fusion                                       = "Jazz|Fusion";
  const AgsVstCString ags_vst_musical_style_kjazz_avantgarde                                   = "Jazz|Avantgarde";
  const AgsVstCString ags_vst_musical_style_kjazz_latin_jazz                                   = "Jazz|Latin Jazz";
  const AgsVstCString ags_vst_musical_style_kjazz_free_jazz                                    = "Jazz|Free Jazz";
  const AgsVstCString ags_vst_musical_style_kjazz_ragtime                                      = "Jazz|Ragtime";
  
  const AgsVstCString ags_vst_musical_style_kpop                                               = "Pop";
  const AgsVstCString ags_vst_musical_style_kpop_britpop                                       = "Pop|Britpop";
  const AgsVstCString ags_vst_musical_style_kpop_rock                                          = "Pop|Pop/Rock";
  const AgsVstCString ags_vst_musical_style_kpop_teen_pop                                      = "Pop|Teen Pop";
  const AgsVstCString ags_vst_musical_style_kpop_chart_dance                                   = "Pop|_chart Dance";
  const AgsVstCString ags_vst_musical_style_kpop80s_pop                                        = "Pop|80's Pop";
  const AgsVstCString ags_vst_musical_style_kpop_dancehall                                     = "Pop|Dancehall";
  const AgsVstCString ags_vst_musical_style_kpop_disco                                         = "Pop|Disco";

  const AgsVstCString ags_vst_musical_style_krock_metal                                        = "Rock/Metal";
  const AgsVstCString ags_vst_musical_style_krock_metal_blues_rock                             = "Rock/Metal|Blues Rock";
  const AgsVstCString ags_vst_musical_style_krock_metal_classic_rock                           = "Rock/Metal|Classic Rock";
  const AgsVstCString ags_vst_musical_style_krock_metal_hard_rock                              = "Rock/Metal|Hard Rock";
  const AgsVstCString ags_vst_musical_style_krock_metal_rock_roll                              = "Rock/Metal|Rock &amp; Roll";
  const AgsVstCString ags_vst_musical_style_krock_metal_singer_songwriter                      = "Rock/Metal|Singer/Songwriter";
  const AgsVstCString ags_vst_musical_style_krock_metal_heavy_metal                            = "Rock/Metal|Heavy Metal";
  const AgsVstCString ags_vst_musical_style_krock_metal_death_black_metal                      = "Rock/Metal|Death/Black Metal";
  const AgsVstCString ags_vst_musical_style_krock_metal_nu_metal                               = "Rock/Metal|NuMetal";
  const AgsVstCString ags_vst_musical_style_krock_metal_reggae                                 = "Rock/Metal|Reggae";
  const AgsVstCString ags_vst_musical_style_krock_metal_ballad                                 = "Rock/Metal|Ballad";
  const AgsVstCString ags_vst_musical_style_krock_metal_alternative_rock                       = "Rock/Metal|Alternative Rock";
  const AgsVstCString ags_vst_musical_style_krock_metal_rockabilly                             = "Rock/Metal|Rockabilly";
  const AgsVstCString ags_vst_musical_style_krock_metal_thrash_metal                           = "Rock/Metal|Thrash Metal";
  const AgsVstCString ags_vst_musical_style_krock_metal_progressive_rock                       = "Rock/Metal|Progressive Rock";

  const AgsVstCString ags_vst_musical_style_kurban_hip_hop_r_b                                 = "Urban (Hip-Hop / R&B)";
  const AgsVstCString ags_vst_musical_style_kurban_hip_hop_r_b_classic                         = "Urban (Hip-Hop / R&B)|Classic R&B";
  const AgsVstCString ags_vst_musical_style_kurban_hip_hop_r_b_modern                          = "Urban (Hip-Hop / R&B)|_modern R&B";
  const AgsVstCString ags_vst_musical_style_kurban_hip_hop_r_b_pop                             = "Urban (Hip-Hop / R&B)|R&B Pop";
  const AgsVstCString ags_vst_musical_style_kurban_hip_hop_r_b_west_coast_hip_hop              = "Urban (Hip-Hop / R&B)|WestCoast Hip-Hop";
  const AgsVstCString ags_vst_musical_style_kurban_hip_hop_r_b_east_coast_hip_hop              = "Urban (Hip-Hop / R&B)|EastCoast Hip-Hop";
  const AgsVstCString ags_vst_musical_style_kurban_hip_hop_r_b_rap_hip_hop                     = "Urban (Hip-Hop / R&B)|Tap/Hip Hop";
  const AgsVstCString ags_vst_musical_style_kurban_hip_hop_r_b_soul                            = "Urban (Hip-Hop / R&B)|Soul";
  const AgsVstCString ags_vst_musical_style_kurban_hip_hop_r_b_funk                            = "Urban (Hip-Hop / R&B)|Funk";

  const AgsVstCString ags_vst_musical_style_kworld_ethnic                                      = "World/Ethnic";
  const AgsVstCString ags_vst_musical_style_kworld_ethnic_africa                               = "World/Ethnic|Africa";
  const AgsVstCString ags_vst_musical_style_kworld_ethnic_asia                                 = "World/Ethnic|Asia";
  const AgsVstCString ags_vst_musical_style_kworld_ethnic_celtic                               = "World/Ethnic|Celtic";
  const AgsVstCString ags_vst_musical_style_kworld_ethnic_europe                               = "World/Ethnic|Europe";
  const AgsVstCString ags_vst_musical_style_kworld_ethnic_klezmer                              = "World/Ethnic|Klezmer";
  const AgsVstCString ags_vst_musical_style_kworld_ethnic_scandinavia                          = "World/Ethnic|Scandinavia";
  const AgsVstCString ags_vst_musical_style_kworld_ethnic_eastern_europe                       = "World/Ethnic|Eastern Europe";
  const AgsVstCString ags_vst_musical_style_kworld_ethnic_india_oriental                       = "World/Ethnic|India/Oriental";
  const AgsVstCString ags_vst_musical_style_kworld_ethnic_north_america                        = "World/Ethnic|North America";
  const AgsVstCString ags_vst_musical_style_kworld_ethnic_south_america                        = "World/Ethnic|South America";
  const AgsVstCString ags_vst_musical_style_kworld_ethnic_australia                            = "World/Ethnic|Australia";

  /* musical character */
  const AgsVstCString ags_vst_musical_character_kmono             = "Mono";
  const AgsVstCString ags_vst_musical_character_kpoly             = "Poly";

  const AgsVstCString ags_vst_musical_character_ksplit            = "Split";
  const AgsVstCString ags_vst_musical_character_klayer            = "Layer";

  const AgsVstCString ags_vst_musical_character_kglide            = "Glide";
  const AgsVstCString ags_vst_musical_character_kglissando        = "Glissando";

  const AgsVstCString ags_vst_musical_character_kmajor            = "Major";
  const AgsVstCString ags_vst_musical_character_kminor            = "Minor";

  const AgsVstCString ags_vst_musical_character_ksingle           = "Single";
  const AgsVstCString ags_vst_musical_character_kensemble         = "Ensemble";

  const AgsVstCString ags_vst_musical_character_kacoustic         = "Acoustic";
  const AgsVstCString ags_vst_musical_character_kelectric         = "Electric";
  const AgsVstCString ags_vst_musical_character_kanalog           = "Analog";
  const AgsVstCString ags_vst_musical_character_kdigital          = "Digital";

  const AgsVstCString ags_vst_musical_character_kvintage          = "Vintage";
  const AgsVstCString ags_vst_musical_character_kmodern           = "Modern";

  const AgsVstCString ags_vst_musical_character_kold              = "Old";
  const AgsVstCString ags_vst_musical_character_knew              = "New";

  const AgsVstCString ags_vst_musical_character_kclean            = "Clean";
  const AgsVstCString ags_vst_musical_character_kdistorted        = "Distorted";

  const AgsVstCString ags_vst_musical_character_kdry              = "Dry";
  const AgsVstCString ags_vst_musical_character_kprocessed        = "Processed";

  const AgsVstCString ags_vst_musical_character_kharmonic         = "Harmonic";
  const AgsVstCString ags_vst_musical_character_kdissonant        = "Dissonant";

  const AgsVstCString ags_vst_musical_character_kclear            = "Clear";
  const AgsVstCString ags_vst_musical_character_knoisy            = "Noisy";

  const AgsVstCString ags_vst_musical_character_kthin             = "Thin";
  const AgsVstCString ags_vst_musical_character_krich             = "Rich";

  const AgsVstCString ags_vst_musical_character_kdark             = "Dark";
  const AgsVstCString ags_vst_musical_character_kbright           = "Bright";

  const AgsVstCString ags_vst_musical_character_kcold             = "Cold";
  const AgsVstCString ags_vst_musical_character_kwarm             = "Warm";

  const AgsVstCString ags_vst_musical_character_kmetallic         = "Metallic";
  const AgsVstCString ags_vst_musical_character_kwooden           = "wooden";

  const AgsVstCString ags_vst_musical_character_kglass            = "Glass";
  const AgsVstCString ags_vst_musical_character_kplastic          = "Plastic";

  const AgsVstCString ags_vst_musical_character_kpercussive       = "Percussive";
  const AgsVstCString ags_vst_musical_character_ksoft             = "Soft";

  const AgsVstCString ags_vst_musical_character_kfast             = "Fast";
  const AgsVstCString ags_vst_musical_character_kslow             = "Slow";

  const AgsVstCString ags_vst_musical_character_kshort            = "Short";
  const AgsVstCString ags_vst_musical_character_klong             = "Long";

  const AgsVstCString ags_vst_musical_character_kattack           = "Attack";
  const AgsVstCString ags_vst_musical_character_krelease          = "Release";

  const AgsVstCString ags_vst_musical_character_kdecay            = "Decay";
  const AgsVstCString ags_vst_musical_character_ksustain          = "Sustain";

  const AgsVstCString ags_vst_musical_character_kfast_attack      = "Fast Attack";
  const AgsVstCString ags_vst_musical_character_kslow_attack      = "Slow Attack";

  const AgsVstCString ags_vst_musical_character_kshort_release    = "Short Release";
  const AgsVstCString ags_vst_musical_character_klong_release     = "Long Release";
  const AgsVstCString ags_vst_musical_character_kstatic           = "Static";
  const AgsVstCString ags_vst_musical_character_kmoving           = "Moving";

  const AgsVstCString ags_vst_musical_character_kloop             = "Loop";
  const AgsVstCString ags_vst_musical_character_kone_shot         = "One Shot";

}
