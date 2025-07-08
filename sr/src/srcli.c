/********************************************************************
* Copyright (C) 2007 Aricent Inc . All Rights Reserved
*
* $Id$
* 
* Description: This file contains Segment Routing related CLI 
*              code.
*********************************************************************/

#ifndef _SR_CLI_C
#define _SR_CLI_C

#include "srincs.h"
#include "srclip.h"
#include "lblmgrex.h"
#include "mplsdbinc.h"
#include "stdftnlw.h"
#include "stdlsrlw.h"
#include "ospfcli.h"
#include "ospf3cli.h"
#include "csr.h"
#include "srdef.h"

tSrAltModuleStatus  gu4AltModuleStatus;
extern UINT4        gu4AddrV4Family;
extern UINT4        gu4AddrV6Family;
extern UINT4        gu4SegmentMpls;
/*****************************************************************************
 * Function Name : cli_process_sr_cmd
 * Description   : This routine handles all the CLI requests and calls appropriate
                   functions based on command.
 * Input(s)      : CliHandle - Index of current CLI context
                   u4Command - Internal index of command
                   Variable number of arguments    
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
PUBLIC INT4
cli_process_sr_cmd (tCliHandle CliHandle, UINT4 u4Command, ...)
{
    va_list             ap;
    UINT4              *args[SR_CLI_MAX_ARGS] = { 0 };
    INT1                argno = 0;
    INT4                i4RetStatus = CLI_SUCCESS;
    INT4                i4argsCounter = SR_ZERO;
    INT4                sidCounter = SR_ZERO;
    UINT4               sidLabel[MAX_SR_LABEL_DEPTH] = { 0 };
    INT4                sidLabelDepth = SR_ZERO;
    INT1                i1LabelIndex = 0;
#ifdef IP6_WANTED
    UINT4               u4IfIndex = SR_ZERO;
#endif
    tGenU4Addr          genAddr;
    tGenU4Addr          nexthop;
    tGenU4Addr          aGenAddrOptRtrs[MAX_SR_LABEL_DEPTH];
    INT4                i4RetVal = SR_FAILURE;
    INT4                i4SrStatus = SR_FAILURE;
    UINT4               u4Afi = SR_ZERO;
    UINT4               u4Value = SR_ZERO;
    UINT4               u4ErrCode = SR_ZERO;
    UINT4               u4PrefixSidType = SR_ZERO;
    UINT4               u4SidIndexType = SR_ZERO;
    UINT4               u4PeerCount = SR_ZERO;
    UINT4               u4Prefix = SR_ZERO;
    INT4                i4AltStatus = SR_ZERO;

    MEMSET (&genAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&nexthop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&aGenAddrOptRtrs, SR_ZERO,
            sizeof (tGenU4Addr) * MAX_SR_LABEL_DEPTH);

    if (SR_INITIALISED != TRUE)
    {
        CLI_SET_ERR (CLI_SR_DOWN);
        return CLI_FAILURE;
    }

#ifdef CLI_WANTED
    CliRegisterLock (CliHandle, SrMainTaskLock, SrMainTaskUnLock);
#endif

    SR_LOCK ();
    va_start (ap, u4Command);

    while (argno < SR_CLI_MAX_ARGS)
    {
        args[argno] = va_arg (ap, UINT4 *);
        if ((args[argno] == NULL) && (argno != 2))
            break;
        argno++;
    }

    va_end (ap);

    switch (u4Command)
    {
        case SR_CLI_MPLS:
            i4RetVal = SrCliConfigMode (SR_GBL_ADM_UP);
            if (i4RetVal == SR_FAILURE)
            {
                i4RetStatus = CLI_FAILURE;
            }
            break;

        case SR_CLI_MPLS_DESTROY:
            i4RetVal = SrCliDisableSrMpls ();
            if (i4RetVal == SR_FAILURE)
            {
                i4RetStatus = CLI_FAILURE;
            }
            else
            {
                i4RetStatus = CLI_SUCCESS;
            }
            break;

		case SR_ALT_CAPABILITY:
			SrCliGetCurMode (&u4Afi);
			switch (u4Afi)
			{
				case SR_CLI_IPV4:           
					i4AltStatus = CLI_PTR_TO_I4 (args[0]);
					i4RetVal = SrCliConfigV4Alternate (CliHandle, i4AltStatus);
					if (i4RetVal != SR_SUCCESS)
					{
						i4RetStatus = CLI_FAILURE;
					}
					else
					{
						i4RetStatus = CLI_SUCCESS;
					}
					break;
				case SR_CLI_IPV6:
					i4AltStatus = CLI_PTR_TO_I4 (args[0]);
					i4RetVal = SrCliConfigV6Alternate (i4AltStatus);
					if (i4RetVal != SR_SUCCESS)
					{
						i4RetStatus = CLI_FAILURE;
					}
					else
					{
						i4RetStatus = CLI_SUCCESS;
					}
					break;

				default:
					break;
			}
			break;

		case SR_CLI_ADDR_FAMILY:
            u4Afi = CLI_PTR_TO_U4 (args[0]);
            u4Value = CLI_PTR_TO_U4 (args[1]);
            i4RetVal = SrCliConfigAddrFamily (u4Afi, u4Value);
            if (i4RetVal != SR_SUCCESS)
            {
                i4RetStatus = CLI_FAILURE;
            }
            else
            {
                i4RetStatus = CLI_SUCCESS;
            }
            break;

        case SR_CLI_ADMIN_UP:
            SrCliGetCurMode (&u4Afi);
            switch (u4Afi)
            {
                case SR_CLI_IPV4:
                    nmhGetFsSrV4Status (&i4SrStatus);
                    if (i4SrStatus == SR_ENABLED)
                    {
                        /* Break if SR is already enabled */
                        i4RetStatus = CLI_SUCCESS;
                        break;
                    }
                    i4RetVal =
                        nmhTestv2FsSrV4Status (&u4ErrCode, (INT4) SR_ENABLED);
                    if (i4RetVal != SNMP_SUCCESS)
                    {
                        i4RetStatus = CLI_FAILURE;
                        break;
                    }
                    i4RetVal = nmhSetFsSrV4Status (SR_ENABLED);
                    if (i4RetVal != SNMP_SUCCESS)
                    {
                        i4RetStatus = CLI_FAILURE;
                    }

                    nmhGetFsSrV4AlternateStatus(&i4AltStatus);
                    if (i4AltStatus == ALTERNATE_ENABLED)
                    {
                        SrRegisterWithRtmForAlt ();
                    }
                    break;

                case SR_CLI_IPV6:
                    nmhGetFsSrV6Status (&i4SrStatus);
                    if (i4SrStatus == SR_ENABLED)
                    {
                        /* Break if SR is already enabled */
                        i4RetStatus = CLI_SUCCESS;
                        break;
                    }
                    i4RetVal =
                        nmhTestv2FsSrV6Status (&u4ErrCode, (INT4) SR_ENABLED);
                    if (i4RetVal != SNMP_SUCCESS)
                    {
                        i4RetStatus = CLI_FAILURE;
                        break;
                    }
                    i4RetVal = nmhSetFsSrV6Status (SR_ENABLED);
                    if (i4RetVal != SNMP_SUCCESS)
                    {
                        i4RetStatus = CLI_FAILURE;
                    }
                    nmhGetFsSrV6AlternateStatus (&i4AltStatus);
                    if(i4AltStatus == ALTERNATE_ENABLED)
                    {
                        SrV3RegisterWithRtmForAlt ();
                        SrV3EnableAlternate ();
                    }
                    break;
                default:
                    break;
            }
            break;

        case SR_CLI_ADMIN_DOWN:
            SrCliGetCurMode (&u4Afi);
            switch (u4Afi)
            {
                case SR_CLI_IPV4:
                    nmhGetFsSrV4Status (&i4SrStatus);
                    if (i4SrStatus == SR_DISABLED)
                    {
                        /* Break if SR is already disabled */
                        i4RetStatus = CLI_SUCCESS;
                        break;
                    }
                    i4RetVal = nmhTestv2FsSrV4Status (&u4ErrCode, SR_DISABLED);
                    if (i4RetVal != SNMP_SUCCESS)
                    {
                        i4RetStatus = CLI_FAILURE;
                        break;
                    }
                    i4RetVal = nmhSetFsSrV4Status (SR_DISABLED);
                    if (i4RetVal != SNMP_SUCCESS)
                    {
                        i4RetStatus = CLI_FAILURE;
                    }
                    nmhGetFsSrV4AlternateStatus(&i4AltStatus);
                    if (i4AltStatus == ALTERNATE_ENABLED)
                    {
                        SrDeRegisterWithRtmForAlt ();
                    }
                    break;
                case SR_CLI_IPV6:
                    nmhGetFsSrV6Status (&i4SrStatus);
                    if (i4SrStatus == SR_DISABLED)
                    {
                        /* Break if SR is already enabled */
                        i4RetStatus = CLI_SUCCESS;
                        break;
                    }
                    i4RetVal = nmhTestv2FsSrV6Status (&u4ErrCode, SR_DISABLED);
                    if (i4RetVal != SNMP_SUCCESS)
                    {
                        i4RetStatus = CLI_FAILURE;
                        break;
                    }
                    i4RetVal = nmhSetFsSrV6Status (SR_DISABLED);
                    if (i4RetVal != SNMP_SUCCESS)
                    {
                        i4RetStatus = CLI_FAILURE;
                    }
                     nmhGetFsSrV6AlternateStatus (&i4AltStatus);
                     if (i4AltStatus == ALTERNATE_ENABLED)
                     {
                         SrV3DeRegisterWithRtmForAlt ();
                     }
                    break;
                default:
                    break;
            }
            break;

        case SR_CLI_SRGB:
            SrCliGetCurMode (&u4Afi);
            switch (u4Afi)
            {
                case SR_CLI_IPV4:
                    i4RetVal = SrCliProcessV4Srgb ((UINT4) *args[0],
                                                   (UINT4) *args[1]);
                    if (i4RetVal != SR_SUCCESS)
                    {
                        i4RetStatus = CLI_FAILURE;
                    }
                    break;
                case SR_CLI_IPV6:
                    i4RetVal = SrCliProcessV6Srgb ((UINT4) *args[0],
                                                   (UINT4) *args[1]);
                    if (i4RetVal != SR_SUCCESS)
                    {
                        i4RetStatus = CLI_FAILURE;
                    }
                    break;
                default:
                    break;
            }
            break;

        case SR_CLI_NO_SRGB:
            SrCliGetCurMode (&u4Afi);
            i4RetVal = SrCliDefaultSrgb (u4Afi);
            if (i4RetVal != SR_SUCCESS)
            {
                i4RetStatus = CLI_FAILURE;
                break;
            }
            switch (u4Afi)
            {
                case SR_CLI_IPV4:
                    IS_V4_SRGB_CONFIGURED = SR_FALSE;
                    break;
                case SR_CLI_IPV6:
                    IS_V6_SRGB_CONFIGURED = SR_FALSE;
                    break;
                default:
                    break;
            }
            break;

        case SR_CLI_ADD_PREFIX_SID:
            u4PrefixSidType = CLI_PTR_TO_U4 (args[0]);
            u4SidIndexType = CLI_PTR_TO_U4 (args[1]);
#ifdef CLI_WANTED
            MplsGetIpv4Ipv6AddrfromCliStr (CliHandle, (CONST CHR1 *)
                                           args[3], &genAddr);
#endif
            SrCliGetCurMode (&u4Afi);
            if (((u4Afi == SR_CLI_IPV4)
                 && (genAddr.u2AddrType == SR_IPV6_ADDR_TYPE))
                || ((u4Afi == SR_CLI_IPV6)
                    && (genAddr.u2AddrType == SR_IPV4_ADDR_TYPE)))
            {
                CLI_SET_ERR (CLI_SR_INV_IPADDR);
                break;
            }
            if (args[4] == NULL)
            {
                /* Node -SID */
                i4RetVal = SrHandleAddSidReq
                    (u4PrefixSidType, u4SidIndexType, (UINT4) *args[2],
                     &genAddr, (UINT4) SR_MAX_UINT4);
            }
            else
            {
                /* Adj - SID */
                if (genAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
                {
                    if (NetIpv4IfIsOurAddress ((UINT4) *args[3]) ==
                        NETIPV4_SUCCESS)
                    {
                        CLI_SET_ERR (CLI_SR_ADJ_SID_CONFIG);
                        i4RetVal = SR_FAILURE;
                        break;
                    }
                    i4RetVal = SrHandleAddSidReq
                        (u4PrefixSidType, u4SidIndexType, (UINT4) *args[2],
                         &genAddr, (UINT4) *args[4]);
                }
#ifdef IP6_WANTED
                else if (genAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
                {
                    if (NetIpv6IsOurAddress (&genAddr.Addr.Ip6Addr, &u4IfIndex)
                        == NETIPV6_SUCCESS)
                    {
                        CLI_SET_ERR (CLI_SR_ADJ_SID_CONFIG);
                        i4RetVal = SR_FAILURE;
                        break;
                    }
                    i4RetVal = SrHandleAddSidReq
                        (u4PrefixSidType, u4SidIndexType, (UINT4) *args[2],
                         &genAddr, (UINT4) *args[5]);
                }
#endif
            }

            if (i4RetVal != SR_SUCCESS)
            {
                i4RetStatus = CLI_FAILURE;
            }
            break;

        case SR_CLI_DEL_PREFIX_SID:
#ifdef CLI_WANTED
            MplsGetIpv4Ipv6AddrfromCliStr (CliHandle, (CONST CHR1 *)
                                           args[0], &genAddr);
#endif
            SrCliGetCurMode (&u4Afi);
            if (((u4Afi == SR_CLI_IPV4)
                 && (genAddr.u2AddrType == SR_IPV6_ADDR_TYPE))
                || ((u4Afi == SR_CLI_IPV6)
                    && (genAddr.u2AddrType == SR_IPV4_ADDR_TYPE)))
            {
                CLI_SET_ERR (CLI_SR_INV_IPADDR);
                break;
            }

            if (args[1] == NULL)
            {
                i4RetVal =
                    SrCliProcessDelSid (CliHandle, &genAddr,
                                        (UINT4) SR_MAX_UINT4);
            }
            else
            {
                i4RetVal =
                    SrCliProcessDelSid (CliHandle, &genAddr, (UINT4) *args[1]);
            }

            if (i4RetVal != SR_SUCCESS)
            {
                i4RetStatus = CLI_FAILURE;
                SR_TRC (SR_MGMT_TRC | SR_FAIL_TRC,
                        "%Not able to delete entry from SID table\r\n");
            }
            break;

        case SR_CLI_ADD_STATIC_BINDING:
#ifdef CLI_WANTED
            MplsGetIpv4Ipv6AddrfromCliStr (CliHandle, (CONST CHR1 *)
                                           args[0], &genAddr);

            MplsGetIpv4Ipv6AddrfromCliStr (CliHandle, (CONST CHR1 *)
                                           args[3], &nexthop);
#endif
            SrCliGetCurMode (&u4Afi);
            if (((u4Afi == SR_CLI_IPV4)
                 && (genAddr.u2AddrType == SR_IPV6_ADDR_TYPE))
                || ((u4Afi == SR_CLI_IPV6)
                    && (genAddr.u2AddrType == SR_IPV4_ADDR_TYPE)))
            {
                CLI_SET_ERR (CLI_SR_INV_IPADDR);
                break;
            }

            sidLabelDepth = argno - SR_FIVE;    /*(Num of Variable args) - (Num of non-label args ) - (Top Label) */
            if ((sidLabelDepth < SR_ZERO)
                || (sidLabelDepth > MAX_SR_LABEL_DEPTH))
            {
                CLI_SET_ERR (CLI_SR_INV_NUM_OF_LABELS);
                i4RetVal = SR_FAILURE;
                break;
            }

            i4argsCounter = argno - SR_ONE;    /*To get the last label in label stack */
            sidCounter = 0;
            i1LabelIndex = 5; /*index of second label*/
            while (args[i1LabelIndex] != NULL)
            {
                    sidLabel[sidCounter] = *args[i1LabelIndex];
                    i1LabelIndex++;
                    sidCounter++;
            }		
            i4RetVal =
                SrCliAddStaticBinding (CliHandle, &genAddr, (UINT4) *args[1],
                                       &nexthop, (UINT4) *args[4], sidLabel,
                                       (UINT4) sidLabelDepth, (UINT4) args[2]);
            if (i4RetVal != SR_SUCCESS)
            {
                i4RetStatus = CLI_FAILURE;
            }
            break;

        case SR_CLI_DEL_STATIC_BINDING:
#ifdef CLI_WANTED
            MplsGetIpv4Ipv6AddrfromCliStr (CliHandle, (CONST CHR1 *)
                                           args[0], &genAddr);

            MplsGetIpv4Ipv6AddrfromCliStr (CliHandle, (CONST CHR1 *)
                                           args[3], &nexthop);
#endif
            SrCliGetCurMode (&u4Afi);
            if (((u4Afi == SR_CLI_IPV4)
                 && (genAddr.u2AddrType == SR_IPV6_ADDR_TYPE))
                || ((u4Afi == SR_CLI_IPV6)
                    && (genAddr.u2AddrType == SR_IPV4_ADDR_TYPE)))
            {
                CLI_SET_ERR (CLI_SR_INV_IPADDR);
                break;
            }
            sidLabelDepth = argno - SR_FIVE;    /*(Num of Variable args) - (Num of non-label args ) - (Top Label) */
            if ((sidLabelDepth < SR_ZERO)
                || (sidLabelDepth > MAX_SR_LABEL_DEPTH))
            {
                CLI_SET_ERR (CLI_SR_INV_NUM_OF_LABELS);
                i4RetVal = SR_FAILURE;
                break;
            }

            i4argsCounter = argno - SR_ONE;    /*To get the last label in label stack */
            sidCounter = 0;
            i1LabelIndex = 5; /*index of second label*/
            while (args[i1LabelIndex] != NULL)
            {
                    sidLabel[sidCounter] = *args[i1LabelIndex];
                    i1LabelIndex++;
                    sidCounter++;
            }		
            i4RetVal =
                SrCliDelStaticBinding (CliHandle, &genAddr, (UINT4) *args[1],
                                       &nexthop, (UINT4) *args[4], sidLabel,
                                       (UINT4) sidLabelDepth, (UINT4) args[2]);
            if (i4RetVal != SR_SUCCESS)
            {
                i4RetStatus = CLI_FAILURE;
            }
            break;

        case SR_SHOW_CONFIG:
            i4RetVal = SrCliShowConfig (CliHandle);
            if (i4RetVal != SR_SUCCESS)
            {
                CliPrintf (CliHandle,
                           "%% Not able display Segment Routing Configurations\r\n");
            }
            break;
        case SR_SHOW_SID_TABLE:
            i4RetVal = SrCliShowSidInterfaceList (CliHandle);
            if (i4RetVal != SR_SUCCESS)
            {
                CliPrintf (CliHandle,
                           "%% Not able display Segment Routing SID Table\r\n");
            }
            break;

        case SR_SHOW_ROUTE:
            if (args[0] != NULL)
            {
               u4Prefix = *((args[0]));
            }

            i4RetVal = SrCliShowRouteInfo (CliHandle,u4Prefix,
                                            CLI_PTR_TO_U4(args[1]));
            if (i4RetVal != SR_SUCCESS)
            {
                CliPrintf (CliHandle,
			    "%% Not able to display SR Route info\r\n");
            }
            break;

        case SR_SHOW_PEER_TABLE:
            i4RetVal = SrCliShowPeerInfoList (CliHandle, u4Command);
            if (i4RetVal != SR_SUCCESS)
            {
                CliPrintf (CliHandle,
                           "%% Not able Display Peer Infor for Dynamic Segment Routing\r\n");
            }
            break;

        case SR_SHOW_PEER_TABLE_DETAIL:
            i4RetVal = SrCliShowPeerInfoList (CliHandle, u4Command);
            if (i4RetVal != SR_SUCCESS)
            {
                CliPrintf (CliHandle,
                           "%% Not able Display Peer Infor for Dynamic Segment Routing\r\n");
            }
            u4PeerCount = TMO_SLL_Count (&(gSrGlobalInfo.routerList));
            CliPrintf (CliHandle,
                       "\nTotal Segment-Routing Peer Count : %d \r\n",
                       u4PeerCount);
            CliPrintf (CliHandle, "\r\n");
            break;

        case SR_CLI_DEBUG:
	    i4RetStatus = SrSetDebugLevel (CliHandle, CLI_PTR_TO_U4 (args[0]), CLI_ENABLE);
            break;

        case SR_CLI_NO_DEBUG:
	    i4RetStatus = SrSetDebugLevel (CliHandle, CLI_PTR_TO_U4 (args[0]), CLI_DISABLE);
            break;

        case SR_SHOW_OSPF_SR_CAPABILITY:
            i4RetVal = SrCliShowOspfSrCapability (CliHandle);
            if (i4RetVal != SR_SUCCESS)
            {
                CliPrintf (CliHandle,
                           "%% Not able to display Segment-Routing Capability Information\r\n");
            }
            break;
        default:
            break;
    }
#ifdef CLI_WANTED
    if ((i4RetStatus == CLI_FAILURE)
        && (CLI_GET_ERR (&u4ErrCode) == CLI_SUCCESS))
    {
        if ((u4ErrCode > 0) && (u4ErrCode < CLI_SR_MAX_ERR))
        {
            CliPrintf (CliHandle, "\r%s", SrCliErrString[u4ErrCode]);
        }
        CLI_SET_ERR (0);
    }
    CliUnRegisterLock (CliHandle);
#endif
    SR_UNLOCK ();
    /*To Avoid coverity */
    UNUSED_PARAM (i4RetVal);
    return i4RetStatus;
}

/*****************************************************************************
 * Function Name : SrCliConfigMode
 * Description   : This routine set the SR Admin status and 
 *                 changes the mode to SR configuration mode
 * Input(s)      : u1SrMode - Sr Mode
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliConfigMode (UINT1 u1SrMode)
{
    INT4                i4RetStatus = SR_SUCCESS;
    UINT4               u4ErrorCode = SR_ZERO;

    if (SR_INITIALISED != TRUE)
    {
        CLI_SET_ERR (CLI_SR_DOWN);
        return SR_FAILURE;
    }

    gSrGlobalInfo.u4CliCmdMode = SR_TRUE;

    if (nmhTestv2FsSrGlobalAdminStatus (&u4ErrorCode, (INT4) u1SrMode)
        == SNMP_FAILURE)
    {
        return SR_FAILURE;
    }
    /* Check whether segment-routing Mpls disable is given or not */
    if (gu4SegmentMpls == SR_ONE)
    {
        CLI_SET_ERR (CLI_SR_DIS_SEG_MPLS);
        return SNMP_FAILURE;
    }
    if (nmhSetFsSrGlobalAdminStatus ((INT4) u1SrMode) == SNMP_SUCCESS)
    {
        if (u1SrMode == SR_GBL_ADM_UP)
        {
#ifdef CLI_WANTED
            i4RetStatus = CliChangePath (SR_CONFIG_MODE);
            if (i4RetStatus == CLI_FAILURE)
            {
                CLI_SET_ERR (CLI_SR_MODE_FAIL);
                i4RetStatus = SR_FAILURE;
            }
            else
            {
                i4RetStatus = SR_SUCCESS;
            }
#endif
        }
    }
    else
    {
        CLI_SET_ERR (CLI_SR_GBL_ADMIN_FAIL);
        i4RetStatus = SR_FAILURE;
    }

    gSrGlobalInfo.u4CliCmdMode = SR_FALSE;
    return i4RetStatus;
}

/*****************************************************************************
 * Function Name : SrCliConfigAddrFamily
 * Description   : This routine sets the the address-family mode on which the SR 
 *                 is to be configured.
 * Input(s)      : u4Afi - Address family type
 *                 u4Value - Enable or disable address family
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliConfigAddrFamily (UINT4 u4Afi, UINT4 u4Set)
{
    INT4                i4RetVal = SNMP_FAILURE;
    INT4                i4SrMode = SR_ZERO;
    UINT4               u4ErrCode = SR_ZERO;

    if (u4Set == SR_ENABLE)
    {
        switch (u4Afi)
        {
            case SR_CLI_IPV4:
                i4RetVal = nmhTestv2FsSrIpv4AddrFamily (&u4ErrCode, SR_ENABLE);
                if (i4RetVal == SNMP_FAILURE)
                {
                    return SR_FAILURE;
                }
                /* Check whether disable address family is given or not */
                if (gu4AddrV4Family == SR_ONE)
                {
                    CLI_SET_ERR (CLI_SR_IPV4_DIS_FAM);
                    return SNMP_FAILURE;
                }
                i4RetVal = nmhSetFsSrIpv4AddrFamily (SR_ENABLE);
                if (i4RetVal == SNMP_SUCCESS)
                {
                    SR_TRC (SR_MGMT_TRC,
                            "SR mode set to address-family ipv4\r\n");
                }
#ifdef CLI_WANTED
                i4RetVal = CliChangePath (SR_CONFIG_V4_MODE);
                if (i4RetVal == CLI_FAILURE)
                {
                    CLI_SET_ERR (CLI_SR_IPV4_MODE_FAIL);
                    return SR_FAILURE;
                }
#endif
                break;

            case SR_CLI_IPV6:
                i4RetVal = nmhTestv2FsSrIpv6AddrFamily (&u4ErrCode, SR_ENABLE);
                if (i4RetVal == SNMP_FAILURE)
                {
                    return SR_FAILURE;
                }
                /* Check whether disable address family is given or not */
                if (gu4AddrV6Family == SR_ONE)
                {
                    CLI_SET_ERR (CLI_SR_IPV6_DIS_FAM);
                    return SNMP_FAILURE;
                }
                i4RetVal = nmhSetFsSrIpv6AddrFamily (SR_ENABLE);
                if (i4RetVal == SR_SUCCESS)
                {
                    SR_TRC (SR_MGMT_TRC,
                            "SR mode set to address-family ipv6\r\n");
                }
#ifdef CLI_WANTED
                i4RetVal = CliChangePath (SR_CONFIG_V6_MODE);
                if (i4RetVal == CLI_FAILURE)
                {
                    CLI_SET_ERR (CLI_SR_IPV6_MODE_FAIL);
                    return SR_FAILURE;
                }
#endif
                break;

            default:
                break;
        }
    }
    else if (u4Set == SR_DISABLE)
    {
        switch (u4Afi)
        {
            case SR_CLI_IPV4:
                i4RetVal = nmhSetFsSrV4Status (SR_DISABLED);
                if (i4RetVal != SNMP_SUCCESS)
                {
                    return SR_FAILURE;
                }

                /* Delete the SID and SRGB details of the self node */
                if (SrDeleteSidTable (SR_IPV4_ADDR_TYPE) != SR_SUCCESS)
                {
                    break;
                }
                else
                {
                    gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex = SR_ZERO;
                    gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMaxIndex = SR_ZERO;
                    IS_V4_SRGB_CONFIGURED = SR_FALSE;
                }

                i4RetVal = nmhTestv2FsSrIpv4AddrFamily (&u4ErrCode, SR_DISABLE);
                if (i4RetVal == SNMP_FAILURE)
                {
                    return SR_FAILURE;
                }
                i4RetVal = nmhSetFsSrIpv4AddrFamily (SR_DISABLE);
                if (i4RetVal == SNMP_SUCCESS)
                {
                    SR_TRC (SR_MGMT_TRC,
                            "SR is disabled on address-family ipv4\r\n");
                }
                break;

            case SR_CLI_IPV6:
                nmhGetFsSrMode (&i4SrMode);
                if (((i4SrMode & SR_CONF_OSPFV3) == SR_CONF_OSPFV3) ||
                    ((i4SrMode & SR_CONF_STATIC_V6) == SR_CONF_STATIC_V6))
                {
                    gu4AddrV6Family = SR_ONE;
                    CLI_SET_ERR (CLI_SR_ENA_IN_OSPFV3);
                    return SR_SUCCESS;
                }

                if (SrDeleteSidTable (SR_IPV6_ADDR_TYPE) != SR_SUCCESS)
                {
                    break;
                }
                else
                {
                    gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex =
                        SR_ZERO;
                    gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MaxIndex =
                        SR_ZERO;
                    IS_V6_SRGB_CONFIGURED = SR_FALSE;
                }

                i4RetVal = nmhSetFsSrV6Status (SR_DISABLED);
                if (i4RetVal != SNMP_SUCCESS)
                {
                    return SR_FAILURE;
                }

                i4RetVal = nmhTestv2FsSrIpv6AddrFamily (&u4ErrCode, SR_DISABLE);
                if (i4RetVal == SNMP_FAILURE)
                {
                    return SR_FAILURE;
                }
                i4RetVal = nmhSetFsSrIpv6AddrFamily (SR_DISABLE);
                if (i4RetVal == SNMP_SUCCESS)
                {
                    SR_TRC (SR_MGMT_TRC,
                            "SR is disabled on address-family ipv6\r\n");
                }
                break;

            default:
                break;
        }
    }
    return SR_SUCCESS;
}

/*****************************************************************************
** Function Name : SrCliGetCfgPrompt
** Description   : This routine sets the current interface index to the mode
**                 information and returns the SR config prompt to be displayed.
** Input(s)      : pi1ModeName - New mode that to be set
** Output(s)     : pi1DispStr  - Prompt for that new mode
** Return(s)     : SR_TRUE or SR_FALSE.
******************************************************************************/
INT1
SrCliGetCfgPrompt (INT1 *pi1ModeName, INT1 *pi1DispStr)
{
    UINT4               u4Len = SR_ZERO;

    if ((!pi1DispStr) || (!pi1ModeName))
    {
        return SR_FALSE;
    }

    u4Len = PTR_TO_U4 (STRLEN (SR_CONFIG_MODE));
    if (STRNCMP (pi1ModeName, SR_CONFIG_MODE, u4Len) != 0)
    {
        return SR_FALSE;
    }

    /* check the mode passed is relavent and move
     * the name pointer to point to the interface index in it */

    pi1ModeName = pi1ModeName + u4Len;
#ifdef CLI_WANTED
    CLI_SET_IFINDEX (CLI_ATOI (pi1ModeName));
#endif
    STRNCPY (pi1DispStr, "(config-sr)#", (MAX_PROMPT_LEN - 1));
    pi1DispStr[(MAX_PROMPT_LEN - 1)] = '\0';

    return SR_TRUE;
}

/******************************************************************************/
/* Function Name     : SrCliGetAfiCfgPrompt                                   */
/* Description       : This function validates the given pi1ModeName          */
/*                     and returns the prompt in pi1DispStr if valid,         */
/* Input Parameters  : pi1ModeName - Mode Name to validate                    */
/*                     pi1DispStr - Display string to be returned.            */
/* Output Parameters : pi1DispStr - Display string.                           */
/* Return Value      : TRUE/FALSE                                             */
/******************************************************************************/
INT1
SrCliGetAfiCfgPrompt (INT1 *pi1ModeName, INT1 *pi1DispStr)
{
    UINT4               u4Len = SR_ZERO;

    if ((!pi1DispStr) || (!pi1ModeName))
    {
        return SR_FALSE;
    }

    u4Len = PTR_TO_U4 (STRLEN (SR_CONFIG_V4_MODE));
    if (STRNCMP (pi1ModeName, SR_CONFIG_V4_MODE, u4Len) == 0)
    {
        pi1ModeName = pi1ModeName + u4Len;
#ifdef CLI_WANTED
        CLI_SET_IFINDEX (CLI_ATOI (pi1ModeName));
#endif
        STRNCPY (pi1DispStr, "(config-sr-v4)#", (MAX_PROMPT_LEN - 1));
        pi1DispStr[(MAX_PROMPT_LEN - 1)] = '\0';
    }
    else if (STRNCMP (pi1ModeName, SR_CONFIG_V6_MODE, u4Len) == 0)
    {
        pi1ModeName = pi1ModeName + u4Len;
#ifdef CLI_WANTED
        CLI_SET_IFINDEX (CLI_ATOI (pi1ModeName));
#endif
        STRNCPY (pi1DispStr, "(config-sr-v6)#", (MAX_PROMPT_LEN - 1));
        pi1DispStr[(MAX_PROMPT_LEN - 1)] = '\0';
    }
    else
    {
        return SR_FALSE;
    }

    return SR_TRUE;
}

