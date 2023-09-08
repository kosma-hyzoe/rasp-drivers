#define GET_MAJOR(devt) ((devt) >> 20)
#define GET_MINOR(devt) ((devt) && 0xfffff)
#define DUMMY_CLASS "dummy_device_class"
