// active tables are stored in stack struct
//
void
activetables_clear (ActiveTables * activetables)
{
  memset (activetables->activetables, '\0',
	  MAX_TABLES_ACTIVE * MAX_FILENAME_LENGTH);
  activetables->size = 0;
}

int
activetables_put (ActiveTables * activetables, char *tablename)
{
  if (activetables->size >= MAX_TABLES_ACTIVE)
    return -1;

  strncpy (activetables->activetables +
	   activetables->size * MAX_FILENAME_LENGTH, tablename,
	   MAX_FILENAME_LENGTH);
  activetables->size++;
  return 0;
}

char *
activetables_get (ActiveTables * activetables, int i)
{
  if (i >= activetables->size || i >= MAX_TABLES_ACTIVE)
    return NULL;
  return activetables->activetables + i * MAX_FILENAME_LENGTH;
}


// update which tables still active
int
activetables_update (ActiveTables * activetables)
{
  // set all tables to inactive
  activetables_clear (activetables);
  struct stat buf;
  struct stat *pbuf = &buf;
  struct dirent **eps;
  int n = scandir (PATH_TO_HISTORY_FILES, &eps, one, alphasort);
  if (n >= 0)
    {
      int cnt;
      for (cnt = 0; cnt < n; ++cnt)
	{
	  assert (strlen (eps[cnt]->d_name) < MAX_FILENAME_LENGTH);
	  char filename[2 * MAX_FILENAME_LENGTH] = PATH_TO_HISTORY_FILES;
	  strncat (filename, eps[cnt]->d_name, MAX_FILENAME_LENGTH);
	  stat (filename, pbuf);
	  if (difftime (time (NULL), pbuf->st_mtime) <
	      SECONDS_BEFORE_TABLE_IS_INACTIVE
	      && strstr (eps[cnt]->d_name, "HH") != NULL)
	    activetables_put (activetables, eps[cnt]->d_name);
	}

      // free the allocated stuff
      while (n--)
	{
	  free (eps[n]);
	}
      free (eps);
      return 0;
    }
  else
    {
      fprintf (stderr, "Couldn't open the hand history directory\n");
      return -1;
    }

}
