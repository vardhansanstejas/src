/*$Id$*/
# include  "lr.h"
# include  "fssnmp.h"
# include  "fssrlw.h"
# include  "fssrwr.h"
# include  "fssrdb.h"

VOID
RegisterFSSR ()
{
    SNMPRegisterMibWithContextIdAndLock (&fssrOID, &fssrEntry, NULL, NULL, NULL,
                                         NULL, SNMP_MSR_TGR_FALSE);
    SNMPAddSysorEntry (&fssrOID, (const UINT1 *) "fssr");
}

VOID
UnRegisterFSSR ()
{
    SNMPUnRegisterMib (&fssrOID, &fssrEntry);
    SNMPDelSysorEntry (&fssrOID, (const UINT1 *) "fssr");
}

INT4
FsSrModeGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhGetFsSrMode (&(pMultiData->i4_SLongValue)));
}

INT4
FsSrV4MinSrGbValueGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhGetFsSrV4MinSrGbValue (&(pMultiData->u4_ULongValue)));
}

INT4
FsSrV4MaxSrGbValueGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhGetFsSrV4MaxSrGbValue (&(pMultiData->u4_ULongValue)));
}

INT4
FsSrModeSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhSetFsSrMode (pMultiData->i4_SLongValue));
}

INT4
FsSrV4MinSrGbValueSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhSetFsSrV4MinSrGbValue (pMultiData->u4_ULongValue));
}

INT4
FsSrV4MaxSrGbValueSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhSetFsSrV4MaxSrGbValue (pMultiData->u4_ULongValue));
}

INT4
FsSrModeTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhTestv2FsSrMode (pu4Error, pMultiData->i4_SLongValue));
}

INT4
FsSrV4MinSrGbValueTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                        tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhTestv2FsSrV4MinSrGbValue (pu4Error, pMultiData->u4_ULongValue));
}

INT4
FsSrV4MaxSrGbValueTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                        tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhTestv2FsSrV4MaxSrGbValue (pu4Error, pMultiData->u4_ULongValue));
}

INT4
FsSrModeDep (UINT4 *pu4Error, tSnmpIndexList * pSnmpIndexList,
             tSNMP_VAR_BIND * pSnmpvarbinds)
{
    return (nmhDepv2FsSrMode (pu4Error, pSnmpIndexList, pSnmpvarbinds));
}

INT4
FsSrV4MinSrGbValueDep (UINT4 *pu4Error, tSnmpIndexList * pSnmpIndexList,
                       tSNMP_VAR_BIND * pSnmpvarbinds)
{
    return (nmhDepv2FsSrV4MinSrGbValue
            (pu4Error, pSnmpIndexList, pSnmpvarbinds));
}

INT4
FsSrV4MaxSrGbValueDep (UINT4 *pu4Error, tSnmpIndexList * pSnmpIndexList,
                       tSNMP_VAR_BIND * pSnmpvarbinds)
{
    return (nmhDepv2FsSrV4MaxSrGbValue
            (pu4Error, pSnmpIndexList, pSnmpvarbinds));
}

INT4
GetNextIndexFsSrSIDTable (tSnmpIndex * pFirstMultiIndex,
                          tSnmpIndex * pNextMultiIndex)
{
    if (pFirstMultiIndex == NULL)
    {
        if (nmhGetFirstIndexFsSrSIDTable
            (&(pNextMultiIndex->pIndex[0].i4_SLongValue),
             pNextMultiIndex->pIndex[1].pOctetStrValue,
             &(pNextMultiIndex->pIndex[2].u4_ULongValue)) == SNMP_FAILURE)
        {
            return SNMP_FAILURE;
        }
    }
    else
    {
        if (nmhGetNextIndexFsSrSIDTable
            (pFirstMultiIndex->pIndex[0].i4_SLongValue,
             &(pNextMultiIndex->pIndex[0].i4_SLongValue),
             pFirstMultiIndex->pIndex[1].pOctetStrValue,
             pNextMultiIndex->pIndex[1].pOctetStrValue,
             pFirstMultiIndex->pIndex[2].u4_ULongValue,
             &(pNextMultiIndex->pIndex[2].u4_ULongValue)) == SNMP_FAILURE)
        {
            return SNMP_FAILURE;
        }
    }

    return SNMP_SUCCESS;
}

INT4
FsSrSIDTypeGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    if (nmhValidateIndexInstanceFsSrSIDTable
        (pMultiIndex->pIndex[0].i4_SLongValue,
         pMultiIndex->pIndex[1].pOctetStrValue,
         pMultiIndex->pIndex[2].u4_ULongValue) == SNMP_FAILURE)
    {
        return SNMP_FAILURE;
    }
    return (nmhGetFsSrSIDType (pMultiIndex->pIndex[0].i4_SLongValue,
                               pMultiIndex->pIndex[1].pOctetStrValue,
                               pMultiIndex->pIndex[2].u4_ULongValue,
                               &(pMultiData->i4_SLongValue)));
}

INT4
FsSrSidIndexTypeGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    if (nmhValidateIndexInstanceFsSrSIDTable
        (pMultiIndex->pIndex[0].i4_SLongValue,
         pMultiIndex->pIndex[1].pOctetStrValue,
         pMultiIndex->pIndex[2].u4_ULongValue) == SNMP_FAILURE)
    {
        return SNMP_FAILURE;
    }
    return (nmhGetFsSrSidIndexType (pMultiIndex->pIndex[0].i4_SLongValue,
                                    pMultiIndex->pIndex[1].pOctetStrValue,
                                    pMultiIndex->pIndex[2].u4_ULongValue,
                                    &(pMultiData->i4_SLongValue)));
}

