/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	sync.c

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	John Chang	2004-09-01      modified for rt2561/2661
*/
#include "rt_config.h"

// 2.4 Ghz channel plan index in the TxPower arrays.
#define	BG_BAND_REGION_0_START	0			// 1,2,3,4,5,6,7,8,9,10,11	
#define	BG_BAND_REGION_0_SIZE	11
#define	BG_BAND_REGION_1_START	0			// 1,2,3,4,5,6,7,8,9,10,11,12,13
#define	BG_BAND_REGION_1_SIZE	13
#define	BG_BAND_REGION_2_START	9			// 10,11
#define	BG_BAND_REGION_2_SIZE	2
#define	BG_BAND_REGION_3_START	9			// 10,11,12,13
#define	BG_BAND_REGION_3_SIZE	4
#define	BG_BAND_REGION_4_START	13			// 14
#define	BG_BAND_REGION_4_SIZE	1
#define	BG_BAND_REGION_5_START	0			// 1,2,3,4,5,6,7,8,9,10,11,12,13,14 
#define	BG_BAND_REGION_5_SIZE	14
#define	BG_BAND_REGION_6_START	2			// 3,4,5,6,7,8,9
#define	BG_BAND_REGION_6_SIZE	7
#define	BG_BAND_REGION_7_START	4			// 5,6,7,8,9,10,11,12,13
#define	BG_BAND_REGION_7_SIZE	9
#define	BG_BAND_REGION_31_START	0			// 1,2,3,4,5,6,7,8,9,10,11,12,13,14 
#define	BG_BAND_REGION_31_SIZE	14

// 5 Ghz channel plan index in the TxPower arrays.
UCHAR A_BAND_REGION_0_CHANNEL_LIST[]={36, 40, 44, 48, 52, 56, 60, 64, 149, 153, 157, 161, 165};
UCHAR A_BAND_REGION_1_CHANNEL_LIST[]={36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140};
UCHAR A_BAND_REGION_2_CHANNEL_LIST[]={36, 40, 44, 48, 52, 56, 60, 64};
UCHAR A_BAND_REGION_3_CHANNEL_LIST[]={52, 56, 60, 64, 149, 153, 157, 161};
UCHAR A_BAND_REGION_4_CHANNEL_LIST[]={149, 153, 157, 161, 165};
UCHAR A_BAND_REGION_5_CHANNEL_LIST[]={149, 153, 157, 161};
UCHAR A_BAND_REGION_6_CHANNEL_LIST[]={36, 40, 44, 48};
UCHAR A_BAND_REGION_7_CHANNEL_LIST[]={36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165};
UCHAR A_BAND_REGION_8_CHANNEL_LIST[]={52, 56, 60, 64};
UCHAR A_BAND_REGION_9_CHANNEL_LIST[]={36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 132, 136, 140, 149, 153, 157, 161, 165};
UCHAR A_BAND_REGION_10_CHANNEL_LIST[]={36, 40, 44, 48, 149, 153, 157, 161, 165};
UCHAR A_BAND_REGION_11_CHANNEL_LIST[]={36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 149, 153, 157, 161};

//BaSizeArray follows the 802.11n definition as MaxRxFactor.  2^(13+factor) bytes. When factor =0, it's about Ba buffer size =8.
UCHAR BaSizeArray[4] = {8,16,32,64};

/* 
	==========================================================================
	Description:
		Update StaCfg->ChannelList[] according to 1) Country Region 2) RF IC type,
		and 3) PHY-mode user selected.
		The outcome is used by driver when doing site survey.

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL
	
	==========================================================================
 */