/******************************************************************************/
/* Function Name     : SrCliGetCurPrompt                                      */
/* Description       : This function validates the given pi1ModeName          */
/* Input Parameters  : None                                                   */
/* Output Parameters : pu1Prompt - pointer to the current prompt              */
/* Return Value      : SUCCESS/FAILURE                                        */
/******************************************************************************/
INT4
SrCliGetCurPrompt (UINT1 *pu1Prompt)
{
    INT1                ai1CurPrompt[MAX_PROMPT_LEN] = { 0 };
    INT1               *pi1Pos = NULL;

#ifdef CLI_WANTED
    if (CliGetCurPrompt (ai1CurPrompt) == CLI_FAILURE)
    {
        return SR_FAILURE;
    }
#endif

    pi1Pos = (INT1 *) STRRCHR ((CHR1 *) ai1CurPrompt, '/');
    if (pi1Pos != NULL)
    {
        pi1Pos = (INT1 *) STRRCHR ((CHR1 *) ai1CurPrompt, '-');
        if (pi1Pos != NULL)
        {
            STRCPY (pu1Prompt, pi1Pos + 1);
        }
    }
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrConfigSrV4Status
 * Description   : This routine set the admin status of address family.
 * Input(s)      : u1SrStatus - Sr Status
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrConfigSrV4Status (UINT1 u1SrStatus)
{
    INT4                i4RetVal = SR_FAILURE;
    UINT4               u4ErrCode = 0;

    i4RetVal = nmhTestv2FsSrV4Status (&u4ErrCode, (INT4) u1SrStatus);
    if (i4RetVal == SR_FAILURE)
    {
        return SR_FAILURE;
    }

    i4RetVal = nmhSetFsSrV4Status ((INT4) u1SrStatus);
    if (i4RetVal == SR_FAILURE)
    {
        return SR_FAILURE;
    }
    return SNMP_FAILURE;
}

/*****************************************************************************
 * Function Name : SrCliProcessAdminUpCmdForV4
 * Description   : This routine enables the SR module and applies the
 *                 configuration in V4 address-family mode
 * Input(s)      : None
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliProcessAdminUpCmdForV4 (VOID)
{
    INT4                i4SrStatus = SR_ZERO;
    INT4                i4RowStatus = MPLS_STATUS_ACTIVE;
    UINT4               u4SrTePathCount = SR_ZERO;
    INT4                i4RetStatus = SR_SUCCESS;
    UINT1               au1DestAddr[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1MandatoryRtrId[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    tSNMP_OCTET_STRING_TYPE snmpDestAddr;
    tSNMP_OCTET_STRING_TYPE snmpMandatoryRtrId;
    tSrTeRtEntryInfo    SrTeRtInfo;
    tOpqLSAInfo         opqLSAInfo;
    UINT1               au1RtrAddrTlv[SR_RI_LSA_TLV_LEN];
    UINT1               au1RtrAddrTlv1[SR_RI_LSA_TLV_LEN];
    UINT1               u1LSAType = TYPE10_OPQ_LSA;

    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    MEMSET (&snmpDestAddr, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    MEMSET (&snmpMandatoryRtrId, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    MEMSET (&opqLSAInfo, 0, sizeof (tOpqLSAInfo));
    MEMSET (au1RtrAddrTlv, 0, sizeof (SR_RI_LSA_TLV_LEN));
    MEMSET (au1RtrAddrTlv1, 0, sizeof (SR_RI_LSA_TLV_LEN));

    snmpDestAddr.pu1_OctetList = au1DestAddr;
    snmpMandatoryRtrId.pu1_OctetList = au1MandatoryRtrId;
    opqLSAInfo.pu1LSA = au1RtrAddrTlv;

    SR_TRC2 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d Entry \n", __func__, __LINE__);

    /* If Sr V4 status is already enabled, return */
    nmhGetFsSrV4Status (&i4SrStatus);
    if (i4SrStatus == SR_ENABLED)
    {
        i4RetStatus = SR_SUCCESS;
        return i4RetStatus;
    }

    /* Get the SR mode to check if any Dynamic protocol is registered */
    if ((SR_MODE & SR_CONF_OSPF) == SR_CONF_OSPF)
    {
        /* SR-TE Start */
        RBTreeCount (gSrGlobalInfo.pSrTeRouteRbTree, &u4SrTePathCount);
        if (u4SrTePathCount != SR_ZERO)
        {
            SR_TRC (SR_CTRL_TRC, "% Sr-TE Configured\r\n");

            /* Make all TePath Entries ACTIVE */
            SrTeSetAllTePathInfoRowStatus (i4RowStatus, SR_IPV4_ADDR_TYPE);
        }
        else
        {
            SR_TRC (SR_CTRL_TRC, "% No Sr-TE Configured\r\n");
        }
    }

    if ((i4RetStatus == SR_SUCCESS) &&
        ((gSrGlobalInfo.isLabelSpaceRsvd == SR_TRUE) ||
         (SrResvLabelSpace () == SR_SUCCESS)))
    {
        if (IS_V4_SRGB_CONFIGURED == SR_FALSE)
        {
            gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex =
                SRGB_DEFAULT_MIN_VALUE;
            gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMaxIndex =
                SRGB_DEFAULT_MAX_VALUE;
            IS_V4_SRGB_CONFIGURED = SR_TRUE;
        }
        gSrGlobalInfo.u4SrV4ModuleStatus = SR_ENABLED;
        i4RetStatus = SR_SUCCESS;

        if ((SR_MODE & SR_CONF_OSPF) == SR_CONF_OSPF)
        {
            if (gSrGlobalInfo.u1SrOspfRegState[OSPF_DEFAULT_CXT_ID] == SR_FALSE)
            {
                if (SrSndMsgToOspf (SR_OSPF_REG,
                            OSPF_DEFAULT_CXT_ID, NULL) != SR_SUCCESS)
                {
                    SR_TRC2 (SR_FAIL_TRC | SR_CTRL_TRC, "%s:%d Failure in requesting LSA\n",
                            __func__, __LINE__);
                    return SR_FAILURE;
                }
            }

            if (SR_RI_LSA_SENT & SR_EXT_PRF_LSA_SENT)
            {
                if (SR_FAILURE == SrUtilSendLSA (NEW_LSA, SR_ZERO, u1LSAType))
                {
                    SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d Failure in sending LSA\n",
                            __func__, __LINE__);
                    return SR_FAILURE;
                }
            }
        }

        if ((SR_MODE & SR_CONF_STATIC) == SR_CONF_STATIC)
        {
            /* Set all the Static FTNs create by SR as ACTIVE */
            if (SrMplsSetAllFTNRowStatus (i4RowStatus) == SR_FAILURE)
            {
                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                         "%s:%d SrMplsSetAllFTNRowStatus: ACTIVE FAILURE ",
                         __func__, __LINE__);
                gSrGlobalInfo.u4SrV4ModuleStatus = SR_DISABLED;
                i4RetStatus = SR_FAILURE;
            }
        }

        /* Set all the ILMs create by SR as ACTIVE */
        if (SrMplsSetAllILMRowStatus (i4RowStatus) == SR_FAILURE)
        {
            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsSetILMRowStatus: ACTIVE FAILURE",
                     __func__, __LINE__);
            gSrGlobalInfo.u4SrV4ModuleStatus = SR_DISABLED;
            i4RetStatus = SR_FAILURE;
        }

        SrAddOrDelILMSidTable (SR_IPV4_ADDR_TYPE, MPLS_MLIB_ILM_CREATE);
    }
    else
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d Failed in reserving the label space with label manager\n",
                __func__, __LINE__);
        i4RetStatus = SR_FAILURE;
    }

    SR_TRC2 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d Exit \n", __func__, __LINE__);
    return i4RetStatus;
}

/*****************************************************************************
 * Function Name : SrCliProcessAdminDownCmdForV4
 * Description   : This routine disables the SR module and applies the
 *                 configuration in V4 address-family mode
 * Input(s)      : None
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliProcessAdminDownCmdForV4 (UINT4 u4CmdStatus)
{
    INT4                i4SrStatus = SR_ZERO;
    INT4                i4RowStatus = MPLS_STATUS_NOT_INSERVICE;
    UINT1               u1LSAType = TYPE10_OPQ_LSA;

    SR_TRC3 (SR_MAIN_TRC, "%s:%d Cmd Status %s \n",
            __func__, __LINE__, (u4CmdStatus == SR_SHUT_CMD) ? "SHUT":"DESTROY");

    if (u4CmdStatus == SR_DESTROY_CMD)
    {
        i4RowStatus = MPLS_STATUS_DESTROY;
    }

    nmhGetFsSrV4Status (&i4SrStatus);
    if (i4SrStatus == SR_DISABLED)
    {
        /* SR is already disabled. Return SUCCESS */
        return SR_SUCCESS;
    }

    /* SrCliProcessAdminDownCmdForV4 will be called from two flows.
     * 1 - When SR is disabled , i.e; shutdown is given in SR mode.
     * 2 - When  OSPF is de-registered from SR.
     * The module status will not be disabled in the de-registration flow */
    if (gSrGlobalInfo.u1OspfDeRegInProgress != SR_TRUE)
    {
        gSrGlobalInfo.u4SrV4ModuleStatus = SR_DISABLED;
    }

    if ((SR_MODE & SR_CONF_OSPF) == SR_CONF_OSPF)
    {
        if ((gSrGlobalInfo.u1SrOspfRegState[OSPF_DEFAULT_CXT_ID] == SR_TRUE)
            && (gSrGlobalInfo.u1OspfDeRegInProgress != SR_TRUE))
        {
            if (SrSndMsgToOspf (SR_OSPF_DEREG,
                     OSPF_DEFAULT_CXT_ID, NULL) != SR_SUCCESS)
            {
                SR_TRC2 (SR_FAIL_TRC | SR_CTRL_TRC, "%s:%d Failure in requesting LSA\n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
        }
    }

    if ((SR_MODE & SR_CONF_STATIC) == SR_CONF_STATIC)
    {
        /* Set all the Static FTNs create by SR as NIS */
        if (SrMplsSetAllFTNRowStatus (i4RowStatus) == SR_FAILURE)
        {
            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsSetFTNRowStatus: NIS FAILURE \n",
                     __func__, __LINE__);
        }
        /* Set all the Static ILMs create by SR as NIS */
        if (SrMplsSetAllILMRowStatus (i4RowStatus) == SR_FAILURE)
        {
            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsSetILMRowStatus: NIS FAILURE \n",
                     __func__, __LINE__);
        }
    }

    if ((SR_MODE & SR_CONF_OSPF) == SR_CONF_OSPF)
    {
        SR_TRC2 (SR_CTRL_TRC, "%s:%d SR Shutdown with OSPF \n",
                __func__, __LINE__);
        if (u4CmdStatus == SR_SHUT_CMD)
        {
            if (SR_FAILURE == SrUtilSendLSA (FLUSHED_LSA, SR_ZERO, u1LSAType))
            {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d SrUtilSendLSA FLUSHED Failed \n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
        }

        SrTeSetAllTePathInfoRowStatus (i4RowStatus, SR_IPV4_ADDR_TYPE);

        if ((u4CmdStatus == SR_DESTROY_CMD) || (u4CmdStatus == SR_SHUT_CMD))
        {
            /* Segment Routing is Disabled. So, LFA entries are
             * alone cleared */
            if (gu4AltModuleStatus.u4SrIPV4AltModuleStatus == ALTERNATE_ENABLED)
            {
                gu4AltModuleStatus.u4SrIPV4AltModuleStatus = ALTERNATE_DISABLED;
                SrLfaLSPHandle ();
                gu4AltModuleStatus.u4SrIPV4AltModuleStatus = ALTERNATE_ENABLED;
            }
        }

        /* Delete all the Dynamic FTNs create by SR */
        SrMplsDynamicCleanUp (SR_IPV4_ADDR_TYPE);

    }
    if (gSrGlobalInfo.u4SrV4ModuleStatus == SR_DISABLED)
    {
        SrAddOrDelILMSidTable (SR_IPV4_ADDR_TYPE, MPLS_MLIB_ILM_DELETE);
    } 

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrCliProcessAdminUpCmdForV6
 * Description   : This routine enables the SR module and applies the
 *                 configuration in V6 address-family mode
 * Input(s)      : None
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliProcessAdminUpCmdForV6 (VOID)
{
    INT4                i4SrStatus = SR_ZERO;
    INT4                i4RowStatus = MPLS_STATUS_ACTIVE;
    UINT4               u4SrTePathCount = SR_ZERO;
    INT4                i4RetStatus = SR_SUCCESS;
    UINT1               au1DestAddr[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1MandatoryRtrId[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    tSNMP_OCTET_STRING_TYPE snmpDestAddr;
    tSNMP_OCTET_STRING_TYPE snmpMandatoryRtrId;
    tSrTeRtEntryInfo    SrTeRtInfo;
    tOpqLSAInfo         opqLSAInfo;
    UINT1               au1RtrAddrTlv[SR_RI_LSA_TLV_LEN];
    UINT1               au1RtrAddrTlv1[SR_RI_LSA_TLV_LEN];

    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    MEMSET (&snmpDestAddr, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    MEMSET (&snmpMandatoryRtrId, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    MEMSET (&opqLSAInfo, 0, sizeof (tOpqLSAInfo));
    MEMSET (au1RtrAddrTlv, 0, sizeof (SR_RI_LSA_TLV_LEN));
    MEMSET (au1RtrAddrTlv1, 0, sizeof (SR_RI_LSA_TLV_LEN));

    snmpDestAddr.pu1_OctetList = au1DestAddr;
    snmpMandatoryRtrId.pu1_OctetList = au1MandatoryRtrId;
    opqLSAInfo.pu1LSA = au1RtrAddrTlv;

    SR_TRC2 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d Entry \n", __func__, __LINE__);

    /* If Sr V6 status is already enabled, return */
    nmhGetFsSrV6Status (&i4SrStatus);
    if (i4SrStatus == SR_ENABLED)
    {
        i4RetStatus = SR_SUCCESS;
        return i4RetStatus;
    }

    /* Get the SR mode to check if any Dynamic protocol is registered */
    if ((SR_MODE & SR_CONF_OSPFV3) == SR_CONF_OSPFV3)
    {
        if (gSrGlobalInfo.pV6SrSelfNodeInfo == NULL)
        {
            SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                    "% Node-SID not configured. Cannot enable SR\r\n");
            i4RetStatus = SR_FAILURE;
        }
        else if (gSrGlobalInfo.pV6SrSelfNodeInfo->u4PrefixSidLabelIndex ==
                 SR_ZERO)
        {
            SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                    "% Node-SID not configured. Cannot enable SR\r\n");
            i4RetStatus = SR_FAILURE;
        }

        /* SR-TE Start */
        RBTreeCount (gSrGlobalInfo.pSrTeRouteRbTree, &u4SrTePathCount);
        if (u4SrTePathCount != SR_ZERO)
        {
            SR_TRC (SR_CTRL_TRC, "% Sr-TE Configured\r\n");

            /* Make all TePath Entries ACTIVE */
            SrTeSetAllTePathInfoRowStatus (i4RowStatus, SR_IPV6_ADDR_TYPE);

        }
        else
        {
            SR_TRC (SR_CTRL_TRC, "% No Sr-TE Configured\r\n");
        }
        /* SR-TE End */
    }

    if ((SR_MODE & SR_CONF_STATIC) == SR_CONF_STATIC)
    {
        /* Set all the Static FTNs create by SR as ACTIVE */
        if (SrMplsSetAllFTNRowStatus (i4RowStatus) == SR_FAILURE)
        {
            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                     "%s:%d SrMplsSetAllFTNRowStatus: ACTIVE FAILURE \n",
                     __func__, __LINE__);
            gSrGlobalInfo.u4SrV4ModuleStatus = SR_DISABLED;
            i4RetStatus = SR_FAILURE;
        }
    }

    if ((i4RetStatus == SR_SUCCESS) &&
        ((gSrGlobalInfo.isLabelSpaceRsvd == SR_TRUE) ||
         (SrResvLabelSpace () == SR_SUCCESS)))
    {
        if (IS_V6_SRGB_CONFIGURED == SR_FALSE)
        {
            gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex =
                SRGB_DEFAULT_MIN_VALUE;
            gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MaxIndex =
                SRGB_DEFAULT_MAX_VALUE;
            IS_V6_SRGB_CONFIGURED = SR_TRUE;
        }
        gSrGlobalInfo.u4SrV6ModuleStatus = SR_ENABLED;
        i4RetStatus = SR_SUCCESS;

        if (SR_FAILURE == SrV3UtilSendLSA (NEW_LSA, SR_ZERO))
        {
            SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "% Failure in sending LSA\r\n");
            return SR_FAILURE;
        }

        /* Set all the ILMs create by SR as ACTIVE */
        if (SrMplsSetAllILMRowStatus (i4RowStatus) == SR_FAILURE)
        {
            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsSetILMRowStatus: ACTIVE FAILURE\n",
                     __func__, __LINE__);
            gSrGlobalInfo.u4SrV6ModuleStatus = SR_DISABLED;
            i4RetStatus = SR_FAILURE;
        }
    }
    else
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d Failed in reserving the label space with label manager\n",
                __func__, __LINE__);
        i4RetStatus = SR_FAILURE;
    }

    SR_TRC2 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d Exit \n", __func__, __LINE__);
    return i4RetStatus;
}

/*****************************************************************************
 * Function Name : SrCliProcessAdminDownCmdForV6
 * Description   : This routine disables the SR module and applies the
 *                 configuration in V6 address-family mode
 * Input(s)      : None
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliProcessAdminDownCmdForV6 (UINT4 u4CmdStatus)
{
    INT4                i4SrStatus = SR_ZERO;
    INT4                i4RowStatus = MPLS_STATUS_NOT_INSERVICE;

    if (u4CmdStatus == SR_DESTROY_CMD)
    {
        i4RowStatus = MPLS_STATUS_DESTROY;
    }

    nmhGetFsSrV6Status (&i4SrStatus);
    if (i4SrStatus == SR_DISABLED)
    {
        /* SR is already disabled. Return SUCCESS */
        return SR_SUCCESS;
    }

    /* SrCliProcessAdminDownCmdForV6 will be called from two flows.
     * 1 - When SR is disabled , i.e; shutdown is given in SR mode.
     * 2 - When  OSPFv3 is de-registered from SR.
     * The module status will not be disabled in the de-registration flow */
    if (gSrGlobalInfo.u1Ospfv3DeRegInProgress != SR_TRUE)
    {
        gSrGlobalInfo.u4SrV6ModuleStatus = SR_DISABLED;
    }

    if ((SR_MODE & SR_CONF_STATIC) == SR_CONF_STATIC)
    {
        /* Set all the Static FTNs create by SR as NIS */
        if (SrMplsSetAllFTNRowStatus (i4RowStatus) == SR_FAILURE)
        {
            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsSetFTNRowStatus: NIS FAILURE \n",
                     __func__, __LINE__);
        }
        /* Set all the Static ILMs create by SR as NIS */
        if (SrMplsSetAllILMRowStatus (i4RowStatus) == SR_FAILURE)
        {
            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsSetILMRowStatus: NIS FAILURE \n",
                     __func__, __LINE__);
        }
    }

    if ((SR_MODE & SR_CONF_OSPFV3) == SR_CONF_OSPFV3)
    {
        if (SR_FAILURE == SrV3UtilSendLSA (FLUSHED_LSA, SR_ZERO))
        {
            SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d SrUtilSendLSA FLUSHED Failed \n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }

        SrTeSetAllTePathInfoRowStatus (i4RowStatus, SR_IPV6_ADDR_TYPE);

        /* Delete all the Dynamic FTNs create by SR */
        SrMplsDynamicCleanUp (SR_IPV6_ADDR_TYPE);

        if (u4CmdStatus == SR_DESTROY_CMD)
        {
            /* Delete all Static Entries: Static Pop, Static PopnFwd(Adj) */
            if (SrMplsSetAllILMRowStatus (i4RowStatus) == SR_FAILURE)
            {
                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                         "%s:%d SrMplsSetILMRowStatus: NIS FAILURE \n",
                         __func__, __LINE__);
            }
        }
    }
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrCliProcessV4Srgb
 * Description   : This routine configures the range of SRGB for Ipv4 Address family
 * Input(s)      : CliHandle - Index of current CLI context
 *                 u4MinSid      - Min range of SRGB
 *                 u4MaxSid  - Max range of SRGB
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliProcessV4Srgb (UINT4 u4MinSid, UINT4 u4MaxSid)
{
    UINT4               u4ErrorCode = 0;
    tSrSidInterfaceInfo *pSrSidIntf = NULL;

    if (u4MaxSid - u4MinSid > MAX_SRGB_SIZE)
    {
        SR_TRC1 (SR_CTRL_TRC | SR_FAIL_TRC, "SRGB range can't be more than %d \n\r",
                 MAX_SRGB_SIZE);
        CLI_SET_ERR (CLI_SR_INV_SRGB);
        return SR_FAILURE;
    }
    pSrSidIntf = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);

    while (pSrSidIntf != NULL)
    {
        if (pSrSidIntf->u4SidType != SR_SID_NODE)
        {
            if (pSrSidIntf->u4PrefixSidLabelIndex >= u4MinSid &&
                pSrSidIntf->u4PrefixSidLabelIndex <= u4MaxSid)
            {
                CLI_SET_ERR (CLI_SR_INV_ADJ_SID);
                return SR_FAILURE;
            }
        }
        pSrSidIntf =
            RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree, pSrSidIntf, NULL);
    }

    if (SNMP_FAILURE == nmhTestv2FsSrV4MinSrGbValue (&u4ErrorCode, u4MinSid))
    {
        return SR_FAILURE;
    }

    if (SNMP_FAILURE == nmhSetFsSrV4MinSrGbValue (u4MinSid))
    {
        SR_TRC (SR_MGMT_TRC | SR_FAIL_TRC, "Cannot change Min SRGB configuration\n");
        return SR_FAILURE;
    }

    if (SNMP_FAILURE == nmhTestv2FsSrV4MaxSrGbValue (&u4ErrorCode, u4MaxSid))
    {
        return SR_FAILURE;
    }

    if (SNMP_FAILURE == nmhSetFsSrV4MaxSrGbValue (u4MaxSid))
    {
        SR_TRC (SR_MGMT_TRC | SR_FAIL_TRC, "Cannot change Max SRGB configuration\n");
        return SR_FAILURE;
    }

    IS_V4_SRGB_CONFIGURED = SR_TRUE;
    return SR_SUCCESS;
}

 /*****************************************************************************
 * Function Name : SrCliProcessV6Srgb
 * Description   : This routine configures the range of SRGB for SR over V6
 *                 address-family
 * Input(s)      : CliHandle - Index of current CLI context
 *                 u4MinSid  - Min range of SRGB
 *                 u4MaxSid  - Max range of SRGB
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliProcessV6Srgb (UINT4 u4MinSid, UINT4 u4MaxSid)
{
    UINT4               u4ErrorCode = 0;
    tSrSidInterfaceInfo *pSrSidIntf = NULL;

    if (u4MaxSid - u4MinSid > MAX_SRGB_SIZE)
    {
        SR_TRC1 (SR_CTRL_TRC | SR_FAIL_TRC, "SRGB range can't be more than %d \n\r",
                 MAX_SRGB_SIZE);
        return SR_FAILURE;
    }

    pSrSidIntf = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);

    while (pSrSidIntf != NULL)
    {
        if (pSrSidIntf->u4SidTypeV3 != SR_SID_NODE)
        {
            if (pSrSidIntf->u4PrefixSidLabelIndex >= u4MinSid &&
                pSrSidIntf->u4PrefixSidLabelIndex <= u4MaxSid)
            {
                CLI_SET_ERR (CLI_SR_INV_ADJ_SID);
                return SR_FAILURE;
            }
        }
        pSrSidIntf =
            RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree, pSrSidIntf, NULL);
    }

    if (SNMP_FAILURE == nmhTestv2FsSrV6MinSrGbValue (&u4ErrorCode, u4MinSid))
    {
        return SR_FAILURE;
    }

    if (SNMP_FAILURE == nmhTestv2FsSrV6MaxSrGbValue (&u4ErrorCode, u4MaxSid))
    {
        return SR_FAILURE;
    }

    if (SNMP_FAILURE == nmhSetFsSrV6MaxSrGbValue (u4MaxSid))
    {
        SR_TRC (SR_MGMT_TRC | SR_FAIL_TRC, "Cannot change Max SRGB configuration\n");
        return SR_FAILURE;
    }

    if (SNMP_FAILURE == nmhSetFsSrV6MinSrGbValue (u4MinSid))
    {
        SR_TRC (SR_MGMT_TRC | SR_FAIL_TRC, "Cannot change Min SRGB configuration\n");
        return SR_FAILURE;
    }

    IS_V6_SRGB_CONFIGURED = SR_TRUE;
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrCliDefaultSrgb
 * Description   : This routine reverts the range of SRGB to default values
 *                 specific to the address-family
 * Input(s)      : CliHandle - Index of current CLI context
 *                 u1Afi - Addr-family in which the command is given
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliDefaultSrgb (UINT4 u1Afi)
{
    INT4                i4SrStatus = SR_ZERO;
    UINT4               isNodeSidPresent = SR_FALSE;
    tSrSidInterfaceInfo *pSrSidIntf = NULL;

    if (u1Afi == SR_CLI_IPV4)
    {
        nmhGetFsSrV4Status (&i4SrStatus);
    }
    else if (u1Afi == SR_CLI_IPV6)
    {
        nmhGetFsSrV6Status (&i4SrStatus);
    }

    if (i4SrStatus == SR_ENABLED)
    {
        CLI_SET_ERR (CLI_SR_CONFIG_FAIL);
        return SR_FAILURE;
    }

    pSrSidIntf = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);

    while (pSrSidIntf != NULL)
    {
        if ((pSrSidIntf->u4SidType != SR_SID_NODE) &&
            (pSrSidIntf->u4SidTypeV3 != SR_SID_NODE))
        {
            pSrSidIntf =
                RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree, pSrSidIntf, NULL);
        }
        else
        {
            isNodeSidPresent = SR_TRUE;
            break;
        }
    }

    if (isNodeSidPresent == SR_FALSE)
    {
        if (u1Afi == SR_CLI_IPV4)
        {
            gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex = SR_ZERO;
            gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMaxIndex = SR_ZERO;
        }
        else if (u1Afi == SR_CLI_IPV6)
        {
            gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex = SR_ZERO;
            gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MaxIndex = SR_ZERO;
        }
    }
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrHandleAddSidReq
 * Description   : This routine handles the Add SID request from CLI.
 * Input(s)      : u4PrefixSidType - Prefix Sid Type
 *                 u4SidIndexType - Sid Index type
 *                 u4SidValue - SID value
 *                 pGenAddr - Ip address
 *                 u4IpAddrMask - Ip address mask
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrHandleAddSidReq (UINT4 u4PrefixSidType,
                   UINT4 u4SidIndexType, UINT4 u4SidValue,
                   tGenU4Addr * pGenAddr, UINT4 u4IpAddrMask)
{
    UINT1               au1SrId[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1SrIdV6[SR_IPV6_ADDR_LENGTH] = { SR_ZERO };
    UINT4               u4ErrorCode  = SR_ZERO;
    INT4                u4AddrType   = SR_ZERO;
    UINT4               u4OutIfIndex = SR_ZERO;
    tIpConfigInfo       IpInfo       = {0};
    tSNMP_OCTET_STRING_TYPE srSidIpAddr;
    tSNMP_OCTET_STRING_TYPE *pSrSidIpAddr = &srSidIpAddr;

    MEMSET (&srSidIpAddr, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));

    if (pGenAddr->u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        u4AddrType = SR_IPV4_ADDR_TYPE;
        srSidIpAddr.pu1_OctetList = au1SrId;
        SR_INTEGER_TO_OCTETSTRING (pGenAddr->Addr.u4Addr, pSrSidIpAddr);
    }
    else if (pGenAddr->u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        u4AddrType = SR_IPV6_ADDR_TYPE;
        srSidIpAddr.pu1_OctetList = au1SrIdV6;
        MEMCPY (pSrSidIpAddr->pu1_OctetList, pGenAddr->Addr.Ip6Addr.u1_addr,
                SR_IPV6_ADDR_LENGTH);
        srSidIpAddr.i4_Length = SR_IPV6_ADDR_LENGTH;
    }
    else
    {
        CLI_SET_ERR (CLI_SR_INV_IPADDR_TYPE);
        return SR_FAILURE;
    }

    if (SNMP_FAILURE ==
        nmhTestv2FsSrSIDType (&u4ErrorCode, u4AddrType, pSrSidIpAddr,
                              u4IpAddrMask, (INT4) u4PrefixSidType))
    {
        return SR_FAILURE;
    }

    if (SNMP_FAILURE ==
        nmhTestv2FsSrSidIndexType (&u4ErrorCode, u4AddrType, pSrSidIpAddr,
                                   u4IpAddrMask, (INT4) u4SidIndexType))
    {
        return SR_FAILURE;
    }

    if (SNMP_FAILURE ==
        nmhTestv2FsSrSIDValue (&u4ErrorCode, u4AddrType, pSrSidIpAddr,
                               u4IpAddrMask, u4SidValue))
    {
        return SR_FAILURE;
    }

    if (SR_SID_ABS_INDEX == u4SidIndexType)
    {
        if (u4PrefixSidType == SR_SID_ADJACENCY)
        {
            if ((u4SidValue < SR_GEN_MIN_LBL) || (u4SidValue > SR_GEN_MAX_LBL))
            {
                CLI_SET_ERR (CLI_SR_INV_ADJ_SID);
                return SR_FAILURE;
            }

            if (u4AddrType == SR_IPV4_ADDR_TYPE)
            {
                if ((u4SidValue >=
                     gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex)
                    && (u4SidValue <=
                        gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMaxIndex))
                {
                    CLI_SET_ERR (CLI_SR_INV_ADJ_SID);
                    return SR_FAILURE;
                }

#ifdef CFA_WANTED
                if (CfaIpIfGetIfIndexFromHostIpAddressInCxt (MPLS_DEF_CONTEXT_ID,
                             pGenAddr->Addr.u4Addr, &u4OutIfIndex) == CFA_FAILURE)
                {
                    CLI_SET_ERR (CLI_SR_INV_ADJ_SID_IP_ADDR);
                    return SR_FAILURE;
                }

                CfaIpIfGetIfInfo (u4OutIfIndex, &IpInfo);

                if ((IpInfo.u4Addr != pGenAddr->Addr.u4Addr) ||
                    (IpInfo.u4NetMask != u4IpAddrMask))
                {
                    CLI_SET_ERR (CLI_SR_INV_ADJ_SID_IP_ADDR);
                    return SR_FAILURE;
                }
#endif
            }
            else if (u4AddrType == SR_IPV6_ADDR_TYPE)
            {
                if ((u4SidValue >=
                     gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex)
                    && (u4SidValue <=
                        gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MaxIndex))
                {
                    CLI_SET_ERR (CLI_SR_INV_ADJ_SID);
                    return SR_FAILURE;
                }
            }
        }
        if (u4PrefixSidType == SR_SID_NODE)
        {
            if (u4AddrType == SR_IPV4_ADDR_TYPE)
            {
                if ((u4SidValue <=
                     gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex)
                    || (u4SidValue >
                        gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMaxIndex))
                {
                    CLI_SET_ERR (CLI_SR_INV_VAL);
                    return SR_FAILURE;
                }
            }
            else if (u4AddrType == SR_IPV6_ADDR_TYPE)
            {
                if ((u4SidValue <=
                     gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex)
                    || (u4SidValue >
                        gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MaxIndex))
                {
                    CLI_SET_ERR (CLI_SR_INV_VAL);
                    return SR_FAILURE;
                }
            }
        }
    }

    if (SNMP_FAILURE ==
        nmhTestv2FsSrSIDRowStatus (&u4ErrorCode, u4AddrType, pSrSidIpAddr,
                                   u4IpAddrMask, CREATE_AND_WAIT))
    {
        return SR_FAILURE;
    }

    if (SNMP_FAILURE ==
        nmhSetFsSrSIDRowStatus (u4AddrType, pSrSidIpAddr, u4IpAddrMask,
                                CREATE_AND_WAIT))
    {
        return SR_FAILURE;
    }

    if (SNMP_FAILURE ==
        nmhSetFsSrSIDType (u4AddrType, pSrSidIpAddr, u4IpAddrMask,
                           (INT4) u4PrefixSidType))
    {
        SR_TRC (SR_MGMT_TRC | SR_FAIL_TRC, "Unable to Set Sid Type \r\n");
        nmhSetFsSrSIDRowStatus (u4AddrType, pSrSidIpAddr, u4IpAddrMask,
                                DESTROY);
        return SR_FAILURE;
    }

    if (SNMP_FAILURE ==
        nmhSetFsSrSidIndexType (u4AddrType, pSrSidIpAddr, u4IpAddrMask,
                                (INT4) u4SidIndexType))
    {
        SR_TRC (SR_MGMT_TRC | SR_FAIL_TRC, "Unable to Sid Index Type \r\n");
        nmhSetFsSrSIDRowStatus (u4AddrType, pSrSidIpAddr, u4IpAddrMask,
                                DESTROY);
        return SR_FAILURE;
    }

    if (SNMP_FAILURE ==
        nmhSetFsSrSIDValue (u4AddrType, pSrSidIpAddr, u4IpAddrMask, u4SidValue))
    {
        SR_TRC (SR_MGMT_TRC | SR_FAIL_TRC, "Unable to Set Sid Index Value \r\n");
        nmhSetFsSrSIDRowStatus (u4AddrType, pSrSidIpAddr, u4IpAddrMask,
                                DESTROY);
        return SR_FAILURE;
    }

    if (SNMP_FAILURE ==
        nmhTestv2FsSrSIDRowStatus (&u4ErrorCode, u4AddrType, pSrSidIpAddr,
                                   u4IpAddrMask, ACTIVE))
    {
        nmhSetFsSrSIDRowStatus (u4AddrType, pSrSidIpAddr, u4IpAddrMask,
                                DESTROY);
        return SR_FAILURE;
    }

    if (SNMP_FAILURE ==
        nmhSetFsSrSIDRowStatus (u4AddrType, pSrSidIpAddr, u4IpAddrMask, ACTIVE))
    {
        SR_TRC (SR_MGMT_TRC | SR_FAIL_TRC, "Unable to Mark Row Status as Active \r\n");
        return SR_FAILURE;
    }
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrCliProcessDelSid
 * Description   : This routine dletes the SID entry from SID database
 * Input(s)      : CliHandle    - Index of current CLI context
 *                 u4IfIpAddr   - IP address
 *                 u4IpAddrMask - Network Mask
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliProcessDelSid (tCliHandle CliHandle, tGenU4Addr * pIfIpAddr,
                    UINT4 u4IpAddrMask)
{
    UINT1               au1SrId[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1SrIdV6[SR_IPV6_ADDR_LENGTH] = { SR_ZERO };
    UINT4               u4ErrorCode = SR_ZERO;
    UINT4               u4AddrType = SR_ZERO;
    tSNMP_OCTET_STRING_TYPE srSidIpAddr;
    tSNMP_OCTET_STRING_TYPE *pSrSidIpAddr = &srSidIpAddr;

    UNUSED_PARAM (CliHandle);
    srSidIpAddr.pu1_OctetList = au1SrId;

    if (pIfIpAddr->u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        u4AddrType = SR_IPV4_ADDR_TYPE;
        srSidIpAddr.pu1_OctetList = au1SrId;
        SR_INTEGER_TO_OCTETSTRING (pIfIpAddr->Addr.u4Addr, pSrSidIpAddr);
    }
    else if (pIfIpAddr->u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        u4AddrType = SR_IPV6_ADDR_TYPE;
        srSidIpAddr.pu1_OctetList = au1SrIdV6;
        MEMCPY (pSrSidIpAddr->pu1_OctetList, pIfIpAddr->Addr.Ip6Addr.u1_addr,
                SR_IPV6_ADDR_LENGTH);

        srSidIpAddr.i4_Length = SR_IPV6_ADDR_LENGTH;
    }
    else
    {
        CLI_SET_ERR (CLI_SR_INV_IPADDR_TYPE);
        return SR_FAILURE;
    }

    if (SNMP_FAILURE ==
        nmhTestv2FsSrSIDRowStatus (&u4ErrorCode, (INT4) u4AddrType,
                                   pSrSidIpAddr, u4IpAddrMask, DESTROY))
    {
        return SR_FAILURE;
    }
    if (SNMP_FAILURE ==
        nmhSetFsSrSIDRowStatus ((INT4) u4AddrType, pSrSidIpAddr, u4IpAddrMask,
                                DESTROY))
    {
        return SR_FAILURE;
    }
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrCliAddStaticBinding
 * Description   : This routine creates static binding of labels for static
 *                                 configuration of Segment Routing
 * Input(s)      : CliHandle - Index of current CLI context
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliAddStaticBinding (tCliHandle CliHandle, tGenU4Addr * pDestAddr,
                       UINT4 u4Mask, tGenU4Addr * pNextHopAddr, UINT4 topLabel,
                       UINT4 sidLabel[], UINT4 sidLabelDepth, UINT4 u4LspId)
{
    INT4                i4SrMode = SR_ZERO;
    INT4                i4SrStatus = SR_ZERO;
    tGenU4Addr          destPrefix;
    tGenU4Addr          nextHop;
    tGenU4Addr          mask;
    UINT1               u1StackCount = SR_ZERO;
    UINT4               u4LabelIndex = SR_ZERO;
    UINT4               u4ErrCode = SR_ZERO;
    UINT4               u4StaticSrLabel = SR_ZERO;
    UINT1               au1DestAddr[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1nextHop[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    tSNMP_OCTET_STRING_TYPE snmpDestAddr;
    tSNMP_OCTET_STRING_TYPE snmpNextHop;
    tSNMP_OCTET_STRING_TYPE *pSnmpDestAddr = NULL;
    tSNMP_OCTET_STRING_TYPE *pSnmpNextHop = NULL;

    MEMSET (&snmpDestAddr, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    MEMSET (&snmpNextHop, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    /*MplsFTNCreate Ends */

    MEMSET (&destPrefix, 0, sizeof (tGenU4Addr));
    MEMSET (&nextHop, 0, sizeof (tGenU4Addr));
    MEMSET (&mask, 0, sizeof (tGenU4Addr));

    pSnmpDestAddr = &snmpDestAddr;
    pSnmpNextHop = &snmpNextHop;

    snmpDestAddr.pu1_OctetList = au1DestAddr;
    snmpNextHop.pu1_OctetList = au1nextHop;