INT4
FsSrSIDValueGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    if (nmhValidateIndexInstanceFsSrSIDTable
        (pMultiIndex->pIndex[0].i4_SLongValue,
         pMultiIndex->pIndex[1].pOctetStrValue,
         pMultiIndex->pIndex[2].u4_ULongValue) == SNMP_FAILURE)
    {
        return SNMP_FAILURE;
    }
    return (nmhGetFsSrSIDValue (pMultiIndex->pIndex[0].i4_SLongValue,
                                pMultiIndex->pIndex[1].pOctetStrValue,
                                pMultiIndex->pIndex[2].u4_ULongValue,
                                &(pMultiData->u4_ULongValue)));
}

INT4
FsSrFlagsGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    if (nmhValidateIndexInstanceFsSrSIDTable
        (pMultiIndex->pIndex[0].i4_SLongValue,
         pMultiIndex->pIndex[1].pOctetStrValue,
         pMultiIndex->pIndex[2].u4_ULongValue) == SNMP_FAILURE)
    {
        return SNMP_FAILURE;
    }
    return (nmhGetFsSrFlags (pMultiIndex->pIndex[0].i4_SLongValue,
                             pMultiIndex->pIndex[1].pOctetStrValue,
                             pMultiIndex->pIndex[2].u4_ULongValue,
                             &(pMultiData->i4_SLongValue)));
}

INT4
FsSrMTIDGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    if (nmhValidateIndexInstanceFsSrSIDTable
        (pMultiIndex->pIndex[0].i4_SLongValue,
         pMultiIndex->pIndex[1].pOctetStrValue,
         pMultiIndex->pIndex[2].u4_ULongValue) == SNMP_FAILURE)
    {
        return SNMP_FAILURE;
    }
    return (nmhGetFsSrMTID (pMultiIndex->pIndex[0].i4_SLongValue,
                            pMultiIndex->pIndex[1].pOctetStrValue,
                            pMultiIndex->pIndex[2].u4_ULongValue,
                            &(pMultiData->i4_SLongValue)));
}

INT4
FsSrAlgoGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    if (nmhValidateIndexInstanceFsSrSIDTable
        (pMultiIndex->pIndex[0].i4_SLongValue,
         pMultiIndex->pIndex[1].pOctetStrValue,
         pMultiIndex->pIndex[2].u4_ULongValue) == SNMP_FAILURE)
    {
        return SNMP_FAILURE;
    }
    return (nmhGetFsSrAlgo (pMultiIndex->pIndex[0].i4_SLongValue,
                            pMultiIndex->pIndex[1].pOctetStrValue,
                            pMultiIndex->pIndex[2].u4_ULongValue,
                            &(pMultiData->i4_SLongValue)));
}

INT4
FsSrWeightGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    if (nmhValidateIndexInstanceFsSrSIDTable
        (pMultiIndex->pIndex[0].i4_SLongValue,
         pMultiIndex->pIndex[1].pOctetStrValue,
         pMultiIndex->pIndex[2].u4_ULongValue) == SNMP_FAILURE)
    {
        return SNMP_FAILURE;
    }
    return (nmhGetFsSrWeight (pMultiIndex->pIndex[0].i4_SLongValue,
                              pMultiIndex->pIndex[1].pOctetStrValue,
                              pMultiIndex->pIndex[2].u4_ULongValue,
                              &(pMultiData->i4_SLongValue)));
}

INT4
FsSrSIDRowStatusGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    if (nmhValidateIndexInstanceFsSrSIDTable
        (pMultiIndex->pIndex[0].i4_SLongValue,
         pMultiIndex->pIndex[1].pOctetStrValue,
         pMultiIndex->pIndex[2].u4_ULongValue) == SNMP_FAILURE)
    {
        return SNMP_FAILURE;
    }
    return (nmhGetFsSrSIDRowStatus (pMultiIndex->pIndex[0].i4_SLongValue,
                                    pMultiIndex->pIndex[1].pOctetStrValue,
                                    pMultiIndex->pIndex[2].u4_ULongValue,
                                    &(pMultiData->i4_SLongValue)));
}

INT4
FsSrSIDTypeSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    return (nmhSetFsSrSIDType (pMultiIndex->pIndex[0].i4_SLongValue,
                               pMultiIndex->pIndex[1].pOctetStrValue,
                               pMultiIndex->pIndex[2].u4_ULongValue,
                               pMultiData->i4_SLongValue));
}

INT4
FsSrSidIndexTypeSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    return (nmhSetFsSrSidIndexType (pMultiIndex->pIndex[0].i4_SLongValue,
                                    pMultiIndex->pIndex[1].pOctetStrValue,
                                    pMultiIndex->pIndex[2].u4_ULongValue,
                                    pMultiData->i4_SLongValue));
}

INT4
FsSrSIDValueSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    return (nmhSetFsSrSIDValue (pMultiIndex->pIndex[0].i4_SLongValue,
                                pMultiIndex->pIndex[1].pOctetStrValue,
                                pMultiIndex->pIndex[2].u4_ULongValue,
                                pMultiData->u4_ULongValue));
}

