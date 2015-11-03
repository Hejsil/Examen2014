/*
 * Programmer: Jimmi Holst Christensen	Date completed: 05-12-2014
 *
 * Loads in and process data about the danish superleague from a file
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_ROUNDS 33
#define MATCHES_PER_ROUND 6
#define MIN_SCORE 7
#define MAX_TEAMS 12
#define MAX_NAME_LENGTH 4
#define MAX_DATECLOCK_LENGTH 6
#define MAX_SPECTATOR_LENGTH 10
#define MAX_SCORE 3
#define SKIPPING 10
#define ASCII_0 48

/* strucks */
struct match_data {
    char day[MAX_NAME_LENGTH], date[MAX_DATECLOCK_LENGTH], time[MAX_DATECLOCK_LENGTH],
         home_team[MAX_NAME_LENGTH], away_team[MAX_NAME_LENGTH];
         
    int  home_score, away_score, total_score, spectators;
};

struct team_data{
    char team_name[MAX_NAME_LENGTH];
    
    int  home_wins, away_wins, matches_tied, matches_lost, matches_played, 
         goals_scored, goals_against, goal_difference,
         total_homefield_spectators,
         points;
};

typedef struct match_data match_data;
typedef struct team_data team_data;

/* function prototypes */
/* --- functions handling the loading and storing of data from the file --- */
void open_match_data(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND]);
void store_match_data(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], FILE *file_input);
match_data make_match_struct(FILE *file_input);
int convert_char_to_int(const char *spectators, int length, int multi);

/* --- functions handling the storing of all the team data --- */
void store_team_data(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], team_data teams[MAX_TEAMS]);
void store_team_names(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], team_data teams[MAX_TEAMS]);
void store_team_rest(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], team_data teams[MAX_TEAMS]);
int check_matchteam_against_teamnames(team_data teams[MAX_TEAMS], const char match_team_name[MAX_NAME_LENGTH], int teams_stored);

/* --- functions handling finding and processing the data loaded from the file --- */
void print_score_or_more(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], int min_score);
void print_highest_round_score(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND]);
void find_highest_round_score(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], int *highest_round_score, int *result);
int calc_round_score(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], int current_round);
void print_most_wins_awayfield(team_data teams[MAX_TEAMS]);
void print_team_lowest_homefield_spectators(team_data teams[MAX_TEAMS]);
int find_team_lowest_homefield_spectators(team_data teams[MAX_TEAMS]);
void print_team_table(team_data teams[MAX_TEAMS]);
void print_user_searched_teams(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], const char weekday[MAX_NAME_LENGTH], const char clock1[MAX_DATECLOCK_LENGTH], const char clock2[MAX_DATECLOCK_LENGTH]);
int check_memorysize_to_allocate(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], const char clock1[MAX_DATECLOCK_LENGTH], const char clock2[MAX_DATECLOCK_LENGTH], const char weekday[MAX_NAME_LENGTH]);
match_data *allocate_memo(int size);
void find_maches_in_time(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], match_data *matches_found, const char clock1[MAX_DATECLOCK_LENGTH], const char clock2[MAX_DATECLOCK_LENGTH], const char weekday[MAX_NAME_LENGTH]);
void print_all(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], team_data teams[MAX_TEAMS], const char weekday[MAX_NAME_LENGTH], const char clock1[MAX_DATECLOCK_LENGTH], const char clock2[MAX_DATECLOCK_LENGTH]);
int sort_matches(const void *ip1, const void *ip2);
int sort_teams(const void *ip1, const void *ip2);


