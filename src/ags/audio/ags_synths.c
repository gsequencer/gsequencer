/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/ags_synths.h>
#include <math.h>

short scale = 32000;

void
ags_synth_sin(AgsSoundcard *soundcard, signed short *buffer, guint offset,
	      guint freq, guint phase, guint length,
	      double volume)
{
  guint samplerate;
  guint i;

  ags_soundcard_get_presets(soundcard,
			    NULL,
			    &samplerate,
			    NULL,
			    NULL);

  for (i = offset; i < offset + length; i++){
    buffer[i] = (signed short) (0xffff & ((int)buffer[i] + (int)(sin ((double)(i + phase) * 2.0 * M_PI * freq / (double)samplerate) * (double)scale * volume)));
  }
}

void
ags_synth_saw(AgsSoundcard *soundcard, signed short *buffer, guint offset, guint freq, guint phase, guint length, double volume)
{
  guint samplerate;
  guint i;

  ags_soundcard_get_presets(soundcard,
			    NULL,
			    &samplerate,
			    NULL,
			    NULL);

  phase = (int)(phase) % (int)(freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < length; i++){
    buffer[i] = (signed short) (0xffff & ((int)buffer[i] + (int)(((((int)(i + phase) % (int)(samplerate / freq)) * 2.0 * freq / samplerate) -1) * scale * volume)));
  }
}

void
ags_synth_triangle(AgsSoundcard *soundcard, signed short *buffer, guint offset, guint freq, guint phase, guint length, double volume)
{
  guint samplerate;
  guint i;
  
  ags_soundcard_get_presets(soundcard,
			    NULL,
			    &samplerate,
			    NULL,
			    NULL);

  phase = (int)(phase) % (int)(freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < length; i++){
    buffer[i] = (signed short) (0xffff & ((int)buffer[i] + (int)((((phase + i) * freq / samplerate * 2.0) - ((int)((float) ((int)((phase + i) * freq / samplerate)) / 2.0) * 2)  -1) * scale * volume)));
  }
}

void
ags_synth_square(AgsSoundcard *soundcard, signed short *buffer, guint offset, guint freq, guint phase, guint length, double volume)
{
  guint samplerate;
  guint i;
  
  ags_soundcard_get_presets(soundcard,
			    NULL,
			    &samplerate,
			    NULL,
			    NULL);

  phase = (int)(phase) % (int)(freq);
  phase = (phase / freq) * (samplerate / freq);

  for (i = offset; i < length; i++){
    if (sin (i + phase) >= 0.0){
      buffer[i] = (signed short) (0xffff & ((int)buffer[i] + (int)(1.0 * scale * volume)));
    }else{
      buffer[i] = (signed short) (0xffff & ((int)buffer[i] + (int)(-1.0 * scale * volume)));
    }
  }
}
