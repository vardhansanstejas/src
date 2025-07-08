/********************************************************************
 *** Copyright (C) 2024 Aricent Inc . All Rights Reserved
 ***
 *** $Id: srdb.c,v 1.1.1.1 2024/10/03
 ***
 *** Description: This file contains routines for DB operations
 ***
 ********************************************************************/

#include "srincs.h"
#include "srtdefs.h"
#include "srtrc.h"
#include "srglob.h"
#include "srmacs.h"
#include "srdb.h"

/* FTN Tunnel Interface Index tables */
eDBRet
SrFTNDbTnlInfoTableRestoreCallback (CONST CHR1 *pTableName,
        CONST CHR1 *pEntryKey, CONST CHR1 *pEntryData, eOperation i1Operation);
UINT1 SrFTNDbRestoreTnlInfoTableEntry (tSrTnlIfTable *pTnlDbEntry);
UINT1 SrFTNDbDeleteTnlInfoTableEntry (tSrTnlIfTable *pTnlDbEntry);
UINT1 SrFTNDbRestoreTnlInfoTable (VOID);
INT4  SrFTNDbStoreTnlInfo (tSrTnlIfTable *pSrTnlInfo);
INT4  SrDbDelFTNTnlInfoFromDB (UINT4 u4Prefix, UINT4 u4NextHop);

/* ILM Tunnel Interface Index tables */
eDBRet
SrILMDbTnlInfoTableRestoreCallback (CONST CHR1 *pTableName,
        CONST CHR1 *pEntryKey, CONST CHR1 *pEntryData, eOperation i1Operation);
UINT1 SrILMDbRestoreTnlInfoTable (VOID);
UINT1 SrILMDbRestoreTnlInfoTableEntry (tSrTnlIfTable *pTnlDbEntry);
UINT1 SrILMDbDeleteTnlInfoTableEntry (tSrTnlIfTable *pTnlDbEntry);
INT4  SrILMDbStoreTnlInfo (tSrTnlIfTable *pTnlInfo);
INT4  SrDbDelILMTnlInfoFromDB (UINT4 u4Prefix, UINT4 u4NextHop);

/*All Tunnel Interface Index Tables */
UINT1 SrDbRegAllRestoreTableNotifyCallback (VOID);
UINT1 SrDbDeleteAllTables ();
UINT1 SrDbRestoreAllTables (VOID);
VOID  SrDbDeleteAllDbTnlInfo ();

/*****************************************************************************/
/* Function Name : SrDbRegAllRestoreTableNotifyCallback                      */
/* Description   : This routine registers callback function to stub for      */
/*                 sync to standby node purpose.                             */
/* Input(s)      : None                                                      */
/* Output(s)     : None                                                      */
/* Return(s)     : SR_SUCCESS or SR_FAILURE                                  */
/*****************************************************************************/
UINT1
SrDbRegAllRestoreTableNotifyCallback (VOID)
{
    SR_TRC2 (SR_CRITICAL_TRC,"ENTRY:%s:%d\n",__func__,__LINE__);
#ifdef TEJAS_WANTED
    if (gDBRegTableNotifyCallback (SR_FTN_TNL_INFO_TBL_NAME,
                SrFTNDbTnlInfoTableRestoreCallback) != DB_SUCCESS)
    {
        SR_TRC2 (SR_FAIL_TRC, "%s:%d Registration of SrFtnTnlInfoTable failed\n",__func__,__LINE__);
        return SR_FAILURE;
    }

    if (gDBRegTableNotifyCallback (SR_ILM_TNL_INFO_TBL_NAME,
                SrILMDbTnlInfoTableRestoreCallback) != DB_SUCCESS)
    {
        SR_TRC2 (SR_FAIL_TRC, "%s:%d Registration of SrIlmTnlInfoTable failed\n",__func__,__LINE__);
        return SR_FAILURE;
    }
#endif
     SR_TRC2 (SR_CRITICAL_TRC,"EXIT:%s:%d SUCCESS\n",__func__,__LINE__);
     return SR_SUCCESS;
}

