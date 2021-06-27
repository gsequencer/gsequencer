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

#include <ags/audio/ags_fm_synth_util.h>

#include <ags/audio/ags_synth_enums.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <math.h>
#include <complex.h>

gpointer ags_fm_synth_util_strct_copy(gpointer ptr);
void ags_fm_synth_util_strct_free(gpointer ptr);

/**
 * SECTION:ags_fm_synth_util
 * @short_description: frequency modulation synth util
 * @title: AgsFMSynthUtil
 * @section_id:
 * @include: ags/audio/ags_fm_synth_util.h
 *
 * Utility functions to compute FM synths.
 */

GType
ags_fm_synth_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fm_synth_util = 0;

    ags_type_fm_synth_util =
      g_boxed_type_register_static("AgsFMSynthUtil",
				   (GBoxedCopyFunc) ags_fm_synth_util_strct_copy,
				   (GBoxedFreeFunc) ags_fm_synth_util_strct_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fm_synth_util);
  }

  return g_define_type_id__volatile;
}

gpointer
ags_fm_synth_util_strct_copy(gpointer ptr)
{
  gpointer retval;

  retval = g_memdup(ptr, sizeof(AgsFMSynthUtil));
 
  return(retval);
}

void
ags_fm_synth_util_strct_free(gpointer ptr)
{
  g_free(ptr);
}

