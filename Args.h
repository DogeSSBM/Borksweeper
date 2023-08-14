#ifndef ARGS_H
#define ARGS_H

void usage(void)
{
    printf("Usage -\n");
    printf("\t./main.out [Width,Height] [Num Bombs] [RNG|ADJ|SAT] [cheat]\n\n");
    printf("Default options are: 30,16 99 RNG\n");
    exit(EXIT_FAILURE);
}

uint parseUint(char *str, const uint len)
{
    char *end;
    const uint ret = strtoul(str, &end, 0);
    if(end-str > len)
        usage();
    return ret;
}

bool sameStrLower(char *a, char *b)
{
    const uint len = strlen(a);
    if(strlen(b) != len)
        return false;
    for(uint i = 0; i < len; i++)
        if(tolower(a[i]) != tolower(b[i]))
            return false;

    return true;
}

ArgType parseArgType(char *arg)
{
    if(!arg || strlen(arg) == 0)
        usage();
    if(isdigit(*arg)){
        if(strchr(arg, ','))
            return A_LEN;
        return A_MIN;
    }
    if(sameStrLower("cheat", arg))
        return A_CHEAT;
    else if(strlen(arg) == 3)
        return A_TYP;
    panic("Unknown arg type! arg: %s", arg);
    return A_ERR;
}

Length parseLen(char *arg)
{
    char *comma = strchr(arg, ',');
    return iC(
        parseUint(arg, comma-arg),
        parseUint(comma+1, strlen(comma+1))
    );
}

uint parseBombs(char *arg)
{
    return parseUint(arg, strlen(arg));
}

BoardType parseBoardType(char *arg)
{
    if(sameStrLower(arg, "rng"))
        return B_RNG;
    if(sameStrLower(arg, "adj"))
        return B_ADJ;
    if(sameStrLower(arg, "sat"))
        return B_SAT;
    panic("Unknown BoardType! arg: %s", arg);
    return B_N;
}

Arg parseArg(char *arg)
{
    Arg ret = {.type = parseArgType(arg)};
    switch(ret.type){
        case A_LEN:
            ret.len = parseLen(arg);
            break;
        case A_MIN:
            ret.min = parseBombs(arg);
            break;
        case A_TYP:
            ret.typ = parseBoardType(arg);
            break;
        case A_CHEAT:
            ret.cheat = true;
            break;
        case A_ERR:
        default:
            usage();
            break;
    }
    return ret;
}

Board boardArgs(int argc, char **argv)
{
    Board board = boardDefault();
    if(argc <= 1)
        return board;
    bool argDone[4] = {0};
    for(int i = 1; i < argc; i++){
        const Arg arg = parseArg(argv[i]);
        if(arg.type >= A_ERR || argDone[arg.type])
            usage();
        argDone[arg.type] = true;
        switch(arg.type){
            case A_LEN:
                board.len = arg.len;
                break;
            case A_MIN:
                board.numBombs = arg.min;
                break;
            case A_TYP:
                board.type = arg.typ;
                break;
            case A_CHEAT:
                board.cheat = true;
                break;
            default:
                usage();
                break;
        }
    }
    board.state = BS_FIRST;
    return board;
}

#endif /* end of include guard: ARGS_H */
