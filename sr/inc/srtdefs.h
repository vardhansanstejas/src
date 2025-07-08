/******************************************************************
 * Copyright (C) 2006 Aricent Inc . All Rights Reserved
 *
 * $Id$
 *
 * Description:This file contains type definitions relating$
 *             to protocol.
 *
 *******************************************************************/
/*****************************************************************************/
/* Copyright (C) 2017 Aricent Inc . All Rights Reserved
 * Copyright (C) 2017 Aricent Inc . All Rights Reserved
 ******************************************************************************
 *    FILE  NAME             : srtdf.h
 *    PRINCIPAL AUTHOR       : Aricent Inc. 
 *    SUBSYSTEM NAME         : SR
 *    MODULE NAME            : SR
 *    LANGUAGE               : ANSI-C
 *    TARGET ENVIRONMENT     : Linux (Portable)                         
 *    DATE OF FIRST RELEASE  :
 *    DESCRIPTION            : This file contains the structure type
 *                             definitions declared and used for SR module 
 *---------------------------------------------------------------------------*/
#ifndef _SR_TDEFS_H_
#define _SR_TDEFS_H_
#include "ospf.h"
#include "ipv6.h"
#if 0
/*Dynamic-SR*/
#define MAX_IPV4_ADDR_LEN 4

typedef UINT1 tIPADDR[MAX_IPV4_ADDR_LEN]
#endif

/*Major Events*/
#define SR_IP_EVENT                 0x00000001
#define SR_OSPF_UP_EVENT            0x00000002
#define SR_IP6_EVENT                0x00000004
#define SR_NETIP6_EVENT             0x00000005
#define SR_CFA_EVENT                0x00000006
#define SR_IP_LFA_EVENT             0x00000007
#define SR_OSPFV3_UP_EVENT          0x00000008
#define SR_OSPF_STATE_EVENT         0x00000009
#define SR_OSPF_ALT_STATE_EVENT     0x00000010
#define SR_OSPFV3_STATE_EVENT       0x00000011
#define SR_OSPFV3_ALT_STATE_EVENT   0x00000012
#define SR_OSPF_TE_ADD_EVENT        0x00000013
#define SR_OSPF_TE_DEL_EVENT        0x00000014 
#define SR_OSPF_MSG_EVENT           0x00000015
#define SR_RM_MSG                   0x00000016  /* GR Support */
#define SR_OSPF_RTR_DISABLE_EVENT   0x00000017

/* IP Events */
#define SR_MAX_IP_EVENTS            5
#define SR_RT_NEW                   0x00000100
#define SR_RT_NH_CHG                0x00000200
#define SR_RT_IFACE_CHG             0x00000400
#define SR_RT_METRIC_CHG            0x00000800
#define SR_RT_DELETED               0x00001000
#define SR_RT_FRR_NH_CHG            0x00002000


#define   LFA_ROUTE_ADD		0x00000001
#define   LFA_ROUTE_DELETE	0x00000002
#define   LFA_ROUTE_MODIFY	0x00000003
#define   LFA_ROUTE_SET		0x00000004                     
#define   LFA_ROUTE_RESET	0x00000005

#define   RLFA_ROUTE_ADD     0x00000001
#define   RLFA_ROUTE_DELETE  0x00000002
#define   RLFA_ROUTE_MODIFY  0x00000003
#define   RLFA_ROUTE_SET     0x00000004
#define   RLFA_ROUTE_RESET   0x00000005

#define   TILFA_ROUTE_ADD     0x00000001
#define   TILFA_ROUTE_DELETE  0x00000002
#define   TILFA_ROUTE_MODIFY  0x00000003
#define   TILFA_ROUTE_SET     0x00000004
#define   TILFA_ROUTE_RESET   0x00000005


#define   IPv6_LFA_ROUTE_ADD         0x00000001
#define   IPv6_LFA_ROUTE_DELETE      0x00000002
#define   IPv6_LFA_ROUTE_SET         0x00000004
#define   IPv6_LFA_ROUTE_RESET       0x00000005
#define   IPv6_LFA_ROUTE_MODIFY      0x00000006

/*Rlfa event handler*/
#define   IPv6_RLFA_ROUTE_ADD         0x00000001
#define   IPv6_RLFA_ROUTE_DELETE      0x00000002
#define   IPv6_RLFA_ROUTE_SET         0x00000004
#define   IPv6_RLFA_ROUTE_RESET       0x00000005
#define   IPv6_RLFA_ROUTE_MODIFY      0x00000006

/*TI-LFA event handler*/
#define   IPv6_TILFA_ROUTE_ADD         0x00000001
#define   IPv6_TILFA_ROUTE_DELETE      0x00000002
#define   IPv6_TILFA_ROUTE_SET         0x00000004
#define   IPv6_TILFA_ROUTE_RESET       0x00000005
#define   IPv6_TILFA_ROUTE_MODIFY      0x00000006

#define SR_FTN_NOT_CREATED          0x00    /*0000 0000*/
#define SR_FTN_CREATED              0x01    /*0000 0001*/
#define SR_ILM_POP_CREATED          0x02    /*0000 0010*/

