/*******************************************************************
* Copyright (C) 2024 Tejas Inc . All Rights Reserved
*
* $Id: srgr.c,v 1.13 2024/10/02 17:03:00 $
*
********************************************************************/

/******************************************************************************
*    FILE  NAME             : srgr.c
*    PRINCIPAL AUTHOR       : Aricent Inc.
*    SUBSYSTEM NAME         : MPLS
*    MODULE NAME            : SR
*    LANGUAGE               : ANSI-C
*    TARGET ENVIRONMENT     : Linux (Portable)
*    DATE OF FIRST RELEASE  :
*    DESCRIPTION            : This file contains routines for the GR functionalities.
*
*---------------------------------------------------------------------------*/

#include "srincs.h"
#include "rtm.h"
#include "mplshwlist.h"
#include "srglob.h"
#include "srrm.h"
#include "srdef.h"

/*****************************************************************************/
/* Function Name : IsSidConfigured                                           */
/* Description   : This function checks the any SID is configured or not.    */
/*                 If configured then returns SUCCESS otherwise FAILURE      */
/* Input(s)      : None                                                      */
/* Output(s)     : None                                                      */
/* Return(s)     : SR_SUCCESS or SR_FAILURE                                  */
/*****************************************************************************/
UINT1
IsSidConfigured (void)
{
    tSrSidInterfaceInfo   *pSrSidIntf = NULL;

    pSrSidIntf = RBTreeGetFirst(gSrGlobalInfo.pSrSidRbTree);

    while(pSrSidIntf != NULL)
    {
        if(pSrSidIntf->u4SidType == SR_SID_NODE)
        {
            return SR_SUCCESS;
        }
        pSrSidIntf = RBTreeGetNext(gSrGlobalInfo.pSrSidRbTree,pSrSidIntf,NULL);
    }

    SR_TRC2 (SR_FAIL_TRC, "%s:%d : RBTreeGetFirst Failed \n",
            __func__, __LINE__);
    return SR_FAILURE;

}

/*****************************************************************************/
/* Function Name : SrProgramMPLSentriesAfterGR                               */
/* Description   : This function performs GR procedure                       */
/*                 If configured then returns SUCCESS otherwise FAILURE      */
/* Input(s)      : None                                                      */
/* Output(s)     : None                                                      */
/* Return(s)     : SR_SUCCESS or SR_FAILURE                                  */
/*****************************************************************************/
VOID
SrProgramMPLSentriesAfterGR(void)
{
    SR_TRC2 (SR_CRITICAL_TRC, "%s:%d ENTRY \n", __func__, __LINE__);

    tTMO_SLL_NODE       *pRtrNode = NULL;
    tTMO_SLL_NODE       *pSrRtrNextHopNode = NULL;
    tSrRtrNextHopInfo   *pSrRtrNextHopInfo = NULL;
    tSrTnlIfTable       *pSrTnlDbInfo = NULL;
    tSrRtrInfo          *pRtrInfo = NULL;
    tSrInSegInfo        SrInSegInfo;
    tSrTnlIfTable       srTnlTable;
    tSrTnlIfTable       *pTnlDbEntry = NULL;
    UINT4               u4TnlIntf = 0;
    UINT4               u4L3Intf = 0;

    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));
    MEMSET (&srTnlTable, SR_ZERO, sizeof (tSrTnlIfTable));

    /* 1. Loop through all the LSAs */
    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pRtrInfo = (tSrRtrInfo *) pRtrNode;
        /* SR-TODO: Process the LSA only if
           1) check the conflict flag: Check whether the LSA has been marked as conflicted
              if conflict-flag is set, check whether it is a winner then only process
           2) check if confict flag is not set
        */

        TMO_SLL_Scan (&(pRtrInfo->NextHopList), pSrRtrNextHopNode, tTMO_SLL_NODE *)
        {
            pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;

            /* 2. If nh is available */
            if (pSrRtrNextHopNode != NULL)
            {
                /* create ILM entry */
                if (SrMplsCreateILM (pRtrInfo, pSrRtrNextHopInfo) == SR_SUCCESS)
                {
                    SR_TRC5 (SR_CRITICAL_TRC, "%s:%d created ILM for Prefix: %x, NextHop: %x, TnlIfIndex: %d",
                            __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr,
                            pSrRtrNextHopInfo->nextHop.Addr.u4Addr,
                            pSrRtrNextHopInfo->u4SwapOutIfIndex);
                }
                else
                {
                    SR_TRC5 (SR_FAIL_TRC, "%s:%d Failed to create ILM. Prefix: %x, NextHop: %x, TnlIfIndex: %d",
                            __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr,
                            pSrRtrNextHopInfo->nextHop.Addr.u4Addr,
                            pSrRtrNextHopInfo->u4SwapOutIfIndex);
                }

                /* create FTN entry */
                if (SrMplsCreateFTN (pRtrInfo, pSrRtrNextHopInfo) == SR_SUCCESS)
                {
                    SR_TRC5 (SR_CRITICAL_TRC, "%s:%d created FTN for Prefix: %x, NextHop: %x, tTnlIfIndex: %d",
                            __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr,
                            pSrRtrNextHopInfo->nextHop.Addr.u4Addr,
                            pSrRtrNextHopInfo->u4OutIfIndex);
                }
                else
                {
                    SR_TRC5 (SR_FAIL_TRC, "%s:%d Failed to create FTN. Prefix: %x, NextHop: %x, TnlIfIndex: %d",
                            __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr,
                            pSrRtrNextHopInfo->nextHop.Addr.u4Addr,
                            pSrRtrNextHopInfo->u4OutIfIndex);
                }
            }
            else
            {
                /** SR-TODO:
                        1) if NH is not available, query it from RTM - possible bcz of OSPF_SR failure
                           cleanup the local DB and inform StubDB also

                        2) For the same prefix, multiple tunnl ifindex could have been created.
                           check this case - swapOutIfIndex
                           chk: peerinfo details - CLI
                **/
            }
        }
    }

    SR_TRC2 (SR_CRITICAL_TRC,"EXIT:%s:%d SUCCESS\n",__func__,__LINE__);
}

