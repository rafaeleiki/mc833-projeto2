#ifndef PROFILE_MANAGER_H
#define PROFILE_MANAGER_H

#include "../shared/profile.h"

#define DB_FILE_NAME "database.bin"
#define PICTURES_DIR_NAME "pictures"
#define EXPERIENCE_PROFILE_NOT_FOUND 0
#define EXPERIENCE_FULL 1
#define EXPERIENCE_ADDED 2

void init_profile_db();

int add_profile(Profile *profile);
int add_experience_to_profile(char *email, char *experience);
int update_profile(Profile *profile);

Profile* get_profile(char* email, int *found);
Profile* get_all_profiles(int *profilesCount);
Profile* get_profiles_by_address(char* address, int *profilesCount);
Profile* get_profiles_by_education(char* courseName, int *profilesCount);
void send_profile_picture(int socket_fd, Profile *profile, long int *delta);

#endif