#define SR_SID_STATUS_DOWN  0x00
#define SR_SID_STATUS_UP    0x01

#define LFA_NODE_NEXTHOP            0x01
#define LFA_NODE_P                  0x02
#define LFA_NODE_Q                  0x04

#define SR_OPR_F_ADD_DELAY_LIST     0x01
#define SRTE_ALARM_CLEAR    0
#define SRTE_ALARM_RAISE    1

#define SRTE_UNKNOWN_SID_ALARM      1
#define SRTE_SID_EXCEEDS_SRGB_ALARM 2

#define SR_TASK_NAME   "SRT"

#define SR_MAX_TIMERS           0

typedef struct _SrCfaInfo
{
   INT4             i4IfIndex;	   /* Interface Index of the port*/
   UINT1            u1IfType;      /* Interface Type*/
   UINT1            u1IfStatus;    /*Interface Status*/
   UINT1            au1Pad[2];
}tSrCfaInfo;

/* IP Event Info */
typedef struct {
    UINT1*  pRouteInfo;
    tGenU4Addr  u4IpAddr;
    UINT4   u4IfIndex;
    UINT4   u4BitMap;
}tSrIpEvtInfo;
typedef struct _SrNetIpv6LfaRtInfo
{
    tIp6Addr            Ip6Dst;     /* The destination address prefix */
    tIp6Addr            NextHop;    /* The next hop address */
    tIp6Addr            LfaNextHop; /* The LFA next hop address */
    tIp6Addr            AltRlfaNxtHop; /*Alternate Next hop of RLFA route */
    UINT4               u4ContextId;/* RTM6 Context ID */
    UINT4               u4Index;    /* Route Interface Index */
    UINT4               u4Metric;   /* Primary Cost */
    UINT4               u4AltCost;  /* Alternate Lfa route cost */
    UINT4               u4AltIfIndex; /* Interface Index of a Alternate route*/
    union
    {
        UINT4           u4RemNodeRouterId; /* Remote Router Id for RLFA or TI-LFA Tunnel, 
                                      applicable when IGP is OSPF */
        UINT1           au1RemNodeSystemId[ISIS_SYS_ID_LEN]; /* Remote System Id for RLFA or 
                                      TI-LFA Tunnel, applicable when IGP is ISIS */
    }uRemoteNodeId;
    union
    {
        UINT4           u4QNodeRouterId; /* Q node router id, applicable if LFA type is
                                        TI-LFA, applicable when IGP is OSPF */
        UINT1           au1QNodeSystemId[ISIS_SYS_ID_LEN]; /* Remote System Id for RLFA or TI-LFA Tunnel,
                                                    applicable when IGP is ISIS */
    }uQNodeId;
    UINT4               u4PaQAdjLocAddr;  /* P&Q adjacency local address, applicable if LFA type is TI-LFA */
    UINT1               u1CmdType;  /* ADD/DELETE/MODIFY LFA route info */
    UINT1               u1Prefixlen;/* The number of significant bits in
                                     * the prefix*/
    INT1                i1Proto;    /* Protocol from which this route is
                                     * learnt. */
    UINT1               u1LfaType;     /* Flag to indicate whether its a LFA or RLFA or TI-LFA route */

}
tSrNetIpv6LfaRtInfo;


typedef struct _SrNetIpLfaRtInfo
{
      UINT4    u4DestNet;     /* Destination Address */
       UINT4    u4DestMask;    /* Net Mask */
       UINT4    u4NextHop;     /* NextHop Address */
       UINT4    u4LfaCost;     /* LFA Route Cost */
       UINT4    u4NextHopIfIndx;  /* Interface Index of Primary Next hop*/
       UINT4    u4AltIfIndx;   /* Interface Index of Alternate LFA */
       UINT4    u4ContextId;   /* Vrf context id */
       UINT4    u4LfaNextHop;  /* LFA NextHop */
       UINT4    u4PrimaryCost;   /* Primary Route Cost */
       union
       {
       UINT4    u4RemNodeRouterId; /* Remote Router Id for RLFA or TI-LFA Tunnel, 
                                      applicable when IGP is OSPF */
       UINT1    au1RemNodeSystemId[ISIS_SYS_ID_LEN]; /* Remote System Id for RLFA or 
                                      TI-LFA Tunnel, applicable when IGP is ISIS */
       }uRemoteNodeId;
       union
       {
       UINT4    u4QNodeRouterId; /* Q node router id, applicable if LFA type is
                                        TI-LFA, applicable when IGP is OSPF */
       UINT1    au1QNodeSystemId[ISIS_SYS_ID_LEN]; /* Remote System Id for RLFA or TI-LFA Tunnel,
                                                    applicable when IGP is ISIS */
       }uQNodeId;
       UINT4    u4PaQAdjLocAddr; /* P&Q adjacency local address, applicable if LFA type is TI-LFA */
       UINT2    u2RtProto;      /* Routing Mechanism via
                                   which the Route is learnt */
       UINT1    u1CmdType;     /* Add / Delete / Modify LFA route */
       UINT1    u1LfaType;     /* Flag to indicate whether
                                   its a LFA or RLFA or TI-LFA */

}
tSrNetIpLfaRtInfo;

