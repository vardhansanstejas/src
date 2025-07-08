/********************************************************************
 * Copyright (C) 2007 Aricent Inc . All Rights Reserved
 *
 * $Id: srdef.h,v 1.19 2018/11/16 11:21:46 artee Exp $
 *
 * Description: This file contains definitions macros for some 
 *              Hard coded values.
 *********************************************************************/
#ifndef _SR_DEF_H_
#define _SR_DEF_H_

#define SR_FALSE               0
#define SR_TRUE                1

enum{
SR_ENABLED=1,
SR_DISABLED
};

#define SR_GET_SELF_SID_INDEX_VAL(pNodeLabel, pSrSidEntry)\
{\
    if (SR_P_INTF_SID_INDEX_TYPE (pSrSidEntry) == SR_SID_REL_INDEX)\
    {\
        *pNodeLabel = SR_P_INTF_SID_INDEX (pSrSidEntry);\
    }\
    else\
    {\
        *pNodeLabel = SR_P_INTF_SID_INDEX (pSrSidEntry) -\
            gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex;\
    }\
}

#define SR_GET_V6_SELF_SID_INDEX_VAL(pNodeLabel)\
{\
    if (gSrGlobalInfo.pV6SrSelfNodeInfo->u1SidIndexType == SR_SID_REL_INDEX)\
    {\
        *pNodeLabel = gSrGlobalInfo.pV6SrSelfNodeInfo->u4PrefixSidLabelIndex;\
    }\
    else\
    {\
        *pNodeLabel = gSrGlobalInfo.pV6SrSelfNodeInfo->u4PrefixSidLabelIndex -\
            gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex;\
    }\
}

#define SR_GET_SELF_IN_LABEL(pInLabel, u4SidValue)\
{\
    if (SrUtilCheckNodeIdConfigured () == SR_SUCCESS)\
    {\
        *pInLabel = gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex +\
            u4SidValue;\
    }\
}

#define SR_GET_SELF_SID_LABEL(pu4Label, pSrSidEntry)\
{\
    if (SR_P_INTF_SID_INDEX_TYPE (pSrSidEntry) == SR_SID_REL_INDEX)\
    {\
        if (pSrSidEntry->ifIpAddr.u2AddrType == SR_IPV4_ADDR_TYPE)\
        {\
            *pu4Label =\
            SR_P_INTF_SID_INDEX (pSrSidEntry) +\
            gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex;\
        }\
        else if (pSrSidEntry->ifIpAddr.u2AddrType == SR_IPV6_ADDR_TYPE)\
        {\
            *pu4Label =\
            SR_P_INTF_SID_INDEX (pSrSidEntry) +\
            gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex;\
        }\
    }\
    else\
    {\
        *pu4Label = SR_P_INTF_SID_INDEX (pSrSidEntry);\
    }\
}

#define SRGB_DEFAULT_MIN_VALUE	700000
#define SRGB_DEFAULT_MAX_VALUE	708000

/*Fec Type to be defined in LspInfo*/
#define SR_FEC_PREFIX_TYPE      1

#define SR_SEM_NAME		((const UINT1 *)"SrSem")

/****************************************************************************/
/* macros for lsa construction in linear buffers                            */
/****************************************************************************/
#define  SR_LADD1BYTE(addr, val)     *((UINT1 *)addr) = val; addr++;
#define  SR_LADD2BYTE(addr, val)     SR_CRU_BMC_WTOPDU(addr, val); addr += 2;
#define  SR_LADD3BYTE(addr, val)     SR_THREE_BYTE_TO_PDU(addr, (val)); addr += 3;
#define  SR_LADD4BYTE(addr, val)     SR_CRU_BMC_DWTOPDU(addr, val); addr += 4;

#define  SR_LADDSTR(addr, src, len)  MEMCPY(addr, src, len); addr += len;    

#define  SR_LGET1BYTE(addr)  *((UINT1 *)addr); addr += 1;
#define  SR_LGET2BYTE(addr)  SR_CRU_BMC_WFROMPDU(addr); addr += 2;
#define  SR_LGET3BYTE(addr)  THREE_BYTE_FROM_PDU(addr); addr += 3;
#define  SR_LGET4BYTE(addr)  SR_CRU_BMC_DWFROMPDU(addr); addr += 4;