int main(int argc, char *argv[]){
    match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND]; 
    team_data teams[MAX_TEAMS];
    int running = 1;
    char clock1[MAX_DATECLOCK_LENGTH] = "18.05", clock2[MAX_DATECLOCK_LENGTH] = "19.05", weekday[MAX_NAME_LENGTH] = "Fre",
         input;
    
    /* opening and storing all data from file */
    open_match_data(matches);
    store_team_data(matches, teams);
    
    /* checking if program was run with the "--print" programparameter */
    if(argc > 1 && strcmp(argv[1], "--print") == 0){
        print_all(matches, teams, weekday, clock1, clock2);
    } else {
        printf("### Welcome to the superleague info processer ###\n");
        
        /* program menu */
        do{
            printf("_________________________________________________\n");
            printf("1. teams with \"x\" score or more.\n");
            printf("2. round with highest goals scored.\n");
            printf("3. teams with most wins on away field.\n");
            printf("4. team with lowest home field spectators.\n");
            printf("5. search for matches.\n");
            printf("6. print table of teams sorted by points.\n");
            printf("7. exit.\n");
            
            /* user menu choice */
            scanf("%c", &input);
            fflush(stdin);
            
            switch (input){
                case '1':
                    do{
                        printf("Choose are score between 0 - 9.\n");
                        scanf("%c", &input);
                        fflush(stdin);
                    }while(input < '0' && input > '9');
                    
                    print_score_or_more(matches, input - ASCII_0);
                    break;
                case '2':
                    print_highest_round_score(matches);
                    break;
                case '3':
                    print_most_wins_awayfield(teams);
                    break;
                case '4':
                    print_team_lowest_homefield_spectators(teams);
                    break;
                case '5':
                    printf("What day should the matches be on (Man, Fre, Lor, Son):\n");
                    scanf("%s", weekday);
                    fflush(stdin);
                    
                    printf("\nBetween what two time periots should the matches be between (18.30 - 20.30)\n");
                    
                    do{
                        printf("The first time should be lower than the second time:\n");
                        scanf("%s - %s", clock1, clock2);
                        fflush(stdin);
                        printf("\n");
                    }while(strcmp(clock1, clock2) > -1);
                    
                    print_user_searched_teams(matches, weekday, clock1, clock2);
                    break;
                case '6':
                    print_team_table(teams);
                    break;
                case '7':
                    running = 0;
                    break;
                default:
                    printf("invalid input\n");
                }
        }while(running);
    }  
    
    return 0;
}


/* --- these functions handle the loading and storing of data from the file --- */
/* opens the match file and calls store_match_data */ 
void open_match_data(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND]){
    FILE *file_input;
    
    /* opening the txt file containing all match data */
    file_input = fopen("superliga-2013-2014.txt", "r");
    
    if (file_input == NULL){
        printf("Was not able to open \"superliga-2013-2014\". The program will exit.\n.");
        exit(0);
    }
    
    store_match_data(matches, file_input);
    
    fclose(file_input);
}

/* all data loaded from file in a 2 dimensional array of structs */ 
void store_match_data(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], FILE *file_input){
    int matches_loaded = 0;
    char input;
        
    /* loop running through the file until it hits EOF */
    while((input = fgetc(file_input)) != EOF){
        ungetc(input, file_input);
        
        /* stores data from file */
        matches[matches_loaded/MATCHES_PER_ROUND][matches_loaded%MATCHES_PER_ROUND] = make_match_struct(file_input);
        
        matches_loaded++;
    }
}

/* stores data from file in a structs, and returns it */ 
match_data make_match_struct(FILE *file_input){
    match_data result;
    char spectators[MAX_SPECTATOR_LENGTH];
    
    fscanf(file_input,"%s ", result.day);
    /* printf("%s ", result.day); */
        
    fscanf(file_input,"%s ", result.date);
    /* printf("%s ", result.date); */
        
    fscanf(file_input,"%s ", result.time);
    /* printf("%s ", result.time); */
        
    fscanf(file_input,"%s - %s ", result.home_team, result.away_team);
    /* printf("%s - %s ", result.home_team, result.away_team);*/
        
    fscanf(file_input,"%d - %d ", &result.home_score, &result.away_score);
    /* printf("%d - %d ", result.home_score, result.away_score);*/
    
    result.total_score = result.home_score + result.away_score;
        
    fscanf(file_input,"%s ", spectators);
    
    result.spectators = convert_char_to_int(spectators, strlen(spectators), 1);
    /*printf("%i\n", result.spectators);*/

    return result;
}

/* converts a string of numbers with dots in between to a interger */
int convert_char_to_int(const char *spectators, int length, int multi){
    if(length == 0)
        return 0;
    else {
        if(*(spectators + length - 1) == '.')
            length--;
        
        return (*(spectators + length - 1) - ASCII_0) * multi + convert_char_to_int(spectators, length - 1, multi * 10);
    }
}


