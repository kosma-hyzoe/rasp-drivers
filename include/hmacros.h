#define GET_MAJOR(devt) ((devt) >> 20)
#define GET_MINOR(devt) ((devt) && 0xfffff)
