// a hand is an array of Action of size MAX_SEATS
// recording the actions of every player in the hand
// Every Action is then processed into a Stat of the same player.
// This Stat is then recorded in the statsdb
//
void
hand_clear (Action hand[MAX_SEATS])
{				// clears all fields
  for (int i = 0; i < MAX_SEATS; i++)
    {
      memset (hand[i].name, (int) '\0', MAX_PLAYER_NAME_LENGTH);
      hand[i].sb = false;
      hand[i].bb = false;
      hand[i].button = false;
      hand[i].vpip = false;
      hand[i].pfr = false;
      hand[i].three_bet = false;
      hand[i].cthree_bet = false;
      hand[i].steal = false;
      hand[i].csteal = false;
//      hand[i].fsteal = false;
      hand[i].cbet = false;
      hand[i].ccbet = false;
//      hand[i].donk = false;
    }
}



int
hand_get_seat (const Action hand[MAX_SEATS], const char *name)
{				// returns -1 if none
  for (int i = 0; i < MAX_SEATS; i++)
    {
      if (strcmp (name, hand[i].name) == 0)
	return i;
    }

  return -1;
}

// returns -1 if no "PokerStars Hand #" is found, else 0
// if no "PokerStars Hand #" is found, input hand[MAX_SEATS] is untouched
//
int
hand_parse (FILE * fp, Action hand[MAX_SEATS])	// parses the next hand from fp, on first failure, fp will point to end of file
{
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  // get to start of hand if there is one
  while (1)
    {
      if (getline (&line, &len, fp) == -1)
	{
	  free (line);		// doc says free even if fail
	  return -1;		// reached end of file
	}
      else if (strstr (line, "PokerStars Hand #") != NULL)
	{			// we search for substring because begining of file starts with weird chars
	  break;
	}
    }
  // we are in a hand, so we can clear the hand
  hand_clear (hand);
  // get button position
  char button_seat = '0';
  if ((read = getline (&line, &len, fp)) != -1)
    {
      button_seat = *(strrchr (line, '#') + 1);
    }

  // set player names and who is button in hand;
  int index = 0;		// used for the array; counts players in order, does not equate to seat numbers as some seats might be empty
  // get to seat section
  while ((read = getline (&line, &len, fp)) != -1
	 && read > strlen ("Seat 1: ($ in chips)")
	 && 0 == memcmp (line, "Seat ", strlen ("Seat "))
	 && 0 == memcmp (line + (read - 12), "in chips)",
			 strlen ("in chips")) && index < MAX_SEATS)
    {
      char seat_number = *(line + strlen ("Seat "));	// todo fails if MAX_SEATS > 9
      char *name_begin = strchr (line, ':') + 2;
      *(strrchr (line, '(') - 1) = '\0';	// write '\0' at end of name; end of name is found from last '(' which should be robust.
      // now name_begin is a string of the player's name

      if (seat_number == button_seat)
	hand[index].button = true;

      assert (strlen (hand[index].name) < MAX_PLAYER_NAME_LENGTH);
      strncpy (hand[index].name, name_begin, MAX_PLAYER_NAME_LENGTH);
      index++;

    }
  // set small and big blinds in hand
  while (0 != memcmp (line, "*** HOLE CARDS ***",
		      strlen ("*** HOLE CARDS ***")))
    {

      const char delim[2] = " ";
      char *name_end = strrchr (line, ':');

      // only process line if it contains a "name:"
      if (name_end != NULL)
	{
	  *name_end = '\0';	// now line starts with string which is player's name
	  char *player = line;	// useless but clearer

	  char *action;
	  char *blind_type;

	  action = strtok (name_end + 2, delim);	// name_end is ':', +2 brings us to start of action word
	  blind_type = strtok (NULL, delim);

	  if (strcmp (action, "posts") == 0
	      && strcmp (blind_type, "small") == 0)
	    hand[hand_get_seat (hand, player)].sb = true;
	  else if (strcmp (action, "posts") == 0
		   && strcmp (blind_type, "big") == 0)
	    hand[hand_get_seat (hand, player)].bb = true;


	}
      getline (&line, &len, fp);
    }

//  getline (&line, &len, fp);  // skip the "Dealt to" line, not needed since we check whether "name:" appears in line.

  // set preflop action in hand
  int raisers = 0;
  int callers = 0;
  while ((read = getline (&line, &len, fp)) != -1 &&
	 0 != memcmp (line, "*** FLOP ***", strlen ("*** FLOP ***")) &&
	 0 != memcmp (line, "*** SUMMARY ***", strlen ("*** SUMMARY ***")))
    {

      char *name_end = strrchr (line, ':');

      // only proceed if line contains a "name:"
      if (name_end != NULL)
	{
	  *name_end = '\0';	// now line starts with string which is player's name
	  char *player = line;	// for clarity only
	  const char delim[2] = " ";
	  char *action = strtok (name_end + 2, delim);	// name_end is ':', +2 brings us to start of action word

	  // folds
	  if (0 == strncmp (action, "folds", 5))	// not sure if "folds" or "folds\n", hence 5
	    {
	      if (raisers == 1)	// could 3b
		hand[hand_get_seat (hand, player)].cthree_bet = true;
	      if (raisers == 0 && callers == 0 && hand[hand_get_seat (hand, player)].button == true)	// could steal
		hand[hand_get_seat (hand, player)].csteal = true;
	    }

	  //calls
	  else if (0 == strcmp (action, "calls"))
	    {
	      hand[hand_get_seat (hand, player)].vpip = true;
	      if (raisers == 1)	// could 3b
		hand[hand_get_seat (hand, player)].cthree_bet = true;
	      if (raisers == 0 && callers == 0 && hand[hand_get_seat (hand, player)].button == true)	// could steal
		hand[hand_get_seat (hand, player)].csteal = true;

	      callers++;
	    }

	  else if (0 == strcmp (action, "raises"))
	    {			// 3b occurs only after 1 raise was already done.
	      hand[hand_get_seat (hand, player)].vpip = true;
	      hand[hand_get_seat (hand, player)].pfr = true;
	      // 3 bettor:
	      if (raisers == 1)
		{
		  hand[hand_get_seat (hand, player)].three_bet = true;
		  hand[hand_get_seat (hand, player)].cthree_bet = true;
		}
	      // steal:
	      if (raisers == 0 && callers == 0
		  && hand[hand_get_seat (hand, player)].button == true)
		{		// steal
		  hand[hand_get_seat (hand, player)].csteal = true;
		  hand[hand_get_seat (hand, player)].steal = true;
		}
	      raisers++;
	    }
	}

    }
// flop
  if (0 == memcmp (line, "*** FLOP ***", strlen ("*** FLOP ***")))
    {				// we have a flop
      while ((read = getline (&line, &len, fp)) != -1 &&
	     0 != memcmp (line, "*** TURN ***", strlen ("*** TURN ***")) &&
	     0 != memcmp (line, "*** SUMMARY ***",
			  strlen ("*** SUMMARY ***")))
	{
	  char *name_end = strrchr (line, ':');

	  // only proceed if line contains a "name:"
	  if (name_end != NULL)
	    {
	      *name_end = '\0';	// now line starts with string which is player's name
	      char *player = line;
	      const char delim[2] = " ";
	      char *action = strtok (name_end + 2, delim);	// name_end is ':', +2 brings us to start of action word

	      // checks
	      if (0 == strncmp (action, "checks", 6))
		{
		  if (hand[hand_get_seat (hand, player)].pfr == true && raisers == 1)	// could cbet in single raised pot
		    hand[hand_get_seat (hand, player)].ccbet = true;
		}

	      if (0 == strcmp (action, "bets"))
		{
		  if (hand[hand_get_seat (hand, player)].pfr == true
		      && raisers == 1)
		    {		// could cbet
		      hand[hand_get_seat (hand, player)].cbet = true;
		      hand[hand_get_seat (hand, player)].ccbet = true;
		    }
		}



	    }


	}
    }

// note: we don't need go end of hand, that's taken care of by very first while loop
  free (line);
  return 0;			// success
}