/* --- these functions handle the storing of all the team data --- */
/* the core function for storing all the team data */
void store_team_data(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], team_data teams[MAX_TEAMS]){
    int i;
    
    /* assining all interger variables in the team array */
    for(i = 0; i < MAX_TEAMS; i++){
        teams[i].home_wins = 0;
        teams[i].away_wins = 0;
        teams[i].matches_lost = 0;
        teams[i].matches_tied = 0;
        teams[i].matches_played = 0;
        teams[i].goals_scored = 0;
        teams[i].goals_against = 0;
        teams[i].total_homefield_spectators = 0;
    }
    
    store_team_names(matches, teams);
    
    store_team_rest(matches, teams);
}

/* finds all teams stored in matches, and stores there names */
void store_team_names(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], team_data teams[MAX_TEAMS]){
    int j, i, teams_stored = 0;

    for(j = 0; j < MAX_ROUNDS; j++)
        for(i = 0; i < MATCHES_PER_ROUND; i++){
            if (check_matchteam_against_teamnames(teams, matches[j][i].home_team, teams_stored) == 0){
                strcpy(teams[teams_stored].team_name, matches[j][i].home_team);
                teams_stored++;
                break;
            }
            
            if (check_matchteam_against_teamnames(teams, matches[j][i].away_team, teams_stored) == 0){
                strcpy(teams[teams_stored].team_name, matches[j][i].away_team);
                teams_stored++;
                break;
            }
        }
}

/* finds the rest of the team data stored in matches, and stores them */
void store_team_rest(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], team_data teams[MAX_TEAMS]){
    int i, j, k;
    
    for(j = 0; j < MAX_ROUNDS; j++)
        for(i = 0; i < MATCHES_PER_ROUND; i++)
            for(k = 0; k < MAX_TEAMS; k++){
                /* checking what data is stored from team "k" in match "i" 
                   data from matches are stored differently depending on if team "k" is home team or away team */
                if(strcmp(teams[k].team_name, matches[j][i].home_team) == 0){
                    teams[k].matches_played += 1;
                    teams[k].goals_scored += matches[j][i].home_score;
                    teams[k].goals_against += matches[j][i].away_score;
                    teams[k].total_homefield_spectators += matches[j][i].spectators;
                    
                    /* stores wins, ties and loses */
                    if(matches[j][i].home_score > matches[j][i].away_score)
                        teams[k].home_wins += 1;
                    else if(matches[j][i].home_score < matches[j][i].away_score)
                        teams[k].matches_lost += 1;
                    else
                        teams[k].matches_tied += 1;
                    
                } else if(strcmp(teams[k].team_name, matches[j][i].away_team) == 0){
                    teams[k].matches_played += 1;
                    teams[k].goals_scored += matches[j][i].away_score;
                    teams[k].goals_against += matches[j][i].home_score;
                    teams[k].total_homefield_spectators += matches[j][i].spectators;
                    
                    /* stores wins, ties and loses */
                    if(matches[j][i].home_score < matches[j][i].away_score)
                        teams[k].away_wins += 1;
                    else if(matches[j][i].home_score > matches[j][i].away_score)
                        teams[k].matches_lost += 1;
                    else
                        teams[k].matches_tied += 1;
                        
                }
            }
    
    /* calculating and storing team points */
    for(i = 0; i < MAX_TEAMS; i++){
        teams[i].points = teams[i].home_wins * 3 + teams[i].away_wins * 3 + teams[i].matches_tied;
        teams[i].goal_difference = teams[i].goals_scored - teams[i].goals_against;
    }
}

/* checking if any of the current stored teams are precent in the match currently checked */
int check_matchteam_against_teamnames(team_data teams[MAX_TEAMS], const char match_team_name[MAX_NAME_LENGTH], int teams_stored){
    int i;
    
    for(i = 0; i < teams_stored; i++){
        if(strcmp(teams[i].team_name, match_team_name) == 0){
            return 1;
        }
    }
    
    return 0;
}


/* --- these functions handle finding and processing the data loaded from the file --- */
/* finding and printing rounds who had at total score of "min_score" or more */
void print_score_or_more(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], int min_score){
    int j, i, total_score;
    
    printf("\nFollowing matches had a total score of %i or more:\n", min_score);
    
    for(j = 0; j < MAX_ROUNDS; j++)
        for(i = 0; i < MATCHES_PER_ROUND; i++){
            total_score = matches[j][i].home_score + matches[j][i].away_score;
            
            if(total_score >= min_score){
                printf("Match %i in round %i had a total score of %i (%s - %s).\n", i, j, total_score, matches[j][i].home_team, matches[j][i].away_team);
            }
        }
}