/* SR TE paramaters for queue posting */
typedef struct _SrTeOspfRtInfo
{
	tGenU4Addr destAddr; /* TE destination */
	tGenU4Addr mandatoryRtrId; /* TE mandatory node */
	tGenU4Addr au4OptRouterIds[MAX_SR_LABEL_DEPTH]; /* optional routerIds */
    UINT4      u4DestMask;   /* destination mask */
	UINT1      u1NoOfOptRouters; /* No of optional routers configured */
    UINT1      au1pad[3];
}
tSrTeOspfRtInfo;

/** SR RM event Parameters **/
typedef struct _SrRmEvtInfo
{
    tRmMsg        *pBuff;     /* Message given by RM module. */
    UINT2          u2Length;   /* Length of message given by RM module. */
    UINT1          u1Event;    /* Event given by RM module. */
    UINT1          u1Reserved; /* Added for alignment purpose */
}tSrRmEvtInfo;

/* Sr Queue Info */
typedef struct _SrQMsg
{
   UINT4            u4MsgType;
   UINT4            u4OspfCxtId;
   UINT4            u4Status;
   union {
            tSrIpEvtInfo   SrIpEvtInfo;      /* IP Event Info */
            tSrCfaInfo     SrCfaInfo;        /* IF Event Info */
            tSrNetIpv6LfaRtInfo SrNetIpv6LfaRtInfo;
            tSrNetIpLfaRtInfo SrNetIpLfaRtInfo; /*Sr LFA/RLFA/TILFA ipv4 event info */
            tSrTeOspfRtInfo SrTeRtInfo; /*Sr TE event info */
            tOsToOpqApp     OstoSrMsgParams;
            tSrRmEvtInfo    SrRmEvt;
         }u; 
}tSrQMsg;

typedef struct _SrGb {
	UINT4                 u4SrGbMinIndex; /* MIN SRGB Value */
	UINT4                 u4SrGbMaxIndex; /* MAX SRGB value */
	UINT4                 u4SrGbV3MinIndex; /* MIN SRGB V3 Value */
    UINT4                 u4SrGbV3MaxIndex; /* MAX SRGB V3 value */
	UINT4				  isSrgbConfigured;	/*Status of SRGB configuration by user
												0 - Not configured, 1- Configured*/
	UINT4				  isSrgbV3Configured;	/*Status of SRGB configuration by user
												0 - Not configured, 1- Configured*/
    INT4                  i4RowStatus ;	  /* Row Status */
}tSrGb;

/*Structure for Storing Prefix related info for which LSP is to be made*/
typedef struct _FEC
{
    uGenU4Addr          Prefix;                 /* Fec/IP Address */
    UINT4               u4MaskOrPrefixLen;      /* Mask or Prefix Length (/24, /32)*/
    UINT2               u2AddrFmly;             /* SR_IPV4_ADDR_TYPE / SR_IPV6_ADDR_TYPE*/
    UINT1               u1FecElmntType;         /* SR_FEC_PREFIX_TYPE / SR_FEC_ADJACENCY_TYPE*/
    UINT1               au1Pad[1];
}tSrFec;

typedef struct _SrContext
{
        tGenU4Addr              rtrId;              /** Loopback Address with SID is associated **/
        tSrGb                   SrGbRange;         /** SRGB block **/
/*Dynamic-SR*/
        UINT4                   u4areaId;
        UINT2                   u2SrLblGroupId;     /* Label Space Group ID */
        UINT1                   u1AdminStatus;
        UINT1                   u1AsbrStatus;
} tSrContext;


/*Structure for InSeg Information*/
typedef struct _SrInSegInfo {

    tSrFec      Fec;
    UINT4       u4InLabel;
    UINT4       u4InIfIndex;
	UINT4		u4OutIfIndex; 					/* To handle out interface in case of Static SR*/
}tSrInSegInfo;

typedef struct _AdjSidNode
{
	tTMO_SLL_NODE		nextAdjSid;
	tGenU4Addr			linkIpAddr;
	UINT4               u4LinkId;
	UINT4				u4Label;
	UINT4				u4NbrRtrId;
    UINT1               u1Flags;     			/* B/V/L/G/P Flags*/
    UINT1               u1MTID;
    UINT1               u1Weight;
    UINT1               u1LinkType;
}tAdjSidNode;

typedef struct _SrSidInterfaceInfo {
	
    tGenU4Addr         ifIpAddr;  				/* Ip Address Of the Interface */
    UINT4	           ifIpAddrMask; 			/* Subnet mask */
    UINT4              u4IfIndex;    			/* CFA-If Index */
    UINT4              u4PrefixSidLabelIndex;   /* Prefix SID Value : It can be assigned label value or Index */
    UINT4              u4SidType;   			/* Prefix-SID/Adjacency-SID/LAN Adjacency-SID/Binding-SID */
    UINT4              u4SidTypeV3;   			/* Prefix-SID/Adjacency-SID/LAN Adjacency-SID/Binding-SID */
	UINT4			   u4MplsTnlInterface;		/* MPLS Tunnel interface for the corresponding MPLS entry*/
    UINT4              u4AreaId;                /* Area ID of OSPF in which this SID configured */
	UINT1			   u1SidIndexType;
    UINT1              u1RowStatus;				/* Row status of SID */
    UINT1              u1AdminStatus;			/* Admin Status */
    UINT1              u1OperStatus;			/* Oper status */
	UINT1			   ipAddrType;
    UINT1              u1Flags;  				/* NP/M/E/V/L Flags - Node SID*/
								     			/* B/V/L/G/P Flags  - Adj SID*/
    UINT1              u1Algo;
    UINT1              u1SidConflictPopDel;
    UINT1              u1AdjSidInfoFlags;
    UINT1              u1AdjSidOprFlags;
    UINT1              au1Pad[2];
} tSrSidInterfaceInfo;

