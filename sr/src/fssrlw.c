/********************************************************************
* Copyright (C) 2006 Aricent Inc . All Rights Reserved
*
* $Id: main.c,v 1.1 2017/07/17 13:33:08 siva Exp $
*
* Description: Protocol Low Level Routines
*********************************************************************/
# include  "lr.h"
# include  "fssnmp.h"
# include  "srincs.h"
# include  "srclip.h"
extern tSrAltModuleStatus gu4AltModuleStatus;

/* Low Level GET Routine for All Objects  */

/****************************************************************************
 Function    :  nmhGetFsSrMode
 Input       :  The Indices

                The Object 
                retValFsSrMode
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrMode (INT4 *pi4RetValFsSrMode)
{
    *pi4RetValFsSrMode = (INT4) gSrGlobalInfo.u4SrMode;
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFsSrV4MinSrGbValue
 Input       :  The Indices

                The Object 
                retValFsSrV4MinSrGbValue
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrV4MinSrGbValue (UINT4 *pu4RetValFsSrV4MinSrGbValue)
{
    if (IS_V4_SRGB_CONFIGURED == SR_TRUE)
    {
        *pu4RetValFsSrV4MinSrGbValue =
            gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex;
    }
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFsSrV4MaxSrGbValue
 Input       :  The Indices

                The Object 
                retValFsSrV4MaxSrGbValue
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrV4MaxSrGbValue (UINT4 *pu4RetValFsSrV4MaxSrGbValue)
{
    if (IS_V4_SRGB_CONFIGURED == SR_TRUE)
    {
        *pu4RetValFsSrV4MaxSrGbValue =
            gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMaxIndex;
    }
    return SNMP_SUCCESS;
}

/* Low Level SET Routine for All Objects  */

/****************************************************************************
 Function    :  nmhSetFsSrMode
 Input       :  The Indices

                The Object 
                setValFsSrMode
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrMode (INT4 i4SetValFsSrMode)
{
    gSrGlobalInfo.u4SrMode = (UINT4) i4SetValFsSrMode;
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhSetFsSrV4MinSrGbValue
 Input       :  The Indices

                The Object 
                setValFsSrV4MinSrGbValue
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrV4MinSrGbValue (UINT4 u4SetValFsSrV4MinSrGbValue)
{
    if (u4SetValFsSrV4MinSrGbValue != 0)
    {
        gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex =
            u4SetValFsSrV4MinSrGbValue;
        IS_V4_SRGB_CONFIGURED = SR_TRUE;
        return SNMP_SUCCESS;
    }
    return SNMP_FAILURE;
}

/****************************************************************************
 Function    :  nmhSetFsSrV4MaxSrGbValue
 Input       :  The Indices

                The Object 
                setValFsSrV4MaxSrGbValue
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrV4MaxSrGbValue (UINT4 u4SetValFsSrV4MaxSrGbValue)
{
    if (u4SetValFsSrV4MaxSrGbValue != 0)
    {
        gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMaxIndex =
            u4SetValFsSrV4MaxSrGbValue;
        IS_V4_SRGB_CONFIGURED = SR_TRUE;
        return SNMP_SUCCESS;
    }
    return SNMP_FAILURE;
}

/* Low Level TEST Routines for All Objects  */

/****************************************************************************
 Function    :  nmhTestv2FsSrMode
 Input       :  The Indices

                The Object 
                testValFsSrMode
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrMode (UINT4 *pu4ErrorCode, INT4 i4TestValFsSrMode)
{
#ifdef IP6_WANTED
    UINT4               u4SrCxtId = SR_ZERO;
    UINT1               u1RelVal = SR_ZERO;
#endif
    switch (i4TestValFsSrMode)
    {
        case SR_CONF_STATIC:
        case SR_CONF_OSPF:
#ifdef OSPF_WANTED
            if (SR_SUCCESS ==
                SrUtilV2CapabilityOpaqueStatus (&u1RelVal, u4SrCxtId))
            {
                if (u1RelVal == SR_DISABLED)
                {
                    *pu4ErrorCode = SNMP_ERR_AUTHORIZATION_ERROR;
                    CLI_SET_ERR (CLI_OPAQUE_DISABLED);
                    return SNMP_FAILURE;
                }
            }
#endif
            break;

        case SR_CONF_OSPFV3:
#ifdef OSPF3_WANTED
            if (SR_SUCCESS ==
                SrUtilV3CapabilityOpaqueStatus (&u1RelVal, u4SrCxtId))
            {
                if (u1RelVal == SR_DISABLED)
                {
                    *pu4ErrorCode = SNMP_ERR_AUTHORIZATION_ERROR;
                    CLI_SET_ERR (CLI_OPAQUE_DISABLED);
                    return SNMP_FAILURE;
                }
                if (SR_DISABLED == SrV6GlobalAdminStatus ())
                {
                    *pu4ErrorCode = SNMP_ERR_AUTHORIZATION_ERROR;
                    CLI_SET_ERR (CLI_SR_DISABLED);
                    return SNMP_FAILURE;
                }
            }
#endif
            break;

        case SR_CONF_STATIC_V6:
            return SNMP_SUCCESS;
            break;

        default:
            *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
            return SNMP_FAILURE;
    }
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhTestv2FsSrV4MinSrGbValue
 Input       :  The Indices

                The Object 
                testValFsSrV4MinSrGbValue
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrV4MinSrGbValue (UINT4 *pu4ErrorCode,
                             UINT4 u4TestValFsSrV4MinSrGbValue)
{
    INT4                i4SrStatus = SR_DISABLED;
    UINT4               u4v6MinSrgbVal = SR_ZERO;
    UINT4               u4v6MaxSrgbVal = SR_ZERO;
    UINT4               isNodeSidPresent = SR_FALSE;
    tSrSidInterfaceInfo *pSrSidIntf = NULL;

    nmhGetFsSrV4Status (&i4SrStatus);

    if (i4SrStatus == SR_ENABLED)
    {
        *pu4ErrorCode = SNMP_ERR_AUTHORIZATION_ERROR;
        CLI_SET_ERR (CLI_SR_CONFIG_FAIL);
        return SNMP_FAILURE;
    }

    if ((u4TestValFsSrV4MinSrGbValue <
         gSystemSize.MplsSystemSize.u4MinSrLblRange)
        || (u4TestValFsSrV4MinSrGbValue >=
            gSystemSize.MplsSystemSize.u4MaxSrLblRange))
    {
        *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
        CLI_SET_ERR (CLI_SRGB_MIN_INV_VAL);
        return SNMP_FAILURE;
    }

    nmhGetFsSrV6MinSrGbValue (&u4v6MinSrgbVal);
    nmhGetFsSrV6MaxSrGbValue (&u4v6MaxSrgbVal);
    if ((u4v6MinSrgbVal != SR_ZERO) && (u4v6MaxSrgbVal != SR_ZERO) &&
        (u4TestValFsSrV4MinSrGbValue >= u4v6MinSrgbVal) &&
        (u4TestValFsSrV4MinSrGbValue <= u4v6MaxSrgbVal))
    {
        *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
        CLI_SET_ERR (CLI_SR_V4_INV_RANGE);
        return SNMP_FAILURE;
    }

    pSrSidIntf = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);

    while (pSrSidIntf != NULL)
    {
        if (pSrSidIntf->u4SidType != SR_SID_NODE)
        {
            /* Adj SID configured */
            if ((IS_V4_SRGB_CONFIGURED == SR_TRUE) &&
                pSrSidIntf->u4PrefixSidLabelIndex >= u4TestValFsSrV4MinSrGbValue
                && pSrSidIntf->u4PrefixSidLabelIndex <= SR_V4_SRGB_MAX_VAL)
            {
                *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
                CLI_SET_ERR (CLI_SR_INV_VAL);
                return SNMP_FAILURE;
            }
            else
            {
                pSrSidIntf =
                    RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree, pSrSidIntf,
                                   NULL);
            }
        }
        else
        {
            /* Node SID Configured */
            isNodeSidPresent = SR_TRUE;
            break;
        }
    }

    if (isNodeSidPresent == SR_TRUE)
    {
        *pu4ErrorCode = SNMP_ERR_AUTHORIZATION_ERROR;
        CLI_SET_ERR (CLI_SR_NODE_SID_EXIST);
        return SNMP_FAILURE;
    }
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhTestv2FsSrV4MaxSrGbValue
 Input       :  The Indices

                The Object 
                testValFsSrV4MaxSrGbValue
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrV4MaxSrGbValue (UINT4 *pu4ErrorCode,
                             UINT4 u4TestValFsSrV4MaxSrGbValue)
{
    INT4                i4SrStatus = SR_DISABLED;
    UINT4               u4v6MinSrgbVal = SR_ZERO;
    UINT4               u4v6MaxSrgbVal = SR_ZERO;
    UINT4               isNodeSidPresent = SR_FALSE;
    tSrSidInterfaceInfo *pSrSidIntf = NULL;

    nmhGetFsSrV4Status (&i4SrStatus);

    if (i4SrStatus == SR_ENABLED)
    {
        *pu4ErrorCode = SNMP_ERR_AUTHORIZATION_ERROR;
        CLI_SET_ERR (CLI_SR_CONFIG_FAIL);
        return SNMP_FAILURE;
    }

    if ((u4TestValFsSrV4MaxSrGbValue <=
         gSystemSize.MplsSystemSize.u4MinSrLblRange)
        || (u4TestValFsSrV4MaxSrGbValue >
            gSystemSize.MplsSystemSize.u4MaxSrLblRange))
    {
        *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
        CLI_SET_ERR (CLI_SRGB_MIN_INV_VAL);
        return SNMP_FAILURE;
    }

    if (SR_V4_SRGB_MIN_VAL == 0)
    {
        *pu4ErrorCode = SNMP_ERR_NO_CREATION;
         CLI_SET_ERR (CLI_SRGB_MIN_NOT_CONFIGURED);
         return SNMP_FAILURE;
    }

    if ((SR_V4_SRGB_MIN_VAL != 0) &&  
        ((IS_V4_SRGB_CONFIGURED == SR_TRUE) &&
         ((INT4) (u4TestValFsSrV4MaxSrGbValue - SR_V4_SRGB_MIN_VAL) >
          MAX_SRGB_SIZE)))
    {
        *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
        CLI_SET_ERR (CLI_SRGB_MAX_INV_VAL);
        return SNMP_FAILURE;
    }

    nmhGetFsSrV6MinSrGbValue (&u4v6MinSrgbVal);
    nmhGetFsSrV6MaxSrGbValue (&u4v6MaxSrgbVal);
    if ((u4v6MinSrgbVal != SR_ZERO) && (u4v6MaxSrgbVal != SR_ZERO) &&
        (u4TestValFsSrV4MaxSrGbValue >= u4v6MinSrgbVal) &&
        (u4TestValFsSrV4MaxSrGbValue <= u4v6MaxSrgbVal))
    {
        *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
        CLI_SET_ERR (CLI_SR_V4_INV_RANGE);
        return SNMP_FAILURE;
    }

    pSrSidIntf = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);
    while (pSrSidIntf != NULL)
    {
        if (pSrSidIntf->u4SidType != SR_SID_NODE)
        {
            if ((IS_V4_SRGB_CONFIGURED == SR_TRUE) &&
                pSrSidIntf->u4PrefixSidLabelIndex <= u4TestValFsSrV4MaxSrGbValue
                && pSrSidIntf->u4PrefixSidLabelIndex >= SR_V4_SRGB_MIN_VAL)
            {
                *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
                CLI_SET_ERR (CLI_SR_INV_VAL);
                return SNMP_FAILURE;
            }
            else
            {
                pSrSidIntf =
                    RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree, pSrSidIntf,
                                   NULL);
            }
        }
        else
        {
            isNodeSidPresent = SR_TRUE;
            break;
        }
    }

    if (isNodeSidPresent == SR_TRUE)
    {
        *pu4ErrorCode = SNMP_ERR_AUTHORIZATION_ERROR;
        CLI_SET_ERR (CLI_SR_NODE_SID_EXIST);
        return SNMP_FAILURE;
    }

    return SNMP_SUCCESS;
}

/* Low Level Dependency Routines for All Objects  */

/****************************************************************************
 Function    :  nmhDepv2FsSrMode
 Output      :  The Dependency Low Lev Routine Take the Indices &
                check whether dependency is met or not.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhDepv2FsSrMode (UINT4 *pu4ErrorCode, tSnmpIndexList * pSnmpIndexList,
                  tSNMP_VAR_BIND * pSnmpVarBind)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (pSnmpIndexList);
    UNUSED_PARAM (pSnmpVarBind);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhDepv2FsSrV4MinSrGbValue
 Output      :  The Dependency Low Lev Routine Take the Indices &
                check whether dependency is met or not.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhDepv2FsSrV4MinSrGbValue (UINT4 *pu4ErrorCode,
                            tSnmpIndexList * pSnmpIndexList,
                            tSNMP_VAR_BIND * pSnmpVarBind)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (pSnmpIndexList);
    UNUSED_PARAM (pSnmpVarBind);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhDepv2FsSrV4MaxSrGbValue
 Output      :  The Dependency Low Lev Routine Take the Indices &
                check whether dependency is met or not.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhDepv2FsSrV4MaxSrGbValue (UINT4 *pu4ErrorCode,
                            tSnmpIndexList * pSnmpIndexList,
                            tSNMP_VAR_BIND * pSnmpVarBind)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (pSnmpIndexList);
    UNUSED_PARAM (pSnmpVarBind);
    return SNMP_SUCCESS;
}

/* LOW LEVEL Routines for Table : FsSrSIDTable. */

/****************************************************************************
 Function    :  nmhValidateIndexInstanceFsSrSIDTable
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask
 Output      :  The Routines Validates the Given Indices.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
/* GET_EXACT Validate Index Instance Routine. */

INT1
nmhValidateIndexInstanceFsSrSIDTable (INT4 i4FsSrSIDAddrType,
                                      tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                                      UINT4 u4FsSrSIDMask)
{
    UNUSED_PARAM (i4FsSrSIDAddrType);
    UNUSED_PARAM (pFsSrSIDNetIp);
    UNUSED_PARAM (u4FsSrSIDMask);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFirstIndexFsSrSIDTable
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask
 Output      :  The Get First Routines gets the Lexicographicaly
                First Entry from the Table.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
/* GET_FIRST Routine. */

INT1
nmhGetFirstIndexFsSrSIDTable (INT4 *pi4FsSrSIDAddrType,
                              tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                              UINT4 *pu4FsSrSIDMask)
{
    tSrSidInterfaceInfo *pSrSidIntf = NULL;

    pSrSidIntf = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);

    if (pSrSidIntf == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGetFirst Failed\n", __func__,
                __LINE__);
        return SNMP_FAILURE;
    }

    *pi4FsSrSIDAddrType = pSrSidIntf->ipAddrType;
    *pu4FsSrSIDMask = pSrSidIntf->ifIpAddrMask;
    if (pSrSidIntf->ifIpAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        SR_INTEGER_TO_OCTETSTRING (pSrSidIntf->ifIpAddr.Addr.u4Addr,
                                   pFsSrSIDNetIp);
    }
    else if (pSrSidIntf->ifIpAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        MEMCPY (pFsSrSIDNetIp->pu1_OctetList,
                pSrSidIntf->ifIpAddr.Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);

        pFsSrSIDNetIp->i4_Length = SR_IPV6_ADDR_LENGTH;
    }
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetNextIndexFsSrSIDTable
 Input       :  The Indices
                FsSrSIDAddrType
                nextFsSrSIDAddrType
                FsSrSIDNetIp
                nextFsSrSIDNetIp
                FsSrSIDMask
                nextFsSrSIDMask
 Output      :  The Get Next function gets the Next Index for
                the Index Value given in the Index Values. The
                Indices are stored in the next_varname variables.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
/* GET_NEXT Routine.  */
INT1
nmhGetNextIndexFsSrSIDTable (INT4 i4FsSrSIDAddrType,
                             INT4 *pi4NextFsSrSIDAddrType,
                             tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                             tSNMP_OCTET_STRING_TYPE * pNextFsSrSIDNetIp,
                             UINT4 u4FsSrSIDMask, UINT4 *pu4NextFsSrSIDMask)
{
    tSrSidInterfaceInfo SrSidIntf;
    tSrSidInterfaceInfo *pSrSidEntry = NULL;

    MEMSET (&SrSidIntf, SR_ZERO, sizeof (tSrSidInterfaceInfo));

    SrSidIntf.ipAddrType = (UINT1) i4FsSrSIDAddrType;
    SrSidIntf.ifIpAddrMask = u4FsSrSIDMask;

    if (SrSidIntf.ipAddrType == SR_IPV4_ADDR_TYPE)
    {
        SrSidIntf.ifIpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
        SR_OCTETSTRING_TO_INTEGER (pFsSrSIDNetIp,
                                   SrSidIntf.ifIpAddr.Addr.u4Addr);
    }
    else if (SrSidIntf.ipAddrType == SR_IPV6_ADDR_TYPE)
    {
        SrSidIntf.ifIpAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
        MEMCPY (SrSidIntf.ifIpAddr.Addr.Ip6Addr.u1_addr,
                pFsSrSIDNetIp->pu1_OctetList, pFsSrSIDNetIp->i4_Length);
    }

    pSrSidEntry = RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree, &SrSidIntf, NULL);

    if (pSrSidEntry == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGetFirst Failed\n", __func__,
                __LINE__);
        return SNMP_FAILURE;
    }
    *pi4NextFsSrSIDAddrType = pSrSidEntry->ipAddrType;

    if (pSrSidEntry->ifIpAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        SR_INTEGER_TO_OCTETSTRING (pSrSidEntry->ifIpAddr.Addr.u4Addr,
                                   pNextFsSrSIDNetIp);
    }
    else if (pSrSidEntry->ifIpAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        MEMCPY (pNextFsSrSIDNetIp->pu1_OctetList,
                pSrSidEntry->ifIpAddr.Addr.Ip6Addr.u1_addr,
                SR_IPV6_ADDR_LENGTH);

        pNextFsSrSIDNetIp->i4_Length = SR_IPV6_ADDR_LENGTH;
    }

    *pu4NextFsSrSIDMask = pSrSidEntry->ifIpAddrMask;
    return SNMP_SUCCESS;
}

/* Low Level GET Routine for All Objects  */

/****************************************************************************
 Function    :  nmhGetFsSrSIDType
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                retValFsSrSIDType
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrSIDType (INT4 i4FsSrSIDAddrType,
                   tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                   UINT4 u4FsSrSIDMask, INT4 *pi4RetValFsSrSIDType)
{
    tSrSidInterfaceInfo SrSidIntf;
    tSrSidInterfaceInfo *pSrSidEntry = NULL;

    MEMSET (&SrSidIntf, SR_ZERO, sizeof (tSrSidInterfaceInfo));

    SrSidIntf.ipAddrType = (UINT1) i4FsSrSIDAddrType;
    SrSidIntf.ifIpAddrMask = u4FsSrSIDMask;

    if (SrSidIntf.ipAddrType == SR_IPV4_ADDR_TYPE)
    {
        SrSidIntf.ifIpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
        SR_OCTETSTRING_TO_INTEGER (pFsSrSIDNetIp,
                                   SrSidIntf.ifIpAddr.Addr.u4Addr);
    }
    else if (SrSidIntf.ipAddrType == SR_IPV6_ADDR_TYPE)
    {
        SrSidIntf.ifIpAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
        MEMCPY (SrSidIntf.ifIpAddr.Addr.Ip6Addr.u1_addr,
                pFsSrSIDNetIp->pu1_OctetList, pFsSrSIDNetIp->i4_Length);
    }

    pSrSidEntry = RBTreeGet (gSrGlobalInfo.pSrSidRbTree, &SrSidIntf);

    if (pSrSidEntry == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGet FAILED\n", __func__,
                __LINE__);
        return SNMP_FAILURE;
    }

    if (SrSidIntf.ipAddrType == SR_IPV4_ADDR_TYPE)
    {
        *pi4RetValFsSrSIDType = (INT4) pSrSidEntry->u4SidType;
    }
    else if (SrSidIntf.ipAddrType == SR_IPV6_ADDR_TYPE)
    {
        *pi4RetValFsSrSIDType = (INT4) pSrSidEntry->u4SidTypeV3;
    }

    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFsSrSidIndexType
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                retValFsSrSidIndexType
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrSidIndexType (INT4 i4FsSrSIDAddrType,
                        tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                        UINT4 u4FsSrSIDMask, INT4 *pi4RetValFsSrSidIndexType)
{
    tSrSidInterfaceInfo SrSidIntf;
    tSrSidInterfaceInfo *pSrSidEntry = NULL;

    MEMSET (&SrSidIntf, SR_ZERO, sizeof (tSrSidInterfaceInfo));

    SrSidIntf.ipAddrType = (UINT1) i4FsSrSIDAddrType;
    SrSidIntf.ifIpAddrMask = u4FsSrSIDMask;

    if (i4FsSrSIDAddrType == SR_IPV4_ADDR_TYPE)
    {
        SrSidIntf.ifIpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
        SR_OCTETSTRING_TO_INTEGER (pFsSrSIDNetIp,
                                   SrSidIntf.ifIpAddr.Addr.u4Addr);
    }
    else if (i4FsSrSIDAddrType == SR_IPV6_ADDR_TYPE)
    {
        SrSidIntf.ifIpAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
        MEMCPY (SrSidIntf.ifIpAddr.Addr.Ip6Addr.u1_addr,
                pFsSrSIDNetIp->pu1_OctetList, pFsSrSIDNetIp->i4_Length);
    }

    pSrSidEntry = RBTreeGet (gSrGlobalInfo.pSrSidRbTree, &SrSidIntf);

    if (pSrSidEntry == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGet Failed\n", __func__,
                __LINE__);
        return SNMP_FAILURE;
    }

    *pi4RetValFsSrSidIndexType = pSrSidEntry->u1SidIndexType;
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFsSrSIDValue
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                retValFsSrSIDValue
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrSIDValue (INT4 i4FsSrSIDAddrType,
                    tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                    UINT4 u4FsSrSIDMask, UINT4 *pu4RetValFsSrSIDValue)
{
    tSrSidInterfaceInfo SrSidIntf;
    tSrSidInterfaceInfo *pSrSidEntry = NULL;

    MEMSET (&SrSidIntf, SR_ZERO, sizeof (tSrSidInterfaceInfo));

    SrSidIntf.ipAddrType = (UINT1) i4FsSrSIDAddrType;
    SrSidIntf.ifIpAddrMask = u4FsSrSIDMask;

    if (SrSidIntf.ipAddrType == SR_IPV4_ADDR_TYPE)
    {
        SrSidIntf.ifIpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
        SR_OCTETSTRING_TO_INTEGER (pFsSrSIDNetIp,
                                   SrSidIntf.ifIpAddr.Addr.u4Addr);
    }
    else if (SrSidIntf.ipAddrType == SR_IPV6_ADDR_TYPE)
    {
        SrSidIntf.ifIpAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
        MEMCPY (SrSidIntf.ifIpAddr.Addr.Ip6Addr.u1_addr,
                pFsSrSIDNetIp->pu1_OctetList, pFsSrSIDNetIp->i4_Length);
    }

    pSrSidEntry = RBTreeGet (gSrGlobalInfo.pSrSidRbTree, &SrSidIntf);

    if (pSrSidEntry == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGet Failed\n", __func__,
                __LINE__);
        return SNMP_FAILURE;
    }

    *pu4RetValFsSrSIDValue = pSrSidEntry->u4PrefixSidLabelIndex;
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFsSrFlags
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                retValFsSrFlags
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrFlags (INT4 i4FsSrSIDAddrType,
                 tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                 UINT4 u4FsSrSIDMask, INT4 *pi4RetValFsSrFlags)
{
    UNUSED_PARAM (i4FsSrSIDAddrType);
    UNUSED_PARAM (pFsSrSIDNetIp);
    UNUSED_PARAM (u4FsSrSIDMask);
    UNUSED_PARAM (pi4RetValFsSrFlags);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFsSrMTID
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                retValFsSrMTID
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrMTID (INT4 i4FsSrSIDAddrType,
                tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                UINT4 u4FsSrSIDMask, INT4 *pi4RetValFsSrMTID)
{
    UNUSED_PARAM (i4FsSrSIDAddrType);
    UNUSED_PARAM (pFsSrSIDNetIp);
    UNUSED_PARAM (u4FsSrSIDMask);
    UNUSED_PARAM (pi4RetValFsSrMTID);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFsSrAlgo
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                retValFsSrAlgo
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrAlgo (INT4 i4FsSrSIDAddrType,
                tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                UINT4 u4FsSrSIDMask, INT4 *pi4RetValFsSrAlgo)
{
    UNUSED_PARAM (i4FsSrSIDAddrType);
    UNUSED_PARAM (pFsSrSIDNetIp);
    UNUSED_PARAM (u4FsSrSIDMask);
    UNUSED_PARAM (pi4RetValFsSrAlgo);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFsSrWeight
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                retValFsSrWeight
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrWeight (INT4 i4FsSrSIDAddrType,
                  tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp, UINT4 u4FsSrSIDMask,
                  INT4 *pi4RetValFsSrWeight)
{
    UNUSED_PARAM (i4FsSrSIDAddrType);
    UNUSED_PARAM (pFsSrSIDNetIp);
    UNUSED_PARAM (u4FsSrSIDMask);
    UNUSED_PARAM (pi4RetValFsSrWeight);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFsSrSIDRowStatus
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                retValFsSrSIDRowStatus
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrSIDRowStatus (INT4 i4FsSrSIDAddrType,
                        tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                        UINT4 u4FsSrSIDMask, INT4 *pi4RetValFsSrSIDRowStatus)
{
    tSrSidInterfaceInfo SrSidIntf;
    tSrSidInterfaceInfo *pSrSidEntry = NULL;

    MEMSET (&SrSidIntf, SR_ZERO, sizeof (tSrSidInterfaceInfo));

    SrSidIntf.ipAddrType = (UINT1) i4FsSrSIDAddrType;
    SrSidIntf.ifIpAddrMask = u4FsSrSIDMask;

    if (SrSidIntf.ipAddrType == SR_IPV4_ADDR_TYPE)
    {
        SrSidIntf.ifIpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
        SR_OCTETSTRING_TO_INTEGER (pFsSrSIDNetIp,
                                   SrSidIntf.ifIpAddr.Addr.u4Addr);
    }
    else if (SrSidIntf.ipAddrType == SR_IPV6_ADDR_TYPE)
    {
        SrSidIntf.ifIpAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
        MEMCPY (SrSidIntf.ifIpAddr.Addr.Ip6Addr.u1_addr,
                pFsSrSIDNetIp->pu1_OctetList, pFsSrSIDNetIp->i4_Length);
    }

    pSrSidEntry = RBTreeGet (gSrGlobalInfo.pSrSidRbTree, &SrSidIntf);

    if (pSrSidEntry == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGet Failed\n", __func__,
                __LINE__);
        return SNMP_FAILURE;
    }

    switch (pSrSidEntry->u1RowStatus)
    {
        case CREATE_AND_WAIT:
            break;

        case ACTIVE:
            break;

        case NOT_IN_SERVICE:
            break;

        case DESTROY:
            break;

        default:
            return SNMP_FAILURE;
    }
    *pi4RetValFsSrSIDRowStatus = pSrSidEntry->u1RowStatus;

    return SNMP_SUCCESS;
}

/* Low Level SET Routine for All Objects  */

