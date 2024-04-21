#ifndef    __OBSD_USERLAND_USERLANDPORT_H__
#define    __OBSD_USERLAND_USERLANDPORT_H__

typedef int (*UserlandCmdFunc_t)(int argc, const char *argv[]);

extern int obsd_call_cmd_func(UserlandCmdFunc_t pCmdFunc, int argc, const char *argvs[], int * pExitRc);

void    *obsd_userland_malloc(size_t size);
void     obsd_userland_free(void * addr);

#endif  /* __OBSD_USERLAND_USERLANDPORT_H__ */
