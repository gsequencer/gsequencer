/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#import "ags_audio_unit_plugin_window_controller.h"

#include <ags/app/machine/ags_audio_unit_bridge.h>

@interface AgsAudioUnitPluginWindowController ()
@end

@implementation AgsAudioUnitPluginWindowController 
- (void)windowDidLoad {
  [super windowDidLoad];
  // Implement this method to handle any initialization after your window controller’s window has been loaded from its nib file.
}
- (BOOL)windowShouldClose:(id)sender {
  return NO;
}
@end
