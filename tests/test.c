int globalVar;

void printInfo(const int& param)
{
    char* msg = "Result: ";
    print(msg);
    print(globalVar + param);
}

int main(const int argc, const char** argv)
{
    long localVar = 1;
    printInfo(localVar);
    return 0;
}