/****************************************************************************
 Function    :  nmhSetFsSrSIDType
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                setValFsSrSIDType
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrSIDType (INT4 i4FsSrSIDAddrType,
                   tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                   UINT4 u4FsSrSIDMask, INT4 i4SetValFsSrSIDType)
{
    tSrSidInterfaceInfo SrSidIntf;
    tSrSidInterfaceInfo *pSrSidEntry = NULL;
    UINT4               u4IpAddr = SR_ZERO;
    UINT4               u4AreaId = SR_ZERO;

    MEMSET (&SrSidIntf, SR_ZERO, sizeof (tSrSidInterfaceInfo));

    if (i4FsSrSIDAddrType == SR_IPV4_ADDR_TYPE)
    {
        SrSidIntf.ifIpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
        SR_OCTETSTRING_TO_INTEGER (pFsSrSIDNetIp,
                                   SrSidIntf.ifIpAddr.Addr.u4Addr);
    }
    else if (i4FsSrSIDAddrType == SR_IPV6_ADDR_TYPE)
    {
        SrSidIntf.ifIpAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
        MEMCPY (SrSidIntf.ifIpAddr.Addr.Ip6Addr.u1_addr,
                pFsSrSIDNetIp->pu1_OctetList, pFsSrSIDNetIp->i4_Length);
    }
    else
    {
        return SNMP_FAILURE;
    }

    SrSidIntf.ipAddrType = (UINT1) i4FsSrSIDAddrType;
    SrSidIntf.ifIpAddrMask = u4FsSrSIDMask;

    pSrSidEntry = RBTreeGet (gSrGlobalInfo.pSrSidRbTree, &SrSidIntf);

    if (pSrSidEntry == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGet Failed\n", __func__,
                __LINE__);
        return SNMP_FAILURE;
    }

    if (i4FsSrSIDAddrType == SR_IPV4_ADDR_TYPE)
    {
        MEMCPY (&u4IpAddr, &SrSidIntf.ifIpAddr.Addr.u4Addr,
                 sizeof (UINT4));
        pSrSidEntry->u4SidType = (UINT4) i4SetValFsSrSIDType;
        /* Fetching Area ID based on IP address */
        SrUtilFetchOspfv2AreadID (u4IpAddr, SR_ZERO, &u4AreaId);
        pSrSidEntry->u4AreaId = u4AreaId;
    }
    else if (i4FsSrSIDAddrType == SR_IPV6_ADDR_TYPE)
    {
        pSrSidEntry->u4SidTypeV3 = (UINT4) i4SetValFsSrSIDType;
    }
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhSetFsSrSidIndexType
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                setValFsSrSidIndexType
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrSidIndexType (INT4 i4FsSrSIDAddrType,
                        tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                        UINT4 u4FsSrSIDMask, INT4 i4SetValFsSrSidIndexType)
{
    tSrSidInterfaceInfo SrSidIntf;
    tSrSidInterfaceInfo *pSrSidEntry = NULL;

    MEMSET (&SrSidIntf, SR_ZERO, sizeof (tSrSidInterfaceInfo));

    if (i4FsSrSIDAddrType == SR_IPV4_ADDR_TYPE)
    {
        SrSidIntf.ifIpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
        SR_OCTETSTRING_TO_INTEGER (pFsSrSIDNetIp,
                                   SrSidIntf.ifIpAddr.Addr.u4Addr);
    }
    else if (i4FsSrSIDAddrType == SR_IPV6_ADDR_TYPE)
    {
        SrSidIntf.ifIpAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
        MEMCPY (SrSidIntf.ifIpAddr.Addr.Ip6Addr.u1_addr,
                pFsSrSIDNetIp->pu1_OctetList, pFsSrSIDNetIp->i4_Length);
    }
    else
    {
        return SNMP_FAILURE;
    }

    SrSidIntf.ipAddrType = (UINT1) i4FsSrSIDAddrType;
    SrSidIntf.ifIpAddrMask = u4FsSrSIDMask;

    pSrSidEntry = RBTreeGet (gSrGlobalInfo.pSrSidRbTree, &SrSidIntf);

    if (pSrSidEntry == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGet Failed\n", __func__,
                __LINE__);
        return SNMP_FAILURE;
    }

    pSrSidEntry->u1SidIndexType = (UINT1) i4SetValFsSrSidIndexType;

    if (pSrSidEntry->u1SidIndexType == SR_SID_ADJACENCY)
    {
        pSrSidEntry->u1Flags |= ADJ_SID_V_FLAG | ADJ_SID_L_FLAG;
    }
    else
    {
        pSrSidEntry->u1Flags = PREFIX_SID_NP_FLAG;
    }
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhSetFsSrSIDValue
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                setValFsSrSIDValue
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrSIDValue (INT4 i4FsSrSIDAddrType,
                    tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                    UINT4 u4FsSrSIDMask, UINT4 u4SetValFsSrSIDValue)
{
    tSrSidInterfaceInfo SrSidIntf;
    tSrSidInterfaceInfo *pSrSidEntry = NULL;

    MEMSET (&SrSidIntf, SR_ZERO, sizeof (tSrSidInterfaceInfo));

    if (i4FsSrSIDAddrType == SR_IPV4_ADDR_TYPE)
    {
        SrSidIntf.ifIpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
        SR_OCTETSTRING_TO_INTEGER (pFsSrSIDNetIp,
                                   SrSidIntf.ifIpAddr.Addr.u4Addr);
    }
    else if (i4FsSrSIDAddrType == SR_IPV6_ADDR_TYPE)
    {
        SrSidIntf.ifIpAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
        MEMCPY (SrSidIntf.ifIpAddr.Addr.Ip6Addr.u1_addr,
                pFsSrSIDNetIp->pu1_OctetList, pFsSrSIDNetIp->i4_Length);
    }
    else
    {
        return SNMP_FAILURE;
    }

    SrSidIntf.ipAddrType = (UINT1) i4FsSrSIDAddrType;
    SrSidIntf.ifIpAddrMask = u4FsSrSIDMask;

    pSrSidEntry = RBTreeGet (gSrGlobalInfo.pSrSidRbTree, &SrSidIntf);

    if (pSrSidEntry == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGet Failed\n", __func__,
                __LINE__);
        return SNMP_FAILURE;
    }
    pSrSidEntry->u4PrefixSidLabelIndex = u4SetValFsSrSIDValue;

    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhSetFsSrFlags
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                setValFsSrFlags
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrFlags (INT4 i4FsSrSIDAddrType,
                 tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp, UINT4 u4FsSrSIDMask,
                 INT4 i4SetValFsSrFlags)
{
    UNUSED_PARAM (i4FsSrSIDAddrType);
    UNUSED_PARAM (pFsSrSIDNetIp);
    UNUSED_PARAM (u4FsSrSIDMask);
    UNUSED_PARAM (i4SetValFsSrFlags);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhSetFsSrMTID
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                setValFsSrMTID
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrMTID (INT4 i4FsSrSIDAddrType, tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                UINT4 u4FsSrSIDMask, INT4 i4SetValFsSrMTID)
{
    UNUSED_PARAM (i4FsSrSIDAddrType);
    UNUSED_PARAM (pFsSrSIDNetIp);
    UNUSED_PARAM (u4FsSrSIDMask);
    UNUSED_PARAM (i4SetValFsSrMTID);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhSetFsSrAlgo
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                setValFsSrAlgo
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrAlgo (INT4 i4FsSrSIDAddrType, tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                UINT4 u4FsSrSIDMask, INT4 i4SetValFsSrAlgo)
{
    UNUSED_PARAM (i4FsSrSIDAddrType);
    UNUSED_PARAM (pFsSrSIDNetIp);
    UNUSED_PARAM (u4FsSrSIDMask);
    UNUSED_PARAM (i4SetValFsSrAlgo);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhSetFsSrWeight
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                setValFsSrWeight
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrWeight (INT4 i4FsSrSIDAddrType,
                  tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                  UINT4 u4FsSrSIDMask, INT4 i4SetValFsSrWeight)
{
    UNUSED_PARAM (i4FsSrSIDAddrType);
    UNUSED_PARAM (pFsSrSIDNetIp);
    UNUSED_PARAM (u4FsSrSIDMask);
    UNUSED_PARAM (i4SetValFsSrWeight);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhSetFsSrSIDRowStatus
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                setValFsSrSIDRowStatus
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrSIDRowStatus (INT4 i4FsSrSIDAddrType,
                        tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                        UINT4 u4FsSrSIDMask, INT4 i4SetValFsSrSIDRowStatus)
{
    UINT1               u1OperStatus = SR_ZERO;
    UINT1               u1LSAType = TYPE10_OPQ_LSA;
    INT4                i4SrStatus = SR_ZERO;

    tSrSidInterfaceInfo SrSidIntf;
    tSrSidInterfaceInfo *pSrSidEntry = NULL;

    nmhGetFsSrV4Status (&i4SrStatus);
    MEMSET (&SrSidIntf, SR_ZERO, sizeof (tSrSidInterfaceInfo));

    if (i4FsSrSIDAddrType == SR_IPV4_ADDR_TYPE)
    {
        SrSidIntf.ifIpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
        SR_OCTETSTRING_TO_INTEGER (pFsSrSIDNetIp,
                                   SrSidIntf.ifIpAddr.Addr.u4Addr);
    }
    else if (i4FsSrSIDAddrType == SR_IPV6_ADDR_TYPE)
    {
        SrSidIntf.ifIpAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
        MEMCPY (SrSidIntf.ifIpAddr.Addr.Ip6Addr.u1_addr,
                pFsSrSIDNetIp->pu1_OctetList, pFsSrSIDNetIp->i4_Length);
    }
    else
    {
        CLI_SET_ERR (CLI_SR_INV_IPADDR_TYPE);
        return SNMP_FAILURE;
    }

    SrSidIntf.ipAddrType = (UINT1) i4FsSrSIDAddrType;
    pSrSidEntry = RBTreeGet (gSrGlobalInfo.pSrSidRbTree, &SrSidIntf);

    switch (i4SetValFsSrSIDRowStatus)
    {
        case CREATE_AND_WAIT:
            SrSidIntf.ipAddrType = (UINT1) i4FsSrSIDAddrType;
            SrSidIntf.ifIpAddr.u2AddrType = (UINT2) i4FsSrSIDAddrType;
            SrSidIntf.ifIpAddrMask = u4FsSrSIDMask;

            if (pSrSidEntry == NULL)
            {
                pSrSidEntry = SrCreateSidInterface (&(SrSidIntf.ifIpAddr));
                if (pSrSidEntry == NULL)
                {
                    SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d SrCreateSidInterface is NULL\n",
                            __func__,__LINE__);
                    return SNMP_FAILURE;
                }

                pSrSidEntry->u1RowStatus = (UINT1) i4SetValFsSrSIDRowStatus;
                pSrSidEntry->ipAddrType = SrSidIntf.ipAddrType;
                pSrSidEntry->u1AdminStatus = SR_ADMIN_DOWN;

                pSrSidEntry->ifIpAddrMask = SrSidIntf.ifIpAddrMask;

                if (SrSidEntryAddToRBtree (pSrSidEntry) == SR_FAILURE)
                {
                    SR_INTF_FREE (pSrSidEntry);
                    return SNMP_FAILURE;
                }
            }
            else
            {
                return SNMP_FAILURE;
            }
            break;

        case ACTIVE:
            if (pSrSidEntry != NULL)
            {
                if (pSrSidEntry->u1RowStatus == CREATE_AND_WAIT)
                {
                    gSrGlobalInfo.u4CliCmdMode = SR_TRUE;
                    if (SR_FAILURE ==
                        SrCliProcessAddSid (pSrSidEntry,
                                            &(pSrSidEntry->ifIpAddr),
                                            u4FsSrSIDMask))
                    {
                        SrDelSid (pSrSidEntry);
                        gSrGlobalInfo.u4CliCmdMode = SR_FALSE;
                        return SNMP_FAILURE;
                    }
                    gSrGlobalInfo.u4CliCmdMode = SR_FALSE;

                    pSrSidEntry->u1OperStatus = u1OperStatus;
                    pSrSidEntry->u1AdminStatus = SR_ADMIN_UP;
                    pSrSidEntry->u1RowStatus = (UINT1) i4SetValFsSrSIDRowStatus;
                    if ((pSrSidEntry->u4SidType == SR_SID_NODE) ||
                        (pSrSidEntry->u4SidTypeV3 == SR_SID_NODE))
                    {
                        if (i4FsSrSIDAddrType == SR_IPV4_ADDR_TYPE)
                        {
                            if ((i4SrStatus == SR_ENABLED) && (SR_MODE & SR_CONF_OSPF) == SR_CONF_OSPF)
                            {
                                if(pSrSidEntry->u4SidType == SR_SID_NODE)
                                {
                                   if (SR_FAILURE == SrUtilSendLSAForSid (NEW_LSA, OSPF_EXT_PRREFIX_TLV_TYPE,
                                                                       u1LSAType, pSrSidEntry))
                                   {
                                       SR_TRC2 (SR_MAIN_TRC, "%s %d Failure in sending LSA\r\n",
                                            __func__, __LINE__);
                                       return SR_FAILURE;
                                   }
                                }
                                if(pSrSidEntry->u4SidType == SR_SID_ADJACENCY)
                                {
                                   if (SR_FAILURE == SrUtilSendLSAForSid (NEW_LSA, OSPF_EXT_LINK_LSA_OPAQUE_TYPE,
                                                                       u1LSAType, pSrSidEntry))
                                   {
                                       SR_TRC2 (SR_MAIN_TRC, "%s %d Failure in sending LSA\r\n",
                                            __func__, __LINE__);
                                       return SR_FAILURE;
                                   }
                                }
                             }
                        }
                        else if (i4FsSrSIDAddrType == SR_IPV6_ADDR_TYPE)
                        {
                            if (SR_SID_ABS_INDEX == pSrSidEntry->u1SidIndexType)
                            {
                                SR_IPV6_NODE_SID =
                                    pSrSidEntry->u4PrefixSidLabelIndex -
                                    gSrGlobalInfo.SrContextV3.SrGbRange.
                                    u4SrGbV3MinIndex;
                            }
                            else
                            {
                                SR_IPV6_NODE_SID =
                                    pSrSidEntry->u4PrefixSidLabelIndex;
                            }

                            /* Initialize the SelfNode pointer in Global Structure for V3 */
                            if (gSrGlobalInfo.pV6SrSelfNodeInfo == NULL)
                            {
                                gSrGlobalInfo.pV6SrSelfNodeInfo = pSrSidEntry;
                            }
                        }
                    }
                }
                else
                {
                    return SNMP_FAILURE;
                }
                break;
            }
            else
            {
                return SNMP_FAILURE;
            }

        case NOT_IN_SERVICE:
            break;

        case DESTROY:
            if (SNMP_FAILURE == SrDelSid (pSrSidEntry))
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d SrDelSid Failed\n", __func__,
                        __LINE__);
                return SNMP_FAILURE;
            }
            break;

        default:
            return SNMP_FAILURE;
    }

    return SNMP_SUCCESS;
}

/* Low Level TEST Routines for All Objects  */

/****************************************************************************
 Function    :  nmhTestv2FsSrSIDType
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                testValFsSrSIDType
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrSIDType (UINT4 *pu4ErrorCode, INT4 i4FsSrSIDAddrType,
                      tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                      UINT4 u4FsSrSIDMask, INT4 i4TestValFsSrSIDType)
{
    INT4                i4SrStatus = SR_DISABLED;

    UNUSED_PARAM (pFsSrSIDNetIp);
    UNUSED_PARAM (u4FsSrSIDMask);

    if (i4FsSrSIDAddrType == SR_IPV4_ADDR_TYPE)
    {
        nmhGetFsSrV4Status (&i4SrStatus);
    }
    else if (i4FsSrSIDAddrType == SR_IPV6_ADDR_TYPE)
    {
        nmhGetFsSrV6Status (&i4SrStatus);
    }

    if (i4FsSrSIDAddrType == SR_IPV4_ADDR_TYPE)
    {
        if ((i4TestValFsSrSIDType == SR_SID_ADJACENCY) ||
            (i4TestValFsSrSIDType == SR_SID_NODE))
        {
            return SNMP_SUCCESS;
        }
    }
    else if (i4FsSrSIDAddrType == SR_IPV6_ADDR_TYPE)
    {
        if ((i4TestValFsSrSIDType == SR_SID_ADJACENCY)
            || (i4TestValFsSrSIDType == SR_SID_NODE))
        {
            if (SR_ZERO != SR_IPV6_NODE_SID
                && (i4TestValFsSrSIDType == SR_SID_NODE))
            {
                *pu4ErrorCode = SNMP_ERR_NO_CREATION;
                CLI_SET_ERR (CLI_SR_NODE_SID_EXIST);
                return SNMP_FAILURE;
            }
            return SNMP_SUCCESS;
        }
    }

    *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
    CLI_SET_ERR (CLI_SR_INV_SID_TYPE);
    return SNMP_FAILURE;
}

/****************************************************************************
 Function    :  nmhTestv2FsSrSidIndexType
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                testValFsSrSidIndexType
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrSidIndexType (UINT4 *pu4ErrorCode, INT4 i4FsSrSIDAddrType,
                           tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                           UINT4 u4FsSrSIDMask, INT4 i4TestValFsSrSidIndexType)
{
    INT4                i4SrStatus = SR_DISABLED;

    UNUSED_PARAM (pFsSrSIDNetIp);
    UNUSED_PARAM (u4FsSrSIDMask);

    if (i4FsSrSIDAddrType == SR_IPV4_ADDR_TYPE)
    {
        nmhGetFsSrV4Status (&i4SrStatus);
    }
    else if (i4FsSrSIDAddrType == SR_IPV6_ADDR_TYPE)
    {
        nmhGetFsSrV6Status (&i4SrStatus);
    }

    switch (i4TestValFsSrSidIndexType)
    {
        case SR_SID_ABS_INDEX:
        case SR_SID_REL_INDEX:
            break;

        default:
            *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
            CLI_SET_ERR (CLI_SR_INV_SID_INDEX_TYPE);
            return SNMP_FAILURE;
    }
    return SNMP_SUCCESS;

}

/****************************************************************************
 Function    :  nmhTestv2FsSrSIDValue
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                testValFsSrSIDValue
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrSIDValue (UINT4 *pu4ErrorCode, INT4 i4FsSrSIDAddrType,
                       tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                       UINT4 u4FsSrSIDMask, UINT4 u4TestValFsSrSIDValue)
{
    INT4                i4SrStatus = SR_DISABLED;
    tSrSidInterfaceInfo *pSrSidEntry = NULL;

    UNUSED_PARAM (pFsSrSIDNetIp);
    UNUSED_PARAM (u4FsSrSIDMask);

    if (i4FsSrSIDAddrType == SR_IPV4_ADDR_TYPE)
    {
        nmhGetFsSrV4Status (&i4SrStatus);
    }
    else if (i4FsSrSIDAddrType == SR_IPV6_ADDR_TYPE)
    {
        nmhGetFsSrV6Status (&i4SrStatus);
    }

    pSrSidEntry = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);

    while (1)
    {
        if (pSrSidEntry == NULL)
        {
            break;
        }

        if ((u4TestValFsSrSIDValue == pSrSidEntry->u4PrefixSidLabelIndex) &&
            (i4FsSrSIDAddrType == pSrSidEntry->ipAddrType))
        {
            *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
            CLI_SET_ERR (CLI_SR_SID_EXISTS);
            return SNMP_FAILURE;
        }
        else if ((i4FsSrSIDAddrType != pSrSidEntry->ipAddrType) &&
                 (u4TestValFsSrSIDValue == pSrSidEntry->u4PrefixSidLabelIndex))
        {
            *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
            CLI_SET_ERR (CLI_SR_SID_EXISTS);
            return SNMP_FAILURE;
        }

        pSrSidEntry =
            RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree, pSrSidEntry, NULL);
    }

    if (i4FsSrSIDAddrType == SR_IPV4_ADDR_TYPE)
    {
        if (IS_V4_SRGB_CONFIGURED == SR_FALSE)
        {
            nmhSetFsSrV4MinSrGbValue (SRGB_DEFAULT_MIN_VALUE);
            nmhSetFsSrV4MaxSrGbValue (SRGB_DEFAULT_MAX_VALUE);
        }

        if ((u4TestValFsSrSIDValue >= SR_MIN_SID_INDEX) &&
            (u4TestValFsSrSIDValue <= SR_MAX_SID_INDEX))
        {
            if (u4TestValFsSrSIDValue + SR_V4_SRGB_MIN_VAL > SR_V4_SRGB_MAX_VAL)
            {
                *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
                CLI_SET_ERR (CLI_SR_INV_VAL);
                return SNMP_FAILURE;
            }
        }
    }
    else if (i4FsSrSIDAddrType == SR_IPV6_ADDR_TYPE)
    {
        if (IS_V6_SRGB_CONFIGURED == SR_FALSE)
        {
            nmhSetFsSrV6MinSrGbValue (SRGB_DEFAULT_MIN_VALUE);
            nmhSetFsSrV6MaxSrGbValue (SRGB_DEFAULT_MAX_VALUE);
        }

        if ((u4TestValFsSrSIDValue >= SR_MIN_SID_INDEX) &&
            (u4TestValFsSrSIDValue <= SR_MAX_SID_INDEX))
        {
            if (u4TestValFsSrSIDValue + SR_V6_SRGB_MIN_VAL > SR_V6_SRGB_MAX_VAL)
            {
                *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
                CLI_SET_ERR (CLI_SR_INV_VAL);
                return SNMP_FAILURE;
            }
        }
    }

    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhTestv2FsSrFlags
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                testValFsSrFlags
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrFlags (UINT4 *pu4ErrorCode, INT4 i4FsSrSIDAddrType,
                    tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                    UINT4 u4FsSrSIDMask, INT4 i4TestValFsSrFlags)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (i4FsSrSIDAddrType);
    UNUSED_PARAM (pFsSrSIDNetIp);
    UNUSED_PARAM (u4FsSrSIDMask);
    UNUSED_PARAM (i4TestValFsSrFlags);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhTestv2FsSrMTID
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                testValFsSrMTID
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrMTID (UINT4 *pu4ErrorCode, INT4 i4FsSrSIDAddrType,
                   tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                   UINT4 u4FsSrSIDMask, INT4 i4TestValFsSrMTID)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (i4FsSrSIDAddrType);
    UNUSED_PARAM (pFsSrSIDNetIp);
    UNUSED_PARAM (u4FsSrSIDMask);
    UNUSED_PARAM (i4TestValFsSrMTID);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhTestv2FsSrAlgo
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                testValFsSrAlgo
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrAlgo (UINT4 *pu4ErrorCode, INT4 i4FsSrSIDAddrType,
                   tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                   UINT4 u4FsSrSIDMask, INT4 i4TestValFsSrAlgo)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (i4FsSrSIDAddrType);
    UNUSED_PARAM (pFsSrSIDNetIp);
    UNUSED_PARAM (u4FsSrSIDMask);
    UNUSED_PARAM (i4TestValFsSrAlgo);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhTestv2FsSrWeight
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                testValFsSrWeight
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrWeight (UINT4 *pu4ErrorCode, INT4 i4FsSrSIDAddrType,
                     tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                     UINT4 u4FsSrSIDMask, INT4 i4TestValFsSrWeight)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (i4FsSrSIDAddrType);
    UNUSED_PARAM (pFsSrSIDNetIp);
    UNUSED_PARAM (u4FsSrSIDMask);
    UNUSED_PARAM (i4TestValFsSrWeight);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhTestv2FsSrSIDRowStatus
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask

                The Object 
                testValFsSrSIDRowStatus
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrSIDRowStatus (UINT4 *pu4ErrorCode, INT4 i4FsSrSIDAddrType,
                           tSNMP_OCTET_STRING_TYPE * pFsSrSIDNetIp,
                           UINT4 u4FsSrSIDMask, INT4 i4TestValFsSrSIDRowStatus)
{
    INT4                i4SrStatus = SR_DISABLED;
    tSrSidInterfaceInfo SrSidIntf;
    tSrSidInterfaceInfo *pSrSidEntry = NULL;
    UINT4               u4Count = 0;

    UNUSED_PARAM (u4FsSrSIDMask);

    MEMSET(&SrSidIntf, SR_ZERO, sizeof(tSrSidInterfaceInfo));

    if (i4FsSrSIDAddrType == SR_IPV4_ADDR_TYPE)
    {
        SrSidIntf.ifIpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
        SR_OCTETSTRING_TO_INTEGER(pFsSrSIDNetIp, SrSidIntf.ifIpAddr.Addr.u4Addr);

        nmhGetFsSrV4Status (&i4SrStatus);
    }
    else if (i4FsSrSIDAddrType == SR_IPV6_ADDR_TYPE)
    {
        SrSidIntf.ifIpAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
        MEMCPY (SrSidIntf.ifIpAddr.Addr.Ip6Addr.u1_addr,
                pFsSrSIDNetIp->pu1_OctetList,
                pFsSrSIDNetIp->i4_Length);

        nmhGetFsSrV6Status (&i4SrStatus);
    }

    SrSidIntf.ipAddrType = (UINT1) i4FsSrSIDAddrType;
    pSrSidEntry = RBTreeGet (gSrGlobalInfo.pSrSidRbTree, &SrSidIntf);

    switch (i4TestValFsSrSIDRowStatus)
    {
        case CREATE_AND_WAIT:
            if (pSrSidEntry != NULL)
            {
                if (pSrSidEntry->u4SidType == SR_SID_NODE)
                {
                    if (pSrSidEntry->ifIpAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
                    {
                       if (SrIsLoopbackUsed (pSrSidEntry->ifIpAddr.Addr.u4Addr)
                              == SNMP_FAILURE)
                       {
                           CLI_SET_ERR (CLI_SR_USED_NODE_SID_IP_ADDR);
                           return SNMP_FAILURE;
                       }
                    }
                }
            }
            break;

        case ACTIVE:
            if (pSrSidEntry != NULL)
            {
                if (pSrSidEntry->u4SidType == SR_SID_NODE)
                {
                    if (pSrSidEntry->ifIpAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
                    {
                       if (NetIpv4IsLoopbackAddress (
                              pSrSidEntry->ifIpAddr.Addr.u4Addr) != NETIPV4_SUCCESS)
                       {
                           CLI_SET_ERR (CLI_SR_INV_NODE_SID_IP_ADDR);
                           return SNMP_FAILURE;
                       }

                       if (UtilOspfIsLoopbackAddressInCxt (
                              OSPF_DEFAULT_CXT_ID, pSrSidEntry->ifIpAddr.Addr.u4Addr) == OSPF_FAILURE)
                       {
                           CLI_SET_ERR (CLI_SR_IP_ADDR_NOT_IN_OSPF);
                           return SNMP_FAILURE;
                       }
                    }
                }
            }
            break;

        case NOT_IN_SERVICE:
            break;

        case DESTROY:
            if (pSrSidEntry == NULL)
            {
                *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
                CLI_SET_ERR (CLI_SR_SID_DEL_NOT_EXITS);
                return SNMP_FAILURE;
            }
            break;

        default:
            *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
            CLI_SET_ERR (CLI_SR_INV_VAL);
            return SNMP_FAILURE;
    }
    return SNMP_SUCCESS;
}

/* Low Level Dependency Routines for All Objects  */

/****************************************************************************
 Function    :  nmhDepv2FsSrSIDTable
 Input       :  The Indices
                FsSrSIDAddrType
                FsSrSIDNetIp
                FsSrSIDMask
 Output      :  The Dependency Low Lev Routine Take the Indices &
                check whether dependency is met or not.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhDepv2FsSrSIDTable (UINT4 *pu4ErrorCode, tSnmpIndexList * pSnmpIndexList,
                      tSNMP_VAR_BIND * pSnmpVarBind)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (pSnmpIndexList);
    UNUSED_PARAM (pSnmpVarBind);
    return SNMP_SUCCESS;
}

/* Low Level GET Routine for All Objects  */

/****************************************************************************
 Function    :  nmhGetFsSrDebugFlag
 Input       :  The Indices

                The Object 
                retValFsSrDebugFlag
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrDebugFlag (UINT4 *pu4RetValFsSrDebugFlag)
{
    *pu4RetValFsSrDebugFlag = SR_TRC_FLAG;
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFsRouterListIndexNext
 Input       :  The Indices

                The Object 
                retValFsRouterListIndexNext
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsRouterListIndexNext (UINT4 *pu4RetValFsRouterListIndexNext)
{
    UINT4               u4Index = 0;

    u4Index = SrOptRtrListGetIndex ();
    if (u4Index == SR_ZERO)
    {
        SR_TRC2 (SR_MGMT_TRC, "%s:%d TE-List Index Table might be full\n",
                 __func__, __LINE__);
        return SNMP_SUCCESS;
    }
    *pu4RetValFsRouterListIndexNext = u4Index;
    return SNMP_SUCCESS;
}

/* Low Level SET Routine for All Objects  */

/****************************************************************************
 Function    :  nmhSetFsSrDebugFlag
 Input       :  The Indices

                The Object 
                setValFsSrDebugFlag
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrDebugFlag (UINT4 u4SetValFsSrDebugFlag)
{
    SR_TRC_FLAG |= u4SetValFsSrDebugFlag;
    return SNMP_SUCCESS;
}

/* Low Level TEST Routines for All Objects  */

/****************************************************************************
 Function    :  nmhTestv2FsSrDebugFlag
 Input       :  The Indices

                The Object 
                testValFsSrDebugFlag
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrDebugFlag (UINT4 *pu4ErrorCode, UINT4 u4TestValFsSrDebugFlag)
{
    if ((u4TestValFsSrDebugFlag < SR_DBG_MIN_TRC) ||
            (u4TestValFsSrDebugFlag > SR_DBG_MAX_TRC ))
    {
        *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
        return SNMP_FAILURE;
    }

    return SNMP_SUCCESS;
}

/* Low Level Dependency Routines for All Objects  */

/****************************************************************************
 Function    :  nmhDepv2FsSrDebugFlag
 Output      :  The Dependency Low Lev Routine Take the Indices &
                check whether dependency is met or not.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhDepv2FsSrDebugFlag (UINT4 *pu4ErrorCode, tSnmpIndexList * pSnmpIndexList,
                       tSNMP_VAR_BIND * pSnmpVarBind)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (pSnmpIndexList);
    UNUSED_PARAM (pSnmpVarBind);
    return SNMP_SUCCESS;
}

/* LOW LEVEL Routines for Table : FsSrTePathTable. */

/****************************************************************************
 Function    :  nmhValidateIndexInstanceFsSrTePathTable
 Input       :  The Indices
                FsSrTeDestAddr
                FsSrTeDestMask
                FsSrTeRouterId
                FsSrTeDestAddrType
 Output      :  The Routines Validates the Given Indices.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
/* GET_EXACT Validate Index Instance Routine. */

INT1
nmhValidateIndexInstanceFsSrTePathTable (tSNMP_OCTET_STRING_TYPE *
                                         pFsSrTeDestAddr,
                                         UINT4 u4FsSrTeDestMask,
                                         tSNMP_OCTET_STRING_TYPE *
                                         pFsSrTeRouterId,
                                         INT4 i4FsSrTeDestAddrType)
{
    UNUSED_PARAM (pFsSrTeDestAddr);
    UNUSED_PARAM (u4FsSrTeDestMask);
    UNUSED_PARAM (pFsSrTeRouterId);
    UNUSED_PARAM (i4FsSrTeDestAddrType);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFirstIndexFsSrTePathTable
 Input       :  The Indices
                FsSrTeDestAddr
                FsSrTeDestMask
                FsSrTeRouterId
                FsSrTeDestAddrType
 Output      :  The Get First Routines gets the Lexicographicaly
                First Entry from the Table.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
/* GET_FIRST Routine. */

INT1
nmhGetFirstIndexFsSrTePathTable (tSNMP_OCTET_STRING_TYPE * pFsSrTeDestAddr,
                                 UINT4 *pu4FsSrTeDestMask,
                                 tSNMP_OCTET_STRING_TYPE * pFsSrTeRouterId,
                                 INT4 *pi4FsSrTeDestAddrType)
{
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;

    if (gSrGlobalInfo.pSrTeRouteRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrTeRouteRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    if ((pSrTeRtInfo = RBTreeGetFirst (gSrGlobalInfo.pSrTeRouteRbTree)) == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGetFirst FAILED\n", __func__,
                 __LINE__);
        return SNMP_FAILURE;
    }

    if (pSrTeRtInfo->destAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        SR_INTEGER_TO_OCTETSTRING (pSrTeRtInfo->destAddr.Addr.u4Addr,
                                   pFsSrTeDestAddr);
        SR_INTEGER_TO_OCTETSTRING (pSrTeRtInfo->mandRtrId.Addr.u4Addr,
                                   pFsSrTeRouterId);
    }
    else if (pSrTeRtInfo->destAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        MEMCPY (pFsSrTeDestAddr->pu1_OctetList,
                pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                SR_IPV6_ADDR_LENGTH);
        MEMCPY (pFsSrTeRouterId->pu1_OctetList,
                pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                SR_IPV6_ADDR_LENGTH);
        pFsSrTeDestAddr->i4_Length = SR_IPV6_ADDR_LENGTH;
        pFsSrTeRouterId->i4_Length = SR_IPV6_ADDR_LENGTH;
    }

    *pu4FsSrTeDestMask = pSrTeRtInfo->u4DestMask;
    *pi4FsSrTeDestAddrType = pSrTeRtInfo->destAddr.u2AddrType;
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetNextIndexFsSrTePathTable
 Input       :  The Indices
                FsSrTeDestAddr
                nextFsSrTeDestAddr
                FsSrTeDestMask
                nextFsSrTeDestMask
                FsSrTeRouterId
                nextFsSrTeRouterId
                FsSrTeDestAddrType
                nextFsSrTeDestAddrType
 Output      :  The Get Next function gets the Next Index for
                the Index Value given in the Index Values. The
                Indices are stored in the next_varname variables.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
/* GET_NEXT Routine.  */
INT1
nmhGetNextIndexFsSrTePathTable (tSNMP_OCTET_STRING_TYPE * pFsSrTeDestAddr,
                                tSNMP_OCTET_STRING_TYPE * pNextFsSrTeDestAddr,
                                UINT4 u4FsSrTeDestMask,
                                UINT4 *pu4NextFsSrTeDestMask,
                                tSNMP_OCTET_STRING_TYPE * pFsSrTeRouterId,
                                tSNMP_OCTET_STRING_TYPE * pNextFsSrTeRouterId,
                                INT4 i4FsSrTeDestAddrType,
                                INT4 *pi4NextFsSrTeDestAddrType)
{
    tSrTeRtEntryInfo    SrTeRtInfo;
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;
    tGenU4Addr          dummyAddr;

    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));

    if (gSrGlobalInfo.pSrTeRouteRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrTeRouteRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    if (i4FsSrTeDestAddrType == SR_IPV4_ADDR_TYPE)
    {
        SR_OCTETSTRING_TO_INTEGER (pFsSrTeDestAddr,
                                   SrTeRtInfo.destAddr.Addr.u4Addr);
        SR_OCTETSTRING_TO_INTEGER (pFsSrTeRouterId,
                                   SrTeRtInfo.mandRtrId.Addr.u4Addr);

        if ((SrTeRtInfo.destAddr.Addr.u4Addr == SR_ZERO) ||
            (SrTeRtInfo.mandRtrId.Addr.u4Addr == SR_ZERO))
        {
            SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC,
                     "%s:%d Either destination or mandatory router is 0.\n",
                     __func__, __LINE__);
            return SNMP_FAILURE;
        }
    }
    else if (i4FsSrTeDestAddrType == SR_IPV6_ADDR_TYPE)
    {
        MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                pFsSrTeDestAddr->pu1_OctetList, pFsSrTeDestAddr->i4_Length);

        MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                pFsSrTeRouterId->pu1_OctetList, pFsSrTeRouterId->i4_Length);

        if ((MEMCMP (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                     dummyAddr.Addr.Ip6Addr.u1_addr,
                     SR_IPV6_ADDR_LENGTH) == SR_ZERO)
            ||
            (MEMCMP
             (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
              dummyAddr.Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH) == SR_ZERO))
        {
            SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC,
                     "%s:%d Either destination or mandatory router is 0.\n",
                     __func__, __LINE__);
            return SNMP_FAILURE;
        }

    }

    SrTeRtInfo.u4DestMask = u4FsSrTeDestMask;
    SrTeRtInfo.destAddr.u2AddrType = (UINT2) i4FsSrTeDestAddrType;
    SrTeRtInfo.mandRtrId.u2AddrType = (UINT2) i4FsSrTeDestAddrType;

    pSrTeRtInfo =
        RBTreeGetNext (gSrGlobalInfo.pSrTeRouteRbTree, &SrTeRtInfo, NULL);
    if (pSrTeRtInfo == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGetNext Failed\n", __func__,
                __LINE__);
        return SNMP_FAILURE;
    }
    if (pSrTeRtInfo->destAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        SR_INTEGER_TO_OCTETSTRING (pSrTeRtInfo->destAddr.Addr.u4Addr,
                                   pNextFsSrTeDestAddr);
        SR_INTEGER_TO_OCTETSTRING (pSrTeRtInfo->mandRtrId.Addr.u4Addr,
                                   pNextFsSrTeRouterId);
    }
    else if (pSrTeRtInfo->destAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        MEMCPY (pNextFsSrTeDestAddr->pu1_OctetList,
                pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                SR_IPV6_ADDR_LENGTH);
        MEMCPY (pNextFsSrTeRouterId->pu1_OctetList,
                pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                SR_IPV6_ADDR_LENGTH);
        pNextFsSrTeDestAddr->i4_Length = SR_IPV6_ADDR_LENGTH;
        pNextFsSrTeRouterId->i4_Length = SR_IPV6_ADDR_LENGTH;
    }

    *pu4NextFsSrTeDestMask = pSrTeRtInfo->u4DestMask;
    *pi4NextFsSrTeDestAddrType = pSrTeRtInfo->destAddr.u2AddrType;
    return SNMP_SUCCESS;
}