/*
 *  * -- tSrRtrInfo --
 *           |
 *           |
 *           V
 *       NextHopList (tSrRtrNextHopInfo)
 *           |                          (SLL)
 *           |------>tSrRtrNextHopEntry ------>tSrRtrNextHopEntry--->tSrRtrNextHopEntry
 *
 * Structure for storing LSA-10 Info*/
typedef struct _SrRtrInfo {

	tTMO_SLL_NODE	   nextRtrId;
	tTMO_SLL           adjSidList;
    tTMO_SLL           NextHopList;	
    tGenU4Addr         prefixId;	  			/* Prefix-ID */
    tGenU4Addr         teSwapNextHop;	  		/* Next-Hop Address for TE swap*/
    tGenU4Addr         advRtrId;                /* AdvRtrId */
	tSrGb			   srgbRange;				/* SRGB Range of SR-Router*/
	UINT4              u4areaId;				/* Area-ID of the LSA received*/
    UINT4              u4SidValue; 				/* Prefix SID Value : It can be assigned label value or Index */
    UINT4              u4SidType;   			/* Prefix-SID/Adjacency-SID/LAN Adjacency-SID/Binding-SID */
    UINT4              u4TeSwapOutIfIndex;     	/* Out Interface to reach nextHop for TeSwap*/
    UINT4              u4TeSwapOutLabel;
    UINT4              u4LfaOutIfIndex;         /* LFA Tunnel Index */
    UINT4              u4LfaSwapOutIfIndex;     /* LFA ILM Tunnel Index*/
    UINT4              u4OutLabel;
    UINT4              u4LfaInLabel;           /* LFA Insegment Label */
	UINT4		       u4LfaNextHop; 			/*LFA next hop*/
	UINT4		       u4DestMask; 				/*LFA Dest mask hop*/
    tIp6Addr           Ip6Dst;     				/* The destination address prefix */
    tIp6Addr           LfaNextHop;              /* The LFA next hop address */
	UINT2			   u2AddrType;				/* Ipv4 or Ipv6*/
    UINT1              u1TeSwapStatus;          /* TRUE/FALSE*/ 
    UINT1              u1Flags;  			    /* NP/M/E/V/L Flags*/
    UINT1              u1Algo;
    UINT1              u1MTID;
	UINT1			   u1SidIndexType;
	UINT1			   u1LSASent;
    UINT1              u1LfaType; /* Flag to check whether route is LFA or RLFA or TI-LFA */ 
    BOOL1       bIsLfaActive; /* This flag is to check whether Lfa route is active or not */
    BOOL1        bIsTeconfigured; /* To check whether TE is configured for the prefix */
    UINT1              u1OprFlags;
    UINT4              u4ExtLsaToBeSent;
    UINT1              u1SIDConflictWin;
    UINT1              u1SIDConflictMPLSDel;
    UINT1              u1SIDConflictAlarm;
    UINT1              u1PrefixConflictAlarm;
    UINT1              u1PrefixConflictMPLSDel;
    UINT1              u1OutOfRangeAlarm;
    UINT1              u1RtrType;
    UINT1              u1SIDStatus;
    UINT1              u1RtrSrAttrFlags; /*Flag to check SRGB/PREFIX_SID/ADJ_SID presence*/
    UINT1              au1Pad[3];
} tSrRtrInfo;


typedef struct _SrRtrNextHopInfo
{
    tTMO_SLL_NODE      nextNextHop;             /* Next pointer*/
    tGenU4Addr         nextHop;                 /* Next-Hop Address */
    tGenU4Addr         PrimarynextHop;          /* Next-Hop Address of Primary path*/
    tGenU4Addr         OldnextHop;              /* Previous Next-Hop Address in case of NH modify*/    
    tGenU4Addr         RemoteNodeAddr;          /* Remote Node Address */
    UINT4              nbrRtrId;                /* Nbr RtrId */
    UINT4              u4SwapOutIfIndex;        /* Out Interface to reach nextHop for Swap Outseg*/
    UINT4              u4OutIfIndex;            /* Out Interface to reach nextHop*/
    UINT4              u4OldOutIfIndex;         /* Previous Next-Hop Address Out Interface in case of NH modify*/    
    UINT1              u1MPLSStatus;            /* SR_FTN_CREATED / SR_FTN_NOT_CREATED / SR_ILM_POP_CREATED
                                                   SR_ILM_POP_NOT_CREATED */
    UINT1              u1FRRNextHop;            /* Flag to identify FRR Type (LFA/RLFA) */
    BOOL1	           bIsFRRHwLsp;             /* This flag is to prevent H/W programming of LFA routes during LFA enablement in SR,
                                                 * It should be programmed only when primary interface is admin down */
    BOOL1              bIsLfaNextHop;           /* Flag to identify LFA nexthop or not in tSrRtrNextHopInfo NextHop SLL List*/
    BOOL1              bIsFRRModifyLsp;          /* This flag is allowed to programm the LFA/RLFA LSP*/
    BOOL1              bIsOnlyCPUpdate;          /* This flag is to check whether to update 
                                                    the control plane entries alone */
    UINT1              au1pad[2];
}tSrRtrNextHopInfo;

