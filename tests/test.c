//test0 include

 #include <header.h>

// test0： global var
//int globalVar;

// test1： global func 
void printInfo(const int& param)
{
    // test2: local var
    char* msg = "Result: ";
    printf("%s", msg);
    printf("%d", globalVar + param);
};

// test3: struct def
struct Person {
    int age = 30;
    char* name;
};

typedef struct Person Person;

int main(const int argc, const char** argv)
{
    Person mark;
    Person* p = &mark;
    p->age = 30;
    mark.name = "Mark";
    int localVar = 1;
    printInfo(localVar);

    bool cond1 = true;
    if (cond1 && cond2 == true) {
        //
    } else {
        //
    }

    return 0;
}
