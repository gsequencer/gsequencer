/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/vst/ags_vst_audio_effect.h>
#include <vst/vstaudioeffect.h>

class AgsVstAudioEffect : public AudioEffect {
  private:
    std::atomic<int> ref_count;
};

extern "C" {

  AgsVstAudioEffect* ags_vst_audio_effect_new() {
    AgsVstAudioEffect *audio_effect;

    audio_effect = new AgsVstAudioEffect();
    audio_effect.ref_count = 1;

    return(audio_effect);
  }

  
}