typedef struct _SrLinkInfoKey {
    UINT4              u4LinkId;       /* OSPF router LSA link id corresponding to this Ext LSA*/
    UINT4              u4LinkData;     /* OSPF router LSA link data  corresponding to this Ext LSA*/
    UINT1              u1LinkType;     /* OSPF router LSA link type  corresponding to this Ext LSA*/
    UINT1              au1pad[3];
} tSrLinkInfoKey;

typedef struct _SrExtLinkLSAInfo {
    tRBNodeEmbd        RbNode;
    tSrLinkInfoKey     LinkInfo;
    tTMO_SLL           adjSidSubTlvList;  /*Adj-SID list*/
    UINT4              u4LsaId;	          /* LSA ID of the Ext Link LSA if generated for the link */
	UINT1              ulFlags;           /* stale flag, originate flag and other flags */
	UINT1              au1pad[3];
} tSrExtLinkLSAInfo;

typedef struct _SrareaLinkInfo {
	tTMO_SLL_NODE	   nextAreanode;
	UINT4              u4AreaId;
	struct rbtree      *pExtLinkLsaInfoTree;
} tareaLinkInfo;

typedef struct _SrRMInfo
{
		UINT4  u4SrRmState;
		UINT4  u4PeerCount;
		UINT4  u4DynBulkUpdatStatus;
		UINT4  u4SrRmGoActiveReason;
		UINT1  u1BulkUpdModuleStatus;
		BOOL1  b1IsBulkReqRcvd;
		UINT1  u1AdminState;
		UINT1  u1Pad;
}tSrRMInfo;

typedef struct _SrGlobalInfo
{
    tOsixTaskId         gSrTaskId;
    tOsixSemId          gSrSemId;
    tOsixQId            gSrQId;      
#ifdef LNXIP4_WANTED
    tOsixQId            gSrIpQId;      
#endif
    struct rbtree      *pSrSidRbTree;         /* Pointer to SID RBTree */
    struct rbtree      *pSrRlfaPathRbTree;      /* Pointer to SR RLFA RBTree */
    struct rbtree      *pSrTilfaPathRbTree;      /* Pointer to SR RLFA RBTree */
    struct rbtree      *pSrLfaNodeRbTree;      /* Pointer to LFA Nexthop/P/Q node tree  */
    struct rbtree      *pSrLfaAdjRbTree;      /* Pointer to LFA Adj tree  */
    struct rbtree      *pSrTeRouteRbTree;     /* Pointer to SR TE RBTree */
    struct rbtree      *pSrStaticRbTree;     /* Pointer to Static Sr Info RBtree */
    struct rbtree      *pSrOsRiLsRbTree;     /* Pointer to Opaque LSA's RI RBTree */
    tSrContext          SrContext;
    tSrContext		SrContextV3;          /* SR Context for OSPFv3 */
    tSrSidInterfaceInfo *pV6SrSelfNodeInfo;       /* Self Node Info */
/*Dynamic-SR*/
    tTMO_SLL		routerList;             /* List of LSA-10 received */
    tTMO_SLL            linkInfoList;        /* Link information to generate Extended Link LSAs */
    UINT4               u4SrMode;               /* Indiactes the modes on which SR is enabled
                                                 * eg : OSPFv2 or OSPFv3 */
    UINT4               u4GblSrAdminStatus;     /* Indicates if SR is enabled globally */ 
    UINT4               u4Ipv4AddrFamily;       /* Indicates if SR is configured for IPv4 */
    UINT4               u4Ipv6AddrFamily;       /* Indicates if SR is configured for IPv6 */
/*Dynamic-SR End*/
    UINT4		u4rtrId;
    UINT4		u4GenLblSpaceMinLbl; 
    UINT4		u4GenLblSpaceMaxLbl;
    UINT4		isLabelSpaceRsvd;
    UINT4               u4SrDumpLvl;	
    UINT4               u4SrDumpDirn;
    UINT4               u4SrDumpType;
    UINT4               u4SrTrcFlag;
    UINT4               u4SrDbgLvl; 
    UINT4               u4SrV4ModuleStatus;  /* Indicates the Admin status of SR IPv4 */
    UINT4               u4SrV6ModuleStatus;  /* Indicates the Admin Status of SR IPv6 */
    UINT4               u4StartupLSASent;
    UINT4               u4Ipv6NodeSid;	
    UINT4               u4CliCmdMode;	
    UINT4               u4OspfRtrID; /* Router-Id for OSPF v3 */
    UINT4               u4Ospfv2RtrID; /* Router-Id for OSPF v2 */
    UINT1               u1SrOspfDisable;	
    UINT1               u1SrOspfV3Disable;	
    UINT1               u1SrOspfRegState[SYS_DEF_MAX_NUM_CONTEXTS];	
    UINT1               u1SrOspfV3RegState[SYS_DEF_MAX_NUM_CONTEXTS];
    UINT1               u1Ospf3AbrStatus; /* Flag to  indicate Abr status
                                              of OSPF3 */
    UINT1               u1OspfAbrStatus; /* Flag to indicate Ospfv2 Abr Status */ 
    UINT1               u1SrInitialised;
    UINT1               u1OspfDeRegInProgress; /* Flag to indicate if OSPF 
                                                De-registration is in progress */
    UINT1               u1Ospfv3DeRegInProgress; /* Flag to indicate if OSPFv3 
                                                    De-registration is in progress */
    UINT1               u1StartupExtPrfLSASent;  /* Indicates Extended prefix LSA sent*/
    UINT1               u1StartupRILSASent;      /* Indicates Router Info LSA sent*/
    UINT1               u1StartupExtLinkLSASent; /* Indicates Extended Link LSA sent*/
    UINT1               u1AdjSidConfigured;      /* Indicates Adjacency SId configured */
    UINT1               u1SrRegStatus;
    UINT1               u1RebootMode; /* On receiving warm reboot mode message from RM,
                                         this flag will be set */
    UINT1               u1IsGrStart;
    tSrRMInfo           srRmInfo;
    UINT1               u1IsSystemReady;
    UINT1               u1IsBlocked;
    BOOL1               bIsMicroLoopAvd;
    BOOL1               bIsDelayTimerStarted;
    tTMO_SLL		    rtDelayList;
    tTimerListId        srTmrListId;
    tTmrAppTimer        delayTimerNode;
    tRBTree             SrFTNTnlDbTree;
    tRBTree             SrILMTnlDbTree;

}tSrGlobalInfo;


