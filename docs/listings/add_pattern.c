#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_pattern.h>

AgsChannel *channel;
AgsPattern *pattern;

guint n_bank_0, n_bank_1;
guint length;

/* create channel */
channel = ags_channel_new(NULL);

/* create pattern, set dimension and add it to channel */
n_bank_0 = 4;
n_bank_1 = 12;

length = 64;

pattern = ags_pattern_new();
ags_pattern_set_dim(pattern,
		    n_bank_0,
		    n_bank_1,
		    length);
ags_channel_add_pattern(channel,
			pattern);