INT4
FsSrFlagsSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    return (nmhSetFsSrFlags (pMultiIndex->pIndex[0].i4_SLongValue,
                             pMultiIndex->pIndex[1].pOctetStrValue,
                             pMultiIndex->pIndex[2].u4_ULongValue,
                             pMultiData->i4_SLongValue));
}

INT4
FsSrMTIDSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    return (nmhSetFsSrMTID (pMultiIndex->pIndex[0].i4_SLongValue,
                            pMultiIndex->pIndex[1].pOctetStrValue,
                            pMultiIndex->pIndex[2].u4_ULongValue,
                            pMultiData->i4_SLongValue));
}

INT4
FsSrAlgoSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    return (nmhSetFsSrAlgo (pMultiIndex->pIndex[0].i4_SLongValue,
                            pMultiIndex->pIndex[1].pOctetStrValue,
                            pMultiIndex->pIndex[2].u4_ULongValue,
                            pMultiData->i4_SLongValue));
}

INT4
FsSrWeightSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    return (nmhSetFsSrWeight (pMultiIndex->pIndex[0].i4_SLongValue,
                              pMultiIndex->pIndex[1].pOctetStrValue,
                              pMultiIndex->pIndex[2].u4_ULongValue,
                              pMultiData->i4_SLongValue));
}

INT4
FsSrSIDRowStatusSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    return (nmhSetFsSrSIDRowStatus (pMultiIndex->pIndex[0].i4_SLongValue,
                                    pMultiIndex->pIndex[1].pOctetStrValue,
                                    pMultiIndex->pIndex[2].u4_ULongValue,
                                    pMultiData->i4_SLongValue));
}

INT4
FsSrSIDTypeTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                 tRetVal * pMultiData)
{
    return (nmhTestv2FsSrSIDType (pu4Error,
                                  pMultiIndex->pIndex[0].i4_SLongValue,
                                  pMultiIndex->pIndex[1].pOctetStrValue,
                                  pMultiIndex->pIndex[2].u4_ULongValue,
                                  pMultiData->i4_SLongValue));
}

INT4
FsSrSidIndexTypeTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                      tRetVal * pMultiData)
{
    return (nmhTestv2FsSrSidIndexType (pu4Error,
                                       pMultiIndex->pIndex[0].i4_SLongValue,
                                       pMultiIndex->pIndex[1].pOctetStrValue,
                                       pMultiIndex->pIndex[2].u4_ULongValue,
                                       pMultiData->i4_SLongValue));
}

INT4
FsSrSIDValueTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                  tRetVal * pMultiData)
{
    return (nmhTestv2FsSrSIDValue (pu4Error,
                                   pMultiIndex->pIndex[0].i4_SLongValue,
                                   pMultiIndex->pIndex[1].pOctetStrValue,
                                   pMultiIndex->pIndex[2].u4_ULongValue,
                                   pMultiData->u4_ULongValue));
}

INT4
FsSrFlagsTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    return (nmhTestv2FsSrFlags (pu4Error,
                                pMultiIndex->pIndex[0].i4_SLongValue,
                                pMultiIndex->pIndex[1].pOctetStrValue,
                                pMultiIndex->pIndex[2].u4_ULongValue,
                                pMultiData->i4_SLongValue));
}

INT4
FsSrMTIDTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    return (nmhTestv2FsSrMTID (pu4Error,
                               pMultiIndex->pIndex[0].i4_SLongValue,
                               pMultiIndex->pIndex[1].pOctetStrValue,
                               pMultiIndex->pIndex[2].u4_ULongValue,
                               pMultiData->i4_SLongValue));
}

INT4
FsSrAlgoTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    return (nmhTestv2FsSrAlgo (pu4Error,
                               pMultiIndex->pIndex[0].i4_SLongValue,
                               pMultiIndex->pIndex[1].pOctetStrValue,
                               pMultiIndex->pIndex[2].u4_ULongValue,
                               pMultiData->i4_SLongValue));
}

INT4
FsSrWeightTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    return (nmhTestv2FsSrWeight (pu4Error,
                                 pMultiIndex->pIndex[0].i4_SLongValue,
                                 pMultiIndex->pIndex[1].pOctetStrValue,
                                 pMultiIndex->pIndex[2].u4_ULongValue,
                                 pMultiData->i4_SLongValue));
}

INT4
FsSrSIDRowStatusTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                      tRetVal * pMultiData)
{
    return (nmhTestv2FsSrSIDRowStatus (pu4Error,
                                       pMultiIndex->pIndex[0].i4_SLongValue,
                                       pMultiIndex->pIndex[1].pOctetStrValue,
                                       pMultiIndex->pIndex[2].u4_ULongValue,
                                       pMultiData->i4_SLongValue));
}

INT4
FsSrSIDTableDep (UINT4 *pu4Error, tSnmpIndexList * pSnmpIndexList,
                 tSNMP_VAR_BIND * pSnmpvarbinds)
{
    return (nmhDepv2FsSrSIDTable (pu4Error, pSnmpIndexList, pSnmpvarbinds));
}

INT4
FsSrDebugFlagGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhGetFsSrDebugFlag (&(pMultiData->u4_ULongValue)));
}

INT4
FsRouterListIndexNextGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhGetFsRouterListIndexNext (&(pMultiData->u4_ULongValue)));
}

INT4
FsSrDebugFlagSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhSetFsSrDebugFlag (pMultiData->u4_ULongValue));
}

INT4
FsSrDebugFlagTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                   tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhTestv2FsSrDebugFlag (pu4Error, pMultiData->u4_ULongValue));
}