/*******************************************************
*** MACROs For Host To Network Byte Order Conversion ***
********************************************************/
#define SR_CRU_BMC_WTOPDU(pu1PduAddr,u2Value) \
        *((UINT2 *)(VOID *)(pu1PduAddr)) = OSIX_HTONS(u2Value);

#define SR_CRU_BMC_DWTOPDU(pu1PduAddr,u4Value) \
         SrUtilOspfDwordToPdu (pu1PduAddr,u4Value)

/*******************************************************
*** MACROs For Network To Host Byte Order Conversion ***
********************************************************/
#define SR_CRU_BMC_WFROMPDU(pu1PduAddr) \
        OSIX_NTOHS(*((UINT2 *)(VOID *)(pu1PduAddr)))

#define SR_CRU_BMC_DWFROMPDU(pu1PduAddr) \
            SrUtilOspfDwordFromPdu (pu1PduAddr)


#define SR_THREE_BYTE_TO_PDU(addr, val) { \
                 *((UINT1 *)(addr)   ) =(UINT1) ((val >> 16)&0xff); \
                 *((UINT1 *)(addr) +1) =(UINT1) ((val >> 8)&0xff); \
                 *((UINT1 *)(addr) +2) =(UINT1) (val & 0xff);}

#define SR_THREE_BYTE_FROM_PDU(addr) \
       ( ((*((UINT1 *)(addr)   ) << 16) & 0x00ff0000) |\
         ((*((UINT1 *)(addr) +1) << 8 ) & 0x0000ff00) |\
         ((*((UINT1 *)(addr) +2)      ) & 0x000000ff) )


/* constants used in comparison functions */
 enum {
 SR_EQUAL = 0,
 SR_GREATER,
 SR_LESS
};

#define  ROUTER_LSA                   1
#define  NETWORK_LSA                  2
#define  TYPE10_OPQ_LSA               10
#define  TYPE11_OPQ_LSA               11
#define  NEW_LSA                      1  /*00000001*/
#define  NEW_LSA_INSTANCE             2   /*00000010*/
#define  FLUSHED_LSA                  4   /*00000100*/

#define SR_SRGB_PRESENT               1  /*00000001*/
#define SR_PREFIX_SID_PRESENT         2  /*00000010*/
#define SR_ADJ_SID_PRESENT            4  /*00000100*/

#define OSPF_LSA_HEADER_LENGTH        20
#define OSPF_ROUTER_LSA_OFFSET        4
#define OSPF_LINK_TYPE_P2P            1
#define OSPF_LINK_TYPE_BROADCAST      2




#define SR_SHUT_CMD                 0
#define SR_DESTROY_CMD              1

#define SR_CONFLICT_WINNER          1
#define SR_CONFLICT_NOT_WINNER      2

#define SR_ALARM_CLEAR              0
#define SR_ALARM_RAISE              1

#define PREFIX_CONFLICT_ALARM       1
#define SID_CONFLICT_ALARM          2
#define OOR_SID_ALARM               3

#define SR_SID_CONFLICT_MPLS_DEL    1
#define SR_PREFIX_CONFLICT_MPLS_DEL 2

#define SR_SID_TYPE_LABEL         1
#define SR_SID_TYPE_INDEX         2

#define SR_OSPF_REG                   1
#define SR_OSPF_SEND_OPQ_LSA      2
#define SR_OSPF_DEREG             3
#define SR_OSPF_LSA_REQ           4

#define SR_OSPFV3_REG               11
#define SR_OSPFV3_SEND_OPQ_LSA      12
#define SR_OSPFV3_DEREG             13
/** SID/Label Sub TLV **/
#define SID_LABEL_SUB_TLV_TYPE                    1
#define SID_LABEL_SUB_TLV_SID_LENGTH              4
#define SID_LABEL_SUB_TLV_LABEL_LENGTH            3

#define SID_LABEL_SUB_TLV_TYPE_V3                 7
#define PREFIX_SID_SUB_TLV            2
#define OSPF_SUCCESS              0
#define OSPF_FAILURE              -1

#define SR_COLD_REBOOT             1
#define SR_WARM_REBOOT             2

#define SR_RI_LSA_TLV_LEN             100
#define V3_SR_RI_LSA_TLV_LEN          200
#define SR_EXT_LINK_LSA_LEN           100

/* Type   - 2 bytes*/
/* Length - 2 bytes*/
/* Value  - 1 bytes*/

