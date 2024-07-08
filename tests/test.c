//test0 include

#include<stdio.h>

// test0： global var
int globalVar;

// test1： global func 
void printInfo(const int& param)
{
    // test2: local var
    char* msg = "Result: ";
    printf("%s", msg);
    printf("%d", globalVar + param);
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
    mark.name = "Mark";
    long localVar = 1;
    printInfo(localVar);

    bool cond1 = true, cond2 = false;
    if (cond1 && cond2) {
        //
    } else if (cond1 || cond2) {
        //
    }

    return 0;
}