INT4
FsSrDebugFlagDep (UINT4 *pu4Error, tSnmpIndexList * pSnmpIndexList,
                  tSNMP_VAR_BIND * pSnmpvarbinds)
{
    return (nmhDepv2FsSrDebugFlag (pu4Error, pSnmpIndexList, pSnmpvarbinds));
}

INT4
GetNextIndexFsSrTePathTable (tSnmpIndex * pFirstMultiIndex,
                             tSnmpIndex * pNextMultiIndex)
{
    if (pFirstMultiIndex == NULL)
    {
        if (nmhGetFirstIndexFsSrTePathTable
            (pNextMultiIndex->pIndex[0].pOctetStrValue,
             &(pNextMultiIndex->pIndex[1].u4_ULongValue),
             pNextMultiIndex->pIndex[2].pOctetStrValue,
             &(pNextMultiIndex->pIndex[3].i4_SLongValue)) == SNMP_FAILURE)
        {
            return SNMP_FAILURE;
        }
    }
    else
    {
        if (nmhGetNextIndexFsSrTePathTable
            (pFirstMultiIndex->pIndex[0].pOctetStrValue,
             pNextMultiIndex->pIndex[0].pOctetStrValue,
             pFirstMultiIndex->pIndex[1].u4_ULongValue,
             &(pNextMultiIndex->pIndex[1].u4_ULongValue),
             pFirstMultiIndex->pIndex[2].pOctetStrValue,
             pNextMultiIndex->pIndex[2].pOctetStrValue,
             pFirstMultiIndex->pIndex[3].i4_SLongValue,
             &(pNextMultiIndex->pIndex[3].i4_SLongValue)) == SNMP_FAILURE)
        {
            return SNMP_FAILURE;
        }
    }

    return SNMP_SUCCESS;
}

INT4
FsSrTeRouterListIndexGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    if (nmhValidateIndexInstanceFsSrTePathTable
        (pMultiIndex->pIndex[0].pOctetStrValue,
         pMultiIndex->pIndex[1].u4_ULongValue,
         pMultiIndex->pIndex[2].pOctetStrValue,
         pMultiIndex->pIndex[3].i4_SLongValue) == SNMP_FAILURE)
    {
        return SNMP_FAILURE;
    }
    return (nmhGetFsSrTeRouterListIndex (pMultiIndex->pIndex[0].pOctetStrValue,
                                         pMultiIndex->pIndex[1].u4_ULongValue,
                                         pMultiIndex->pIndex[2].pOctetStrValue,
                                         pMultiIndex->pIndex[3].i4_SLongValue,
                                         &(pMultiData->u4_ULongValue)));
}

INT4
FsSrTePathRowStatusGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    if (nmhValidateIndexInstanceFsSrTePathTable
        (pMultiIndex->pIndex[0].pOctetStrValue,
         pMultiIndex->pIndex[1].u4_ULongValue,
         pMultiIndex->pIndex[2].pOctetStrValue,
         pMultiIndex->pIndex[3].i4_SLongValue) == SNMP_FAILURE)
    {
        return SNMP_FAILURE;
    }
    return (nmhGetFsSrTePathRowStatus (pMultiIndex->pIndex[0].pOctetStrValue,
                                       pMultiIndex->pIndex[1].u4_ULongValue,
                                       pMultiIndex->pIndex[2].pOctetStrValue,
                                       pMultiIndex->pIndex[3].i4_SLongValue,
                                       &(pMultiData->i4_SLongValue)));
}

INT4
FsSrTeRouterListIndexSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    return (nmhSetFsSrTeRouterListIndex (pMultiIndex->pIndex[0].pOctetStrValue,
                                         pMultiIndex->pIndex[1].u4_ULongValue,
                                         pMultiIndex->pIndex[2].pOctetStrValue,
                                         pMultiIndex->pIndex[3].i4_SLongValue,
                                         pMultiData->u4_ULongValue));
}

INT4
FsSrTePathRowStatusSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    return (nmhSetFsSrTePathRowStatus (pMultiIndex->pIndex[0].pOctetStrValue,
                                       pMultiIndex->pIndex[1].u4_ULongValue,
                                       pMultiIndex->pIndex[2].pOctetStrValue,
                                       pMultiIndex->pIndex[3].i4_SLongValue,
                                       pMultiData->i4_SLongValue));
}

INT4
FsSrTeRouterListIndexTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                           tRetVal * pMultiData)
{
    return (nmhTestv2FsSrTeRouterListIndex (pu4Error,
                                            pMultiIndex->pIndex[0].
                                            pOctetStrValue,
                                            pMultiIndex->pIndex[1].
                                            u4_ULongValue,
                                            pMultiIndex->pIndex[2].
                                            pOctetStrValue,
                                            pMultiIndex->pIndex[3].
                                            i4_SLongValue,
                                            pMultiData->u4_ULongValue));
}

INT4
FsSrTePathRowStatusTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                         tRetVal * pMultiData)
{
    return (nmhTestv2FsSrTePathRowStatus (pu4Error,
                                          pMultiIndex->pIndex[0].pOctetStrValue,
                                          pMultiIndex->pIndex[1].u4_ULongValue,
                                          pMultiIndex->pIndex[2].pOctetStrValue,
                                          pMultiIndex->pIndex[3].i4_SLongValue,
                                          pMultiData->i4_SLongValue));
}