/*****************************************************************************/
/* Function Name : SrDbRestoreAllTables                                      */
/* Description   : This routine restores the TnlInfo table                   */
/*                 and updates necessary information                         */
/* Input(s)      : None                                                      */
/* Output(s)     : None                                                      */
/* Return(s)     : SR_SUCCESS or SR_FAILURE                                  */
/*****************************************************************************/
UINT1
SrDbRestoreAllTables (VOID)
{
    SR_TRC2 (SR_CRITICAL_TRC,"ENTRY:%s:%d\n",__func__,__LINE__);
#ifdef TEJAS_WANTED
    /* Table to restore Tunnel Interface Index from StubDB to local RB tree */
    if (gDBRestoreTable (SR_FTN_TNL_INFO_TBL_NAME) != DB_SUCCESS)
    {
        SR_TRC2 (SR_FAIL_TRC, "%s:%d Restoration of SrFTNDbRestoreTnlInfoTable failed\n",__func__,__LINE__);
        return SR_FAILURE;
    }

    /* Table to restore Tunnel Interface Index from StubDB to local RB tree */
    if (gDBRestoreTable (SR_ILM_TNL_INFO_TBL_NAME) != DB_SUCCESS)
    {
        SR_TRC2 (SR_FAIL_TRC, "%s:%d Restoration of SrILMDbRestoreTnlInfoTable failed\n",__func__,__LINE__);
        return SR_FAILURE;
    }
#endif
    SR_TRC2 (SR_CRITICAL_TRC,"EXIT:%s:%d SUCCESS\n",__func__,__LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************/
/* Function Name : SrFTNDbRestoreTnlInfoTable                                */
/* Description   : This routine restores the TnlInfo table                   */
/*                 and updates necessary information                         */
/* Input(s)      : None                                                      */
/* Output(s)     : None                                                      */
/* Return(s)     : RPTE_SUCCESS or RPTE_FAILURE                              */
/*****************************************************************************/
UINT1
SrFTNDbRestoreTnlInfoTable (VOID)
{
    SR_TRC2 (SR_CRITICAL_TRC,"ENTRY:%s:%d\n",__func__,__LINE__);

#ifdef TEJAS_WANTED
    if (gDBRestoreTable (SR_FTN_TNL_INFO_TBL_NAME) != DB_SUCCESS)
    {
        SR_TRC (SR_FAIL_TRC,
                "Resoration of SrFTNDbRestoreTnlInfoTable failed\n");
        return SR_FAILURE;
    }
#endif
    SR_TRC2 (SR_CRITICAL_TRC,"EXIT:%s:%d SUCCESS\n",__func__,__LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************/
/* Function Name : SrILMDbRestoreTnlInfoTable                                */
/* Description   : This routine restores the TnlInfo table                   */
/*                 and updates necessary information                         */
/* Input(s)      : None                                                      */
/* Output(s)     : None                                                      */
/* Return(s)     : RPTE_SUCCESS or RPTE_FAILURE                              */
/*****************************************************************************/
UINT1
SrILMDbRestoreTnlInfoTable (VOID)
{
    SR_TRC2 (SR_CRITICAL_TRC,"ENTRY:%s:%d\n",__func__,__LINE__);
#ifdef TEJAS_WANTED
    if (gDBRestoreTable (SR_ILM_TNL_INFO_TBL_NAME) != DB_SUCCESS)
    {
        SR_TRC (SR_FAIL_TRC,
                "Resoration of SrILMDbRestoreTnlInfoTable failed\n");
        return SR_FAILURE;
    }
#endif
    SR_TRC2 (SR_CRITICAL_TRC,"EXIT:%s:%d SUCCESS\n",__func__,__LINE__);
    return SR_SUCCESS;
}

/********************************************************************************/
/* Function Name : SrFTNDbRestoreTnlInfoTableEntry                              */
/* Description   : This routine restores the Tunnel Info entry                  */
/* Input(s)      : pFTNTnlDbEntry - Pointer to entry to be restored to TnlInfo  */
/* Output(s)     : None                                                         */
/* Return(s)     : SR_SUCCESS or SR_FAILURE                                     */
/********************************************************************************/
UINT1
SrFTNDbRestoreTnlInfoTableEntry (tSrTnlIfTable *pTnlDbEntry)
{
    SR_TRC2 (SR_CRITICAL_TRC,"ENTRY:%s:%d\n",__func__,__LINE__);

    tSrTnlIfTable *pTmpTnlDbEntry = NULL;

    SR_TRC2 (SR_CRITICAL_TRC,"Restoring for Tnl prefix: %x nexthop: %x\n", pTnlDbEntry->u4Prefix, pTnlDbEntry->u4NextHop);

    /* 1. Notify CFA to reserve the Tunnel If Index */
    if (CfaIfmCreateDynamicMplsTunnelInterface (pTnlDbEntry->u4SrTnlIfIndex,NULL) == CFA_FAILURE)
    {
        SR_TRC3(SR_FAIL_TRC, "Failed to create CFA Interface FTN Tnl Id: %d, Prefix: %x NextHop: %x\n",
                pTnlDbEntry->u4SrTnlIfIndex, pTnlDbEntry->u4Prefix, pTnlDbEntry->u4NextHop);

        return SR_FAILURE;
    }

    /* 2. Mark the entry as stale during restore*/
    pTnlDbEntry->u1stale = SR_TRUE;

    /* 3. Add the entry in the RB tree */
    if (SR_ALLOCATE_FTN_TNL_INFO_MEM_BLOCK(pTmpTnlDbEntry) == NULL)
    {
        SR_TRC3 (SR_FAIL_TRC,"MemAlloc failed for FTNTnl Id: %d, Prefix: %x NextHop: %x\n",
                 pTnlDbEntry->u4SrTnlIfIndex, pTnlDbEntry->u4Prefix, pTnlDbEntry->u4NextHop);

        SR_TRC (SR_FAIL_TRC,"SrFTNDbRestoreTnlInfoTableEntry: INTMD-EXIT\n");
        return SR_FAILURE;
    }

    MEMCPY (pTmpTnlDbEntry, pTnlDbEntry, sizeof (tSrTnlIfTable));

    /* 4. Add TnlDbInfo to RB-Tree */
    if (RBTreeAdd (gSrGlobalInfo.SrFTNTnlDbTree,
                (tRBElem *) pTmpTnlDbEntry) != RB_SUCCESS)
    {
        SR_TRC (SR_CRITICAL_TRC, "SrFTNDbRestoreTnlInfoTableEntry: RB-Tree Add failed .\n");
        if (SR_MEM_FAILURE == SR_FTN_TNL_INFO_MEM_FREE(pTmpTnlDbEntry))
        {
           SR_TRC (SR_FAIL_TRC,"ERROR! Failed to release FTN entry \n");
           return SR_FAILURE;
        }

        return SR_FAILURE;
    }

    SR_TRC2 (SR_CRITICAL_TRC,"EXIT:%s:%d SUCCESS\n",__func__,__LINE__);

    return SR_SUCCESS;
}

/********************************************************************************/
/* Function Name : SrILMDbRestoreTnlInfoTableEntry                              */
/* Description   : This routine restores the Tunnel Info entry                  */
/* Input(s)      : pTnlDbEntry - Pointer to entry to be restored to TnlInfo     */
/* Output(s)     : None                                                         */
/* Return(s)     : SR_SUCCESS or SR_FAILURE                                     */
/********************************************************************************/
UINT1
SrILMDbRestoreTnlInfoTableEntry (tSrTnlIfTable *pTnlDbEntry)
{
    SR_TRC2 (SR_CRITICAL_TRC,"ENTRY:%s:%d\n",__func__,__LINE__);

    tSrTnlIfTable *pTmpTnlDbEntry = NULL;

    SR_TRC2 (SR_CRITICAL_TRC,"Restoring for Tnl prefix: %x nexthop: %x\n", pTnlDbEntry->u4Prefix, pTnlDbEntry->u4NextHop);

    /* 1. Notify to CFA to reserve the TnlIfIndex */
    if (CfaIfmCreateDynamicMplsTunnelInterface (pTnlDbEntry->u4SrTnlIfIndex,NULL) == CFA_FAILURE)
    {
        SR_TRC3(SR_FAIL_TRC, "Failed to create CFA Interface ILM Tnl Id: %d, Prefix: %x, NextHop: %x\n",
                pTnlDbEntry->u4SrTnlIfIndex, pTnlDbEntry->u4Prefix, pTnlDbEntry->u4NextHop);

        return SR_FAILURE;
    }

    /* 2. Mark the entry as stale during restore*/
    pTnlDbEntry->u1stale = SR_TRUE;

    /* 3. Add the entry in the RB tree */
    if(SR_ALLOCATE_ILM_TNL_INFO_MEM_BLOCK(pTmpTnlDbEntry) == NULL)
    {
        SR_TRC3 (SR_FAIL_TRC, "MemAlloc failed for New Tunnel ILMTnl Id: %d, Prefix: %x NextHop: %x\n",
                 pTnlDbEntry->u4SrTnlIfIndex, pTnlDbEntry->u4Prefix, pTnlDbEntry->u4NextHop);

        SR_TRC (SR_FAIL_TRC,"SrILMDbRestoreTnlInfoTableEntry: INTMD-EXIT \n");
        return SR_FAILURE;
    }

    MEMCPY (pTmpTnlDbEntry, pTnlDbEntry, sizeof (tSrTnlIfTable));

    /* 4. Add TnlDbInfo to RB-Tree */
    if (RBTreeAdd (gSrGlobalInfo.SrILMTnlDbTree,
                (tRBElem *) pTmpTnlDbEntry) != RB_SUCCESS)
    {
        SR_TRC (SR_FAIL_TRC, "SrILMDbRestoreTnlInfoTableEntry: RB-Tree Add failed .\n");
        if (SR_MEM_FAILURE == SR_ILM_TNL_INFO_MEM_FREE(pTmpTnlDbEntry))
        {
           SR_TRC (SR_FAIL_TRC,"ERROR! Failed to release ILM entry \n");
           return SR_FAILURE;
        }

        return SR_FAILURE;
    }

    SR_TRC2 (SR_CRITICAL_TRC,"EXIT:%s:%d SUCCESS\n",__func__,__LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************/
/* Function Name : SrFTNDbDeleteTnlInfoTableEntry                            */
/* Description   : This routine deletes the Tunnel Info entry                */
/* Input(s)      : pFTNTnlDbEntry - Pointer to entry to be deleted           */
/*                 and restored to TnlInfo                                   */
/* Output(s)     : None                                                      */
/* Return(s)     : SR_SUCCESS or SR_FAILURE                                  */
/*****************************************************************************/
UINT1
SrFTNDbDeleteTnlInfoTableEntry (tSrTnlIfTable *pTnlDbEntry)
{
    SR_TRC2 (SR_CRITICAL_TRC,"ENTRY:%s:%d\n",__func__,__LINE__);

    tSrTnlIfTable *pSrTnlInfo = NULL;
    INT4 i4RetVal = 0;

    SR_TRC2 (SR_CRITICAL_TRC,"Delete Tunnel Prefix: %x NextHop: %x\n",
            pTnlDbEntry->u4Prefix, pTnlDbEntry->u4NextHop);

    if (SrUtlCheckTnlInTnlDbTable (SR_FTN_TNL_INFO_TABLE,
                pTnlDbEntry->u4Prefix,
                pTnlDbEntry->u4NextHop,
                &pSrTnlInfo) == SR_SUCCESS)
    {
        if (RBTreeRem (gSrGlobalInfo.SrFTNTnlDbTree, pSrTnlInfo) == NULL)
        {
            SR_TRC2 (SR_FAIL_TRC, "%s:%d ERROR! Failed to remove FTN entry from RB Tree\n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }

        if (pTnlDbEntry->u4Prefix != 0 && pTnlDbEntry->u4NextHop != 0)
        {
            if ((i4RetVal = CfaIfmDeleteDynamicMplsTunnelInterface (pTnlDbEntry->u4SrTnlIfIndex,
                            NULL, TRUE)) == CFA_FAILURE)
            {
                if (SR_MEM_FAILURE == SR_FTN_TNL_INFO_MEM_FREE(pSrTnlInfo))
                {
                    SR_TRC2 (SR_FAIL_TRC, "%s:%d ERROR! Failed to release FTN entry memory\n", __func__, __LINE__);
                    return SR_FAILURE;
                }

                SR_TRC3 (SR_FAIL_TRC, "Failed to delete CFA Interface FTN Tnl Id: %d, Prefix: %x, NextHop: %x\n",
                         pTnlDbEntry->u4SrTnlIfIndex, pTnlDbEntry->u4Prefix, pTnlDbEntry->u4NextHop);
                return SR_FAILURE;
            }
            SR_TRC3 (SR_FAIL_TRC, "CFA Interface deletion success for FTN Tnl Id: %d, Prefix: %x NextHop: %x\n",
                     pTnlDbEntry->u4SrTnlIfIndex, pTnlDbEntry->u4Prefix, pTnlDbEntry->u4NextHop);
        }

        MEMSET (pSrTnlInfo, SR_ZERO, sizeof (tSrTnlIfTable));
        if (SR_FTN_TNL_INFO_MEM_FREE(pSrTnlInfo) == SR_MEM_FAILURE)
        {
            SR_TRC2 (SR_FAIL_TRC, "%s:%d ERROR! Failed to release FTN entry memory\n", __func__, __LINE__);
            return SR_FAILURE;
        }
    }

    SR_TRC2 (SR_CRITICAL_TRC,"EXIT:%s:%d SUCCESS\n",__func__,__LINE__);

    return SR_SUCCESS;
}

/*****************************************************************************/
/* Function Name : SrILMDbDeleteTnlInfoTableEntry                            */
/* Description   : This routine restores the Tunnel Info entry               */
/* Input(s)      : pTnlDbEntry - Pointer to entry to be deleted and restored */
/*                 to TnlInfo                                                */
/* Output(s)     : None                                                      */
/* Return(s)     : SR_SUCCESS or SR_FAILURE                                  */
/*****************************************************************************/
UINT1
SrILMDbDeleteTnlInfoTableEntry (tSrTnlIfTable *pTnlDbEntry)
{
    SR_TRC2 (SR_CRITICAL_TRC,"ENTRY:%s:%d\n",__func__,__LINE__);

    tSrTnlIfTable *pSrTnlInfo = NULL;
    INT4 i4RetVal = 0;

    SR_TRC2 (SR_CRITICAL_TRC,"Delete Tunnel Prefix: %x NextHop: %x\n",
             pTnlDbEntry->u4Prefix, pTnlDbEntry->u4NextHop);

    if (SrUtlCheckTnlInTnlDbTable (SR_ILM_TNL_INFO_TABLE,
                pTnlDbEntry->u4Prefix,
                pTnlDbEntry->u4NextHop,
                &pSrTnlInfo) == SR_SUCCESS)
    {
        if (RBTreeRem (gSrGlobalInfo.SrILMTnlDbTree, pSrTnlInfo) == NULL)
        {
            SR_TRC2 (SR_FAIL_TRC, "%s:%d ERROR! Failed to remove FTN entry from RB Tree\n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }

        if (pTnlDbEntry->u4Prefix != 0 && pTnlDbEntry->u4NextHop != 0)
        {
            i4RetVal = CfaIfmDeleteDynamicMplsTunnelInterface (pTnlDbEntry->u4SrTnlIfIndex, NULL, TRUE);
            if (i4RetVal == CFA_FAILURE)
            {
                if (SR_MEM_FAILURE == SR_ILM_TNL_INFO_MEM_FREE(pSrTnlInfo))
                {
                    SR_TRC2 (SR_FAIL_TRC, "%s:%d ERROR! Failed to release FTN entry memory\n", __func__, __LINE__);
                    return SR_FAILURE;
                }
                SR_TRC3 (SR_FAIL_TRC, "Failed to delete CFA Interface ILM Tnl Id: %d, Prefix: %x, NextHop: %x\n",
                         pTnlDbEntry->u4SrTnlIfIndex, pTnlDbEntry->u4Prefix, pTnlDbEntry->u4NextHop);
                return SR_FAILURE;
            }
            SR_TRC3 (SR_CRITICAL_TRC, "CFA Interface deletion success for ILM Tnl Id: %d, Prefix: %x, NextHop: %x\n",
                     pTnlDbEntry->u4SrTnlIfIndex, pTnlDbEntry->u4Prefix, pTnlDbEntry->u4NextHop);
        }

        MEMSET (pSrTnlInfo, SR_ZERO, sizeof (tSrTnlIfTable));
        if (SR_MEM_FAILURE == SR_ILM_TNL_INFO_MEM_FREE(pSrTnlInfo))
        {
            SR_TRC2 (SR_FAIL_TRC, "%s:%d ERROR! Failed to release ILM entry memory\n", __func__, __LINE__);
            return SR_FAILURE;
        }
    }
    else
    {
        SR_TRC (SR_CRITICAL_TRC,"ERROR!! SrILMDbDeleteTnlInfoTableEntry. Failed to remove entry from RB tree \n");
    }

    SR_TRC2 (SR_CRITICAL_TRC,"EXIT:%s:%d SUCCESS\n",__func__,__LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************/
/* Function Name : SrFTNDbTnlInfoTableRestoreCallback                        */
/* Description   : Callback function to Restores the Tunnel info             */
/*                 table from STUB-DB to our local RB Tree                   */
/* Input(s)      : table name, key, data, operation                          */
/* Return(s)     : SR_SUCCESS/SR_FAILURE                                     */
/*****************************************************************************/
eDBRet
SrFTNDbTnlInfoTableRestoreCallback (CONST CHR1 *pTableName,
        CONST CHR1 *pEntryKey, CONST CHR1 *pEntryData, eOperation i1Operation)
{
    SR_TRC2 (SR_CRITICAL_TRC,"ENTRY:%s:%d\n",__func__,__LINE__);

    tSrTnlIfTable      srTnlDbInfo;
    CHR1               au1KeyStr [MAX_SR_TABLE_KEY_LEN];
    CHR1               au1DataStr [MAX_SR_TABLE_ENTRY_DATA_LEN];
    CHR1               *pTempKey = NULL;
    CHR1               *pTempData = NULL;

    MEMSET (&srTnlDbInfo, 0, sizeof(tSrTnlIfTable));
    MEMSET (au1KeyStr, 0, MAX_SR_TABLE_KEY_LEN);
    MEMSET (au1DataStr, 0, MAX_SR_TABLE_ENTRY_DATA_LEN);

    if (STRNCMP (pTableName, SR_FTN_TNL_INFO_TBL_NAME,
                STRLEN (SR_FTN_TNL_INFO_TBL_NAME)) != SR_ZERO)
    {
        SR_TRC (SR_FAIL_TRC,"SrFTNDbTnlInfoTableRestoreCallback: Recevied Invalid Table Name\n");
        return DB_FAILURE;
    }

    if ((i1Operation < OP_ADD) || (i1Operation > OP_EDIT))
    {
        SR_TRC(SR_FAIL_TRC, "SrFTNDbTnlInfoTableRestoreCallback: Received Invalid Operation\n");
        return DB_FAILURE;
    }

    /* Parsing the Tunnel info from the DB table */
    STRNCPY (au1KeyStr, pEntryKey, MAX_SR_TABLE_KEY_LEN);
    pTempKey = STRTOK (au1KeyStr, ",");
    if (pTempKey != NULL)
    {
        srTnlDbInfo.u4Prefix = (UINT4)ATOI(pTempKey);
    }
    pTempKey = STRTOK (NULL, ",");
    if (pTempKey != NULL)
    {
        srTnlDbInfo.u4NextHop = (UINT4)ATOI(pTempKey);
    }

    if ((i1Operation == OP_ADD) || (i1Operation == OP_EDIT))
    {
        STRNCPY (au1DataStr, pEntryData, MAX_SR_TABLE_ENTRY_DATA_LEN);
        pTempData = STRTOK (au1DataStr, ",");
        if (pTempData !=NULL)
        {
            srTnlDbInfo.u4SrTnlIfIndex = (UINT4)ATOI(pTempData);
        }

        if (SrFTNDbRestoreTnlInfoTableEntry (&srTnlDbInfo) != SR_SUCCESS)
        {
            SR_TRC (SR_CRITICAL_TRC, "SrFTNDbTnlInfoTableRestoreCallback: Restore of Tnl Info failed\n");
            return DB_FAILURE;
        }
    }
    else if (i1Operation == OP_DELETE)
    {
        STRNCPY (au1DataStr, pEntryData, MAX_SR_TABLE_ENTRY_DATA_LEN);
        pTempData = STRTOK (au1DataStr, ",");
        if (pTempData !=NULL)
        {
            srTnlDbInfo.u4SrTnlIfIndex = (UINT4)ATOI(pTempData);
        }

        if (SrFTNDbDeleteTnlInfoTableEntry (&srTnlDbInfo) != SR_SUCCESS)
        {
            SR_TRC (SR_FAIL_TRC, "SrFTNDbTnlInfoTableRestoreCallback: Restore of Tnl Info deletion failed\n");
            return DB_FAILURE;
        }
    }

    SR_TRC2 (SR_CRITICAL_TRC,"EXIT:%s:%d SUCCESS\n",__func__,__LINE__);
    return DB_SUCCESS;
}

/*****************************************************************************/
/* Function Name : SrILMDbTnlInfoTableRestoreCallback                        */
/* Description   : Callback function to Restores the Tunnel info             */
/*                 table from STUB-DB to our local RB Tree                   */
/* Input(s)      : table name, key, data, operation                          */
/* Return(s)     : SR_SUCCESS/SR_FAILURE                                     */
/*****************************************************************************/
eDBRet
SrILMDbTnlInfoTableRestoreCallback (CONST CHR1 *pTableName,
        CONST CHR1 *pEntryKey, CONST CHR1 *pEntryData, eOperation i1Operation)
{
    SR_TRC2 (SR_CRITICAL_TRC,"ENTRY:%s:%d\n",__func__,__LINE__);

    tSrTnlIfTable      srTnlDbInfo;
    CHR1               au1KeyStr [MAX_SR_TABLE_KEY_LEN];
    CHR1               au1DataStr [MAX_SR_TABLE_ENTRY_DATA_LEN];
    CHR1               *pTempKey = NULL;
    CHR1               *pTempData = NULL;

    MEMSET (&srTnlDbInfo, 0, sizeof(tSrTnlIfTable));
    MEMSET (au1KeyStr, 0, MAX_SR_TABLE_KEY_LEN);
    MEMSET (au1DataStr, 0, MAX_SR_TABLE_ENTRY_DATA_LEN);

    if (STRNCMP (pTableName, SR_ILM_TNL_INFO_TBL_NAME,
                STRLEN (SR_ILM_TNL_INFO_TBL_NAME)) != SR_ZERO)
    {
        SR_TRC (SR_FAIL_TRC, "SrILMDbTnlInfoTableRestoreCallback: Recevied Invalid Table Name\n");
        return DB_FAILURE;
    }

    if ((i1Operation < OP_ADD) || (i1Operation > OP_EDIT))
    {
        SR_TRC(SR_FAIL_TRC, "SrILMDbTnlInfoTableRestoreCallback: Received Invalid Operation\n");
        return DB_FAILURE;
    }

    /* Parsing the Tunnel info from the DB table */
    STRNCPY (au1KeyStr, pEntryKey, MAX_SR_TABLE_KEY_LEN);
    pTempKey = STRTOK (au1KeyStr, ",");
    if (pTempKey !=NULL)
    {
        srTnlDbInfo.u4Prefix = (UINT4)ATOI(pTempKey);
    }
    pTempKey = STRTOK (NULL, ",");
    if (pTempKey != NULL)
    {
        srTnlDbInfo.u4NextHop = (UINT4)ATOI(pTempKey);
    }

    if ((i1Operation == OP_ADD) || (i1Operation == OP_EDIT))
    {
        STRNCPY (au1DataStr, pEntryData, MAX_SR_TABLE_ENTRY_DATA_LEN);
        pTempData = STRTOK (au1DataStr, ",");
        if (pTempData !=NULL)
        {
            srTnlDbInfo.u4SrTnlIfIndex = (UINT4)ATOI(pTempData);
        }

        if (SrILMDbRestoreTnlInfoTableEntry (&srTnlDbInfo) != SR_SUCCESS)
        {
            SR_TRC (SR_FAIL_TRC, "SrILMDbTnlInfoTableRestoreCallback: Restore of Tnl Info failed\n");
            return DB_FAILURE;
        }
    }
    else if (i1Operation == OP_DELETE)
    {
        STRNCPY (au1DataStr, pEntryData, MAX_SR_TABLE_ENTRY_DATA_LEN);
        pTempData = STRTOK (au1DataStr, ",");
        if (pTempData !=NULL)
        {
            srTnlDbInfo.u4SrTnlIfIndex = (UINT4)ATOI(pTempData);
        }

        if (SrILMDbDeleteTnlInfoTableEntry (&srTnlDbInfo) != SR_SUCCESS)
        {
            SR_TRC (SR_FAIL_TRC, "SrILMDbTnlInfoTableRestoreCallback: Restore of Tnl Info deletion failed\n");
            return DB_FAILURE;
        }
    }

    SR_TRC2 (SR_CRITICAL_TRC,"EXIT:%s:%d SUCCESS\n",__func__,__LINE__);
    return DB_SUCCESS;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrFTNDbStoreTnlInfo                                        */
/*                                                                           */
/* Description  : This function stores the SR Tunnel IfIndex into StubDB     */
/*                                                                           */
/* Input        : SR Tunnel IfIndex Info                                     */
/*                                                                           */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : SR_SUCCESS/SR_FAILURE                                      */
/*                                                                           */
/*****************************************************************************/
INT4
SrFTNDbStoreTnlInfo (tSrTnlIfTable *pSrTnlInfo)
{
    SR_TRC2 (SR_CRITICAL_TRC,"ENTRY:%s:%d\n",__func__,__LINE__);

    CHR1        au1TableName [MAX_SR_TABLE_NAME_LEN];
    CHR1        au1Key [MAX_SR_TABLE_KEY_LEN];
    CHR1        au1EntryData [MAX_SR_TABLE_ENTRY_DATA_LEN];
    tXcEntry    *pXcEntry = NULL;
    UINT4       u4TnlInIfIndex = 0;
    UINT4       u4TnlOutIfIndex = 0;
    UINT4       u4IngressLsrID = 0;
    UINT4       u4EgressLsrID = 0;
    UINT4       u4InLabel = 0;
    UINT4       u4OutLabel = 0;
    UINT4       u4CurL3If = 0;
    UINT4       u4RevTnlIfIndex = 0;
    UINT1       u1CreateDynMplsIntf = SR_FALSE;

    MEMSET (au1TableName, 0, MAX_SR_TABLE_NAME_LEN);
    MEMSET (au1Key, 0, MAX_SR_TABLE_KEY_LEN);
    MEMSET (au1EntryData, 0, MAX_SR_TABLE_ENTRY_DATA_LEN);

    STRNCPY (au1TableName, SR_FTN_TNL_INFO_TBL_NAME, (MAX_SR_TABLE_NAME_LEN - 1));
    SNPRINTF (au1Key, MAX_SR_TABLE_KEY_LEN,"%d,%d", pSrTnlInfo->u4Prefix, pSrTnlInfo->u4NextHop);

    SNPRINTF (au1EntryData, MAX_SR_TABLE_ENTRY_DATA_LEN, "%d", pSrTnlInfo->u4SrTnlIfIndex);

#ifdef TEJAS_WANTED
    SR_TRC5 (SR_CRITICAL_TRC,"%s:%d prefix = %x nexthop = %x tunidx = %d \n",
             __func__,__LINE__, pSrTnlInfo->u4Prefix, pSrTnlInfo->u4NextHop, pSrTnlInfo->u4SrTnlIfIndex);
    /* Adding Tunnel Info table to DB */
    if (gDBAddEntry (au1TableName, au1Key, au1EntryData) == SR_FALSE)
    {
        SR_TRC (SR_FAIL_TRC, "DB Addition of TnlInfoTable failed\n");
        return SR_FAILURE;
    }
#endif
    SR_TRC2 (SR_CRITICAL_TRC,"EXIT:%s:%d SUCCESS\n",__func__,__LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrILMDbStoreTnlInfo                                        */
/*                                                                           */
/* Description  : This function stores the SR Tunnel IfIndex into StubDB     */
/*                                                                           */
/* Input        : SR Tunnel IfIndex Info                                     */
/*                                                                           */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : SR_SUCCESS/SR_FAILURE                                      */
/*                                                                           */
/*****************************************************************************/
INT4
SrILMDbStoreTnlInfo (tSrTnlIfTable *pSrTnlInfo)
{
    SR_TRC2 (SR_CRITICAL_TRC,"ENTRY:%s:%d\n",__func__,__LINE__);

    CHR1        au1TableName [MAX_SR_TABLE_NAME_LEN];
    CHR1        au1Key [MAX_SR_TABLE_KEY_LEN];
    CHR1        au1EntryData [MAX_SR_TABLE_ENTRY_DATA_LEN];
    tXcEntry    *pXcEntry = NULL;
    UINT4       u4TnlInIfIndex = 0;
    UINT4       u4TnlOutIfIndex = 0;
    UINT4       u4IngressLsrID = 0;
    UINT4       u4EgressLsrID = 0;
    UINT4       u4InLabel = 0;
    UINT4       u4OutLabel = 0;
    UINT4       u4CurL3If = 0;
    UINT4       u4RevTnlIfIndex = 0;
    UINT1       u1CreateDynMplsIntf = SR_FALSE;

    MEMSET (au1TableName, 0, MAX_SR_TABLE_NAME_LEN);
    MEMSET (au1Key, 0, MAX_SR_TABLE_KEY_LEN);
    MEMSET (au1EntryData, 0, MAX_SR_TABLE_ENTRY_DATA_LEN);

    STRNCPY (au1TableName, SR_ILM_TNL_INFO_TBL_NAME, (MAX_SR_TABLE_NAME_LEN - 1));
    SNPRINTF (au1Key, MAX_SR_TABLE_KEY_LEN,"%d,%d", pSrTnlInfo->u4Prefix, pSrTnlInfo->u4NextHop);

    SNPRINTF (au1EntryData, MAX_SR_TABLE_ENTRY_DATA_LEN, "%d",pSrTnlInfo->u4SrTnlIfIndex);

#ifdef TEJAS_WANTED
    /* Adding Tunnel Info table to DB */
    SR_TRC5 (SR_CRITICAL_TRC,"%s:%d prefix = %x nexthop = %x tunidx = %d \n",
             __func__,__LINE__, pSrTnlInfo->u4Prefix, pSrTnlInfo->u4NextHop, pSrTnlInfo->u4SrTnlIfIndex);
    if (gDBAddEntry (au1TableName, au1Key, au1EntryData) == SR_FALSE)
    {
        SR_TRC (SR_FAIL_TRC, "DB Addition of TnlInfoTable failed\n");
        return SR_FAILURE;
    }
#endif
    SR_TRC2 (SR_CRITICAL_TRC,"EXIT:%s:%d SUCCESS\n",__func__,__LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrDbDelFTNTnlInfoFromDB                                    */
/*                                                                           */
/* Description  : Function to remove Tunnel Info from DB                     */
/*                                                                           */
/* Input        : Tunnel Info                                                */
/*                                                                           */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : SR_SUCCESS/SR_FAILURE                                      */
/*                                                                           */
/*****************************************************************************/
INT4
SrDbDelFTNTnlInfoFromDB (UINT4 u4Prefix, UINT4 u4NextHop)
{
    SR_TRC2 (SR_CRITICAL_TRC,"ENTRY:%s:%d\n",__func__,__LINE__);

    CHR1 au1TableName [MAX_SR_TABLE_NAME_LEN];
    CHR1 au1Key [MAX_SR_TABLE_KEY_LEN];

    MEMSET (au1TableName, 0, MAX_SR_TABLE_NAME_LEN);
    MEMSET (au1Key, 0, MAX_SR_TABLE_KEY_LEN);

    /* Delete from FTN table */
    STRNCPY (au1TableName, SR_FTN_TNL_INFO_TBL_NAME, STRLEN(SR_FTN_TNL_INFO_TBL_NAME));
    SNPRINTF (au1Key, MAX_SR_TABLE_KEY_LEN,"%d,%d", u4Prefix, u4NextHop);

    SR_TRC1 (SR_CRITICAL_TRC, "Key: %s\n", au1Key);

#ifdef TEJAS_WANTED
    /* Deleting Tunnel Info table from DB */
    if (gDBDltEntry (au1TableName, au1Key) != DB_SUCCESS)
    {
        SR_TRC (SR_FAIL_TRC, "DB deletion of Tunnel Info failed\n");
        return SR_FAILURE;
    }
#endif

    SR_TRC2 (SR_CRITICAL_TRC,"EXIT:%s:%d SUCCESS\n",__func__,__LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrDbDelILMTnlInfoFromDB                                    */
/*                                                                           */
/* Description  : Function to remove Tunnel Info from DB                     */
/*                                                                           */
/* Input        : Tunnel Info                                                */
/*                                                                           */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : SR_SUCCESS/SR_FAILURE                                      */
/*                                                                           */
/*****************************************************************************/
INT4
SrDbDelILMTnlInfoFromDB (UINT4 u4Prefix, UINT4 u4NextHop)
{
    SR_TRC2 (SR_CRITICAL_TRC,"ENTRY:%s:%d\n",__func__,__LINE__);

    CHR1 au1TableName [MAX_SR_TABLE_NAME_LEN];
    CHR1 au1Key [MAX_SR_TABLE_KEY_LEN];

    MEMSET (au1TableName, 0, MAX_SR_TABLE_NAME_LEN);
    MEMSET (au1Key, 0, MAX_SR_TABLE_KEY_LEN);

    /* Delete from ILM table */
    STRNCPY (au1TableName, SR_ILM_TNL_INFO_TBL_NAME, STRLEN(SR_ILM_TNL_INFO_TBL_NAME));
    SNPRINTF (au1Key, MAX_SR_TABLE_KEY_LEN,"%d,%d", u4Prefix, u4NextHop);

    SR_TRC1 (SR_CRITICAL_TRC, "Key: %s\n", au1Key);

#ifdef TEJAS_WANTED
    /* Deleting Tunnel Info table from DB */
    if (gDBDltEntry (au1TableName, au1Key) != DB_SUCCESS)
    {
        SR_TRC (SR_FAIL_TRC, "DB deletion of Tunnel Info failed\n");
        return SR_FAILURE;
    }
#endif

    SR_TRC2 (SR_CRITICAL_TRC,"EXIT:%s:%d SUCCESS\n",__func__,__LINE__);
    return SR_SUCCESS;
}

/* RB Tree cleanup */
VOID
SrDbDeleteAllDbTnlInfo ()
{
    SR_TRC2 (SR_CRITICAL_TRC,"ENTRY:%s:%d\n",__func__,__LINE__);

    tSrTnlIfTable         *pSrFTNTnlDbInfo = NULL;
    tSrTnlIfTable         *pSrNextFTNTnlDbInfo = NULL;
    tSrTnlIfTable         *pSrILMTnlDbInfo = NULL;
    tSrTnlIfTable         *pSrNextILMTnlDbInfo = NULL;
    UINT4                 u4L3Ifindex =0;

    /* Cleaning up FTN table */
    pSrFTNTnlDbInfo = RBTreeGetFirst (gSrGlobalInfo.SrFTNTnlDbTree);

    while (pSrFTNTnlDbInfo != NULL)
    {
        /* Get the next Tnl-If-Index from the RB tree */
        pSrNextFTNTnlDbInfo = RBTreeGetNext (gSrGlobalInfo.SrFTNTnlDbTree,
                (tRBElem *) pSrFTNTnlDbInfo,
                NULL);

        SR_TRC3 (SR_CRITICAL_TRC, "Tnl Id: %d, Prefix: %x, NextHop: %x \n",
                 pSrFTNTnlDbInfo->u4SrTnlIfIndex, pSrFTNTnlDbInfo->u4Prefix, pSrFTNTnlDbInfo->u4NextHop);

        /* 1. Notify to CFA module to release the Tunnel If Index */
        if (CfaUtilGetIfIndexFromMplsTnlIf
                   (pSrFTNTnlDbInfo->u4SrTnlIfIndex, &u4L3Ifindex, TRUE) != CFA_FAILURE)
        {
            if(u4L3Ifindex != 0)
            {
                if (SrFtnCfaIfmDeleteStackMplsTunnelInterface
                           (pSrFTNTnlDbInfo->u4Prefix, pSrFTNTnlDbInfo->u4NextHop, u4L3Ifindex,
                            pSrFTNTnlDbInfo->u4SrTnlIfIndex) == SR_SUCCESS)
                {
                    SR_TRC2 (SR_CRITICAL_TRC,"MPLS tunnel interface deleted: %d cfa Index: %d\n",
                             pSrFTNTnlDbInfo->u4SrTnlIfIndex,u4L3Ifindex);
                }
                else
                {
                    SR_TRC2 (SR_FAIL_TRC,"Error in deleting MPLS tunnel interface: %d,cfa Index: %d\n",
                             pSrFTNTnlDbInfo->u4SrTnlIfIndex,u4L3Ifindex);
                }
            }
            else
            {
                if (CfaIfmDeleteDynamicMplsTunnelInterface
                    (pSrFTNTnlDbInfo->u4SrTnlIfIndex,NULL,TRUE) == CFA_SUCCESS)
                {
                    SR_TRC1 (SR_CRITICAL_TRC,"MPLS tunnel interface deleted: %d\n",
                             pSrFTNTnlDbInfo->u4SrTnlIfIndex);
                }
                else
                {
                    SR_TRC1 (SR_FAIL_TRC,"Error in deleting MPLS tunnel interface deleted: %d\n",
                             pSrFTNTnlDbInfo->u4SrTnlIfIndex);
                }
            }
        }

        /* 2. Delete the Tnl-If-Index entry from persistent DB */
        if (SrDbDelFTNTnlInfoFromDB(pSrFTNTnlDbInfo->u4Prefix, pSrFTNTnlDbInfo->u4NextHop) == SR_FAILURE)
        {
            SR_TRC2 (SR_FAIL_TRC,
                     "Error in deleting MPLS tunnel interface in FTN table, prefix: %x nexthop: %x\n",
                     pSrFTNTnlDbInfo->u4Prefix, pSrFTNTnlDbInfo->u4NextHop);
        }

        /* 3. Delete the Tnl-If-Index entry from the RB tree */
        if (RBTreeRem (gSrGlobalInfo.SrFTNTnlDbTree, pSrFTNTnlDbInfo) == NULL)
        {
            SR_TRC2 (SR_FAIL_TRC, "%s:%d ERROR! Failed to remove FTN entry from RB Tree\n",
                     __func__, __LINE__);
        }

        MEMSET (pSrFTNTnlDbInfo, SR_ZERO, sizeof (tSrTnlIfTable));
        if (SR_MEM_FAILURE == SR_FTN_TNL_INFO_MEM_FREE(pSrFTNTnlDbInfo))
        {
            SR_TRC2 (SR_FAIL_TRC, "%s:%d ERROR! Failed to release FTN entry memory\n", __func__, __LINE__);
        }

        pSrFTNTnlDbInfo = pSrNextFTNTnlDbInfo;
    }

    /* Cleaning up ILM table */
    pSrILMTnlDbInfo = RBTreeGetFirst (gSrGlobalInfo.SrILMTnlDbTree);

    while (pSrILMTnlDbInfo != NULL)
    {
        /* Get the next Tnl If Index */
        pSrNextILMTnlDbInfo = RBTreeGetNext (gSrGlobalInfo.SrILMTnlDbTree,(tRBElem *) pSrILMTnlDbInfo,NULL);

        SR_TRC3(SR_CRITICAL_TRC, "SrDbDeleteILMDbTnlInfo:  Tnl Id: %d, Prefix: %x NextHop: %x\n",
                pSrILMTnlDbInfo->u4SrTnlIfIndex, pSrILMTnlDbInfo->u4Prefix, pSrILMTnlDbInfo->u4NextHop);

        /* 1. Notify to CFA module to release the Tunnel If Index */
        if (CfaUtilGetIfIndexFromMplsTnlIf
                   (pSrILMTnlDbInfo->u4SrTnlIfIndex, &u4L3Ifindex, TRUE) != CFA_FAILURE)
        {
            if(u4L3Ifindex != 0)
            {
                if (SrIlmCfaIfmDeleteStackMplsTunnelInterface
                           (pSrILMTnlDbInfo->u4Prefix, pSrILMTnlDbInfo->u4NextHop, u4L3Ifindex,
                            pSrILMTnlDbInfo->u4SrTnlIfIndex) == SR_SUCCESS)
                {
                    SR_TRC1 (SR_CRITICAL_TRC,"MPLS tunnel interface deleted: %d\n",
                             pSrILMTnlDbInfo->u4SrTnlIfIndex);
                }
                else
                {
                    SR_TRC1 (SR_FAIL_TRC,"Error in deleting MPLS tunnel interface deleted: %d\n",
                             pSrILMTnlDbInfo->u4SrTnlIfIndex);
                }
            }
            else
            {
                if (CfaIfmDeleteDynamicMplsTunnelInterface
                           (pSrILMTnlDbInfo->u4SrTnlIfIndex,NULL,TRUE) == CFA_SUCCESS)
                {
                    SR_TRC1 (SR_CRITICAL_TRC,"MPLS tunnel interface deleted: %d\n",
                             pSrILMTnlDbInfo->u4SrTnlIfIndex);
                }
                else
                {
                    SR_TRC1 (SR_FAIL_TRC,"Error in deleting MPLS tunnel interface deleted: %d\n",
                             pSrILMTnlDbInfo->u4SrTnlIfIndex);
                }

             }
        }

        /* 2. Delete the Tnl-If-Index entry from persistent DB */
        if (SR_FAILURE == SrDbDelILMTnlInfoFromDB(pSrILMTnlDbInfo->u4Prefix, pSrILMTnlDbInfo->u4NextHop))
        {
            SR_TRC2 (SR_FAIL_TRC,"Error in deleting MPLS tunnel interface in ILM table, prefix: %x, nexthop: %x\n",
                     pSrILMTnlDbInfo->u4Prefix, pSrILMTnlDbInfo->u4NextHop);
        }

        /* 3. Delete the Tnl-If-Index entry from RB tree */
        if (RBTreeRem (gSrGlobalInfo.SrILMTnlDbTree, pSrILMTnlDbInfo) == NULL)
        {
            SR_TRC2 (SR_FAIL_TRC, "%s:%d ERROR! Failed to remove ILM entry from RB Tree\n",
                     __func__, __LINE__);
        }

        MEMSET (pSrILMTnlDbInfo, SR_ZERO, sizeof (tSrTnlIfTable));
        if (SR_MEM_FAILURE == SR_ILM_TNL_INFO_MEM_FREE(pSrILMTnlDbInfo))
        {
            SR_TRC2 (SR_FAIL_TRC, "%s:%d ERROR! Failed to release ILM entry memory\n", __func__, __LINE__);
        }

        pSrILMTnlDbInfo = pSrNextILMTnlDbInfo;
    }

    SR_TRC2 (SR_CRITICAL_TRC,"EXIT:%s:%d SUCCESS\n",__func__,__LINE__);
    return;
}