#ifndef CLI_WANTED
    UNUSED_PARAM (sidLabel);
    UNUSED_PARAM (sidLabelDepth);
    UNUSED_PARAM (topLabel);
#endif

    if (pDestAddr->u2AddrType == SR_CLI_IPV4)
    {
        nmhGetFsSrV4Status (&i4SrStatus);
    }
    else if (pDestAddr->u2AddrType == SR_CLI_IPV6)
    {
        nmhGetFsSrV6Status (&i4SrStatus);
    }

    if (pDestAddr->u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        destPrefix.u2AddrType = SR_IPV4_ADDR_TYPE;
        nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;

        MEMCPY (&(destPrefix.Addr.u4Addr), &pDestAddr->Addr.u4Addr,
                SR_IPV4_ADDR_LENGTH);
        MEMCPY (&(nextHop.Addr.u4Addr), &pNextHopAddr->Addr.u4Addr,
                SR_IPV4_ADDR_LENGTH);

        mask.Addr.u4Addr = u4Mask;
        mask.u2AddrType = MPLS_IPV4_ADDR_TYPE;

        MplsGetPrefix (&destPrefix.Addr, &mask.Addr,
                       destPrefix.u2AddrType, &destPrefix.Addr);
    }
    else if (pDestAddr->u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        destPrefix.u2AddrType = SR_IPV6_ADDR_TYPE;
        nextHop.u2AddrType = SR_IPV6_ADDR_TYPE;

        MEMCPY (destPrefix.Addr.Ip6Addr.u1_addr,
                pDestAddr->Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);
        MEMCPY (nextHop.Addr.Ip6Addr.u1_addr,
                pNextHopAddr->Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);

        MEMCPY (mask.Addr.Ip6Addr.u1_addr, &u4Mask, sizeof (UINT4));
        mask.u2AddrType = SR_IPV6_ADDR_TYPE;

        MplsGetPrefix (&destPrefix.Addr, &mask.Addr,
                       destPrefix.u2AddrType, &destPrefix.Addr);
    }

    SR_INTEGER_TO_OCTETSTRING (pDestAddr->Addr.u4Addr, pSnmpDestAddr);
    SR_INTEGER_TO_OCTETSTRING (pNextHopAddr->Addr.u4Addr, pSnmpNextHop);

    if (nmhTestv2FsStaticSrRowStatus (&u4ErrCode, pSnmpDestAddr, u4Mask,
                                      pDestAddr->u2AddrType, u4LspId,
                                      MPLS_STATUS_CREATE_AND_WAIT) ==
        SNMP_FAILURE)
    {
        CliPrintf (CliHandle,
                   "%%Unable to add Entry (Test-CW) in Static Sr Table \r\n");
        return SR_FAILURE;
    }

    if (nmhSetFsStaticSrRowStatus (pSnmpDestAddr, u4Mask, pDestAddr->u2AddrType,
                                   u4LspId, MPLS_STATUS_CREATE_AND_WAIT) == SNMP_FAILURE)
    {
        CliPrintf (CliHandle,
                   "%%Unable to add Entry (Set-CW) in Static Sr Table \r\n");
        return SR_FAILURE;
    }
    /* Making Row Status to NIS */
    if (nmhTestv2FsStaticSrRowStatus (&u4ErrCode, pSnmpDestAddr, u4Mask,
                                      pDestAddr->u2AddrType,
                                      u4LspId, MPLS_STATUS_NOT_INSERVICE) ==
        SNMP_FAILURE)
    {
        CliPrintf (CliHandle,
                   "%%Unable to add Entry (Test-CW) in Static Sr Table \r\n");
        return SR_FAILURE;
    }
    if (nmhSetFsStaticSrRowStatus (pSnmpDestAddr, u4Mask, pDestAddr->u2AddrType,
                                   u4LspId, MPLS_STATUS_NOT_INSERVICE) == SNMP_FAILURE)
    {
        CliPrintf (CliHandle,
                   "%%Unable to add Entry (Set-CW) in Static Sr Table \r\n");
        return SR_FAILURE;
    }

    if (nmhGetFsStaticSrLabelStackIndex
        (pSnmpDestAddr, u4Mask, pDestAddr->u2AddrType, u4LspId,
         &u4LabelIndex) == SNMP_FAILURE)
    {
        CliPrintf (CliHandle, "%%Unable to get Label List Index \r\n");
        nmhSetFsStaticSrRowStatus (pSnmpDestAddr, u4Mask, pDestAddr->u2AddrType,
                                   u4LspId, DESTROY);
        return SR_FAILURE;
    }
    u4StaticSrLabel = topLabel;
    if (nmhTestv2FsStaticSrLabelStackLabel
        (&u4ErrCode, u4LabelIndex, SR_ZERO, u4StaticSrLabel) == SNMP_FAILURE)
    {
        nmhSetFsStaticSrRowStatus (pSnmpDestAddr, u4Mask, pDestAddr->u2AddrType, u4LspId,
                                   DESTROY);
        return SR_FAILURE;
    }

    if (nmhSetFsStaticSrLabelStackLabel (u4LabelIndex, SR_ZERO, u4StaticSrLabel)
        == SNMP_FAILURE)
    {
        nmhSetFsStaticSrRowStatus (pSnmpDestAddr, u4Mask, pDestAddr->u2AddrType, u4LspId,
                                   DESTROY);
        return SR_FAILURE;
    }

    if (sidLabelDepth != SR_ZERO)
    {
        /* Fill Stack Labels other than Top-Label */
        for (u1StackCount = SR_ZERO; u1StackCount < sidLabelDepth;
             u1StackCount++)
        {
            u4StaticSrLabel = sidLabel[u1StackCount];
            if (nmhTestv2FsStaticSrLabelStackLabel
                (&u4ErrCode, u4LabelIndex, u1StackCount + SR_ONE,
                 u4StaticSrLabel) == SNMP_FAILURE)
            {
                nmhSetFsStaticSrRowStatus (pSnmpDestAddr, u4Mask,
                                           pDestAddr->u2AddrType, u4LspId, DESTROY);
                return SR_FAILURE;
            }

            if (nmhSetFsStaticSrLabelStackLabel
                (u4LabelIndex, u1StackCount + SR_ONE,
                 u4StaticSrLabel) == SNMP_FAILURE)
            {
                nmhSetFsStaticSrRowStatus (pSnmpDestAddr, u4Mask,
                                           pDestAddr->u2AddrType, u4LspId, DESTROY);
                return SR_FAILURE;
            }
        }
    }

    if (nmhTestv2FsStaticSrNextHopAddr (&u4ErrCode, pSnmpDestAddr, u4Mask,
                                        pDestAddr->u2AddrType, u4LspId,
                                        pSnmpNextHop) == SNMP_FAILURE)
    {
        if (u4ErrCode == SNMP_ERR_WRONG_VALUE)
        {
            CliPrintf (CliHandle, "%%Wrong value for NextHop \r\n");
        }
        else
        {
            CliPrintf (CliHandle,
                       "%%Unable to add Entry in Static Sr Table \r\n");
        }
        return SR_FAILURE;
    }

    if (nmhSetFsStaticSrNextHopAddr (pSnmpDestAddr, u4Mask,
                                     pDestAddr->u2AddrType, u4LspId,
                                     pSnmpNextHop) == SNMP_FAILURE)
    {
        CliPrintf (CliHandle, "%% NextHop Set failed for static Sr\n");
    }

    /*If nexthop is 0, use top-sid nexthop as dynamic-nexthop*/
    if ((nextHop.u2AddrType == SR_IPV4_ADDR_TYPE) && (nextHop.Addr.u4Addr == SR_ZERO)) {
        if (SrStaticGetDynamicNextHop(pDestAddr, u4LspId) == SR_FAILURE) {
            CliPrintf (CliHandle, "%%Warning: Alarm Present\r\n");
            return SR_SUCCESS;
        }
    }

    /* MplsFTNCreate call ends */

    if (nmhTestv2FsStaticSrRowStatus (&u4ErrCode, pSnmpDestAddr, u4Mask,
                                      pDestAddr->u2AddrType, u4LspId,
                                      MPLS_STATUS_ACTIVE) == SNMP_FAILURE)
    {
        CliPrintf (CliHandle,
                   "%%Unable to add Entry (Test-CW) in Static Sr Table \r\n");
        return SR_FAILURE;
    }

    if (nmhSetFsStaticSrRowStatus (pSnmpDestAddr, u4Mask, pDestAddr->u2AddrType, u4LspId,
                                   MPLS_STATUS_ACTIVE) == SNMP_FAILURE)
    {
        CliPrintf (CliHandle,
                   "%%Unable to add Entry (Set-CW) in Static Sr Table \r\n");
        return SR_FAILURE;
    }

    nmhGetFsSrMode (&i4SrMode);
    i4SrMode |= SR_CONF_STATIC;
    nmhSetFsSrMode (i4SrMode);

    UNUSED_PARAM (CliHandle);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrCliDelStaticBinding
 * Description   : This routine deletes static binding of labels for static
 *                                 configuration of Segment Routing
 * Input(s)      : CliHandle - Index of current CLI context
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliDelStaticBinding (tCliHandle CliHandle, tGenU4Addr * pDestAddr,
                       UINT4 u4Mask, tGenU4Addr * pNextHopAddr, UINT4 topLabel,
                       UINT4 sidLabel[], UINT4 sidLabelDepth, UINT4 u4LspId)
{
    INT4                i4SrMode = SR_ZERO;
    INT4                i4SrStatus = SR_ZERO;
    UINT4               u4LabelCounter = SR_ZERO;
    tGenU4Addr          destPrefix;
    tGenU4Addr          nextHop;
    tGenU4Addr          mask;
    tFtnEntry          *pFtnEntry = NULL;
    tXcEntry           *pXcEntry = NULL;
    tLblEntry          *pLblEntry = NULL;

    UINT4               u4ErrCode = SR_ZERO;
    UINT1               au1DestAddr[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1nextHop[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    tSNMP_OCTET_STRING_TYPE snmpDestAddr;
    tSNMP_OCTET_STRING_TYPE snmpNextHop;
    tSNMP_OCTET_STRING_TYPE *pSnmpDestAddr = NULL;
    tSNMP_OCTET_STRING_TYPE *pSnmpNextHop = NULL;
    tSrStaticEntryInfo* pStaticSrInfo = NULL;

    MEMSET (&snmpDestAddr, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    MEMSET (&snmpNextHop, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));

    MEMSET (&destPrefix, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&nextHop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&mask, 0, sizeof (tGenU4Addr));

    pSnmpDestAddr = &snmpDestAddr;
    pSnmpNextHop = &snmpNextHop;

    snmpDestAddr.pu1_OctetList = au1DestAddr;
    snmpNextHop.pu1_OctetList = au1nextHop;
    if (pDestAddr->u2AddrType == SR_CLI_IPV4)
    {
        nmhGetFsSrV4Status (&i4SrStatus);
    }
    else if (pDestAddr->u2AddrType == SR_CLI_IPV6)
    {
        nmhGetFsSrV6Status (&i4SrStatus);
    }

    if (pDestAddr->u2AddrType == SR_IPV4_ADDR_TYPE)
    {

        destPrefix.u2AddrType = SR_IPV4_ADDR_TYPE;
        nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;

        MEMCPY (&(destPrefix.Addr.u4Addr), &pDestAddr->Addr.u4Addr,
                SR_IPV4_ADDR_LENGTH);

        /*If nexthop is 0, use dynamic-nexthop*/
        if (pNextHopAddr->Addr.u4Addr == SR_ZERO) {
            pStaticSrInfo = SrGetStaticSrFromDestAddr (pDestAddr, u4LspId);
            if (!pStaticSrInfo) {
                CliPrintf (CliHandle, "%% No such entry exists\r\n");
                return SR_FAILURE;
            }

            if (pStaticSrInfo->DynamicNextHop.Addr.u4Addr == SR_ZERO) {
                SR_INTEGER_TO_OCTETSTRING (pDestAddr->Addr.u4Addr, pSnmpDestAddr);
                if (nmhSetFsStaticSrRowStatus (pSnmpDestAddr, u4Mask, pDestAddr->u2AddrType, u4LspId, DESTROY) == SNMP_FAILURE) {
                    CliPrintf (CliHandle, "%%Unable to remove Entry in Static Sr Table \r\n");
                    return SR_FAILURE;
                }

                nmhGetFsSrMode (&i4SrMode);
                i4SrMode &= ~SR_CONF_STATIC;
                nmhSetFsSrMode (i4SrMode);

                return SR_SUCCESS;
            }

            MEMCPY (&(nextHop.Addr.u4Addr), &(pStaticSrInfo->DynamicNextHop.Addr.u4Addr), SR_IPV4_ADDR_LENGTH);
        }
        else {
            MEMCPY (&(nextHop.Addr.u4Addr), &pNextHopAddr->Addr.u4Addr, SR_IPV4_ADDR_LENGTH);
        }

        mask.Addr.u4Addr = u4Mask;
        mask.u2AddrType = MPLS_IPV4_ADDR_TYPE;

        MplsGetPrefix (&destPrefix.Addr, &mask.Addr,
                       destPrefix.u2AddrType, &destPrefix.Addr);

        MPLS_CMN_LOCK ();
        pFtnEntry =
            MplsSignalGetFtnTableEntryWithOwner (destPrefix.Addr.u4Addr,
                                        nextHop.Addr.u4Addr, MPLS_OWNER_SR_TE, u4LspId, 0);
        MPLS_CMN_UNLOCK ();
    }
    else if (pDestAddr->u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        destPrefix.u2AddrType = SR_IPV6_ADDR_TYPE;
        nextHop.u2AddrType = SR_IPV6_ADDR_TYPE;

        MEMCPY (destPrefix.Addr.Ip6Addr.u1_addr,
                pDestAddr->Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);
        MEMCPY (nextHop.Addr.Ip6Addr.u1_addr,
                pNextHopAddr->Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);

        MEMCPY (mask.Addr.Ip6Addr.u1_addr, &u4Mask, sizeof (UINT4));
        mask.u2AddrType = SR_IPV6_ADDR_TYPE;

        MplsGetPrefix (&destPrefix.Addr, &mask.Addr,
                       destPrefix.u2AddrType, &destPrefix.Addr);
#ifdef MPLS_IPV6_WANTED
        MPLS_CMN_LOCK ();
        pFtnEntry = MplsSignalGetFtnIpv6TableEntry (&destPrefix.Addr.Ip6Addr);
        MPLS_CMN_UNLOCK ();
#endif
    }

    if (pFtnEntry != NULL)
    {
        pXcEntry = (tXcEntry *) pFtnEntry->pActionPtr;

        if ((pXcEntry != NULL) && (pXcEntry->pOutIndex != NULL))
        {
            if (topLabel != pXcEntry->pOutIndex->u4Label)
            {
                CliPrintf (CliHandle,
                           "%% Label Stack do not match with existing FTN entry\r\n");
                return SR_FAILURE;
            }

            if (pXcEntry->mplsLabelIndex != NULL)
            {
                TMO_SLL_Scan (&(pXcEntry->mplsLabelIndex->LblList), pLblEntry,
                              tLblEntry *)
                {
                    if (pLblEntry->u4Label != sidLabel[u4LabelCounter++])
                    {
                        CliPrintf (CliHandle,
                                   "%% Label Stack do not match with existing FTN entry\r\n");
                        return SR_FAILURE;
                    }
                }

                if (sidLabelDepth != u4LabelCounter)
                {
                    CliPrintf (CliHandle,
                               "%% Label Stack do not match with existing FTN entry\r\n");
                    return SR_FAILURE;
                }
            }
            else if (sidLabelDepth != SR_ZERO)
            {
                CliPrintf (CliHandle,
                           "%% Label Stack do not match with existing FTN entry\r\n");
                return SR_FAILURE;
            }
            /* MplsFTNDelete call starts */
            SR_INTEGER_TO_OCTETSTRING (pDestAddr->Addr.u4Addr, pSnmpDestAddr);
            SR_INTEGER_TO_OCTETSTRING (pNextHopAddr->Addr.u4Addr, pSnmpNextHop);
            if (nmhTestv2FsStaticSrRowStatus (&u4ErrCode, pSnmpDestAddr, u4Mask,
                                              pDestAddr->u2AddrType, u4LspId,
                                              DESTROY) == SNMP_FAILURE)
            {
                CliPrintf (CliHandle,
                           "%%Unable to remove Entry (Test-CW) in Static Sr Table \r\n");
                return SR_FAILURE;
            }

            if (nmhSetFsStaticSrRowStatus
                (pSnmpDestAddr, u4Mask, pDestAddr->u2AddrType, u4LspId,
                 DESTROY) == SNMP_FAILURE)
            {
                CliPrintf (CliHandle,
                           "%%Unable to remove Entry (Set-CW) in Static Sr Table \r\n");
                return SR_FAILURE;
            }

            /* MplsFTNDelete call ends */
        }
        else
        {
            CliPrintf (CliHandle,
                       "%% Cannot fetch existing FTN entry details\r\n");
            return SR_FAILURE;
        }
    }
    else
    {
        CliPrintf (CliHandle, "%% No such entry exists\r\n");
        return SR_FAILURE;
    }
    nmhGetFsSrMode (&i4SrMode);
    i4SrMode &= ~SR_CONF_STATIC;
    nmhSetFsSrMode (i4SrMode);

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrMplsSetAllFTNRowStatus
 * Description   : This routine is used to make Row Status of all STATIC FTNs
 *                 created by SR as NIS/ACTIVE
 *
 * Input         : u4RowStatus - Row status of static FTNs to be made
 *                  if u4RowStatus == NIS
 *                  if u4RowStatus == ACTIVE
 *
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrMplsSetAllFTNRowStatus (INT4 i4RowStatus)
{
    tXcEntry           *pXcEntry = NULL;
    tFtnEntry          *pFtnEntry = NULL;
    UINT4               u4FtnIndex = SR_ZERO;
    UINT4               u4XCIndex = SR_ZERO;
    UINT4               u4InIndex = SR_ZERO;
    UINT4               u4OutIndex = SR_ZERO;
    UINT4               u4ErrCode = SR_ZERO;
    UINT4               u4L3Intf = SR_ZERO;
    INT4                i4IfIndex = SR_ZERO;

    static UINT1        au1XCIndex[MPLS_INDEX_LENGTH];
    static UINT4        au4ActionPointer[MPLS_TE_XC_TABLE_OFFSET];
    static UINT1        au1InSegIndex[MPLS_INDEX_LENGTH];
    static UINT1        au1OutSegIndex[MPLS_INDEX_LENGTH];

    tSNMP_OCTET_STRING_TYPE InSegmentIndex;
    tSNMP_OCTET_STRING_TYPE XCIndex;
    tSNMP_OCTET_STRING_TYPE OutSegmentIndex;
    tSNMP_OID_TYPE      ActionPointer;

    ActionPointer.pu4_OidList = au4ActionPointer;
    XCIndex.pu1_OctetList = au1XCIndex;
    XCIndex.i4_Length = MPLS_INDEX_LENGTH;
    InSegmentIndex.pu1_OctetList = au1InSegIndex;
    InSegmentIndex.i4_Length = MPLS_INDEX_LENGTH;
    OutSegmentIndex.pu1_OctetList = au1OutSegIndex;
    OutSegmentIndex.i4_Length = MPLS_INDEX_LENGTH;

    MEMSET (au1InSegIndex, 0, MPLS_INDEX_LENGTH);
    MEMSET (au1OutSegIndex, 0, MPLS_INDEX_LENGTH);
    MEMSET (au1XCIndex, 0, MPLS_INDEX_LENGTH);
    MEMSET (&u4XCIndex, 0, sizeof (UINT4));

    SR_TRC2 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d ENTRY \n", __func__, __LINE__);

    switch (i4RowStatus)
    {
        case MPLS_STATUS_NOT_INSERVICE:
            SR_TRC2 (SR_CTRL_TRC, "%s:%d MPLS_STATUS_NOT_INSERVICE \n", __func__,
                     __LINE__);
            while (1)
            {
                if ((u4FtnIndex != 0) &&
                    ((nmhGetNextIndexMplsFTNTable (u4FtnIndex, &u4FtnIndex)) ==
                     SNMP_FAILURE))
                {
                    break;
                }

                /* Only executed first time */
                if ((u4FtnIndex == 0) &&
                    ((nmhGetFirstIndexMplsFTNTable (&u4FtnIndex)) ==
                     SNMP_FAILURE))
                {
                    break;
                }

                if ((nmhGetMplsFTNActionPointer (u4FtnIndex, &ActionPointer))
                    == SNMP_FAILURE)
                {
                    break;
                }

                MPLS_OID_TO_INTEGER ((&ActionPointer), u4XCIndex,
                                     MPLS_XC_INDEX_START_OFFSET);
                MPLS_INTEGER_TO_OCTETSTRING (u4XCIndex, (&XCIndex));

                MPLS_OID_TO_INTEGER ((&ActionPointer), u4InIndex,
                                     MPLS_IN_INDEX_START_OFFSET);
                MPLS_INTEGER_TO_OCTETSTRING (u4InIndex, (&InSegmentIndex));

                MPLS_OID_TO_INTEGER ((&ActionPointer), u4OutIndex,
                                     MPLS_OUT_INDEX_START_OFFSET);
                MPLS_INTEGER_TO_OCTETSTRING (u4OutIndex, (&OutSegmentIndex));
                MPLS_CMN_LOCK ();
                if ((pFtnEntry = MplsGetFtnTableEntry (u4FtnIndex)) == NULL)
                {
                    MPLS_CMN_UNLOCK ();
                    return SR_FAILURE;
                }

                pXcEntry = pFtnEntry->pActionPtr;
                if (pXcEntry == NULL)
                {
                    MPLS_CMN_UNLOCK ();
                    continue;
                }

                /* If LSP is not created by Static Config, continue */
                if (pXcEntry->u1Owner != MPLS_OWNER_SNMP)
                {
                    MPLS_CMN_UNLOCK ();
                    continue;
                }

                /* If LSP is not created by SR, continue */
                if (pXcEntry->u1IsLspCreatedBySR == MPLS_FALSE)
                {
                    MPLS_CMN_UNLOCK ();
                    continue;
                }

                MPLS_CMN_UNLOCK ();

                /* Lower level routine for making FTN row status NIS */
                if ((nmhSetMplsFTNRowStatus
                     (u4FtnIndex, MPLS_STATUS_NOT_INSERVICE)) == SNMP_FAILURE)
                {
                    return SR_FAILURE;
                }
                if ((nmhTestv2MplsXCRowStatus (&u4ErrCode, &XCIndex,
                                               &InSegmentIndex,
                                               &OutSegmentIndex,
                                               i4RowStatus)) == SNMP_FAILURE)
                {
                    return SR_FAILURE;
                }

                if ((nmhSetMplsXCRowStatus (&XCIndex, &InSegmentIndex,
                                            &OutSegmentIndex,
                                            i4RowStatus)) == SNMP_FAILURE)
                {
                    return SR_FAILURE;
                }

                if (u4OutIndex != 0)
                {
                    if ((nmhTestv2MplsOutSegmentRowStatus (&u4ErrCode,
                                                           &OutSegmentIndex,
                                                           i4RowStatus)) ==
                        SNMP_FAILURE)
                    {
                        return SR_FAILURE;
                    }
                    if ((nmhSetMplsOutSegmentRowStatus
                         (&OutSegmentIndex, i4RowStatus)) == SNMP_FAILURE)
                    {
                        return SR_FAILURE;
                    }

                }
            }
            break;
        case MPLS_STATUS_ACTIVE:

            SR_TRC2 (SR_CTRL_TRC, "%s:%d MPLS_STATUS_ACTIVE \n", __func__,
                     __LINE__);

            while (1)
            {
                if ((u4FtnIndex != 0) &&
                    ((nmhGetNextIndexMplsFTNTable (u4FtnIndex, &u4FtnIndex)) ==
                     SNMP_FAILURE))
                {
                    break;
                }

                /* Only executed first time */
                if ((u4FtnIndex == 0) &&
                    ((nmhGetFirstIndexMplsFTNTable (&u4FtnIndex)) ==
                     SNMP_FAILURE))
                {
                    break;
                }

                MPLS_CMN_LOCK ();
                if ((pFtnEntry = MplsGetFtnTableEntry (u4FtnIndex)) == NULL)
                {
                    MPLS_CMN_UNLOCK ();
                    return SR_FAILURE;
                }

                pXcEntry = pFtnEntry->pActionPtr;
                if (pXcEntry == NULL)
                {
                    MPLS_CMN_UNLOCK ();
                    continue;
                }

                /* If LSP is not created by Static Config, continue */
                if (pXcEntry->u1Owner != MPLS_OWNER_SNMP)
                {
                    MPLS_CMN_UNLOCK ();
                    continue;
                }
                /* If LSP is not created by SR, continue */
                if (pXcEntry->u1IsLspCreatedBySR == MPLS_FALSE)
                {
                    MPLS_CMN_UNLOCK ();
                    continue;
                }

                MPLS_CMN_UNLOCK ();

                /* Lower level routine for making FTN row status NIS */
                if ((nmhSetMplsFTNRowStatus
                     (u4FtnIndex, MPLS_STATUS_ACTIVE)) == SNMP_FAILURE)
                {
                    return SR_FAILURE;
                }

            }

            break;
        case MPLS_STATUS_DESTROY:

            SR_TRC2 (SR_CTRL_TRC, "%s:%d MPLS_STATUS_ACTIVE \n", __func__,
                     __LINE__);

            while (1)
            {
                if ((u4FtnIndex != 0) &&
                    ((nmhGetNextIndexMplsFTNTable (u4FtnIndex, &u4FtnIndex)) ==
                     SNMP_FAILURE))
                {
                    break;
                }

                /* Only executed first time */
                if ((u4FtnIndex == 0) &&
                    ((nmhGetFirstIndexMplsFTNTable (&u4FtnIndex)) ==
                     SNMP_FAILURE))
                {
                    break;
                }
                if ((nmhGetMplsFTNActionPointer (u4FtnIndex, &ActionPointer))
                    == SNMP_FAILURE)
                {
                    break;
                }

                MPLS_OID_TO_INTEGER ((&ActionPointer), u4XCIndex,
                                     MPLS_XC_INDEX_START_OFFSET);
                MPLS_INTEGER_TO_OCTETSTRING (u4XCIndex, (&XCIndex));

                MPLS_OID_TO_INTEGER ((&ActionPointer), u4InIndex,
                                     MPLS_IN_INDEX_START_OFFSET);
                MPLS_INTEGER_TO_OCTETSTRING (u4InIndex, (&InSegmentIndex));

                MPLS_OID_TO_INTEGER ((&ActionPointer), u4OutIndex,
                                     MPLS_OUT_INDEX_START_OFFSET);
                MPLS_INTEGER_TO_OCTETSTRING (u4OutIndex, (&OutSegmentIndex));

                MPLS_CMN_LOCK ();
                if ((pFtnEntry = MplsGetFtnTableEntry (u4FtnIndex)) == NULL)
                {
                    MPLS_CMN_UNLOCK ();
                    return SR_FAILURE;
                }

                pXcEntry = pFtnEntry->pActionPtr;
                if (pXcEntry == NULL)
                {
                    MPLS_CMN_UNLOCK ();
                    continue;
                }

                /* If LSP is not created by Static Config, continue */
                if (pXcEntry->u1Owner != MPLS_OWNER_SNMP)
                {
                    MPLS_CMN_UNLOCK ();
                    continue;
                }
                /* If LSP is not created by SR, continue */
                if (pXcEntry->u1IsLspCreatedBySR == MPLS_FALSE)
                {
                    MPLS_CMN_UNLOCK ();
                    continue;
                }

                MPLS_CMN_UNLOCK ();

                /* Lower level routine for making FTN row status NIS */
                if ((nmhSetMplsFTNRowStatus
                     (u4FtnIndex, MPLS_STATUS_DESTROY)) == SNMP_FAILURE)
                {
                    return SR_FAILURE;
                }
                if ((nmhTestv2MplsXCRowStatus (&u4ErrCode, &XCIndex,
                                               &InSegmentIndex,
                                               &OutSegmentIndex,
                                               i4RowStatus)) == SNMP_FAILURE)
                {
                    return SR_FAILURE;
                }

                if ((nmhSetMplsXCRowStatus (&XCIndex, &InSegmentIndex,
                                            &OutSegmentIndex,
                                            i4RowStatus)) == SNMP_FAILURE)
                {
                    return SR_FAILURE;
                }
                if (u4OutIndex != 0)
                {
                    /* get the interface index b4 deleting the out seg entry */
                    if ((nmhGetMplsOutSegmentInterface (&OutSegmentIndex,
                                                        &i4IfIndex)) ==
                        SNMP_FAILURE)
                    {
                        return SR_FAILURE;
                    }

                    if ((nmhTestv2MplsOutSegmentRowStatus (&u4ErrCode,
                                                           &OutSegmentIndex,
                                                           i4RowStatus)) ==
                        SNMP_FAILURE)
                    {
                        return SR_FAILURE;
                    }
                    if ((nmhSetMplsOutSegmentRowStatus
                         (&OutSegmentIndex, i4RowStatus)) == SNMP_FAILURE)
                    {
                        return SR_FAILURE;
                    }

                    if ((CfaUtilGetIfIndexFromMplsTnlIf
                         ((UINT4) i4IfIndex, &u4L3Intf, TRUE)) == CFA_FAILURE)
                    {
                        return SR_FAILURE;
                    }
                    if (MplsDeleteMplsIfOrMplsTnlIf
                        (u4L3Intf, (UINT4) i4IfIndex, CFA_MPLS_TUNNEL,
                         MPLS_TRUE) == MPLS_FAILURE)
                    {
                        return SR_FAILURE;
                    }
                }
            }
            break;
        default:
            SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d FAILURE EXIT \n", __func__, __LINE__);
            return SR_FAILURE;
    }

    SR_TRC2 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrMplsSetAllILMRowStatus
 * Description   : This routine is used to make Row Status of all STATIC ILMs
 *                 created by SR as NIS/ACTIVE
 *
 * Input         : u4RowStatus - Row status of static ILMs to be made
 *                  if u4RowStatus == NIS
 *                  if u4RowStatus == ACTIVE
 *
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrMplsSetAllILMRowStatus (INT4 i4RowStatus)
{
    tXcEntry           *pXcEntry = NULL;
    UINT4               u4XcIndex = SR_ZERO;
    UINT4               u4InIndex = SR_ZERO;
    UINT4               u4OutIndex = SR_ZERO;

    SR_TRC2 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d ENTRY \n", __func__, __LINE__);

    switch (i4RowStatus)
    {
        case MPLS_STATUS_NOT_INSERVICE:

            SR_TRC2 (SR_CTRL_TRC, "%s:%d MPLS_STATUS_NOT_INSERVICE \n", __func__,
                     __LINE__);

            while (1)
            {
                MPLS_CMN_LOCK ();
                if (u4XcIndex != SR_ZERO)
                {
                    pXcEntry =
                        MplsXCTableNextEntry (u4XcIndex, u4InIndex, u4OutIndex);
                    if (pXcEntry == NULL)
                    {
                        MPLS_CMN_UNLOCK ();
                        break;
                    }
                }

                /* Only executed first time to get first XcEntry */
                if (u4XcIndex == SR_ZERO)
                {
                    pXcEntry = MplsXCTableNextEntry (SR_ZERO, SR_ZERO, SR_ZERO);
                    if (pXcEntry == NULL)
                    {
                        MPLS_CMN_UNLOCK ();
                        break;
                    }
                }
                u4XcIndex = pXcEntry->u4Index;

                if (XC_ININDEX (pXcEntry) == NULL)
                {
                    u4InIndex = SR_ZERO;
                }
                else
                {
                    u4InIndex = XC_ININDEX (pXcEntry)->u4Index;
                }

                if (XC_OUTINDEX (pXcEntry) == NULL)
                {
                    u4OutIndex = SR_ZERO;
                }
                else
                {
                    u4OutIndex = XC_OUTINDEX (pXcEntry)->u4Index;
                }

                /* If LSP is not created by Static Config, continue */
                if (pXcEntry->u1Owner != MPLS_OWNER_SNMP)
                {
                    MPLS_CMN_UNLOCK ();
                    continue;
                }

                /* If LSP is not created by SR, continue */
                if (pXcEntry->u1IsLspCreatedBySR == MPLS_FALSE)
                {
                    MPLS_CMN_UNLOCK ();
                    continue;
                }

                MPLS_CMN_UNLOCK ();
                /* Lower level routine for making ILM(XC,IN,OUT) row status NIS */
                if ((SrSetMplsILMRowStatus (u4XcIndex, u4InIndex, u4OutIndex,
                                            i4RowStatus)) == SR_FAILURE)
                {
                    return SR_FAILURE;
                }
                else
                {
                    SR_TRC5 (SR_CTRL_TRC,
                             "%s:%d u4XcIndex %d u4InIndex %d u4OutIndex %d : MPLS_STATUS_NOT_INSERVICE \n",
                             __func__, __LINE__, u4XcIndex, u4InIndex,
                             u4OutIndex);
                }
            }

            break;
        case MPLS_STATUS_ACTIVE:

            SR_TRC2 (SR_CTRL_TRC, "%s:%d MPLS_STATUS_ACTIVE \n", __func__,
                     __LINE__);

            while (1)
            {
                MPLS_CMN_LOCK ();
                if (u4XcIndex != SR_ZERO)
                {
                    pXcEntry =
                        MplsXCTableNextEntry (u4XcIndex, u4InIndex, u4OutIndex);
                    if (pXcEntry == NULL)
                    {
                        MPLS_CMN_UNLOCK ();
                        break;
                    }
                }

                /* Only executed first time to get first XcEntry */
                if (u4XcIndex == SR_ZERO)
                {
                    pXcEntry = MplsXCTableNextEntry (SR_ZERO, SR_ZERO, SR_ZERO);
                    if (pXcEntry == NULL)
                    {
                        MPLS_CMN_UNLOCK ();
                        break;
                    }
                }
                u4XcIndex = pXcEntry->u4Index;

                if (XC_ININDEX (pXcEntry) == NULL)
                {
                    u4InIndex = SR_ZERO;
                }
                else
                {
                    u4InIndex = XC_ININDEX (pXcEntry)->u4Index;
                }

                if (XC_OUTINDEX (pXcEntry) == NULL)
                {
                    u4OutIndex = SR_ZERO;
                }
                else
                {
                    u4OutIndex = XC_OUTINDEX (pXcEntry)->u4Index;
                }

                /* If LSP is not created by Static Config, continue */
                if (pXcEntry->u1Owner != MPLS_OWNER_SNMP)
                {
                    MPLS_CMN_UNLOCK ();
                    continue;
                }

                /* If LSP is not created by SR, continue */
                if (pXcEntry->u1IsLspCreatedBySR == MPLS_FALSE)
                {
                    MPLS_CMN_UNLOCK ();
                    continue;
                }

                MPLS_CMN_UNLOCK ();

                /* Lower level routine for making ILM(XC,IN,OUT) row status ACTIVE */
                if ((SrSetMplsILMRowStatus (u4XcIndex, u4InIndex, u4OutIndex,
                                            i4RowStatus)) == SR_FAILURE)
                {
                    return SR_FAILURE;
                }
                else
                {
                    SR_TRC5 (SR_CTRL_TRC,
                             "%s:%d u4XcIndex %d u4InIndex %d u4OutIndex %d : ACTIVE \n",
                             __func__, __LINE__, u4XcIndex, u4InIndex,
                             u4OutIndex);
                }

            }

            break;
        case MPLS_STATUS_DESTROY:

            SR_TRC2 (SR_CTRL_TRC, "%s:%d MPLS_STATUS_DESTROY \n", __func__,
                     __LINE__);

            while (1)
            {
                MPLS_CMN_LOCK ();
                if (u4XcIndex != SR_ZERO)
                {
                    pXcEntry =
                        MplsXCTableNextEntry (u4XcIndex, u4InIndex, u4OutIndex);
                    if (pXcEntry == NULL)
                    {
                        MPLS_CMN_UNLOCK ();
                        break;
                    }
                }

                /* Only executed first time to get first XcEntry */
                if (u4XcIndex == SR_ZERO)
                {
                    pXcEntry = MplsXCTableNextEntry (SR_ZERO, SR_ZERO, SR_ZERO);
                    if (pXcEntry == NULL)
                    {
                        MPLS_CMN_UNLOCK ();
                        break;
                    }
                }
                u4XcIndex = pXcEntry->u4Index;

                if (XC_ININDEX (pXcEntry) == NULL)
                {
                    u4InIndex = SR_ZERO;
                }
                else
                {
                    u4InIndex = XC_ININDEX (pXcEntry)->u4Index;
                }

                if (XC_OUTINDEX (pXcEntry) == NULL)
                {
                    u4OutIndex = SR_ZERO;
                }
                else
                {
                    u4OutIndex = XC_OUTINDEX (pXcEntry)->u4Index;
                }

                /* If LSP is not created by Static Config, continue */
                if (pXcEntry->u1Owner != MPLS_OWNER_SNMP)
                {
                    MPLS_CMN_UNLOCK ();
                    continue;
                }

                /* If LSP is not created by SR, continue */
                if (pXcEntry->u1IsLspCreatedBySR == MPLS_FALSE)
                {
                    MPLS_CMN_UNLOCK ();
                    continue;
                }

                MPLS_CMN_UNLOCK ();
                /* Lower level routine for making ILM(XC,IN,OUT) row status ACTIVE */
                if ((SrSetMplsILMRowStatus (u4XcIndex, u4InIndex, u4OutIndex,
                                            i4RowStatus)) == SR_FAILURE)
                {
                    return SR_FAILURE;
                }
                else
                {
                    SR_TRC5 (SR_CTRL_TRC,
                             "%s:%d u4XcIndex %d u4InIndex %d u4OutIndex %d : DESTROYED \n",
                             __func__, __LINE__, u4XcIndex, u4InIndex,
                             u4OutIndex);
                }
            }

            break;

        default:
            SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d FAILURE EXIT \n", __func__, __LINE__);
            return SR_FAILURE;
    }

    SR_TRC2 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************
* Function Name : SrSetOspfSrStatus
* Description   : This routine Sets the Ospf Segment Routing Status.
* Input(s)      : u4OspfContextId - Ospf Context Id
*                 u4srStatus - Enable/Disable
* Output(s)     : None
* Return(s)     : SR_SUCCESS / SR_FAILURE
*****************************************************************************/
UINT4
SrSetOspfSrStatus (UINT4 u4OspfContextId, UINT4 u4srStatus)
{
    UINT4               u4ErrCode = SR_ZERO;
    INT4                i4SrMode = SR_ZERO;
    INT4                i4RetStatus = SR_FAILURE;
    INT4                i4RetValue = SR_FAILURE;
    UINT4               u4SrAltStatus = SR_ZERO;
    INT4                i4SrStatus = SR_ZERO;

    SR_TRC1 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s ENTRY \n", __func__);

    if (nmhTestv2FsSrMode (&u4ErrCode, SR_CONF_OSPF) == SNMP_FAILURE)
    {
        return SR_FAILURE;
    }

    nmhGetFsSrMode (&i4SrMode);
    nmhGetFsSrV4Status (&i4SrStatus);

    if (u4OspfContextId == OSPF_DEFAULT_CXT_ID)
    {
        if (u4srStatus == SR_ENABLED)
        {
            SR_UNLOCK ();
            if(i4SrStatus == SR_ENABLED)
            {
               if (SrSndMsgToOspf (SR_OSPF_REG,
                                u4OspfContextId, NULL) != SR_SUCCESS)
               {
                   gSrGlobalInfo.u1SrRegStatus = SR_FALSE;
                   SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "Registeration failed\n");
                   return SR_FAILURE;
               }
            }
            SR_LOCK ();
            i4SrMode |= SR_CONF_OSPF;
            nmhSetFsSrMode (i4SrMode);
            if (gSrGlobalInfo.u1SrOspfDisable == SR_TRUE)
            {
                SrOspfUpEventHandler ();
                gSrGlobalInfo.u1SrOspfDisable = SR_FALSE;
            }

	    /* Check for segment-routing alternate status */
	    /* and register with RTM for installing the routes */
	    nmhGetFsSrV4AlternateStatus (&u4SrAltStatus);
	    if (u4SrAltStatus == ALTERNATE_ENABLED)
            {
                SrRegisterWithRtmForAlt();
            }
        }
        else
        {
            /* Set u1OspfDeRegInProgress to TRUE to indiacte that SR OSPF is going to be degeristered */
            gSrGlobalInfo.u1OspfDeRegInProgress = SR_TRUE;
            if (SR_SUCCESS == SrCliProcessAdminDownCmdForV4 (SR_SHUT_CMD))
            {
                SR_TRC (SR_CTRL_TRC, "Mark Admin State Down\n");
            }
            i4RetStatus = SrCliProcessAdminDownCmdForV4 (SR_DESTROY_CMD);
            if (i4RetStatus != SR_SUCCESS)
            {
                return SR_FAILURE;
            }

            if (SrSndMsgToOspf (SR_OSPF_DEREG,
                                u4OspfContextId, NULL) != SR_SUCCESS)
            {
                SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "Registeration failed\n");
                return SR_FAILURE;
            }
            /* Reset SR Mode */
            i4SrMode &= ~SR_CONF_OSPF;
            nmhSetFsSrMode (i4SrMode);
            gSrGlobalInfo.u1SrRegStatus = SR_TRUE;
            /* Check whether disable of IPv4 Address family is configured or not */
            if (gu4AddrV4Family == SR_ONE)
            {
                i4RetValue = SrCliConfigAddrFamily (SR_CLI_IPV4, SR_DISABLE);
                if (i4RetValue == SR_SUCCESS)
                {
                    gu4AddrV4Family = SR_ZERO;
                    SR_TRC (SR_CTRL_TRC,
                            "IPv4 Address family disable is success \n\r");
                }
                else
                {
                    SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                            "Failure in IPv4 Address family disable \n\r");
                }
            }
            /* Check whether disable of MPLS is configured or not */
            if ((i4SrMode == SR_ZERO) && (gu4SegmentMpls == SR_ONE))
            {
                i4RetValue = SrCliDisableSrMpls ();
                if (i4RetValue == SR_SUCCESS)
                {
                    gu4SegmentMpls = SR_ZERO;
                    SR_TRC (SR_CTRL_TRC,
                            "MPLS segment routing disable is success \n\r");
                }
                else
                {
                    SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                            "Failure in MPLS segment routing disable \n\r");
                }
            }
            gSrGlobalInfo.u1SrOspfDisable = SR_TRUE;
            gSrGlobalInfo.u1OspfDeRegInProgress = SR_FALSE;
        }
    }
    SR_TRC1 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s EXIT \n", __func__);

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrDeleteSidTable
 * Description   : This routine deletes all the entries from SID Table.
 * Input(s)      : CliHandle - Index of current CLI context
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrDeleteSidTable (UINT1 u1AddrType)
{
    tSrSidInterfaceInfo *pSrSidEntry = NULL;
    tSrSidInterfaceInfo srSidInfo;

    MEMSET (&srSidInfo, SR_ZERO, sizeof (tSrSidInterfaceInfo));

    pSrSidEntry = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);

    while (pSrSidEntry != NULL)
    {
        if (pSrSidEntry->ipAddrType == SR_IPV4_ADDR_TYPE)
        {
            MEMCPY (&(srSidInfo.ifIpAddr.Addr.u4Addr),
                    &(pSrSidEntry->ifIpAddr.Addr.u4Addr), SR_IPV4_ADDR_LENGTH);
            srSidInfo.ifIpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
        }
        else if (pSrSidEntry->ipAddrType == SR_IPV6_ADDR_TYPE)
        {
            MEMCPY (srSidInfo.ifIpAddr.Addr.Ip6Addr.u1_addr,
                    pSrSidEntry->ifIpAddr.Addr.Ip6Addr.u1_addr,
                    SR_IPV6_ADDR_LENGTH);
            srSidInfo.ifIpAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
        }
        srSidInfo.ipAddrType = pSrSidEntry->ipAddrType;

        if (pSrSidEntry->ipAddrType != u1AddrType)
        {
            pSrSidEntry = RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree,
                                         &srSidInfo, NULL);
            continue;
        }
        SrDelSid (pSrSidEntry);
        pSrSidEntry = RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree,
                                     &srSidInfo, NULL);
    }

    if (u1AddrType == SR_IPV6_ADDR_TYPE)
    {
        gSrGlobalInfo.pV6SrSelfNodeInfo = NULL;
        SR_IPV6_NODE_SID = SR_ZERO;
    }

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrCliTestAndSetTeConfig
 * Description   : This routine create a SR TE path for a destination
 * Input(s)      : CliHandle - Cli Handle
 *                 *pDestAddr - Pointer to TE destination address
 *                 u4DestMask - destination mask
 *                 pMandatoryRtrId - Pointer to the Mandatory Rtr ID
 *                 au4OptRouterIds - Array of optional router id
 *                 u1NoOfOptRouters - No. of optional routers
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliTestAndSetTeConfig (tGenU4Addr * pDestAddr, UINT4 u4DestMask,
                         tGenU4Addr * pMandatoryRtrId,
                         tGenU4Addr au4OptRouterIds[], UINT1 u1NoOfOptRouters)
{
    tCliHandle          CliHandle = SR_ZERO;
    UINT4               u4ErrCode = SR_ZERO;
    UINT1               au1DestAddr[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1MandatoryRtrId[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1OptionalRtrId[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1DestAddrV6[SR_IPV6_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1MandatoryRtrIdV6[SR_IPV6_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1OptionalRtrIdV6[SR_IPV6_ADDR_LENGTH] = { SR_ZERO };
    UINT4               u4counter = SR_ZERO;
    UINT4               u4RtrListIndex = SR_ZERO;
    tSNMP_OCTET_STRING_TYPE snmpDestAddr;
    tSNMP_OCTET_STRING_TYPE snmpMandatoryRtrId;
    tSNMP_OCTET_STRING_TYPE *pSnmpDestAddr = NULL;
    tSNMP_OCTET_STRING_TYPE *pSnmpMandatoryRtrId = NULL;

    tSNMP_OCTET_STRING_TYPE snmpOptionalRtrId;
    tSNMP_OCTET_STRING_TYPE *pSnmpOptionalRtrId = NULL;

    MEMSET (&snmpDestAddr, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    MEMSET (&snmpMandatoryRtrId, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    MEMSET (&snmpOptionalRtrId, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));

    pSnmpDestAddr = &snmpDestAddr;
    pSnmpMandatoryRtrId = &snmpMandatoryRtrId;
    pSnmpOptionalRtrId = &snmpOptionalRtrId;

    if (pDestAddr->u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        snmpDestAddr.pu1_OctetList = au1DestAddr;
        snmpMandatoryRtrId.pu1_OctetList = au1MandatoryRtrId;
        snmpOptionalRtrId.pu1_OctetList = au1OptionalRtrId;

        SR_INTEGER_TO_OCTETSTRING (pDestAddr->Addr.u4Addr, pSnmpDestAddr);
        SR_INTEGER_TO_OCTETSTRING (pMandatoryRtrId->Addr.u4Addr,
                                   pSnmpMandatoryRtrId);
    }
    else if (pDestAddr->u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        snmpDestAddr.pu1_OctetList = au1DestAddrV6;
        snmpMandatoryRtrId.pu1_OctetList = au1MandatoryRtrIdV6;
        snmpOptionalRtrId.pu1_OctetList = au1OptionalRtrIdV6;

        MEMCPY (pSnmpDestAddr->pu1_OctetList, pDestAddr->Addr.Ip6Addr.u1_addr,
                SR_IPV6_ADDR_LENGTH);
        MEMCPY (pSnmpMandatoryRtrId->pu1_OctetList,
                pMandatoryRtrId->Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);

        pSnmpDestAddr->i4_Length = SR_IPV6_ADDR_LENGTH;
        pSnmpMandatoryRtrId->i4_Length = SR_IPV6_ADDR_LENGTH;
    }
    /* Create TE-Path Entry */
    if (nmhTestv2FsSrTePathRowStatus (&u4ErrCode, pSnmpDestAddr, u4DestMask,
                                      pSnmpMandatoryRtrId,
                                      pDestAddr->u2AddrType,
                                      MPLS_STATUS_CREATE_AND_WAIT) ==
        SNMP_FAILURE)
    {
        if (u4ErrCode == SNMP_ERR_BAD_VALUE)
        {
            CliPrintf (CliHandle,
                       "%%DestIpAddr & Loose Hop can't be same \r\n");
        }
        else
        {
            CliPrintf (CliHandle,
                       "%%Unable to add Entry (Test-CW) in TE-Path Table \r\n");
        }
        return SR_FAILURE;
    }

    if (nmhSetFsSrTePathRowStatus (pSnmpDestAddr, u4DestMask,
                                   pSnmpMandatoryRtrId, pDestAddr->u2AddrType,
                                   MPLS_STATUS_CREATE_AND_WAIT) == SNMP_FAILURE)
    {
        CliPrintf (CliHandle,
                   "%%Unable to add Entry (Set-CW) in TE-Path Table \r\n");
        return SR_FAILURE;
    }

    /* Making Row Status to NIS. Row status will be made ACTIVE upon no shut of SR */
    if (nmhTestv2FsSrTePathRowStatus (&u4ErrCode, pSnmpDestAddr, u4DestMask,
                                      pSnmpMandatoryRtrId,
                                      pDestAddr->u2AddrType,
                                      MPLS_STATUS_NOT_INSERVICE) ==
        SNMP_FAILURE)
    {
        CliPrintf (CliHandle,
                   "%%Unable to add Entry (Test-NIS) in TE-Path Table \r\n");
        return SR_FAILURE;
    }

    if (nmhSetFsSrTePathRowStatus (pSnmpDestAddr, u4DestMask,
                                   pSnmpMandatoryRtrId, pDestAddr->u2AddrType,
                                   MPLS_STATUS_NOT_INSERVICE) == SNMP_FAILURE)
    {
        CliPrintf (CliHandle,
                   "%%Unable to add Entry (Set-NIS) in TE-Path Table \r\n");
        return SR_FAILURE;
    }

    if (nmhGetFsSrTeRouterListIndex (pSnmpDestAddr, u4DestMask,
                                     pSnmpMandatoryRtrId, pDestAddr->u2AddrType,
                                     &u4RtrListIndex) == SNMP_FAILURE)
    {
        CliPrintf (CliHandle, "%%Unable to get Router List Index \r\n");
        nmhSetFsSrTePathRowStatus (pSnmpDestAddr, u4DestMask,
                                   pSnmpMandatoryRtrId, pDestAddr->u2AddrType,
                                   DESTROY);
        return SR_FAILURE;
    }

    for (u4counter = SR_ZERO; u4counter < u1NoOfOptRouters; u4counter++)
    {

        /*              au4OptRouterIds[u4counter] = OSIX_NTOHL(au4OptRouterIds[u4counter]); */
        if (au4OptRouterIds[u4counter].u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            SR_INTEGER_TO_OCTETSTRING (au4OptRouterIds[u4counter].Addr.u4Addr,
                                       pSnmpOptionalRtrId);
        }
        else if (au4OptRouterIds[u4counter].u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            MEMCPY (pSnmpOptionalRtrId->pu1_OctetList,
                    au4OptRouterIds[u4counter].Addr.Ip6Addr.u1_addr,
                    SR_IPV6_ADDR_LENGTH);

            pSnmpOptionalRtrId->i4_Length = SR_IPV6_ADDR_LENGTH;
        }

        if (nmhTestv2FsSrTeRtrRowStatus (&u4ErrCode, u4RtrListIndex,
                                         u4counter + SR_ONE,
                                         CREATE_AND_WAIT) == SNMP_FAILURE)
        {
            CliPrintf (CliHandle,
                       "%%Test Failed: Unable to add Optional Router in TE-Path Table \r\n");
            nmhSetFsSrTePathRowStatus (pSnmpDestAddr, u4DestMask,
                                       pSnmpMandatoryRtrId,
                                       pDestAddr->u2AddrType, DESTROY);
            return SR_FAILURE;

        }
        if (nmhSetFsSrTeRtrRowStatus
            (u4RtrListIndex, u4counter + SR_ONE,
             CREATE_AND_WAIT) == SNMP_FAILURE)
        {
            CliPrintf (CliHandle,
                       "%%Set Failed: Unable to add Optional Router in TE-Path Table \r\n");
            nmhSetFsSrTePathRowStatus (pSnmpDestAddr, u4DestMask,
                                       pSnmpMandatoryRtrId,
                                       pDestAddr->u2AddrType, DESTROY);
            return SR_FAILURE;
        }
        if (nmhTestv2FsSrTeOptRouterId (&u4ErrCode, u4RtrListIndex,
                                        u4counter + SR_ONE,
                                        pSnmpOptionalRtrId) == SNMP_FAILURE)
        {
            CliPrintf (CliHandle,
                       "%%Unable to set Optional Router ID in TE-Path Table \r\n");
            nmhSetFsSrTePathRowStatus (pSnmpDestAddr, u4DestMask,
                                       pSnmpMandatoryRtrId,
                                       pDestAddr->u2AddrType, DESTROY);
            return SR_FAILURE;

        }
        if (nmhSetFsSrTeOptRouterId
            (u4RtrListIndex, u4counter + SR_ONE,
             pSnmpOptionalRtrId) == SNMP_FAILURE)
        {
            CliPrintf (CliHandle,
                       "%%Unable to set Optional Router ID in TE-Path Table \r\n");
            nmhSetFsSrTePathRowStatus (pSnmpDestAddr, u4DestMask,
                                       pSnmpMandatoryRtrId,
                                       pDestAddr->u2AddrType, DESTROY);
            return SR_FAILURE;
        }
    }
    if (nmhTestv2FsSrTePathRowStatus (&u4ErrCode, pSnmpDestAddr, u4DestMask,
                                      pSnmpMandatoryRtrId,
                                      pDestAddr->u2AddrType,
                                      MPLS_STATUS_ACTIVE) == SNMP_FAILURE)
    {
        return SR_FAILURE;
    }

    if (nmhSetFsSrTePathRowStatus (pSnmpDestAddr, u4DestMask,
                                   pSnmpMandatoryRtrId, pDestAddr->u2AddrType,
                                   MPLS_STATUS_ACTIVE) == SNMP_FAILURE)
    {
        return SR_FAILURE;
    }

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrCliTestAndDeleteTeConfig
 * Description   : This routine delete the created SR TE path
 * Input(s)      : CliHandle - Cli Handle
 *                 *pDestAddr - Pointer to TE destination address
 *                 u4DestMask - destination mask
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliTestAndDeleteTeConfig (tGenU4Addr * pDestAddr, UINT4 u4DestMask)
{
    tCliHandle          CliHandle = SR_ZERO;
    UINT4               u4ErrCode = SR_ZERO;
    UINT1               au1DestAddr[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1DestAddrV6[SR_IPV6_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1MandatoryRtrId[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1MandatoryRtrIdV6[SR_IPV6_ADDR_LENGTH] = { SR_ZERO };

    tSNMP_OCTET_STRING_TYPE snmpDestAddr;
    tSNMP_OCTET_STRING_TYPE snmpMandatoryRtrId;
    tSNMP_OCTET_STRING_TYPE *pSnmpDestAddr = NULL;
    tSNMP_OCTET_STRING_TYPE *pSnmpMandatoryRtrId = NULL;

    MEMSET (&snmpDestAddr, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    MEMSET (&snmpMandatoryRtrId, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));

    pSnmpDestAddr = &snmpDestAddr;
    pSnmpMandatoryRtrId = &snmpMandatoryRtrId;

    if (pDestAddr->u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        snmpDestAddr.pu1_OctetList = au1DestAddr;
        snmpMandatoryRtrId.pu1_OctetList = au1MandatoryRtrId;

        SR_INTEGER_TO_OCTETSTRING (pDestAddr->Addr.u4Addr, pSnmpDestAddr);
    }
    else if (pDestAddr->u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        snmpDestAddr.pu1_OctetList = au1DestAddrV6;
        snmpMandatoryRtrId.pu1_OctetList = au1MandatoryRtrIdV6;

        MEMCPY (pSnmpDestAddr->pu1_OctetList, pDestAddr->Addr.Ip6Addr.u1_addr,
                SR_IPV6_ADDR_LENGTH);

        pSnmpDestAddr->i4_Length = SR_IPV6_ADDR_LENGTH;
    }

    /* Create TE-Path Entry */
    if (nmhTestv2FsSrTePathRowStatus (&u4ErrCode, pSnmpDestAddr, u4DestMask,
                                      pSnmpMandatoryRtrId,
                                      pDestAddr->u2AddrType,
                                      MPLS_STATUS_DESTROY) == SNMP_FAILURE)
    {
        CliPrintf (CliHandle,
                   "%%Unable to delete Entry (Test-DESTROY) in TE-Path Table \r\n");
        return SR_FAILURE;
    }

    if (nmhSetFsSrTePathRowStatus (pSnmpDestAddr, u4DestMask,
                                   pSnmpMandatoryRtrId, pDestAddr->u2AddrType,
                                   MPLS_STATUS_DESTROY) == SNMP_FAILURE)
    {
        CliPrintf (CliHandle,
                   "%%Unable to delete Entry (Set-DESTROY) in TE-Path Table \r\n");
        return SR_FAILURE;
    }

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrTeSetAllTePathInfoRowStatus
 * Description   : This routine sets TE Path Info row status
 * Input(s)      : i4RowStatus - Row status of the table
 *                 u2AddrType - Address type (IPv4 or IPv6)
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrTeSetAllTePathInfoRowStatus (INT4 i4RowStatus, UINT2 u2AddrType)
{
    UINT4               u4ErrCode = SR_ZERO;
    tGenU4Addr          cmpDestAddr;
    tGenU4Addr          dummyAddr;
    tGenU4Addr          destAddr;
    UINT4               u4DestMask = SR_ZERO;
    tGenU4Addr          mandRtrId;
    UINT4               u4PrevDestMask = SR_ZERO;

    UINT1               au1DestAddrV6[SR_IPV6_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1PrevDestAddrV6[SR_IPV6_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1MandatoryRtrIdV6[SR_IPV6_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1PrevMandatoryRtrIdV6[SR_IPV6_ADDR_LENGTH] =
        { SR_ZERO };
    INT4                i4NextFsSrTeDestAddrType = SR_ZERO;
    INT4                i4FsSrTeDestAddrType = SR_ZERO;

    tSNMP_OCTET_STRING_TYPE snmpDestAddr;
    tSNMP_OCTET_STRING_TYPE snmpPrevDestAddr;
    tSNMP_OCTET_STRING_TYPE snmpMandatoryRtrId;
    tSNMP_OCTET_STRING_TYPE snmpPrevMandatoryRtrId;
    tSNMP_OCTET_STRING_TYPE *pSnmpDestAddr = NULL;
    tSNMP_OCTET_STRING_TYPE *pSnmpPrevDestAddr = NULL;
    tSNMP_OCTET_STRING_TYPE *pSnmpMandatoryRtrId = NULL;
    tSNMP_OCTET_STRING_TYPE *pSnmpPrevMandatoryRtrId = NULL;

    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&cmpDestAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&destAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&mandRtrId, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&snmpDestAddr, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    MEMSET (&snmpPrevDestAddr, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    MEMSET (&snmpMandatoryRtrId, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    MEMSET (&snmpPrevMandatoryRtrId, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));

    pSnmpDestAddr = &snmpDestAddr;
    pSnmpPrevDestAddr = &snmpPrevDestAddr;
    pSnmpMandatoryRtrId = &snmpMandatoryRtrId;
    pSnmpPrevMandatoryRtrId = &snmpPrevMandatoryRtrId;

    snmpDestAddr.pu1_OctetList = au1DestAddrV6;
    snmpPrevDestAddr.pu1_OctetList = au1PrevDestAddrV6;
    snmpMandatoryRtrId.pu1_OctetList = au1MandatoryRtrIdV6;
    snmpPrevMandatoryRtrId.pu1_OctetList = au1PrevMandatoryRtrIdV6;

    while (SR_ONE)
    {
        /*Only Execute in 1st iteration */
        if ((MEMCMP (&cmpDestAddr, &dummyAddr, sizeof (tGenU4Addr)) == SR_ZERO)
            &&
            (nmhGetFirstIndexFsSrTePathTable
             (pSnmpDestAddr, &u4DestMask, pSnmpMandatoryRtrId,
              &i4FsSrTeDestAddrType) == SNMP_FAILURE))
        {
            break;
        }

        if ((MEMCMP (&cmpDestAddr, &dummyAddr, sizeof (tGenU4Addr)) != SR_ZERO)
            &&
            ((nmhGetNextIndexFsSrTePathTable
              (pSnmpPrevDestAddr, pSnmpDestAddr, u4PrevDestMask, &u4DestMask,
               pSnmpPrevMandatoryRtrId, pSnmpMandatoryRtrId,
               i4FsSrTeDestAddrType,
               &i4NextFsSrTeDestAddrType) == SNMP_FAILURE)))
        {
            break;
        }

        if (u2AddrType != (UINT2) i4FsSrTeDestAddrType)
        {
            if (i4FsSrTeDestAddrType == SR_IPV4_ADDR_TYPE)
            {
                cmpDestAddr.Addr.u4Addr = SR_ONE;
            }
            else if (i4FsSrTeDestAddrType == SR_IPV6_ADDR_TYPE)
            {
                MEMSET (&cmpDestAddr.Addr.Ip6Addr.u1_addr, SR_ONE,
                        SR_IPV6_ADDR_LENGTH);
            }
            continue;
        }

        if (i4NextFsSrTeDestAddrType != SR_ZERO)
        {
            i4FsSrTeDestAddrType = i4NextFsSrTeDestAddrType;
            i4NextFsSrTeDestAddrType = SR_ZERO;
        }

        if (i4FsSrTeDestAddrType == SR_IPV4_ADDR_TYPE)
        {
            cmpDestAddr.Addr.u4Addr = SR_ONE;

            SR_OCTETSTRING_TO_INTEGER (pSnmpDestAddr, destAddr.Addr.u4Addr);
            SR_OCTETSTRING_TO_INTEGER (pSnmpMandatoryRtrId,
                                       mandRtrId.Addr.u4Addr);

            destAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
            mandRtrId.u2AddrType = SR_IPV4_ADDR_TYPE;
        }
        else if (i4FsSrTeDestAddrType == SR_IPV6_ADDR_TYPE)
        {
            MEMSET (&cmpDestAddr.Addr.Ip6Addr.u1_addr, SR_ONE,
                    SR_IPV6_ADDR_LENGTH);

            MEMCPY (destAddr.Addr.Ip6Addr.u1_addr,
                    pSnmpDestAddr->pu1_OctetList, pSnmpDestAddr->i4_Length);

            MEMCPY (mandRtrId.Addr.Ip6Addr.u1_addr,
                    pSnmpMandatoryRtrId->pu1_OctetList,
                    pSnmpMandatoryRtrId->i4_Length);

            destAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
            mandRtrId.u2AddrType = SR_IPV6_ADDR_TYPE;
        }
        if (nmhTestv2FsSrTePathRowStatus (&u4ErrCode, pSnmpDestAddr, u4DestMask,
                                          pSnmpMandatoryRtrId,
                                          i4FsSrTeDestAddrType,
                                          i4RowStatus) == SNMP_FAILURE)
        {
            SR_TRC3 (SR_MGMT_TRC | SR_FAIL_TRC,
                     "%s:%d FAILURE:(Test-%d) in TE-Path Table\n", __func__,
                     __LINE__, i4RowStatus);
            return SR_FAILURE;
        }

        if (nmhSetFsSrTePathRowStatus (pSnmpDestAddr, u4DestMask,
                                       pSnmpMandatoryRtrId,
                                       i4FsSrTeDestAddrType,
                                       i4RowStatus) == SNMP_FAILURE)
        {
            SR_TRC3 (SR_MGMT_TRC | SR_FAIL_TRC,
                     "%s:%d FAILURE: (Set-%d) in TE-Path Table\n", __func__,
                     __LINE__, i4RowStatus);
            return SR_FAILURE;
        }

        /* Update for Next Iteration */
        u4PrevDestMask = u4DestMask;

        if (i4FsSrTeDestAddrType == SR_IPV4_ADDR_TYPE)
        {
            SR_INTEGER_TO_OCTETSTRING (destAddr.Addr.u4Addr, pSnmpPrevDestAddr);
            SR_INTEGER_TO_OCTETSTRING (mandRtrId.Addr.u4Addr,
                                       pSnmpPrevMandatoryRtrId);
        }
        else if (i4FsSrTeDestAddrType == SR_IPV6_ADDR_TYPE)
        {
            MEMCPY (pSnmpDestAddr->pu1_OctetList, destAddr.Addr.Ip6Addr.u1_addr,
                    SR_IPV6_ADDR_LENGTH);

            MEMCPY (pSnmpMandatoryRtrId->pu1_OctetList,
                    mandRtrId.Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);

            pSnmpDestAddr->i4_Length = SR_IPV6_ADDR_LENGTH;
            pSnmpMandatoryRtrId->i4_Length = SR_IPV6_ADDR_LENGTH;
        }
    }
    return SR_SUCCESS;
}

/*****************************************************************************
* Function Name : SrV3SetOspfSrStatus
* Description   : This routine Sets the Segment Routing Status for Ospfv3
* Input(s)      : u4OspfContextId - Ospf Context Id
*                 u4srStatus - Enable/Disable
* Output(s)     : None
* Return(s)     : SR_SUCCESS / SR_FAILURE
*****************************************************************************/
UINT4
SrV3SetOspfSrStatus (UINT4 u4OspfContextId, UINT4 u4srStatus)
{
    INT4                i4SrStatus = SR_DISABLED;
    INT4                i4SrMode = SR_ZERO;
    INT4                i4RetStatus = SR_FAILURE;
    INT4                i4RetValue = SR_FAILURE;
    UINT4               u4SrEnabled = SR_DISABLED;
    UINT4               u4ErrCode = SR_ZERO;

    SR_TRC1 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s ENTRY \n", __func__);
    nmhGetFsSrV6Status (&i4SrStatus);

    /* return failure if SR is not enabled for IPv6 */
    if (i4SrStatus == SR_DISABLED)
    {
        return SR_FAILURE;
    }

    /* Check if OSPFv3 is already registered. If Yes, return */
    if ((SR_MODE & SR_CONF_OSPFV3) == SR_CONF_OSPFV3)
    {
        u4SrEnabled = SR_ONE;
    }
    else
    {
        /* SR_TWO is equivalent to OSPF_FALSE */
        u4SrEnabled = SR_TWO;
    }

    if (u4SrEnabled == u4srStatus)
    {
        return SR_SUCCESS;
    }

    if (nmhTestv2FsSrMode (&u4ErrCode, SR_CONF_OSPFV3) == SNMP_FAILURE)
    {
        return SR_FAILURE;
    }
    nmhGetFsSrMode (&i4SrMode);
    if (u4OspfContextId == OSPFV3_DEFAULT_CXT_ID)
    {
        if (u4srStatus == OSPFV3_SR_ENABLED)
        {
            SR_UNLOCK ();
            if (SrSndMsgToOspfV3 (SR_OSPFV3_REG,
                                  u4OspfContextId, NULL) != SR_SUCCESS)
            {
                SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "Registeration failed\n");
                return SR_FAILURE;
            }
            SR_LOCK ();
            i4SrMode |= SR_CONF_OSPFV3;
            nmhSetFsSrMode (i4SrMode);
            SrOspfv3UpEventHandler ();
        }
        else
        {
            /* Set gSrGlobalInfo.u1Ospfv3DeRegInProgress to 
             * indicate that OSPFv3 is going to be de-registered */
            gSrGlobalInfo.u1Ospfv3DeRegInProgress = SR_TRUE;
            if (SR_SUCCESS == SrCliProcessAdminDownCmdForV6 (SR_SHUT_CMD))
            {
                SR_TRC (SR_CTRL_TRC, "Mark Admin State Down\n");
            }
            i4RetStatus = SrCliProcessAdminDownCmdForV6 (SR_DESTROY_CMD);
            if (i4RetStatus != SR_SUCCESS)
            {
                return SR_FAILURE;
            }

            if (SrSndMsgToOspfV3 (SR_OSPFV3_DEREG,
                                  u4OspfContextId, NULL) != SR_SUCCESS)
            {
                SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "De-Registeration failed\n");
                return SR_FAILURE;
            }
            /* Reset SR Mode */
            i4SrMode &= ~SR_CONF_OSPFV3;
            nmhSetFsSrMode (i4SrMode);
            if (gu4AddrV6Family == SR_ONE)
            {
                i4RetValue = SrCliConfigAddrFamily (SR_CLI_IPV6, SR_DISABLE);
                if (i4RetValue == SR_SUCCESS)
                {
                    gu4AddrV6Family = SR_ZERO;
                    SR_TRC (SR_CTRL_TRC,
                            "Disable of IPv6 Address family is success \n\r");
                }
                else
                {
                    SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                            "Failure in Ipv6 Address family disable \n\r");
                }
            }
            /* Check whether disable of MPLS is configured or not */
            if ((i4SrMode == SR_ZERO) && (gu4SegmentMpls == SR_ONE))
            {
                i4RetValue = SrCliDisableSrMpls ();
                if (i4RetValue == SR_SUCCESS)
                {
                    gu4SegmentMpls = SR_ZERO;
                    SR_TRC (SR_CTRL_TRC,
                            "MPLS segment routing disable is success \n\r");
                }
                else
                {
                    SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                            "Failure in MPLS segment routing disable \n\r");
                }
            }
            gSrGlobalInfo.u1Ospfv3DeRegInProgress = SR_FALSE;
        }
    }
    SR_TRC1 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s EXIT \n", __func__);

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrCliConfigV4Alternate
 * Description   : This routine sets the Alternate IPv4 status.
 * Input(s)      : CliHandle - Index of current CLI context
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliConfigV4Alternate (tCliHandle CliHandle, UINT4 u4AltStatus)
{
    UINT4   u4ErrorCode = 0;

    if (nmhTestv2FsSrV4AlternateStatus (&u4ErrorCode, (INT4)u4AltStatus) ==
        SNMP_FAILURE)
    {
        CliPrintf (CliHandle, "\r Alternate cannot be configured if SR is down\r\n");
        return SR_FAILURE;
    }

    if (nmhSetFsSrV4AlternateStatus ((INT4)u4AltStatus) == SNMP_FAILURE)
    {
        return SR_FAILURE;
    }

    return SR_SUCCESS;
 }

/*****************************************************************************
 * Function Name : SrCliConfigV6Alternate
 * Description   : This routine sets the Alternate IPv6 status.
 * Input(s)      : CliHandle - Index of current CLI context
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliConfigV6Alternate (UINT4 u4AltStatus)
{
    INT4                i4RetStatus = SR_FAILURE;
    i4RetStatus = SrV3CliAltConfig ((INT4) u4AltStatus);
    if (i4RetStatus != SNMP_SUCCESS)
    {
        return SR_FAILURE;
    }
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrV3LfaCliConfig
 * Description   : This routine sets the LFA IPv4 status.
 * Input(s)      : CliHandle - Index of current CLI context
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrV3CliAltConfig (INT4 i4SetAltStatus)
{
    UINT4               u4ErrorCode = 0;

    if (nmhTestv2FsSrV6AlternateStatus (&u4ErrorCode, i4SetAltStatus) ==
        SNMP_FAILURE)
    {
        SR_TRC (SR_CRITICAL_TRC | SR_MGMT_TRC,
                "% Alternate cannot be configured if SR is down\r\n");
        return SR_FAILURE;
    }
    if (nmhSetFsSrV6AlternateStatus (i4SetAltStatus) == SNMP_FAILURE)
    {
        return SR_FAILURE;
    }
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrCliShowConfig
 * Description   : This routine displays the configurations for Segment Routing
 *                 such as module status, SRGB configuration etc.
 * Input(s)      : CliHandle - Index of current CLI context
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliShowConfig (tCliHandle CliHandle)
{
    INT4                i4SrStatus = SR_ZERO;
    INT4                i4Sr6Status = SR_ZERO;
    INT4                i4SrMode = SR_ZERO;
    INT4                i4SrAltStatus = ALTERNATE_DISABLED;
    UINT4               u4Count = SR_ZERO;

    nmhGetFsSrV4Status (&i4SrStatus);
    nmhGetFsSrV6Status (&i4Sr6Status);

    if ((i4SrStatus == SR_ENABLED) &&
        (i4Sr6Status == SR_ENABLED))
    {
        CliPrintf (CliHandle,
                "\r\nSR Status                : Enabled (IPv4, IPv6) \r\n");
    }
    else if ((i4SrStatus == SR_ENABLED) && (i4Sr6Status == SR_DISABLED))
    {
        CliPrintf (CliHandle,
                "\r\nSR Status                : Enabled (IPv4) \r\n");
    }
    else if ((i4SrStatus == SR_DISABLED) && (i4Sr6Status == SR_ENABLED))
    {
        CliPrintf (CliHandle,
                "\r\nSR Status                : Enabled (IPv6) \r\n");
    }
    else if ((i4SrStatus == SR_DISABLED) && (i4Sr6Status == SR_DISABLED))
    {
        CliPrintf (CliHandle,
                "\r\nSR Status                : Disabled \r\n");
    }

    nmhGetFsSrMode (&i4SrMode);
    if ((i4SrMode & SR_CONF_OSPF) == SR_CONF_OSPF)
    {
        CliPrintf (CliHandle, "Mode                     : OSPF\n");
    }
    else if ((i4SrMode & SR_CONF_OSPFV3) == SR_CONF_OSPFV3)
    {
        CliPrintf (CliHandle, "Mode                     : OSPFv3\n");
    }
    else if ((i4SrMode & SR_CONF_STATIC) == SR_CONF_STATIC)
    {
        CliPrintf (CliHandle, "Mode                     : Static SR over IPv4\n");
    }
    else if ((i4SrMode & SR_CONF_STATIC_V6) == SR_CONF_STATIC_V6)
    {
        CliPrintf (CliHandle, "Mode                     : Static SR over IPv6\n");
    }

    if ((IS_V4_SRGB_CONFIGURED == SR_TRUE) ||
        ((gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex ==
          SRGB_DEFAULT_MIN_VALUE)
         && gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMaxIndex ==
         SRGB_DEFAULT_MAX_VALUE))
    {
        CliPrintf (CliHandle, "IPv4 SRGB Range          :\n");
        CliPrintf (CliHandle, "   SRGB Start            : %u\n",
                   gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex);
        CliPrintf (CliHandle, "   SRGB End              : %u\n",
                   gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMaxIndex);

        if (gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMaxIndex-
            gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex > 0)
        {
            CliPrintf (CliHandle, "   Range                 : %u\n",
                    gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMaxIndex-
                    gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex);
        }
        else
        {
            CliPrintf (CliHandle, "   Range                 : Invalid Value. Range cannot be negative \n");
        }
    }
    else
    {
        CliPrintf (CliHandle, "IPv4 SRGB Range          : Not Configured\n");
    }

#if 0 /* Currently not supported. To be enabled later */ 
    if ((IS_V6_SRGB_CONFIGURED == SR_TRUE) ||
        ((gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex ==
          SRGB_DEFAULT_MIN_VALUE)
         && gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MaxIndex ==
         SRGB_DEFAULT_MAX_VALUE))
    {
        CliPrintf (CliHandle, "IPv6 SRGB Range                     :\n");
        CliPrintf (CliHandle, "   SRGB Start            : %u\n",
                   gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex);
        CliPrintf (CliHandle, "   SRGB End              : %u\n",
                   gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MaxIndex);

        if (gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbMaxIndex-
            gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbMinIndex > 0)
        {
            CliPrintf (CliHandle, "   Range                 : %u\n",
                    gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbMaxIndex-
                    gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbMinIndex);
        }
        else
        {
            CliPrintf (CliHandle, "   Range                 : Invalid Value. Range cannot be negative \n");
        }
    }
    else
    {
        CliPrintf (CliHandle, "IPv6 SRGB Range          : Not Configured\n");
    }
#endif

    nmhGetFsSrV4AlternateStatus (&i4SrAltStatus);
    if (i4SrAltStatus == ALTERNATE_ENABLED)
    {
        CliPrintf (CliHandle, "IPv4 Alternate Status    : Enabled\n");
    }
    else
    {
        CliPrintf (CliHandle, "IPv4 Alternate Status    : Disabled\n");
    }

    i4SrAltStatus = ALTERNATE_DISABLED;

#if 0 /* Currently not supported. To be enabled later */ 
    nmhGetFsSrV6AlternateStatus (&i4SrAltStatus);
    if (i4SrAltStatus == ALTERNATE_ENABLED)
    {
        CliPrintf (CliHandle, "IPv6 Alternate Status    : Enabled\n");
    }
    else
    {
        CliPrintf (CliHandle, "IPv6 Alternate Status    : Disabled\n");
    }
#endif

    RBTreeCount (gSrGlobalInfo.pSrSidRbTree, &u4Count);
    CliPrintf (CliHandle,"Self SID Count           : %d \n", u4Count);

    SrShowLblCountFromLblGroup (CliHandle, gSrGlobalInfo.SrContext.u2SrLblGroupId);
 
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SriCliShowSidInterfaceList
 * Description   : This routine displays the SID databse information in a
                                   sorted order of IP addresses.
 * Input(s)      : CliHandle - Index of current CLI context
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliShowSidInterfaceList (tCliHandle CliHandle)
{
    CliPrintf (CliHandle, "\r\n%-8s%-12s%-12s%-18s%-18s\r\n", "SID",
               "Index Type", "Type", "IpAddr", "IpMask");

    CliPrintf (CliHandle, "%-8s%-12s%-12s%-18s%-18s\r\n", "---", "----------",
               "----", "------", "-----");

    RBTreeWalk (gSrGlobalInfo.pSrSidRbTree, SrCliSidIntfWalkFn, &CliHandle,
                SR_ZERO);

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrCliShowRouteInfo
 * Description   : This routine displays the SR route information with
                   nexthop details.
 * Input(s)      : CliHandle - Index of current CLI context
                   prefix - IPv4 address
                   mask   - prefix length of the route
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliShowRouteInfo (tCliHandle CliHandle, UINT4 u4Prefix, UINT4 u4Mask)
{
   tSrRtrInfo         *pRtrInfo = NULL;
   tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
   tSrRlfaPathInfo    *pSrRlfaPathInfo = NULL;
   tOutSegment        *pOutSegment = NULL;
   tTMO_SLL_NODE      *pRtrNode = NULL;
   tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
   UINT4               u4destMask = 32;
   UINT1               au1IfName[CFA_MAX_PORT_NAME_LENGTH];
   INT1               *piIfName;
   UINT4               u4L3Intf = SR_ZERO;
   UINT4               u4LfaOutLabel = SR_ZERO;
   UINT1               u1PrefixFound = FALSE;
   tSrTilfaPathInfo    SrTilfaPathInfo;
   tSrTilfaPathInfo   *pSrTilfaPathInfo = NULL;
   tSrTilfaAdjInfo    *pTiLfaAdjInfo = NULL;
   CHR1               *pIpAddr = NULL;
   CHR1               *pNextHopIp = NULL;
   CHR1               *pAltNextHop = NULL;
   CHR1               *pAreaId = NULL;
   UINT1               u1IsPrimaryDisplayed = FALSE;

   piIfName = (INT1 *) (&au1IfName[0]);

   MEMSET (au1IfName, SR_ZERO, CFA_MAX_PORT_NAME_LENGTH);
   MEMSET (&SrTilfaPathInfo, SR_ZERO, sizeof(tSrTilfaPathInfo));

   TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
   {
	   pRtrInfo = (tSrRtrInfo *) pRtrNode;

	   if (pRtrInfo == NULL)
		   return SR_FAILURE;

	   if ((u4Prefix != SR_ZERO) && (u4Mask != SR_ZERO))
	   {
		   if (pRtrInfo->prefixId.Addr.u4Addr != u4Prefix)
		   {
			   continue;
		   }
		   else
		   {
			   u1PrefixFound = TRUE;
		   }
	   }

	   if (pRtrInfo->prefixId.u2AddrType == SR_IPV4_ADDR_TYPE)
	   {
		   CLI_CONVERT_IPADDR_TO_STR (pIpAddr,pRtrInfo->prefixId.Addr.u4Addr);
	   }

	   CliPrintf (CliHandle, "\r\nRoute entry for %s/%d  ",pIpAddr,u4destMask);

	   CLI_CONVERT_IPADDR_TO_STR (pAreaId, pRtrInfo->u4areaId);
       CliPrintf (CliHandle, "Area-ID: %s\r\n",pAreaId);

	   pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) TMO_SLL_First (&(pRtrInfo->NextHopList));

	   while (pSrRtrNextHopInfo != NULL)
	   {
		   CLI_CONVERT_IPADDR_TO_STR (pNextHopIp,pSrRtrNextHopInfo->nextHop.Addr.u4Addr);

		   if ((pSrRtrNextHopInfo->u1FRRNextHop == SR_PRIMARY_NEXTHOP) &&
				   (u1IsPrimaryDisplayed == FALSE))
		   {
			   if(CfaUtilGetIfIndexFromMplsTnlIf
					   (pSrRtrNextHopInfo->u4OutIfIndex,
						&u4L3Intf, TRUE) == CFA_SUCCESS)
			   {
				   if(CfaCliGetIfName (u4L3Intf,piIfName) == CFA_FAILURE)
				   {
					   /* Dont return failure here */
				   }
			   }

			   CliPrintf (CliHandle,"\r\t Primary NextHop: %s, via %s\r\n",
					   pNextHopIp,(INT1 *) au1IfName);
			   CliPrintf (CliHandle,"\r\t OutLabel : %d\r\n",pRtrInfo->u4OutLabel);
			   CliPrintf (CliHandle,"\r\t InLabel : %d\r\n",pRtrInfo->u4LfaInLabel);
               CliPrintf (CliHandle,"\r\t Tunnel OutIfIdx : %d\r\n",
                                             pSrRtrNextHopInfo->u4OutIfIndex);
               CliPrintf (CliHandle,"\r\t Tunnel SwapOutIfIdx : %d\r\n",
                                             pSrRtrNextHopInfo->u4SwapOutIfIndex);               

			   if(pRtrInfo->u1LfaType == SR_LFA_ROUTE)
			   {
                   CLI_CONVERT_IPADDR_TO_STR (pAltNextHop,pRtrInfo->u4LfaNextHop);

				   MEMSET (au1IfName, SR_ZERO, CFA_MAX_PORT_NAME_LENGTH);

				   if(CfaUtilGetIfIndexFromMplsTnlIf
						   (pRtrInfo->u4LfaOutIfIndex,
							&u4L3Intf, TRUE) == CFA_SUCCESS)
				   {
					   if(CfaCliGetIfName (u4L3Intf,piIfName) == CFA_FAILURE)
					   {
						   /* Dont return failure here */
					   }
				   }

				   pOutSegment = MplsGetLspOutSegmentFromIfIndex (pRtrInfo->u4LfaOutIfIndex);
				   if ((pOutSegment != NULL) &&
						   (pOutSegment->u1Owner == MPLS_OWNER_SR))
				   {
					   u4LfaOutLabel = pOutSegment->u4Label;
				   }
				   CliPrintf (CliHandle,"\r\n\t Alternate NextHop: %s, via %s\r\n",
						   pAltNextHop,(INT1 *) au1IfName);
				   CliPrintf (CliHandle,"\r\t LFA Type :  LFA\r\n");
				   CliPrintf (CliHandle,"\r\t OutLabel(s) : %d\n",u4LfaOutLabel);
                   CliPrintf (CliHandle,"\r\t Tunnel OutIfIdx : %d\r\n",
                           pRtrInfo->u4LfaOutIfIndex);
                   CliPrintf (CliHandle,"\r\t Tunnel SwapOutIfIdx : %d\r\n",
                           pRtrInfo->u4LfaSwapOutIfIndex);
			   }

			   u1IsPrimaryDisplayed = TRUE;
			   if (pRtrInfo->u1LfaType == SR_RLFA_ROUTE ||
					   pRtrInfo->u1LfaType == SR_TILFA_ROUTE)
			   {
				   pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) 
					   TMO_SLL_First(&(pRtrInfo->NextHopList));
				   continue;
			   }
		   }

		   if(u1IsPrimaryDisplayed == TRUE)
		   {
			   if(pSrRtrNextHopInfo->u1FRRNextHop == SR_RLFA_NEXTHOP)
			   {              
				   pSrRlfaPathInfo = SrGetRlfaPathEntryFromDestAddr (&(pRtrInfo->prefixId));

				   if ((pSrRlfaPathInfo != NULL) &&
						   (pSrRlfaPathInfo->rlfaNextHop.Addr.u4Addr ==
							pSrRtrNextHopInfo->nextHop.Addr.u4Addr))
				   {
					   CLI_CONVERT_IPADDR_TO_STR (pAltNextHop,
							   pSrRlfaPathInfo->rlfaNextHop.Addr.u4Addr);
					   MEMSET (au1IfName, SR_ZERO, CFA_MAX_PORT_NAME_LENGTH);

					   if(CfaCliGetIfName (pSrRlfaPathInfo->rlfaOutIfIndex
								   ,piIfName) == CFA_FAILURE)
					   {
						   /* Dont return failure here */
					   }

					   CliPrintf (CliHandle,"\r\n\t Alternate NextHop: %s, via %s\r\n",
							   pAltNextHop,(INT1 *) au1IfName);
					   CliPrintf (CliHandle,"\r\t LFA Type :  RLFA\r\n");
					   CliPrintf (CliHandle,"\r\t Remote Label : %d \r\n",
							   pSrRlfaPathInfo->u4RemoteLabel);
					   CliPrintf (CliHandle,"\r\t Destination Label : %d \r\n",
							   pSrRlfaPathInfo->u4DestLabel);
                       CliPrintf (CliHandle,"\r\t Tunnel OutIfIdx : %d\r\n",
                               pSrRlfaPathInfo->u4RlfaTunIndex);
                       CliPrintf (CliHandle,"\r\t Tunnel SwapOutIfIdx : %d\r\n",
                               pSrRlfaPathInfo->u4RlfaIlmTunIndex);
				   }
			   }
			   else if (pSrRtrNextHopInfo->u1FRRNextHop == SR_TILFA_NEXTHOP)
			   {
				   SrTilfaPathInfo.destAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
				   SrTilfaPathInfo.destAddr.Addr.u4Addr = pRtrInfo->prefixId.Addr.u4Addr;

				   pSrTilfaPathInfo = RBTreeGet (gSrGlobalInfo.pSrTilfaPathRbTree,
						   &SrTilfaPathInfo);
				   if ((pSrTilfaPathInfo != NULL) &&
						   (pSrTilfaPathInfo->lfaNextHopAddr.Addr.u4Addr ==
							pSrRtrNextHopInfo->nextHop.Addr.u4Addr))
				   {

					   CLI_CONVERT_IPADDR_TO_STR (pAltNextHop,
							   pSrTilfaPathInfo->lfaNextHopAddr.Addr.u4Addr);
					   MEMSET (au1IfName, SR_ZERO, CFA_MAX_PORT_NAME_LENGTH);

					   if(CfaCliGetIfName (pSrTilfaPathInfo->u4OutIfIndex,
								   piIfName) == CFA_FAILURE)
					   {
						   /* Dont return failure here */
					   }

					   CliPrintf (CliHandle,"\r\n\t Alternate NextHop: %s, via %s\r\n",
							   pAltNextHop,(INT1 *) au1IfName);
					   CliPrintf (CliHandle,"\r\t LFA Type :  TI-LFA\r\n");
					   CliPrintf (CliHandle,"\r\t Remote Label : %d \r\n",
							   pSrTilfaPathInfo->u4RemoteLabel);
					   CliPrintf (CliHandle,"\r\t OutLabel[s] {adj-sid} :");
					   TMO_SLL_Scan (&(pSrTilfaPathInfo->pqAdjSidList),
							   pTiLfaAdjInfo, tSrTilfaAdjInfo *)
					   {
						   if(pTiLfaAdjInfo!=NULL)
						   {
							   CliPrintf (CliHandle,"%8d",pTiLfaAdjInfo->u4AdjSid);
						   }
					   }
					   CliPrintf (CliHandle,"\r\n\t Destination Label : %d \r\n",
							   pSrTilfaPathInfo->u4DestLabel);
                       CliPrintf (CliHandle, "\r\t Tunnel OutIfIdx : %d\r\n",
                               pSrTilfaPathInfo->u4FtnTunIndex);
                       CliPrintf (CliHandle,"\r\t Tunnel SwapOut IfIdx :%d\r\n",
                               pSrTilfaPathInfo->u4IlmTunIndex);
				   }
			   }
		   }
		   pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) TMO_SLL_Next 
			   (&(pRtrInfo->NextHopList),
				&(pSrRtrNextHopInfo->nextNextHop));
	   }

	   u1IsPrimaryDisplayed = FALSE;
	   if (u1PrefixFound == TRUE)
	   {
		   /* Route entry for given prefix is found. */
		   break;
	   }
   }

   return SR_SUCCESS;
}
/*****************************************************************************
 * Function Name : SrCliSidIntfWalkFn
 * Description   : This routine is called by RBTreeWalk while traversal of the
 *                                 RB Tree.
 * Input         : pRBEle        - RBTree Node
 *                 visit         - Order of visiting the tree
 *                 u4Level       - Level of the tree
 *                 pArg          - Input arg to the function
 *                 pOut          - Output from the function
 *
 * Output(s)     : None
 * Return(s)     : RB_WALK_CONT
 *****************************************************************************/
INT4
SrCliSidIntfWalkFn (tRBElem * node, eRBVisit visit, UINT4 u4Level,
                    VOID *pArg, VOID *pOut)
{
    tSrSidInterfaceInfo *pSrSidEntry = NULL;
    UINT1               sidType[MAX_SR_TEXT_SIZE] = { SR_ZERO };
    UINT1               ipAddr[MAX_SR_TEXT_SIZE] = { SR_ZERO };
    UINT1               ipAddrMask[MAX_SR_TEXT_SIZE] = { SR_ZERO };
    UINT1               sidIndexType[MAX_SR_TEXT_SIZE] = { SR_ZERO };
    UINT4               labelIndex = SR_ZERO;
    tCliHandle          CliHandle = SR_ZERO;

    UINT1              *pTemp = NULL;
    UINT4               u4TempAddr = SR_ZERO;

    UNUSED_PARAM (u4Level);
    UNUSED_PARAM (pOut);

    CliHandle = *(tCliHandle *) pArg;

    if ((visit == postorder) || (visit == leaf))
    {
        if (node != NULL)
        {
            pSrSidEntry = (tSrSidInterfaceInfo *) node;

            if (pSrSidEntry->ipAddrType == SR_IPV4_ADDR_TYPE)
            {
                if (pSrSidEntry->u4SidType == SR_SID_NODE)
                {
                    MEMCPY (sidType, NODE_SID, strlen (NODE_SID));
                }
                else
                {
                    MEMCPY (sidType, ADJ_SID, strlen (ADJ_SID));
                }
            }
            else if (pSrSidEntry->ipAddrType == SR_IPV6_ADDR_TYPE)
            {
                if (pSrSidEntry->u4SidTypeV3 == SR_SID_NODE)
                {
                    MEMCPY (sidType, NODE_SID, strlen (NODE_SID));
                }
                else
                {
                    MEMCPY (sidType, ADJ_SID, strlen (ADJ_SID));
                }
            }

            if (((pSrSidEntry->u1SidIndexType == SR_SID_REL_INDEX)
                 && (pSrSidEntry->u4SidType == SR_SID_NODE))
                || ((pSrSidEntry->u1SidIndexType == SR_SID_REL_INDEX)
                    && (pSrSidEntry->u4SidTypeV3 == SR_SID_NODE)))
            {
                MEMCPY (sidIndexType, "Relative", sizeof ("Relative"));
                labelIndex = pSrSidEntry->u4PrefixSidLabelIndex;
            }
            else
            {
                MEMCPY (sidIndexType, "Absolute", sizeof ("Absolute"));
                labelIndex = pSrSidEntry->u4PrefixSidLabelIndex;
            }
            if (pSrSidEntry->ipAddrType == SR_IPV4_ADDR_TYPE)
            {
                u4TempAddr = pSrSidEntry->ifIpAddr.Addr.u4Addr;
                u4TempAddr = OSIX_HTONL (u4TempAddr);
                pTemp = (UINT1 *) &u4TempAddr;
                SPRINTF ((char *__restrict__) ipAddr, "%d.%d.%d.%d", pTemp[0],
                         pTemp[1], pTemp[2], pTemp[3]);

                u4TempAddr = pSrSidEntry->ifIpAddrMask;
                u4TempAddr = OSIX_HTONL (u4TempAddr);
                pTemp = (UINT1 *) &u4TempAddr;
                SPRINTF ((char *__restrict__) ipAddrMask, "%d.%d.%d.%d",
                         pTemp[0], pTemp[1], pTemp[2], pTemp[3]);
            }
            else if (pSrSidEntry->ipAddrType == SR_IPV6_ADDR_TYPE)
            {
                SPRINTF ((char *__restrict__) ipAddr, "%s",
                         Ip6PrintAddr (&pSrSidEntry->ifIpAddr.Addr.Ip6Addr));

                SPRINTF ((char *__restrict__) ipAddrMask, "%d", 128);
            }
            CliPrintf (CliHandle, "%-8d%-12s%-12s%-18s%-18s\r\n", labelIndex,
                       sidIndexType, sidType, ipAddr, ipAddrMask);
        }
    }
    return RB_WALK_CONT;
}

/*****************************************************************************
 * Function Name : SrCliShowPeerInfoList
 * Description   : This routine displays the Peer databse information for
 *                 Dynamic Segment Routing.
 * Input(s)      : CliHandle - Index of current CLI context
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliShowPeerInfoList (tCliHandle CliHandle, UINT4 u4Command)
{
    tTMO_SLL_NODE      *pRtrNode;
    tTMO_SLL_NODE      *pAdjSid;
    tSrRtrInfo         *pRtrInfo = NULL;
    UINT1               ipAddr[MAX_SR_TEXT_SIZE] = { SR_ZERO };
    UINT1               advRtrId[MAX_SR_TEXT_SIZE] = { SR_ZERO };
    UINT1               areaId[MAX_SR_TEXT_SIZE] = { SR_ZERO };
    tAdjSidNode        *pAdjSidInfo = NULL;
    UINT4               adjCount = SR_ZERO;
    UINT1              *pTemp = NULL;
    UINT4               u4TempAddr = SR_ZERO;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
#ifdef NPAPI_WANTED
    INT4                i4HwIndex = 0;
#endif

    if (((SR_MODE & SR_CONF_OSPF) == SR_CONF_OSPF) ||
        ((SR_MODE & SR_CONF_OSPFV3) == SR_CONF_OSPFV3))
    {
        TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
        {
            MEMSET (advRtrId, SR_ZERO, (sizeof (UINT1) * MAX_SR_TEXT_SIZE));
            MEMSET (ipAddr, SR_ZERO, (sizeof (UINT1) * MAX_SR_TEXT_SIZE));
            MEMSET (areaId, SR_ZERO, (sizeof (UINT1) * MAX_SR_TEXT_SIZE));

            pRtrInfo = (tSrRtrInfo *) pRtrNode;
            u4TempAddr = pRtrInfo->u4areaId;
            u4TempAddr = OSIX_HTONL (u4TempAddr);
            pTemp = (UINT1 *) &u4TempAddr;
            SPRINTF ((char *__restrict__) areaId, "%d.%d.%d.%d", pTemp[0],
                     pTemp[1], pTemp[2], pTemp[3]);

            if (pRtrInfo->prefixId.u2AddrType == SR_IPV4_ADDR_TYPE)
            {
                u4TempAddr = pRtrInfo->prefixId.Addr.u4Addr;
                u4TempAddr = OSIX_HTONL (u4TempAddr);
                pTemp = (UINT1 *) &u4TempAddr;
                SPRINTF ((char *__restrict__) ipAddr, "%d.%d.%d.%d", pTemp[0],
                         pTemp[1], pTemp[2], pTemp[3]);
                CliPrintf (CliHandle, "\r\n%-16s%-18s\r\n", "Node Prefix",
                           ipAddr);
            }
            else if (pRtrInfo->prefixId.u2AddrType == SR_IPV6_ADDR_TYPE)
            {
                SPRINTF ((char *__restrict__) ipAddr, "%s",
                         Ip6PrintAddr (&pRtrInfo->prefixId.Addr.Ip6Addr));

                CliPrintf (CliHandle, "\r\n%-16s%-18s\r\n", "Node Prefix",
                           ipAddr);
            }
            CliPrintf (CliHandle, "%34s\r\n",
                       "----------------------------------");
            CliPrintf (CliHandle, "%-12s%-18s\r\n", "AreaID    : ", areaId);
            u4TempAddr = pRtrInfo->advRtrId.Addr.u4Addr;
            u4TempAddr = OSIX_HTONL (u4TempAddr);
            pTemp = (UINT1 *) &u4TempAddr;
            SPRINTF ((char *__restrict__) advRtrId, "%d.%d.%d.%d", pTemp[0],
                     pTemp[1], pTemp[2], pTemp[3]);
            CliPrintf (CliHandle, "%-12s%-18s\r\n", "Adv RouterId   : ",
                       advRtrId);
            CliPrintf (CliHandle, "%-12s%-12d\r\n", "SID       : ",
                       pRtrInfo->u4SidValue);
            if (pRtrInfo->prefixId.u2AddrType == SR_IPV4_ADDR_TYPE)
            {
                CliPrintf (CliHandle, "%-12s%-12d\r\n", "Min-SRGB  : ",
                           pRtrInfo->srgbRange.u4SrGbMinIndex);
                CliPrintf (CliHandle, "%-12s%-12d\r\n", "Max-SRGB  : ",
                           pRtrInfo->srgbRange.u4SrGbMaxIndex);
            }
            else if (pRtrInfo->prefixId.u2AddrType == SR_IPV6_ADDR_TYPE)
            {
                CliPrintf (CliHandle, "%-12s%-12d\r\n", "Min-SRGB  : ",
                           pRtrInfo->srgbRange.u4SrGbV3MinIndex);
                CliPrintf (CliHandle, "%-12s%-12d\r\n", "Max-SRGB  : ",
                           pRtrInfo->srgbRange.u4SrGbV3MaxIndex);
            }
            TMO_SLL_Scan (&(pRtrInfo->adjSidList), pAdjSid, tTMO_SLL_NODE *)
            {
                adjCount++;
                if (adjCount == SR_ONE)
                {
                    CliPrintf (CliHandle, "%-16s\r\n", "Adjacencies :");
                }
                pAdjSidInfo = (tAdjSidNode *) pAdjSid;
                pTemp = (UINT1 *) &pAdjSidInfo->linkIpAddr.Addr.u4Addr;
                SPRINTF ((char *__restrict__) ipAddr, "%d.%d.%d.%d", pTemp[0],
                         pTemp[1], pTemp[2], pTemp[3]);

                CliPrintf (CliHandle, "\t%s%-18s", "Link Addr : ", ipAddr);
                CliPrintf (CliHandle, "\t%s%-12d\r\n", "Label : ",
                           pAdjSidInfo->u4Label);
            }
            adjCount = SR_ZERO;
            CliPrintf (CliHandle, "\r\n");

            if (u4Command == SR_SHOW_PEER_TABLE_DETAIL)
            {
                CliPrintf (CliHandle, "Additional Details:\n");
                CliPrintf (CliHandle, "ABR status                   : %d \r\n",
                           gSrGlobalInfo.u1OspfAbrStatus);
                u4TempAddr = pRtrInfo->teSwapNextHop.Addr.u4Addr;
                u4TempAddr = OSIX_HTONL (u4TempAddr);
                pTemp = (UINT1 *) &u4TempAddr;
                SPRINTF ((char *__restrict__) ipAddr, "%d.%d.%d.%d", pTemp[0],
                         pTemp[1], pTemp[2], pTemp[3]);
                CliPrintf (CliHandle, "Swap NextHop                 : %s \r\n",
                           ipAddr);

                u4TempAddr = pRtrInfo->advRtrId.Addr.u4Addr;
                u4TempAddr = OSIX_HTONL (u4TempAddr);
                pTemp = (UINT1 *) &u4TempAddr;
                SPRINTF ((char *__restrict__) ipAddr, "%d.%d.%d.%d", pTemp[0],
                         pTemp[1], pTemp[2], pTemp[3]);
                CliPrintf (CliHandle, "Advertisment Router Id       : %s \r\n",
                           ipAddr);

                u4TempAddr = pRtrInfo->u4areaId;
                u4TempAddr = OSIX_HTONL (u4TempAddr);
                pTemp = (UINT1 *) &u4TempAddr;
                SPRINTF ((char *__restrict__) ipAddr, "%d.%d.%d.%d", pTemp[0],
                         pTemp[1], pTemp[2], pTemp[3]);
                CliPrintf (CliHandle, "AreaId                       : %s \r\n",
                           ipAddr);

                CliPrintf (CliHandle, "Sid Value                    : %d \r\n",
                           pRtrInfo->u4SidValue);
                CliPrintf (CliHandle, "Sid Type                     : %d \r\n",
                           pRtrInfo->u4SidType);
                CliPrintf (CliHandle, "Te Swap-Out Interface Index  : %d \r\n",
                           pRtrInfo->u4TeSwapOutIfIndex);
                CliPrintf (CliHandle, "Te Swap-Out Label            : %d \r\n",
                           pRtrInfo->u4TeSwapOutLabel);
                CliPrintf (CliHandle, "Lfa Out Interface Index      : %d \r\n",
                           pRtrInfo->u4LfaOutIfIndex);
                CliPrintf (CliHandle, "Lfa Swap-Out Interface Index : %d \r\n",
                           pRtrInfo->u4LfaSwapOutIfIndex);
                CliPrintf (CliHandle, "Out Label                    : %d \r\n",
                           pRtrInfo->u4OutLabel);
                CliPrintf (CliHandle, "LFA In-Label                : %d \r\n",
                           pRtrInfo->u4LfaInLabel);
                u4TempAddr = pRtrInfo->u4LfaNextHop;;
                u4TempAddr = OSIX_HTONL (u4TempAddr);
                pTemp = (UINT1 *) &u4TempAddr;
                SPRINTF ((char *__restrict__) ipAddr, "%d.%d.%d.%d", pTemp[0],
                         pTemp[1], pTemp[2], pTemp[3]);
                CliPrintf (CliHandle, "LFA Next-Hop                 : %s \r\n",
                           ipAddr);

                u4TempAddr = pRtrInfo->u4LfaNextHop;
                u4TempAddr = OSIX_HTONL (u4TempAddr);
                pTemp = (UINT1 *) &u4TempAddr;
                SPRINTF ((char *__restrict__) ipAddr, "%d.%d.%d.%d", pTemp[0],
                         pTemp[1], pTemp[2], pTemp[3]);
                CliPrintf (CliHandle, "Alt RLFA Next-Hop            : %s \r\n",
                           ipAddr);

                u4TempAddr = pRtrInfo->u4DestMask;
                u4TempAddr = OSIX_HTONL (u4TempAddr);
                pTemp = (UINT1 *) &u4TempAddr;
                SPRINTF ((char *__restrict__) ipAddr, "%d.%d.%d.%d", pTemp[0],
                         pTemp[1], pTemp[2], pTemp[3]);
                CliPrintf (CliHandle, "Destination Mask             : %s \r\n",
                           ipAddr);

                CliPrintf (CliHandle, "External Lsa-To-Be-Sent flag : %d \r\n",
                           pRtrInfo->u4ExtLsaToBeSent);
                CliPrintf (CliHandle, "Te Swap Status               : %d \r\n",
                           pRtrInfo->u1TeSwapStatus);
                CliPrintf (CliHandle, "Flag value                   : %d \r\n",
                           pRtrInfo->u1Flags);
                CliPrintf (CliHandle, "Algo flag value              : %d \r\n",
                           pRtrInfo->u1Algo);
                CliPrintf (CliHandle, "MT ID                        : %d \r\n",
                           pRtrInfo->u1MTID);
                CliPrintf (CliHandle, "Sid Index Type               : %d \r\n",
                           pRtrInfo->u1SidIndexType);
                CliPrintf (CliHandle, "LSA Sent                     : %d \r\n",
                           pRtrInfo->u1LSASent);
                CliPrintf (CliHandle, "LFA Type                     : %d \r\n",
                           pRtrInfo->u1LfaType);
                CliPrintf (CliHandle, "Lfa Active Flag              : %d \r\n",
                           pRtrInfo->bIsLfaActive);
                CliPrintf (CliHandle, "Te configured Flag           : %d \r\n",
                           pRtrInfo->bIsTeconfigured);
                CliPrintf (CliHandle, "Prefix-conflict Alarm        : %s \r\n",
                           ((pRtrInfo->u1PrefixConflictAlarm == SR_ALARM_RAISE) ? "Yes" : "No" ));
                CliPrintf (CliHandle, "SID-conflict Alarm           : %s \r\n",
                           ((pRtrInfo->u1SIDConflictAlarm == SR_ALARM_RAISE) ? "Yes" : "No" ));
                CliPrintf (CliHandle, "Out-Of-Range SID Alarm       : %s \r\n",
                           ((pRtrInfo->u1OutOfRangeAlarm == SR_ALARM_RAISE) ? "Yes" : "No" ));
                CliPrintf (CliHandle, "Available Next Hop Count     : %d \r\n",
                           TMO_SLL_Count (&(pRtrInfo->NextHopList)));

                TMO_SLL_Scan (&(pRtrInfo->NextHopList),
                              pSrRtrNextHopNode, tTMO_SLL_NODE *)
                {
                    pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;

                    u4TempAddr = pSrRtrNextHopInfo->nextHop.Addr.u4Addr;
                    u4TempAddr = OSIX_HTONL (u4TempAddr);
                    pTemp = (UINT1 *) &u4TempAddr;
                    SPRINTF ((char *__restrict__) ipAddr, "%d.%d.%d.%d",
                             pTemp[0], pTemp[1], pTemp[2], pTemp[3]);
                    CliPrintf (CliHandle, "\nNextHop %s Details\r\n", ipAddr);

                    u4TempAddr = pSrRtrNextHopInfo->PrimarynextHop.Addr.u4Addr;
                    u4TempAddr = OSIX_HTONL (u4TempAddr);
                    pTemp = (UINT1 *) &u4TempAddr;
                    SPRINTF ((char *__restrict__) ipAddr, "%d.%d.%d.%d",
                             pTemp[0], pTemp[1], pTemp[2], pTemp[3]);
                    CliPrintf (CliHandle,
                               "Primary NextHop              : %s \r\n",
                               ipAddr);

                    u4TempAddr = pSrRtrNextHopInfo->RemoteNodeAddr.Addr.u4Addr;
                    u4TempAddr = OSIX_HTONL (u4TempAddr);
                    pTemp = (UINT1 *) &u4TempAddr;
                    SPRINTF ((char *__restrict__) ipAddr, "%d.%d.%d.%d",
                             pTemp[0], pTemp[1], pTemp[2], pTemp[3]);
                    CliPrintf (CliHandle,
                               "Remote Node Address          : %s \r\n",
                               ipAddr);

                    u4TempAddr = pSrRtrNextHopInfo->nbrRtrId;
                    u4TempAddr = OSIX_HTONL (u4TempAddr);
                    pTemp = (UINT1 *) &u4TempAddr;
                    SPRINTF ((char *__restrict__) ipAddr, "%d.%d.%d.%d",
                             pTemp[0], pTemp[1], pTemp[2], pTemp[3]);
                    CliPrintf (CliHandle,
                               "neighbour Route Id           : %s \r\n",
                               ipAddr);

                    CliPrintf (CliHandle,
                               "Swap Out Interface Index     : %d \r\n",
                               pSrRtrNextHopInfo->u4SwapOutIfIndex);
                    CliPrintf (CliHandle,
                               "Out Interface Index          : %d \r\n",
                               pSrRtrNextHopInfo->u4OutIfIndex);
                    CliPrintf (CliHandle,
                               "MPLS Status Flag             : %d \r\n",
                               pSrRtrNextHopInfo->u1MPLSStatus);
                    CliPrintf (CliHandle,
                               "FRR NextHop Type             : %d \r\n",
                               pSrRtrNextHopInfo->u1FRRNextHop);
                    CliPrintf (CliHandle,
                               "FRR Hw Lsp Flag              : %d \r\n",
                               pSrRtrNextHopInfo->bIsFRRHwLsp);
                    CliPrintf (CliHandle,
                               "RLFA NextHop                 : %d \r\n",
                               pSrRtrNextHopInfo->bIsLfaNextHop);
                    CliPrintf (CliHandle,
                               "FRR Modify Lsp Flag          : %d \r\n",
                               pSrRtrNextHopInfo->bIsFRRModifyLsp);
                    CliPrintf (CliHandle,
                               "Only CP Update Flag          : %d \r\n",
                               pSrRtrNextHopInfo->bIsOnlyCPUpdate);
#ifdef NPAPI_WANTED
                    MplsGetL3IntfForIndex ((INT4) pSrRtrNextHopInfo->
                                           u4SwapOutIfIndex, &i4HwIndex);
                    CliPrintf (CliHandle,
                               "L3 Interface for Index  %d            : %d \r\n",
                               pSrRtrNextHopInfo->u4SwapOutIfIndex, i4HwIndex);

                    i4HwIndex = 0;
                    MplsGetEgressIntForIndex ((INT4) pSrRtrNextHopInfo->
                                              u4SwapOutIfIndex, &i4HwIndex);
                    CliPrintf (CliHandle,
                               "Egress ID For Interface for Index %d  : %d \r\n",
                               pSrRtrNextHopInfo->u4SwapOutIfIndex, i4HwIndex);
#endif
                }
            }

        }
    }
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrSetMplsILMRowStatus
 * Description   : This routine is used to make Row Status of all STATIC ILMs
 *                 created by SR as NIS/ACTIVE
 *
 * Input         : u4RowStatus - Row status of static ILMs to be made
 *                  if u4RowStatus == NIS
 *                  if u4RowStatus == ACTIVE
 *
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrSetMplsILMRowStatus (UINT4 u4XcIndex, UINT4 u4InIndex,
                       UINT4 u4OutIndex, INT4 i4SetValMplsXCRowStatus)
{
    UINT4               u4ErrCode = SR_ZERO;
    UINT4               u4L3Intf = SR_ZERO;
    INT4                i4IfIndex = SR_ZERO;

    tSNMP_OCTET_STRING_TYPE XCIndex;
    tSNMP_OCTET_STRING_TYPE InSegmentIndex;
    tSNMP_OCTET_STRING_TYPE OutSegmentIndex;

    static UINT1        au1XCIndex[MPLS_INDEX_LENGTH];
    static UINT1        au1InSegIndex[MPLS_INDEX_LENGTH];
    static UINT1        au1OutSegIndex[MPLS_INDEX_LENGTH];

    XCIndex.pu1_OctetList = au1XCIndex;
    XCIndex.i4_Length = MPLS_INDEX_LENGTH;
    InSegmentIndex.pu1_OctetList = au1InSegIndex;
    InSegmentIndex.i4_Length = MPLS_INDEX_LENGTH;
    OutSegmentIndex.pu1_OctetList = au1OutSegIndex;
    OutSegmentIndex.i4_Length = MPLS_INDEX_LENGTH;

    SR_TRC6 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC,
             "%s:%d ENTRY u4XcIndex %d u4InIndex %d u4OutIndex %d RowStatus %d \n",
             __func__, __LINE__, u4XcIndex, u4InIndex, u4OutIndex,
             i4SetValMplsXCRowStatus);
    MPLS_INTEGER_TO_OCTETSTRING (u4XcIndex, (&XCIndex));
    MPLS_INTEGER_TO_OCTETSTRING (u4InIndex, (&InSegmentIndex));
    MPLS_INTEGER_TO_OCTETSTRING (u4OutIndex, (&OutSegmentIndex));

    /*NIS/ACTIVE/DESTROY XcEntry */
    if ((nmhSetMplsXCRowStatus (&XCIndex, &InSegmentIndex,
                                &OutSegmentIndex,
                                i4SetValMplsXCRowStatus)) == SNMP_FAILURE)
    {
        SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d nmhSetMplsXCRowStatus FAILURE \n", __func__,
                 __LINE__);
        return SR_FAILURE;
    }

    /*For DESTROY Pop/Pop & Fwd Entry, InSeg Destroy is also required */
    if (i4SetValMplsXCRowStatus == MPLS_STATUS_DESTROY)
    {
        /* InSeg Clean-Up */
        /* get the interface index b4 deleting the out seg entry */
        if ((nmhGetMplsInSegmentInterface (&InSegmentIndex,
                                           &i4IfIndex)) == SNMP_FAILURE)
        {
            SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d nmhGetMplsInSegmentInterface FAILURE \n",
                     __func__, __LINE__);
            return CLI_FAILURE;
        }

        if ((CfaUtilGetIfIndexFromMplsTnlIf
             ((UINT4) i4IfIndex, &u4L3Intf, TRUE)) == CFA_FAILURE)
        {
            SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s:%d CfaUtilGetIfIndexFromMplsTnlIf FAILURE \n", __func__,
                     __LINE__);
            return CLI_FAILURE;
        }
        if (MplsDeleteMplsIfOrMplsTnlIf (u4L3Intf,
                                         (UINT4) i4IfIndex,
                                         CFA_MPLS_TUNNEL,
                                         MPLS_TRUE) == MPLS_FAILURE)
        {
            SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d MplsDeleteMplsIfOrMplsTnlIf FAILURE \n",
                     __func__, __LINE__);
            return CLI_FAILURE;
        }

        if ((nmhTestv2MplsInSegmentRowStatus (&u4ErrCode,
                                              &InSegmentIndex,
                                              i4SetValMplsXCRowStatus)) ==
            SNMP_FAILURE)
        {
            SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC,
                     "%s:%d nmhTestv2MplsInSegmentRowStatus FAILURE \n", __func__,
                     __LINE__);
            return SR_FAILURE;
        }

        if ((nmhSetMplsInSegmentRowStatus
             (&InSegmentIndex, i4SetValMplsXCRowStatus)) == SNMP_FAILURE)
        {
            SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d nmhSetMplsInSegmentRowStatus FAILURE \n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }

        /* OutSeg Clean-Up */
        if (u4OutIndex != SR_ZERO)
        {
            /*Delete Out-Segment */
            if (nmhGetMplsOutSegmentInterface (&OutSegmentIndex, &i4IfIndex)
                == SNMP_FAILURE)
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC,
                         "%s:%d nmhGetMplsOutSegmentInterface FAILURE \n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }

            if ((CfaUtilGetIfIndexFromMplsTnlIf
                 ((UINT4) i4IfIndex, &u4L3Intf, TRUE)) == CFA_FAILURE)
            {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d CfaUtilGetIfIndexFromMplsTnlIf FAILURE \n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }

            if (MplsDeleteMplsIfOrMplsTnlIf (u4L3Intf, (UINT4) i4IfIndex,
                                             CFA_MPLS_TUNNEL, MPLS_TRUE)
                == MPLS_FAILURE)
            {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d MplsDeleteMplsIfOrMplsTnlIf FAILURE \n", __func__,
                         __LINE__);
                return SR_FAILURE;
            }

            if ((nmhTestv2MplsOutSegmentRowStatus (&u4ErrCode,
                                                   &OutSegmentIndex,
                                                   MPLS_STATUS_DESTROY)) ==
                SNMP_FAILURE)
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC,
                         "%s:%d nmhTestv2MplsOutSegmentRowStatus FAILURE \n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
            if ((nmhSetMplsOutSegmentRowStatus
                 (&OutSegmentIndex, MPLS_STATUS_DESTROY)) == SNMP_FAILURE)
            {
                SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC,
                         "%s:%d nmhSetMplsOutSegmentRowStatus FAILURE \n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
        }
    }

    SR_TRC2 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrCliProcessAdjSidCfg
 * Description   : This routine to process Adj SID configuraiton
 * Input(s)      : pSrSidInfo - Pointer to SID structure
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliProcessAdjSidCfg (tSrSidInterfaceInfo * pSrSidInfo)
{

    tTMO_SLL_NODE       *pOspfNbrNode = NULL;
    tSrV4OspfNbrInfo    *pOspfNbr     = NULL;
    tGenU4Addr          *pIfIpAddr    = NULL;
    UINT4                u4OutIfIndex = 0;
    UINT1                isNbrExist   = SR_FALSE;
    UINT2                u2MlibOperation = 0;
    tGenU4Addr           nextHop;
    tGenU4Addr           destPrefix;

    if (NULL == pSrSidInfo)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d Pointer to SID structure pSrSidInfio == NULL \n",
                __func__, __LINE__);
        return SR_FAILURE;
    }

    SR_TRC5 (SR_CTRL_TRC, "%s:%d Adj Sid %d for the interface %x cfg type %d\n",
            __func__, __LINE__, pSrSidInfo->u4PrefixSidLabelIndex,
            pSrSidInfo->ifIpAddr.Addr.u4Addr, pSrSidInfo->u1AdjSidOprFlags);

    pIfIpAddr = &(pSrSidInfo->ifIpAddr);

    /* Search for OSPF nbr list whether Nbr node present */
    TMO_SLL_Scan (&(gSrV4OspfNbrList), pOspfNbrNode, tTMO_SLL_NODE *)
    {
        pOspfNbr = (tSrV4OspfNbrInfo *) pOspfNbrNode;
        if (pOspfNbr->u4IfIpAddr == pIfIpAddr->Addr.u4Addr)
        {
            isNbrExist = SR_TRUE;
            /* update the area Id */
            pSrSidInfo->u4AreaId = pOspfNbr->u4AreaId;

            /* ToDo: we will update ospf link type which need to update in
               nbr info once we support broadcast */
            break;
        }
    }

    /* If Nbr is not exist, should not create ILM entry, if already created
       delete the entry */
    if (SR_FALSE == isNbrExist)
    {
        if (pSrSidInfo->u1AdjSidInfoFlags&SR_ADJ_SID_ILM_CREATE)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_CRITICAL_TRC,
                    "%s:%d ILM create set for Sid Info %x with no Nbr exist\n",
                    __func__, __LINE__, pIfIpAddr->Addr.u4Addr);
        }
        else
        {
            return SR_SUCCESS;
        }
    }

    /* set dest info as interface info */
    destPrefix.u2AddrType  = pSrSidInfo->ipAddrType;
    destPrefix.Addr.u4Addr = pIfIpAddr->Addr.u4Addr;

    /* set nexthop info as ospf nbr info */
    nextHop.u2AddrType  = pSrSidInfo->ipAddrType;
    if (NULL != pOspfNbr)
    {
    nextHop.Addr.u4Addr = pOspfNbr->ospfV4NbrInfo.u4NbrIpAddr;
    }

#ifdef CFA_WANTED
    if (CfaIpIfGetIfIndexFromHostIpAddressInCxt (MPLS_DEF_CONTEXT_ID,
            pIfIpAddr->Addr.u4Addr, &u4OutIfIndex) ==  CFA_FAILURE)
    {
        SR_TRC3 (SR_CTRL_TRC | SR_CRITICAL_TRC,
                "%s:%d Unable to fetch interface index for IP :0x%x \n",
                __func__, __LINE__, pIfIpAddr->Addr.u4Addr);
        return SR_FAILURE;
    }
#endif


    if ((SR_ADJ_SID_OP_CFG_DEL == pSrSidInfo->u1AdjSidOprFlags)
        || (SR_FALSE == isNbrExist))
    {
        u2MlibOperation = MPLS_MLIB_ILM_DELETE;
    }
    else
    {
        u2MlibOperation = MPLS_MLIB_ILM_CREATE;

    }

    /* create or delete ILM entry based on config and nbr exist */
    if (SrUtilCreateorDeleteILM (pSrSidInfo, pSrSidInfo->u4PrefixSidLabelIndex,
                                &destPrefix, &nextHop, (UINT2) u2MlibOperation,
                                 u4OutIfIndex) == SR_FAILURE)
    {
        if(MPLS_MLIB_ILM_CREATE == u2MlibOperation)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_CTRL_TRC,
                "%s:%d Error in adding MPLS ILM for Adj Sid (%d) in config flow \n",
                __func__, __LINE__, pSrSidInfo->u4PrefixSidLabelIndex);
        }
        else
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                "%s:%d Error in deleting MPLS ILM for Adj Sid (%d) in config flow \n",
                __func__, __LINE__, pSrSidInfo->u4PrefixSidLabelIndex);
        }
        return SR_FAILURE;
    }

    /* set or reset ILM entry create flag based on ILM operation */
    if (MPLS_MLIB_ILM_CREATE == u2MlibOperation)
    {
        pSrSidInfo->u1AdjSidInfoFlags |= SR_ADJ_SID_ILM_CREATE;
        pSrSidInfo->u4IfIndex = u4OutIfIndex;
        SR_TRC4 (SR_CTRL_TRC,
                "%s:%d Added MPLS ILM for Adj Sid (%d) in config flow (%d)\n",
                __func__, __LINE__, pSrSidInfo->u4PrefixSidLabelIndex,
                pSrSidInfo->u1AdjSidOprFlags);
    }
    else
    {
        pSrSidInfo->u1AdjSidInfoFlags &= ~SR_ADJ_SID_ILM_CREATE;
        pSrSidInfo->u4IfIndex = 0;
        SR_TRC4 (SR_CTRL_TRC,
                "%s:%d Deleted MPLS ILM for Adj Sid (%d) in config flow (%d)\n",
                __func__, __LINE__, pSrSidInfo->u4PrefixSidLabelIndex,
                pSrSidInfo->u1AdjSidOprFlags);
    }

    if (NULL != pOspfNbr)
    {
    if (SrProcessAdjSidChange(pSrSidInfo, pOspfNbr) == SR_FAILURE)
    {
        SR_TRC2 (SR_FAIL_TRC | SR_CTRL_TRC,
                "%s:%d SrProcessAdjSidChange Failed \n",__func__, __LINE__);
        return SR_FAILURE;
        }
    }

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrCliProcessAddSid
 * Description   : This routine adds the SID to SID datbase
 * Input(s)      : pSrSidInfo - Pointer to SID structure
 *                 pIfIpAddr    - Pointer to IP address
                   u4IpAddrMask - IP Mask
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliProcessAddSid (tSrSidInterfaceInfo * pSrSidInfo,
                    tGenU4Addr * pIfIpAddr, UINT4 u4IpAddrMask)
{
    UINT4               u4PrefixSidType = SR_ZERO;
    UINT4               u4SidIndexType = SR_ZERO;
    UINT4               u4SidValue = SR_ZERO;
    UINT4               u4OutIfIndex = SR_ZERO;
    UINT1               u1Ret        = SR_SUCCESS;
    tCfaIfInfo          CfaIfInfo;
    tGenU4Addr          destPrefix;
    tGenU4Addr          nextHop;
    tNetIpv4RtInfo      NetIpRtInfo;
    tNetIpv6RtInfo      NetIpV6RtInfo;
    tRtInfoQueryMsg     RtQuery;
    tNetIpv6RtInfoQueryMsg RtV6Query;
    INT4                i4SrStatus = SR_ZERO;

    MEMSET (&NetIpRtInfo, 0, sizeof (tNetIpv4RtInfo));
    MEMSET (&NetIpV6RtInfo, 0, sizeof (tNetIpv6RtInfo));
    MEMSET (&RtQuery, 0, sizeof (tRtInfoQueryMsg));
    MEMSET (&RtV6Query, 0, sizeof (tNetIpv6RtInfoQueryMsg));
    MEMSET (&CfaIfInfo, 0, sizeof (tCfaIfInfo));
    MEMSET (&destPrefix, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&nextHop, SR_ZERO, sizeof (tGenU4Addr));

    destPrefix.u2AddrType = pIfIpAddr->u2AddrType;
    nextHop.u2AddrType = pIfIpAddr->u2AddrType;
    if (pSrSidInfo == NULL)
    {
        SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "%% Failure: SID Entry pointer is NULL\r\n");
        return SR_FAILURE;
    }

    if (destPrefix.u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        u4PrefixSidType = pSrSidInfo->u4SidType;
    }
    else if (destPrefix.u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        u4PrefixSidType = pSrSidInfo->u4SidTypeV3;
    }

    u4SidIndexType = pSrSidInfo->u1SidIndexType;
    u4SidValue = pSrSidInfo->u4PrefixSidLabelIndex;
    if (u4PrefixSidType == SR_SID_NODE)
    {
        if (pSrSidInfo->ifIpAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            if (NetIpv4IsLoopbackAddress (pIfIpAddr->Addr.u4Addr) !=
                NETIPV4_SUCCESS)
            {
                SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                        "% Failure: Node SID Ip Address should be LoopBack Ip Address\r\n");
                return SR_FAILURE;
            }
        }

        if (u4SidIndexType == SR_SID_REL_INDEX)
        {
            if (pSrSidInfo->ifIpAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
            {
                u4SidValue += gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex;
            }
            else if (pSrSidInfo->ifIpAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
            {
                u4SidValue +=
                    gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex;
            }
        }
        pSrSidInfo->u4IfIndex = SR_ZERO;

        nmhGetFsSrV4Status (&i4SrStatus);
        if (i4SrStatus == SR_ENABLED)
         {
            if (SrUtilCreateorDeleteILM (pSrSidInfo, u4SidValue, &destPrefix,
                                         &nextHop, MPLS_MLIB_ILM_CREATE,
                                         SR_ZERO) == SR_FAILURE)
            {
                SR_TRC (SR_CRITICAL_TRC | SR_FAIL_TRC,
                        "% Failure: MplsCliFtnLabelBinding Failed\r\n");
                return SR_FAILURE;
            }
        }

        if (pSrSidInfo->ifIpAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            MEMCPY (gSrGlobalInfo.SrContextV3.rtrId.Addr.Ip6Addr.u1_addr,
                    pIfIpAddr->Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);
        }
    }
    else if ((u4PrefixSidType == SR_SID_ADJACENCY))
    {
        if (pSrSidInfo->ifIpAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
        {
#if 1
            pSrSidInfo->u1AdjSidOprFlags = SR_ADJ_SID_OP_CFG_ADD;
            u1Ret = SrCliProcessAdjSidCfg(pSrSidInfo);
            pSrSidInfo->u1AdjSidOprFlags = SR_ADJ_SID_OP_NONE;

            return u1Ret;

#else

            RtQuery.u4DestinationIpAddress = pIfIpAddr->Addr.u4Addr;
            RtQuery.u4DestinationSubnetMask = u4IpAddrMask;
            RtQuery.u1QueryFlag = RTM_QUERIED_FOR_NEXT_HOP;

            if (NetIpv4GetRoute (&RtQuery, &NetIpRtInfo) == NETIPV4_SUCCESS)
            {
                if (NetIpv4GetCfaIfIndexFromPort (NetIpRtInfo.u4RtIfIndx,
                                                  &u4OutIfIndex) ==
                    NETIPV4_FAILURE)
                {
                    SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                            " Unable to fetch Next hop information\n");
                    return CLI_FAILURE;
                }
            }
            nextHop.Addr.u4Addr = pIfIpAddr->Addr.u4Addr;

#ifdef CFA_WANTED
            if (CfaIpIfGetIfIndexFromHostIpAddressInCxt (MPLS_DEF_CONTEXT_ID,
                                                         pIfIpAddr->Addr.u4Addr,
                                                         &u4OutIfIndex) ==
                CFA_FAILURE)
            {
                SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "% Unable to reach next hop\n");
                return CLI_FAILURE;
            }
#endif
#endif
        }
#ifdef IP6_WANTED
        else if (pSrSidInfo->ifIpAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            RtV6Query.u4ContextId = MPLS_DEF_CONTEXT_ID;
            RtV6Query.u1QueryFlag = RTM6_QUERIED_FOR_NEXT_HOP;
            MEMCPY (RtV6Query.Ip6Dst.u1_addr,
                    pIfIpAddr->Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);

            if (NetIpv6GetRoute (&RtV6Query, &NetIpV6RtInfo) == NETIPV6_SUCCESS)
            {
                if (NetIpv6GetCfaIfIndexFromPort (NetIpV6RtInfo.u4Index,
                                                  &u4OutIfIndex) ==
                    NETIPV4_FAILURE)
                {
                    SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                            " Unable to fetch Next hop information\n");
                    return CLI_FAILURE;
                }
            }
            MEMCPY (nextHop.Addr.Ip6Addr.u1_addr,
                    pIfIpAddr->Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);
        }
