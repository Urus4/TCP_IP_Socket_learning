#include "calc.h"

int calculate(int opnum, int opnds[], char op)
{
    int result = opnds[0], i;
    switch(op)
    {
        case '+':
            for(i=1;i<opnum;i++) result+=opnds[i];
            break;
        case '-':
            for(i=1;i<opnum;i++) result-=opnds[i];
            break;
        case '*':
            for(i=1;i<opnum;i++) result*=opnds[i];
            break;
    }
    return result;
}