/** OSPF-SR LSA Handling **/
typedef struct _OspfSrLsa {
    UINT1         au1Lsa[MAX_SR_LSAS];
}tOspfSrLsa;

typedef struct _OsSrArea
{
	UINT1 a;
}tOsSrArea;


typedef struct _SrRouteEntryInfo
{
    tGenU4Addr          destAddr;     /* The destination address */
    tGenU4Addr          nextHop;      /* The nexthop address to which any
                                           datagrams destined to the destination,
                                           to be forwarded.*/
    UINT4               u4DestMask;     /* The net mask to be applied to
                                           get the non-host portion for the
                                           destination */
    UINT4               u4RtIfIndx;     /* The interface through which the
                                           route is learnt */
    UINT4               u4RtNxtHopAS;   /* The autonomous system number of
                                           next hop  */
    UINT4               u4OldNextHop;   /* Old Next Hop Address */
    UINT4               u4OldRtIfIndx;   /* Old Next hop Route index */
    INT4                i4Metric1;      /* The reachability cost for the
                                           destination */
    UINT4               u4RowStatus;   /* Status of the row */
    UINT4               u4Flag;        /* Indicates reachablity of route,Best route & Hardware status */
    UINT2               u2AddrType;
    UINT1               au1Pad[2];
}
tSrRouteEntryInfo;

typedef struct _LfaRouteEntryInfo
{
    UINT4               u4DestAddr;     /* The destination address */
    UINT4               u4DestMask;     /* The net mask to be applied to
                                           get the non-host portion for the
                                           destination */
    UINT4               u4NextHop;      /* The nexthop address to which any
                                           datagrams destined to the destination,
                                           to be forwarded.*/
    UINT4               u4RtIfIndx;     /* The interface through which the
                                           route is learnt */
    UINT4               u4RtNxtHopAS;   /* The autonomous system number of
                                           next hop  */
    INT4                i4Metric1;      /* The reachability cost for the
                                           destination */
    UINT4               u4RowStatus;   /* Status of the row */
    UINT4               u4Flag;        /* Indicates reachablity of route,Best route & Hardware status */
    UINT2               u2AddrType;
    UINT1               au1Pad[2];
}
tLfaRouteEntryInfo;


/*

-- tSrTePathInfo --
         |    
         |            
         V
    tSrTeRtrListEntry
         |                    (SLL)
         |------>tSrRtrEntry ------>tSrRtrEntry--->tSrRtrEntry

tSrTeRtrListEntry is Optional RTRId List. If tSrTeRtrListEntry is NULL,
that means only one Router-Id is configured for TE Path.
*/


/* TE Optional Router List*/
typedef struct _SrTeRtrEntry {
    UINT4          u4Index;                 /* Table Index */
    tTMO_SLL       RtrList;                 /* Router Entries in SLL */
} tSrTeRtrListEntry;


/* TE Optional Router-Id Entry*/
typedef struct SrRtrEntry {
 tTMO_SLL_NODE Next;                        /* Next RtrId in the same RtrList */
 UINT4         u4RtrIndex;                  /* RtrId Index in a Index Group */
 tGenU4Addr    routerId;                  /* RtrId */
 UINT1         u1RowStatus;                 /* SNMP Row Status for this Entry */
 UINT1         u1Storage;                   /* Storage Type */
 UINT1         u1Pad[2];                    /* Padding Object */
}tSrRtrEntry;