/* Low Level GET Routine for All Objects  */

/****************************************************************************
 Function    :  nmhGetFsSrTeRouterListIndex
 Input       :  The Indices
                FsSrTeDestAddr
                FsSrTeDestMask
                FsSrTeRouterId
                FsSrTeDestAddrType

                The Object 
                retValFsSrTeRouterListIndex
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrTeRouterListIndex (tSNMP_OCTET_STRING_TYPE * pFsSrTeDestAddr,
                             UINT4 u4FsSrTeDestMask,
                             tSNMP_OCTET_STRING_TYPE * pFsSrTeRouterId,
                             INT4 i4FsSrTeDestAddrType,
                             UINT4 *pu4RetValFsSrTeRouterListIndex)
{
    tSrTeRtEntryInfo    SrTeRtInfo;
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;

    UNUSED_PARAM (pu4RetValFsSrTeRouterListIndex);
    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));

    if (gSrGlobalInfo.pSrTeRouteRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrTeRouteRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    if (i4FsSrTeDestAddrType == SR_IPV4_ADDR_TYPE)
    {
        SR_OCTETSTRING_TO_INTEGER (pFsSrTeDestAddr,
                                   SrTeRtInfo.destAddr.Addr.u4Addr);
        SR_OCTETSTRING_TO_INTEGER (pFsSrTeRouterId,
                                   SrTeRtInfo.mandRtrId.Addr.u4Addr);
    }
    else if (i4FsSrTeDestAddrType == SR_IPV6_ADDR_TYPE)
    {
        MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                pFsSrTeDestAddr->pu1_OctetList, pFsSrTeDestAddr->i4_Length);

        MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                pFsSrTeRouterId->pu1_OctetList, pFsSrTeRouterId->i4_Length);
    }
    SrTeRtInfo.u4DestMask = u4FsSrTeDestMask;
    SrTeRtInfo.destAddr.u2AddrType = (UINT2) i4FsSrTeDestAddrType;
    SrTeRtInfo.mandRtrId.u2AddrType = (UINT2) i4FsSrTeDestAddrType;

    SR_TRC5 (SR_MGMT_TRC,
             "%s:%d Calling RBTreeGet: DestAddr %x Mask %x RtrId %x\n",
             __func__, __LINE__, SrTeRtInfo.destAddr.Addr.u4Addr,
             SrTeRtInfo.u4DestMask, SrTeRtInfo.mandRtrId.Addr.u4Addr);

    pSrTeRtInfo = RBTreeGet (gSrGlobalInfo.pSrTeRouteRbTree, &SrTeRtInfo);
    if (pSrTeRtInfo == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGet Failed\n", __func__,
                __LINE__);
        return SNMP_FAILURE;
    }

    *pu4RetValFsSrTeRouterListIndex = pSrTeRtInfo->srTeRtrListIndex.u4Index;
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFsSrTePathRowStatus
 Input       :  The Indices
                FsSrTeDestAddr
                FsSrTeDestMask
                FsSrTeRouterId
                FsSrTeDestAddrType

                The Object 
                retValFsSrTePathRowStatus
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrTePathRowStatus (tSNMP_OCTET_STRING_TYPE * pFsSrTeDestAddr,
                           UINT4 u4FsSrTeDestMask,
                           tSNMP_OCTET_STRING_TYPE * pFsSrTeRouterId,
                           INT4 i4FsSrTeDestAddrType,
                           INT4 *pi4RetValFsSrTePathRowStatus)
{
    tSrTeRtEntryInfo    SrTeRtInfo;
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;

    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));

    if (i4FsSrTeDestAddrType == SR_IPV4_ADDR_TYPE)
    {
        SR_OCTETSTRING_TO_INTEGER (pFsSrTeDestAddr,
                                   SrTeRtInfo.destAddr.Addr.u4Addr);
        SR_OCTETSTRING_TO_INTEGER (pFsSrTeRouterId,
                                   SrTeRtInfo.mandRtrId.Addr.u4Addr);
    }
    else if (i4FsSrTeDestAddrType == SR_IPV6_ADDR_TYPE)
    {
        MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                pFsSrTeDestAddr->pu1_OctetList, pFsSrTeDestAddr->i4_Length);

        MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                pFsSrTeRouterId->pu1_OctetList, pFsSrTeRouterId->i4_Length);
    }

    SrTeRtInfo.u4DestMask = u4FsSrTeDestMask;
    SrTeRtInfo.destAddr.u2AddrType = (UINT2) i4FsSrTeDestAddrType;
    SrTeRtInfo.mandRtrId.u2AddrType = (UINT2) i4FsSrTeDestAddrType;

    pSrTeRtInfo = RBTreeGet (gSrGlobalInfo.pSrTeRouteRbTree, &SrTeRtInfo);
    if (pSrTeRtInfo == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGet Failed\n", __func__,
                __LINE__);
        return SNMP_FAILURE;
    }

    *pi4RetValFsSrTePathRowStatus = pSrTeRtInfo->u1RowStatus;
    return SNMP_SUCCESS;
}

/* Low Level SET Routine for All Objects  */