/* print round with the highest goals scored */
void print_highest_round_score(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND]){
    int round_with_highest_score = 0, highest_round_score = 0;
        
    find_highest_round_score(matches, &highest_round_score, &round_with_highest_score);
    printf("\nRound %i had the highest round score of %i.\n", round_with_highest_score + 1, highest_round_score);
}

/* finds and returns the round with the highest goals scored */
void find_highest_round_score(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], int *highest_round_score, int *result){
    int round_score, i;
        
    for(i = 0; i < MAX_ROUNDS; i++){
        round_score = calc_round_score(matches, i);
        if (round_score > *highest_round_score){
            *highest_round_score = round_score;
            *result = i;
        }
    }
}
    
/* calculates and returns the sum of all goals scored per match in the current round */
int calc_round_score(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], int current_round){
    int res = 0, i;
    
    for(i = 0; i < MATCHES_PER_ROUND; i++){
        res += matches[current_round][i].total_score;
    }
    
    return res;
}

/* prints teams who won more on away field than home field */
void print_most_wins_awayfield(team_data teams[MAX_TEAMS]){
    int i;
    
    printf("\nFollowing teams won more on away field than home field:\n");
    
    for(i = 0; i < MAX_TEAMS; i++)
        if(teams[i].away_wins > teams[i].home_wins){
            printf("%s\n", teams[i].team_name);
        }
}

/* print team with lowest home field spectators */
void print_team_lowest_homefield_spectators(team_data teams[MAX_TEAMS]){ 
    int team_lowest_spectators;
    
    team_lowest_spectators = find_team_lowest_homefield_spectators(teams);
    printf("\nThe team who had the lowest amount of home field spectators was %s with %i.\n", teams[team_lowest_spectators].team_name, teams[team_lowest_spectators].total_homefield_spectators);
}

/* finds and returns the team who had the lowest amouts of spectators on home field */
int find_team_lowest_homefield_spectators(team_data teams[MAX_TEAMS]){
    int i, res = 0;
    
    for(i = 0; i < MAX_TEAMS; i++)
        if(teams[i].total_homefield_spectators < teams[res].total_homefield_spectators)
            res = i;
    
    return res;
}

/* prints a table of all the teams sorted by points */
void print_team_table(team_data teams[MAX_TEAMS]){
    int i;
    
    qsort(teams, MAX_TEAMS, sizeof(team_data), sort_teams);
    
    printf("\nTable of teams sorted by their points:\n");
    printf("_______________________________________________________\n");
    printf("| Nr. | Team | Mp | W  | T  | L  | Gs | Ga | +/- | P  |\n");
    printf("|_____|______|____|____|____|____|____|____|_____|____|\n");
    
    for(i = 0; i < MAX_TEAMS; i++){
        printf("| %-3i | %-4s | %2i | %2i | %2i | %2i | %2i | %2i | %3i | %2i |\n", i + 1, teams[i].team_name, teams[i].matches_played, teams[i].home_wins + teams[i].away_wins, teams[i].matches_tied, teams[i].matches_lost, 
                                                                                           teams[i].goals_scored, teams[i].goals_against, teams[i].goal_difference, teams[i].points);
        printf("|_____|______|____|____|____|____|____|____|_____|____|\n");
    }
    
    printf("Mp: Matches played, W: Matches won, T: Matches tied, L: Matches lost, Gs: Goals scored, Ga: Goals against, +/-: Goal difference, P: Points.\n");
}

