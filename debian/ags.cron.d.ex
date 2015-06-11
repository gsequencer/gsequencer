#
# Regular cron jobs for the ags package
#
0 4	* * *	root	[ -x /usr/bin/ags_maintenance ] && /usr/bin/ags_maintenance