/****************************************************************************
 Function    :  nmhSetFsSrTeRouterListIndex
 Input       :  The Indices
                FsSrTeDestAddr
                FsSrTeDestMask
                FsSrTeRouterId
                FsSrTeDestAddrType

                The Object 
                setValFsSrTeRouterListIndex
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrTeRouterListIndex (tSNMP_OCTET_STRING_TYPE * pFsSrTeDestAddr,
                             UINT4 u4FsSrTeDestMask,
                             tSNMP_OCTET_STRING_TYPE * pFsSrTeRouterId,
                             INT4 i4FsSrTeDestAddrType,
                             UINT4 u4SetValFsSrTeRouterListIndex)
{
    tSrTeRtEntryInfo    SrTeRtInfo;
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;

    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));

    if (gSrGlobalInfo.pSrTeRouteRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrTeRouteRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    if (i4FsSrTeDestAddrType == SR_IPV4_ADDR_TYPE)
    {
        SR_OCTETSTRING_TO_INTEGER (pFsSrTeDestAddr,
                                   SrTeRtInfo.destAddr.Addr.u4Addr);
        SR_OCTETSTRING_TO_INTEGER (pFsSrTeRouterId,
                                   SrTeRtInfo.mandRtrId.Addr.u4Addr);
    }
    else if (i4FsSrTeDestAddrType == SR_IPV6_ADDR_TYPE)
    {
        MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                pFsSrTeDestAddr->pu1_OctetList, pFsSrTeDestAddr->i4_Length);

        MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                pFsSrTeRouterId->pu1_OctetList, pFsSrTeRouterId->i4_Length);
    }

    SrTeRtInfo.u4DestMask = u4FsSrTeDestMask;
    SrTeRtInfo.destAddr.u2AddrType = (UINT2) i4FsSrTeDestAddrType;
    SrTeRtInfo.mandRtrId.u2AddrType = (UINT2) i4FsSrTeDestAddrType;

    pSrTeRtInfo = RBTreeGet (gSrGlobalInfo.pSrTeRouteRbTree, &SrTeRtInfo);
    if (pSrTeRtInfo == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGet Failed\n", __func__,
                __LINE__);
        return SNMP_FAILURE;
    }

    pSrTeRtInfo->srTeRtrListIndex.u4Index = u4SetValFsSrTeRouterListIndex;
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhSetFsSrTePathRowStatus
 Input       :  The Indices
                FsSrTeDestAddr
                FsSrTeDestMask
                FsSrTeRouterId
                FsSrTeDestAddrType

                The Object 
                setValFsSrTePathRowStatus
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrTePathRowStatus (tSNMP_OCTET_STRING_TYPE * pFsSrTeDestAddr,
                           UINT4 u4FsSrTeDestMask,
                           tSNMP_OCTET_STRING_TYPE * pFsSrTeRouterId,
                           INT4 i4FsSrTeDestAddrType,
                           INT4 i4SetValFsSrTePathRowStatus)
{
    tSrTeRtEntryInfo    SrTeRtInfo;
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;
    tSrRtrEntry        *pSrPeerRtrInfo = NULL;
    tTMO_SLL_NODE      *pSrOptRtrInfo = NULL;
    UINT4               u4DestAddr = SR_ZERO;
    UINT4               u4MandRtrId = SR_ZERO;
    INT4                i4LfaStatus = SR_ZERO;
    tSrRtrInfo         *pRtrInfo = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tGenU4Addr          DestAddr;

    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    MEMSET (&DestAddr, SR_ZERO, sizeof (tGenU4Addr));

    if (i4FsSrTeDestAddrType == SR_IPV4_ADDR_TYPE)
    {
        MEMCPY (&u4DestAddr, pFsSrTeDestAddr->pu1_OctetList, IPV4_ADDR_LENGTH);
        MEMCPY (&u4MandRtrId, pFsSrTeRouterId->pu1_OctetList, IPV4_ADDR_LENGTH);
        u4DestAddr = OSIX_NTOHL (u4DestAddr);
        u4MandRtrId = OSIX_NTOHL (u4MandRtrId);

        SR_TRC6 (SR_MGMT_TRC,
                 "%s:%d ENTRY DestAddr %x Mask %x MandRtrId %x RowStatus %d\n",
                 __func__, __LINE__, u4DestAddr, u4FsSrTeDestMask, u4MandRtrId,
                 i4SetValFsSrTePathRowStatus);

        SrTeRtInfo.destAddr.Addr.u4Addr = u4DestAddr;
        SrTeRtInfo.mandRtrId.Addr.u4Addr = u4MandRtrId;
    }
    else if (i4FsSrTeDestAddrType == SR_IPV6_ADDR_TYPE)
    {
        MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                pFsSrTeDestAddr->pu1_OctetList, SR_IPV6_ADDR_LENGTH);
        MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                pFsSrTeRouterId->pu1_OctetList, SR_IPV6_ADDR_LENGTH);
    }

    SrTeRtInfo.u4DestMask = u4FsSrTeDestMask;
    SrTeRtInfo.mandRtrId.u2AddrType = (UINT2) i4FsSrTeDestAddrType;
    SrTeRtInfo.destAddr.u2AddrType = (UINT2) i4FsSrTeDestAddrType;
    pSrTeRtInfo = SrGetTeRouteTableEntry (&SrTeRtInfo);

    switch (i4SetValFsSrTePathRowStatus)
    {
        case CREATE_AND_WAIT:

            if (pSrTeRtInfo != NULL)
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d SR-TE path Already Exist\n",
                         __func__, __LINE__);
                return SNMP_FAILURE;
            }
            else
            {
                pSrTeRtInfo = SrCreateTeRouteTableEntry (&SrTeRtInfo);

                if (pSrTeRtInfo == NULL)
                {
                    SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d SR-TE path Already Exist\n",
                             __func__, __LINE__);
                    return SNMP_FAILURE;
                }

                pSrTeRtInfo->u1RowStatus = NOT_READY;
            }

            break;
        case NOT_IN_SERVICE:

            if (pSrTeRtInfo == NULL)
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d SR-TE path doesn't Exist\n",
                         __func__, __LINE__);
                return SNMP_FAILURE;
            }

            pSrTeRtInfo->u1RowStatus = NOT_IN_SERVICE;

            break;

        case ACTIVE:

            if (pSrTeRtInfo == NULL)
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d SR-TE path doesn't Exist\n",
                         __func__, __LINE__);
                return SNMP_FAILURE;
            }

            TMO_SLL_Scan (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                          pSrOptRtrInfo, tTMO_SLL_NODE *)
            {
                pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;
                pSrPeerRtrInfo->u1RowStatus = ACTIVE;
            }
            pSrTeRtInfo->u1RowStatus = ACTIVE;

            /* To Process SR TE LSP */
            if (SrTeProcess (&(pSrTeRtInfo->destAddr), SR_TRUE) == SR_FAILURE)
            {
                SR_TRC3 (SR_MGMT_TRC | SR_FAIL_TRC,
                         " Line %d func %s SR TE processing failed for prefix %x \r\n",
                         __LINE__, __func__, pSrTeRtInfo->destAddr.Addr.u4Addr);
                return SNMP_FAILURE;
            }

            break;

        case DESTROY:

            if (pSrTeRtInfo == NULL)
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d SR-TE path doesn't Exist\n",
                         __func__, __LINE__);
                return SNMP_FAILURE;
            }
            if (i4FsSrTeDestAddrType == SR_IPV4_ADDR_TYPE)
            {
                DestAddr.Addr.u4Addr = pSrTeRtInfo->destAddr.Addr.u4Addr;
                DestAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
            }

            if (i4FsSrTeDestAddrType == SR_IPV6_ADDR_TYPE)
            {
                MEMCPY (DestAddr.Addr.Ip6Addr.u1_addr,
                        pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH);
                DestAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
            }

            if (SrTeProcess (&(pSrTeRtInfo->destAddr), SR_FALSE) == SR_FAILURE)
            {
                SR_TRC3 (SR_MGMT_TRC | SR_FAIL_TRC,
                         " Line %d func %s SR TE processing failed for prefix %x \r\n",
                         __LINE__, __func__, pSrTeRtInfo->destAddr.Addr.u4Addr);
                return SNMP_FAILURE;
            }

            if (SrDeleteTeRouteTableEntry (pSrTeRtInfo) == SR_FAILURE)
            {
                SR_TRC3 (SR_MGMT_TRC | SR_FAIL_TRC,
                         "%s:%d SR-TE path Deletion Failure for Prefix : %x \n",
                         __func__, __LINE__, pSrTeRtInfo->destAddr.Addr.u4Addr);
                return SNMP_FAILURE;
            }
            if ((pRtrInfo = SrGetSrRtrInfoFromRtrId (&DestAddr)) == NULL)
            {
                SR_TRC3 (SR_MGMT_TRC | SR_FAIL_TRC,
                         "%s:%d SrGetSrRtrInfoFromRtrId FAILURE for RTR ID : %x \n",
                         __func__, __LINE__, DestAddr.Addr.u4Addr);
                return SNMP_FAILURE;
            }
            /* Once SR_TE is removed, need to create primary and FRR LSp for the respective route */
            if (SrFtnCreateorDelete (pRtrInfo, NULL, SR_TRUE) == SR_SUCCESS)
            {
                SR_TRC3 (SR_MGMT_TRC,
                         " Line %d func %s Primary and FRR Lsp processing succeeded for prefix  %x \r\n",
                         __LINE__, __func__, DestAddr.Addr.u4Addr);
            }
            else
            {
                SR_TRC3 (SR_MGMT_TRC | SR_FAIL_TRC,
                         " Line %d func %s Primary and FRR Lsp processing failed for prefix  %x \r\n",
                         __LINE__, __func__, DestAddr.Addr.u4Addr);
                return SNMP_FAILURE;
            }

            /* To Get SR LFA status */
            nmhGetFsSrV4AlternateStatus (&i4LfaStatus);

            if (i4LfaStatus == ALTERNATE_ENABLED)
            {
                TMO_SLL_Scan (&(pRtrInfo->NextHopList),
                              pSrRtrNextHopInfo, tSrRtrNextHopInfo *)
                {
                    if (SrFrrFtnCreateOrDelete
                        (pRtrInfo, pSrRtrNextHopInfo, SR_TRUE) == SR_FAILURE)
                    {
                        SR_TRC3 (SR_MGMT_TRC | SR_FAIL_TRC,
                                 " Line %d func %s Primary and FRR Lsp processing failed for prefix  %x \r\n",
                                 __LINE__, __func__, DestAddr.Addr.u4Addr);
                        return SNMP_FAILURE;
                    }
                }

            }

            break;

        default:
            return SNMP_FAILURE;
    }

    return SNMP_SUCCESS;

}

/* Low Level TEST Routines for All Objects  */

/****************************************************************************
 Function    :  nmhTestv2FsSrTeRouterListIndex
 Input       :  The Indices
                FsSrTeDestAddr
                FsSrTeDestMask
                FsSrTeRouterId
                FsSrTeDestAddrType

                The Object 
                testValFsSrTeRouterListIndex
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrTeRouterListIndex (UINT4 *pu4ErrorCode,
                                tSNMP_OCTET_STRING_TYPE * pFsSrTeDestAddr,
                                UINT4 u4FsSrTeDestMask,
                                tSNMP_OCTET_STRING_TYPE * pFsSrTeRouterId,
                                INT4 i4FsSrTeDestAddrType,
                                UINT4 u4TestValFsSrTeRouterListIndex)
{
    INT4                i4GblSrMode = SR_DISABLE;

    UNUSED_PARAM (pFsSrTeDestAddr);
    UNUSED_PARAM (u4FsSrTeDestMask);
    UNUSED_PARAM (pFsSrTeRouterId);
    UNUSED_PARAM (i4FsSrTeDestAddrType);

    nmhGetFsSrGlobalAdminStatus (&i4GblSrMode);
    if (i4GblSrMode == SR_DISABLE)
    {
        *pu4ErrorCode = SNMP_ERR_AUTHORIZATION_ERROR;
        return SNMP_FAILURE;
    }

    if ((u4TestValFsSrTeRouterListIndex <= 0) ||
        (u4TestValFsSrTeRouterListIndex > MAX_SR_TE_PATH_ENTRIES))
    {
        *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
        return SNMP_FAILURE;
    }
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhTestv2FsSrTePathRowStatus
 Input       :  The Indices
                FsSrTeDestAddr
                FsSrTeDestMask
                FsSrTeRouterId
                FsSrTeDestAddrType

                The Object 
                testValFsSrTePathRowStatus
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrTePathRowStatus (UINT4 *pu4ErrorCode,
                              tSNMP_OCTET_STRING_TYPE * pFsSrTeDestAddr,
                              UINT4 u4FsSrTeDestMask,
                              tSNMP_OCTET_STRING_TYPE * pFsSrTeRouterId,
                              INT4 i4FsSrTeDestAddrType,
                              INT4 i4TestValFsSrTePathRowStatus)
{
    tSrTeRtEntryInfo    SrTeRtInfo;
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;
    UINT4               u4DestAddr = SR_ZERO;
    UINT4               u4MandRtrId = SR_ZERO;
    INT4                i4SrMode = SR_MODE_DEFAULT;
#ifdef IP6_WANTED
    UINT4               u4IfIndex = SR_ZERO;
#endif
    tGenU4Addr          destAddr;
    tGenU4Addr          mandRtrAddr;
    tGenU4Addr          dummyAddr;

    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    MEMSET (&destAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&mandRtrAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));

    if (i4FsSrTeDestAddrType == SR_IPV4_ADDR_TYPE)
    {
        if ((pFsSrTeDestAddr != NULL)
            && (pFsSrTeDestAddr->pu1_OctetList != NULL))
        {
            MEMCPY (&u4DestAddr, pFsSrTeDestAddr->pu1_OctetList,
                    IPV4_ADDR_LENGTH);
        }

        if ((pFsSrTeRouterId != NULL)
            && (pFsSrTeRouterId->pu1_OctetList != NULL))
        {
            MEMCPY (&u4MandRtrId, pFsSrTeRouterId->pu1_OctetList,
                    IPV4_ADDR_LENGTH);
        }
        u4DestAddr = OSIX_NTOHL (u4DestAddr);
        u4MandRtrId = OSIX_NTOHL (u4MandRtrId);
        SR_TRC6 (SR_MGMT_TRC,
                 "%s:%d ENTRY DestAddr %x Mask %x MandRtrId %x RowStatus %d\n",
                 __func__, __LINE__, u4DestAddr, u4FsSrTeDestMask, u4MandRtrId,
                 i4TestValFsSrTePathRowStatus);
    }
    else if (i4FsSrTeDestAddrType == SR_IPV6_ADDR_TYPE)
    {
        if ((pFsSrTeDestAddr != NULL)
            && (pFsSrTeDestAddr->pu1_OctetList != NULL))
        {
            MEMCPY (destAddr.Addr.Ip6Addr.u1_addr,
                    pFsSrTeDestAddr->pu1_OctetList, SR_IPV6_ADDR_LENGTH);
        }

        if ((pFsSrTeRouterId != NULL)
            && (pFsSrTeRouterId->pu1_OctetList != NULL))
        {
            MEMCPY (mandRtrAddr.Addr.Ip6Addr.u1_addr,
                    pFsSrTeRouterId->pu1_OctetList, SR_IPV6_ADDR_LENGTH);
        }

        destAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
        mandRtrAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
    }

    nmhGetFsSrMode (&i4SrMode);

    if (i4SrMode == SR_MODE_DEFAULT)
    {
        *pu4ErrorCode = SNMP_ERR_AUTHORIZATION_ERROR;
        return SNMP_FAILURE;
    }

    if (((i4FsSrTeDestAddrType == SR_IPV4_ADDR_TYPE) &&
         ((i4SrMode & SR_CONF_OSPF) != SR_CONF_OSPF)) ||
        ((i4FsSrTeDestAddrType == SR_IPV6_ADDR_TYPE) &&
         ((i4SrMode & SR_CONF_OSPFV3) != SR_CONF_OSPFV3)))
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d SR Mode is not Dynamic \n", __func__,
                 __LINE__);
        *pu4ErrorCode = SNMP_ERR_NO_CREATION;
        return SNMP_FAILURE;
    }

    if (i4FsSrTeDestAddrType == SR_IPV4_ADDR_TYPE)
    {
        if (u4DestAddr == u4MandRtrId)
        {
            SR_TRC2 (SR_MGMT_TRC, "%s:%d SR Mode is not Dynamic \n", __func__,
                     __LINE__);
            SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC,
                     "%s:%d DestIpAddr & MandRtrId can't be same \n",
                     __func__, __LINE__);
            *pu4ErrorCode = SNMP_ERR_BAD_VALUE;
            return SNMP_FAILURE;
        }

        SrTeRtInfo.destAddr.Addr.u4Addr = u4DestAddr;
        SrTeRtInfo.mandRtrId.Addr.u4Addr = u4MandRtrId;
    }
    else if (i4FsSrTeDestAddrType == SR_IPV6_ADDR_TYPE)
    {
        if (MEMCMP
            (destAddr.Addr.Ip6Addr.u1_addr, mandRtrAddr.Addr.Ip6Addr.u1_addr,
             SR_IPV6_ADDR_LENGTH) == SR_ZERO)
        {
            SR_TRC2 (SR_MGMT_TRC, "%s:%d SR Mode is not Dynamic \n", __func__,
                     __LINE__);
            SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC,
                     "%s:%d DestIpAddr & MandRtrId can't be same \n",
                     __func__, __LINE__);
            *pu4ErrorCode = SNMP_ERR_BAD_VALUE;
            return SNMP_FAILURE;
        }
        MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                destAddr.Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);
        MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                mandRtrAddr.Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);
    }

    SrTeRtInfo.u4DestMask = u4FsSrTeDestMask;
    SrTeRtInfo.destAddr.u2AddrType = (UINT2) i4FsSrTeDestAddrType;
    SrTeRtInfo.mandRtrId.u2AddrType = (UINT2) i4FsSrTeDestAddrType;
    pSrTeRtInfo = SrGetTeRouteTableEntry (&SrTeRtInfo);

    switch (i4TestValFsSrTePathRowStatus)
    {
        case CREATE_AND_WAIT:

            /*Test 1: Te-Path (DestAddr,Mask & MandRtrId) should not be already present (Check RBTree) */

            if (pSrTeRtInfo != NULL)
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d SR-TE path Already Exist\n",
                         __func__, __LINE__);
                *pu4ErrorCode = SNMP_ERR_INCONSISTENT_VALUE;
                return SNMP_FAILURE;
            }
            if (i4FsSrTeDestAddrType == SR_IPV4_ADDR_TYPE)
            {
                /*Test 2: IP Address related checks */
                if (u4DestAddr == SR_ZERO || u4MandRtrId == SR_ZERO)
                {
                    *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
                    return SNMP_FAILURE;
                }
                else if (!((MPLS_IS_ADDR_CLASS_A (u4DestAddr)) ||
                           (MPLS_IS_ADDR_CLASS_B (u4DestAddr)) ||
                           (MPLS_IS_ADDR_CLASS_C (u4DestAddr)) ||
                           (MPLS_IS_ADDR_CLASS_A (u4MandRtrId)) ||
                           (MPLS_IS_ADDR_CLASS_B (u4MandRtrId)) ||
                           (MPLS_IS_ADDR_CLASS_C (u4MandRtrId))))
                {
                    *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
                    return SNMP_FAILURE;
                }

                if ((MPLS_IS_ADDR_LOOPBACK (u4DestAddr)) ||
                    (MPLS_IS_BROADCAST_ADDR (u4DestAddr)) ||
                    (MPLS_IS_ADDR_LOOPBACK (u4MandRtrId)) ||
                    (MPLS_IS_BROADCAST_ADDR (u4MandRtrId)))
                {
                    *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
                    return SNMP_FAILURE;
                }
                /*Test 3: DestAddr & MandRtrId should not be Self IP Addr */
                if (NetIpv4IfIsOurAddress (u4DestAddr) == NETIPV4_SUCCESS)
                {
                    *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
                    return SNMP_FAILURE;
                }
                else if (NetIpv4IfIsOurAddress (u4MandRtrId) == NETIPV4_SUCCESS)
                {
                    *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
                    return SNMP_FAILURE;
                }
            }
            else if (i4FsSrTeDestAddrType == SR_IPV6_ADDR_TYPE)
            {
                if ((MEMCMP
                     (destAddr.Addr.Ip6Addr.u1_addr,
                      dummyAddr.Addr.Ip6Addr.u1_addr,
                      SR_IPV6_ADDR_LENGTH) == SR_ZERO)
                    ||
                    (MEMCMP
                     (mandRtrAddr.Addr.Ip6Addr.u1_addr,
                      dummyAddr.Addr.Ip6Addr.u1_addr,
                      SR_IPV6_ADDR_LENGTH) == SR_ZERO))
                {
                    *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
                    return SNMP_FAILURE;
                }
#ifdef IP6_WANTED
                if (NetIpv6IsOurAddress (&destAddr.Addr.Ip6Addr, &u4IfIndex) ==
                    NETIPV6_SUCCESS)
                {
                    *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
                    return SNMP_FAILURE;
                }
                else if (NetIpv6IsOurAddress
                         (&mandRtrAddr.Addr.Ip6Addr,
                          &u4IfIndex) == NETIPV6_SUCCESS)
                {
                    *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
                    return SNMP_FAILURE;
                }
#endif
            }
            break;
        case ACTIVE:

            if (pSrTeRtInfo == NULL)
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d SR-TE path doesn't Exist\n",
                         __func__, __LINE__);
                *pu4ErrorCode = SNMP_ERR_NO_CREATION;
                return SNMP_FAILURE;
            }

            break;

        case NOT_IN_SERVICE:

            if (pSrTeRtInfo == NULL)
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d SR-TE path doesn't Exist\n",
                         __func__, __LINE__);
                return SNMP_FAILURE;
            }

            break;

        case DESTROY:

            if (pSrTeRtInfo == NULL)
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d SR-TE path doesn't Exist\n",
                         __func__, __LINE__);
                *pu4ErrorCode = SNMP_ERR_NO_CREATION;
                return SNMP_FAILURE;
            }

            break;

        default:
            return SNMP_FAILURE;
    }
    return SNMP_SUCCESS;
}

/* Low Level Dependency Routines for All Objects  */

/****************************************************************************
 Function    :  nmhDepv2FsSrTePathTable
 Input       :  The Indices
                FsSrTeDestAddr
                FsSrTeDestMask
                FsSrTeRouterId
                FsSrTeDestAddrType
 Output      :  The Dependency Low Lev Routine Take the Indices &
                check whether dependency is met or not.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhDepv2FsSrTePathTable (UINT4 *pu4ErrorCode, tSnmpIndexList * pSnmpIndexList,
                         tSNMP_VAR_BIND * pSnmpVarBind)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (pSnmpIndexList);
    UNUSED_PARAM (pSnmpVarBind);
    return SNMP_SUCCESS;
}

/* LOW LEVEL Routines for Table : FsSrTeRtrListTable. */

/****************************************************************************
 Function    :  nmhValidateIndexInstanceFsSrTeRtrListTable
 Input       :  The Indices
                FsSrTeRtrListIndex
                FsSrTeRouterIndex
 Output      :  The Routines Validates the Given Indices.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
/* GET_EXACT Validate Index Instance Routine. */

INT1
nmhValidateIndexInstanceFsSrTeRtrListTable (UINT4 u4FsSrTeRtrListIndex,
                                            UINT4 u4FsSrTeRouterIndex)
{
    if (((u4FsSrTeRtrListIndex > 0)
         && (u4FsSrTeRtrListIndex <= SR_MAX_TE_RT_ENTRIES))
        && ((u4FsSrTeRouterIndex > 0)
            && (u4FsSrTeRouterIndex <= SR_MAX_TE_OPTIONAL_RTR)))
    {
        return SNMP_SUCCESS;
    }
    return SNMP_FAILURE;
}

/****************************************************************************
 Function    :  nmhGetFirstIndexFsSrTeRtrListTable
 Input       :  The Indices
                FsSrTeRtrListIndex
                FsSrTeRouterIndex
 Output      :  The Get First Routines gets the Lexicographicaly
                First Entry from the Table.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
/* GET_FIRST Routine. */

INT1
nmhGetFirstIndexFsSrTeRtrListTable (UINT4 *pu4FsSrTeRtrListIndex,
                                    UINT4 *pu4FsSrTeRouterIndex)
{
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;
    tSrRtrEntry        *pSrPeerRtrInfo = NULL;

    if (gSrGlobalInfo.pSrTeRouteRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrTeRouteRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    if ((pSrTeRtInfo = RBTreeGetFirst (gSrGlobalInfo.pSrTeRouteRbTree)) == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGetFirst FAILED\n", __func__,
                 __LINE__);
        return SNMP_FAILURE;
    }
    pSrPeerRtrInfo =
        (tSrRtrEntry *)
        TMO_SLL_First (&(pSrTeRtInfo->srTeRtrListIndex.RtrList));
    if (pSrPeerRtrInfo != NULL)
    {
        *pu4FsSrTeRtrListIndex = pSrTeRtInfo->srTeRtrListIndex.u4Index;
        *pu4FsSrTeRouterIndex = pSrPeerRtrInfo->u4RtrIndex;
        return SNMP_SUCCESS;
    }
    return SNMP_FAILURE;
}

