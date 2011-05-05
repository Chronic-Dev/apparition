#ifndef MBDB_H
#define MBDB_H

#include <unistd.h>
#include <time.h>

struct mbdb_property {
    char name[];
    char value[];
    struct mbdb_property *next;
};

struct mbdb_record {
    char domain[];
    char path[];
    char target[];	      // absolute path
    char datahash[];	      // SHA1 hash
    char unknown1[];
    uint16_t mode;	      // Axxx = symlink, 4xxx = dir, 8xxx = file
    uint32_t unknown2;
    uint32_t unknown3;
    uint32_t uid;
    uint32_t gid;
    uint32_t time1;
    uint32_t time2;
    uint32_t time3;
    uint64_t length;	      // 0 if link or dir
    uint8_t flag;	      // 0 if link or dir
    uint8_t properties;	      // number of properties
    struct mbdb_property *property;
};

struct mbdb_file {
    uint8_t magic[6];		    // 'mbdb\5\0'
    struct mbdb_record *record;
};

#endif

