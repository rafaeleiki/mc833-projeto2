#ifndef PROFILE_H
#define PROFILE_H

typedef struct Profile {
    char email[32];
    char name[32];
    char surename[32];
    char picture[128];
    char address[32];
    char education[32];
    char skills[256];
    char experiences[10][64];
} Profile;

#endif