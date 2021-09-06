// File stuff:   
// we use gdbm to store it so we can quit the program several times a day without the hands of the day being processed twice
// we delete the file database daily

int
one (const struct dirent *unused)
{
  return 1;
}

void
historyfile_clear (HistoryFile * historyfile)
{
  memset (historyfile->tablename, '\0', MAX_FILENAME_LENGTH);
  memset (historyfile->players, '\0', MAX_SEATS * MAX_PLAYER_NAME_LENGTH);
  historyfile->offset = 0;
  historyfile->size = 0;
}

char *
historyfile_get_player (HistoryFile * hf, int i)
{
  if (i >= hf->size || i >= MAX_SEATS)
    return NULL;
  else
    return hf->players + MAX_PLAYER_NAME_LENGTH * i;
}

void
historyfile_put_player (HistoryFile * hf, const char *playername)
{
  assert (strlen (playername) > 0
	  && strlen (playername) < MAX_PLAYER_NAME_LENGTH);
  if (strlen (playername) == 0)	// empty name
    return;			// do nothing
  strncpy (hf->players + MAX_PLAYER_NAME_LENGTH * hf->size, playername,
	   MAX_PLAYER_NAME_LENGTH);
  hf->size++;
}
