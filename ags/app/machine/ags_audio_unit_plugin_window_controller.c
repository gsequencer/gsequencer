#import "ags_audio_unit_plugin_window_controller.h"

@interface AgsAudioUnitPluginWindowController ()
@end

@implementation AgsAudioUnitPluginWindowController 
- (void)windowDidLoad {
  [super windowDidLoad];
  // Implement this method to handle any initialization after your window controller’s window has been loaded from its nib file.
}
- (BOOL)windowShouldClose:(id)sender {
  [[NSApplication sharedApplication] hide: self];

  return NO;
}
@end