VOID BuildChannelList(
	IN PRTMP_ADAPTER pAd)
{
	UCHAR i, j, index=0, num=0;
	PUCHAR	pChannelList = NULL;

	NdisZeroMemory(pAd->ChannelList, MAX_NUM_OF_CHANNELS * sizeof(CHANNEL_TX_POWER));

	// if not 11a-only mode, channel list starts from 2.4Ghz band
	if ((pAd->CommonCfg.PhyMode != PHY_11A) 
	)
	{
		switch (pAd->CommonCfg.CountryRegion  & 0x7f)
		{
			case REGION_0_BG_BAND:	// 1 -11
				NdisMoveMemory(&pAd->ChannelList[index], &pAd->TxPower[BG_BAND_REGION_0_START], sizeof(CHANNEL_TX_POWER) * BG_BAND_REGION_0_SIZE);
				index += BG_BAND_REGION_0_SIZE;
				break;
			case REGION_1_BG_BAND:	// 1 - 13
				NdisMoveMemory(&pAd->ChannelList[index], &pAd->TxPower[BG_BAND_REGION_1_START], sizeof(CHANNEL_TX_POWER) * BG_BAND_REGION_1_SIZE);
				index += BG_BAND_REGION_1_SIZE;
				break;
			case REGION_2_BG_BAND:	// 10 - 11
				NdisMoveMemory(&pAd->ChannelList[index], &pAd->TxPower[BG_BAND_REGION_2_START], sizeof(CHANNEL_TX_POWER) * BG_BAND_REGION_2_SIZE);
				index += BG_BAND_REGION_2_SIZE;
				break;
			case REGION_3_BG_BAND:	// 10 - 13
				NdisMoveMemory(&pAd->ChannelList[index], &pAd->TxPower[BG_BAND_REGION_3_START], sizeof(CHANNEL_TX_POWER) * BG_BAND_REGION_3_SIZE);
				index += BG_BAND_REGION_3_SIZE;
				break;
			case REGION_4_BG_BAND:	// 14
				NdisMoveMemory(&pAd->ChannelList[index], &pAd->TxPower[BG_BAND_REGION_4_START], sizeof(CHANNEL_TX_POWER) * BG_BAND_REGION_4_SIZE);
				index += BG_BAND_REGION_4_SIZE;
				break;
			case REGION_5_BG_BAND:	// 1 - 14
				NdisMoveMemory(&pAd->ChannelList[index], &pAd->TxPower[BG_BAND_REGION_5_START], sizeof(CHANNEL_TX_POWER) * BG_BAND_REGION_5_SIZE);
				index += BG_BAND_REGION_5_SIZE;
				break;
			case REGION_6_BG_BAND:	// 3 - 9
				NdisMoveMemory(&pAd->ChannelList[index], &pAd->TxPower[BG_BAND_REGION_6_START], sizeof(CHANNEL_TX_POWER) * BG_BAND_REGION_6_SIZE);
				index += BG_BAND_REGION_6_SIZE;
				break;
			case REGION_7_BG_BAND:  // 5 - 13
				NdisMoveMemory(&pAd->ChannelList[index], &pAd->TxPower[BG_BAND_REGION_7_START], sizeof(CHANNEL_TX_POWER) * BG_BAND_REGION_7_SIZE);
				index += BG_BAND_REGION_7_SIZE;
				break;
			case REGION_31_BG_BAND:	// 1 - 14
				NdisMoveMemory(&pAd->ChannelList[index], &pAd->TxPower[BG_BAND_REGION_31_START], sizeof(CHANNEL_TX_POWER) * BG_BAND_REGION_31_SIZE);
				index += BG_BAND_REGION_31_SIZE;
				break;
			default:            // Error. should never happen
				break;
		}   
		for (i=0; i<index; i++)
			pAd->ChannelList[i].MaxTxPwr = 20;
	}

	if ((pAd->CommonCfg.PhyMode == PHY_11A) || (pAd->CommonCfg.PhyMode == PHY_11ABG_MIXED) 
	)
	{
		switch (pAd->CommonCfg.CountryRegionForABand & 0x7f)
		{
			case REGION_0_A_BAND:
				num = sizeof(A_BAND_REGION_0_CHANNEL_LIST)/sizeof(UCHAR);
				pChannelList = A_BAND_REGION_0_CHANNEL_LIST;
				break;
			case REGION_1_A_BAND:
				num = sizeof(A_BAND_REGION_1_CHANNEL_LIST)/sizeof(UCHAR);
				pChannelList = A_BAND_REGION_1_CHANNEL_LIST;
				break;
			case REGION_2_A_BAND:
				num = sizeof(A_BAND_REGION_2_CHANNEL_LIST)/sizeof(UCHAR);
				pChannelList = A_BAND_REGION_2_CHANNEL_LIST;
				break;
			case REGION_3_A_BAND:
				num = sizeof(A_BAND_REGION_3_CHANNEL_LIST)/sizeof(UCHAR);
				pChannelList = A_BAND_REGION_3_CHANNEL_LIST;
				break;
			case REGION_4_A_BAND:
				num = sizeof(A_BAND_REGION_4_CHANNEL_LIST)/sizeof(UCHAR);
				pChannelList = A_BAND_REGION_4_CHANNEL_LIST;
				break;
			case REGION_5_A_BAND:
				num = sizeof(A_BAND_REGION_5_CHANNEL_LIST)/sizeof(UCHAR);
				pChannelList = A_BAND_REGION_5_CHANNEL_LIST;
				break;
			case REGION_6_A_BAND:
				num = sizeof(A_BAND_REGION_6_CHANNEL_LIST)/sizeof(UCHAR);
				pChannelList = A_BAND_REGION_6_CHANNEL_LIST;
				break;
			case REGION_7_A_BAND:
				num = sizeof(A_BAND_REGION_7_CHANNEL_LIST)/sizeof(UCHAR);
				pChannelList = A_BAND_REGION_7_CHANNEL_LIST;
				break;
			case REGION_8_A_BAND:
				num = sizeof(A_BAND_REGION_8_CHANNEL_LIST)/sizeof(UCHAR);
				pChannelList = A_BAND_REGION_8_CHANNEL_LIST;
				break;
			case REGION_9_A_BAND:
				num = sizeof(A_BAND_REGION_9_CHANNEL_LIST)/sizeof(UCHAR);
				pChannelList = A_BAND_REGION_9_CHANNEL_LIST;
				break;

			case REGION_10_A_BAND:
				num = sizeof(A_BAND_REGION_10_CHANNEL_LIST)/sizeof(UCHAR);
				pChannelList = A_BAND_REGION_10_CHANNEL_LIST;
				break;

			case REGION_11_A_BAND:
				num = sizeof(A_BAND_REGION_11_CHANNEL_LIST)/sizeof(UCHAR);
				pChannelList = A_BAND_REGION_11_CHANNEL_LIST;
				break;	

			default:            // Error. should never happen
				DBGPRINT(RT_DEBUG_WARN,("countryregion=%d not support", pAd->CommonCfg.CountryRegionForABand));
				break;
		}

		if (num != 0)
		{
			UCHAR RadarCh[15]={52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140};
			for (i=0; i<num; i++)
			{
				for (j=0; j<MAX_NUM_OF_CHANNELS; j++)
				{
					if (pChannelList[i] == pAd->TxPower[j].Channel)
						NdisMoveMemory(&pAd->ChannelList[index+i], &pAd->TxPower[j], sizeof(CHANNEL_TX_POWER));
					}
				for (j=0; j<15; j++)
				{
					if (pChannelList[i] == RadarCh[j])
						pAd->ChannelList[index+i].DfsReq = TRUE;
				}
				pAd->ChannelList[index+i].MaxTxPwr = 20;
			}
			index += num;
		}
	}

	pAd->ChannelListNum = index;	
	DBGPRINT(RT_DEBUG_TRACE,("country code=%d/%d, RFIC=%d, PHY mode=%d, support %d channels\n", 
		pAd->CommonCfg.CountryRegion, pAd->CommonCfg.CountryRegionForABand, pAd->RfIcType, pAd->CommonCfg.PhyMode, pAd->ChannelListNum));
#ifdef DBG	
	for (i=0;i<pAd->ChannelListNum;i++)
	{
		DBGPRINT_RAW(RT_DEBUG_TRACE,("BuildChannel # %d :: Pwr0 = %d, Pwr1 =%d, \n ", pAd->ChannelList[i].Channel, pAd->ChannelList[i].Power, pAd->ChannelList[i].Power2));
	}
#endif
}

