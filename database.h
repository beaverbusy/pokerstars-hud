// stats database stuff:

const uint32_t SIZEOF_HAND_NUMBER = sizeof (((Stat *) NULL)->hand_number);
const uint32_t SIZEOF_VPIP = sizeof (((Stat *) NULL)->vpip);
const uint32_t SIZEOF_PFR = sizeof (((Stat *) NULL)->pfr);
const uint32_t SIZEOF_THREE_BET = sizeof (((Stat *) NULL)->three_bet);
const uint32_t SIZEOF_CTHREE_BET = sizeof (((Stat *) NULL)->cthree_bet);
const uint32_t SIZEOF_STEAL = sizeof (((Stat *) NULL)->steal);
const uint32_t SIZEOF_CSTEAL = sizeof (((Stat *) NULL)->csteal);
const uint32_t SIZEOF_CBET = sizeof (((Stat *) NULL)->cbet);
const uint32_t SIZEOF_CCBET = sizeof (((Stat *) NULL)->ccbet);

const uint32_t OFFSET_HAND_NUMBER = 0;
const uint32_t OFFSET_VPIP = OFFSET_HAND_NUMBER + SIZEOF_HAND_NUMBER;
const uint32_t OFFSET_PFR = OFFSET_VPIP + SIZEOF_VPIP;
const uint32_t OFFSET_THREE_BET = OFFSET_PFR + SIZEOF_PFR;
const uint32_t OFFSET_CTHREE_BET = OFFSET_THREE_BET + SIZEOF_THREE_BET;
const uint32_t OFFSET_STEAL = OFFSET_CTHREE_BET + SIZEOF_CTHREE_BET;
const uint32_t OFFSET_CSTEAL = OFFSET_STEAL + SIZEOF_STEAL;
const uint32_t OFFSET_CBET = OFFSET_CSTEAL + SIZEOF_CSTEAL;
const uint32_t OFFSET_CCBET = OFFSET_CBET + SIZEOF_CBET;

void
stat_to_byte (char dest[sizeof (Stat)], Stat const *source)
{
  assert (OFFSET_CCBET + SIZEOF_CCBET <= sizeof (Stat));
  memcpy (dest + OFFSET_HAND_NUMBER, &(source->hand_number),
	  SIZEOF_HAND_NUMBER);
  memcpy (dest + OFFSET_VPIP, &(source->vpip), SIZEOF_VPIP);
  memcpy (dest + OFFSET_PFR, &(source->pfr), SIZEOF_PFR);
  memcpy (dest + OFFSET_THREE_BET, &(source->three_bet), SIZEOF_THREE_BET);
  memcpy (dest + OFFSET_CTHREE_BET, &(source->cthree_bet), SIZEOF_CTHREE_BET);
  memcpy (dest + OFFSET_STEAL, &(source->steal), SIZEOF_STEAL);
  memcpy (dest + OFFSET_CSTEAL, &(source->csteal), SIZEOF_CSTEAL);
  memcpy (dest + OFFSET_CBET, &(source->cbet), SIZEOF_CBET);
  memcpy (dest + OFFSET_CCBET, &(source->ccbet), SIZEOF_CCBET);
}

void
stat_from_byte (Stat * dest, const char source[sizeof (Stat)])
{
  memcpy (&(dest->hand_number), source + OFFSET_HAND_NUMBER,
	  SIZEOF_HAND_NUMBER);
  memcpy (&(dest->vpip), source + OFFSET_VPIP, SIZEOF_VPIP);
  memcpy (&(dest->pfr), source + OFFSET_PFR, SIZEOF_PFR);
  memcpy (&(dest->three_bet), source + OFFSET_THREE_BET, SIZEOF_THREE_BET);
  memcpy (&(dest->cthree_bet), source + OFFSET_CTHREE_BET, SIZEOF_CTHREE_BET);
  memcpy (&(dest->steal), source + OFFSET_STEAL, SIZEOF_STEAL);
  memcpy (&(dest->csteal), source + OFFSET_CSTEAL, SIZEOF_CSTEAL);
  memcpy (&(dest->cbet), source + OFFSET_CBET, SIZEOF_CBET);
  memcpy (&(dest->ccbet), source + OFFSET_CCBET, SIZEOF_CCBET);
}


