typedef struct Activeplayers
{
  char activeplayers[MAX_TABLES_ACTIVE * MAX_SEATS * MAX_PLAYER_NAME_LENGTH];
  int size;
} Activeplayers;

typedef struct ActiveTables
{
  char activetables[MAX_TABLES_ACTIVE * MAX_FILENAME_LENGTH];
  int size;
} ActiveTables;

typedef struct Action
{
  char name[MAX_PLAYER_NAME_LENGTH];
  bool sb;
  bool bb;
  bool button;
  bool vpip;
  bool pfr;
  bool three_bet;
  bool cthree_bet;
//  bool fthree_bet;
  bool steal;
  bool csteal;
//  bool fsteal;
  bool cbet;
  bool ccbet;
//  bool fcbet;
//  bool donk;
} Action;


typedef struct Stat
{
  unsigned int hand_number;
  unsigned int vpip;
  unsigned int pfr;
  unsigned int three_bet;
  unsigned int cthree_bet;
//  unsigned int fthree_bet;
  unsigned int steal;
  unsigned int csteal;
//  unsigned int fsteal;
  unsigned int cbet;
  unsigned int ccbet;
//  unsigned int fcbet;
//  unsigned int donk;
} Stat;


typedef struct HistoryFile
{
  char tablename[MAX_FILENAME_LENGTH];
  char players[MAX_SEATS * MAX_PLAYER_NAME_LENGTH];
  long int offset;
  int size;
} HistoryFile;

// functions
// activeplayers.h
void activeplayers_clear (Activeplayers * activeplayers);
char *activeplayers_get (Activeplayers * activeplayers, int pos);
bool activeplayers_isin (Activeplayers * activeplayers, char *playername);
int activeplayers_put (Activeplayers * activeplayers, char *playername);
int compare_names (const void *v1, const void *v2);
void activeplayers_sort (Activeplayers * activeplayers);
void
activeplayers_update (Activeplayers * activeplayers, GDBM_FILE filesdb,
		      ActiveTables * activetables);

// activetables.h
void activetables_clear (ActiveTables * activetables);
int activetables_put (ActiveTables * activetables, char *tablename);
char *activetables_get (ActiveTables * activetables, int i);
int one (const struct dirent *unused);
int activetables_update (ActiveTables * activetables);

// database.h
void stat_to_byte (char dest[sizeof (Stat)], Stat const *source);
void stat_from_byte (Stat * dest, const char source[sizeof (Stat)]);
void hand_to_database (GDBM_FILE statsdb, Action hand[MAX_SEATS]);
int stat_from_database (GDBM_FILE statsdb, Stat * stat, char *playername);
void
historyFile_to_byte (char dest[sizeof (HistoryFile)],
		     HistoryFile const *source);
void
historyFile_from_byte (HistoryFile * dest,
		       const char source[sizeof (HistoryFile)]);
void historyFile_to_database (GDBM_FILE filesdb, HistoryFile * hf);
int
historyFile_from_database (GDBM_FILE filesdb, char *tablename,
			   HistoryFile * hf);

// hands.h
void hand_clear (Action hand[MAX_SEATS]);
int hand_get_seat (const Action hand[MAX_SEATS], const char *name);
int hand_parse (FILE * fp, Action hand[MAX_SEATS]);
int
hand_process_table (GDBM_FILE statsdb, GDBM_FILE filesdb, char *tablename);

// historyfiles.h
int one (const struct dirent *unused);
void historyfile_clear (HistoryFile * historyfile);
char *historyfile_get_player (HistoryFile * hf, int i);
void historyfile_put_player (HistoryFile * hf, const char *playername);

// printing.h
void printing (GDBM_FILE statsdb, Activeplayers * activeplayers);
void stat_print (const Stat * stat);
