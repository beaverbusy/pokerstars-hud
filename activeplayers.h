void
activeplayers_clear (Activeplayers * activeplayers)
{
  memset (activeplayers->activeplayers, '\0',
	  MAX_TABLES_ACTIVE * MAX_SEATS * MAX_PLAYER_NAME_LENGTH);
  activeplayers->size = 0;
}

char *
activeplayers_get (Activeplayers * activeplayers, int pos)
{
  if (pos >= activeplayers->size)
    return NULL;
  return activeplayers->activeplayers + pos * MAX_PLAYER_NAME_LENGTH;
}

bool
activeplayers_isin (Activeplayers * activeplayers, char *playername)
{
  for (int i = 0; i < activeplayers->size; i++)
    {
      if (strncmp
	  (activeplayers->activeplayers + i * MAX_PLAYER_NAME_LENGTH,
	   playername, MAX_PLAYER_NAME_LENGTH) == 0)
	return true;
    }
  return false;
}

int
activeplayers_put (Activeplayers * activeplayers, char *playername)
{
  assert (activeplayers->size < MAX_TABLES_ACTIVE * MAX_SEATS);
  if (activeplayers->size >= MAX_TABLES_ACTIVE * MAX_SEATS)
    return -1;			// full

  strncpy (activeplayers->activeplayers +
	   MAX_PLAYER_NAME_LENGTH * activeplayers->size, playername,
	   MAX_PLAYER_NAME_LENGTH);
  activeplayers->size++;
  return 0;
}

int
compare_names (const void *v1, const void *v2)
{
  char *p1 = (char *) v1;
  char *p2 = (char *) v2;
  return strncasecmp (p1, p2, MAX_PLAYER_NAME_LENGTH);
}

void
activeplayers_sort (Activeplayers * activeplayers)
{
  assert (activeplayers->size < MAX_TABLES_ACTIVE * MAX_SEATS);
  qsort (activeplayers->activeplayers, activeplayers->size,
	 MAX_PLAYER_NAME_LENGTH, compare_names);
}

void
activeplayers_update (Activeplayers * activeplayers, GDBM_FILE filesdb,
		      ActiveTables * activetables)
{
  activeplayers_clear (activeplayers);
  HistoryFile historyfile;
  for (int i = 0; i < activetables->size; i++)
    {
      historyfile_clear (&historyfile);
      if (historyFile_from_database
	  (filesdb, activetables_get (activetables, i), &historyfile) != 0)
	continue;		// curiously table not found
      for (int j = 0; j < historyfile.size; j++)
	{
	  if (activeplayers_isin
	      (activeplayers,
	       historyfile_get_player (&historyfile, j)) == false)
	    // player not seen yet
	    activeplayers_put (activeplayers,
			       historyfile_get_player (&historyfile, j));

	}
    }
  activeplayers_sort (activeplayers);
}