#define RI_LSA_SR_ALGO_TLV_TYPE       8
#define RI_LSA_SR_ALGO_TLV_LEN        1  /*only 1 algo supported so length is 1 for now*/
#define RI_LSA_SR_ALGO_SPF            0

#define RI_LSA_SR_SID_LABEL_RANGE_TLV_TYPE        9
#define RI_LSA_SR_SID_LABEL_RANGE_TLV_LEN     	  12  /*only 1 TLV SID label TLV present. so fixed length*/

#define OSPFV3_EXTENDED_PREFIX_TLV_LEN                  36

#define OSPFV3_INTRA_AREA_PRFIX_TLV_TYPE			6
#define OSPFV3_INTRA_AREA_PRFIX_TLV_LEN				36
#define OSPFV3_EXT_INTRA_AREA_PREFIX_LSA_HDR_LEN    32
#define OSPFV3_INTRA_AREA_PRFIX_TLV_ADDR_OFFSET     8

#define OSPFV3_INTRA_AREA_PRFIX_TLV_HDR_LEN        28
#define OSPFV3_ROUTER_LINK_TLV_TYPE					1
#define OSPFV3_ROUTER_LINK_TLV_LEN					24

#define OSPFV3_INTER_AREA_PRFIX_TLV_TYPE			3
#define OSPFV3_INTER_AREA_PRFIX_TLV_LEN				36
#define OSPFV3_EXT_INTER_AREA_PREFIX_LSA_HDR_LEN    20
#define OSPFV3_INTER_AREA_PRFIX_TLV_ADDR_OFFSET     8

#define RI_LSA_OPAQUE_TYPE            4
#define V3_RI_LSA_OPAQUE_TYPE        12
#define V3_E_INTRA_AREA_PREFIX_LSA_TYPE       0x29

#define RI_LSA_OPQ_TYPE   		                4
#define EXT_PREFIX_LSA_OPQ_TYPE                 7
#define EXT_LINK_LSA_OPQ_TYPE					8

#define ADJ_SID_TLV             2
#define LAN_ADJ_SID_TLV         3


#define SR_OPQ_LSA_SUPPORTED      7                   /** TYPE9_BIT_MASK  |
                                                          TYPE10_BIT_MASK |
                                                          TYPE11_BIT_MASK   **/
#define  LINK_SCOPE_OPQ_LSA_MASK       1
#define AREA_SCOPE_OPQ_LSA_MASK	2

#define  OSPFV3_LINK_SCOPE_OPQ_LSA            0x000c
#define OSPFV3_AREA_SCOPE_OPQ_LSA 0x200c
#define  OSPFV3_EXT_ROUTER_LSA                0xA021
#define  OSPFV3_EXT_INTER_AREA_PREFIX_LSA     0xA023
#define  OSPFV3_EXT_INTRA_AREA_PREFIX_LSA     0xA029
#define  OSPFV3_ROUTER_LSA                    0x2001
#define SR_OSPF_INFO              31          /** OSPF_OPQ_LSA_TO_OPQ_APP |
                                                              OSPF_RTR_LSA_TO_OPQ_APP |
                                                          OSPF_NET_LSA_TO_OPQ_APP **/

/* Extended LSA TLV type*/
#define  OSPFV3_EXT_ROUTER_TLV                0x0001
#define  OSPFV3_EXT_INTER_AREA_PREFIX_TLV     0x0003
#define  OSPFV3_EXT_INTRA_AREA_PREFIX_TLV     0x0006

#define OSPF_EXT_PRREFIX_LSA_OPAQUE_TYPE	7				
#define OSPF_EXT_PRREFIX_TLV_TYPE			1
#define OSPF_EXT_PRREFIX_TLV_LENGTH            20
#define OSPF_INTER_AREA               3
#define OSPF_INTRA_AREA               1

#define PREFIX_SID_SUB_TLV_TYPE        2
#define PREFIX_SID_SUB_TLV_LEN     	   8    /*Always sending SID Index so length fixed at 8*/ 

#define PREFIX_SID_SUB_TLV_TYPE_V3        4
#define PREFIX_SID_SUB_TLV_LEN_V3		  8

#define PREFIX_SID_NP_FLAG			  	0x40
#define PREFIX_SID_M_FLAG			  	0x20
#define PREFIX_SID_E_FLAG			  	0x10
#define PREFIX_SID_V_FLAG			  	0x08
#define PREFIX_SID_L_FLAG			  	0x04