/* RLFA Route  Information*/
typedef struct _SrRlfaPathInfo {
	tTMO_SLL_NODE	   nextTePath;	
    tGenU4Addr         destAddr;				/* Destination IP */
    tGenU4Addr         mandRtrId;				/* Path u4MandRtrId (Mandatory Router-Id)*/
    tGenU4Addr         rlfaNextHop;
    UINT4	           u4DestMask;				/* Destination Mask */
    UINT4              u4RlfaTunIndex;          /* FTN Tunnel index */
    UINT4              u4RlfaIlmTunIndex;        /* ILM Tunnel Index */
    UINT4              u4RemoteLabel;
    UINT4              u4DestLabel;
    UINT4		rlfaOutIfIndex;
    UINT1              u1RowStatus;             /* SNMP Row Status for this Entry */
    UINT1              u1MPLSStatus;            /* Dynamic Variable : SR_FTN_CREATED / SR_FTN_NOT_CREATED*/
    BOOL1       bIsRlfaActive;
    UINT1         u1Pad;
} tSrRlfaPathInfo;

/*TI-LFA Adj Node info*/
typedef struct _SrTilfaAdjInfo {
 tTMO_SLL_NODE Next;                  /* Next RtrId in the same RtrList */
 tGenU4Addr    AdjIp;                 /* Interface IP of P Adj to Q */
 UINT4         u4AdjSid;              /* Adj SID advertised by P for PQ link*/
 UINT4         u4AreaId;              /* area Id of the Node */
}tSrTilfaAdjInfo;

/* LFA router node */
typedef struct _SrLfaNodeInfo {
    tRBNodeEmbd   RbNode;
    tTMO_SLL      NexthopLfaList;       /* LFA Entries to which this Node As NH*/
    tTMO_SLL      PLfaList;             /* LFA Entries to which this Node As P */
    tTMO_SLL      QLfaList;             /* LFA Entries to which this Node As Q */
    UINT4         u4RtrId;	            /* Router Id of the Node */
    UINT4         u4AreaId;	            /* Area ID of the Node */
}tSrLfaNodeInfo;

/* LFA Adj node */
typedef struct _SrLfaAdjInfo {
    tRBNodeEmbd   RbNode;
    tTMO_SLL      LfaList;       /* LFA Entries to which this Node as part of  Adj SID List*/
    tGenU4Addr    AdjIp;         /* Interface IP of P Adj to Q */
    UINT4         u4AreaId;	     /* Area ID of the Node */
}tSrLfaAdjInfo;

/* Ti-LFA Route  Information*/
typedef struct _SrTilfaPathInfo {
	tRBNodeEmbd        RbNode;
	tTMO_SLL_NODE	   nexthopNode;     /* List node to Nexthop list in LFA Node */
	tTMO_SLL_NODE	   pNode;           /* List node to P list in LFA Node */
	tTMO_SLL_NODE	   qNode;           /* List node to Q list in LFA Node */
	tGenU4Addr         destAddr;            /* Destination Addr */
	tGenU4Addr         lfaNextHopAddr;      /* TI-LFA Next hop address */
    UINT4              u4LfaNexthopRtrId;     /* TI-LFA Next hop node Router Id*/
    UINT4              u4PRtrId;	         /* P Node Router Id */
    UINT4              u4QRtrId;           /* Q Node Router Id */
    tTMO_SLL           pqAdjSidList;      /* PQ Adj Sid  list */
    UINT4              u4RemoteLabel;    /* Label to reach to the P node */
    UINT4              u4DestLabel;      /* Label to reach destinaiton from Q node */
    UINT4		       u4OutIfIndex;
    UINT4              u4FtnTunIndex;          /* FTN Tunnel index */
    UINT4              u4IlmTunIndex;        /* ILM Tunnel Index */
    UINT1              u1RowStatus;             /* SNMP Row Status for this Entry */
    UINT1              u1MPLSStatus;            /* Dynamic Variable : SR_FTN_CREATED / SR_FTN_NOT_CREATED*/
    UINT1              u1LfaStatusFlags;
    UINT1              u1Pad;
} tSrTilfaPathInfo;

typedef struct _SrAdjLfaListNode {
    tTMO_SLL_NODE	   adjNode;           /* List node to Adj list in LFA Node */
    tSrTilfaPathInfo  *pLfaPathInfo;
} tSrAdjLfaListNode;

typedef struct _SrTeLabelStack {
    UINT4       u4TopLabel;
    UINT4       u4LabelStack[MAX_MPLSDB_LABELS_PER_ENTRY];
    UINT1       u1StackSize;
    UINT1       u1Pad[3];
}tSrTeLabelStack;