INT4
FsSrTePathTableDep (UINT4 *pu4Error, tSnmpIndexList * pSnmpIndexList,
                    tSNMP_VAR_BIND * pSnmpvarbinds)
{
    return (nmhDepv2FsSrTePathTable (pu4Error, pSnmpIndexList, pSnmpvarbinds));
}

INT4
GetNextIndexFsSrTeRtrListTable (tSnmpIndex * pFirstMultiIndex,
                                tSnmpIndex * pNextMultiIndex)
{
    if (pFirstMultiIndex == NULL)
    {
        if (nmhGetFirstIndexFsSrTeRtrListTable
            (&(pNextMultiIndex->pIndex[0].u4_ULongValue),
             &(pNextMultiIndex->pIndex[1].u4_ULongValue)) == SNMP_FAILURE)
        {
            return SNMP_FAILURE;
        }
    }
    else
    {
        if (nmhGetNextIndexFsSrTeRtrListTable
            (pFirstMultiIndex->pIndex[0].u4_ULongValue,
             &(pNextMultiIndex->pIndex[0].u4_ULongValue),
             pFirstMultiIndex->pIndex[1].u4_ULongValue,
             &(pNextMultiIndex->pIndex[1].u4_ULongValue)) == SNMP_FAILURE)
        {
            return SNMP_FAILURE;
        }
    }

    return SNMP_SUCCESS;
}

INT4
FsSrTeOptRouterIdGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    if (nmhValidateIndexInstanceFsSrTeRtrListTable
        (pMultiIndex->pIndex[0].u4_ULongValue,
         pMultiIndex->pIndex[1].u4_ULongValue) == SNMP_FAILURE)
    {
        return SNMP_FAILURE;
    }
    return (nmhGetFsSrTeOptRouterId (pMultiIndex->pIndex[0].u4_ULongValue,
                                     pMultiIndex->pIndex[1].u4_ULongValue,
                                     pMultiData->pOctetStrValue));
}

INT4
FsSrTeRtrRowStatusGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    if (nmhValidateIndexInstanceFsSrTeRtrListTable
        (pMultiIndex->pIndex[0].u4_ULongValue,
         pMultiIndex->pIndex[1].u4_ULongValue) == SNMP_FAILURE)
    {
        return SNMP_FAILURE;
    }
    return (nmhGetFsSrTeRtrRowStatus (pMultiIndex->pIndex[0].u4_ULongValue,
                                      pMultiIndex->pIndex[1].u4_ULongValue,
                                      &(pMultiData->i4_SLongValue)));
}

INT4
FsSrTeOptRouterIdSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    return (nmhSetFsSrTeOptRouterId (pMultiIndex->pIndex[0].u4_ULongValue,
                                     pMultiIndex->pIndex[1].u4_ULongValue,
                                     pMultiData->pOctetStrValue));
}

INT4
FsSrTeRtrRowStatusSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    return (nmhSetFsSrTeRtrRowStatus (pMultiIndex->pIndex[0].u4_ULongValue,
                                      pMultiIndex->pIndex[1].u4_ULongValue,
                                      pMultiData->i4_SLongValue));
}

INT4
FsSrTeOptRtrAddrTypeSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiData);
    UNUSED_PARAM (pMultiIndex);
    return SNMP_SUCCESS;
}

INT4
FsSrTeOptRouterIdTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                       tRetVal * pMultiData)
{
    return (nmhTestv2FsSrTeOptRouterId (pu4Error,
                                        pMultiIndex->pIndex[0].u4_ULongValue,
                                        pMultiIndex->pIndex[1].u4_ULongValue,
                                        pMultiData->pOctetStrValue));
}

INT4
FsSrTeRtrRowStatusTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                        tRetVal * pMultiData)
{
    return (nmhTestv2FsSrTeRtrRowStatus (pu4Error,
                                         pMultiIndex->pIndex[0].u4_ULongValue,
                                         pMultiIndex->pIndex[1].u4_ULongValue,
                                         pMultiData->i4_SLongValue));
}

INT4
FsSrTeOptRtrAddrTypeTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                          tRetVal * pMultiData)
{
    UNUSED_PARAM (pu4Error);
    UNUSED_PARAM (pMultiData);
    UNUSED_PARAM (pMultiIndex);
    return SNMP_SUCCESS;
}

INT4
FsSrTeRtrListTableDep (UINT4 *pu4Error, tSnmpIndexList * pSnmpIndexList,
                       tSNMP_VAR_BIND * pSnmpvarbinds)
{
    return (nmhDepv2FsSrTeRtrListTable
            (pu4Error, pSnmpIndexList, pSnmpvarbinds));
}

INT4
FsSrV4StatusGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhGetFsSrV4Status (&(pMultiData->i4_SLongValue)));
}

INT4
FsSrV4AlternateStatusGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhGetFsSrV4AlternateStatus (&(pMultiData->i4_SLongValue)));
}

INT4
FsSrV6MinSrGbValueGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhGetFsSrV6MinSrGbValue (&(pMultiData->u4_ULongValue)));
}

INT4
FsSrV6MaxSrGbValueGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhGetFsSrV6MaxSrGbValue (&(pMultiData->u4_ULongValue)));
}

INT4
FsSrV6AlternateStatusGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhGetFsSrV6AlternateStatus (&(pMultiData->i4_SLongValue)));
}

INT4
FsSrV6StatusGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhGetFsSrV6Status (&(pMultiData->i4_SLongValue)));
}

INT4
FsSrGlobalAdminStatusGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhGetFsSrGlobalAdminStatus (&(pMultiData->i4_SLongValue)));
}