/*****************************************************************************/
/* Function Name : SrDeleteStaleEntries                                      */
/* Description   : This function deletes stale entries                       */
/* Input(s)      : None                                                      */
/* Output(s)     : None                                                      */
/* Return(s)     : None                                                      */
/*****************************************************************************/
VOID
SrDeleteStaleEntries ()
{
    SR_TRC2 (SR_CRITICAL_TRC,"ENTRY:%s:%d\n",__func__,__LINE__);

    tSrTnlIfTable         *pSrFTNTnlDbInfo = NULL;
    tSrTnlIfTable         *pSrILMTnlDbInfo = NULL;
    UINT4                 u4L3Ifindex =0;

    /* Cleaning up FTN table */
    pSrFTNTnlDbInfo = RBTreeGetFirst (gSrGlobalInfo.SrFTNTnlDbTree);

    while (pSrFTNTnlDbInfo != NULL)
    {
        SR_TRC4 (SR_CRITICAL_TRC, "SrDeleteStaleEntries:  Tnl Id: %d, Prefix: %x NextHop: %x Stale : %d\n",
                 pSrFTNTnlDbInfo->u4SrTnlIfIndex, pSrFTNTnlDbInfo->u4Prefix, pSrFTNTnlDbInfo->u4NextHop, pSrFTNTnlDbInfo->u1stale);

        if(pSrFTNTnlDbInfo->u1stale == SR_TRUE)
        {
            /* 1. Notify to CFA module to release the Tunnel If Index */
            if (CfaUtilGetIfIndexFromMplsTnlIf
                       (pSrFTNTnlDbInfo->u4SrTnlIfIndex, &u4L3Ifindex, TRUE) != CFA_FAILURE)
            {
                if(u4L3Ifindex != 0)
                {
                    if (SrFtnCfaIfmDeleteStackMplsTunnelInterface
                               (pSrFTNTnlDbInfo->u4Prefix,
                                pSrFTNTnlDbInfo->u4NextHop,
                                u4L3Ifindex,
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

            /* 4. Release memory */
            MEMSET (pSrFTNTnlDbInfo, SR_ZERO, sizeof (tSrTnlIfTable));
            if (SR_MEM_FAILURE == SR_FTN_TNL_INFO_MEM_FREE(pSrFTNTnlDbInfo))
            {
                SR_TRC2 (SR_FAIL_TRC, "%s:%d ERROR! Failed to release FTN entry memory\n", __func__, __LINE__);
            }

        }
        /* Get the next Tnl-If-Index from the RB tree */
        pSrFTNTnlDbInfo = RBTreeGetNext (gSrGlobalInfo.SrFTNTnlDbTree,
                (tRBElem *) pSrFTNTnlDbInfo,
                NULL);
   }

    /* Cleaning up ILM table */
    pSrILMTnlDbInfo = RBTreeGetFirst (gSrGlobalInfo.SrILMTnlDbTree);

    while (pSrILMTnlDbInfo != NULL)
    {
        SR_TRC4 (SR_CRITICAL_TRC, "SrDeleteStaleEntries:  Tnl Id: %d, Prefix: %x, NextHop: %x, Stale : %d\n",
                 pSrILMTnlDbInfo->u4SrTnlIfIndex, pSrILMTnlDbInfo->u4Prefix, pSrILMTnlDbInfo->u4NextHop, pSrILMTnlDbInfo->u1stale);

        if(pSrILMTnlDbInfo->u1stale == SR_TRUE)
        {
            /* 1. Notify to CFA module to release the Tunnel If Index */
            if (CfaUtilGetIfIndexFromMplsTnlIf
                       (pSrILMTnlDbInfo->u4SrTnlIfIndex, &u4L3Ifindex, TRUE) != CFA_FAILURE)
            {
                if(u4L3Ifindex != 0)
                {
                    if (SrIlmCfaIfmDeleteStackMplsTunnelInterface
                               (pSrILMTnlDbInfo->u4Prefix,
                                pSrILMTnlDbInfo->u4NextHop,
                                u4L3Ifindex,
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
                SR_TRC2 (SR_FAIL_TRC,"Error in deleting MPLS tunnel interface in ILM table, prefix: %x nexthop: %x\n",
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

        }
        /* Get the next Tnl-If-Index from the RB tree */
        pSrILMTnlDbInfo = RBTreeGetNext (gSrGlobalInfo.SrILMTnlDbTree,
                (tRBElem *) pSrILMTnlDbInfo,
                NULL);
   }

    SR_TRC2 (SR_CRITICAL_TRC,"EXIT:%s:%d SUCCESS\n",__func__,__LINE__);
    return;
}