#define EXT_PREFIX_A_FLAG				0x80
#define EXT_PREFIX_N_FLAG				0x40

#define OSPF_EXT_LINK_LSA_OPAQUE_TYPE		8
#define OSPF_EXT_LINK_TLV_TYPE				1
#define OSPF_EXT_LINK_TLV_FIXED_LENGTH      12

#define ADJ_SID_SUB_TLV_TYPE        		2
#if 0
#define ADJ_SID_SUB_TLV_LEN         		(7 + 1) /*1 byte padded for alignment*/
#endif

#define ADJ_SID_LABEL_SUB_TLV_LEN         	7 /* 3 bytes label*/
#define ADJ_SID_INDEX_SUB_TLV_LEN         	8 /* 4 bytes index */

#define LAN_ADJ_SID_LABEL_SUB_TLV_LEN       11
#define LAN_ADJ_SID_INDEX_SUB_TLV_LEN       12

#define ADJ_SID_SUB_TLV_LEN             	8
#define LAN_ADJ_SID_SUB_TLV_LEN             12






#define ADJ_SID_B_FLAG			  		0x80
#define ADJ_SID_V_FLAG				  	0x40
#define ADJ_SID_L_FLAG				  	0x20
#define ADJ_SID_G_FLAG				  	0x10
#define ADJ_SID_P_FLAG				  	0x08

#define OSPF_EXT_LINK_INFO_STALE        0x80
#define OSPF_EXT_LINK_INFO_FLUSH        0x04
#define OSPF_EXT_LINK_INFO_ORG          0x02
#define OSPF_EXT_LINK_INFO_NEW          0x01

#define SR_ADJ_SID_ILM_CREATE           0x01

/* Adj SID operational flags */
#define SR_ADJ_SID_OP_NONE                 0x00
#define SR_ADJ_SID_OP_CFG_ADD              0x01
#define SR_ADJ_SID_OP_CFG_DEL              0x02
#define SR_ADJ_SID_OP_ADD_LSA              0x04
#define SR_ADJ_SID_OP_DEL_LSA              0x08

/* Ti-LFA status flags */
#define SR_TILFA_SET_NONE                    0x00
#define SR_TILFA_SET_P_LABEL                 0x01
#define SR_TILFA_SET_PQ_ADJ                  0x02
#define SR_TILFA_SET_DEST_LABEL              0x04
#define SR_TILFA_SET_LABEL_STACK             0x80

#define SR_TILFA_LABEL_STACK_DONE          (SR_TILFA_SET_P_LABEL | SR_TILFA_SET_PQ_ADJ | SR_TILFA_SET_DEST_LABEL)

/*Dynamic-SR End*/

#define MAX_IP_ADDR_LEN        4
#define MAX_IPV6_ADDR_LEN      16

#define THREE_BYTE_TO_PDU(addr, val) { \
                 *((UINT1 *)(addr)   ) =(UINT1) ((val >> 16)&0xff); \
                 *((UINT1 *)(addr) +1) =(UINT1) ((val >> 8)&0xff); \
                 *((UINT1 *)(addr) +2) =(UINT1) (val & 0xff);}

#define THREE_BYTE_FROM_PDU(addr) \
       ( ((*((UINT1 *)(addr)   ) << 16) & 0x00ff0000) |\
         ((*((UINT1 *)(addr) +1) << 8 ) & 0x0000ff00) |\
         ((*((UINT1 *)(addr) +2)      ) & 0x000000ff) )


#define  REDIRECTLSP              1

/*******************************************************************************************/
#define SR_EV_CFG_MSG_IN_QUEUE     0x01
#define SR_APP_REG_DREG_EVENT      0x02
#define SR_CFA_IFSTATUS_CHANGE     0x03
#define SR_CFA_INTF_DELETE         0x04
#define SR_MSG_EVT                 0x05
#define SR_OSPF_LSA                0x06

/* Timer Related Definitions */
#define   SR_TIMER_EXP_EVENT        0x07

#define SR_EV_ALL                   (SR_EV_CFG_MSG_IN_QUEUE |\
                                     SR_APP_REG_DREG_EVENT |\
                                     SR_CFA_IFSTATUS_CHANGE |\
                                     SR_CFA_INTF_DELETE |\
                                     SR_MSG_EVT |\
									 SR_OSPF_LSA |\
									 SR_TIMER_EXP_EVENT)


