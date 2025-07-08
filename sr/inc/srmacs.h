/* Copyright (C) 2017 Aricent Inc . All Rights Reserved
*******************************************************************************
**    FILE  NAME             : srmacs.h
**    PRINCIPAL AUTHOR       : Aricent Inc.
**    SUBSYSTEM NAME         : SR
**    MODULE NAME            : SR
**    LANGUAGE               : ANSI-C
**    TARGET ENVIRONMENT     : Linux  (Portable)
**    DATE OF FIRST RELEASE  :
**    DESCRIPTION            : Contains macros used for SR Support.
**---------------------------------------------------------------------------*/

#ifndef _SR_MACS_H
#define _SR_MACS_H

#define SR_GBL_INFO                		gSrGlobalInfo

#define SR_MOD_STATUS               	gSrGblInfo.u4SrModuleStatus
#define SR_GBL_SEM_ID  	                gSrGblInfo.gSrSemId
#define SR_GBL_QID                    	gSrGblInfo.gSrQId

#ifdef LNXIP4_WANTED
#define SR_GBL_IP_QID                 	gSrGblInfo.gSrIpQId
#endif

#define SR_GBL_TE_DBG_FLAG            	SR_GBL_INFO->u4SrDbgFlag
#define SR_GBL_TE_DBG_LVL            	SR_GBL_INFO->u4SrDbgLvl
#define SR_GBL_SOCK_FD                	SR_GBL_INFO->i4SrSockFd
#define SR_GBL_UDP_READ_SOCK_FD       	SR_GBL_INFO->i4SrUdpReadSockFd
#define SR_GBL_UDP_WRITE_SOCK_FD      	SR_GBL_INFO->i4SrUdpWriteSockFd

#define SR_GBL_IF_HSH_TBL             	SR_GBL_INFO->pIfHashTable
#define SR_GBL_SENDER_TBL             	SR_GBL_INFO->pSenderTable
#define SR_GBL_TIMER_LIST             	SR_GBL_INFO->SrTimerList

/***************************************************************************************************/

#define SR_LOCK							SrMainTaskLock
#define SR_UNLOCK						SrMainTaskUnLock


#define SR_P_INTF_IP_ADDR_MASK(x)		x->ifIpAddrMask
#define SR_P_INTF_IFINDEX(x)			x->u4IfIndex
#define SR_P_INTF_SID_INDEX(x)			x->u4PrefixSidLabelIndex
#define SR_P_ROW_STATUS(x)				x->u1RowStatus
#define SR_P_ADMIN_STATUS(x)			x->u1AdminStatus
#define SR_P_OPER_STATUS(x)				x->u1OperStatus
#define SR_P_INTF_SID_TYPE(x)			x->u4SidType
#define SR_P_INTF_SID_INDEX_TYPE(x)		x->u1SidIndexType
#define IS_V4_SRGB_CONFIGURED			gSrGlobalInfo.SrContext.SrGbRange.isSrgbConfigured
#define IS_V6_SRGB_CONFIGURED			gSrGlobalInfo.SrContextV3.SrGbRange.isSrgbV3Configured
#define SR_INITIALISED                         (gSrGlobalInfo.u1SrInitialised)


/*Dynamic-SR*/
#define SR_MODE							SR_GBL_INFO.u4SrMode	
#define SR_P_CXT_RTR_ID(x)				x->u4SrOspfCxtId[0]
#define OSPFSR_CXT     					gSrGlobalInfo.SrContext

#define SR_V4_SRGB_MIN_VAL					gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex
#define SR_V4_SRGB_MAX_VAL					gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMaxIndex

#define SR_V6_SRGB_MIN_VAL             gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex
#define SR_V6_SRGB_MAX_VAL             gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MaxIndex

#define SR_IPV6_NODE_SID				gSrGlobalInfo.u4Ipv6NodeSid

#define SR_RI_LSA_SENT                 SR_GBL_INFO.u1StartupRILSASent
#define SR_EXT_PRF_LSA_SENT            SR_GBL_INFO.u1StartupExtPrfLSASent
#define SR_EXT_LINK_LSA_SENT           SR_GBL_INFO.u1StartupExtLinkLSASent
#define SR_ADJ_SID                     SR_GBL_INFO.u1AdjSidConfigured

#endif