/* 
	==========================================================================
	Description:
		This routine return the first channel number according to the country 
		code selection and RF IC selection (signal band or dual band). It is called
		whenever driver need to start a site survey of all supported channels.
	Return:
		ch - the first channel number of current country code setting

	IRQL = PASSIVE_LEVEL

	==========================================================================
 */
UCHAR FirstChannel(
	IN PRTMP_ADAPTER pAd)
{
	return pAd->ChannelList[0].Channel;
}

/* 
	==========================================================================
	Description:
		This routine returns the next channel number. This routine is called
		during driver need to start a site survey of all supported channels.
	Return:
		next_channel - the next channel number valid in current country code setting.
	Note:
		return 0 if no more next channel
	==========================================================================
 */
UCHAR NextChannel(
	IN PRTMP_ADAPTER pAd, 
	IN UCHAR channel)
{
	int i;
	UCHAR next_channel = 0;
			
	for (i = 0; i < (pAd->ChannelListNum - 1); i++)
		if (channel == pAd->ChannelList[i].Channel)
		{
			next_channel = pAd->ChannelList[i+1].Channel;
			break;
	}
	return next_channel;
}

/* 
	==========================================================================
	Description:
		This routine is for Cisco Compatible Extensions 2.X 
		Spec31. AP Control of Client Transmit Power
	Return:
		None
	Note:
	   Required by Aironet dBm(mW)
		   0dBm(1mW),   1dBm(5mW), 13dBm(20mW), 15dBm(30mW),
		  17dBm(50mw), 20dBm(100mW)

	   We supported 
		   3dBm(Lowest), 6dBm(10%), 9dBm(25%), 12dBm(50%),
		  14dBm(75%),   15dBm(100%)

		The client station's actual transmit power shall be within +/- 5dB of
		the minimum value or next lower value.
	==========================================================================
 */
