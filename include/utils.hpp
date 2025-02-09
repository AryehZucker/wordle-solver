#ifndef UTILS_H
#define UTILS_H

#include <fstream>

struct DataA;
struct DataL;

int weight(int bitstr);

void showDataA(const char *word, const struct DataA *data);
void printData(int letters, const struct DataL *letter_data, int bad_letters, std::ostream &file);

#endif