/****************************************************************************
 Function    :  nmhGetNextIndexFsSrTeRtrListTable
 Input       :  The Indices
                FsSrTeRtrListIndex
                nextFsSrTeRtrListIndex
                FsSrTeRouterIndex
                nextFsSrTeRouterIndex
 Output      :  The Get Next function gets the Next Index for
                the Index Value given in the Index Values. The
                Indices are stored in the next_varname variables.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
/* GET_NEXT Routine.  */
INT1
nmhGetNextIndexFsSrTeRtrListTable (UINT4 u4FsSrTeRtrListIndex,
                                   UINT4 *pu4NextFsSrTeRtrListIndex,
                                   UINT4 u4FsSrTeRouterIndex,
                                   UINT4 *pu4NextFsSrTeRouterIndex)
{
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;
    tTMO_SLL_NODE      *pSrOptRtrInfo = NULL;
    tSrRtrEntry        *pSrPeerRtrInfo = NULL;
    tSrRtrEntry        *pSrTmpPeerRtrInfo = NULL;

    if (gSrGlobalInfo.pSrTeRouteRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrTeRouteRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    if ((pSrTeRtInfo = RBTreeGetFirst (gSrGlobalInfo.pSrTeRouteRbTree)) == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGetFirst FAILED\n", __func__,
                 __LINE__);
        return SNMP_FAILURE;
    }

    while (pSrTeRtInfo != NULL)
    {
        if (pSrTeRtInfo->srTeRtrListIndex.u4Index == u4FsSrTeRtrListIndex)
        {
            TMO_SLL_Scan (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                          pSrOptRtrInfo, tTMO_SLL_NODE *)
            {
                pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;
                if (pSrPeerRtrInfo->u4RtrIndex == u4FsSrTeRouterIndex)
                {
                    if (pSrPeerRtrInfo->u4RtrIndex <
                        pSrTeRtInfo->srTeRtrListIndex.RtrList.u4_Count)
                    {
                        pSrTmpPeerRtrInfo =
                            (tSrRtrEntry *) (pSrOptRtrInfo->pNext);
                        *pu4NextFsSrTeRouterIndex =
                            pSrTmpPeerRtrInfo->u4RtrIndex;
                        *pu4NextFsSrTeRtrListIndex =
                            pSrTeRtInfo->srTeRtrListIndex.u4Index;
                        return SNMP_SUCCESS;
                    }
                    else
                    {
                        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC,
                                 "%s:%d Index points to last element in list.\n",
                                 __func__, __LINE__);
                        return SNMP_FAILURE;
                    }
                }
            }
            SR_TRC3 (SR_MGMT_TRC | SR_FAIL_TRC,
                     "%s:%d No Router entry found with Router index: %d\n",
                     __func__, __LINE__, u4FsSrTeRtrListIndex);
            return SNMP_FAILURE;
        }
        pSrTeRtInfo =
            RBTreeGetNext (gSrGlobalInfo.pSrTeRouteRbTree, pSrTeRtInfo, NULL);
    }
    SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d No such entry present \n", __func__,
             __LINE__);
    return SNMP_FAILURE;
}

/* Low Level GET Routine for All Objects  */

/****************************************************************************
 Function    :  nmhGetFsSrTeOptRouterId
 Input       :  The Indices
                FsSrTeRtrListIndex
                FsSrTeRouterIndex

                The Object 
                retValFsSrTeOptRouterId
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrTeOptRouterId (UINT4 u4FsSrTeRtrListIndex,
                         UINT4 u4FsSrTeRouterIndex,
                         tSNMP_OCTET_STRING_TYPE * pRetValFsSrTeOptRouterId)
{
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;
    tTMO_SLL_NODE      *pSrOptRtrInfo = NULL;
    tSrRtrEntry        *pSrPeerRtrInfo = NULL;

    if (gSrGlobalInfo.pSrTeRouteRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrTeRouteRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    if ((pSrTeRtInfo = RBTreeGetFirst (gSrGlobalInfo.pSrTeRouteRbTree)) == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGetFirst FAILED\n", __func__,
                 __LINE__);
        return SNMP_FAILURE;
    }
    while (pSrTeRtInfo != NULL)
    {
        if (pSrTeRtInfo->srTeRtrListIndex.u4Index == u4FsSrTeRtrListIndex)
        {
            TMO_SLL_Scan (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                          pSrOptRtrInfo, tTMO_SLL_NODE *)
            {
                pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;
                if (pSrPeerRtrInfo->u4RtrIndex == u4FsSrTeRouterIndex)
                {
                    if (pSrPeerRtrInfo->routerId.u2AddrType ==
                        SR_IPV4_ADDR_TYPE)
                    {
                        SR_INTEGER_TO_OCTETSTRING (pSrPeerRtrInfo->routerId.
                                                   Addr.u4Addr,
                                                   pRetValFsSrTeOptRouterId);
                    }
                    else if (pSrPeerRtrInfo->routerId.u2AddrType ==
                             SR_IPV6_ADDR_TYPE)
                    {
                        MEMCPY (pRetValFsSrTeOptRouterId->pu1_OctetList,
                                pSrPeerRtrInfo->routerId.Addr.Ip6Addr.u1_addr,
                                SR_IPV6_ADDR_LENGTH);

                        pRetValFsSrTeOptRouterId->i4_Length =
                            SR_IPV6_ADDR_LENGTH;
                    }
                    return SNMP_SUCCESS;
                }
            }
            SR_TRC3 (SR_MGMT_TRC | SR_FAIL_TRC,
                     "%s:%d No Router entry found with Router index: %d\n",
                     __func__, __LINE__, u4FsSrTeRtrListIndex);
            return SNMP_FAILURE;
        }
        pSrTeRtInfo =
            RBTreeGetNext (gSrGlobalInfo.pSrTeRouteRbTree, pSrTeRtInfo, NULL);
    }
    SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d No such entry present \n", __func__,
             __LINE__);
    return SNMP_FAILURE;
}

/****************************************************************************
 Function    :  nmhGetFsSrTeRtrRowStatus
 Input       :  The Indices
                FsSrTeRtrListIndex
                FsSrTeRouterIndex

                The Object 
                retValFsSrTeRtrRowStatus
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrTeRtrRowStatus (UINT4 u4FsSrTeRtrListIndex,
                          UINT4 u4FsSrTeRouterIndex,
                          INT4 *pi4RetValFsSrTeRtrRowStatus)
{
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;
    tTMO_SLL_NODE      *pSrOptRtrInfo = NULL;
    tSrRtrEntry        *pSrPeerRtrInfo = NULL;

    if (gSrGlobalInfo.pSrTeRouteRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrTeRouteRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    if ((pSrTeRtInfo = RBTreeGetFirst (gSrGlobalInfo.pSrTeRouteRbTree)) == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGetFirst FAILED\n", __func__,
                 __LINE__);
        return SNMP_FAILURE;
    }
    while (pSrTeRtInfo != NULL)
    {
        if (pSrTeRtInfo->srTeRtrListIndex.u4Index == u4FsSrTeRtrListIndex)
        {
            TMO_SLL_Scan (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                          pSrOptRtrInfo, tTMO_SLL_NODE *)
            {
                pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;
                if (pSrPeerRtrInfo->u4RtrIndex == u4FsSrTeRouterIndex)
                {
                    *pi4RetValFsSrTeRtrRowStatus = pSrPeerRtrInfo->u1RowStatus;
                    return SNMP_SUCCESS;
                }
            }
            SR_TRC3 (SR_MGMT_TRC | SR_FAIL_TRC,
                     "%s:%d No Router entry found with Router index: %d\n",
                     __func__, __LINE__, u4FsSrTeRtrListIndex);
            return SNMP_FAILURE;
        }
        pSrTeRtInfo =
            RBTreeGetNext (gSrGlobalInfo.pSrTeRouteRbTree, pSrTeRtInfo, NULL);
    }
    SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d No such entry present \n", __func__,
             __LINE__);
    return SNMP_FAILURE;
}

/* Low Level SET Routine for All Objects  */

/****************************************************************************
 Function    :  nmhSetFsSrTeOptRouterId
 Input       :  The Indices
                FsSrTeRtrListIndex
                FsSrTeRouterIndex

                The Object 
                setValFsSrTeOptRouterId
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrTeOptRouterId (UINT4 u4FsSrTeRtrListIndex,
                         UINT4 u4FsSrTeRouterIndex,
                         tSNMP_OCTET_STRING_TYPE * pSetValFsSrTeOptRouterId)
{
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;
    tTMO_SLL_NODE      *pSrOptRtrInfo = NULL;
    tSrRtrEntry        *pSrPeerRtrInfo = NULL;

    if (gSrGlobalInfo.pSrTeRouteRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrTeRouteRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    if ((pSrTeRtInfo = RBTreeGetFirst (gSrGlobalInfo.pSrTeRouteRbTree)) == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGetFirst FAILED\n", __func__,
                 __LINE__);
        return SNMP_FAILURE;
    }
    while (pSrTeRtInfo != NULL)
    {
        if (pSrTeRtInfo->srTeRtrListIndex.u4Index == u4FsSrTeRtrListIndex)
        {
            TMO_SLL_Scan (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                          pSrOptRtrInfo, tTMO_SLL_NODE *)
            {
                pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;
                if (pSrPeerRtrInfo->u4RtrIndex == u4FsSrTeRouterIndex)
                {
                    if (pSetValFsSrTeOptRouterId->i4_Length ==
                        SR_IPV4_ADDR_LENGTH)
                    {
                        pSrPeerRtrInfo->routerId.u2AddrType = SR_IPV4_ADDR_TYPE;
                        SR_OCTETSTRING_TO_INTEGER (pSetValFsSrTeOptRouterId,
                                                   pSrPeerRtrInfo->routerId.
                                                   Addr.u4Addr);
                    }
                    else if (pSetValFsSrTeOptRouterId->i4_Length ==
                             SR_IPV6_ADDR_LENGTH)
                    {
                        pSrPeerRtrInfo->routerId.u2AddrType = SR_IPV6_ADDR_TYPE;
                        MEMCPY (pSrPeerRtrInfo->routerId.Addr.Ip6Addr.u1_addr,
                                pSetValFsSrTeOptRouterId->pu1_OctetList,
                                pSetValFsSrTeOptRouterId->i4_Length);

                    }
                    return SNMP_SUCCESS;
                }
            }
            SR_TRC3 (SR_MGMT_TRC | SR_FAIL_TRC,
                     "%s:%d No Router entry found with Router index: %d\n",
                     __func__, __LINE__, u4FsSrTeRtrListIndex);
            return SNMP_FAILURE;
        }
        pSrTeRtInfo =
            RBTreeGetNext (gSrGlobalInfo.pSrTeRouteRbTree, pSrTeRtInfo, NULL);
    }
    SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d No such entry present \n", __func__,
             __LINE__);
    return SNMP_FAILURE;
}

/****************************************************************************
 Function    :  nmhSetFsSrTeRtrRowStatus
 Input       :  The Indices
                FsSrTeRtrListIndex
                FsSrTeRouterIndex

                The Object 
                setValFsSrTeRtrRowStatus
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrTeRtrRowStatus (UINT4 u4FsSrTeRtrListIndex,
                          UINT4 u4FsSrTeRouterIndex,
                          INT4 i4SetValFsSrTeRtrRowStatus)
{
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;
    tTMO_SLL_NODE      *pSrOptRtrInfo = NULL;
    tSrRtrEntry        *pSrPeerRtrInfo = NULL;

    if (gSrGlobalInfo.pSrTeRouteRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrTeRouteRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    if ((pSrTeRtInfo = RBTreeGetFirst (gSrGlobalInfo.pSrTeRouteRbTree)) == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGetFirst FAILED\n", __func__,
                 __LINE__);
        return SNMP_FAILURE;
    }

    while (pSrTeRtInfo != NULL)
    {
        if (pSrTeRtInfo->srTeRtrListIndex.u4Index == u4FsSrTeRtrListIndex)
        {
            TMO_SLL_Scan (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                          pSrOptRtrInfo, tTMO_SLL_NODE *)
            {
                pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;
                if (pSrPeerRtrInfo->u4RtrIndex == u4FsSrTeRouterIndex)
                {
                    break;
                }
                pSrPeerRtrInfo = NULL;
            }

            switch (i4SetValFsSrTeRtrRowStatus)
            {
                case CREATE_AND_WAIT:
                    if (pSrPeerRtrInfo != NULL)
                    {
                        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC,
                                 "%s:%d Optional Router ID in Te-Path Already Exist\n",
                                 __func__, __LINE__);
                        return SNMP_FAILURE;
                    }
                    pSrPeerRtrInfo =
                        SrCreateOptRtrEntry (u4FsSrTeRtrListIndex,
                                             u4FsSrTeRouterIndex);
                    if (pSrPeerRtrInfo == NULL)
                    {
                        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC,
                                 "%s:%d Error while creating optional Router ID\n",
                                 __func__, __LINE__);
                        return SNMP_FAILURE;
                    }
                    pSrPeerRtrInfo->u1RowStatus = NOT_READY;
                    break;
                case ACTIVE:
                case NOT_IN_SERVICE:

                    if (pSrPeerRtrInfo == NULL)
                    {
                        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC,
                                 "%s:%d Router ID does not Exist\n", __func__,
                                 __LINE__);
                        return SNMP_FAILURE;
                    }
                    pSrPeerRtrInfo->u1RowStatus =
                        (UINT1) i4SetValFsSrTeRtrRowStatus;
                    break;

                case DESTROY:
                    /*If No Te PathEntry present then DESTROY Optional-RTR */
                    if (pSrPeerRtrInfo == NULL)
                    {
                        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC,
                                 "%s:%d Router ID does not Exist\n", __func__,
                                 __LINE__);
                        return SNMP_FAILURE;
                    }
                    else
                    {
                        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC,
                                 "%s:%d Cannot delete Optional Router ID."
                                 "TE-Path containing the Optional Router ID still exist.\n",
                                 __func__, __LINE__);
                        return SNMP_FAILURE;
                    }
                    break;
                default:
                    return SNMP_FAILURE;
            }
            return SNMP_SUCCESS;
        }
        pSrTeRtInfo =
            RBTreeGetNext (gSrGlobalInfo.pSrTeRouteRbTree, pSrTeRtInfo, NULL);
    }
    SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d No such entry present \n", __func__,
             __LINE__);
    return SNMP_FAILURE;
}

/* Low Level TEST Routines for All Objects  */

/****************************************************************************
 Function    :  nmhTestv2FsSrTeOptRouterId
 Input       :  The Indices
                FsSrTeRtrListIndex
                FsSrTeRouterIndex

                The Object 
                testValFsSrTeOptRouterId
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrTeOptRouterId (UINT4 *pu4ErrorCode,
                            UINT4 u4FsSrTeRtrListIndex,
                            UINT4 u4FsSrTeRouterIndex,
                            tSNMP_OCTET_STRING_TYPE * pTestValFsSrTeOptRouterId)
{
#ifdef IP6_WANTED
    UINT4               u4IfIndex = SR_ZERO;
#endif
    UINT4               u4OptRtrId = SR_ZERO;
    INT4                i4GblSrMode = SR_MODE_DEFAULT;
    tGenU4Addr          optRtrId;

    MEMSET (&optRtrId, SR_ZERO, sizeof (tGenU4Addr));

    UNUSED_PARAM (u4FsSrTeRtrListIndex);
    UNUSED_PARAM (u4FsSrTeRouterIndex);

    nmhGetFsSrGlobalAdminStatus (&i4GblSrMode);
    if (i4GblSrMode == SR_DISABLE)
    {
        *pu4ErrorCode = SNMP_ERR_AUTHORIZATION_ERROR;
        return SNMP_FAILURE;
    }

    if (pTestValFsSrTeOptRouterId->i4_Length == SR_IPV4_ADDR_LENGTH)
    {
        SR_OCTETSTRING_TO_INTEGER (pTestValFsSrTeOptRouterId, u4OptRtrId);
    }
    else if (pTestValFsSrTeOptRouterId->i4_Length == SR_IPV6_ADDR_LENGTH)
    {
        optRtrId.u2AddrType = SR_IPV6_ADDR_TYPE;
        MEMCPY (optRtrId.Addr.Ip6Addr.u1_addr,
                pTestValFsSrTeOptRouterId->pu1_OctetList,
                pTestValFsSrTeOptRouterId->i4_Length);

    }
    if (u4OptRtrId != SR_ZERO)
    {
        if (!((MPLS_IS_ADDR_CLASS_A (u4OptRtrId)) ||
              (MPLS_IS_ADDR_CLASS_B (u4OptRtrId)) ||
              (MPLS_IS_ADDR_CLASS_C (u4OptRtrId))))
        {
            *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
            return SNMP_FAILURE;
        }

        if ((MPLS_IS_ADDR_LOOPBACK (u4OptRtrId)) ||
            (MPLS_IS_BROADCAST_ADDR (u4OptRtrId)))
        {
            *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
            return SNMP_FAILURE;
        }

        if (NetIpv4IfIsOurAddress (u4OptRtrId) == NETIPV4_SUCCESS)
        {
            *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
            return SNMP_FAILURE;
        }
    }
#ifdef IP6_WANTED
    else if (optRtrId.u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        if (NetIpv6IsOurAddress (&optRtrId.Addr.Ip6Addr, &u4IfIndex) ==
            NETIPV6_SUCCESS)
        {
            *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
            return SNMP_FAILURE;
        }
    }
#endif
    else
    {
        *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
        return SNMP_FAILURE;
    }
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhTestv2FsSrTeRtrRowStatus
 Input       :  The Indices
                FsSrTeRtrListIndex
                FsSrTeRouterIndex

                The Object 
                testValFsSrTeRtrRowStatus
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrTeRtrRowStatus (UINT4 *pu4ErrorCode,
                             UINT4 u4FsSrTeRtrListIndex,
                             UINT4 u4FsSrTeRouterIndex,
                             INT4 i4TestValFsSrTeRtrRowStatus)
{
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;
    tTMO_SLL_NODE      *pSrOptRtrInfo = NULL;
    tSrRtrEntry        *pSrPeerRtrInfo = NULL;
    INT4                i4GblSrMode = SR_MODE_DEFAULT;

    nmhGetFsSrGlobalAdminStatus (&i4GblSrMode);
    if (i4GblSrMode == SR_DISABLE)
    {
        *pu4ErrorCode = SNMP_ERR_AUTHORIZATION_ERROR;
        return SNMP_FAILURE;
    }

    if (gSrGlobalInfo.pSrTeRouteRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrTeRouteRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    if ((pSrTeRtInfo = RBTreeGetFirst (gSrGlobalInfo.pSrTeRouteRbTree)) == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGetFirst FAILED\n", __func__,
                 __LINE__);
        return SNMP_FAILURE;
    }
    while (pSrTeRtInfo != NULL)
    {
        if (pSrTeRtInfo->srTeRtrListIndex.u4Index == u4FsSrTeRtrListIndex)
        {
            TMO_SLL_Scan (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                          pSrOptRtrInfo, tTMO_SLL_NODE *)
            {
                pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;
                if (pSrPeerRtrInfo->u4RtrIndex == u4FsSrTeRouterIndex)
                {
                    break;
                }
                pSrPeerRtrInfo = NULL;
            }

            switch (i4TestValFsSrTeRtrRowStatus)
            {
                case CREATE_AND_WAIT:
                    if (pSrPeerRtrInfo != NULL)
                    {
                        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC,
                                 "%s:%d Optional Router ID in Te-Path Already Exist\n",
                                 __func__, __LINE__);
                        *pu4ErrorCode = SNMP_ERR_INCONSISTENT_VALUE;
                        return SNMP_FAILURE;
                    }
                    else
                    {
                        return SNMP_SUCCESS;
                    }
                case ACTIVE:
                case NOT_IN_SERVICE:
                case DESTROY:
                    if (pSrPeerRtrInfo == NULL)
                    {
                        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC,
                                 "%s:%d Router ID does not Exist\n", __func__,
                                 __LINE__);
                        *pu4ErrorCode = SNMP_ERR_NO_CREATION;
                        return SNMP_FAILURE;
                    }
                    break;
                default:
                    return SNMP_FAILURE;
            }
        }
        pSrTeRtInfo =
            RBTreeGetNext (gSrGlobalInfo.pSrTeRouteRbTree, pSrTeRtInfo, NULL);
    }
    SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d No such entry present \n", __func__,
             __LINE__);
    return SNMP_FAILURE;
}

/* Low Level Dependency Routines for All Objects  */

/****************************************************************************
 Function    :  nmhDepv2FsSrTeRtrListTable
 Input       :  The Indices
                FsSrTeRtrListIndex
                FsSrTeRouterIndex
 Output      :  The Dependency Low Lev Routine Take the Indices &
                check whether dependency is met or not.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhDepv2FsSrTeRtrListTable (UINT4 *pu4ErrorCode,
                            tSnmpIndexList * pSnmpIndexList,
                            tSNMP_VAR_BIND * pSnmpVarBind)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (pSnmpIndexList);
    UNUSED_PARAM (pSnmpVarBind);
    return SNMP_SUCCESS;
}

/* Low Level GET Routine for All Objects  */

/****************************************************************************
 Function    :  nmhGetFsSrV4Status
 Input       :  The Indices

                The Object 
                retValFsSrV4Status
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrV4Status (INT4 *pi4RetValFsSrV4Status)
{
    *pi4RetValFsSrV4Status = (INT4) gSrGlobalInfo.u4SrV4ModuleStatus;
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFsSrV4AlternateStatus
 Input       :  The Indices

                The Object 
                retValFsSrV4AlternateStatus
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrV4AlternateStatus (INT4 *pi4RetValFsSrV4AlternateStatus)
{
    *pi4RetValFsSrV4AlternateStatus =
        (INT4) gu4AltModuleStatus.u4SrIPV4AltModuleStatus;
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFsSrV6MinSrGbValue
 Input       :  The Indices

                The Object 
                retValFsSrV6MinSrGbValue
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrV6MinSrGbValue (UINT4 *pu4RetValFsSrV6MinSrGbValue)
{
    if (IS_V6_SRGB_CONFIGURED == SR_TRUE)
    {
        *pu4RetValFsSrV6MinSrGbValue =
            gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex;
    }
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFsSrV6MaxSrGbValue
 Input       :  The Indices

                The Object 
                retValFsSrV6MaxSrGbValue
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrV6MaxSrGbValue (UINT4 *pu4RetValFsSrV6MaxSrGbValue)
{
    if (IS_V6_SRGB_CONFIGURED == SR_TRUE)
    {
        *pu4RetValFsSrV6MaxSrGbValue =
            gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MaxIndex;
    }
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFsSrV6AlternateStatus
 Input       :  The Indices

                The Object 
                retValFsSrV6AlternateStatus
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrV6AlternateStatus (INT4 *pi4RetValFsSrV6AlternateStatus)
{
    *pi4RetValFsSrV6AlternateStatus =
        (INT4) gu4AltModuleStatus.u4SrIPV6AltModuleStatus;
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFsSrV6Status
 Input       :  The Indices

                The Object 
                retValFsSrV6Status
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrV6Status (INT4 *pi4RetValFsSrV6Status)
{
    *pi4RetValFsSrV6Status = (INT4) gSrGlobalInfo.u4SrV6ModuleStatus;
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFsSrGlobalAdminStatus
 Input       :  The Indices

                The Object 
                retValFsSrGlobalAdminStatus
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrGlobalAdminStatus (INT4 *pi4RetValFsSrGlobalAdminStatus)
{
    *pi4RetValFsSrGlobalAdminStatus = (INT4) gSrGlobalInfo.u4GblSrAdminStatus;
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFsSrIpv4AddrFamily
 Input       :  The Indices

                The Object 
                retValFsSrIpv4AddrFamily
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrIpv4AddrFamily (INT4 *pi4RetValFsSrIpv4AddrFamily)
{
    *pi4RetValFsSrIpv4AddrFamily = (INT4) gSrGlobalInfo.u4Ipv4AddrFamily;
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFsSrIpv6AddrFamily
 Input       :  The Indices

                The Object 
                retValFsSrIpv6AddrFamily
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsSrIpv6AddrFamily (INT4 *pi4RetValFsSrIpv6AddrFamily)
{
    *pi4RetValFsSrIpv6AddrFamily = (INT4) gSrGlobalInfo.u4Ipv6AddrFamily;
    return SNMP_SUCCESS;
}

/* Low Level SET Routine for All Objects  */

