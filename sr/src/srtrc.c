#ifndef _SR_TRC_C
#define _SR_TRC_C

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "srincs.h"

#define SR_TRC_BUF_SIZE    2000
/*extern tSrGlobalInfo gSrGlobalInfo;*/

/*****************************************************************************
**                                                                           *
** Function     :  SrTrcPrint                                                 *
**                                                                           *
** Description  :  prints the trace - with filename and line no              *
**                                                                           *
** Input        : fname   - File name                                        *
**                u4Line  - Line no                                          *
**                s       - strong to be printed                             *
**                                                                           *
** Output       : None                                                       *
**                                                                           *
** Returns      : Returns string                                             *
**                                                                           *
******************************************************************************/
PUBLIC VOID
SrTrcPrint (const char *fname, UINT4 u4Line, const char *s)
{

    tOsixSysTime        sysTime = 0;
    const char         *pc1Fname = fname;

    if (s == NULL)
    {
        return;
    }
    while (*fname != '\0')
    {
        if (*fname == '/')
            pc1Fname = (fname + 1);
        fname++;
    }
    OsixGetSysTime (&sysTime);
    printf ("Sr: %d:%s:%d:   %s", sysTime, pc1Fname, u4Line, s);
}

/*****************************************************************************
**                                                                           *
** Function     : SrTrcWrite                                                  *
**                                                                           *
** Description  :  prints the trace - without filename and line no ,         *
**                 Useful for dumping packets                                *
**                                                                           *
** Input        : s - string to be printed                                   *
**                                                                           *
** Output       : None                                                       *
**                                                                           *
** Returns      : Returns string                                             *
**                                                                           *
******************************************************************************/
PUBLIC VOID
SrTrcWrite (CHR1 * s)
{
    if (s != NULL)
    {
        printf ("%s", s);
    }
}

/****************************************************************************
**                                                                          *
** Function     : SrTrc                                                      *
**                                                                          *
** Description  : converts variable argument in to string depending on flag *
**                                                                          *
** Input        : u4Flags  - Trace flag                                     *
**                fmt  - format strong, variable argument                    *
**                                                                          *
** Output       : None                                                      *
**                                                                          *
** Returns      : Returns string                                            *
**                                                                          *
******************************************************************************/
PUBLIC CHR1        *
SrTrc (UINT4 u4Flags, const char *fmt, ...)
{
    va_list             ap;

    static CHR1         buf[SR_TRC_BUF_SIZE];
    MEMSET (&ap, SR_ZERO, sizeof (va_list));
    if (!(u4Flags & SR_TRC_FLAG))
    {
        return (NULL);
    }
    va_start (ap, fmt);
    vsprintf (&buf[0], fmt, ap);
    va_end (ap);

    return (&buf[0]);
}

#endif
