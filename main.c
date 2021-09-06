// note: fixed length array in struct, get automatically deep copied by the compiler!, not a pointer copy
// 
//
// stats: https://drivehud.com/knowledgebase/what-are-the-hud-stat-definitions/
// VPIP         Voluntarily put in pot (preflop). Any money placed into the pot voluntarily, which does not include the blinds, or folding from the big blind to a raise.
// PFR  Pre-flop Raise. Anytime you make a raise before the flop.
// 3-Bet        Three Bet. Anytime there’s a 3rd raise in the pot. Pre-flop, this happens on what appears to be the second raise, but in reality, the first raise counts as two raises.
// our cbet is only calculated in single raised pots preflop. one could add a cbet for 3bet pots.
// add aggression factor = (bets + raises)/calls
// todo code to remove the filedb daily
// todo: valgrind done, 0 leaks
// todo write hand parser
// todo: prettyfy the printing, all to a string to avoid tearing, color, per table output?, depend on terminal size etc...
// todo fix need to link to handhistories
// todo write hud via xlib
// todo 1 db per type/stakes
// todo truncate names


// uncomment following line to disable all asserts 
//#define NDEBUG
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <gdbm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

#include "constants.h"
#include "declarations.h"
#include "historyfiles.h"
#include "database.h"
#include "hands.h"
#include "activetables.h"
#include "activeplayers.h"
#include "printing.h"
#include "debug.h"


/// quit on C-c
volatile sig_atomic_t keep_going = 1;
int failures = 0;
void
exit_cleanup (int sig)
{
  keep_going = 0;
}




int
main ()
{
  // quit cleanly on C-c
  signal (SIGINT, exit_cleanup);
//   databases: one for player stats, and an ephemeral one for files
  GDBM_FILE statsdb =
    gdbm_open (PATH_TO_DATABASE_OF_STATS, 10, GDBM_WRCREAT | GDBM_SYNC,
	       S_IRWXU, NULL);
  if (statsdb == NULL)
    {
      printf ("error: couldn't open db of hands\n");
      exit (2);
    }

  GDBM_FILE filesdb =
    gdbm_open (PATH_TO_DATABASE_OF_FILES, 10, GDBM_WRCREAT | GDBM_SYNC,
	       S_IRWXU, NULL);
  if (filesdb == NULL)
    {
      printf ("error: couldn't open db of files\n");
      gdbm_close (statsdb);	// close first db
      exit (2);
    }

// record active tables
  ActiveTables activetables;
  activetables_clear (&activetables);

// active players currenty on active tables
  Activeplayers activeplayers;
  activeplayers_clear (&activeplayers);

  while (1)
    {
      if (keep_going == 0 || failures > 10)
	goto do_exit_cleanup;

      // get current active tables
      if (activetables_update (&activetables) == -1) {
         failures++;
         goto wait_a_bit;	// problem reading hand history dir try again in a while
      }


      // process any new hands on active tables
      for (int i = 0; i < activetables.size; i++)
	hand_process_table (statsdb, filesdb,
			    activetables_get (&activetables, i));
      // get current active players
      activeplayers_update (&activeplayers, filesdb, &activetables);
      // print their stats
      printing (statsdb, &activeplayers);
      // take a nap
wait_a_bit:
      sleep (DELAY_BETWEEN_HANDS_UPDATES);
      // wash rinse repeat
    }

  // before quitting
do_exit_cleanup:
  gdbm_close (statsdb);		// could peridically call gdbm_sync although that is done by the GDBM_SYNC flag alread selon docs
  gdbm_close (filesdb);		// could peridically call gdbm_sync although that is done by the GDBM_SYNC flag alread selon docs
  system ("clear");
  printf ("Caught quit signal. Synced databases. Bye\n");
  printf("There were %d failures. Consult error-log in prog's directory\n", failures);
  return 0;
}
