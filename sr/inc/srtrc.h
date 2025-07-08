#ifndef _SR_TRC_H_
#define _SR_TRC_H_

#define  SR_TRC_FLAG  gSrGlobalInfo.u4SrTrcFlag
#define  SR_NAME      "SR"
#define  SR_FN_ENTRY_EXIT_TRC       0x00000001
#define  SR_MGMT_TRC                0x00000002
#define  SR_MAIN_TRC                0x00000004
#define  SR_UTIL_TRC                0x00000008
#define  SR_RESOURCE_TRC            0x00000010
#define  SR_FAIL_TRC                0x00000020
#define  SR_CTRL_TRC                0x00000040
#define  SR_CRITICAL_TRC            0x00000080
#define  SR_DISABLEALL_TRC          0x00000000
#define  SR_ALL_TRC                SR_FN_ENTRY_EXIT_TRC |\
                                   SR_MGMT_TRC |\
                                   SR_MAIN_TRC |\
                                   SR_UTIL_TRC |\
                                   SR_RESOURCE_TRC |\
                                   SR_FAIL_TRC |\
                                   SR_CTRL_TRC
#define SR_DBG_MIN_TRC             SR_DISABLEALL_TRC
#define SR_DBG_MAX_TRC             SR_PPP_TRC
#define SR_PPP_TRC                 0x80000000




#define SR_TRC(u4Value, pu1Format)     \
           if((u4Value) & (SR_TRC_FLAG)) \
               UtlTrcLog  (SR_TRC_FLAG, u4Value, "SR", pu1Format)

#define SR_TRC1(u4Value, pu1Format, Arg1)     \
           if((u4Value) & (SR_TRC_FLAG)) \
               UtlTrcLog  (SR_TRC_FLAG, u4Value, "SR", pu1Format, Arg1)

#define SR_TRC2(u4Value, pu1Format, Arg1, Arg2)     \
           if((u4Value) & (SR_TRC_FLAG)) \
               UtlTrcLog  (SR_TRC_FLAG, u4Value, "SR", pu1Format, Arg1, Arg2)

#define SR_TRC3(u4Value, pu1Format, Arg1, Arg2, Arg3)     \
           if((u4Value) & (SR_TRC_FLAG)) \
               UtlTrcLog  (SR_TRC_FLAG, u4Value, "SR", pu1Format, Arg1, Arg2, Arg3)

#define SR_TRC4(u4Value, pu1Format, Arg1, Arg2, Arg3, Arg4)     \
           if((u4Value) & (SR_TRC_FLAG)) \
               UtlTrcLog  (SR_TRC_FLAG, u4Value, "SR", pu1Format, Arg1, Arg2, Arg3, Arg4)

#define SR_TRC5(u4Value, pu1Format, Arg1, Arg2, Arg3, Arg4, Arg5)     \
           if((u4Value) & (SR_TRC_FLAG)) \
               UtlTrcLog  (SR_TRC_FLAG, u4Value, "SR", pu1Format, Arg1, Arg2, Arg3, Arg4, Arg5)

#define SR_TRC6(u4Value, pu1Format, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)     \
           if((u4Value) & (SR_TRC_FLAG)) \
               UtlTrcLog  (SR_TRC_FLAG, u4Value, "SR", pu1Format, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)

PUBLIC VOID SrTrcPrint (const char *fname, UINT4 u4Line, const char *s);
PUBLIC VOID SrTrcWrite (CHR1 * s);
PUBLIC CHR1 * SrTrc (UINT4 u4Flags, const char *fmt, ...);

#endif