/****************************************************************************
 Function    :  nmhSetFsSrV4Status
 Input       :  The Indices

                The Object 
                setValFsSrV4Status
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrV4Status (INT4 i4SetValFsSrV4Status)
{
    INT1                i1RetStatus = SNMP_FAILURE;
    INT4                i4RetVal = SNMP_FAILURE;
    UINT4               u4SrCxtId = SR_ZERO;

    if (i4SetValFsSrV4Status == SR_ENABLED)
    {
        if (SrCliProcessAdminUpCmdForV4 () == SR_SUCCESS)
        {
            i1RetStatus = SNMP_SUCCESS;
        }
        if (gSrGlobalInfo.u1SrRegStatus == SR_FALSE)
        {
            i4RetVal = (INT4) SrSetOspfSrStatus (u4SrCxtId, SR_ENABLED);
            if (i4RetVal == SR_SUCCESS)
            {
                gSrGlobalInfo.u1SrRegStatus = SR_TRUE;
            }
        }
    }
    else if (i4SetValFsSrV4Status == SR_DISABLED)
    {
        if (SrCliProcessAdminDownCmdForV4 (SR_SHUT_CMD) == SR_SUCCESS)
        {
            i1RetStatus = SNMP_SUCCESS;
        }
    }
    return i1RetStatus;
}

/****************************************************************************
 Function    :  nmhSetFsSrV4AlternateStatus
 Input       :  The Indices

                The Object 
                setValFsSrV4AlternateStatus
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrV4AlternateStatus (INT4 i4SetValFsSrV4AlternateStatus)
{
    gu4AltModuleStatus.u4SrIPV4AltModuleStatus =
        (UINT4) i4SetValFsSrV4AlternateStatus;

    /*After the setting the LFA status enable, Register with RTM to fetch the LFA route information */
    if (gu4AltModuleStatus.u4SrIPV4AltModuleStatus == ALTERNATE_ENABLED)
    {
        SrRegisterWithRtmForAlt ();
    }
    else
    {
        SrDeRegisterWithRtmForAlt ();
    }
    SrLfaLSPHandle ();

    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhSetFsSrV6MinSrGbValue
 Input       :  The Indices

                The Object 
                setValFsSrV6MinSrGbValue
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrV6MinSrGbValue (UINT4 u4SetValFsSrV6MinSrGbValue)
{
    if (u4SetValFsSrV6MinSrGbValue != 0)
    {
        gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex =
            u4SetValFsSrV6MinSrGbValue;
        IS_V6_SRGB_CONFIGURED = SR_TRUE;
        return SNMP_SUCCESS;
    }
    return SNMP_FAILURE;
}

/****************************************************************************
 Function    :  nmhSetFsSrV6MaxSrGbValue
 Input       :  The Indices

                The Object 
                setValFsSrV6MaxSrGbValue
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrV6MaxSrGbValue (UINT4 u4SetValFsSrV6MaxSrGbValue)
{
    if (u4SetValFsSrV6MaxSrGbValue != 0)
    {
        gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MaxIndex =
            u4SetValFsSrV6MaxSrGbValue;
        IS_V6_SRGB_CONFIGURED = SR_TRUE;
        return SNMP_SUCCESS;
    }
    return SNMP_FAILURE;
}

/****************************************************************************
 Function    :  nmhSetFsSrV6AlternateStatus
 Input       :  The Indices

                The Object 
                setValFsSrV6AlternateStatus
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrV6AlternateStatus (INT4 i4SetValFsSrV6AlternateStatus)
{
    gu4AltModuleStatus.u4SrIPV6AltModuleStatus =
        (UINT4) i4SetValFsSrV6AlternateStatus;

    /*After the setting the LFA status enable, Register with RTM to fetch the LFA route information */
    if (gu4AltModuleStatus.u4SrIPV6AltModuleStatus == ALTERNATE_ENABLED)
    {
        SrV3RegisterWithRtmForAlt ();
        SrV3EnableAlternate ();
    }
    else
    {
        SrV3DeRegisterWithRtmForAlt ();
    }

    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhSetFsSrV6Status
 Input       :  The Indices

                The Object 
                setValFsSrV6Status
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrV6Status (INT4 i4SetValFsSrV6Status)
{
    INT1                i1RetStatus = SNMP_FAILURE;

    if (i4SetValFsSrV6Status == SR_ENABLED)
    {
        if (SrCliProcessAdminUpCmdForV6 () == SR_SUCCESS)
        {
            i1RetStatus = SNMP_SUCCESS;
        }
    }
    else if (i4SetValFsSrV6Status == SR_DISABLED)
    {
        if (SrCliProcessAdminDownCmdForV6 (SR_SHUT_CMD) == SR_SUCCESS)
        {
            i1RetStatus = SNMP_SUCCESS;
        }
    }
    return i1RetStatus;
}

/****************************************************************************
 Function    :  nmhSetFsSrGlobalAdminStatus
 Input       :  The Indices

                The Object 
                setValFsSrGlobalAdminStatus
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrGlobalAdminStatus (INT4 i4SetValFsSrGlobalAdminStatus)
{
    gSrGlobalInfo.u4GblSrAdminStatus = (UINT4) i4SetValFsSrGlobalAdminStatus;
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhSetFsSrIpv4AddrFamily
 Input       :  The Indices

                The Object 
                setValFsSrIpv4AddrFamily
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrIpv4AddrFamily (INT4 i4SetValFsSrIpv4AddrFamily)
{
    if (i4SetValFsSrIpv4AddrFamily == SR_ENABLE)
    {
        gSrGlobalInfo.u4Ipv4AddrFamily = SR_ENABLE;
    }
    else
    {
        gSrGlobalInfo.u4Ipv4AddrFamily = SR_DISABLE;
    }

    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhSetFsSrIpv6AddrFamily
 Input       :  The Indices

                The Object 
                setValFsSrIpv6AddrFamily
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsSrIpv6AddrFamily (INT4 i4SetValFsSrIpv6AddrFamily)
{
    if (i4SetValFsSrIpv6AddrFamily == SR_ENABLE)
    {
        gSrGlobalInfo.u4Ipv6AddrFamily = SR_ENABLE;
    }
    else
    {
        gSrGlobalInfo.u4Ipv6AddrFamily = SR_DISABLE;
    }

    return SNMP_SUCCESS;
}

/* Low Level TEST Routines for All Objects  */

/****************************************************************************
 Function    :  nmhTestv2FsSrV4Status
 Input       :  The Indices

                The Object 
                testValFsSrV4Status
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrV4Status (UINT4 *pu4ErrorCode, INT4 i4TestValFsSrV4Status)
{
    tSrSidInterfaceInfo   *pSrSidIntf = NULL;
    UINT1                  u1NodeSid = SR_ZERO;

    if (i4TestValFsSrV4Status == SR_ENABLE)
    {
        return SNMP_SUCCESS;
    }
    else if (i4TestValFsSrV4Status == SR_DISABLE)
    {
        return SNMP_SUCCESS;
    }

    *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
    CLI_SET_ERR (CLI_SR_INV_VAL);
    return SNMP_FAILURE;
}

/****************************************************************************
 Function    :  nmhTestv2FsSrV4AlternateStatus
 Input       :  The Indices

                The Object 
                testValFsSrV4AlternateStatus
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrV4AlternateStatus (UINT4 *pu4ErrorCode,
                                INT4 i4TestValFsSrV4AlternateStatus)
{
    INT4                i4AltStatus = ALTERNATE_DISABLED;
    INT4                i4SrMode = SR_MODE_DEFAULT;

#if 0
    if (i4TestValFsSrV4AlternateStatus == ALTERNATE_ENABLED)
    {
        /*Check the SR and LFA status at OSPF level */
        nmhGetFsSrMode (&i4SrMode);
        if ((i4SrMode & SR_CONF_OSPF) != SR_CONF_OSPF)
        {
            *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
            return SNMP_FAILURE;
        }
    }
#endif

    nmhGetFsSrV4AlternateStatus (&i4AltStatus);
    if (i4AltStatus == i4TestValFsSrV4AlternateStatus)
    {
        return SNMP_SUCCESS;
    }

    switch (i4TestValFsSrV4AlternateStatus)
    {
        case ALTERNATE_ENABLED:
        case ALTERNATE_DISABLED:
            break;

        default:
            *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
            return SNMP_FAILURE;
    }
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhTestv2FsSrV6MinSrGbValue
 Input       :  The Indices

                The Object 
                testValFsSrV6MinSrGbValue
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrV6MinSrGbValue (UINT4 *pu4ErrorCode,
                             UINT4 u4TestValFsSrV6MinSrGbValue)
{
    INT4                i4SrStatus = SR_DISABLED;
    UINT4               u4v4MinSrgbVal = SR_DISABLED;
    UINT4               u4v4MaxSrgbVal = SR_DISABLED;
    UINT4               isNodeSidPresent = SR_FALSE;
    tSrSidInterfaceInfo *pSrSidIntf = NULL;

    nmhGetFsSrV6Status (&i4SrStatus);

    if (i4SrStatus == SR_ENABLED)
    {
        *pu4ErrorCode = SNMP_ERR_AUTHORIZATION_ERROR;
        CLI_SET_ERR (CLI_SR_CONFIG_FAIL);
        return SNMP_FAILURE;

    }

    if ((u4TestValFsSrV6MinSrGbValue <
         gSystemSize.MplsSystemSize.u4MinSrLblRange)
        || (u4TestValFsSrV6MinSrGbValue >=
            gSystemSize.MplsSystemSize.u4MaxSrLblRange))
    {
        *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
        CLI_SET_ERR (CLI_SR_INV_VAL);
        return SNMP_FAILURE;
    }

    if (((gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MaxIndex != 0) &&
         (IS_V6_SRGB_CONFIGURED == SR_TRUE) &&
         (SR_V6_SRGB_MAX_VAL <= u4TestValFsSrV6MinSrGbValue)) ||
        ((IS_V6_SRGB_CONFIGURED == SR_TRUE) &&
         ((INT4) (SR_V6_SRGB_MAX_VAL - u4TestValFsSrV6MinSrGbValue) >
          MAX_SRGB_SIZE)))
    {
        *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
        CLI_SET_ERR (CLI_SR_INV_VAL);
        return SNMP_FAILURE;
    }

    nmhGetFsSrV4MinSrGbValue (&u4v4MinSrgbVal);
    nmhGetFsSrV4MaxSrGbValue (&u4v4MaxSrgbVal);
    if ((u4v4MinSrgbVal != SR_ZERO) && (u4v4MaxSrgbVal != SR_ZERO) &&
        (u4TestValFsSrV6MinSrGbValue >= u4v4MinSrgbVal) &&
        (u4TestValFsSrV6MinSrGbValue <= u4v4MaxSrgbVal))
    {
        *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
        CLI_SET_ERR (CLI_SR_V6_INV_RANGE);
        return SNMP_FAILURE;
    }

    pSrSidIntf = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);

    while (pSrSidIntf != NULL)
    {
        if (pSrSidIntf->u4SidTypeV3 != SR_SID_NODE)
        {
            if ((IS_V6_SRGB_CONFIGURED == SR_TRUE) &&
                pSrSidIntf->u4PrefixSidLabelIndex >= u4TestValFsSrV6MinSrGbValue
                && pSrSidIntf->u4PrefixSidLabelIndex <= SR_V6_SRGB_MAX_VAL)
            {
                *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
                CLI_SET_ERR (CLI_SR_INV_VAL);
                return SNMP_FAILURE;
            }
            else
            {
                pSrSidIntf =
                    RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree, pSrSidIntf,
                                   NULL);
            }
        }
        else
        {
            isNodeSidPresent = SR_TRUE;
            break;
        }
    }

    if (isNodeSidPresent == SR_TRUE)
    {
        *pu4ErrorCode = SNMP_ERR_AUTHORIZATION_ERROR;
        CLI_SET_ERR (CLI_SR_NODE_SID_EXIST);
        return SNMP_FAILURE;
    }

    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhTestv2FsSrV6MaxSrGbValue
 Input       :  The Indices

                The Object 
                testValFsSrV6MaxSrGbValue
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrV6MaxSrGbValue (UINT4 *pu4ErrorCode,
                             UINT4 u4TestValFsSrV6MaxSrGbValue)
{
    INT4                i4SrStatus = SR_DISABLED;
    UINT4               u4v4MinSrgbVal = SR_ZERO;
    UINT4               u4v4MaxSrgbVal = SR_ZERO;
    UINT4               isNodeSidPresent = SR_FALSE;
    tSrSidInterfaceInfo *pSrSidIntf = NULL;

    nmhGetFsSrV6Status (&i4SrStatus);

    if (i4SrStatus == SR_ENABLED)
    {
        *pu4ErrorCode = SNMP_ERR_AUTHORIZATION_ERROR;
        CLI_SET_ERR (CLI_SR_CONFIG_FAIL);
        return SNMP_FAILURE;
    }

    if ((u4TestValFsSrV6MaxSrGbValue <=
         gSystemSize.MplsSystemSize.u4MinSrLblRange)
        || (u4TestValFsSrV6MaxSrGbValue >
            gSystemSize.MplsSystemSize.u4MaxSrLblRange))
    {
        *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
        CLI_SET_ERR (CLI_SR_INV_VAL);
        return SNMP_FAILURE;
    }

    if (((gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex != 0) &&
         (IS_V6_SRGB_CONFIGURED == SR_TRUE) &&
         (SR_V6_SRGB_MIN_VAL >= u4TestValFsSrV6MaxSrGbValue)) ||
        ((IS_V6_SRGB_CONFIGURED == SR_TRUE) &&
         ((INT4) (u4TestValFsSrV6MaxSrGbValue - SR_V6_SRGB_MIN_VAL) >
          MAX_SRGB_SIZE)))
    {
        *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
        CLI_SET_ERR (CLI_SR_INV_VAL);
        return SNMP_FAILURE;
    }

    nmhGetFsSrV4MinSrGbValue (&u4v4MinSrgbVal);
    nmhGetFsSrV4MaxSrGbValue (&u4v4MaxSrgbVal);
    if ((u4v4MinSrgbVal != SR_ZERO) && (u4v4MaxSrgbVal != SR_ZERO) &&
        (u4TestValFsSrV6MaxSrGbValue >= u4v4MinSrgbVal) &&
        (u4TestValFsSrV6MaxSrGbValue <= u4v4MaxSrgbVal))
    {
        *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
        CLI_SET_ERR (CLI_SR_V6_INV_RANGE);
        return SNMP_FAILURE;
    }

    pSrSidIntf = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);
    while (pSrSidIntf != NULL)
    {
        if (pSrSidIntf->u4SidTypeV3 != SR_SID_NODE)
        {
            if ((IS_V6_SRGB_CONFIGURED == SR_TRUE) &&
                pSrSidIntf->u4PrefixSidLabelIndex <= u4TestValFsSrV6MaxSrGbValue
                && pSrSidIntf->u4PrefixSidLabelIndex >= SR_V6_SRGB_MIN_VAL)
            {
                *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
                CLI_SET_ERR (CLI_SR_INV_VAL);
                return SNMP_FAILURE;
            }
            else
            {
                pSrSidIntf =
                    RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree, pSrSidIntf,
                                   NULL);
            }
        }
        else
        {
            isNodeSidPresent = SR_TRUE;
            break;
        }
    }

    if (isNodeSidPresent == SR_TRUE)
    {
        *pu4ErrorCode = SNMP_ERR_AUTHORIZATION_ERROR;
        CLI_SET_ERR (CLI_SR_NODE_SID_EXIST);
        return SNMP_FAILURE;
    }

    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhTestv2FsSrV6AlternateStatus
 Input       :  The Indices

                The Object 
                testValFsSrV6AlternateStatus
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrV6AlternateStatus (UINT4 *pu4ErrorCode,
                                INT4 i4TestValFsSrV6AlternateStatus)
{
    INT4                i4AltStatus = ALTERNATE_DISABLED;
    INT4                i4SrMode = SR_MODE_DEFAULT;

#if 0
    if (i4TestValFsSrV6AlternateStatus == ALTERNATE_ENABLED)
    {
        /*Check the SR and LFA status at OSPF level */
        nmhGetFsSrMode (&i4SrMode);
        if ((i4SrMode & SR_CONF_OSPFV3) != SR_CONF_OSPFV3)
        {
            *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
            return SNMP_FAILURE;
        }
    }
#endif

    nmhGetFsSrV6AlternateStatus (&i4AltStatus);
    if (i4AltStatus == i4TestValFsSrV6AlternateStatus)
    {
        return SNMP_SUCCESS;
    }

    switch (i4TestValFsSrV6AlternateStatus)
    {
        case ALTERNATE_ENABLED:
        case ALTERNATE_DISABLED:
            break;

        default:
            *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
            return SNMP_FAILURE;
    }
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhTestv2FsSrV6Status
 Input       :  The Indices

                The Object 
                testValFsSrV6Status
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrV6Status (UINT4 *pu4ErrorCode, INT4 i4TestValFsSrV6Status)
{
    if ((i4TestValFsSrV6Status == SR_ENABLE) ||
        (i4TestValFsSrV6Status == SR_DISABLE))
    {
        return SNMP_SUCCESS;
    }
    *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
    CLI_SET_ERR (CLI_SR_INV_VAL);
    return SNMP_FAILURE;
}

/****************************************************************************
 Function    :  nmhTestv2FsSrGlobalAdminStatus
 Input       :  The Indices

                The Object 
                testValFsSrGlobalAdminStatus
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrGlobalAdminStatus (UINT4 *pu4ErrorCode,
                                INT4 i4TestValFsSrGlobalAdminStatus)
{
    if ((i4TestValFsSrGlobalAdminStatus == SR_CLI_ADMIN_UP) ||
        (i4TestValFsSrGlobalAdminStatus == SR_CLI_ADMIN_DOWN))
    {
        return SNMP_SUCCESS;
    }
    *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
    CLI_SET_ERR (CLI_SR_INV_GBL_ADMIN_VAL);
    return SNMP_FAILURE;
}

/****************************************************************************
 Function    :  nmhTestv2FsSrIpv4AddrFamily
 Input       :  The Indices

                The Object 
                testValFsSrIpv4AddrFamily
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrIpv4AddrFamily (UINT4 *pu4ErrorCode,
                             INT4 i4TestValFsSrIpv4AddrFamily)
{
    if ((i4TestValFsSrIpv4AddrFamily == SR_ENABLE) ||
        (i4TestValFsSrIpv4AddrFamily == SR_DISABLE))
    {
        return SNMP_SUCCESS;
    }
    CLI_SET_ERR (CLI_SR_INV_IPV4_MODE_VAL);
    *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
    return SNMP_FAILURE;
}

/****************************************************************************
 Function    :  nmhTestv2FsSrIpv6AddrFamily
 Input       :  The Indices

                The Object 
                testValFsSrIpv6AddrFamily
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsSrIpv6AddrFamily (UINT4 *pu4ErrorCode,
                             INT4 i4TestValFsSrIpv6AddrFamily)
{
    if ((i4TestValFsSrIpv6AddrFamily == SR_ENABLE) ||
        (i4TestValFsSrIpv6AddrFamily == SR_DISABLE))
    {
        return SNMP_SUCCESS;
    }
    CLI_SET_ERR (CLI_SR_INV_IPV6_MODE_VAL);
    *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
    return SNMP_FAILURE;
}

/* Low Level Dependency Routines for All Objects  */

/****************************************************************************
 Function    :  nmhDepv2FsSrV4Status
 Output      :  The Dependency Low Lev Routine Take the Indices &
                check whether dependency is met or not.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhDepv2FsSrV4Status (UINT4 *pu4ErrorCode, tSnmpIndexList * pSnmpIndexList,
                      tSNMP_VAR_BIND * pSnmpVarBind)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (pSnmpIndexList);
    UNUSED_PARAM (pSnmpVarBind);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhDepv2FsSrV4AlternateStatus
 Output      :  The Dependency Low Lev Routine Take the Indices &
                check whether dependency is met or not.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhDepv2FsSrV4AlternateStatus (UINT4 *pu4ErrorCode,
                               tSnmpIndexList * pSnmpIndexList,
                               tSNMP_VAR_BIND * pSnmpVarBind)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (pSnmpIndexList);
    UNUSED_PARAM (pSnmpVarBind);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhDepv2FsSrV6MinSrGbValue
 Output      :  The Dependency Low Lev Routine Take the Indices &
                check whether dependency is met or not.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhDepv2FsSrV6MinSrGbValue (UINT4 *pu4ErrorCode,
                            tSnmpIndexList * pSnmpIndexList,
                            tSNMP_VAR_BIND * pSnmpVarBind)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (pSnmpIndexList);
    UNUSED_PARAM (pSnmpVarBind);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhDepv2FsSrV6MaxSrGbValue
 Output      :  The Dependency Low Lev Routine Take the Indices &
                check whether dependency is met or not.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhDepv2FsSrV6MaxSrGbValue (UINT4 *pu4ErrorCode,
                            tSnmpIndexList * pSnmpIndexList,
                            tSNMP_VAR_BIND * pSnmpVarBind)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (pSnmpIndexList);
    UNUSED_PARAM (pSnmpVarBind);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhDepv2FsSrV6AlternateStatus
 Output      :  The Dependency Low Lev Routine Take the Indices &
                check whether dependency is met or not.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhDepv2FsSrV6AlternateStatus (UINT4 *pu4ErrorCode,
                               tSnmpIndexList * pSnmpIndexList,
                               tSNMP_VAR_BIND * pSnmpVarBind)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (pSnmpIndexList);
    UNUSED_PARAM (pSnmpVarBind);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhDepv2FsSrV6Status
 Output      :  The Dependency Low Lev Routine Take the Indices &
                check whether dependency is met or not.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhDepv2FsSrV6Status (UINT4 *pu4ErrorCode, tSnmpIndexList * pSnmpIndexList,
                      tSNMP_VAR_BIND * pSnmpVarBind)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (pSnmpIndexList);
    UNUSED_PARAM (pSnmpVarBind);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhDepv2FsSrGlobalAdminStatus
 Output      :  The Dependency Low Lev Routine Take the Indices &
                check whether dependency is met or not.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhDepv2FsSrGlobalAdminStatus (UINT4 *pu4ErrorCode,
                               tSnmpIndexList * pSnmpIndexList,
                               tSNMP_VAR_BIND * pSnmpVarBind)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (pSnmpIndexList);
    UNUSED_PARAM (pSnmpVarBind);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhDepv2FsSrIpv4AddrFamily
 Output      :  The Dependency Low Lev Routine Take the Indices &
                check whether dependency is met or not.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhDepv2FsSrIpv4AddrFamily (UINT4 *pu4ErrorCode,
                            tSnmpIndexList * pSnmpIndexList,
                            tSNMP_VAR_BIND * pSnmpVarBind)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (pSnmpIndexList);
    UNUSED_PARAM (pSnmpVarBind);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhDepv2FsSrIpv6AddrFamily
 Output      :  The Dependency Low Lev Routine Take the Indices &
                check whether dependency is met or not.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhDepv2FsSrIpv6AddrFamily (UINT4 *pu4ErrorCode,
                            tSnmpIndexList * pSnmpIndexList,
                            tSNMP_VAR_BIND * pSnmpVarBind)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (pSnmpIndexList);
    UNUSED_PARAM (pSnmpVarBind);
    return SNMP_SUCCESS;
}

/* Low Level GET Routine for All Objects  */

/****************************************************************************
 *  Function    :  nmhGetFsStaticSrNextHopAddr
 *  Input       :  The Indices
 *                 FsStaticSrDestAddr
 *                 FsStaticSrDestMask
 *                 FsStaticSrDestAddrType
 *
 *                 The Object
 *                 retValFsStaticSrNextHopAddr
 *  Output      :  The Get Low Lev Routine Take the Indices &
 *                 store the Value requested in the Return val.
 *  Returns     :  SNMP_SUCCESS or SNMP_FAILURE
 ****************************************************************************/
INT1
nmhGetFsStaticSrNextHopAddr (tSNMP_OCTET_STRING_TYPE * pFsStaticSrDestAddr,
                             UINT4 u4FsStaticSrDestMask,
                             INT4 i4FsStaticSrDestAddrType,
                             UINT4 u4FsStaticSrLspId,
                             tSNMP_OCTET_STRING_TYPE *
                             pRetValFsStaticSrNextHopAddr)
{
    tSrStaticEntryInfo *pSrStaticInfo = NULL;
    tSrStaticEntryInfo  SrStaticInfo;

    MEMSET (&SrStaticInfo, SR_ZERO, sizeof (tSrStaticEntryInfo));

    if (gSrGlobalInfo.pSrStaticRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrStaticRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    SR_OCTETSTRING_TO_INTEGER (pFsStaticSrDestAddr,
                               SrStaticInfo.destAddr.Addr.u4Addr);

    SrStaticInfo.u4DestMask = u4FsStaticSrDestMask;
    SrStaticInfo.destAddr.u2AddrType = (UINT2) i4FsStaticSrDestAddrType;
    SrStaticInfo.u4LspId = u4FsStaticSrLspId;

    pSrStaticInfo = RBTreeGet (gSrGlobalInfo.pSrStaticRbTree, &SrStaticInfo);
    if (pSrStaticInfo == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGet FAILED\n", __func__,
                __LINE__);
        return SNMP_FAILURE;
    }

    SR_INTEGER_TO_OCTETSTRING (pSrStaticInfo->NextHop.Addr.u4Addr,
                               pRetValFsStaticSrNextHopAddr);
    return SNMP_SUCCESS;

}

/****************************************************************************
 *  Function    :  nmhGetFsStaticSrLabelStackIndex
 *  Input       :  The Indices
 *                 FsStaticSrDestAddr
 *                 FsStaticSrDestMask
 *                 FsStaticSrDestAddrType
 *
 *                 The Object
 *                 retValFsStaticSrLabelStackIndex
 *  Output      :  The Get Low Lev Routine Take the Indices &
 *                 store the Value requested in the Return val.
 *  Returns     :  SNMP_SUCCESS or SNMP_FAILURE
 *****************************************************************************/
INT1
nmhGetFsStaticSrLabelStackIndex (tSNMP_OCTET_STRING_TYPE * pFsStaticSrDestAddr,
                                 UINT4 u4FsStaticSrDestMask,
                                 INT4 i4FsStaticSrDestAddrType,
                                 UINT4 u4FsStaticSrLspId,
                                 UINT4 *pu4RetValFsStaticSrLabelStackIndex)
{
    tSrStaticEntryInfo *pSrStaticInfo = NULL;
    tSrStaticEntryInfo  SrStaticInfo;

    MEMSET (&SrStaticInfo, SR_ZERO, sizeof (tSrStaticEntryInfo));

    if (gSrGlobalInfo.pSrStaticRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrStaticRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    SR_OCTETSTRING_TO_INTEGER (pFsStaticSrDestAddr,
                               SrStaticInfo.destAddr.Addr.u4Addr);

    SrStaticInfo.u4DestMask = u4FsStaticSrDestMask;
    SrStaticInfo.destAddr.u2AddrType = (UINT2) i4FsStaticSrDestAddrType;
    SrStaticInfo.u4LspId = u4FsStaticSrLspId;

    pSrStaticInfo = RBTreeGet (gSrGlobalInfo.pSrStaticRbTree, &SrStaticInfo);

    if (pSrStaticInfo == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGet Failed\n", __func__,
                __LINE__);
        return SNMP_FAILURE;
    }

    *pu4RetValFsStaticSrLabelStackIndex =
        pSrStaticInfo->srStaticLblListIndex.u4Index;
    return SNMP_SUCCESS;

}

/****************************************************************************
 *  Function    :  nmhGetFsStaticSrLabelStackLabel
 *  Input       :  The Indices
 *                 FsStaticLabelStackIndex
 *                 FsStaticSrLabelIndex
 *
 *                 The Object
 *                 retValFsStaticSrLabelStackLabel
 *  Output      :  The Get Low Lev Routine Take the Indices &
 *                 store the Value requested in the Return val.
 *  Returns     :  SNMP_SUCCESS or SNMP_FAILURE
 *****************************************************************************/
INT1
nmhGetFsStaticSrLabelStackLabel (UINT4 u4FsStaticLabelStackIndex,
                                 UINT4 u4FsStaticSrLabelIndex,
                                 UINT4 *pu4RetValFsStaticSrLabelStackLabel)
{
    tSrStaticEntryInfo *pSrStaticInfo = NULL;
    tTMO_SLL_NODE      *pSrStaticOptLbl = NULL;
    tStaticSrLbl       *pSrStaticLbl = NULL;

    if (gSrGlobalInfo.pSrStaticRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrStaticRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    if ((pSrStaticInfo =
         RBTreeGetFirst (gSrGlobalInfo.pSrStaticRbTree)) == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGetFirst FAILED\n", __func__,
                 __LINE__);
        return SNMP_FAILURE;
    }

    while (pSrStaticInfo != NULL)
    {
        if (pSrStaticInfo->srStaticLblListIndex.u4Index ==
            u4FsStaticLabelStackIndex)
        {
            TMO_SLL_Scan (&(pSrStaticInfo->srStaticLblListIndex.StaticLblList),
                          pSrStaticOptLbl, tTMO_SLL_NODE *)
            {
                pSrStaticLbl = (tStaticSrLbl *) pSrStaticOptLbl;
                if (pSrStaticLbl->u4LblIndex == u4FsStaticSrLabelIndex)
                {
                    *pu4RetValFsStaticSrLabelStackLabel = pSrStaticLbl->u4Label;
                    return SNMP_SUCCESS;
                }
            }
            *pu4RetValFsStaticSrLabelStackLabel = 0;
            SR_TRC3 (SR_MGMT_TRC,
                     "%s:%d No entry found with StackLabel index: %d\n",
                     __func__, __LINE__, u4FsStaticLabelStackIndex);
            return SNMP_SUCCESS;
        }
        pSrStaticInfo =
            RBTreeGetNext (gSrGlobalInfo.pSrStaticRbTree, pSrStaticInfo, NULL);
    }
    SR_TRC2 (SR_MGMT_TRC, "%s:%d No such entry present \n", __func__,
             __LINE__);
    return SNMP_SUCCESS;
}

/****************************************************************************
 *  Function    :  nmhSetFsStaticSrLabelStackLabel
 *  Input       :  The Indices
 *                 FsStaticLabelStackIndex
 *                 FsStaticSrLabelIndex
 *
 *                 The Object
 *                 setValFsStaticSrLabelStackLabel
 *  Output      :  The Set Low Lev Routine Take the Indices &
 *                 Sets the Value accordingly.
 *  Returns     :  SNMP_SUCCESS or SNMP_FAILURE
 *****************************************************************************/
INT1
nmhSetFsStaticSrLabelStackLabel (UINT4 u4FsStaticLabelStackIndex,
                                 UINT4 u4FsStaticSrLabelIndex,
                                 UINT4 u4SetValFsStaticSrLabelStackLabel)
{
    tSrStaticEntryInfo *pSrStaticInfo = NULL;
    tTMO_SLL_NODE      *pSrStaticOptLbl = NULL;
    tStaticSrLbl       *pSrStaticLbl = NULL;

    if (gSrGlobalInfo.pSrStaticRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrStaticRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    if ((pSrStaticInfo =
         RBTreeGetFirst (gSrGlobalInfo.pSrStaticRbTree)) == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGetFirst FAILED\n", __func__,
                 __LINE__);
        return SNMP_FAILURE;
    }

    while (pSrStaticInfo != NULL)
    {
        if (pSrStaticInfo->srStaticLblListIndex.u4Index ==
            u4FsStaticLabelStackIndex)
        {
            TMO_SLL_Scan (&(pSrStaticInfo->srStaticLblListIndex.StaticLblList),
                          pSrStaticOptLbl, tTMO_SLL_NODE *)
            {
                pSrStaticLbl = (tStaticSrLbl *) pSrStaticOptLbl;
                if (pSrStaticLbl->u4LblIndex == u4FsStaticSrLabelIndex)
                {
                    pSrStaticLbl = NULL;
                    break;
                }
                pSrStaticLbl = NULL;
            }

            pSrStaticLbl =
                SrCreateStaticSrLblEntry (u4FsStaticLabelStackIndex,
                                          u4FsStaticSrLabelIndex);
            if (pSrStaticLbl == NULL)
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC,
                         "%s:%d Error while creating Static Sr Label\n",
                         __func__, __LINE__);
                return SNMP_FAILURE;
            }
            pSrStaticLbl->u4Label = u4SetValFsStaticSrLabelStackLabel;

            if (u4FsStaticSrLabelIndex == SR_ZERO) {
                pSrStaticInfo->SrStaticLblStack.u4TopLabel = u4SetValFsStaticSrLabelStackLabel;
            }
            pSrStaticInfo->SrStaticLblStack.u4LabelStack[u4FsStaticSrLabelIndex] = u4SetValFsStaticSrLabelStackLabel;
            if ((pSrStaticInfo->SrStaticLblStack.u1StackSize) < (u4FsStaticSrLabelIndex + 1)) {
                pSrStaticInfo->SrStaticLblStack.u1StackSize = u4FsStaticSrLabelIndex + 1;
            }

            return SNMP_SUCCESS;
        }
        pSrStaticInfo =
            RBTreeGetNext (gSrGlobalInfo.pSrStaticRbTree, pSrStaticInfo, NULL);
    }
    SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d No such entry present \n", __func__,
             __LINE__);
    return SNMP_FAILURE;
}