void
hand_to_database (GDBM_FILE statsdb, Action hand[MAX_SEATS])
{
  for (int i = 0; i < MAX_SEATS; i++)
    {
      if (hand[i].name[0] == '\0')
	continue;		// empty name
      datum key = {.dptr = (hand[i].name),.dsize = 1 + strlen (hand[i].name)
      };
      if (gdbm_exists (statsdb, key) != 1)
	{			// first time this player is seen
	  Stat entry_new = {.hand_number = 1,
	    .vpip = (hand[i].vpip) ? 1 : 0,
	    .pfr = (hand[i].pfr) ? 1 : 0,
	    .three_bet = (hand[i].three_bet) ? 1 : 0,
	    .cthree_bet = (hand[i].cthree_bet) ? 1 : 0,
	    .steal = (hand[i].steal) ? 1 : 0,
	    .csteal = (hand[i].csteal) ? 1 : 0,
	    .cbet = (hand[i].cbet) ? 1 : 0,
	    .ccbet = (hand[i].ccbet) ? 1 : 0
	  };
	  char entry_new_byte[sizeof (Stat)];
	  stat_to_byte (entry_new_byte, &entry_new);
	  datum entry_new_datum = {.dptr = entry_new_byte,.dsize =
	      sizeof (Stat)
	  };
	  gdbm_store (statsdb, key, entry_new_datum, GDBM_REPLACE);
	}

      else
	{			// player already in database
	  datum entry_old_datum = gdbm_fetch (statsdb, key);
	  Stat entry_old;
	  stat_from_byte (&entry_old, entry_old_datum.dptr);
	  free (entry_old_datum.dptr);	// done with the this entry
	  entry_old.hand_number++;
	  if (hand[i].vpip)
	    entry_old.vpip++;
	  if (hand[i].pfr)
	    entry_old.pfr++;
	  if (hand[i].three_bet)
	    entry_old.three_bet++;
	  if (hand[i].cthree_bet)
	    entry_old.cthree_bet++;
	  if (hand[i].steal)
	    entry_old.steal++;
	  if (hand[i].csteal)
	    entry_old.csteal++;
	  if (hand[i].cbet)
	    entry_old.cbet++;
	  if (hand[i].ccbet)
	    entry_old.ccbet++;

	  char entry_new_byte[sizeof (Stat)];
	  stat_to_byte (entry_new_byte, &entry_old);
	  datum entry_new_datum = {.dptr = entry_new_byte,.dsize =
	      sizeof (Stat)
	  };
	  gdbm_store (statsdb, key, entry_new_datum, GDBM_REPLACE);
	}
    }
}

int
stat_from_database (GDBM_FILE statsdb, Stat * stat, char *playername)
{
  datum key = {.dptr = playername,.dsize = 1 + strlen (playername) };
  if (gdbm_exists (statsdb, key) != 1)
    return -1;			// entry not exist
  datum stat_byte = gdbm_fetch (statsdb, key);
  stat_from_byte (stat, stat_byte.dptr);
  free (stat_byte.dptr);	// done with the this entry
  return 0;
}


// history file db stuff:


const uint32_t SIZEOF_TABLENAME = sizeof (char) * MAX_FILENAME_LENGTH;
const uint32_t SIZEOF_PLAYERS =
  sizeof (char) * MAX_SEATS * MAX_PLAYER_NAME_LENGTH;
const uint32_t SIZEOF_OFFSET = sizeof (long int);
const uint32_t SIZEOF_SIZE = sizeof (int);

const uint32_t OFFSET_TABLENAME = 0;
const uint32_t OFFSET_PLAYERS = OFFSET_TABLENAME + SIZEOF_TABLENAME;
const uint32_t OFFSET_OFFSET = OFFSET_PLAYERS + SIZEOF_PLAYERS;
const uint32_t OFFSET_SIZE = OFFSET_OFFSET + SIZEOF_OFFSET;

void
historyFile_to_byte (char dest[sizeof (HistoryFile)],
		     HistoryFile const *source)
{
  assert (sizeof (HistoryFile) >= OFFSET_SIZE + SIZEOF_SIZE);
  memcpy (dest + OFFSET_TABLENAME, &(source->tablename), SIZEOF_TABLENAME);
  memcpy (dest + OFFSET_PLAYERS, &(source->players), SIZEOF_PLAYERS);
  memcpy (dest + OFFSET_OFFSET, &(source->offset), SIZEOF_OFFSET);
  memcpy (dest + OFFSET_SIZE, &(source->size), SIZEOF_SIZE);
}

void
historyFile_from_byte (HistoryFile * dest,
		       const char source[sizeof (HistoryFile)])
{
  memcpy (&(dest->tablename), source + OFFSET_TABLENAME, SIZEOF_TABLENAME);
  memcpy (&(dest->players), source + OFFSET_PLAYERS, SIZEOF_PLAYERS);
  memcpy (&(dest->offset), source + OFFSET_OFFSET, SIZEOF_OFFSET);
  memcpy (&(dest->size), source + OFFSET_SIZE, SIZEOF_SIZE);
}


void
historyFile_to_database (GDBM_FILE filesdb, HistoryFile * hf)
{
  datum key = {.dptr = hf->tablename,.dsize = 1 + strlen (hf->tablename) };
  char entry_new_byte[sizeof (HistoryFile)];
  historyFile_to_byte (entry_new_byte, hf);
  datum entry_new_datum = {.dptr = entry_new_byte,.dsize =
      sizeof (HistoryFile)
  };
  gdbm_store (filesdb, key, entry_new_datum, GDBM_REPLACE);
}


int
historyFile_from_database (GDBM_FILE filesdb, char *tablename,
			   HistoryFile * hf)
{
  datum key = {.dptr = tablename,.dsize = 1 + strlen (tablename) };
  if (gdbm_exists (filesdb, key) != 1)
    return -1;			// entry not exist
  datum entry_old_datum = gdbm_fetch (filesdb, key);
  historyfile_clear (hf);
  historyFile_from_byte (hf, entry_old_datum.dptr);
  free (entry_old_datum.dptr);
  return 0;
}