/**
 * ags_fm_synth_util_sin_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate sin wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_sin_s8(gint8 *buffer,
			 gdouble freq, gdouble phase, gdouble volume,
			 guint samplerate,
			 guint offset, guint n_frames,
			 guint lfo_osc_mode,
			 gdouble lfo_freq, gdouble lfo_depth,
			 gdouble tuning)
{
  static const gdouble scale = 127.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  { 
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {   
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_sin_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate sin wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_sin_s16(gint16 *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames,
			  guint lfo_osc_mode,
			  gdouble lfo_freq, gdouble lfo_depth,
			  gdouble tuning)
{
  static const gdouble scale = 32767.0;
  guint i;

  if(buffer == NULL){
    return;
  }
    
  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {    
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffff & ((gint32) buffer[i] + (gint32) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffff & ((gint32) buffer[i] + (gint32) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffff & ((gint32) buffer[i] + (gint32) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffff & ((gint32) buffer[i] + (gint32) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffff & ((gint32) buffer[i] + (gint32) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_sin_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate sin wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_sin_s24(gint32 *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames,
			  guint lfo_osc_mode,
			  gdouble lfo_freq, gdouble lfo_depth,
			  gdouble tuning)
{
  static const gdouble scale = 8388607.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_sin_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate sin wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_sin_s32(gint32 *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames,
			  guint lfo_osc_mode,
			  gdouble lfo_freq, gdouble lfo_depth,
			  gdouble tuning)
{
  static const gdouble scale = 214748363.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_sin_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate sin wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_sin_s64(gint64 *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate,
			  guint offset, guint n_frames,
			  guint lfo_osc_mode,
			  gdouble lfo_freq, gdouble lfo_depth,
			  gdouble tuning)
{
  static const gdouble scale = 9223372036854775807.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) * scale * volume)));
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_sin_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate sin wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_sin_float(gfloat *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames,
			    guint lfo_osc_mode,
			    gdouble lfo_freq, gdouble lfo_depth,
			    gdouble tuning)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (float) ((gdouble) buffer[i] + (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (float) ((gdouble) buffer[i] + (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (float) ((gdouble) buffer[i] + (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (float) ((gdouble) buffer[i] + (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (float) ((gdouble) buffer[i] + (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) * volume));
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_sin_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate sin wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_sin_double(gdouble *buffer,
			     gdouble freq, gdouble phase, gdouble volume,
			     guint samplerate,
			     guint offset, guint n_frames,
			     guint lfo_osc_mode,
			     gdouble lfo_freq, gdouble lfo_depth,
			     gdouble tuning)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (double) ((gdouble) buffer[i] + (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (double) ((gdouble) buffer[i] + (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (double) ((gdouble) buffer[i] + (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (double) ((gdouble) buffer[i] + (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (double) ((gdouble) buffer[i] + (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) * volume));
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_sin_complex:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate sin wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_sin_complex(AgsComplex *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames,
			      guint lfo_osc_mode,
			      gdouble lfo_freq, gdouble lfo_depth,
			      gdouble tuning)
{
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;

  double y;

  guint i_stop;
  guint i;

  if(buffer == NULL){
    return;
  }

  c_ptr = buffer;
  c_ptr_ptr = &c_ptr;
  
  i_stop = offset + n_frames;

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < i_stop; i++, c_ptr++){
      y  = (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) * volume);

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < i_stop; i++, c_ptr++){
      y = (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) * volume);

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < i_stop; i++, c_ptr++){
      y = (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) * volume);

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < i_stop; i++, c_ptr++){
      y = (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) * volume);

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < i_stop; i++, c_ptr++){
      y = (gdouble) (sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) * volume);

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_sawtooth_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate sawtooth wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_sawtooth_s8(gint8 *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames,
			      guint lfo_osc_mode,
			      gdouble lfo_freq, gdouble lfo_depth,
			      gdouble tuning)
{
  static const gdouble scale = 127.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_sawtooth_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate sawtooth wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_sawtooth_s16(gint16 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       guint lfo_osc_mode,
			       gdouble lfo_freq, gdouble lfo_depth,
			       gdouble tuning)
{
  static const gdouble scale = 32767.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_sawtooth_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate sawtooth wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_sawtooth_s24(gint32 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       guint lfo_osc_mode,
			       gdouble lfo_freq, gdouble lfo_depth,
			       gdouble tuning)
{
  static const gdouble scale = 8388607.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_sawtooth_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate sawtooth wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_sawtooth_s32(gint32 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       guint lfo_osc_mode,
			       gdouble lfo_freq, gdouble lfo_depth,
			       gdouble tuning)
{
  static const gdouble scale = 214748363.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_sawtooth_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate sawtooth wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_sawtooth_s64(gint64 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       guint lfo_osc_mode,
			       gdouble lfo_freq, gdouble lfo_depth,
			       gdouble tuning)
{
  static const gdouble scale = 9223372036854775807.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate) - 1.0) * scale * volume)));
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_sawtooth_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate sawtooth wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_sawtooth_float(gfloat *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames,
				 guint lfo_osc_mode,
				 gdouble lfo_freq, gdouble lfo_depth,
				 gdouble tuning)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = ((double) buffer[i] + (double) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate) - 1.0) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = ((double) buffer[i] + (double) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate) - 1.0) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = ((double) buffer[i] + (double) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate) - 1.0) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = ((double) buffer[i] + (double) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate) - 1.0) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = ((double) buffer[i] + (double) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate) - 1.0) * volume));
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_sawtooth_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate sawtooth wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_sawtooth_double(gdouble *buffer,
				  gdouble freq, gdouble phase, gdouble volume,
				  guint samplerate,
				  guint offset, guint n_frames,
				  guint lfo_osc_mode,
				  gdouble lfo_freq, gdouble lfo_depth,
				  gdouble tuning)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = ((double) buffer[i] + (double) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate) - 1.0) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = ((double) buffer[i] + (double) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate) - 1.0) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = ((double) buffer[i] + (double) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate) - 1.0) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = ((double) buffer[i] + (double) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate) - 1.0) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = ((double) buffer[i] + (double) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate) - 1.0) * volume));
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_sawtooth_complex:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate sawtooth wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_sawtooth_complex(AgsComplex *buffer,
				   gdouble freq, gdouble phase, gdouble volume,
				   guint samplerate,
				   guint offset, guint n_frames,
				   guint lfo_osc_mode,
				   gdouble lfo_freq, gdouble lfo_depth,
				   gdouble tuning)
{
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;

  double y;

  guint i_stop;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  c_ptr = buffer;
  c_ptr_ptr = &c_ptr;

  i_stop = offset + n_frames;

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++, c_ptr++){
      y = (double) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate) - 1.0) * volume);

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++, c_ptr++){
      y = (double) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate) - 1.0) * volume);

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++, c_ptr++){
      y = (double) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate) - 1.0) * volume);

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++, c_ptr++){
      y = (double) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate) - 1.0) * volume);

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++, c_ptr++){
      y = (double) (((((int) ceil(i + phase) % (int) ceil(samplerate / freq)) * 2.0 * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate) - 1.0) * volume);

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_triangle_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate triangle wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_triangle_s8(gint8 *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames,
			      guint lfo_osc_mode,
			      gdouble lfo_freq, gdouble lfo_depth,
			      gdouble tuning)
{
  static const gdouble scale = 127.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) ((((phase + i) * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) ((((phase + i) * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_triangle_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate triangle wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_triangle_s16(gint16 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       guint lfo_osc_mode,
			       gdouble lfo_freq, gdouble lfo_depth,
			       gdouble tuning)
{
  static const gdouble scale = 32767.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) ((((phase + i) * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) ((((phase + i) * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_triangle_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate triangle wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_triangle_s24(gint32 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       guint lfo_osc_mode,
			       gdouble lfo_freq, gdouble lfo_depth,
			       gdouble tuning)
{
  static const gdouble scale = 8388607.0;
  gdouble current_freq;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) ((((phase + i) * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) ((((phase + i) * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_triangle_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate triangle wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_triangle_s32(gint32 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       guint lfo_osc_mode,
			       gdouble lfo_freq, gdouble lfo_depth,
			       gdouble tuning)
{
  static const gdouble scale = 214748363.0;
  guint i;

  if(buffer == NULL){
    return;
  }
  
  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) ((((phase + i) * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) ((((phase + i) * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_triangle_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate triangle wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_triangle_s64(gint64 *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       guint lfo_osc_mode,
			       gdouble lfo_freq, gdouble lfo_depth,
			       gdouble tuning)
{
  static const gdouble scale = 9223372036854775807.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) ((((phase + i) * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) ((((phase + i) * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * scale * volume)));
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_triangle_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate triangle wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_triangle_float(gfloat *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames,
				 guint lfo_osc_mode,
				 gdouble lfo_freq, gdouble lfo_depth,
				 gdouble tuning)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = ((double) buffer[i] + (double) ((((phase + i) * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = ((double) buffer[i] + (double) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = ((double) buffer[i] + (double) ((((phase + i) * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = ((double) buffer[i] + (double) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = ((double) buffer[i] + (double) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * volume));
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_triangle_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate triangle wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_triangle_double(gdouble *buffer,
				  gdouble freq, gdouble phase, gdouble volume,
				  guint samplerate,
				  guint offset, guint n_frames,
				  guint lfo_osc_mode,
				  gdouble lfo_freq, gdouble lfo_depth,
				  gdouble tuning)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = ((double) buffer[i] + (double) ((((phase + i) * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = ((double) buffer[i] + (double) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = ((double) buffer[i] + (double) ((((phase + i) * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = ((double) buffer[i] + (double) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * volume));
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      buffer[i] = ((double) buffer[i] + (double) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * volume));
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_triangle_complex:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate triangle wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_triangle_complex(AgsComplex *buffer,
				   gdouble freq, gdouble phase, gdouble volume,
				   guint samplerate,
				   guint offset, guint n_frames,
				   guint lfo_osc_mode,
				   gdouble lfo_freq, gdouble lfo_depth,
				   gdouble tuning)
{
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;

  double y;

  guint i_stop;
  guint i;

  if(buffer == NULL){
    return;
  }

  phase = (int) ceil(phase) % (int) ceil(freq);
  phase = ceil(phase / freq) * ceil(samplerate / freq);

  c_ptr = buffer;
  c_ptr_ptr = &c_ptr;

  i_stop = offset + n_frames;

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++, c_ptr++){
      y = (double) ((((phase + i) * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * volume);

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++, c_ptr++){
      y = (double) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * volume);

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++, c_ptr++){
      y = (double) ((((phase + i) * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * volume);

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++, c_ptr++){
      y = (double) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * volume);

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++, c_ptr++){
      y = (double) ((((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate * 2.0) - ((int) ((double) ((int) ((phase + i) * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / samplerate)) / 2.0) * 2) - 1.0) * volume);

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_square_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate square wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_square_s8(gint8 *buffer,
			    gdouble freq, gdouble phase, gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames,
			    guint lfo_osc_mode,
			    gdouble lfo_freq, gdouble lfo_depth,
			    gdouble tuning)
{
  static const gdouble scale = 127.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_square_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate square wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_square_s16(gint16 *buffer,
			     gdouble freq, gdouble phase, gdouble volume,
			     guint samplerate,
			     guint offset, guint n_frames,
			     guint lfo_osc_mode,
			     gdouble lfo_freq, gdouble lfo_depth,
			     gdouble tuning)
{
  static const gdouble scale = 32767.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_square_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate square wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_square_s24(gint32 *buffer,
			     gdouble freq, gdouble phase, gdouble volume,
			     guint samplerate,
			     guint offset, guint n_frames,
			     guint lfo_osc_mode,
			     gdouble lfo_freq, gdouble lfo_depth,
			     gdouble tuning)
{
  static const gdouble scale = 8388607.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_square_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate square wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_square_s32(gint32 *buffer,
			     gdouble freq, gdouble phase, gdouble volume,
			     guint samplerate,
			     guint offset, guint n_frames,
			     guint lfo_osc_mode,
			     gdouble lfo_freq, gdouble lfo_depth,
			     gdouble tuning)
{
  static const gdouble scale = 214748363.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_square_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate square wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_square_s64(gint64 *buffer,
			     gdouble freq, gdouble phase, gdouble volume,
			     guint samplerate,
			     guint offset, guint n_frames,
			     guint lfo_osc_mode,
			     gdouble lfo_freq, gdouble lfo_depth,
			     gdouble tuning)
{
  static const gdouble scale = 9223372036854775807.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_square_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate square wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_square_float(gfloat *buffer,
			       gdouble freq, gdouble phase, gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       guint lfo_osc_mode,
			       gdouble lfo_freq, gdouble lfo_depth,
			       gdouble tuning)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (buffer[i] + (1.0 * volume));
      }else{
	buffer[i] = (buffer[i] + (-1.0 * volume));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (buffer[i] + (1.0 * volume));
      }else{
	buffer[i] = (buffer[i] + (-1.0 * volume));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (buffer[i] + (1.0 * volume));
      }else{
	buffer[i] = (buffer[i] + (-1.0 * volume));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (buffer[i] + (1.0 * volume));
      }else{
	buffer[i] = (buffer[i] + (-1.0 * volume));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (buffer[i] + (1.0 * volume));
      }else{
	buffer[i] = (buffer[i] + (-1.0 * volume));
      }
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_square_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate square wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_square_double(gdouble *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames,
				guint lfo_osc_mode,
				gdouble lfo_freq, gdouble lfo_depth,
				gdouble tuning)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (buffer[i] + (1.0 * volume));
      }else{
	buffer[i] = (buffer[i] + (-1.0 * volume));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (buffer[i] + (1.0 * volume));
      }else{
	buffer[i] = (buffer[i] + (-1.0 * volume));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (buffer[i] + (1.0 * volume));
      }else{
	buffer[i] = (buffer[i] + (-1.0 * volume));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (buffer[i] + (1.0 * volume));
      }else{
	buffer[i] = (buffer[i] + (-1.0 * volume));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	buffer[i] = (buffer[i] + (1.0 * volume));
      }else{
	buffer[i] = (buffer[i] + (-1.0 * volume));
      }
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_square_complex:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate square wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_square_complex(AgsComplex *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames,
				 guint lfo_osc_mode,
				 gdouble lfo_freq, gdouble lfo_depth,
				 gdouble tuning)
{
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;

  double y;
  complex z0, z1;

  guint i_stop;
  guint i;

  if(buffer == NULL){
    return;
  }

  c_ptr = buffer;
  c_ptr_ptr = &c_ptr;
  
  i_stop = offset + n_frames;

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < i_stop; i++, c_ptr++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	y = (1.0 * volume);
      }else{
	y = (-1.0 * volume);
      }

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < i_stop; i++, c_ptr++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	y = (1.0 * volume);
      }else{
	y = (-1.0 * volume);
      }

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < i_stop; i++, c_ptr++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	y = (1.0 * volume);
      }else{
	y = (-1.0 * volume);
      }

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < i_stop; i++, c_ptr++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	y = (1.0 * volume);
      }else{
	y = (-1.0 * volume);
      }

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < i_stop; i++, c_ptr++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= 0.0){
	y = (1.0 * volume);
      }else{
	y = (-1.0 * volume);
      }

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_impulse_s8:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate impulse wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_impulse_s8(gint8 *buffer,
			     gdouble freq, gdouble phase, gdouble volume,
			     guint samplerate,
			     guint offset, guint n_frames,
			     guint lfo_osc_mode,
			     gdouble lfo_freq, gdouble lfo_depth,
			     gdouble tuning)
{
  static const gdouble scale = 127.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint8) (0xff & ((gint16) buffer[i] + (gint16) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_impulse_s16:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate impulse wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_impulse_s16(gint16 *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames,
			      guint lfo_osc_mode,
			      gdouble lfo_freq, gdouble lfo_depth,
			      gdouble tuning)
{
  static const gdouble scale = 32767.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint16) (0xffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_impulse_s24:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate impulse wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_impulse_s24(gint32 *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames,
			      guint lfo_osc_mode,
			      gdouble lfo_freq, gdouble lfo_depth,
			      gdouble tuning)
{
  static const gdouble scale = 8388607.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint32) (0xffffffff & ((gint32) buffer[i] + (gint32) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_impulse_s32:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate impulse wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_impulse_s32(gint32 *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames,
			      guint lfo_osc_mode,
			      gdouble lfo_freq, gdouble lfo_depth,
			      gdouble tuning)
{
  static const gdouble scale = 214748363.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint32) (0xffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_impulse_s64:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate impulse wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_impulse_s64(gint64 *buffer,
			      gdouble freq, gdouble phase, gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames,
			      guint lfo_osc_mode,
			      gdouble lfo_freq, gdouble lfo_depth,
			      gdouble tuning)
{
  static const gdouble scale = 9223372036854775807.0;
  guint i;

  if(buffer == NULL){
    return;
  }

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (1.0 * scale * volume)));
      }else{
	buffer[i] = (gint64) (0xffffffffffffffff & ((gint64) buffer[i] + (gint64) (-1.0 * scale * volume)));
      }
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_impulse_float:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate impulse wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_impulse_float(gfloat *buffer,
				gdouble freq, gdouble phase, gdouble volume,
				guint samplerate,
				guint offset, guint n_frames,
				guint lfo_osc_mode,
				gdouble lfo_freq, gdouble lfo_depth,
				gdouble tuning)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (buffer[i] + (1.0 * volume));
      }else{
	buffer[i] = (buffer[i] + (-1.0 * volume));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (buffer[i] + (1.0 * volume));
      }else{
	buffer[i] = (buffer[i] + (-1.0 * volume));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (buffer[i] + (1.0 * volume));
      }else{
	buffer[i] = (buffer[i] + (-1.0 * volume));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (buffer[i] + (1.0 * volume));
      }else{
	buffer[i] = (buffer[i] + (-1.0 * volume));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (buffer[i] + (1.0 * volume));
      }else{
	buffer[i] = (buffer[i] + (-1.0 * volume));
      }
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_impulse_double:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate impulse wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_impulse_double(gdouble *buffer,
				 gdouble freq, gdouble phase, gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames,
				 guint lfo_osc_mode,
				 gdouble lfo_freq, gdouble lfo_depth,
				 gdouble tuning)
{
  guint i;

  if(buffer == NULL){
    return;
  }

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (buffer[i] + (1.0 * volume));
      }else{
	buffer[i] = (buffer[i] + (-1.0 * volume));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (buffer[i] + (1.0 * volume));
      }else{
	buffer[i] = (buffer[i] + (-1.0 * volume));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (buffer[i] + (1.0 * volume));
      }else{
	buffer[i] = (buffer[i] + (-1.0 * volume));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (buffer[i] + (1.0 * volume));
      }else{
	buffer[i] = (buffer[i] + (-1.0 * volume));
      }
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	buffer[i] = (buffer[i] + (1.0 * volume));
      }else{
	buffer[i] = (buffer[i] + (-1.0 * volume));
      }
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_impulse_complex:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 * 
 * Generate frequency modulate impulse wave.
 * 
 * Since: 3.0.0
 */
