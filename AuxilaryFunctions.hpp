#ifndef AUXILARY_FUNCTIONS_HPP
#define AUXILARY_FUNCTIONS_HPP

#include <vector>


std::vector<int> generateRandomVector(int size, int min_val, int max_val);
bool startsWithOne(int num);
int getDigitsMultiplication(int num);
void runInputProcess(char const* pipeName, int minVal, int maxVal);
void runOutputProcess(char const* pipeName);


#endif // !AUXILARY_FUNCTIONS_HPP