INT4
FsSrIpv4AddrFamilyGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhGetFsSrIpv4AddrFamily (&(pMultiData->i4_SLongValue)));
}

INT4
FsSrIpv6AddrFamilyGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhGetFsSrIpv6AddrFamily (&(pMultiData->i4_SLongValue)));
}

INT4
FsSrV4StatusSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhSetFsSrV4Status (pMultiData->i4_SLongValue));
}

INT4
FsSrV4AlternateStatusSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhSetFsSrV4AlternateStatus (pMultiData->i4_SLongValue));
}

INT4
FsSrV6MinSrGbValueSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhSetFsSrV6MinSrGbValue (pMultiData->u4_ULongValue));
}

INT4
FsSrV6MaxSrGbValueSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhSetFsSrV6MaxSrGbValue (pMultiData->u4_ULongValue));
}

INT4
FsSrV6AlternateStatusSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhSetFsSrV6AlternateStatus (pMultiData->i4_SLongValue));
}

INT4
FsSrV6StatusSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhSetFsSrV6Status (pMultiData->i4_SLongValue));
}

INT4
FsSrGlobalAdminStatusSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhSetFsSrGlobalAdminStatus (pMultiData->i4_SLongValue));
}

INT4
FsSrIpv4AddrFamilySet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhSetFsSrIpv4AddrFamily (pMultiData->i4_SLongValue));
}

INT4
FsSrIpv6AddrFamilySet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhSetFsSrIpv6AddrFamily (pMultiData->i4_SLongValue));
}

INT4
FsSrV4StatusTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                  tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhTestv2FsSrV4Status (pu4Error, pMultiData->i4_SLongValue));
}

INT4
FsSrV4AlternateStatusTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                           tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhTestv2FsSrV4AlternateStatus
            (pu4Error, pMultiData->i4_SLongValue));
}

INT4
FsSrV6MinSrGbValueTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                        tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhTestv2FsSrV6MinSrGbValue (pu4Error, pMultiData->u4_ULongValue));
}

INT4
FsSrV6MaxSrGbValueTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                        tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhTestv2FsSrV6MaxSrGbValue (pu4Error, pMultiData->u4_ULongValue));
}

INT4
FsSrV6AlternateStatusTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                           tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhTestv2FsSrV6AlternateStatus
            (pu4Error, pMultiData->i4_SLongValue));
}

INT4
FsSrV6StatusTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                  tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhTestv2FsSrV6Status (pu4Error, pMultiData->i4_SLongValue));
}

INT4
FsSrGlobalAdminStatusTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                           tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhTestv2FsSrGlobalAdminStatus
            (pu4Error, pMultiData->i4_SLongValue));
}

INT4
FsSrIpv4AddrFamilyTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                        tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhTestv2FsSrIpv4AddrFamily (pu4Error, pMultiData->i4_SLongValue));
}

INT4
FsSrIpv6AddrFamilyTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                        tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhTestv2FsSrIpv6AddrFamily (pu4Error, pMultiData->i4_SLongValue));
}

INT4
FsSrV4StatusDep (UINT4 *pu4Error, tSnmpIndexList * pSnmpIndexList,
                 tSNMP_VAR_BIND * pSnmpvarbinds)
{
    return (nmhDepv2FsSrV4Status (pu4Error, pSnmpIndexList, pSnmpvarbinds));
}

INT4
FsSrV4AlternateStatusDep (UINT4 *pu4Error, tSnmpIndexList * pSnmpIndexList,
                          tSNMP_VAR_BIND * pSnmpvarbinds)
{
    return (nmhDepv2FsSrV4AlternateStatus
            (pu4Error, pSnmpIndexList, pSnmpvarbinds));
}

INT4
FsSrV6MinSrGbValueDep (UINT4 *pu4Error, tSnmpIndexList * pSnmpIndexList,
                       tSNMP_VAR_BIND * pSnmpvarbinds)
{
    return (nmhDepv2FsSrV6MinSrGbValue
            (pu4Error, pSnmpIndexList, pSnmpvarbinds));
}

INT4
FsSrV6MaxSrGbValueDep (UINT4 *pu4Error, tSnmpIndexList * pSnmpIndexList,
                       tSNMP_VAR_BIND * pSnmpvarbinds)
{
    return (nmhDepv2FsSrV6MaxSrGbValue
            (pu4Error, pSnmpIndexList, pSnmpvarbinds));
}

INT4
FsSrV6AlternateStatusDep (UINT4 *pu4Error, tSnmpIndexList * pSnmpIndexList,
                          tSNMP_VAR_BIND * pSnmpvarbinds)
{
    return (nmhDepv2FsSrV6AlternateStatus
            (pu4Error, pSnmpIndexList, pSnmpvarbinds));
}

INT4
FsSrV6StatusDep (UINT4 *pu4Error, tSnmpIndexList * pSnmpIndexList,
                 tSNMP_VAR_BIND * pSnmpvarbinds)
{
    return (nmhDepv2FsSrV6Status (pu4Error, pSnmpIndexList, pSnmpvarbinds));
}

INT4
FsSrGlobalAdminStatusDep (UINT4 *pu4Error, tSnmpIndexList * pSnmpIndexList,
                          tSNMP_VAR_BIND * pSnmpvarbinds)
{
    return (nmhDepv2FsSrGlobalAdminStatus
            (pu4Error, pSnmpIndexList, pSnmpvarbinds));
}

