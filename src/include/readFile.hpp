#ifndef PRIME_PRACTICE_READFILE_HPP
#define PRIME_PRACTICE_READFILE_HPP

#include "include/types.h"

struct ReadFileResult {
    void *data;
    u32 len;
};

/**
 * Reads the entire file and returns a pointer to an array (which you now take ownership of)
 * @param path
 * @return
 */
ReadFileResult readFileSync(const char* path);

#endif //PRIME_PRACTICE_READFILE_HPP