#define SR_INTF_QID					SrMemPoolIds[MAX_SR_INTF_SIZING_ID]
#define SR_RTR_SID_QID				SrMemPoolIds[MAX_SR_RTR_SIZING_ID]
#define SR_RTR_NH_QID				SrMemPoolIds[MAX_SR_RTR_NH_SIZING_ID]
#define SR_QDEPTH_QID				SrMemPoolIds[MAX_SR_QDEPTH_SIZING_ID]
#define SR_RTINFO_QID				SrMemPoolIds[MAX_SR_RTINFO_SIZING_ID]
#define SR_RLFA_PATH_INFO             SrMemPoolIds[MAX_SR_RLFA_PATH_SIZING_ID]
#define SR_TE_RTR_LIST_INFO         SrMemPoolIds[MAX_SR_TE_RTR_LIST_SIZING_ID]
#define SR_TE_RTR_INFO              SrMemPoolIds[MAX_SR_TE_RTR_SIZING_ID]
#define SR_PEER_ADJ_LIST_POOL       SrMemPoolIds[MAX_SR_PEER_ADJ_SIZING_ID]
#define SR_OSPFV6_NBR_LIST_POOL       SrMemPoolIds[MAX_SR_OSPFV6_NBR_SIZING_ID]
#define SR_TE_RT_ENTRY_INFO             SrMemPoolIds[MAX_SR_TE_RT_ENTRY_SIZING_ID]
#define SR_OSPFV4_NBR_LIST_POOL       SrMemPoolIds[MAX_SR_OSPFV4_NBR_SIZING_ID]
#define SR_STATIC_ENTRY_INFO             SrMemPoolIds[MAX_SR_STATIC_ENTRY_SIZING_ID]
#define SR_STATIC_LBL_INFO               SrMemPoolIds[MAX_ST_STATIC_LBL_SIZING_ID]
#define SR_OSPFV4_AREA_LINK         SrMemPoolIds[MAX_SR_AREA_LINK_INFO_SIZING_ID]
#define SR_OSPFV4_EXT_LINK_INFO      SrMemPoolIds[MAX_SR_EXT_LINK_LSA_INFO_SIZING_ID]
#define SR_OSPF_LSA_POOL            SrMemPoolIds[MAX_SR_OSPF_LSA_SIZING_ID]
#define SR_OSPF_RI_LSA_POOL         SrMemPoolIds[MAX_SR_OSPF_RI_LSA_SIZING_ID]
#define SR_TILFA_PATH_INFO             SrMemPoolIds[MAX_SR_TILFA_PATH_SIZING_ID]
#define SR_TILFA_ADJ_INFO             SrMemPoolIds[MAX_SR_TILFA_ADJ_SIZING_ID]
#define SR_LFA_NODE_INFO             SrMemPoolIds[MAX_SR_LFA_NODE_SIZING_ID]
#define SR_LFA_ADJ_INFO              SrMemPoolIds[MAX_SR_LFA_ADJ_SIZING_ID]
#define SR_LFA_ADJ_LIST_NODE_INFO    SrMemPoolIds[MAX_SR_LFA_ADJ_LIST_NODE_SIZING_ID]
#define SR_GR_FTN_TNL_DB_POOL        SrMemPoolIds[MAX_SR_GR_FTN_TNL_DB_SIZING_ID]
#define SR_GR_ILM_TNL_DB_POOL        SrMemPoolIds[MAX_SR_GR_ILM_TNL_DB_SIZING_ID]
#define SR_RTR_DELAY_RT_INFO         SrMemPoolIds[MAX_SR_RTR_DELAY_RT_INFO_SIZING_ID]

/** Macros for memory allocation using cru buffers **/
#define SR_INTF_ALLOC(pu1Block) \
        SR_CRU_BUF_ALLOCATE_FREE_OBJ(SR_INTF_QID,pu1Block,tSrSidInterfaceInfo)

#define SR_RTR_LIST_ALLOC(pu1Block) \
        SR_CRU_BUF_ALLOCATE_FREE_OBJ(SR_RTR_SID_QID,pu1Block,tSrRtrInfo)

#define SR_RTR_NH_LIST_ALLOC(pu1Block) \
        SR_CRU_BUF_ALLOCATE_FREE_OBJ(SR_RTR_NH_QID,pu1Block,tSrRtrNextHopInfo)

