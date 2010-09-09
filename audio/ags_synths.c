#include "ags_synths.h"
#include <math.h>

short scale = 32000;

void
ags_synth_sin(AgsDevout *devout, short *buffer, guint offset, guint freq, guint phase, guint length, double volume)
{
  guint i;

  for (i = offset; i < offset + length; i++){
    buffer[i] = (short) (0xffff & ((int)buffer[i] + (int)(sin ((double)(i + phase) * 2.0 * M_PI * freq / (double)devout->frequency) * (double)scale * volume)));
  }
}

void
ags_synth_saw(AgsDevout *devout, short *buffer, guint offset, guint freq, guint phase, guint length, double volume)
{
  guint i;

  phase = (int)(phase) % (int)(freq);
  phase = (phase / freq) * (devout->frequency / freq);

  for (i = offset; i < length; i++){
    buffer[i] = (short) (0xffff & ((int)buffer[i] + (int)(((((int)(i + phase) % (int)(devout->frequency / freq)) * 2.0 * freq / devout->frequency) -1) * scale * volume)));
  }
}

void
ags_synth_triangle(AgsDevout *devout, short *buffer, guint offset, guint freq, guint phase, guint length, double volume)
{
  guint i;

  phase = (int)(phase) % (int)(freq);
  phase = (phase / freq) * (devout->frequency / freq);

  for (i = offset; i < length; i++){
    buffer[i] = (short) (0xffff & ((int)buffer[i] + (int)((((phase + i) * freq / devout->frequency * 2.0) - ((int)((float) ((int)((phase + i) * freq / devout->frequency)) / 2.0) * 2)  -1) * scale * volume)));
  }
}

void
ags_synth_square(AgsDevout *devout, short *buffer, guint offset, guint freq, guint phase, guint length, double volume)
{
  guint i;

  phase = (int)(phase) % (int)(freq);
  phase = (phase / freq) * (devout->frequency / freq);

  for (i = offset; i < length; i++){
    if (sin (i + phase) >= 0.0){
      buffer[i] = (short) (0xffff & ((int)buffer[i] + (int)(1.0 * scale * volume)));
    }else{
      buffer[i] = (short) (0xffff & ((int)buffer[i] + (int)(-1.0 * scale * volume)));
    }
  }
}
