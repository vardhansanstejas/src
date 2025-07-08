/*****************************************************************************/
/* Copyright (C) 2007 Aricent Inc . All Rights Reserved
 *    SUBSYSTEM NAME         : SR 
 *    MODULE NAME            : SR
 *    LANGUAGE               : ANSI-C
 *    TARGET ENVIRONMENT     : Linux (Portable)                         
 *    DATE OF FIRST RELEASE  :
 *    DESCRIPTION            : This file contains sizing relayed routines.
 *                       These routines help in memory management such
 *                       as creation and deletion of memory pools.
 *---------------------------------------------------------------------------*/

#define _SRSZ_C
#include "srincs.h"

extern INT4         IssSzRegisterModuleSizingParams (CHR1 * pu1ModName,
                                                     tFsModSizingParams *
                                                     pModSizingParams);

extern INT4         IssSzRegisterModulePoolId (CHR1 * pu1ModName,
                                               tMemPoolId * pModPoolId);

/*---------------------------------------------------------------------------*/
/*
 * Function Name   : SrSizingMemCreateMemPools
 * Description     : This function requests the creation of memory pools for SR 
 *                         module for all sizing params.
 * Input (s)       : None
 * Output (s)      : None
 * Returns         : OSIX_FAILURE / OSIX_SUCCESS
 */
/*---------------------------------------------------------------------------*/
INT4
SrSizingMemCreateMemPools (VOID)
{
    UINT4               u4RetVal = SR_ZERO;
    UINT4               i4SizingId = SR_ZERO;

    for (i4SizingId = 0; i4SizingId < SR_MAX_SIZING_ID; i4SizingId++)
    {
        u4RetVal = MemCreateMemPool (SrSizingParams[i4SizingId].u4StructSize,
                                     SrSizingParams
                                     [i4SizingId].u4PreAllocatedUnits,
                                     MEM_DEFAULT_MEMORY_TYPE,
                                     &(SrMemPoolIds[i4SizingId]));

        if (u4RetVal == MEM_FAILURE)
        {
            SrSizingMemDeleteMemPools ();
            return OSIX_FAILURE;
        }
    }
    return OSIX_SUCCESS;
}

/*---------------------------------------------------------------------------*/
/*
 * Function Name   : SrSzRegisterModuleSizingParams
 * Description     : This function is responsible for registering mem pool Ids.
 * Input (s)       : pu1ModName - Module name ("SR")
 * Output (s)      : None
 * Returns         : OSIX_FAILURE / OSIX_SUCCESS
 */
/*---------------------------------------------------------------------------*/
INT4
SrSzRegisterModuleSizingParams (CHR1 * pu1ModName)
{
    /* Copy the Module Name */
    IssSzRegisterModuleSizingParams (pu1ModName, SrSizingParams);
    IssSzRegisterModulePoolId (pu1ModName, SrMemPoolIds);
    return OSIX_SUCCESS;
}

/*---------------------------------------------------------------------------*/
/*
 * Function Name   : SrSizingMemDeleteMemPools
 * Description     : This function requests deletion of memory pools for SR
 *              module for all sizing params.
 * Input (s)       : None
 * Output (s)      : None
 * Returns         : OSIX_FAILURE / OSIX_SUCCESS
 */
/*---------------------------------------------------------------------------*/
VOID
SrSizingMemDeleteMemPools (VOID)
{
    UINT4               i4SizingId;

    for (i4SizingId = 0; i4SizingId < SR_MAX_SIZING_ID; i4SizingId++)
    {
        if (SrMemPoolIds[i4SizingId] != 0)
        {
            MemDeleteMemPool (SrMemPoolIds[i4SizingId]);
            SrMemPoolIds[i4SizingId] = 0;
        }
    }
}