INT4
FsSrIpv4AddrFamilyDep (UINT4 *pu4Error, tSnmpIndexList * pSnmpIndexList,
                       tSNMP_VAR_BIND * pSnmpvarbinds)
{
    return (nmhDepv2FsSrIpv4AddrFamily
            (pu4Error, pSnmpIndexList, pSnmpvarbinds));
}

INT4
FsSrIpv6AddrFamilyDep (UINT4 *pu4Error, tSnmpIndexList * pSnmpIndexList,
                       tSNMP_VAR_BIND * pSnmpvarbinds)
{
    return (nmhDepv2FsSrIpv6AddrFamily
            (pu4Error, pSnmpIndexList, pSnmpvarbinds));
}

INT4
GetNextIndexFsStaticSrLabelStackTable (tSnmpIndex * pFirstMultiIndex,
                                       tSnmpIndex * pNextMultiIndex)
{
    if (pFirstMultiIndex == NULL)
    {
        if (nmhGetFirstIndexFsStaticSrLabelStackTable
            (&(pNextMultiIndex->pIndex[0].u4_ULongValue),
             &(pNextMultiIndex->pIndex[1].u4_ULongValue)) == SNMP_FAILURE)
        {
            return SNMP_FAILURE;
        }
    }
    else
    {
        if (nmhGetNextIndexFsStaticSrLabelStackTable
            (pFirstMultiIndex->pIndex[0].u4_ULongValue,
             &(pNextMultiIndex->pIndex[0].u4_ULongValue),
             pFirstMultiIndex->pIndex[1].u4_ULongValue,
             &(pNextMultiIndex->pIndex[1].u4_ULongValue)) == SNMP_FAILURE)
        {
            return SNMP_FAILURE;
        }
    }

    return SNMP_SUCCESS;
}

INT4
FsStaticSrLabelStackLabelGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    if (nmhValidateIndexInstanceFsStaticSrLabelStackTable
        (pMultiIndex->pIndex[0].u4_ULongValue,
         pMultiIndex->pIndex[1].u4_ULongValue) == SNMP_FAILURE)
    {
        return SNMP_FAILURE;
    }
    return (nmhGetFsStaticSrLabelStackLabel
            (pMultiIndex->pIndex[0].u4_ULongValue,
             pMultiIndex->pIndex[1].u4_ULongValue,
             &(pMultiData->u4_ULongValue)));

}

INT4
FsStaticSrLabelStackLabelSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    return (nmhSetFsStaticSrLabelStackLabel
            (pMultiIndex->pIndex[0].u4_ULongValue,
             pMultiIndex->pIndex[1].u4_ULongValue, 
             pMultiData->u4_ULongValue));

}

INT4
FsStaticSrLabelStackLabelTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                               tRetVal * pMultiData)
{
    return (nmhTestv2FsStaticSrLabelStackLabel (pu4Error,
                                                pMultiIndex->
                                                pIndex[0].u4_ULongValue,
                                                pMultiIndex->
                                                pIndex[1].u4_ULongValue,
                                                pMultiData->u4_ULongValue));
}

INT4
FsStaticSrLabelStackTableDep (UINT4 *pu4Error, tSnmpIndexList * pSnmpIndexList,
                              tSNMP_VAR_BIND * pSnmpvarbinds)
{
    return (nmhDepv2FsStaticSrLabelStackTable
            (pu4Error, pSnmpIndexList, pSnmpvarbinds));
}

INT4
GetNextIndexFsStaticSrTable (tSnmpIndex * pFirstMultiIndex,
                             tSnmpIndex * pNextMultiIndex)
{
    if (pFirstMultiIndex == NULL)
    {
        if (nmhGetFirstIndexFsStaticSrTable
            (pNextMultiIndex->pIndex[0].pOctetStrValue,
             &(pNextMultiIndex->pIndex[1].u4_ULongValue),
             &(pNextMultiIndex->pIndex[2].i4_SLongValue),
             &(pNextMultiIndex->pIndex[3].u4_ULongValue)) == SNMP_FAILURE)
        {
            return SNMP_FAILURE;
        }
    }
    else
    {
        if (nmhGetNextIndexFsStaticSrTable
            (pFirstMultiIndex->pIndex[0].pOctetStrValue,
             pNextMultiIndex->pIndex[0].pOctetStrValue,
             pFirstMultiIndex->pIndex[1].u4_ULongValue,
             &(pNextMultiIndex->pIndex[1].u4_ULongValue),
             pFirstMultiIndex->pIndex[2].i4_SLongValue,
             &(pNextMultiIndex->pIndex[2].i4_SLongValue),
             pFirstMultiIndex->pIndex[3].u4_ULongValue,
             &(pNextMultiIndex->pIndex[3].u4_ULongValue)
             ) == SNMP_FAILURE)
        {
            return SNMP_FAILURE;
        }
    }

    return SNMP_SUCCESS;
}

INT4
FsStaticSrNextHopAddrGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    if (nmhValidateIndexInstanceFsStaticSrTable
        (pMultiIndex->pIndex[0].pOctetStrValue,
         pMultiIndex->pIndex[1].u4_ULongValue,
         pMultiIndex->pIndex[2].i4_SLongValue,
         pMultiIndex->pIndex[3].u4_ULongValue) == SNMP_FAILURE)
    {
        return SNMP_FAILURE;
    }
    return (nmhGetFsStaticSrNextHopAddr (pMultiIndex->pIndex[0].pOctetStrValue,
                                         pMultiIndex->pIndex[1].u4_ULongValue,
                                         pMultiIndex->pIndex[2].i4_SLongValue,
                                         pMultiIndex->pIndex[3].u4_ULongValue,
                                         pMultiData->pOctetStrValue));
}

