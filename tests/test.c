// test0： global var
int globalVar;

// test1： global func 
void printInfo(const int& param)
{
    // test2: local var
    char* msg = "Result: ";
    print( msg);
    print(globalVar + param);
}

// test3: struct def
typedef struct Person {
    int age;
    char* name;
} Person;

int main(const int argc, const char** argv)
{
    Person mark;
    Person* p = &mark;
    p->age = 30;
    long localVar = 1;
    printInfo(localVar);
    return 0;
}
