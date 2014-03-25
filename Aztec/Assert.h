/********************************************************************************
 * Add Intwine information here
 ********************************************************************************/

extern void Assert(int flag, char *file, unsigned long line, char *msg);
extern void AssertInt(int flag, char *file, unsigned long line, char *msg, int intval);
extern void AssertMsg(int flag, char *file, unsigned long line, char *msg, char *msg2);

#define ASRT(a, b) Assert((a), __FILE__, __LINE__, (b))
#define ASRT_INT(a, b, c) AssertInt((a), __FILE__, __LINE__, (b), (c))
#define ASRT_MSG(a, b) AssertMsg((a), __FILE__, __LINE__, (b), (c))

