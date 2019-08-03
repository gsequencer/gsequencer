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

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ibstream.h>

#include <pluginterfaces/base/ibstream.h>

extern "C" {

  AgsVstTUID*
  ags_vst_ibstream_get_iid()
  {
    extern const Steinberg::TUID IBStream__iid;

    return((AgsVstTUID *) (&IBStream__iid));
  }

  AgsVstTUID*
  ags_vst_isizeable_stream_get_iid()
  {
    extern const Steinberg::TUID ISizeableStream__iid;

    return((AgsVstTUID *) (&ISizeableStream__iid));
  }

}
