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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_speaker.h>

#include <pluginterfaces/vst/vstspeaker.h>

extern "C" {

  gint32 ags_vst_speaker_get_channel_count(AgsVstSpeakerArrangement arr)
  {
    return(Steinberg::Vst::SpeakerArr::getChannelCount(arr));
  }

  gint32 ags_vst_speaker_get_speaker_index(AgsVstSpeaker speaker, AgsVstSpeakerArrangement arr)
  {
    return(Steinberg::Vst::SpeakerArr::getSpeakerIndex(speaker, arr));
  }
  
  AgsVstSpeaker ags_vst_speaker_get_speaker(AgsVstSpeakerArrangement *arr, gint32 index)
  {
    return(Steinberg::Vst::SpeakerArr::getSpeaker(const_cast<Steinberg::Vst::SpeakerArrangement&>(arr[0]), index));
  }

  gboolean ags_vst_speaker_is_subset_of(AgsVstSpeakerArrangement *arr_sub_set, AgsVstSpeakerArrangement *arr)
  {
    return(Steinberg::Vst::SpeakerArr::isSubsetOf(const_cast<Steinberg::Vst::SpeakerArrangement&>(arr_sub_set[0]), const_cast<Steinberg::Vst::SpeakerArrangement&>(arr[0])));
  }

  gboolean ags_vst_speaker_is_auro(AgsVstSpeakerArrangement *arr)
  {
    return(Steinberg::Vst::SpeakerArr::isAuro(const_cast<Steinberg::Vst::SpeakerArrangement&>(arr[0])));
  }
  
  gboolean ags_vst_speaker_has_top_speakers(AgsVstSpeakerArrangement *arr)
  {
    return(Steinberg::Vst::SpeakerArr::hasTopSpeakers(const_cast<Steinberg::Vst::SpeakerArrangement&>(arr[0])));
  }

  gboolean ags_vst_speaker_has_bottom_speakers(AgsVstSpeakerArrangement *arr)
  {
    return(Steinberg::Vst::SpeakerArr::hasBottomSpeakers(const_cast<Steinberg::Vst::SpeakerArrangement&>(arr[0])));
  }

  gboolean ags_vst_speaker_has_middle_speakers(AgsVstSpeakerArrangement *arr)
  {
    return(Steinberg::Vst::SpeakerArr::hasMiddleSpeakers(const_cast<Steinberg::Vst::SpeakerArrangement&>(arr[0])));
  }

  gboolean ags_vst_speaker_has_lfe(AgsVstSpeakerArrangement *arr)
  {
    return(Steinberg::Vst::SpeakerArr::hasLfe(const_cast<Steinberg::Vst::SpeakerArrangement&>(arr[0])));
  }

  gboolean ags_vst_speaker_is_3d(AgsVstSpeakerArrangement *arr)
  {
    return(Steinberg::Vst::SpeakerArr::is3D(const_cast<Steinberg::Vst::SpeakerArrangement&>(arr[0])));
  }

  gboolean ags_vst_speaker_is_ambisonics(AgsVstSpeakerArrangement *arr)
  {
    return(Steinberg::Vst::SpeakerArr::isAmbisonics(const_cast<Steinberg::Vst::SpeakerArrangement&>(arr[0])));
  }

  AgsVstSpeakerArrangement ags_vst_speaker_get_speaker_arrangement_from_string(AgsVstCString arr_str)
  {
    return((AgsVstSpeakerArrangement) Steinberg::Vst::SpeakerArr::getSpeakerArrangementFromString(arr_str));
  }

  AgsVstCString ags_vst_speaker_get_speaker_arrangement_string(AgsVstSpeakerArrangement arr,
							       gboolean with_speakers_name)
  {
    return((AgsVstCString) Steinberg::Vst::SpeakerArr::getSpeakerArrangementString(arr,
										   with_speakers_name));
  }

  AgsVstCString ags_vst_speaker_get_speaker_short_name(AgsVstSpeakerArrangement *arr,
						       gint32 index)
  {
    return((AgsVstCString) Steinberg::Vst::SpeakerArr::getSpeakerShortName(const_cast<Steinberg::Vst::SpeakerArrangement&>(arr[0]),
									   index));
  }

}