/****************************************************************************
 Function    :  nmhTestv2FsStaticSrLabelStackLabel
 Input       :  The Indices
                FsStaticLabelStackIndex
                FsStaticSrLabelIndex

                The Object
                testValFsStaticSrLabelStackLabel
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsStaticSrLabelStackLabel (UINT4 *pu4ErrorCode,
                                    UINT4 u4FsStaticLabelStackIndex,
                                    UINT4 u4FsStaticSrLabelIndex,
                                    UINT4 u4TestValFsStaticSrLabelStackLabel)
{
    tSrStaticEntryInfo *pSrStaticInfo = NULL;
    tTMO_SLL_NODE      *pSrStaticOptLbl = NULL;
    tStaticSrLbl       *pSrStaticLbl = NULL;

    UNUSED_PARAM (pu4ErrorCode);

    if (gSrGlobalInfo.pSrStaticRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrStaticRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    if ((pSrStaticInfo =
         RBTreeGetFirst (gSrGlobalInfo.pSrStaticRbTree)) == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGetFirst FAILED\n", __func__,
                 __LINE__);
        return SNMP_FAILURE;
    }

    while (pSrStaticInfo != NULL)
    {
        if (pSrStaticInfo->srStaticLblListIndex.u4Index ==
            u4FsStaticLabelStackIndex)
        {
            TMO_SLL_Scan (&(pSrStaticInfo->srStaticLblListIndex.StaticLblList),
                          pSrStaticOptLbl, tTMO_SLL_NODE *)
            {
                pSrStaticLbl = (tStaticSrLbl *) pSrStaticOptLbl;
                if (pSrStaticLbl->u4LblIndex == u4FsStaticSrLabelIndex)
                {
                    pSrStaticLbl = NULL;
                    break;
                }
                pSrStaticLbl = NULL;
            }
            if ((pSrStaticLbl == NULL)
                && (u4TestValFsStaticSrLabelStackLabel > 0)
                && (u4TestValFsStaticSrLabelStackLabel >= SR_GEN_MIN_LBL)
                && (u4TestValFsStaticSrLabelStackLabel <= SR_GEN_MAX_LBL))
            {
                return SNMP_SUCCESS;
            }
            else
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d Lable Out of range\n", __func__,
                         __LINE__);
                return SNMP_FAILURE;
            }
        }
        pSrStaticInfo =
            RBTreeGetNext (gSrGlobalInfo.pSrStaticRbTree, pSrStaticInfo, NULL);
    }
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhDepv2FsStaticSrLabelStackTable
 Input       :  The Indices
                FsStaticLabelStackIndex
                FsStaticSrLabelIndex
 Output      :  The Dependency Low Lev Routine Take the Indices &
                check whether dependency is met or not.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhDepv2FsStaticSrLabelStackTable (UINT4 *pu4ErrorCode,
                                   tSnmpIndexList * pSnmpIndexList,
                                   tSNMP_VAR_BIND * pSnmpVarBind)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (pSnmpIndexList);
    UNUSED_PARAM (pSnmpVarBind);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhValidateIndexInstanceFsStaticSrTable
 Input       :  The Indices
                FsStaticSrDestAddr
                FsStaticSrDestMask
                FsStaticSrDestAddrType
 Output      :  The Routines Validates the Given Indices.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
/* GET_EXACT Validate Index Instance Routine. */

INT1
nmhValidateIndexInstanceFsStaticSrTable (tSNMP_OCTET_STRING_TYPE *
                                         pFsStaticSrDestAddr,
                                         UINT4 u4FsStaticSrDestMask,
                                         INT4 i4FsStaticSrDestAddrType,
                                         UINT4 u4FsStaticSrLspId)
{
    UNUSED_PARAM (pFsStaticSrDestAddr);
    UNUSED_PARAM (u4FsStaticSrDestMask);
    UNUSED_PARAM (i4FsStaticSrDestAddrType);
    UNUSED_PARAM (u4FsStaticSrLspId);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFirstIndexFsStaticSrTable
 Input       :  The Indices
                FsStaticSrDestAddr
                FsStaticSrDestMask
                FsStaticSrDestAddrType
 Output      :  The Get First Routines gets the Lexicographicaly
                First Entry from the Table.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
/* GET_FIRST Routine. */

INT1
nmhGetFirstIndexFsStaticSrTable (tSNMP_OCTET_STRING_TYPE * pFsStaticSrDestAddr,
                                 UINT4 *pu4FsStaticSrDestMask,
                                 INT4 *pi4FsStaticSrDestAddrType,
                                 UINT4 *pu4FsStaticSrLspId)
{
    tSrStaticEntryInfo *pSrStaticInfo = NULL;

    if (gSrGlobalInfo.pSrStaticRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrStaticRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    if ((pSrStaticInfo =
         RBTreeGetFirst (gSrGlobalInfo.pSrStaticRbTree)) == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGetFirst FAILED\n", __func__,
                 __LINE__);
        return SNMP_FAILURE;
    }

    if (pSrStaticInfo->destAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        SR_INTEGER_TO_OCTETSTRING (pSrStaticInfo->destAddr.Addr.u4Addr,
                                   pFsStaticSrDestAddr);
    }

    *pu4FsStaticSrDestMask = pSrStaticInfo->u4DestMask;
    *pi4FsStaticSrDestAddrType = pSrStaticInfo->destAddr.u2AddrType;;
    *pu4FsStaticSrLspId = pSrStaticInfo->u4LspId;
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetNextIndexFsStaticSrTable
 Input       :  The Indices
                FsStaticSrDestAddr
                nextFsStaticSrDestAddr
                FsStaticSrDestMask
                nextFsStaticSrDestMask
                FsStaticSrDestAddrType
                nextFsStaticSrDestAddrType
 Output      :  The Get Next function gets the Next Index for
                the Index Value given in the Index Values. The
                Indices are stored in the next_varname variables.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
/* GET_NEXT Routine.  */
INT1
nmhGetNextIndexFsStaticSrTable (tSNMP_OCTET_STRING_TYPE * pFsStaticSrDestAddr,
                                tSNMP_OCTET_STRING_TYPE *
                                pNextFsStaticSrDestAddr,
                                UINT4 u4FsStaticSrDestMask,
                                UINT4 *pu4NextFsStaticSrDestMask,
                                INT4 i4FsStaticSrDestAddrType,
                                INT4 *pi4NextFsStaticSrDestAddrType,
                                UINT4 u4FsStaticSrLspId,
                                UINT4 *pu4NextFsStaticSrLspId)
{
    tSrStaticEntryInfo *pSrStaticInfo = NULL;
    tSrStaticEntryInfo  SrStaticInfo;
    tGenU4Addr          dummyAddr;

    MEMSET (&SrStaticInfo, SR_ZERO, sizeof (tSrStaticEntryInfo));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));

    if (gSrGlobalInfo.pSrStaticRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrStaticRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    if (i4FsStaticSrDestAddrType == SR_IPV4_ADDR_TYPE)
    {
        SR_OCTETSTRING_TO_INTEGER (pFsStaticSrDestAddr,
                                   SrStaticInfo.destAddr.Addr.u4Addr);

        if (SrStaticInfo.destAddr.Addr.u4Addr == SR_ZERO)
        {
            SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d Destination router is 0.", __func__,
                     __LINE__);
            return SNMP_FAILURE;
        }
    }

    SrStaticInfo.u4DestMask = u4FsStaticSrDestMask;
    SrStaticInfo.destAddr.u2AddrType = (UINT2) i4FsStaticSrDestAddrType;
    SrStaticInfo.u4LspId = u4FsStaticSrLspId;

    pSrStaticInfo =
        RBTreeGetNext (gSrGlobalInfo.pSrStaticRbTree, &SrStaticInfo, NULL);

    if (pSrStaticInfo == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGetNext FAILED", __func__,
                __LINE__);
        return SNMP_FAILURE;
    }

    if (pSrStaticInfo->destAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        SR_INTEGER_TO_OCTETSTRING (pSrStaticInfo->destAddr.Addr.u4Addr,
                                   pNextFsStaticSrDestAddr);
    }

    *pu4NextFsStaticSrDestMask = pSrStaticInfo->u4DestMask;
    *pi4NextFsStaticSrDestAddrType = pSrStaticInfo->destAddr.u2AddrType;
    *pu4NextFsStaticSrLspId = pSrStaticInfo->u4LspId;
    return SNMP_SUCCESS;
}

/****************************************************************************
 *  Function    :  nmhSetFsStaticSrLabelStackIndex
 *  Input       :  The Indices
 *                 FsStaticSrDestAddr
 *                 FsStaticSrDestMask
 *                 FsStaticSrDestAddrType
 *
 *                 The Object
 *                 setValFsStaticSrLabelStackIndex
 *  Output      :  The Set Low Lev Routine Take the Indices &
 *                 Sets the Value accordingly.
 *  Returns     :  SNMP_SUCCESS or SNMP_FAILURE
 ****************************************************************************/
INT1
nmhSetFsStaticSrLabelStackIndex (tSNMP_OCTET_STRING_TYPE * pFsStaticSrDestAddr,
                                 UINT4 u4FsStaticSrDestMask,
                                 INT4 i4FsStaticSrDestAddrType,
                                 UINT4 u4FsStaticSrLspId,
                                 UINT4 u4SetValFsStaticSrLabelStackIndex)
{
    tSrStaticEntryInfo *pSrStaticInfo = NULL;
    tSrStaticEntryInfo  SrStaticInfo;

    MEMSET (&SrStaticInfo, SR_ZERO, sizeof (tSrStaticEntryInfo));

    if (gSrGlobalInfo.pSrStaticRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrStaticRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    SR_OCTETSTRING_TO_INTEGER (pFsStaticSrDestAddr,
                               SrStaticInfo.destAddr.Addr.u4Addr);

    SrStaticInfo.u4DestMask = u4FsStaticSrDestMask;
    SrStaticInfo.destAddr.u2AddrType = (UINT2) i4FsStaticSrDestAddrType;
    SrStaticInfo.u4LspId = u4FsStaticSrLspId;

    pSrStaticInfo = RBTreeGet (gSrGlobalInfo.pSrStaticRbTree, &SrStaticInfo);

    if (pSrStaticInfo == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGet Failed\n", __func__,
                __LINE__);
        return SNMP_FAILURE;
    }

    pSrStaticInfo->srStaticLblListIndex.u4Index =
        u4SetValFsStaticSrLabelStackIndex;
    return SNMP_SUCCESS;
}

/****************************************************************************
 *  Function    :  nmhSetFsStaticSrNextHopAddr
 *  Input       :  The Indices
 *                 FsStaticSrDestAddr
 *                 FsStaticSrDestMask
 *                 FsStaticSrDestAddrType
 *
 *                 The Object
 *                 setValFsStaticSrNextHopAddr
 *  Output      :  The Set Low Lev Routine Take the Indices &
 *                 Sets the Value accordingly.
 *  Returns     :  SNMP_SUCCESS or SNMP_FAILURE
 *****************************************************************************/