void
ags_fm_synth_util_impulse_complex(AgsComplex *buffer,
				  gdouble freq, gdouble phase, gdouble volume,
				  guint samplerate,
				  guint offset, guint n_frames,
				  guint lfo_osc_mode,
				  gdouble lfo_freq, gdouble lfo_depth,
				  gdouble tuning)
{
  AgsComplex *c_ptr;
  AgsComplex **c_ptr_ptr;

  double y;

  guint i_stop;
  guint i;

  if(buffer == NULL){
    return;
  }

  c_ptr = buffer;
  c_ptr_ptr = &c_ptr;
  
  i_stop = offset + n_frames;

  switch(lfo_osc_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    for(i = offset; i < offset + n_frames; i++, c_ptr++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + sin(i * 2.0 * M_PI * lfo_freq / samplerate) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	y = (1.0 * volume);
      }else{
	y = (-1.0 * volume);
      }

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    for(i = offset; i < offset + n_frames; i++, c_ptr++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((((int) ceil(i) % (int) ceil(samplerate / lfo_freq)) * 2.0 * lfo_freq / samplerate) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	y = (1.0 * volume);
      }else{
	y = (-1.0 * volume);
      }

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    for(i = offset; i < offset + n_frames; i++, c_ptr++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + (((i) * lfo_freq / samplerate * 2.0) - ((int) ((double) ((int) ((i) * lfo_freq / samplerate)) / 2.0) * 2) - 1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	y = (1.0 * volume);
      }else{
	y = (-1.0 * volume);
      }

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    for(i = offset; i < offset + n_frames; i++, c_ptr++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= 0.0) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	y = (1.0 * volume);
      }else{
	y = (-1.0 * volume);
      }

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    for(i = offset; i < offset + n_frames; i++, c_ptr++){
      if(sin((gdouble) (i + phase) * 2.0 * M_PI * (freq * exp2(tuning / 1200.0 + ((sin((gdouble) (i) * 2.0 * M_PI * lfo_freq / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)) ? 1.0: -1.0) * lfo_depth)) / (gdouble) samplerate) >= sin(2.0 * M_PI * 3.0 / 5.0)){
	y = (1.0 * volume);
      }else{
	y = (-1.0 * volume);
      }

      AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(y, c_ptr_ptr);
    }
  }
  break;
  }
}

