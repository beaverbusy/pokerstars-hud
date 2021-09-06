void
printing (GDBM_FILE statsdb, Activeplayers * activeplayers)
{
  system ("clear");
  printf ("%-15s  %-5s  %-5s %-5s %-5s %-5s %-5s", "   Player   ", "Hands",
	  "Vpip", "Pfr", "3Bet", "Steal", "Cbet");
  printf ("   %-15s   %-5s  %-5s %-5s %-5s %-5s %-5s\n", "   Player   ",
	  "Hands", "Vpip", "Pfr", "3Bet", "Steal", "Cbet");
  Stat stat;
  int bound = 0;
  // even/odd number of players
  if (activeplayers->size % 2 == 0)
    bound = (activeplayers->size - 2) / 2;
  else
    bound = (activeplayers->size - 2) / 2;
  for (int i = 0; i <= bound; i++)
    {
//      if (activeplayers_get (activeplayers, i) == NULL)
//      continue;

//      if (stat_from_database
//        (statsdb, &stat, activeplayers_get (activeplayers, i)) != -1)
//        get stat no check, print left then right column
      stat_from_database (statsdb, &stat,
			  activeplayers_get (activeplayers, i));
      printf ("%-15s |", activeplayers_get (activeplayers, i));
      stat_print (&stat);
      stat_from_database (statsdb, &stat,
			  activeplayers_get (activeplayers, bound + 1 + i));
      printf ("   %-15s |", activeplayers_get (activeplayers, bound + 1 + i));
      stat_print (&stat);
      printf ("\n");
    }

// last guy printed on last line
  if (activeplayers->size % 2 != 0)
    {
      stat_from_database (statsdb, &stat,
			  activeplayers_get (activeplayers,
					     activeplayers->size - 1));
      printf("                                                         ");
      printf ("%-15s |",
	      activeplayers_get (activeplayers, activeplayers->size - 1));
      stat_print (&stat);
      printf ("\n");
    }

//  printf ("\n\n\n Pokerstars Hud v%s.\nPress Ctrl-c to quit.\n", HUD_VERSION);
//  printf ("\n");
}

void
stat_print (const Stat * stat)
{
  printf
    ("%-5d | %-3.0f | %-3.0f | %-3.0f | %-3.0f | %-3.0f |",
     (stat->hand_number < 99999) ? stat->hand_number : 99999,
     (float) stat->vpip * 100 / stat->hand_number,
     (float) stat->pfr * 100 / stat->hand_number,
     stat->cthree_bet ==
     0 ? 0.0 : (float) stat->three_bet * 100 / stat->cthree_bet,
     stat->csteal == 0 ? 0.0 : (float) stat->steal * 100 / stat->csteal,
     stat->ccbet == 0 ? 0.0 : (float) stat->cbet * 100 / stat->ccbet);
}