/* print matches who fits the user search */
void print_user_searched_teams(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], const char weekday[MAX_NAME_LENGTH], const char clock1[MAX_DATECLOCK_LENGTH], const char clock2[MAX_DATECLOCK_LENGTH]){
    match_data *matches_found = NULL;
    int i, matches_found_size;
    
    matches_found_size = check_memorysize_to_allocate(matches, weekday, clock1, clock2);
    matches_found = allocate_memo(matches_found_size);
    find_maches_in_time(matches, matches_found, weekday, clock1, clock2);
    
    qsort(matches_found, matches_found_size, sizeof(match_data), sort_matches);
    
    if(matches_found_size > 0){
        printf("\nAll matches played on %s, from %s - %s sorted by points:\n", weekday, clock1, clock2);
        
        for(i = 0; i < matches_found_size; i++)
            printf("%s %s %s %-3s - %-3s %i - %i %i\n", (matches_found + i)->day, (matches_found + i)->date, (matches_found + i)->time, (matches_found + i)->home_team, 
                                                        (matches_found + i)->away_team, (matches_found + i)->home_score, (matches_found + i)->away_score, (matches_found + i)->spectators);
    } else
        printf("No matches played on %s, from %s - %s was found\n", weekday, clock1, clock2);
        
    free(matches_found);
}

/* checking how much memory should be allocated for the array storing all matches fitting the user input */
int check_memorysize_to_allocate(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], const char weekday[MAX_NAME_LENGTH], const char clock1[MAX_DATECLOCK_LENGTH], const char clock2[MAX_DATECLOCK_LENGTH]){
    int j, i, res = 0;
    
    for(j = 0; j < MAX_ROUNDS; j++)
        for(i = 0; i < MATCHES_PER_ROUND; i++)
            if(strcmp(matches[j][i].day, weekday) == 0 && strcmp(matches[j][i].time, clock1) > -1 && strcmp(matches[j][i].time, clock2) < 1)
                res++;
                
    return res;
}

/* allocating memory for all matches fitting the user input */
match_data *allocate_memo(int size){
    match_data *res = NULL;
    
    res = (match_data *)malloc(size * sizeof(match_data));
    
    if(res == NULL){
        printf("Could'nt allocate memory");
        exit (1);
    }
    
    return res;
}

/* storing all matches fitting the user input */
void find_maches_in_time(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], match_data *matches_found, const char weekday[MAX_NAME_LENGTH], const char clock1[MAX_DATECLOCK_LENGTH], const char clock2[MAX_DATECLOCK_LENGTH]){
    int j, i, matches_stored = 0;
    
    for(j = 0; j < MAX_ROUNDS; j++){
        for(i = 0; i < MATCHES_PER_ROUND; i++){
            if(strcmp(matches[j][i].day, weekday) == 0 && strcmp(matches[j][i].time, clock1) > -1 && strcmp(matches[j][i].time, clock2) < 1){
                *(matches_found + matches_stored) = matches[j][i];
                matches_stored++;
            }
        }
    }
}

/* prints all data */
void print_all(match_data matches[MAX_ROUNDS][MATCHES_PER_ROUND], team_data teams[MAX_TEAMS], const char weekday[MAX_NAME_LENGTH], const char clock1[MAX_DATECLOCK_LENGTH], const char clock2[MAX_DATECLOCK_LENGTH]){
    printf("_________________________________________________\n");
    print_score_or_more(matches, MIN_SCORE);
    printf("_________________________________________________\n");
    print_highest_round_score(matches);
    printf("_________________________________________________\n");
    print_most_wins_awayfield(teams);
    printf("_________________________________________________\n");
    print_team_lowest_homefield_spectators(teams);
    printf("_________________________________________________\n");
    print_user_searched_teams(matches, weekday, clock1, clock2);
    printf("_________________________________________________\n");
    print_team_table(teams);
}

/* qsort function for sorting match score from highest to lowest */
int sort_matches(const void *ip1, const void *ip2){
    match_data *in1 = (match_data *)ip1,
               *in2 = (match_data *)ip2;
    
    return in2->total_score - in1->total_score;
}

/* qsort function for sorting team points from highest to lowest. If to points are equal, then the teams will be sorted by goal difference, and if thats equal then its sorted by goals scored */
int sort_teams(const void *ip1, const void *ip2){
    team_data *in1 = (team_data *)ip1,
              *in2 = (team_data *)ip2;
    
    if (in2->points < in1->points)
        return -1;
    else if(in2->points > in1->points)
        return 1;
    else {
        if (in2->goal_difference < in1->goal_difference)
            return -1;
        else if (in2->goal_difference > in1->goal_difference)
            return 1;
        else{
            if(in2->goals_scored < in1->goals_scored)
                return -1;
            else if(in2->goals_scored > in1->goals_scored)
                return 1;
            else 
                return strcmp(in1->team_name, in2->team_name);
        }
    }
}