int
hand_process_table (GDBM_FILE statsdb, GDBM_FILE filesdb, char *tablename)
{
  long int offset_recorded = 0;
  Action hand[MAX_SEATS];
  hand_clear (hand);
  HistoryFile historyfile;
  historyfile_clear (&historyfile);
  // check if recorded offset is nonzero
  if (historyFile_from_database (filesdb, tablename, &historyfile) != -1)	//file already in db
    offset_recorded = historyfile.offset;

  FILE *fp = NULL;
  char fullpath[2 * MAX_FILENAME_LENGTH] = PATH_TO_HISTORY_FILES;
  strncat (fullpath, tablename, MAX_FILENAME_LENGTH);
  fp = fopen (fullpath, "r");
  if (fp == NULL)
    {
      fprintf (stderr, "couldn't open history file: %s\n", tablename);
      return -1;
    }
  // move to end of file
  fseek (fp, 0, SEEK_END);

  // exit if no new hands since last time
  if (ftell (fp) == offset_recorded)
    return 0;
  // else move to previous recorded offset
  fseek (fp, offset_recorded, SEEK_SET);

  // process all new hands
  while (hand_parse (fp, hand) != -1)
    hand_to_database (statsdb, hand);
  assert (strlen (hand[0].name) > 0);

  // store new HistoryFile record
  historyfile_clear (&historyfile);
  strncpy (historyfile.tablename, tablename, MAX_FILENAME_LENGTH);
  for (int i = 0; i < MAX_SEATS; i++)
    if (strlen (hand[i].name) > 0)	// seat not-empty
      historyfile_put_player (&historyfile, hand[i].name);

  historyfile.offset = ftell (fp);
  historyFile_to_database (filesdb, &historyfile);

  fclose (fp);
  return 0;
}