VOID ChangeToCellPowerLimit(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR         AironetCellPowerLimit)
{
	//valud 0xFF means that hasn't found power limit information 
	//from the AP's Beacon/Probe response.
	if (AironetCellPowerLimit == 0xFF)
		return;  
	
	if (AironetCellPowerLimit < 6) //Used Lowest Power Percentage.
		pAd->CommonCfg.TxPowerPercentage = 6; 
	else if (AironetCellPowerLimit < 9)
		pAd->CommonCfg.TxPowerPercentage = 10;
	else if (AironetCellPowerLimit < 12)
		pAd->CommonCfg.TxPowerPercentage = 25;
	else if (AironetCellPowerLimit < 14)
		pAd->CommonCfg.TxPowerPercentage = 50;
	else if (AironetCellPowerLimit < 15)
		pAd->CommonCfg.TxPowerPercentage = 75;
	else
		pAd->CommonCfg.TxPowerPercentage = 100; //else used maximum

	if (pAd->CommonCfg.TxPowerPercentage > pAd->CommonCfg.TxPowerDefault)
		pAd->CommonCfg.TxPowerPercentage = pAd->CommonCfg.TxPowerDefault;
	
}

CHAR	ConvertToRssi(
	IN PRTMP_ADAPTER pAd,
	IN	CHAR			Rssi,
	IN  UCHAR   RssiNumber)
{
	UCHAR	RssiOffset, LNAGain;

	// Rssi equals to zero should be an invalid value
	if (Rssi == 0)
		return -99;

	LNAGain = GET_LNA_GAIN(pAd);
    if (pAd->LatchRfRegs.Channel > 14)
    {
        if (RssiNumber == 0)
			RssiOffset = pAd->ARssiOffset0;
		else if (RssiNumber == 1)
			RssiOffset = pAd->ARssiOffset1;
		else
			RssiOffset = pAd->ARssiOffset2;
    }
    else
    {
        if (RssiNumber == 0)
			RssiOffset = pAd->BGRssiOffset0;
		else if (RssiNumber == 1)
			RssiOffset = pAd->BGRssiOffset1;
		else
			RssiOffset = pAd->BGRssiOffset2;
    }
	
    return (-12 - RssiOffset - LNAGain - Rssi);
}

/*
	==========================================================================
	Description:
		Scan next channel
	==========================================================================
 */
VOID ScanNextChannel(
	IN PRTMP_ADAPTER pAd) 
{
	HEADER_802_11   Hdr80211;
	PUCHAR          pOutBuffer = NULL;
	NDIS_STATUS     NStatus;
	ULONG           FrameLen = 0;
	UCHAR           SsidLen = 0, ScanType = pAd->MlmeAux.ScanType, BBPValue = 0;
	UINT			ScanTimeIn5gChannel = SHORT_CHANNEL_TIME;


#ifdef RALINK_ATE
	// Nothing to do in ATE mode. 
	if (ATE_ON(pAd))
		return;
#endif // RALINK_ATE //

	if (pAd->MlmeAux.Channel == 0) 
	{
		if ((pAd->CommonCfg.BBPCurrentBW == BW_40)
			)
		{
			AsicSwitchChannel(pAd, pAd->CommonCfg.CentralChannel, FALSE);
			AsicLockChannel(pAd, pAd->CommonCfg.CentralChannel);
			RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &BBPValue);
			BBPValue &= (~0x18);
			BBPValue |= 0x10;
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, BBPValue);
			DBGPRINT(RT_DEBUG_TRACE, ("SYNC - End of SCAN, restore to 40MHz channel %d, Total BSS[%02d]\n",pAd->CommonCfg.CentralChannel, pAd->ScanTab.BssNr));
		}
		else
		{
			AsicSwitchChannel(pAd, pAd->CommonCfg.Channel, FALSE);
			AsicLockChannel(pAd, pAd->CommonCfg.Channel);
			DBGPRINT(RT_DEBUG_TRACE, ("SYNC - End of SCAN, restore to channel %d, Total BSS[%02d]\n",pAd->CommonCfg.Channel, pAd->ScanTab.BssNr));
		}
		