/**
 * ags_fm_synth_util_sin:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 *
 * Generate frequency modulate sin wave.
 *
 * Since: 3.0.0
 */
void
ags_fm_synth_util_sin(void *buffer,
		      gdouble freq, gdouble phase, gdouble volume,
		      guint samplerate, guint audio_buffer_util_format,
		      guint offset, guint n_frames,
		      guint lfo_osc_mode,
		      gdouble lfo_freq, gdouble lfo_depth,
		      gdouble tuning)
{
  if(buffer == NULL){
    return;
  }

  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_fm_synth_util_sin_s8((gint8 *) buffer,
			     freq, phase, volume,
			     samplerate,
			     offset, n_frames,
			     lfo_osc_mode,
			     lfo_freq, lfo_depth,
			     tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_fm_synth_util_sin_s16((gint16 *) buffer,
			      freq, phase, volume,
			      samplerate,
			      offset, n_frames,
			      lfo_osc_mode,
			      lfo_freq, lfo_depth,
			      tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_fm_synth_util_sin_s24((gint32 *) buffer,
			      freq, phase, volume,
			      samplerate,
			      offset, n_frames,
			      lfo_osc_mode,
			      lfo_freq, lfo_depth,
			      tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_fm_synth_util_sin_s32((gint32 *) buffer,
			      freq, phase, volume,
			      samplerate,
			      offset, n_frames,
			      lfo_osc_mode,
			      lfo_freq, lfo_depth,
			      tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_fm_synth_util_sin_s64((gint64 *) buffer,
			      freq, phase, volume,
			      samplerate,
			      offset, n_frames,
			      lfo_osc_mode,
			      lfo_freq, lfo_depth,
			      tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_fm_synth_util_sin_float((gfloat *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames,
				lfo_osc_mode,
				lfo_freq, lfo_depth,
				tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_fm_synth_util_sin_double((gdouble *) buffer,
				 freq, phase, volume,
				 samplerate,
				 offset, n_frames,
				 lfo_osc_mode,
				 lfo_freq, lfo_depth,
				 tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_fm_synth_util_sin_complex((AgsComplex *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames,
				  lfo_osc_mode,
				  lfo_freq, lfo_depth,
				  tuning);
  }
  break;
  default:
  {
    g_warning("ags_fm_synth_util_sin() - unsupported format");
  }
  }
}

/**
 * ags_fm_synth_util_sawtooth:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 *
 * Generate frequency modulate sawtooth wave.
 *
 * Since: 3.0.0
 */
void
ags_fm_synth_util_sawtooth(void *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate, guint audio_buffer_util_format,
			   guint offset, guint n_frames,
			   guint lfo_osc_mode,
			   gdouble lfo_freq, gdouble lfo_depth,
			   gdouble tuning)
{
  if(buffer == NULL){
    return;
  }

  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_fm_synth_util_sawtooth_s8((gint8 *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames,
				  lfo_osc_mode,
				  lfo_freq, lfo_depth,
				  tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_fm_synth_util_sawtooth_s16((gint16 *) buffer,
				   freq, phase, volume,
				   samplerate,
				   offset, n_frames,
				   lfo_osc_mode,
				   lfo_freq, lfo_depth,
				   tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_fm_synth_util_sawtooth_s24((gint32 *) buffer,
				   freq, phase, volume,
				   samplerate,
				   offset, n_frames,
				   lfo_osc_mode,
				   lfo_freq, lfo_depth,
				   tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_fm_synth_util_sawtooth_s32((gint32 *) buffer,
				   freq, phase, volume,
				   samplerate,
				   offset, n_frames,
				   lfo_osc_mode,
				   lfo_freq, lfo_depth,
				   tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_fm_synth_util_sawtooth_s64((gint64 *) buffer,
				   freq, phase, volume,
				   samplerate,
				   offset, n_frames,
				   lfo_osc_mode,
				   lfo_freq, lfo_depth,
				   tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_fm_synth_util_sawtooth_float((gfloat *) buffer,
				     freq, phase, volume,
				     samplerate,
				     offset, n_frames,
				     lfo_osc_mode,
				     lfo_freq, lfo_depth,
				     tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_fm_synth_util_sawtooth_double((gdouble *) buffer,
				      freq, phase, volume,
				      samplerate,
				      offset, n_frames,
				      lfo_osc_mode,
				      lfo_freq, lfo_depth,
				      tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_fm_synth_util_sawtooth_complex((AgsComplex *) buffer,
				       freq, phase, volume,
				       samplerate,
				       offset, n_frames,
				       lfo_osc_mode,
				       lfo_freq, lfo_depth,
				       tuning);
  }
  break;
  default:
  {
    g_warning("ags_fm_synth_util_sawtooth() - unsupported format");
  }
  }
}

/**
 * ags_fm_synth_util_triangle:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 *
 * Generate frequency modulate triangle wave.
 *
 * Since: 3.0.0
 */
void
ags_fm_synth_util_triangle(void *buffer,
			   gdouble freq, gdouble phase, gdouble volume,
			   guint samplerate, guint audio_buffer_util_format,
			   guint offset, guint n_frames,
			   guint lfo_osc_mode,
			   gdouble lfo_freq, gdouble lfo_depth,
			   gdouble tuning)
{
  if(buffer == NULL){
    return;
  }

  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_fm_synth_util_triangle_s8((gint8 *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames,
				  lfo_osc_mode,
				  lfo_freq, lfo_depth,
				  tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_fm_synth_util_triangle_s16((gint16 *) buffer,
				   freq, phase, volume,
				   samplerate,
				   offset, n_frames,
				   lfo_osc_mode,
				   lfo_freq, lfo_depth,
				   tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_fm_synth_util_triangle_s24((gint32 *) buffer,
				   freq, phase, volume,
				   samplerate,
				   offset, n_frames,
				   lfo_osc_mode,
				   lfo_freq, lfo_depth,
				   tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_fm_synth_util_triangle_s32((gint32 *) buffer,
				   freq, phase, volume,
				   samplerate,
				   offset, n_frames,
				   lfo_osc_mode,
				   lfo_freq, lfo_depth,
				   tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_fm_synth_util_triangle_s64((gint64 *) buffer,
				   freq, phase, volume,
				   samplerate,
				   offset, n_frames,
				   lfo_osc_mode,
				   lfo_freq, lfo_depth,
				   tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_fm_synth_util_triangle_float((gfloat *) buffer,
				     freq, phase, volume,
				     samplerate,
				     offset, n_frames,
				     lfo_osc_mode,
				     lfo_freq, lfo_depth,
				     tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_fm_synth_util_triangle_double((gdouble *) buffer,
				      freq, phase, volume,
				      samplerate,
				      offset, n_frames,
				      lfo_osc_mode,
				      lfo_freq, lfo_depth,
				      tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_fm_synth_util_triangle_complex((AgsComplex *) buffer,
				       freq, phase, volume,
				       samplerate,
				       offset, n_frames,
				       lfo_osc_mode,
				       lfo_freq, lfo_depth,
				       tuning);
  }
  break;
  default:
  {
    g_warning("ags_fm_synth_util_triangle() - unsupported format");
  }
  }
}

/**
 * ags_fm_synth_util_square:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 *
 * Generate frequency modulate square wave.
 *
 * Since: 3.0.0
 */
void
ags_fm_synth_util_square(void *buffer,
			 gdouble freq, gdouble phase, gdouble volume,
			 guint samplerate, guint audio_buffer_util_format,
			 guint offset, guint n_frames,
			 guint lfo_osc_mode,
			 gdouble lfo_freq, gdouble lfo_depth,
			 gdouble tuning)
{
  if(buffer == NULL){
    return;
  }

  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_fm_synth_util_square_s8((gint8 *) buffer,
				freq, phase, volume,
				samplerate,
				offset, n_frames,
				lfo_osc_mode,
				lfo_freq, lfo_depth,
				tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_fm_synth_util_square_s16((gint16 *) buffer,
				 freq, phase, volume,
				 samplerate,
				 offset, n_frames,
				 lfo_osc_mode,
				 lfo_freq, lfo_depth,
				 tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_fm_synth_util_square_s24((gint32 *) buffer,
				 freq, phase, volume,
				 samplerate,
				 offset, n_frames,
				 lfo_osc_mode,
				 lfo_freq, lfo_depth,
				 tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_fm_synth_util_square_s32((gint32 *) buffer,
				 freq, phase, volume,
				 samplerate,
				 offset, n_frames,
				 lfo_osc_mode,
				 lfo_freq, lfo_depth,
				 tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_fm_synth_util_square_s64((gint64 *) buffer,
				 freq, phase, volume,
				 samplerate,
				 offset, n_frames,
				 lfo_osc_mode,
				 lfo_freq, lfo_depth,
				 tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_fm_synth_util_square_float((gfloat *) buffer,
				   freq, phase, volume,
				   samplerate,
				   offset, n_frames,
				   lfo_osc_mode,
				   lfo_freq, lfo_depth,
				   tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_fm_synth_util_square_double((gdouble *) buffer,
				    freq, phase, volume,
				    samplerate,
				    offset, n_frames,
				    lfo_osc_mode,
				    lfo_freq, lfo_depth,
				    tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_fm_synth_util_square_complex((AgsComplex *) buffer,
				     freq, phase, volume,
				     samplerate,
				     offset, n_frames,
				     lfo_osc_mode,
				     lfo_freq, lfo_depth,
				     tuning);
  }
  break;
  default:
  {
    g_warning("ags_fm_synth_util_square() - unsupported format");
  }
  }
}

/**
 * ags_fm_synth_util_impulse:
 * @buffer: the audio buffer
 * @freq: the frequency of the sin wave
 * @phase: the phase of the sin wave
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 * @lfo_osc_mode: the LFO's oscillator mode
 * @lfo_freq: the LFO's frequency
 * @lfo_depth: the LFO's depth
 * @tuning: the tuninig
 *
 * Generate frequency modulate impulse wave.
 *
 * Since: 3.0.0
 */
void
ags_fm_synth_util_impulse(void *buffer,
			  gdouble freq, gdouble phase, gdouble volume,
			  guint samplerate, guint audio_buffer_util_format,
			  guint offset, guint n_frames,
			  guint lfo_osc_mode,
			  gdouble lfo_freq, gdouble lfo_depth,
			  gdouble tuning)
{
  if(buffer == NULL){
    return;
  }

  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_fm_synth_util_impulse_s8((gint8 *) buffer,
				 freq, phase, volume,
				 samplerate,
				 offset, n_frames,
				 lfo_osc_mode,
				 lfo_freq, lfo_depth,
				 tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_fm_synth_util_impulse_s16((gint16 *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames,
				  lfo_osc_mode,
				  lfo_freq, lfo_depth,
				  tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_fm_synth_util_impulse_s24((gint32 *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames,
				  lfo_osc_mode,
				  lfo_freq, lfo_depth,
				  tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_fm_synth_util_impulse_s32((gint32 *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames,
				  lfo_osc_mode,
				  lfo_freq, lfo_depth,
				  tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_fm_synth_util_impulse_s64((gint64 *) buffer,
				  freq, phase, volume,
				  samplerate,
				  offset, n_frames,
				  lfo_osc_mode,
				  lfo_freq, lfo_depth,
				  tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_fm_synth_util_impulse_float((gfloat *) buffer,
				    freq, phase, volume,
				    samplerate,
				    offset, n_frames,
				    lfo_osc_mode,
				    lfo_freq, lfo_depth,
				    tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_fm_synth_util_impulse_double((gdouble *) buffer,
				     freq, phase, volume,
				     samplerate,
				     offset, n_frames,
				     lfo_osc_mode,
				     lfo_freq, lfo_depth,
				     tuning);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_fm_synth_util_impulse_complex((AgsComplex *) buffer,
				      freq, phase, volume,
				      samplerate,
				      offset, n_frames,
				      lfo_osc_mode,
				      lfo_freq, lfo_depth,
				      tuning);
  }
  break;
  default:
  {
    g_warning("ags_fm_synth_util_impulse() - unsupported format");
  }
  }
}
