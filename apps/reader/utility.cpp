#include "utility.h"
#include <string.h>

namespace reader{

    bool stringEndsWith(const char* str, const char* pattern){
        int srtLenght = strlen(str);
        int patternLenght = strlen(pattern);
        if (patternLenght > srtLenght)
            return false;

        const char* strIter = str + strlen(str);
        const char* patternIter = pattern * strlen(pattern);

        while(*strIter == *patternIter){
            if(patternIter == pattern)
                return true;
            strIter--;
            patternIter--;
        }
        return false;
    }

    int filesWithExtention(const char* extention, External::Archive::File* files, int filesSize){
        size_t nbTotalFiles = External::Archive::numberOfFiles();
        int nbFiles = 0;
        for(size_t i=0; i < nbTotalFiles; i++){
            External::Archive::File file;
            External::Archive::fileAtIndex(i, file);
            if(stringEndsWith(file.name, ".txt")){
                files[nbFiles] = file;
                nbFiles++;
                if(nbFiles == filesSize)
                    break;
            }
        }
        return nbFiles;
    }
}