#define SR_Q_MEM_ALLOC(pu1Block) \
        SR_CRU_BUF_ALLOCATE_FREE_OBJ (SR_QDEPTH_QID, pu1Block, tSrQMsg)

#define SR_RTINFO_MEM_ALLOC(pu1Block) \
        SR_CRU_BUF_ALLOCATE_FREE_OBJ (SR_RTINFO_QID, pu1Block, tSrRouteEntryInfo)

#define SR_RLFA_PATH_MEM_ALLOC(pu1Block) \
        SR_CRU_BUF_ALLOCATE_FREE_OBJ (SR_RLFA_PATH_INFO, pu1Block, tSrRlfaPathInfo)

#define SR_TE_RTR_LIST_MEM_ALLOC(pu1Block) \
        SR_CRU_BUF_ALLOCATE_FREE_OBJ (SR_TE_RTR_LIST_INFO, pu1Block, tSrTeRtrListEntry)

#define SR_TE_RTR_MEM_ALLOC(pu1Block) \
        SR_CRU_BUF_ALLOCATE_FREE_OBJ (SR_TE_RTR_INFO, pu1Block, tSrRtrEntry)

#define SR_PEER_ADJ_SID_MEM_ALLOC(pu1Block) \
        SR_CRU_BUF_ALLOCATE_FREE_OBJ(SR_PEER_ADJ_LIST_POOL,pu1Block,tAdjSidNode)

#define SR_PEER_ADJ_SID_MEM_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_PEER_ADJ_LIST_POOL, (UINT1 *)pu1Block)

#define SR_OSPFV6_NBR_INFO_MEM_ALLOC(pu1Block) \
        SR_CRU_BUF_ALLOCATE_FREE_OBJ(SR_OSPFV6_NBR_LIST_POOL,pu1Block,tSrV6OspfNbrInfo)

#define SR_TE_RT_ENTRY_MEM_ALLOC(pu1Block) \
        SR_CRU_BUF_ALLOCATE_FREE_OBJ(SR_TE_RT_ENTRY_INFO,pu1Block,tSrTeRtEntryInfo)

#define SR_CRU_BUF_ALLOCATE_FREE_OBJ(PoolId, pu1Block, type) \
        (pu1Block = (type *)(MemAllocMemBlk ((tMemPoolId)PoolId)))

#define SR_STATIC_ENTRY_MEM_ALLOC(pu1Block) \
        SR_CRU_BUF_ALLOCATE_FREE_OBJ(SR_STATIC_ENTRY_INFO,pu1Block,tSrStaticEntryInfo)

#define SR_STATIC_LBL_MEM_ALLOC(pu1Block) \
        SR_CRU_BUF_ALLOCATE_FREE_OBJ(SR_STATIC_LBL_INFO,pu1Block,tStaticSrLbl)

#define SR_AREA_LINK_INFO_MEM_ALLOC(pu1Block) \
        SR_CRU_BUF_ALLOCATE_FREE_OBJ (SR_OSPFV4_AREA_LINK, pu1Block, tareaLinkInfo)

#define SR_EXT_LINK_LSA_INFO_MEM_ALLOC(pu1Block) \
        SR_CRU_BUF_ALLOCATE_FREE_OBJ (SR_OSPFV4_EXT_LINK_INFO, pu1Block, tSrExtLinkLSAInfo)

#define SR_OSPF_LSA_MEM_ALLOC(pu1Block) \
        SR_CRU_BUF_ALLOCATE_FREE_OBJ (SR_OSPF_LSA_POOL, pu1Block, tOspfSrLsa)

#define SR_OSPF_RI_LSA_MEM_ALLOC(pu1Block) \
        SR_CRU_BUF_ALLOCATE_FREE_OBJ (SR_OSPF_RI_LSA_POOL, pu1Block, tSrOsRiLsInfo)
		
#define SR_TILFA_PATH_MEM_ALLOC(pu1Block) \
            SR_CRU_BUF_ALLOCATE_FREE_OBJ (SR_TILFA_PATH_INFO, pu1Block, tSrTilfaPathInfo)

#define SR_TILFA_ADJ_MEM_ALLOC(pu1Block) \
            SR_CRU_BUF_ALLOCATE_FREE_OBJ (SR_TILFA_ADJ_INFO, pu1Block, tSrTilfaAdjInfo)