INT1
nmhSetFsStaticSrNextHopAddr (tSNMP_OCTET_STRING_TYPE * pFsStaticSrDestAddr,
                             UINT4 u4FsStaticSrDestMask,
                             INT4 i4FsStaticSrDestAddrType,
                             UINT4 u4FsStaticSrLspId,
                             tSNMP_OCTET_STRING_TYPE *
                             pSetValFsStaticSrNextHopAddr)
{
    tSrStaticEntryInfo *pSrStaticInfo = NULL;
    tSrStaticEntryInfo  SrStaticInfo;

    MEMSET (&SrStaticInfo, SR_ZERO, sizeof (tSrStaticEntryInfo));

    if (gSrGlobalInfo.pSrStaticRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrStaticRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    SR_OCTETSTRING_TO_INTEGER (pFsStaticSrDestAddr,
                               SrStaticInfo.destAddr.Addr.u4Addr);

    SrStaticInfo.u4DestMask = u4FsStaticSrDestMask;
    SrStaticInfo.destAddr.u2AddrType = (UINT2) i4FsStaticSrDestAddrType;
    SrStaticInfo.u4LspId = u4FsStaticSrLspId;

    pSrStaticInfo = RBTreeGet (gSrGlobalInfo.pSrStaticRbTree, &SrStaticInfo);
    if (pSrStaticInfo == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGet Failed\n", __func__,
                __LINE__);
        return SNMP_FAILURE;
    }
    SR_OCTETSTRING_TO_INTEGER (pSetValFsStaticSrNextHopAddr,
                               pSrStaticInfo->NextHop.Addr.u4Addr);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhValidateIndexInstanceFsStaticSrLabelStackTable
 Input       :  The Indices
                FsStaticLabelStackIndex
                FsStaticSrLabelIndex
 Output      :  The Routines Validates the Given Indices.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
/* GET_EXACT Validate Index Instance Routine. */

INT1
nmhValidateIndexInstanceFsStaticSrLabelStackTable (UINT4
                                                   u4FsStaticLabelStackIndex,
                                                   UINT4 u4FsStaticSrLabelIndex)
{
    if (((u4FsStaticLabelStackIndex > 0)
         && (u4FsStaticLabelStackIndex <= SR_MAX_STATIC_ENTRIES))
        && (u4FsStaticSrLabelIndex <= SR_MAX_STATICLBL_ENTRIES))
    {
        return SNMP_SUCCESS;
    }
    return SNMP_FAILURE;
}

/****************************************************************************
 Function    :  nmhGetFirstIndexFsStaticSrLabelStackTable
 Input       :  The Indices
                FsStaticLabelStackIndex
                FsStaticSrLabelIndex
 Output      :  The Get First Routines gets the Lexicographicaly
                First Entry from the Table.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
/* GET_FIRST Routine. */

INT1
nmhGetFirstIndexFsStaticSrLabelStackTable (UINT4 *pu4FsStaticLabelStackIndex,
                                           UINT4 *pu4FsStaticSrLabelIndex)
{
    tSrStaticEntryInfo *pSrStaticInfo = NULL;
    tStaticSrLbl       *pSrStaticLblInfo = NULL;

    if (gSrGlobalInfo.pSrStaticRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrStaticRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    if ((pSrStaticInfo =
         RBTreeGetFirst (gSrGlobalInfo.pSrStaticRbTree)) == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGetFirst FAILED\n", __func__,
                 __LINE__);
        return SNMP_FAILURE;
    }

    pSrStaticLblInfo =
        (tStaticSrLbl *)
        TMO_SLL_First (&(pSrStaticInfo->srStaticLblListIndex.StaticLblList));

    if (pSrStaticLblInfo != NULL)
    {
        *pu4FsStaticLabelStackIndex =
            pSrStaticInfo->srStaticLblListIndex.u4Index;
        *pu4FsStaticSrLabelIndex = pSrStaticLblInfo->u4LblIndex;
        return SNMP_SUCCESS;
    }
    return SNMP_FAILURE;
}

/****************************************************************************
 Function    :  nmhGetNextIndexFsStaticSrLabelStackTable
 Input       :  The Indices
                FsStaticLabelStackIndex
                nextFsStaticLabelStackIndex
                FsStaticSrLabelIndex
                nextFsStaticSrLabelIndex
 Output      :  The Get Next function gets the Next Index for
                the Index Value given in the Index Values. The
                Indices are stored in the next_varname variables.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
/* GET_NEXT Routine.  */
INT1
nmhGetNextIndexFsStaticSrLabelStackTable (UINT4 u4FsStaticLabelStackIndex,
                                          UINT4 *pu4NextFsStaticLabelStackIndex,
                                          UINT4 u4FsStaticSrLabelIndex,
                                          UINT4 *pu4NextFsStaticSrLabelIndex)
{
    tSrStaticEntryInfo *pSrStaticInfo = NULL;
    tTMO_SLL_NODE      *pSrOptStaticLblInfo = NULL;
    tStaticSrLbl       *pSrStaticLblInfo = NULL;
    tStaticSrLbl       *pSrStaticTmpLblInfo = NULL;

    if (gSrGlobalInfo.pSrStaticRbTree == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrStaticRbTree == NULL\n",
                __func__,__LINE__);
        return SNMP_FAILURE;
    }

    if ((pSrStaticInfo =
         RBTreeGetFirst (gSrGlobalInfo.pSrStaticRbTree)) == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGetFirst FAILED\n", __func__,
                 __LINE__);
        return SNMP_FAILURE;
    }
    while (pSrStaticInfo != NULL)
    {
        if (pSrStaticInfo->srStaticLblListIndex.u4Index ==
            u4FsStaticLabelStackIndex)
        {
            TMO_SLL_Scan (&(pSrStaticInfo->srStaticLblListIndex.StaticLblList),
                          pSrOptStaticLblInfo, tTMO_SLL_NODE *)
            {
                pSrStaticLblInfo = (tStaticSrLbl *) pSrOptStaticLblInfo;
                if (pSrStaticLblInfo->u4LblIndex == u4FsStaticSrLabelIndex)
                {
                    if (pSrStaticLblInfo->u4LblIndex <
                        pSrStaticInfo->srStaticLblListIndex.
                        StaticLblList.u4_Count)
                    {
                        pSrStaticTmpLblInfo =
                            (tStaticSrLbl *) (pSrOptStaticLblInfo->pNext);
                        *pu4NextFsStaticSrLabelIndex =
                            pSrStaticTmpLblInfo->u4LblIndex;
                        *pu4NextFsStaticLabelStackIndex =
                            pSrStaticInfo->srStaticLblListIndex.u4Index;
                        return SNMP_SUCCESS;
                    }
                    else
                    {
                        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC,
                                 "%s:%d Index points to last element in list.\n",
                                 __func__, __LINE__);
                        return SNMP_FAILURE;
                    }
                }
            }
            SR_TRC3 (SR_MGMT_TRC | SR_FAIL_TRC,
                     "%s:%d No entry found with StackLabel index: %d\n",
                     __func__, __LINE__, u4FsStaticLabelStackIndex);
            return SNMP_FAILURE;
        }
        pSrStaticInfo =
            RBTreeGetNext (gSrGlobalInfo.pSrStaticRbTree, pSrStaticInfo, NULL);
    }
    SR_TRC2 (SR_MGMT_TRC |SR_FAIL_TRC, "%s:%d No such entry present \n", __func__,
             __LINE__);
    return SNMP_FAILURE;

}

/****************************************************************************
 Function    :  nmhGetFsStaticSrRowStatus
 Input       :  The Indices
                FsStaticSrDestAddr
                FsStaticSrDestMask
                FsStaticSrDestAddrType

                The Object
                retValFsStaticSrRowStatus
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsStaticSrRowStatus (tSNMP_OCTET_STRING_TYPE * pFsStaticSrDestAddr,
                           UINT4 u4FsStaticSrDestMask,
                           INT4 i4FsStaticSrDestAddrType,
                           UINT4 u4FsSrStaticLspId,
                           INT4 *pi4RetValFsStaticSrRowStatus)
{
    tSrStaticEntryInfo *pSrStaticInfo = NULL;
    tSrStaticEntryInfo  SrStaticInfo;

    MEMSET (&SrStaticInfo, SR_ZERO, sizeof (tSrStaticEntryInfo));

    SR_OCTETSTRING_TO_INTEGER (pFsStaticSrDestAddr,
                               SrStaticInfo.destAddr.Addr.u4Addr);

    SrStaticInfo.u4DestMask = u4FsStaticSrDestMask;
    SrStaticInfo.destAddr.u2AddrType = (UINT2) i4FsStaticSrDestAddrType;
    SrStaticInfo.u4LspId = u4FsSrStaticLspId;

    pSrStaticInfo = RBTreeGet (gSrGlobalInfo.pSrStaticRbTree, &SrStaticInfo);
    if (pSrStaticInfo == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d RBTreeGet Failed\n", __func__,
                __LINE__);
        return SNMP_FAILURE;
    }

    *pi4RetValFsStaticSrRowStatus = pSrStaticInfo->u1RowStatus;
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhSetFsStaticSrRowStatus
 Input       :  The Indices
                FsStaticSrDestAddr
                FsStaticSrDestMask
                FsStaticSrDestAddrType

                The Object
                setValFsStaticSrRowStatus
 Output      :  The Set Low Lev Routine Take the Indices &
                Sets the Value accordingly.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhSetFsStaticSrRowStatus (tSNMP_OCTET_STRING_TYPE * pFsStaticSrDestAddr,
                           UINT4 u4FsStaticSrDestMask,
                           INT4 i4FsStaticSrDestAddrType,
                           UINT4 u4FsStaticSrLspId,
                           INT4 i4SetValFsStaticSrRowStatus)
{
    tSrStaticEntryInfo  SrStaticInfo;
    tSrStaticEntryInfo *pSrStaticInfo = NULL;

    UINT4               u4MplsTunIndex = SR_ZERO;
    UINT4               u4DestAddr = SR_ZERO;
    tGenU4Addr          DestAddr;

    MEMSET (&SrStaticInfo, SR_ZERO, sizeof (tSrStaticEntryInfo));
    MEMSET (&DestAddr, SR_ZERO, sizeof (tGenU4Addr));

    if ((pFsStaticSrDestAddr != NULL)
        && (pFsStaticSrDestAddr->pu1_OctetList != NULL))
    {
        MEMCPY (&u4DestAddr, pFsStaticSrDestAddr->pu1_OctetList,
                IPV4_ADDR_LENGTH);
    }
    u4DestAddr = OSIX_NTOHL (u4DestAddr);

    SR_TRC6 (SR_MGMT_TRC,
             "%s:%d ENTRY DestAddr %x Mask %x AddrType %x RowStatus %d\n",
             __func__, __LINE__, u4DestAddr, u4FsStaticSrDestMask,
             i4FsStaticSrDestAddrType, i4SetValFsStaticSrRowStatus);

    SrStaticInfo.destAddr.Addr.u4Addr = u4DestAddr;

    SrStaticInfo.u4DestMask = u4FsStaticSrDestMask;
    SrStaticInfo.destAddr.u2AddrType = (UINT2) i4FsStaticSrDestAddrType;
    SrStaticInfo.u4LspId = u4FsStaticSrLspId;

    pSrStaticInfo = SrGetStaticSrTableEntry (&SrStaticInfo);

    switch (i4SetValFsStaticSrRowStatus)
    {
        case CREATE_AND_WAIT:

            if (pSrStaticInfo != NULL)
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "\n%s:%d Static Sr Already Exist\n",
                         __func__, __LINE__);
                return SNMP_FAILURE;
            }
            else
            {
                pSrStaticInfo = SrCreateStaticSrTableEntry (&SrStaticInfo);

                if (pSrStaticInfo == NULL)
                {
                    SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d Static Sr Exist\n", __func__,
                             __LINE__);
                    return SNMP_FAILURE;
                }

                pSrStaticInfo->u1RowStatus = NOT_READY;
            }

            break;

        case NOT_IN_SERVICE:

            if (pSrStaticInfo == NULL)
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d Static Sr  doesn't Exist\n",
                         __func__, __LINE__);
                return SNMP_FAILURE;
            }

            if (pSrStaticInfo->u4MplsTunIndex) {
                u4MplsTunIndex = pSrStaticInfo->u4MplsTunIndex;
                if (SrStaticProcessCreateOrDel(&(pSrStaticInfo->destAddr), SR_FALSE, pSrStaticInfo->u4LspId, &u4MplsTunIndex) == SR_FAILURE) {
                    SR_TRC4 (SR_FAIL_TRC,"[%s: %d] ERROR! Processing NOT_IN_SERVICE failed for prefix: %x lspid: %d\r\n",
                            __func__, __LINE__, pSrStaticInfo->destAddr.Addr.u4Addr, pSrStaticInfo->u4LspId);
                    return SNMP_FAILURE;
                }
                pSrStaticInfo->u4MplsTunIndex = MPLS_ZERO;
            }

            pSrStaticInfo->u1RowStatus = NOT_IN_SERVICE;

            break;

        case ACTIVE:

            if (pSrStaticInfo == NULL)
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d Static Sr doesn't Exist\n",
                         __func__, __LINE__);
                return SNMP_FAILURE;
            }

            pSrStaticInfo->u1RowStatus = ACTIVE;

            if (SrStaticProcessCreateOrDel (&(pSrStaticInfo->destAddr), SR_TRUE,
                                            pSrStaticInfo->u4LspId, &u4MplsTunIndex) == SR_FAILURE)
            {
                SR_TRC3 (SR_MGMT_TRC | SR_FAIL_TRC,
                         " Line %d func %s Static Sr processing failed for prefix %x \r\n",
                         __LINE__, __func__,
                         pSrStaticInfo->destAddr.Addr.u4Addr);
                return SNMP_FAILURE;
            }
            pSrStaticInfo->u4MplsTunIndex = u4MplsTunIndex;
            break;

        case DESTROY:

            if (pSrStaticInfo == NULL)
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d Static Sr doesn't Exist\n",
                         __func__, __LINE__);
                return SNMP_FAILURE;
            }
            if (i4FsStaticSrDestAddrType == SR_IPV4_ADDR_TYPE)
            {
                DestAddr.Addr.u4Addr = pSrStaticInfo->destAddr.Addr.u4Addr;
                DestAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
            }

            if (pSrStaticInfo->u4MplsTunIndex) {

            u4MplsTunIndex = pSrStaticInfo->u4MplsTunIndex;
            if (SrStaticProcessCreateOrDel
                (&(pSrStaticInfo->destAddr), SR_FALSE,
                 pSrStaticInfo->u4LspId,
                 &u4MplsTunIndex) == SR_FAILURE)
            {
                SR_TRC3 (SR_MGMT_TRC | SR_FAIL_TRC,
                         " Line %d func %s Static Sr Processing failed for prefix %x \r\n",
                         __LINE__, __func__,
                         pSrStaticInfo->destAddr.Addr.u4Addr);
                return SNMP_FAILURE;
            }
            pSrStaticInfo->u4MplsTunIndex = MPLS_ZERO;

            }

            if (SrDeleteStaticSrTableEntry (pSrStaticInfo) == SR_FAILURE)
            {
                SR_TRC3 (SR_MGMT_TRC | SR_FAIL_TRC,
                         "%s:%d Static Sr Deletion Failure for Prefix : %x \n",
                         __func__, __LINE__,
                         pSrStaticInfo->destAddr.Addr.u4Addr);
                return SNMP_FAILURE;
            }
            break;

        default:
            return SNMP_FAILURE;
    }

    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhTestv2FsStaticSrNextHopAddr
 Input       :  The Indices
                FsStaticSrDestAddr
                FsStaticSrDestMask
                FsStaticSrDestAddrType

                The Object
                testValFsStaticSrNextHopAddr
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsStaticSrNextHopAddr (UINT4 *pu4ErrorCode,
                                tSNMP_OCTET_STRING_TYPE * pFsStaticSrDestAddr,
                                UINT4 u4FsStaticSrDestMask,
                                INT4 i4FsStaticSrDestAddrType,
                                UINT4 u4FsStaticLspId,
                                tSNMP_OCTET_STRING_TYPE *
                                pTestValFsStaticSrNextHopAddr)
{
    tSrStaticEntryInfo *pSrStaticInfo = NULL;
    tSrStaticEntryInfo  SrStaticInfo;
    UINT4               u4DestAddr = SR_ZERO;
    UINT4               u4NextHop = SR_ZERO;

    tGenU4Addr          destAddr;
    tGenU4Addr          nextHop;
    tGenU4Addr          dummyAddr;

    MEMSET (&destAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&nextHop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&SrStaticInfo, SR_ZERO, sizeof (tSrStaticEntryInfo));

    if ((pTestValFsStaticSrNextHopAddr != NULL)
        && (pTestValFsStaticSrNextHopAddr->pu1_OctetList != NULL))
    {
        MEMCPY (&u4NextHop, pTestValFsStaticSrNextHopAddr->pu1_OctetList,
                IPV4_ADDR_LENGTH);
    }

    if ((pFsStaticSrDestAddr != NULL)
        && (pFsStaticSrDestAddr->pu1_OctetList != NULL))
    {
        MEMCPY (&u4DestAddr, pFsStaticSrDestAddr->pu1_OctetList,
                IPV4_ADDR_LENGTH);
    }

    u4DestAddr = OSIX_NTOHL (u4DestAddr);
    u4NextHop = OSIX_NTOHL (u4NextHop);

    SrStaticInfo.destAddr.Addr.u4Addr = u4DestAddr;
    SrStaticInfo.NextHop.Addr.u4Addr = u4NextHop;
    SrStaticInfo.u4DestMask = u4FsStaticSrDestMask;
    SrStaticInfo.destAddr.u2AddrType = (UINT2) i4FsStaticSrDestAddrType;
    SrStaticInfo.NextHop.u2AddrType = (UINT2) i4FsStaticSrDestAddrType;
    SrStaticInfo.u4LspId = u4FsStaticLspId;

    pSrStaticInfo = SrGetStaticSrTableEntry (&SrStaticInfo);

    if (pSrStaticInfo == NULL)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d Static Sr Entry Doesnt Exist\n",
                 __func__, __LINE__);
        *pu4ErrorCode = SNMP_ERR_INCONSISTENT_VALUE;
        return SNMP_FAILURE;
    }
    if (i4FsStaticSrDestAddrType == SR_IPV4_ADDR_TYPE)
    {
        if (u4DestAddr == SR_ZERO /*|| u4NextHop == SR_ZERO*/)
        {
            *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
            return SNMP_FAILURE;
        }
        else if (!((MPLS_IS_ADDR_CLASS_A (u4DestAddr)) ||
                   (MPLS_IS_ADDR_CLASS_B (u4DestAddr)) ||
                   (MPLS_IS_ADDR_CLASS_C (u4DestAddr)) ||
                   (MPLS_IS_ADDR_CLASS_A (u4NextHop)) ||
                   (MPLS_IS_ADDR_CLASS_B (u4NextHop)) ||
                   (MPLS_IS_ADDR_CLASS_C (u4NextHop))))
        {
            *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
            return SNMP_FAILURE;
        }

        /*Test 3: DestAddr & MandRtrId should not be Self IP Addr */
        if (NetIpv4IfIsOurAddress (u4DestAddr) == NETIPV4_SUCCESS)
        {
            *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
            return SNMP_FAILURE;
        }
        else if (NetIpv4IfIsOurAddress (u4NextHop) == NETIPV4_SUCCESS)
        {
            *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
            return SNMP_FAILURE;
        }
    }
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhTestv2FsStaticSrLabelStackIndex
 Input       :  The Indices
                FsStaticSrDestAddr
                FsStaticSrDestMask
                FsStaticSrDestAddrType

                The Object
                testValFsStaticSrLabelStackIndex
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsStaticSrLabelStackIndex (UINT4 *pu4ErrorCode,
                                    tSNMP_OCTET_STRING_TYPE *
                                    pFsStaticSrDestAddr,
                                    UINT4 u4FsStaticSrDestMask,
                                    INT4 i4FsStaticSrDestAddrType,
                                    UINT4 u4FsStaticSrLspId,
                                    UINT4 u4TestValFsStaticSrLabelStackIndex)
{
    INT4                i4GblSrMode = SR_DISABLE;

    UNUSED_PARAM (pFsStaticSrDestAddr);
    UNUSED_PARAM (u4FsStaticSrDestMask);
    UNUSED_PARAM (i4FsStaticSrDestAddrType);
    UNUSED_PARAM (u4FsStaticSrLspId);

    nmhGetFsSrGlobalAdminStatus (&i4GblSrMode);
    if (i4GblSrMode == SR_DISABLE)
    {
        *pu4ErrorCode = SNMP_ERR_AUTHORIZATION_ERROR;
        return SNMP_FAILURE;
    }

    if ((u4TestValFsStaticSrLabelStackIndex <= 0) ||
        (u4TestValFsStaticSrLabelStackIndex > MAX_MPLSDB_LABELS_PER_ENTRY))
    {
        *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
        return SNMP_FAILURE;
    }
    return SNMP_SUCCESS;

}

/****************************************************************************
 Function    :  nmhTestv2FsStaticSrRowStatus
 Input       :  The Indices
                FsStaticSrDestAddr
                FsStaticSrDestMask
                FsStaticSrDestAddrType

                The Object
                testValFsStaticSrRowStatus
 Output      :  The Test Low Lev Routine Take the Indices &
                Test whether that Value is Valid Input for Set.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhTestv2FsStaticSrRowStatus (UINT4 *pu4ErrorCode,
                              tSNMP_OCTET_STRING_TYPE * pFsStaticSrDestAddr,
                              UINT4 u4FsStaticSrDestMask,
                              INT4 i4FsStaticSrDestAddrType,
                              UINT4 u4FsStaticSrLspId,
                              INT4 i4TestValFsStaticSrRowStatus)
{
    tSrStaticEntryInfo *pSrStaticInfo = NULL;
    tSrStaticEntryInfo  SrStaticInfo;
    UINT4               u4DestAddr = SR_ZERO;

    tGenU4Addr          destAddr;
    tGenU4Addr          nextHop;
    tGenU4Addr          dummyAddr;

    MEMSET (&destAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&nextHop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&SrStaticInfo, SR_ZERO, sizeof (tSrStaticEntryInfo));

    if ((pFsStaticSrDestAddr != NULL)
        && (pFsStaticSrDestAddr->pu1_OctetList != NULL))
    {
        MEMCPY (&u4DestAddr, pFsStaticSrDestAddr->pu1_OctetList,
                IPV4_ADDR_LENGTH);
    }
    u4DestAddr = OSIX_NTOHL (u4DestAddr);

    SrStaticInfo.destAddr.Addr.u4Addr = u4DestAddr;
    SrStaticInfo.u4DestMask = u4FsStaticSrDestMask;
    SrStaticInfo.destAddr.u2AddrType = (UINT2) i4FsStaticSrDestAddrType;
    SrStaticInfo.u4LspId = u4FsStaticSrLspId;

    pSrStaticInfo = SrGetStaticSrTableEntry (&SrStaticInfo);

    switch (i4TestValFsStaticSrRowStatus)
    {
        case CREATE_AND_WAIT:

            /*Test 1: Static SR (DestAddr,Mask) should not be already present (Check RBTree) */

            if (pSrStaticInfo != NULL)
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d Static Sr Already Exist\n",
                         __func__, __LINE__);
                *pu4ErrorCode = SNMP_ERR_INCONSISTENT_VALUE;
                return SNMP_FAILURE;
            }

            /*Test 2: IP Address related checks */
            if (u4DestAddr == SR_ZERO)
            {
                *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
                return SNMP_FAILURE;
            }
            else if (!((MPLS_IS_ADDR_CLASS_A (u4DestAddr)) ||
                       (MPLS_IS_ADDR_CLASS_B (u4DestAddr)) ||
                       (MPLS_IS_ADDR_CLASS_C (u4DestAddr))))
            {
                *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
                return SNMP_FAILURE;
            }

            if ((MPLS_IS_ADDR_LOOPBACK (u4DestAddr)) ||
                (MPLS_IS_BROADCAST_ADDR (u4DestAddr)))
            {
                *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
                return SNMP_FAILURE;
            }

            /*Test 3: DestAddr & MandRtrId should not be Self IP Addr */
            if (NetIpv4IfIsOurAddress (u4DestAddr) == NETIPV4_SUCCESS)
            {
                *pu4ErrorCode = SNMP_ERR_WRONG_VALUE;
                return SNMP_FAILURE;
            }
            break;
        case ACTIVE:

            if (pSrStaticInfo == NULL)
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d Static Sr doesn't Exist\n",
                         __func__, __LINE__);
                *pu4ErrorCode = SNMP_ERR_NO_CREATION;
                return SNMP_FAILURE;
            }

            break;

        case NOT_IN_SERVICE:

            if (pSrStaticInfo == NULL)
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d Static Sr path doesn't Exist\n",
                         __func__, __LINE__);
                return SNMP_FAILURE;
            }

            break;

        case DESTROY:

            if (pSrStaticInfo == NULL)
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d Static Sr doesn't Exist\n",
                         __func__, __LINE__);
                *pu4ErrorCode = SNMP_ERR_NO_CREATION;
                return SNMP_FAILURE;
            }

            break;

        default:
            return SNMP_FAILURE;
    }
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhDepv2FsStaticSrTable
 Input       :  The Indices
                FsStaticSrDestAddr
                FsStaticSrDestMask
                FsStaticSrDestAddrType
 Output      :  The Dependency Low Lev Routine Take the Indices &
                check whether dependency is met or not.
                Stores the value of error code in the Return val
 Error Codes :  The following error codes are to be returned
                SNMP_ERR_WRONG_LENGTH ref:(4 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_WRONG_VALUE ref:(6 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_NO_CREATION ref:(7 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_NAME ref:(8 of Sect 4.2.5 of rfc1905)
                SNMP_ERR_INCONSISTENT_VALUE ref:(10 of Sect 4.2.5 of rfc1905)
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhDepv2FsStaticSrTable (UINT4 *pu4ErrorCode, tSnmpIndexList * pSnmpIndexList,
                         tSNMP_VAR_BIND * pSnmpVarBind)
{
    UNUSED_PARAM (pu4ErrorCode);
    UNUSED_PARAM (pSnmpIndexList);
    UNUSED_PARAM (pSnmpVarBind);
    return SNMP_SUCCESS;
}

/****************************************************************************
 Function    :  nmhGetFsLabelStackIndexNext
 Input       :  The Indices

                The Object
                retValFsLabelStackIndexNext
 Output      :  The Get Low Lev Routine Take the Indices &
                store the Value requested in the Return val.
 Returns     :  SNMP_SUCCESS or SNMP_FAILURE
****************************************************************************/
INT1
nmhGetFsLabelStackIndexNext (UINT4 *pu4RetValFsLabelStackIndexNext)
{
    UINT4               u4Index = 0;
    u4Index = SrStaticLblListGetIndex ();

    if (u4Index == SR_ZERO)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d Label Index Table might be full\n",
                 __func__, __LINE__);
        return SNMP_FAILURE;
    }
    *pu4RetValFsLabelStackIndexNext = u4Index;
    return SNMP_SUCCESS;
}