#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
			pAd->Mlme.ApSyncMachine.CurrState = AP_SYNC_IDLE;
			RTMPResumeMsduTransmission(pAd);
			if (!((pAd->CommonCfg.Channel > 14) && (pAd->CommonCfg.bIEEE80211H == TRUE)))
			{
				AsicEnableBssSync(pAd);
			}
		}
#endif // CONFIG_AP_SUPPORT //

		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS);
	} 
#ifdef RT2870
#endif // RT2870 //
	else 
	{

		AsicSwitchChannel(pAd, pAd->MlmeAux.Channel, TRUE);
		AsicLockChannel(pAd, pAd->MlmeAux.Channel);


		//Global country domain(ch1-11:active scan, ch12-14 passive scan)
		if ((pAd->MlmeAux.Channel <= 14) && (pAd->MlmeAux.Channel >= 12) && ((pAd->CommonCfg.CountryRegion & 0x7f) == REGION_31_BG_BAND))
		{
			ScanType = SCAN_PASSIVE;
		}

		// We need to shorten active scan time in order for WZC connect issue
		// Chnage the channel scan time for CISCO stuff based on its IAPP announcement
		if (ScanType == FAST_SCAN_ACTIVE)
			RTMPSetTimer(&pAd->MlmeAux.ScanTimer, FAST_ACTIVE_SCAN_TIME);
		else // must be SCAN_PASSIVE or SCAN_ACTIVE
		{
			if ((pAd->CommonCfg.PhyMode == PHY_11ABG_MIXED) 
			)
			{
				if (pAd->MlmeAux.Channel > 14)
					RTMPSetTimer(&pAd->MlmeAux.ScanTimer, ScanTimeIn5gChannel);
				else	
				RTMPSetTimer(&pAd->MlmeAux.ScanTimer, MIN_CHANNEL_TIME);
			}
			else
				RTMPSetTimer(&pAd->MlmeAux.ScanTimer, MAX_CHANNEL_TIME);
		}

		if ((ScanType == SCAN_ACTIVE) || (ScanType == FAST_SCAN_ACTIVE) ||
#ifdef WSC_STA_SUPPORT
			((ScanType == SCAN_WSC_ACTIVE) && (pAd->OpMode == OPMODE_STA)) ||
#endif // WSC_STA_SUPPORT //
			(ScanType == SCAN_CISCO_ACTIVE))
		{
			NStatus = MlmeAllocateMemory(pAd, &pOutBuffer);  //Get an unused nonpaged memory
			if (NStatus != NDIS_STATUS_SUCCESS)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("SYNC - ScanNextChannel() allocate memory fail\n"));

#ifdef CONFIG_AP_SUPPORT
				IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
					pAd->Mlme.ApSyncMachine.CurrState = AP_SYNC_IDLE;
#endif // CONFIG_AP_SUPPORT //
				return;
			}

			// There is no need to send broadcast probe request if active scan is in effect.
			if ((ScanType == SCAN_ACTIVE) || (ScanType == FAST_SCAN_ACTIVE)
#ifdef WSC_STA_SUPPORT
				|| ((ScanType == SCAN_WSC_ACTIVE) && (pAd->OpMode == OPMODE_STA))
#endif // WSC_STA_SUPPORT //
				)
				SsidLen = pAd->MlmeAux.SsidLen;
			else
				SsidLen = 0;

			MgtMacHeaderInit(pAd, &Hdr80211, SUBTYPE_PROBE_REQ, 0, BROADCAST_ADDR, BROADCAST_ADDR);
			MakeOutgoingFrame(pOutBuffer,               &FrameLen,
							  sizeof(HEADER_802_11),    &Hdr80211,
							  1,                        &SsidIe,
							  1,                        &SsidLen,
							  SsidLen,			        pAd->MlmeAux.Ssid,
							  1,                        &SupRateIe,
							  1,                        &pAd->CommonCfg.SupRateLen,
							  pAd->CommonCfg.SupRateLen,  pAd->CommonCfg.SupRate, 
							  END_OF_ARGS);

			if (pAd->CommonCfg.ExtRateLen)
			{
				ULONG Tmp;
				MakeOutgoingFrame(pOutBuffer + FrameLen,            &Tmp,
								  1,                                &ExtRateIe,
								  1,                                &pAd->CommonCfg.ExtRateLen,
								  pAd->CommonCfg.ExtRateLen,          pAd->CommonCfg.ExtRate, 
								  END_OF_ARGS);
				FrameLen += Tmp;
			}