#define SR_LFA_NODE_MEM_ALLOC(pu1Block) \
            SR_CRU_BUF_ALLOCATE_FREE_OBJ (SR_LFA_NODE_INFO, pu1Block, tSrLfaNodeInfo)

#define SR_LFA_ADJ_MEM_ALLOC(pu1Block) \
            SR_CRU_BUF_ALLOCATE_FREE_OBJ (SR_LFA_ADJ_INFO, pu1Block, tSrLfaAdjInfo)

#define SR_LFA_ADJ_LIST_NODE_MEM_ALLOC(pu1Block) \
            SR_CRU_BUF_ALLOCATE_FREE_OBJ (SR_LFA_ADJ_LIST_NODE_INFO, pu1Block, tSrAdjLfaListNode)

#define SR_ALLOCATE_FTN_TNL_INFO_MEM_BLOCK(pu1Block) \
            SR_CRU_BUF_ALLOCATE_FREE_OBJ(SR_GR_FTN_TNL_DB_POOL, pu1Block,tSrTnlIfTable)

#define SR_ALLOCATE_ILM_TNL_INFO_MEM_BLOCK(pu1Block) \
            SR_CRU_BUF_ALLOCATE_FREE_OBJ(SR_GR_ILM_TNL_DB_POOL, pu1Block,tSrTnlIfTable)
#define SR_RTR_DELAY_RT_INFO_ALLOC(pu1Block) \
        SR_CRU_BUF_ALLOCATE_FREE_OBJ(SR_RTR_DELAY_RT_INFO,pu1Block,tSrDelayRtInfo)

/**Macros for releasing allocated memory using cru buffers **/
#define SR_INTF_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_INTF_QID,(UINT1 *)pu1Block)

#define SR_RTR_LIST_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_RTR_SID_QID,(UINT1 *)pu1Block)

#define SR_RTR_NH_LIST_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_RTR_NH_QID,(UINT1 *)pu1Block)

#define SR_Q_MEM_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_QDEPTH_QID, (UINT1 *)pu1Block)

#define SR_RTINFO_MEM_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_RTINFO_QID, (UINT1 *)pu1Block)

#define SR_RLFA_PATH_MEM_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_RLFA_PATH_INFO, (UINT1 *)pu1Block)

#define SR_TE_RTR_LIST_MEM_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_TE_RTR_LIST_INFO, (UINT1 *)pu1Block)

#define SR_TE_RTR_MEM_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_TE_RTR_INFO, (UINT1 *)pu1Block)

#define SR_PEER_ADJ_SID_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_PEER_ADJ_SID_MEM,(UINT1 *)pu1Block)

#define SR_OSPFV6_NBR_INFO_MEM_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_OSPFV6_NBR_LIST_POOL,(UINT1 *)pu1Block)

#define SR_TE_RT_ENTRY_MEM_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_TE_RT_ENTRY_INFO,(UINT1 *)pu1Block)

#define SR_CRU_BUF_RELEASE_FREE_OBJ(PoolId,ppu1Block) \
        MemReleaseMemBlock((tMemPoolId)PoolId,(UINT1 *)ppu1Block)

#define SR_OSPFV4_NBR_INFO_MEM_ALLOC(pu1Block) \
        SR_CRU_BUF_ALLOCATE_FREE_OBJ(SR_OSPFV4_NBR_LIST_POOL,pu1Block,tSrV4OspfNbrInfo)

#define SR_OSPFV4_NBR_INFO_MEM_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_OSPFV4_NBR_LIST_POOL,(UINT1 *)pu1Block)

 #define SR_STATIC_ENTRY_MEM_FREE(pu1Block) \
         SR_CRU_BUF_RELEASE_FREE_OBJ(SR_STATIC_ENTRY_INFO,(UINT1 *)pu1Block)

 #define SR_STATIC_LBL_MEM_FREE(pu1Block) \
         SR_CRU_BUF_RELEASE_FREE_OBJ(SR_STATIC_LBL_INFO,(UINT1 *)pu1Block)

#define SR_AREA_LINK_INFO_MEM_FREE(pu1Block) \
         SR_CRU_BUF_RELEASE_FREE_OBJ(SR_OSPFV4_AREA_LINK,(UINT1 *)pu1Block)