/* SR TE Route Information*/
typedef struct _SrTeRtEntryInfo {
    tRBNodeEmbd        RbNode;
    tGenU4Addr         destAddr;             /* Destination IP */
    tGenU4Addr         mandRtrId;            /* Path u4MandRtrId (Mandatory Router-Id)*/
    tGenU4Addr         PrimaryNexthop;           /* Primary nexthop  of a mandatory node */
    tGenU4Addr         LfaNexthop;           /* Lfa Nexthop of a mandatory node */
    tGenU4Addr         LfaMandRtrId;         /*Mandatory node based on LFA nexthop */
    tGenU4Addr         FrrIlmNexthop;         /*SR FRR ILM nexthop*/
    tSrTeRtrListEntry  srTeRtrListIndex;     /* Te Rtr List Index (SR_MAX_TE_OPTIONAL_RTR = 5)*/
    tSrTeLabelStack      SrPriTeLblStack ;     /* Te Label Stack for Primary Path , used while FRR LSP switchover to Primary */
    UINT4              u4DestMask;           /* Destination Mask */
    UINT4              u4OutLfaIfIndex;         /* SR TE LFA mpls index */
    UINT4              u4FRRIlmIfIndex;         /* SR FRR ILM interface index */
    UINT1              u1RowStatus;          /* SNMP Row Status for this Entry */
    UINT1              u1MPLSStatus;         /* Dynamic Variable : SR_FTN_CREATED / SR_FTN_NOT_CREATED*/
    UINT1              u1LfaMPLSStatus;         /* TE  LFA FTN status */
    BOOL1              bIsSrTeFRRActive;     /* This flag is to check whether Sr TE Lfa route is active or not */
} tSrTeRtEntryInfo;

 typedef struct _StaticSrLabelStack {
    UINT4       u4TopLabel;
    UINT4       u4LabelStack[MAX_MPLSDB_LABELS_PER_ENTRY];
    UINT1       u1StackSize;
    UINT1       u1Pad[3];
}tStaticSrLabelStack;
   
    /* TE Optional Router-Id Entry*/
    typedef struct StaticSrLbl {
    tTMO_SLL_NODE Next;                     /* Next label in the same List */
    UINT4         u4LblIndex;                  /* Label Index in a Index Group */
    UINT4         u4Label;                    /* Padding Object */
}tStaticSrLbl;

typedef struct _StaticSrLblListEntry {
    UINT4          u4Index;                 /* Table Index */
    tTMO_SLL       StaticLblList;            /* Dest Addr Entries in SLL */
}tStaticSrLblListEntry;

typedef struct _SrStaticEntryInfo {
    tRBNodeEmbd             RbNode;
    tGenU4Addr              destAddr;             /* Destination IP */
    tGenU4Addr              NextHop;              /* Nexthop  of a mandatory node */
    tStaticSrLblListEntry   srStaticLblListIndex;
    tStaticSrLabelStack     SrStaticLblStack;     /*  Label Stack for Primary Path */
    UINT4                   u4DestMask;           /* Destination Mask */
    UINT4                   u4MplsTunIndex;
    UINT4                   u4LspId;
    tSrRtrInfo*             pTopSIDRtrInfo;
    tGenU4Addr              DynamicNextHop;
    UINT1                   u1RowStatus;          /* SNMP Row Status for this Entry */
    UINT1                   u1UnknownSIDAlarm;
    UINT1                   u1SIDExceedsSRGBAlarm;
    UINT1                   u1Pad;
} tSrStaticEntryInfo;

typedef struct _SrOs3NbrInfo {
     tIp6Addr  nbrIpv6Addr;
     UINT4	    nbrRtrId;
     UINT4     u4InterfaceId;
     UINT4     u4AddrlessIf;
} tSrOs3NbrInfo; 

typedef struct _SrOs6pfNbrInfo {
	tTMO_SLL_NODE	   nextNbrNode;
    tSrOs3NbrInfo	   ospfV6NbrInfo;	
}tSrV6OspfNbrInfo;

typedef struct _SrOs4NbrInfo {
     UINT4     u4NbrIpAddr;
     UINT4     u4NbrRtrId;     
} tSrOs4NbrInfo;

typedef struct _SrV4OspfNbrInfo {
    tTMO_SLL_NODE      nextNbrNode;
    tSrOs4NbrInfo      ospfV4NbrInfo;
    UINT4  u4IfIpAddr;
    UINT4  u4AddrlessIf;
    UINT4  u4AreaId;
}tSrV4OspfNbrInfo;

typedef struct {
    tTMO_SLL_NODE       nextNode;
    tSrRouteEntryInfo srRouteInfo;
    tGenU4Addr  u4IpAddr;
    UINT4   u4IfIndex;
    UINT4   u4BitMap;
}tSrDelayRtInfo;

typedef struct _SrTimerDesc {
    VOID                (*pTimerExpFunc)(VOID *);
    INT2                i2Offset;
                            /* If this field is -1 then the fn takes no
                             * parameter
                             */
    UINT1               au1Rsvd[2]; /* Included for 4-byte Alignment */
} tSrTimerDesc;

typedef struct _SrOsRiLsInfo {
     tRBNodeEmbd    RbNode;
     tGenU4Addr     advRtrId;
     UINT4	        u4SrGbMinIndex;
     UINT4	        u4SrGbMaxIndex;
     UINT1          u1Algo;
     UINT1          au1Pad[3];
} tSrOsRiLsInfo; 

#endif
