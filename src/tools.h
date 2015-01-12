#ifndef _TOOLS_H_
#define _TOOLS_H_

#define RETURN_FAILURE	(-1)
#define RETURN_SUCCESS	(0)

#define max(a,b) (a>=b?a:b)
#define min(a,b) (a<=b?a:b)

#if RETURN_FAILURE >= 0
# error "RETURN_FAILURE must be negative"
#endif

/* will not return but exit.
   return an int in order you can return fatal() in a non void function. */
int fatal(int assert, const char *fname, const char *fmt, ...);

/* function fatal */
#define ffatal(assert,...) fatal(assert, __func__, __VA_ARGS__)

/* not yet */
#define NYI() ffatal(FALSE, "Not Yet Implemented")

extern char *strdup(const char*);

#endif