#define SR_EXT_LINK_LSA_INFO_MEM_FREE(pu1Block) \
         SR_CRU_BUF_RELEASE_FREE_OBJ(SR_OSPFV4_EXT_LINK_INFO,(UINT1 *)pu1Block)

#define SR_OSPF_LSA_MEM_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_OSPF_LSA_POOL, (UINT1 *)pu1Block)

#define SR_OSPF_RI_LSA_MEM_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_OSPF_RI_LSA_POOL, (UINT1 *)pu1Block)
		
#define SR_TILFA_PATH_MEM_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_TILFA_PATH_INFO, (UINT1 *)pu1Block)

#define SR_TILFA_ADJ_MEM_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_TILFA_ADJ_INFO, (UINT1 *)pu1Block)

#define SR_LFA_NODE_MEM_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_LFA_NODE_INFO, (UINT1 *)pu1Block)

#define SR_LFA_ADJ_MEM_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_LFA_ADJ_INFO, (UINT1 *)pu1Block)

#define SR_LFA_ADJ_LIST_NODE_MEM_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_LFA_ADJ_LIST_NODE_INFO, (UINT1 *)pu1Block)

#define SR_FTN_TNL_INFO_MEM_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_GR_FTN_TNL_DB_POOL, (UINT1 *)pu1Block)

#define SR_ILM_TNL_INFO_MEM_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_GR_ILM_TNL_DB_POOL, (UINT1 *)pu1Block)
#define SR_RTR_DELAY_RT_INFO_MEM_FREE(pu1Block) \
        SR_CRU_BUF_RELEASE_FREE_OBJ(SR_RTR_DELAY_RT_INFO, (UINT1 *)pu1Block)

#define SR_OFFSET(x,y)   FSAP_OFFSETOF(x,y)

#define GET_SR_IF_PTR_FROM_SORT_IF_LIST(x) ((tSrSidInterfaceInfo *)(((UINT1 *)(x))\
										- SR_OFFSET(tSrSidInterfaceInfo,nextSortSidIf)))
#define SR_UPDT_LSA_FLAG(u1LsaStatus, pOpqLSAInfo)\
{\
   if (u1LsaStatus == FLUSHED_LSA)\
   {\
     pOpqLSAInfo->u1LSAStatus = NEW_LSA_INSTANCE;\
     pOpqLSAInfo->i1SrSid = SR_INVALID_SID;\
     pOpqLSAInfo->u1FlushTriggr = SR_TRUE;\
   }\
   else\
   {\
     pOpqLSAInfo->u1LSAStatus = u1LsaStatus;\
     pOpqLSAInfo->i1SrSid = SR_ONE;\
     if (u1LsaStatus == NEW_LSA)\
     {\
          pOpqLSAInfo->i1SrSid = SR_TWO;\
     }\
   }\
}

#define SR_UPDT_LSA_STATUS(pExtLinkInfo, u1LsaStatus)\
{\
    if (pExtLinkInfo->ulFlags & OSPF_EXT_LINK_INFO_STALE)\
    {\
        u1LsaStatus = FLUSHED_LSA;\
    }\
    else if(pExtLinkInfo->ulFlags & OSPF_EXT_LINK_INFO_FLUSH)\
    {\
        u1LsaStatus = FLUSHED_LSA;\
    }\
    else if(pExtLinkInfo->ulFlags & OSPF_EXT_LINK_INFO_NEW)\
    {\
        u1LsaStatus = NEW_LSA;\
    }\
    else if(pExtLinkInfo->ulFlags & OSPF_EXT_LINK_INFO_ORG)\
    {\
        u1LsaStatus = NEW_LSA_INSTANCE;\
    }\
    else\
    {\
        u1LsaStatus = NEW_LSA;\
    }\
}


#define SR_PEER_RTR_LIST_FREE(u1IsPeerExist, pRtrNode)\
{\
   if (u1IsPeerExist == SR_FALSE)\
   {\
      SR_RTR_LIST_FREE (pRtrNode);\
   }\
}

#define   SR_SLL_DELETE_LIST(pList, DelFuncOrMacro)\
{\
    while (TMO_SLL_Count(pList) != 0)\
    {\
        DelFuncOrMacro(TMO_SLL_Get(pList));\
    }\
}

#define SrShowLblInfoFromLblGroup   LblMgrShowLblInfoFromLblGroup
#define SrShowLblCountFromLblGroup  LblMgrShowLblCountFromLblGroup

#endif
