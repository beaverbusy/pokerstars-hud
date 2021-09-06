// changeable stuff:
#define PATH_TO_HISTORY_FILES "/home/archie/bin/c/pokerhud/link_to_hand_histories/"
#define PATH_TO_DATABASE_OF_STATS "/home/archie/bin/c/pokerhud/pokerhud_database_of_stats"
#define PATH_TO_DATABASE_OF_FILES "/home/archie/bin/c/pokerhud/pokerhud_database_of_files"

// time in sec between two checks whether hands were played
#define DELAY_BETWEEN_HANDS_UPDATES 5

// time in seconds before a table with no action is considered closed, and its players will not be displayed anymore
#define SECONDS_BEFORE_TABLE_IS_INACTIVE 360.0

#define HUD_VERSION "0.1"

#define MAX_PLAYER_NAME_LENGTH 30
#define MAX_FILENAME_LENGTH 128
// currently fails if MAX_SEATS > 9 in hands.h
#define MAX_SEATS 9
// max tables that could have been visited in last SECONDS_BEFORE_TABLE_IS_INACTIVE seconds:
#define MAX_TABLES_ACTIVE 50