#endif
        pSrSidInfo->u4IfIndex = u4OutIfIndex;
        if (SrUtilCreateorDeleteILM (pSrSidInfo, u4SidValue, &destPrefix,
                                     &nextHop, MPLS_MLIB_ILM_CREATE,
                                     u4OutIfIndex) == SR_FAILURE)
        {
            SR_TRC (SR_CRITICAL_TRC | SR_FAIL_TRC, "% Failure: MplsCliStaticXC Failed\r\n");
            return SR_FAILURE;
        }
    }
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrDelSid
 * Description   : This routine deletes the SID entry from SID database
 * Input(s)      : pSrSidEntry - Pointer to Sid Entry in RBtree
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrDelSid (tSrSidInterfaceInfo * pSrSidEntry)
{
    UINT4               label = SR_ZERO;
    tGenU4Addr          nextHop;
    tGenU4Addr          destPrefix;
    UINT1               u1Ret = SR_SUCCESS;
    UINT1               u1LSAType = TYPE10_OPQ_LSA;

    MEMSET (&nextHop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&destPrefix, SR_ZERO, sizeof (tGenU4Addr));

    if (pSrSidEntry == NULL)
    {
        SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "% Pointer to Sid Entry in RBtree is NULL\r\n");
        return SNMP_FAILURE;
    }
    else
    {
        nextHop.u2AddrType = pSrSidEntry->ifIpAddr.u2AddrType;
        destPrefix.u2AddrType = pSrSidEntry->ifIpAddr.u2AddrType;

        if ((pSrSidEntry->u4SidType == SR_SID_NODE) ||
            (pSrSidEntry->u4SidTypeV3 == SR_SID_NODE))
        {
            if (SR_P_INTF_SID_INDEX_TYPE (pSrSidEntry) == SR_SID_REL_INDEX)
            {
                if (pSrSidEntry->ifIpAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
                {
                    label =
                        SR_P_INTF_SID_INDEX (pSrSidEntry) +
                        gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex;
                }
                else if (pSrSidEntry->ifIpAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
                {
                    label =
                        SR_P_INTF_SID_INDEX (pSrSidEntry) +
                        gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex;
                }
            }
            else
            {
                label = SR_P_INTF_SID_INDEX (pSrSidEntry);
            }

            if ((SR_MODE & SR_CONF_OSPF) == SR_CONF_OSPF)
            {
                if(pSrSidEntry->u4SidType == SR_SID_NODE)
                {
                   if (SR_FAILURE == SrUtilSendLSAForSid (FLUSHED_LSA, OSPF_EXT_PRREFIX_TLV_TYPE, u1LSAType, pSrSidEntry))
                   {
                       SR_TRC2 (SR_CTRL_TRC, "%s %d Failure in sending LSA\r\n",
                             __func__, __LINE__);
                       return SR_FAILURE;
                   }
                }
                if(pSrSidEntry->u4SidType == SR_SID_ADJACENCY)
                {
                   if (SR_FAILURE == SrUtilSendLSAForSid (FLUSHED_LSA, OSPF_EXT_LINK_LSA_OPAQUE_TYPE, u1LSAType, pSrSidEntry))
                   {
                       SR_TRC2 (SR_CTRL_TRC, "%s %d Failure in sending LSA\r\n",
                             __func__, __LINE__);
                       return SR_FAILURE;
                   }
                }
            }

            if (SrUtilCreateorDeleteILM (pSrSidEntry, label,
                                         &destPrefix, &nextHop,
                                         (UINT2) MPLS_MLIB_ILM_DELETE,
                                         SR_ZERO) == SR_FAILURE)
            {
                SR_TRC (SR_CRITICAL_TRC | SR_FAIL_TRC,
                        "Error in deleting corresponding MPLS ILM \n");
                return SNMP_FAILURE;
            }

            if (pSrSidEntry->ifIpAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
            {
                MEMSET (gSrGlobalInfo.SrContextV3.rtrId.Addr.Ip6Addr.u1_addr,
                        SR_ZERO, SR_IPV6_ADDR_LENGTH);
            }
            SR_IPV6_NODE_SID = SR_ZERO;
            gSrGlobalInfo.pV6SrSelfNodeInfo = NULL;
        }
        else
        {
            if (pSrSidEntry->ifIpAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
            {
#if 1
                pSrSidEntry->u1AdjSidOprFlags = SR_ADJ_SID_OP_CFG_DEL;
                u1Ret = SrCliProcessAdjSidCfg(pSrSidEntry);
                pSrSidEntry->u1AdjSidOprFlags = SR_ADJ_SID_OP_NONE;

                if (SR_SUCCESS != u1Ret)
                {
                    return SNMP_FAILURE;
                }

                if (RBTreeRem (gSrGlobalInfo.pSrSidRbTree, pSrSidEntry) == NULL)
                {
                    SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "Error in deleting SR entry from RB Tree\n");
                    return SNMP_FAILURE;
                }
                SR_INTF_FREE (pSrSidEntry);
                return SNMP_SUCCESS;
#else
                nextHop.Addr.u4Addr = pSrSidEntry->ifIpAddr.Addr.u4Addr;
#endif
            }
            else if (pSrSidEntry->ifIpAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
            {
                MEMCPY (nextHop.Addr.Ip6Addr.u1_addr,
                        pSrSidEntry->ifIpAddr.Addr.Ip6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH);

                if (SrUtilCreateorDeleteILM
                    (pSrSidEntry, SR_P_INTF_SID_INDEX (pSrSidEntry), &destPrefix,
                     &nextHop, (UINT2) MPLS_MLIB_ILM_DELETE,
                     pSrSidEntry->u4IfIndex) == SR_FAILURE)
                {
                    SR_TRC (SR_CRITICAL_TRC | SR_FAIL_TRC,
                            "Error in deleting corresponding MPLS ILM \n");
                    return SNMP_FAILURE;
                }
	    }
        }

        if (RBTreeRem (gSrGlobalInfo.pSrSidRbTree, pSrSidEntry) == NULL)
        {
            SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "Error in deleting SR entry from RB Tree\n");
            return SNMP_FAILURE;
        }
    }

    SR_INTF_FREE (pSrSidEntry);
    return SNMP_SUCCESS;
}

#ifdef CLI_WANTED
/*****************************************************************************
 * Function Name : SrCliShowRunningConfig
 * Description   : This routine is used to print the current configurations
 *                                 of Segment Routing
 * Input         : CliHandle    - Index of current CLI context
 *
 * Output(s)     : None
 * Return(s)     : None
 *****************************************************************************/
PUBLIC VOID
SrCliShowRunningConfig (tCliHandle CliHandle)
{
    INT4                i4GblSrMode = SR_DISABLED;
    INT4                srSidAddrType = SR_ZERO;
    INT4                srSidNextAddrType = SR_ZERO;
    UINT4               srSidIpMask = SR_ZERO;
    UINT4               srSidNextIpMask = SR_ZERO;
    UINT4               srgbRange[SR_TWO] = { SR_ZERO };
    INT4                i4RetStatus = SNMP_FAILURE;
    INT4                i4IpAddrFamily = SR_FAILURE;
    UINT4               u4ipAddr = SR_ZERO;
    UINT1               ipAddr[MAX_SR_TEXT_SIZE] = { SR_ZERO };
    UINT1               ipAddrMask[MAX_SR_TEXT_SIZE] = { SR_ZERO };
    tSNMP_OCTET_STRING_TYPE srSidIpAddr;
    tSNMP_OCTET_STRING_TYPE srSidNextIpAddr;
    tSNMP_OCTET_STRING_TYPE *pSrSidNextIpAddr = &srSidNextIpAddr;
    tSNMP_OCTET_STRING_TYPE *pSrSidIpAddr = &srSidIpAddr;
    UINT1               au1Ipv4Addr[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1NextIpv6Addr[SR_IPV6_ADDR_LENGTH] = { SR_ZERO };
    UINT1              *pTemp = NULL;
    UINT4               u4TempAddr = SR_ZERO;
    UINT4               sidNodeCount = SR_ZERO;
    UINT4               sidNodeCounter = SR_ZERO;
    INT4                i4SrStatus = SR_ZERO;
    UINT1               u1Flag = SR_FALSE;
    UINT4               u4SrAltStatus = SR_ZERO;
    tSrSidInterfaceInfo SrSidIntf;
    tSrSidInterfaceInfo *pSrSidEntry = NULL;

    MEMSET (&srSidIpAddr, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    MEMSET (&srSidNextIpAddr, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    MEMSET (&SrSidIntf, SR_ZERO, sizeof (tSrSidInterfaceInfo));

    srSidIpAddr.pu1_OctetList = au1Ipv4Addr;
    srSidNextIpAddr.pu1_OctetList = au1NextIpv6Addr;

    nmhGetFsSrGlobalAdminStatus (&i4GblSrMode);
    if (i4GblSrMode == SR_ENABLED)
    {
        CliPrintf (CliHandle, "segment-routing mpls\r\n");
        FilePrintf (CliHandle, "segment-routing mpls\r\n");

        nmhGetFsSrIpv4AddrFamily (&i4IpAddrFamily);
        if (i4IpAddrFamily == SR_ENABLED)
        {
            CliPrintf (CliHandle, "segment-routing address-family ipv4\r\n");
            FilePrintf (CliHandle, "segment-routing address-family ipv4\r\n");

            nmhGetFsSrV4AlternateStatus (&u4SrAltStatus);
            if (u4SrAltStatus == ALTERNATE_ENABLED)
            {
                CliPrintf (CliHandle, "segment-routing alternate enable\r\n");
                FilePrintf (CliHandle, "segment-routing alternate enable\r\n");
            }
            else if (u4SrAltStatus == ALTERNATE_DISABLED)
            {
                CliPrintf (CliHandle, "segment-routing alternate disable\r\n");
                FilePrintf (CliHandle, "segment-routing alternate disable\r\n");
            }

            if (IS_V4_SRGB_CONFIGURED == SR_TRUE)
            {
                /*Check has been introduced to not to print label range with 0 values upon MSR
                 * ToDo: Validate funcions for SR, Min, Max are required to ignore nmhGetFsSrMinSrGbValue at write-startup cmd*/
                if ((gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex !=
                     SR_ZERO)
                    && (gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMaxIndex !=
                        SR_ZERO))
                {
                    if ((gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex !=
                         SRGB_DEFAULT_MIN_VALUE)
                        || (gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMaxIndex !=
                            SRGB_DEFAULT_MAX_VALUE))
                    {
                        u1Flag = SR_TRUE;
                        nmhGetFsSrV4MinSrGbValue (&srgbRange[0]);
                        nmhGetFsSrV4MaxSrGbValue (&srgbRange[1]);
                        CliPrintf (CliHandle,
                                   "srgb label range min %u max %u\r\n",
                                   srgbRange[0], srgbRange[1]);
                        FilePrintf (CliHandle,
                                    "srgb label range min %u max %u\r\n",
                                    srgbRange[0], srgbRange[1]);
                    }
                }
            }
            i4RetStatus = nmhGetFirstIndexFsSrSIDTable (&srSidNextAddrType,
                                                        pSrSidNextIpAddr,
                                                        &srSidNextIpMask);

            RBTreeCount (gSrGlobalInfo.pSrSidRbTree, &sidNodeCount);
            sidNodeCounter = SR_ZERO;
            while (i4RetStatus != SNMP_FAILURE)
            {
                if (sidNodeCounter++ == sidNodeCount)
                {
                    break;
                }

                SrSidIntf.ipAddrType = (UINT1) srSidNextAddrType;
                if (SrSidIntf.ipAddrType == SR_IPV4_ADDR_TYPE)
                {
                    SR_OCTETSTRING_TO_INTEGER (pSrSidNextIpAddr,
                                               SrSidIntf.ifIpAddr.Addr.u4Addr);
                }
                else if (SrSidIntf.ipAddrType == SR_IPV6_ADDR_TYPE)
                {
                    MEMCPY (SrSidIntf.ifIpAddr.Addr.Ip6Addr.u1_addr,
                            pSrSidNextIpAddr->pu1_OctetList,
                            pSrSidNextIpAddr->i4_Length);
                }

                pSrSidEntry =
                    RBTreeGet (gSrGlobalInfo.pSrSidRbTree, &SrSidIntf);

                if (pSrSidEntry == NULL)
                {
                    break;
                }

                if ((pSrSidEntry->u1RowStatus != ACTIVE)
                    || (pSrSidEntry->ipAddrType == SR_IPV6_ADDR_TYPE))
                {
                    srSidAddrType = pSrSidEntry->ipAddrType;
                    srSidNextAddrType = SR_ZERO;

                    MEMCPY (&u4ipAddr, &pSrSidEntry->ifIpAddr.Addr.u4Addr,
                            sizeof (UINT4));
                    SR_INTEGER_TO_OCTETSTRING (u4ipAddr, pSrSidIpAddr);
                    MEMSET (pSrSidNextIpAddr->pu1_OctetList, SR_ZERO,
                            sizeof (pSrSidNextIpAddr->i4_Length));

                    MEMCPY (&srSidIpMask, &pSrSidEntry->ifIpAddrMask,
                            sizeof (UINT4));
                    srSidIpMask = OSIX_HTONL (srSidIpMask);
                    srSidNextIpMask = SR_ZERO;

                    nmhGetNextIndexFsSrSIDTable (srSidAddrType,
                                                 &srSidNextAddrType,
                                                 &srSidIpAddr, &srSidNextIpAddr,
                                                 srSidIpMask, &srSidNextIpMask);

                    continue;
                }

                CliPrintf (CliHandle, "segment-routing add sid type ");
                FilePrintf (CliHandle, "segment-routing add sid type ");

                if (pSrSidEntry->u4SidType == SR_SID_NODE)
                {
                    if (pSrSidEntry->u1SidIndexType == SR_SID_REL_INDEX)
                    {
                        CliPrintf (CliHandle, "Node-SID sid-value relative ");
                        FilePrintf (CliHandle, "Node-SID sid-value relative ");
                    }
                    else
                    {
                        CliPrintf (CliHandle, "Node-SID sid-value absolute ");
                        FilePrintf (CliHandle, "Node-SID sid-value absolute ");
                    }
                }
                else if (pSrSidEntry->u4SidType == SR_SID_ADJACENCY)
                {
                    u1Flag = SR_TRUE;
                    CliPrintf (CliHandle, "Adj-SID sid-value ");
                    FilePrintf (CliHandle, "Adj-SID sid-value ");
                }
                CliPrintf (CliHandle, "%u ",
                           pSrSidEntry->u4PrefixSidLabelIndex);
                FilePrintf (CliHandle, "%u ",
                            pSrSidEntry->u4PrefixSidLabelIndex);
                if (pSrSidEntry->ipAddrType == SR_IPV4_ADDR_TYPE)
                {
                    u4TempAddr = pSrSidEntry->ifIpAddr.Addr.u4Addr;
                    u4TempAddr = OSIX_HTONL (u4TempAddr);
                    pTemp = (UINT1 *) &u4TempAddr;
                    SPRINTF ((char *__restrict__) ipAddr, "%d.%d.%d.%d",
                             pTemp[0], pTemp[1], pTemp[2], pTemp[3]);
                }
                else if (pSrSidEntry->ipAddrType == SR_IPV6_ADDR_TYPE)
                {
                    SPRINTF ((char *__restrict__) ipAddr, "%s",
                             Ip6PrintAddr (&pSrSidEntry->ifIpAddr.Addr.
                                           Ip6Addr));
                }

                u4TempAddr = pSrSidEntry->ifIpAddrMask;
                u4TempAddr = OSIX_HTONL (u4TempAddr);
                pTemp = (UINT1 *) &u4TempAddr;
                SPRINTF ((char *__restrict__) ipAddrMask, "%d.%d.%d.%d",
                         pTemp[0], pTemp[1], pTemp[2], pTemp[3]);

                if (pSrSidEntry->u4SidType == SR_SID_ADJACENCY)
                {
                    CliPrintf (CliHandle, "net %s %s\r\n", ipAddr, ipAddrMask);
                    FilePrintf (CliHandle, "net %s %s\r\n", ipAddr, ipAddrMask);
                }
                else
                {
                    CliPrintf (CliHandle, "net %s\r\n", ipAddr);
                    FilePrintf (CliHandle, "net %s\r\n", ipAddr);
                }
                srSidAddrType = pSrSidEntry->ipAddrType;
                srSidNextAddrType = SR_ZERO;

                MEMCPY (&u4ipAddr, &pSrSidEntry->ifIpAddr.Addr.u4Addr,
                        sizeof (UINT4));
                SR_INTEGER_TO_OCTETSTRING (u4ipAddr, pSrSidIpAddr);
                MEMSET (pSrSidNextIpAddr->pu1_OctetList, SR_ZERO,
                        sizeof (pSrSidNextIpAddr->i4_Length));

                MEMCPY (&srSidIpMask, &pSrSidEntry->ifIpAddrMask,
                        sizeof (UINT4));
                srSidIpMask = OSIX_HTONL (srSidIpMask);
                srSidNextIpMask = SR_ZERO;

                nmhGetNextIndexFsSrSIDTable (srSidAddrType, &srSidNextAddrType,
                                             &srSidIpAddr, &srSidNextIpAddr,
                                             srSidIpMask, &srSidNextIpMask);
            }

            nmhGetFsSrV4Status (&i4SrStatus);
            if (i4SrStatus == SR_ENABLED)
            {
                CliPrintf (CliHandle, "no shutdown\r\n");
                FilePrintf (CliHandle, "no shutdown\r\n");
            }
            else
            {
                CliPrintf (CliHandle, "shutdown\r\n");
                FilePrintf (CliHandle, "shutdown\r\n");
            }

            if (SrStaticBindingShowRunningConfig (CliHandle) == SNMP_SUCCESS)
            {
                SR_TRC2 (SR_CTRL_TRC, "%s:%d Static SR show run success\n",
                         __func__, __LINE__);
            }
            CliPrintf (CliHandle, "!\r\n");
            FilePrintf (CliHandle, "!\r\n");
        }
        i4IpAddrFamily = SR_FAILURE;
        nmhGetFsSrIpv6AddrFamily (&i4IpAddrFamily);
        if (i4IpAddrFamily == SR_ENABLED)
        {
            CliPrintf (CliHandle, "segment-routing address-family ipv6\r\n");
            FilePrintf (CliHandle, "segment-routing address-family ipv6\r\n");

            nmhGetFsSrV6AlternateStatus (&u4SrAltStatus);
            if (u4SrAltStatus == ALTERNATE_ENABLED)
            {
                CliPrintf (CliHandle, "segment-routing alternate enable\r\n");
                FilePrintf (CliHandle, "segment-routing alternate enable\r\n");
            }
            else if (u4SrAltStatus == ALTERNATE_DISABLED)
            {
                CliPrintf (CliHandle, "segment-routing alternate disable\r\n");
                FilePrintf (CliHandle, "segment-routing alternate disable\r\n");
            }

            if (IS_V6_SRGB_CONFIGURED == SR_TRUE)
            {
                /*Check has been introduced to not to print label range with 0 values upon MSR
                 * ToDo: Validate funcions for SR, Min, Max are required to ignore nmhGetFsSrMinSrGbValue at write-startup cmd*/
                if ((gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex !=
                     SR_ZERO)
                    && (gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MaxIndex !=
                        SR_ZERO))
                {
                    if ((gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex !=
                         SRGB_DEFAULT_MIN_VALUE)
                        || (gSrGlobalInfo.SrContextV3.SrGbRange.
                            u4SrGbV3MaxIndex != SRGB_DEFAULT_MAX_VALUE))
                    {
                        CliPrintf (CliHandle, "shutdown\r\n");
                        FilePrintf (CliHandle, "shutdown\r\n");
                        u1Flag = SR_TRUE;
                        nmhGetFsSrV6MinSrGbValue (&srgbRange[0]);
                        nmhGetFsSrV6MaxSrGbValue (&srgbRange[1]);
                        CliPrintf (CliHandle,
                                   "srgb label range min %u max %u\r\n",
                                   srgbRange[0], srgbRange[1]);
                        FilePrintf (CliHandle,
                                    "srgb label range min %u max %u\r\n",
                                    srgbRange[0], srgbRange[1]);
                    }
                }
            }
            i4RetStatus =
                nmhGetFirstIndexFsSrSIDTable (&srSidNextAddrType,
                                              pSrSidNextIpAddr,
                                              &srSidNextIpMask);

            RBTreeCount (gSrGlobalInfo.pSrSidRbTree, &sidNodeCount);
            sidNodeCounter = SR_ZERO;
            while (i4RetStatus != SNMP_FAILURE)
            {
                if (sidNodeCounter++ == sidNodeCount)
                {
                    break;
                }
                SrSidIntf.ipAddrType = (UINT1) srSidNextAddrType;
                if (SrSidIntf.ipAddrType == SR_IPV4_ADDR_TYPE)
                {
                    SR_OCTETSTRING_TO_INTEGER (pSrSidNextIpAddr,
                                               SrSidIntf.ifIpAddr.Addr.u4Addr);
                }
                else if (SrSidIntf.ipAddrType == SR_IPV6_ADDR_TYPE)
                {
                    MEMCPY (SrSidIntf.ifIpAddr.Addr.Ip6Addr.u1_addr,
                            pSrSidNextIpAddr->pu1_OctetList,
                            pSrSidNextIpAddr->i4_Length);
                }
                pSrSidEntry =
                    RBTreeGet (gSrGlobalInfo.pSrSidRbTree, &SrSidIntf);

                if (pSrSidEntry == NULL)
                {
                    break;
                }
                if ((pSrSidEntry->u1RowStatus != ACTIVE)
                    || (pSrSidEntry->ipAddrType == SR_IPV4_ADDR_TYPE))
                {
                    srSidAddrType = pSrSidEntry->ipAddrType;
                    srSidNextAddrType = SR_ZERO;
                    MEMCPY (&u4ipAddr, &pSrSidEntry->ifIpAddr.Addr.u4Addr,
                            sizeof (UINT4));
                    SR_INTEGER_TO_OCTETSTRING (u4ipAddr, pSrSidIpAddr);
                    MEMSET (pSrSidNextIpAddr->pu1_OctetList, SR_ZERO,
                            sizeof (pSrSidNextIpAddr->i4_Length));

                    MEMCPY (&srSidIpMask, &pSrSidEntry->ifIpAddrMask,
                            sizeof (UINT4));
                    srSidIpMask = OSIX_HTONL (srSidIpMask);
                    srSidNextIpMask = SR_ZERO;

                    nmhGetNextIndexFsSrSIDTable (srSidAddrType,
                                                 &srSidNextAddrType,
                                                 &srSidIpAddr, &srSidNextIpAddr,
                                                 srSidIpMask, &srSidNextIpMask);

                    continue;
                }

                if (u1Flag != SR_TRUE)
                {
                    CliPrintf (CliHandle, "shutdown\r\n");
                    FilePrintf (CliHandle, "shutdown\r\n");
                }

                CliPrintf (CliHandle, "segment-routing add sid type ");
                FilePrintf (CliHandle, "segment-routing add sid type ");

                if (pSrSidEntry->u4SidTypeV3 == SR_SID_NODE)
                {
                    if (pSrSidEntry->u1SidIndexType == SR_SID_REL_INDEX)
                    {
                        CliPrintf (CliHandle, "Node-SID sid-value relative ");
                        FilePrintf (CliHandle, "Node-SID sid-value relative ");
                    }
                    else
                    {
                        CliPrintf (CliHandle, "Node-SID sid-value absolute ");
                        FilePrintf (CliHandle, "Node-SID sid-value absolute ");
                    }
                }
                else if (pSrSidEntry->u4SidTypeV3 == SR_SID_ADJACENCY)
                {
                    CliPrintf (CliHandle, "Adj-SID sid-value ");
                    FilePrintf (CliHandle, "Adj-SID sid-value ");
                }
                CliPrintf (CliHandle, "%u ",
                           pSrSidEntry->u4PrefixSidLabelIndex);
                FilePrintf (CliHandle, "%u ",
                            pSrSidEntry->u4PrefixSidLabelIndex);
                if (pSrSidEntry->ipAddrType == SR_IPV4_ADDR_TYPE)
                {
                    u4TempAddr = pSrSidEntry->ifIpAddr.Addr.u4Addr;
                    u4TempAddr = OSIX_HTONL (u4TempAddr);
                    pTemp = (UINT1 *) &u4TempAddr;
                    SPRINTF ((char *__restrict__) ipAddr, "%d.%d.%d.%d",
                             pTemp[0], pTemp[1], pTemp[2], pTemp[3]);
                }
                else if (pSrSidEntry->ipAddrType == SR_IPV6_ADDR_TYPE)
                {
                    SPRINTF ((char *__restrict__) ipAddr, "%s",
                             Ip6PrintAddr (&pSrSidEntry->ifIpAddr.Addr.
                                           Ip6Addr));
                }

                u4TempAddr = pSrSidEntry->ifIpAddrMask;
                u4TempAddr = OSIX_HTONL (u4TempAddr);
                pTemp = (UINT1 *) &u4TempAddr;
                SPRINTF ((char *__restrict__) ipAddrMask, "%d.%d.%d.%d",
                         pTemp[0], pTemp[1], pTemp[2], pTemp[3]);

                if (pSrSidEntry->u4SidTypeV3 == SR_SID_ADJACENCY)
                {
                    CliPrintf (CliHandle, "net %s %s\r\n", ipAddr, ipAddrMask);
                    FilePrintf (CliHandle, "net %s %s\r\n", ipAddr, ipAddrMask);
                }
                else
                {
                    CliPrintf (CliHandle, "net %s\r\n", ipAddr);
                    FilePrintf (CliHandle, "net %s\r\n", ipAddr);
                }
                srSidAddrType = pSrSidEntry->ipAddrType;
                srSidNextAddrType = SR_ZERO;
                MEMCPY (&u4ipAddr, &pSrSidEntry->ifIpAddr.Addr.u4Addr,
                        sizeof (UINT4));
                SR_INTEGER_TO_OCTETSTRING (u4ipAddr, pSrSidIpAddr);
                MEMSET (pSrSidNextIpAddr->pu1_OctetList, SR_ZERO,
                        sizeof (pSrSidNextIpAddr->i4_Length));

                MEMCPY (&srSidIpMask, &pSrSidEntry->ifIpAddrMask,
                        sizeof (UINT4));
                srSidIpMask = OSIX_HTONL (srSidIpMask);
                srSidNextIpMask = SR_ZERO;

                nmhGetNextIndexFsSrSIDTable (srSidAddrType, &srSidNextAddrType,
                                             &srSidIpAddr, &srSidNextIpAddr,
                                             srSidIpMask, &srSidNextIpMask);
            }
            MplsStaticBindingShowRunningConfig (CliHandle, SR_TRUE);
            i4SrStatus = SR_DISABLED;
            nmhGetFsSrV6Status (&i4SrStatus);
            if (i4SrStatus == SR_ENABLED)
            {
                CliPrintf (CliHandle, "no shutdown\r\n");
                FilePrintf (CliHandle, "no shutdown\r\n");
            }
            CliPrintf (CliHandle, "!\r\n");
            FilePrintf (CliHandle, "!\r\n");
        }
    }
    CliPrintf (CliHandle, "!\r\n");
    FilePrintf (CliHandle, "!\r\n");
}

/*****************************************************************************
 * Function Name : SrStaticBindingShowRunningConfig
 * Description   : This routine is used to print the current configurations
 *                                 of Segment Routing Static Binding
 * Input         : CliHandle    - Index of current CLI context
 *
 * Output(s)     : None
 * Return(s)     : None
 *****************************************************************************/
INT4
SrStaticBindingShowRunningConfig (tCliHandle CliHandle)
{
    CHR1               *pc1DestAddr = NULL;
    CHR1               *pc1MaxAddr = NULL;
    CHR1               *pc1NextHopAddr = NULL;
    UINT4               u4NextHopAddr = SR_ZERO;
    UINT4               u4StackCount = SR_ZERO;
    UINT4               u4StaticSrLabelStackIndex = SR_ZERO;
    UINT4               u4Label = SR_ZERO;
    UINT1               au1DestAddr[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1nextHop[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    tSrStaticEntryInfo *pStaticSrInfo = NULL;

    tSNMP_OCTET_STRING_TYPE snmpDestAddr;
    tSNMP_OCTET_STRING_TYPE snmpNextHop;
    tSNMP_OCTET_STRING_TYPE *pSnmpDestAddr = NULL;
    tSNMP_OCTET_STRING_TYPE *pSnmpNextHop = NULL;

    MEMSET (&snmpDestAddr, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    MEMSET (&snmpNextHop, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));

    pSnmpDestAddr = &snmpDestAddr;
    pSnmpNextHop = &snmpNextHop;
    snmpDestAddr.pu1_OctetList = au1DestAddr;
    snmpNextHop.pu1_OctetList = au1nextHop;

    if (gSrGlobalInfo.pSrStaticRbTree == NULL)
    {
        SR_TRC2 (SR_CTRL_TRC, "%s:%d Static SR is NULL\n", __func__,
                 __LINE__);
        return SR_SUCCESS;
    }

    if ((pStaticSrInfo =
         RBTreeGetFirst (gSrGlobalInfo.pSrStaticRbTree)) == NULL)
    {
        SR_TRC2 (SR_CTRL_TRC,
                 "%s:%d Segment-Routing Static Binding not configured\n",
                 __func__, __LINE__);
        return SR_SUCCESS;
    }

    while (pStaticSrInfo != NULL)
    {
        CliPrintf (CliHandle, "segment-routing static binding ");
        FilePrintf (CliHandle, "segment-routing static binding ");
        CLI_CONVERT_IPADDR_TO_STR (pc1DestAddr,
                                   pStaticSrInfo->destAddr.Addr.u4Addr);
        CliPrintf (CliHandle, "%s ", pc1DestAddr);
        FilePrintf (CliHandle, "%s ", pc1DestAddr);

        CLI_CONVERT_IPADDR_TO_STR (pc1MaxAddr, pStaticSrInfo->u4DestMask);
        CliPrintf (CliHandle, "%s ", pc1MaxAddr);
        FilePrintf (CliHandle, "%s ", pc1MaxAddr);

        SR_INTEGER_TO_OCTETSTRING (pStaticSrInfo->destAddr.Addr.u4Addr,
                                   pSnmpDestAddr);

        if (pStaticSrInfo->u4LspId != 0)
		{
			CliPrintf (CliHandle, "id %d ", pStaticSrInfo->u4LspId);
			FilePrintf (CliHandle, "id %d ", pStaticSrInfo->u4LspId);
		}

        if (nmhGetFsStaticSrNextHopAddr
            (pSnmpDestAddr, pStaticSrInfo->u4DestMask,
             pStaticSrInfo->destAddr.u2AddrType, pStaticSrInfo->u4LspId, pSnmpNextHop) != SNMP_SUCCESS)
        {
            SR_TRC2 (SR_CTRL_TRC, "%s:%d NextHop Get Failure \n", __func__,
                     __LINE__);
        }

        SR_OCTETSTRING_TO_INTEGER (pSnmpNextHop, u4NextHopAddr);
        CLI_CONVERT_IPADDR_TO_STR (pc1NextHopAddr, u4NextHopAddr);

        CliPrintf (CliHandle, "nexthop %s ", pc1NextHopAddr);
        FilePrintf (CliHandle, "nexthop %s ", pc1NextHopAddr);

        CliPrintf (CliHandle, "label-stack ");
        FilePrintf (CliHandle, "label-stack ");

        if (nmhGetFsStaticSrLabelStackIndex (pSnmpDestAddr,
                                             pStaticSrInfo->u4DestMask,
                                             (INT4) SR_IPV4_ADDR_TYPE,
                                             pStaticSrInfo->u4LspId,
                                             &u4StaticSrLabelStackIndex) !=
            SNMP_SUCCESS)
        {
            SR_TRC2 (SR_MGMT_TRC | SR_FAIL_TRC, "%s:%d LabelStack Index Get Failure \n",
                     __func__, __LINE__);
        }
        /* Printing Top Label*/
        nmhGetFsStaticSrLabelStackLabel (u4StaticSrLabelStackIndex,
                                         u4StackCount, &u4Label);
        CliPrintf (CliHandle, "%u", u4Label);
        FilePrintf (CliHandle, "%u", u4Label);

        /*Printing rest of labels in label stack*/
        for (u4StackCount = SR_ONE;
             u4StackCount < MAX_MPLSDB_LABELS_PER_ENTRY; u4StackCount++)
        {
            nmhGetFsStaticSrLabelStackLabel (u4StaticSrLabelStackIndex,
                                             u4StackCount, &u4Label);
            if (u4Label == SR_ZERO)
            {
                continue;
            }
            else
            {
                CliPrintf (CliHandle, " %u", u4Label);
                FilePrintf (CliHandle, " %u", u4Label);
            }
        }
        CliPrintf (CliHandle, "\r\n");
        FilePrintf (CliHandle, "\r\n");
        pStaticSrInfo =
            RBTreeGetNext (gSrGlobalInfo.pSrStaticRbTree, pStaticSrInfo, NULL);
    }
    return SR_SUCCESS;
}
#endif

/*****************************************************************************
 * Function Name : IssSrShowDebugging
 * Description   : This routine displays the status of debugging flag.
 * Input(s)      : CliHandle - Index of current CLI context
 * Output(s)     : None
 * Return(s)     : None
 *****************************************************************************/
VOID
IssSrShowDebugging (tCliHandle CliHandle)
{
    UINT4               debugMode;
    if (nmhGetFsSrDebugFlag (&debugMode) == SNMP_SUCCESS)
    {
        if (debugMode != SR_ZERO)
        {
            CliPrintf (CliHandle, "\r\nSR :\n  SR debugging is ON\r\n");
        }
    }
}

/******************************************************************************/
/* Function Name     : SrCliGetCurMode                                        */
/* Description       : This function returns the current configuration mode   */
/* Input Parameters  : None                                                   */
/* Output Parameters : pu4Afi - Address family mode                           */
/* Return Value      : None                                                   */
/******************************************************************************/
VOID
SrCliGetCurMode (UINT4 *pu4Afi)
{
    UINT1               au1CliPrompt[MAX_PROMPT_LEN] = { SR_ZERO };
    if (SrCliGetCurPrompt (au1CliPrompt) == SR_FAILURE)
    {
        SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "%Not able to get cli prompt\r\n");
        return;
    }
    if ((STRNCMP (au1CliPrompt, SR_IPV4_ADDR_FAMLY_MODE,
                  STRLEN (SR_IPV4_ADDR_FAMLY_MODE)) == 0))
    {
        *pu4Afi = SR_CLI_IPV4;
    }
    else if ((STRNCMP (au1CliPrompt, SR_IPV6_ADDR_FAMLY_MODE,
                       STRLEN (SR_IPV6_ADDR_FAMLY_MODE)) == 0))
    {
        *pu4Afi = SR_CLI_IPV6;
    }
}

/******************************************************************************/
/* Function Name     : SrCliGetSrMode                                         */
/* Description       : This function returns the current segment-routing mode */
/* Input Parameters  : None                                                   */
/* Output Parameters : pi4SrMode - Address family mode                        */
/* Return Value      : None                                                   */
/******************************************************************************/
VOID
SrCliGetSrMode (INT4 *pi4SrMode)
{
    nmhGetFsSrMode (pi4SrMode);
}

/******************************************************************************/
/* Function Name     : SrCliGetSrV4AlternateStatus                            */
/* Description       : This function returns the segment-routing alternate    */
/*                     status for Ipv4                                        */
/* Input Parameters  : None                                                   */
/* Output Parameters : pi4SrAltStatus - Ipv4 Atlernate status                 */
/* Return Value      : None                                                   */
/******************************************************************************/
VOID
SrCliGetSrV4AlternateStatus (INT4 *pi4SrAltStatus)
{
    nmhGetFsSrV4AlternateStatus (pi4SrAltStatus);
}

/******************************************************************************/
/* Function Name     : SrShowTePathInfoTable                                  */
/* Description       : This function displays the TE path table info          */
/* Input Parameters  : None                                                   */
/* Output Parameters : None                                                   */
/* Return Value      : None                                                   */
/******************************************************************************/
INT4
SrShowTePathInfoTable (tCliHandle CliHandle, UINT4 u4AddrType)
{
    tGenU4Addr          cmpDestAddr;
    tGenU4Addr          dummyAddr;
    tGenU4Addr          destAddr;
    tGenU4Addr          mandRtrId;
    UINT4               u4DestMask = SR_ZERO;
    UINT4               u4PrevDestMask = SR_ZERO;
    UINT4               u4TempDestAddr = SR_ZERO;
    UINT4               u4TempDestMask = SR_ZERO;
    UINT4               u4TempRtrAddr = SR_ZERO;
    INT4                i4FsSrTeDestAddrType = SR_ZERO;
    INT4                i4NextFsSrTeDestAddrType = SR_ZERO;
    UINT1               au1DestAddrV6[SR_IPV6_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1PrevDestAddrV6[SR_IPV6_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1MandatoryRtrIdV6[SR_IPV6_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1PrevMandatoryRtrIdV6[SR_IPV6_ADDR_LENGTH] =
        { SR_ZERO };
    UINT1               au1PrintDestAddr[MAX_SR_TEXT_SIZE] = { SR_ZERO };
    UINT1               au1PrintDestMask[MAX_SR_TEXT_SIZE] = { SR_ZERO };
    UINT1               au1PrintMandRtrId[MAX_SR_TEXT_SIZE] = { SR_ZERO };
    UINT1               destAddress[MAX_SR_TEXT_SIZE] = { SR_ZERO };
    UINT1               destMask[MAX_SR_TEXT_SIZE] = { SR_ZERO };
    UINT1               mandRouterId[MAX_SR_TEXT_SIZE] = { SR_ZERO };
    UINT1              *pTemp = NULL;

    tSNMP_OCTET_STRING_TYPE snmpDestAddr;
    tSNMP_OCTET_STRING_TYPE snmpPrevDestAddr;
    tSNMP_OCTET_STRING_TYPE snmpMandatoryRtrId;
    tSNMP_OCTET_STRING_TYPE snmpPrevMandatoryRtrId;
    tSNMP_OCTET_STRING_TYPE *pSnmpDestAddr = NULL;
    tSNMP_OCTET_STRING_TYPE *pSnmpPrevDestAddr = NULL;
    tSNMP_OCTET_STRING_TYPE *pSnmpMandatoryRtrId = NULL;
    tSNMP_OCTET_STRING_TYPE *pSnmpPrevMandatoryRtrId = NULL;

    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&cmpDestAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&snmpDestAddr, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    MEMSET (&snmpPrevDestAddr, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    MEMSET (&snmpMandatoryRtrId, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    MEMSET (&snmpPrevMandatoryRtrId, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    MEMSET (&mandRtrId, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&destAddr, SR_ZERO, sizeof (tGenU4Addr));
    destAddr.u2AddrType = SR_IPV4_ADDR_TYPE;

    pSnmpDestAddr = &snmpDestAddr;
    pSnmpPrevDestAddr = &snmpPrevDestAddr;
    pSnmpMandatoryRtrId = &snmpMandatoryRtrId;
    pSnmpPrevMandatoryRtrId = &snmpPrevMandatoryRtrId;

    snmpDestAddr.pu1_OctetList = au1DestAddrV6;
    snmpPrevDestAddr.pu1_OctetList = au1PrevDestAddrV6;
    snmpMandatoryRtrId.pu1_OctetList = au1MandatoryRtrIdV6;
    snmpPrevMandatoryRtrId.pu1_OctetList = au1PrevMandatoryRtrIdV6;

    do
    {
        /*Only Execute in 1st iteration */
        if ((MEMCMP (&cmpDestAddr, &dummyAddr, sizeof (tGenU4Addr)) == SR_ZERO)
            &&
            (nmhGetFirstIndexFsSrTePathTable
             (pSnmpDestAddr, &u4DestMask, pSnmpMandatoryRtrId,
              &i4FsSrTeDestAddrType) == SNMP_FAILURE))
        {
            break;
        }

        if ((MEMCMP (&cmpDestAddr, &dummyAddr, sizeof (tGenU4Addr)) != SR_ZERO)
            &&
            ((nmhGetNextIndexFsSrTePathTable
              (pSnmpPrevDestAddr, pSnmpDestAddr, u4PrevDestMask, &u4DestMask,
               pSnmpPrevMandatoryRtrId, pSnmpMandatoryRtrId,
               i4FsSrTeDestAddrType,
               &i4NextFsSrTeDestAddrType) == SNMP_FAILURE)))
        {
            break;
        }

        if (i4NextFsSrTeDestAddrType != SR_ZERO)
        {
            i4FsSrTeDestAddrType = i4NextFsSrTeDestAddrType;
            i4NextFsSrTeDestAddrType = SR_ZERO;
        }

        MEMSET (au1PrintDestAddr, SR_ZERO, MAX_SR_TEXT_SIZE);
        MEMSET (au1PrintDestMask, SR_ZERO, MAX_SR_TEXT_SIZE);
        MEMSET (au1PrintMandRtrId, SR_ZERO, MAX_SR_TEXT_SIZE);

        if ((i4FsSrTeDestAddrType == SR_IPV4_ADDR_TYPE) &&
            (u4AddrType == SR_IPV4_ADDR_TYPE))
        {
            cmpDestAddr.Addr.u4Addr = SR_ONE;

            SR_OCTETSTRING_TO_INTEGER (pSnmpDestAddr, destAddr.Addr.u4Addr);
            SR_OCTETSTRING_TO_INTEGER (pSnmpMandatoryRtrId,
                                       mandRtrId.Addr.u4Addr);
            destAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
            mandRtrId.u2AddrType = SR_IPV4_ADDR_TYPE;

            u4TempDestAddr = destAddr.Addr.u4Addr;
            u4TempDestAddr = OSIX_HTONL (u4TempDestAddr);
            pTemp = (UINT1 *) &u4TempDestAddr;
            SPRINTF ((char *__restrict__) destAddress, "%d.%d.%d.%d", pTemp[0],
                     pTemp[1], pTemp[2], pTemp[3]);

            u4TempDestMask = u4DestMask;
            u4TempDestMask = OSIX_HTONL (u4TempDestMask);
            pTemp = (UINT1 *) &u4TempDestMask;
            SPRINTF ((char *__restrict__) destMask, "%d.%d.%d.%d", pTemp[0],
                     pTemp[1], pTemp[2], pTemp[3]);

            u4TempRtrAddr = mandRtrId.Addr.u4Addr;
            u4TempRtrAddr = OSIX_HTONL (u4TempRtrAddr);
            pTemp = (UINT1 *) &u4TempRtrAddr;
            SPRINTF ((char *__restrict__) mandRouterId, "%d.%d.%d.%d", pTemp[0],
                     pTemp[1], pTemp[2], pTemp[3]);

            CliPrintf (CliHandle,
                       "segment-routing te destination %s %s path %s",
                       destAddress, destMask, mandRouterId);
#ifdef CLI_WANTED
            FilePrintf (CliHandle,
                        "segment-routing te destination %s %s path %s",
                        destAddress, destMask, mandRouterId);
#endif
        }
        else if ((i4FsSrTeDestAddrType == SR_IPV6_ADDR_TYPE) &&
                 (u4AddrType == SR_IPV6_ADDR_TYPE))
        {
            MEMSET (cmpDestAddr.Addr.Ip6Addr.u1_addr, SR_ONE,
                    SR_IPV6_ADDR_LENGTH);

            MEMCPY (destAddr.Addr.Ip6Addr.u1_addr,
                    pSnmpDestAddr->pu1_OctetList, pSnmpDestAddr->i4_Length);

            MEMCPY (mandRtrId.Addr.Ip6Addr.u1_addr,
                    pSnmpMandatoryRtrId->pu1_OctetList,
                    pSnmpMandatoryRtrId->i4_Length);

            destAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
            mandRtrId.u2AddrType = SR_IPV6_ADDR_TYPE;

            SPRINTF ((char *__restrict__) au1PrintDestAddr, "%s",
                     Ip6PrintAddr (&destAddr.Addr.Ip6Addr));

            SPRINTF ((char *__restrict__) au1PrintMandRtrId, "%s",
                     Ip6PrintAddr (&mandRtrId.Addr.Ip6Addr));
            CliPrintf (CliHandle,
                       "segment-routing te destination %s %d path %s",
                       au1PrintDestAddr, u4DestMask, au1PrintMandRtrId);
#ifdef CLI_WANTED
            FilePrintf (CliHandle,
                        "segment-routing te destination %s %s path %s",
                        au1PrintDestAddr, au1PrintDestMask, au1PrintMandRtrId);
#endif
        }

        SrCliShowOptionalRouters (CliHandle, &destAddr, u4DestMask, &mandRtrId);

        u4PrevDestMask = u4DestMask;
        if ((i4FsSrTeDestAddrType == SR_IPV4_ADDR_TYPE) &&
            (u4AddrType == SR_IPV4_ADDR_TYPE))
        {
            SR_INTEGER_TO_OCTETSTRING (destAddr.Addr.u4Addr, pSnmpPrevDestAddr);
            SR_INTEGER_TO_OCTETSTRING (mandRtrId.Addr.u4Addr,
                                       pSnmpPrevMandatoryRtrId);
        }
        else if ((i4FsSrTeDestAddrType == SR_IPV6_ADDR_TYPE) &&
                 (u4AddrType == SR_IPV6_ADDR_TYPE))
        {
            MEMCPY (pSnmpDestAddr->pu1_OctetList, destAddr.Addr.Ip6Addr.u1_addr,
                    SR_IPV6_ADDR_LENGTH);

            MEMCPY (pSnmpMandatoryRtrId->pu1_OctetList,
                    mandRtrId.Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);

            pSnmpDestAddr->i4_Length = SR_IPV6_ADDR_LENGTH;
            pSnmpMandatoryRtrId->i4_Length = SR_IPV6_ADDR_LENGTH;
        }
    }
    while (SR_ONE);

    return SR_SUCCESS;
}

/******************************************************************************/
/* Function Name     : SrCliShowOptionalRouters                               */
/* Description       : This function displays the TE path table info          */
/* Input Parameters  : CliHandle = Cli Handle                                 */
/*                     pDestAddr - Pointer to Destination address             */
/*                     u4DestMask - Destination mask                          */
/*                     pMandRtrId - Pointer to Mandatory router id            */
/* Output Parameters : None                                                   */
/* Return Value      : None                                                   */
/******************************************************************************/
PUBLIC VOID
SrCliShowOptionalRouters (tCliHandle CliHandle, tGenU4Addr * pDestAddr,
                          UINT4 u4DestMask, tGenU4Addr * pMandRtrId)
{
    tGenU4Addr          rtrId;
    tSrTeRtEntryInfo    SrTeRtInfo;
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;
    tTMO_SLL_NODE      *pSrOptRtrInfo = NULL;
    tSrRtrEntry        *pSrPeerRtrInfo = NULL;
    UINT1               tmpIpAddr[SR_IPV6_ADDR_LENGTH];

    UINT1               aRtrId[MAX_SR_TEXT_SIZE] = { SR_ZERO };

    MEMSET (&rtrId, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (tmpIpAddr, SR_ZERO, sizeof (tIPADDR));
    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));

    if (pDestAddr->u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        SrTeRtInfo.destAddr.Addr.u4Addr = pDestAddr->Addr.u4Addr;
        SrTeRtInfo.u4DestMask = u4DestMask;
        SrTeRtInfo.mandRtrId.Addr.u4Addr = pMandRtrId->Addr.u4Addr;
        SrTeRtInfo.destAddr.u2AddrType = pDestAddr->u2AddrType;
        SrTeRtInfo.mandRtrId.u2AddrType = pMandRtrId->u2AddrType;
    }
    else if (pDestAddr->u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                pDestAddr->Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);

        MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                pMandRtrId->Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);
        SrTeRtInfo.destAddr.u2AddrType = pDestAddr->u2AddrType;
        SrTeRtInfo.mandRtrId.u2AddrType = pMandRtrId->u2AddrType;
        SrTeRtInfo.u4DestMask = u4DestMask;
    }

    pSrTeRtInfo = RBTreeGet (gSrGlobalInfo.pSrTeRouteRbTree, &SrTeRtInfo);

    if (pSrTeRtInfo != NULL)
    {
        TMO_SLL_Scan (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                      pSrOptRtrInfo, tTMO_SLL_NODE *)
        {
            pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;
            if (pDestAddr->u2AddrType == SR_IPV4_ADDR_TYPE)
            {
                rtrId.Addr.u4Addr =
                    OSIX_HTONL (pSrPeerRtrInfo->routerId.Addr.u4Addr);
                MEMCPY (tmpIpAddr, &rtrId.Addr.u4Addr, sizeof (tIPADDR));

                SPRINTF ((char *__restrict__) aRtrId, "%d.%d.%d.%d",
                         tmpIpAddr[0], tmpIpAddr[1], tmpIpAddr[2],
                         tmpIpAddr[3]);
            }
            else if (pDestAddr->u2AddrType == SR_IPV6_ADDR_TYPE)
            {
                SPRINTF ((char *__restrict__) aRtrId, "%s",
                         Ip6PrintAddr (&pSrPeerRtrInfo->routerId.Addr.Ip6Addr));
            }
            CliPrintf (CliHandle, " %s", aRtrId);
#ifdef CLI_WANTED
            FilePrintf (CliHandle, " %s", aRtrId);
#endif
        }
    }
    CliPrintf (CliHandle, "\n");
#ifdef CLI_WANTED
    FilePrintf (CliHandle, "\n");
#endif
}

/******************************************************************************/
/* Function Name     : SrCliGetSrV6AlternateStatus                            */
/* Description       : This function returns the segment-routing alternate    */
/*                     status for Ipv6                                        */
/* Input Parameters  : None                                                   */
/* Output Parameters : pi4SrAltStatus - Ipv4 Atlernate status                 */
/* Return Value      : None                                                   */
/******************************************************************************/
VOID
SrCliGetSrV6AlternateStatus (INT4 *pi4SrAltStatus)
{
    nmhGetFsSrV6AlternateStatus (pi4SrAltStatus);
}

/******************************************************************************/
/* Function Name     : SrCliDisableSrMpls                                     */
/* Description       : This function disables MPLS on SR                      */
/* Input Parameters  : None                                                   */
/* Return Value      : None                                                   */
/******************************************************************************/
INT4
SrCliDisableSrMpls (VOID)
{
    INT4                i4RetStatus = SR_SUCCESS;
    INT4                i4SrMode = SR_ZERO;
    INT4                i4RetVal = SR_FAILURE;

    nmhGetFsSrMode (&i4SrMode);
    if (i4SrMode != SR_ZERO)
    {
        gu4SegmentMpls = SR_ONE;
        CLI_SET_ERR (CLI_SR_DIS_FAILED);
        i4RetStatus = CLI_SUCCESS;
        return i4RetStatus;
    }
    /* Delete the SID and SRGB details of the self node */
    if (SrDeleteSidTable (SR_IPV4_ADDR_TYPE) != SR_SUCCESS)
    {
        return i4RetStatus;
    }
    else if (SrDeleteSidTable (SR_IPV6_ADDR_TYPE) != SR_SUCCESS)
    {
        return i4RetStatus;
    }
    else
    {
        /* Reset the SRGB values for Ipv4 */
        gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex = SR_ZERO;
        gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMaxIndex = SR_ZERO;
        IS_V4_SRGB_CONFIGURED = SR_FALSE;
        /* Reset the SRGB values for Ipv6 */
        gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex = SR_ZERO;
        gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MaxIndex = SR_ZERO;
        IS_V6_SRGB_CONFIGURED = SR_FALSE;
    }
    /* Disable SR Admin Status for V4 and V6 */
    i4RetVal = nmhSetFsSrV4Status (SR_DISABLED);
    if (i4RetVal != SNMP_SUCCESS)
    {
        i4RetStatus = CLI_FAILURE;
    }

    i4RetVal = nmhSetFsSrV6Status (SR_DISABLED);
    if (i4RetVal != SNMP_SUCCESS)
    {
        i4RetStatus = CLI_FAILURE;
    }

    i4RetVal = SrCliConfigMode (SR_GBL_ADM_DOWN);
    if (i4RetVal != SNMP_SUCCESS)
    {
        i4RetStatus = CLI_FAILURE;
    }
    return i4RetStatus;
}

/*****************************************************************************
 * Function Name : SrCliShowOspfSrCapability
 * Description   : This routine displays the Segment-Routing capability
 *                 information.
 * Input(s)      : CliHandle - Index of current CLI context
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
INT4
SrCliShowOspfSrCapability (tCliHandle CliHandle)
{
    tTMO_SLL_NODE      *pRtrNode;
    tSrRtrInfo         *pRtrInfo = NULL;
    UINT1               ipAddr[MAX_SR_TEXT_SIZE] = { SR_ZERO };
    UINT1               advRtrId[MAX_SR_TEXT_SIZE] = { SR_ZERO };
    UINT1               areaId[MAX_SR_TEXT_SIZE] = { SR_ZERO };
    UINT1              *pTemp = NULL;
    UINT4               u4TempAddr = SR_ZERO;

    if (((SR_MODE & SR_CONF_OSPF) == SR_CONF_OSPF) ||
        ((SR_MODE & SR_CONF_OSPFV3) == SR_CONF_OSPFV3))
    {
        CliPrintf (CliHandle, "\r%-26s\r\n",
                           "Tag ospf Segment-Routing:");
        CliPrintf (CliHandle, "%34s\r\n",
                   "----------------------------------");
        TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
        {
            MEMSET (advRtrId, SR_ZERO, (sizeof (UINT1) * MAX_SR_TEXT_SIZE));
            MEMSET (ipAddr, SR_ZERO, (sizeof (UINT1) * MAX_SR_TEXT_SIZE));
            MEMSET (areaId, SR_ZERO, (sizeof (UINT1) * MAX_SR_TEXT_SIZE));

            pRtrInfo = (tSrRtrInfo *) pRtrNode;
            u4TempAddr = pRtrInfo->u4areaId;
            u4TempAddr = OSIX_HTONL (u4TempAddr);
            pTemp = (UINT1 *) &u4TempAddr;

            if (pRtrInfo->prefixId.u2AddrType == SR_IPV4_ADDR_TYPE)
            {
                u4TempAddr = pRtrInfo->prefixId.Addr.u4Addr;
                u4TempAddr = OSIX_HTONL (u4TempAddr);
                pTemp = (UINT1 *) &u4TempAddr;
                SPRINTF ((char *__restrict__) ipAddr, "%d.%d.%d.%d", pTemp[0],
                         pTemp[1], pTemp[2], pTemp[3]);
                CliPrintf (CliHandle, "\r%-16s%-18s\r\n",
                           "Advertisement Router Capability :", ipAddr);
            }
            else if (pRtrInfo->prefixId.u2AddrType == SR_IPV6_ADDR_TYPE)
            {
                SPRINTF ((char *__restrict__) ipAddr, "%s",
                         Ip6PrintAddr (&pRtrInfo->prefixId.Addr.Ip6Addr));

                CliPrintf (CliHandle, "\r%-16s%-18s\r\n",
                           "Advertisement Router Capability :", ipAddr);
            }
            CliPrintf (CliHandle, "Algorithm :%d \r\n",
                           pRtrInfo->u1Algo);
            CliPrintf (CliHandle, "SID       :%d \r\n",
                       pRtrInfo->u4SidValue);
            if (pRtrInfo->prefixId.u2AddrType == SR_IPV4_ADDR_TYPE)
            {
                CliPrintf (CliHandle, "Total SID'S Supported :%d\r\n",
                               pRtrInfo->srgbRange.u4SrGbMaxIndex -
                               pRtrInfo->srgbRange.u4SrGbMinIndex + 1);
            }
            else if (pRtrInfo->prefixId.u2AddrType == SR_IPV6_ADDR_TYPE)
            {
                CliPrintf (CliHandle, "Total SID'S Supported :%d\r\n",
                               pRtrInfo->srgbRange.u4SrGbV3MaxIndex -
                               pRtrInfo->srgbRange.u4SrGbV3MinIndex + 1);
            }
            CliPrintf (CliHandle, "%-12s%-12d\r\n",
                       "SID Range List Count :", 1);
            if (pRtrInfo->prefixId.u2AddrType == SR_IPV4_ADDR_TYPE)
            {
                CliPrintf (CliHandle, "SID's Range :%d - %d\r\n",
                               pRtrInfo->srgbRange.u4SrGbMinIndex,
                               pRtrInfo->srgbRange.u4SrGbMaxIndex);
            }
            else if (pRtrInfo->prefixId.u2AddrType == SR_IPV6_ADDR_TYPE)
            {
                CliPrintf (CliHandle, "SID's Range :%d - %d\r\n",
                               pRtrInfo->srgbRange.u4SrGbV3MinIndex,
                               pRtrInfo->srgbRange.u4SrGbV3MaxIndex);
            }
            CliPrintf (CliHandle, "%34s\r\n",
                       "----------------------------------");
        }
    }
    return SR_SUCCESS;
}
/****************************************************************************
 * Function     : SrSetDebugLevel
 *
 * Description  : This function configures the SR global debug flag or the
 *                 context based SR debug flag.
 *
 * Input        : u4TrcFlag - The context Id where the debug flag is
 *                               configured / Invalid context Id - to
 *                               configure global debug flag
 *                u4TrcValue  - The configured trace value.
 *
 * Output       : None
 *
 * Returns      : CLI_SUCCESS/CLI_FAILURE
 *
 ***************************************************************************/
 UINT4
SrSetDebugLevel (tCliHandle CliHandle, UINT4 u4TrcVal, UINT4 u4TrcFlag)
{
    UINT4     u4ErrCode = 0;
    UINT4     u4TraceValue = 0;

    if (nmhGetFsSrDebugFlag (&u4TraceValue) == SNMP_FAILURE)
    {
        return CLI_FAILURE;
    }

    if (u4TrcFlag == CLI_ENABLE)
    {
        u4TraceValue |= u4TrcVal;
    }
    else if (u4TrcFlag == CLI_DISABLE)
    {
        if(u4TrcVal != 0)
        {
            u4TraceValue &= (~(u4TrcVal));
        }
            /*Ensure critical trace is always enabled*/
            u4TraceValue |= SR_CRITICAL_TRC;
    }
    if (nmhTestv2FsSrDebugFlag (&u4ErrCode, u4TraceValue)
            == SNMP_FAILURE)
    {
        return CLI_FAILURE;
    }
    if (nmhSetFsSrDebugFlag (u4TraceValue)
            == SNMP_FAILURE)
    {
        CLI_FATAL_ERROR (CliHandle);
        return CLI_FAILURE;
    }
    return CLI_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrAddOrDelILMSidTable
 * Description   : This routine givens indication to add or del the ILMs in 
 *                 SID table when SR made shut
 * Input(s)      : u1AddrType - Address Type (IPv4/IPv6)
 *                 u2IlmFlag - ILM create or Delete flag
 * Output(s)     : None
 * Return(s)     : None
 *****************************************************************************/
VOID
SrAddOrDelILMSidTable (UINT1 u1AddrType, UINT2 u2IlmFlag)
{
    tSrSidInterfaceInfo *pSrSidEntry = NULL;
    tSrSidInterfaceInfo srSidInfo;

    MEMSET (&srSidInfo, SR_ZERO, sizeof (tSrSidInterfaceInfo));

    pSrSidEntry = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);

    while (pSrSidEntry != NULL)
    {
        if (pSrSidEntry->ipAddrType == SR_IPV4_ADDR_TYPE)
        {
            MEMCPY (&(srSidInfo.ifIpAddr.Addr.u4Addr),
                    &(pSrSidEntry->ifIpAddr.Addr.u4Addr), SR_IPV4_ADDR_LENGTH);
            srSidInfo.ifIpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
        }
        else if (pSrSidEntry->ipAddrType == SR_IPV6_ADDR_TYPE)
        {
            MEMCPY (srSidInfo.ifIpAddr.Addr.Ip6Addr.u1_addr,
                    pSrSidEntry->ifIpAddr.Addr.Ip6Addr.u1_addr,
                    SR_IPV6_ADDR_LENGTH);
            srSidInfo.ifIpAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
        }
        srSidInfo.ipAddrType = pSrSidEntry->ipAddrType;

        if (pSrSidEntry->ipAddrType != u1AddrType)
        {
            pSrSidEntry = RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree,
                    &srSidInfo, NULL);
            continue;
        }
        SrAddorDelILM (pSrSidEntry, u2IlmFlag);
        pSrSidEntry = RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree,
                &srSidInfo, NULL);
    }
    return;
}

/*****************************************************************************
 * Function Name : SrAddorDelILM
 * Description   : This routine add or del the ILMs when SR made shut
 * Input(s)      : u1AddrType - Address Type (IPv4/IPv6)
 *                 u2IlmFlag - ILM create or Delete flag
 * Output(s)     : None
 * Return(s)     : None
 *****************************************************************************/
VOID
SrAddorDelILM (tSrSidInterfaceInfo *pSrSidEntry, UINT2 u2IlmFlag)
{
    UINT4               u4Label = SR_ZERO;
    tGenU4Addr          nextHop;
    tGenU4Addr          destPrefix;

    MEMSET (&nextHop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&destPrefix, SR_ZERO, sizeof (tGenU4Addr));
    if (pSrSidEntry == NULL)
    {
        SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "%% Failure: SID Entry pointer is NULL\r\n");
        return SNMP_FAILURE;
    }
    else
    {
        if (u2IlmFlag == MPLS_MLIB_ILM_CREATE)
        {
            gSrGlobalInfo.u4CliCmdMode = SR_TRUE;
        }
        nextHop.u2AddrType = pSrSidEntry->ifIpAddr.u2AddrType;
        destPrefix.u2AddrType = pSrSidEntry->ifIpAddr.u2AddrType;

        if ((pSrSidEntry->u4SidType == SR_SID_NODE) ||
                (pSrSidEntry->u4SidTypeV3 == SR_SID_NODE))
        {
            SR_GET_SELF_SID_LABEL (&u4Label, pSrSidEntry);
            if (SrUtilCreateorDeleteILM (pSrSidEntry, u4Label,
                        &destPrefix, &nextHop,
                        u2IlmFlag, SR_ZERO) == SR_FAILURE)
            {
                SR_TRC (SR_CRITICAL_TRC | SR_FAIL_TRC,
                        "Error in deleting corresponding MPLS ILM \n");
                return SNMP_FAILURE;
            }
        }
        else if ((u2IlmFlag == MPLS_MLIB_ILM_DELETE)
                && (pSrSidEntry->u1AdjSidInfoFlags & SR_ADJ_SID_ILM_CREATE))
        {
            if (pSrSidEntry->ifIpAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
            {
                nextHop.Addr.u4Addr = pSrSidEntry->ifIpAddr.Addr.u4Addr;
            }
            else if (pSrSidEntry->ifIpAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
            {
                MEMCPY (nextHop.Addr.Ip6Addr.u1_addr,
                        pSrSidEntry->ifIpAddr.Addr.Ip6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH);
            }

            if (SrUtilCreateorDeleteILM (pSrSidEntry,
                        SR_P_INTF_SID_INDEX (pSrSidEntry),
                        &destPrefix, &nextHop, u2IlmFlag,
                        pSrSidEntry->u4IfIndex) == SR_FAILURE)
            {
                SR_TRC (SR_CRITICAL_TRC | SR_FAIL_TRC,
                        "Error in deleting corresponding MPLS ILM \n");
                return SNMP_FAILURE;
            }
        }
        if (u2IlmFlag == MPLS_MLIB_ILM_CREATE)
        {
            gSrGlobalInfo.u4CliCmdMode = SR_FALSE;
        }
    }
    return;
}
#endif