#ifdef WSC_STA_SUPPORT
			if (pAd->OpMode == OPMODE_STA)
			{
				// Append WSC information in probe request if WSC state is running
				if ((pAd->StaCfg.WscControl.WscEnProbeReqIE) && (pAd->StaCfg.WscControl.WscConfMode != WSC_DISABLE))
				{
					UCHAR		WscBuf[256], WscIeLen = 0;
					ULONG 		WscTmpLen = 0;
				
					NdisZeroMemory(WscBuf, 256);
					WscMakeProbeReqIE(pAd, &WscBuf[0], &WscIeLen);

					MakeOutgoingFrame(pOutBuffer + FrameLen,              &WscTmpLen,
									WscIeLen,                             &WscBuf,
									END_OF_ARGS);

					FrameLen += WscTmpLen;
				}
			}
#endif // WSC_STA_SUPPORT //

			MiniportMMRequest(pAd, 0, pOutBuffer, FrameLen);
			MlmeFreeMemory(pAd, pOutBuffer);
		}

		// For SCAN_CISCO_PASSIVE, do nothing and silently wait for beacon or other probe reponse
		
#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
			pAd->Mlme.ApSyncMachine.CurrState = AP_SCAN_LISTEN;
#endif // CONFIG_AP_SUPPORT //

	}
}

VOID MgtProbReqMacHeaderInit(
	IN	PRTMP_ADAPTER	pAd, 
	IN OUT PHEADER_802_11 pHdr80211, 
	IN UCHAR SubType, 
	IN UCHAR ToDs, 
	IN PUCHAR pDA, 
	IN PUCHAR pBssid) 
{
	NdisZeroMemory(pHdr80211, sizeof(HEADER_802_11));
	
	pHdr80211->FC.Type = BTYPE_MGMT;
	pHdr80211->FC.SubType = SubType;
	if (SubType == SUBTYPE_ACK)
		pHdr80211->FC.Type = BTYPE_CNTL;
	pHdr80211->FC.ToDs = ToDs;
	COPY_MAC_ADDR(pHdr80211->Addr1, pDA);
	COPY_MAC_ADDR(pHdr80211->Addr2, pAd->CurrentAddress);
	COPY_MAC_ADDR(pHdr80211->Addr3, pBssid);
}

#ifdef CONFIG_AP_SUPPORT
VOID Send_Probe_Request(
	IN PRTMP_ADAPTER pAd) 
{
	HEADER_802_11   Hdr80211;
	PUCHAR          pOutBuffer = NULL;
	NDIS_STATUS     NStatus;
	ULONG           FrameLen = 0;
	UCHAR           SsidLen = 0; 

	{
   			NStatus = MlmeAllocateMemory(pAd, &pOutBuffer);  //Get an unused nonpaged memory
			if (NStatus != NDIS_STATUS_SUCCESS)
			{
				return;
			}

			SsidLen = 0;

			MgtProbReqMacHeaderInit(pAd, &Hdr80211, SUBTYPE_PROBE_REQ, 0, BROADCAST_ADDR, BROADCAST_ADDR);
			MakeOutgoingFrame(pOutBuffer,               &FrameLen,
							  sizeof(HEADER_802_11),    &Hdr80211,
							  1,                        &SsidIe,
							  1,                        &SsidLen,
							  SsidLen,			        pAd->MlmeAux.Ssid,
							  1,                        &SupRateIe,
							  1,                        &pAd->CommonCfg.SupRateLen,
							  pAd->CommonCfg.SupRateLen,  pAd->CommonCfg.SupRate, 
							  END_OF_ARGS);

			if (pAd->CommonCfg.ExtRateLen)
			{
				ULONG Tmp;
				MakeOutgoingFrame(pOutBuffer + FrameLen,            &Tmp,
								  1,                                &ExtRateIe,
								  1,                                &pAd->CommonCfg.ExtRateLen,
								  pAd->CommonCfg.ExtRateLen,          pAd->CommonCfg.ExtRate, 
								  END_OF_ARGS);
				FrameLen += Tmp;
			}


			MiniportMMRequest(pAd, 0, pOutBuffer, FrameLen);
			MlmeFreeMemory(pAd, pOutBuffer);
		}
}



#endif // CONFIG_AP_SUPPORT //

