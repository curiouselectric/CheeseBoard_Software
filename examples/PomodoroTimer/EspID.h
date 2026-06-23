#pragma once

// Length is number of characters of expected ID, plus 1 for \0 string termination
#define ESPID_LENGTH 6+1

class EspID_ {
public:
    EspID_() {;}
    void begin();
    const char* get() { return _id; }

private:
    char _id[ESPID_LENGTH];

};

extern EspID_ EspID;