INT4
FsStaticSrLabelStackIndexGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    if (nmhValidateIndexInstanceFsStaticSrTable
        (pMultiIndex->pIndex[0].pOctetStrValue,
         pMultiIndex->pIndex[1].u4_ULongValue,
         pMultiIndex->pIndex[2].i4_SLongValue,
         pMultiIndex->pIndex[3].u4_ULongValue) == SNMP_FAILURE)
    {
        return SNMP_FAILURE;
    }
    return (nmhGetFsStaticSrLabelStackIndex
            (pMultiIndex->pIndex[0].pOctetStrValue,
             pMultiIndex->pIndex[1].u4_ULongValue,
             pMultiIndex->pIndex[2].i4_SLongValue,
             pMultiIndex->pIndex[3].u4_ULongValue,
             &(pMultiData->u4_ULongValue)));
}

INT4
FsStaticSrRowStatusGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    if (nmhValidateIndexInstanceFsStaticSrTable
        (pMultiIndex->pIndex[0].pOctetStrValue,
         pMultiIndex->pIndex[1].u4_ULongValue,
         pMultiIndex->pIndex[2].i4_SLongValue,
         pMultiIndex->pIndex[3].u4_ULongValue) == SNMP_FAILURE)
    {
        return SNMP_FAILURE;
    }
    return (nmhGetFsStaticSrRowStatus (pMultiIndex->pIndex[0].pOctetStrValue,
                                       pMultiIndex->pIndex[1].u4_ULongValue,
                                       pMultiIndex->pIndex[2].i4_SLongValue,
                                       pMultiIndex->pIndex[3].u4_ULongValue,
                                       &(pMultiData->i4_SLongValue)));
}

INT4
FsStaticSrNextHopAddrSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    return (nmhSetFsStaticSrNextHopAddr (pMultiIndex->pIndex[0].pOctetStrValue,
                                         pMultiIndex->pIndex[1].u4_ULongValue,
                                         pMultiIndex->pIndex[2].i4_SLongValue,
                                         pMultiIndex->pIndex[3].u4_ULongValue,
                                         pMultiData->pOctetStrValue));
}

INT4
FsStaticSrLabelStackIndexSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    return (nmhSetFsStaticSrLabelStackIndex
            (pMultiIndex->pIndex[0].pOctetStrValue,
             pMultiIndex->pIndex[1].u4_ULongValue,
             pMultiIndex->pIndex[2].i4_SLongValue, pMultiIndex->pIndex[3].u4_ULongValue, pMultiData->u4_ULongValue));
}

INT4
FsStaticSrRowStatusSet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    return (nmhSetFsStaticSrRowStatus (pMultiIndex->pIndex[0].pOctetStrValue,
                                       pMultiIndex->pIndex[1].u4_ULongValue,
                                       pMultiIndex->pIndex[2].i4_SLongValue,
                                       pMultiIndex->pIndex[3].u4_ULongValue,
                                       pMultiData->i4_SLongValue));
}

INT4
FsStaticSrNextHopAddrTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                           tRetVal * pMultiData)
{
    return (nmhTestv2FsStaticSrNextHopAddr (pu4Error,
                                            pMultiIndex->
                                            pIndex[0].pOctetStrValue,
                                            pMultiIndex->
                                            pIndex[1].u4_ULongValue,
                                            pMultiIndex->
                                            pIndex[2].i4_SLongValue,
                                            pMultiIndex->pIndex[3].u4_ULongValue,
                                            pMultiData->pOctetStrValue));
}

INT4
FsStaticSrLabelStackIndexTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                               tRetVal * pMultiData)
{
    return (nmhTestv2FsStaticSrLabelStackIndex (pu4Error,
                                                pMultiIndex->
                                                pIndex[0].pOctetStrValue,
                                                pMultiIndex->
                                                pIndex[1].u4_ULongValue,
                                                pMultiIndex->
                                                pIndex[2].i4_SLongValue,
                                                pMultiData->u4_ULongValue,
                                                pMultiIndex->pIndex[3].u4_ULongValue));
}

INT4
FsStaticSrRowStatusTest (UINT4 *pu4Error, tSnmpIndex * pMultiIndex,
                         tRetVal * pMultiData)
{
    return (nmhTestv2FsStaticSrRowStatus (pu4Error,
                                          pMultiIndex->pIndex[0].pOctetStrValue,
                                          pMultiIndex->pIndex[1].u4_ULongValue,
                                          pMultiIndex->pIndex[2].i4_SLongValue,
                                          pMultiIndex->pIndex[3].u4_ULongValue,
                                          pMultiData->i4_SLongValue));
}

INT4
FsStaticSrTableDep (UINT4 *pu4Error, tSnmpIndexList * pSnmpIndexList,
                    tSNMP_VAR_BIND * pSnmpvarbinds)
{
    return (nmhDepv2FsStaticSrTable (pu4Error, pSnmpIndexList, pSnmpvarbinds));
}

INT4
FsLabelStackIndexNextGet (tSnmpIndex * pMultiIndex, tRetVal * pMultiData)
{
    UNUSED_PARAM (pMultiIndex);
    return (nmhGetFsLabelStackIndexNext (&(pMultiData->u4_ULongValue)));
}
