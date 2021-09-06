void
debug_stat_print (const Stat * stat)
{
  printf
    ("no: %d, vpip: %d, pfr: %d, 3b: %d, c3b: %d, st: %d, cst: %d, cb: %d, ccb: %d",
     stat->hand_number, stat->vpip, stat->pfr, stat->three_bet,
     stat->cthree_bet, stat->steal, stat->csteal, stat->cbet, stat->ccbet);
}

void
action_print (const Action * action)
{
  printf
    ("%13s, sb: %d, bb: %d, btn: %d, vpip: %d, pfr: %d, 3B: %d, c3B: %d, St: %d, cSt: %d, CB: %d, cCB: %d  \n",
     action->name, action->sb, action->bb, action->button, action->vpip,
     action->pfr, action->three_bet, action->cthree_bet, action->steal,
     action->csteal, action->cbet, action->ccbet);
}

void
hand_print (const Action hand[MAX_SEATS])
{
  for (int i = 0; i < MAX_SEATS; i++)
    {
      if (hand[i].name[0] != '\0')
	{			// non-empty seat
	  printf ("Seat %d: ", i + 1);
	  action_print (&hand[i]);
	}
    }
}

void
database_print (GDBM_FILE const dbf)
{

  datum key = gdbm_firstkey (dbf);
  printf ("db content:\n");
  while (key.dptr)
    {
      datum nextkey;
      datum entry_datum = gdbm_fetch (dbf, key);
      Stat entry_stat;
      stat_from_byte (&entry_stat, entry_datum.dptr);
      printf ("%s :", key.dptr);
      stat_print (&entry_stat);

      // Obtain the next key 
      nextkey = gdbm_nextkey (dbf, key);
      // Reclaim the memory used by the key 
      free (key.dptr);
      // Use nextkey in the next iteration. 
      key = nextkey;
    }
}

void
debug_activeplayers (Activeplayers * activeplayers)
{
  printf ("debug: activeplayers: \n");
  for (int i = 0; i < activeplayers->size; i++)
    printf ("%d. %s\n", i, activeplayers_get (activeplayers, i));
}

void
debug_activetables (ActiveTables * activetables)
{
  printf ("debug: active tables: \n");
  for (int i = 0; i < activetables->size; i++)
    printf ("%d. %s\n", i, activetables_get (activetables, i));
}
