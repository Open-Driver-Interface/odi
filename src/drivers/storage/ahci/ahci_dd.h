#ifndef _AHCI_DD_H 
#define _AHCI_DD_H
#include "../../../core/types.h"

//Structures
struct ahci_sata_ident_test
{
    u16   config;      /* lots of obsolete bit flags */
    u16   cyls;      /* obsolete */
    u16   reserved2;   /* special config */
    u16   heads;      /* "physical" heads */
    u16   track_bytes;   /* unformatted bytes per track */
    u16   sector_bytes;   /* unformatted bytes per sector */
    u16   sectors;   /* "physical" sectors per track */
    u16   vendor0;   /* vendor unique */
    u16   vendor1;   /* vendor unique */
    u16   vendor2;   /* vendor unique */
    u8   serial_no[20];   /* 0 = not_specified */
    u16   buf_type;
    u16   buf_size;   /* 512 byte increments; 0 = not_specified */
    u16   ecc_bytes;   /* for r/w long cmds; 0 = not_specified */
    u8   fw_rev[8];   /* 0 = not_specified */
    u8   model[40];   /* 0 = not_specified */
    u16  multi_count; /* Multiple Count */
    u16   dword_io;   /* 0=not_implemented; 1=implemented */
    u16   capability1;   /* vendor unique */
    u16   capability2;   /* bits 0:DMA 1:LBA 2:IORDYsw 3:IORDYsup word: 50 */
    u8   vendor5;   /* vendor unique */
    u8   tPIO;      /* 0=slow, 1=medium, 2=fast */
    u8   vendor6;   /* vendor unique */
    u8   tDMA;      /* 0=slow, 1=medium, 2=fast */
    u16   field_valid;   /* bits 0:cur_ok 1:eide_ok */
    u16   cur_cyls;   /* logical cylinders */
    u16   cur_heads;   /* logical heads word 55*/
    u16   cur_sectors;   /* logical sectors per track */
    u16   cur_capacity0;   /* logical total sectors on drive */
    u16   cur_capacity1;   /*  (2 words, misaligned int)     */
    u8   multsect;   /* current multiple sector count */
    u8   multsect_valid;   /* when (bit0==1) multsect is ok */
    u32   lba_capacity;   /* total number of sectors */
    u16   dma_1word;   /* single-word dma info */
    u16   dma_mword;   /* multiple-word dma info */
    u16  eide_pio_modes; /* bits 0:mode3 1:mode4 */
    u16  eide_dma_min;   /* min mword dma cycle time (ns) */
    u16  eide_dma_time;   /* recommended mword dma cycle time (ns) */
    u16  eide_pio;       /* min cycle time (ns), no IORDY  */
    u16  eide_pio_iordy; /* min cycle time (ns), with IORDY */
    u16   words69_70[2];   /* reserved words 69-70 */
    u16   words71_74[4];   /* reserved words 71-74 */
    u16  queue_depth;   /*  */
    u16  sata_capability;   /*  SATA Capabilities word 76*/
    u16  sata_additional;   /*  Additional Capabilities */
    u16  sata_supported;   /* SATA Features supported  */
    u16  features_enabled;   /* SATA features enabled */
    u16  major_rev_num;   /*  Major rev number word 80 */
    u16  minor_rev_num;   /*  */
    u16  command_set_1;   /* bits 0:Smart 1:Security 2:Removable 3:PM */
    u16  command_set_2;   /* bits 14:Smart Enabled 13:0 zero */
    u16  cfsse;      /* command set-feature supported extensions */
    u16  cfs_enable_1;   /* command set-feature enabled */
    u16  cfs_enable_2;   /* command set-feature enabled */
    u16  csf_default;   /* command set-feature default */
    u16  dma_ultra;   /*  */
    u16   word89;      /* reserved (word 89) */
    u16   word90;      /* reserved (word 90) */
    u16   CurAPMvalues;   /* current APM values */
    u16   word92;         /* reserved (word 92) */
    u16   comreset;      /* should be cleared to 0 */
    u16  accoustic;      /*  accoustic management */
    u16  min_req_sz;      /* Stream minimum required size */
    u16  transfer_time_dma;   /* Streaming Transfer Time-DMA */
    u16  access_latency;      /* Streaming access latency-DMA & PIO WORD 97*/
    u32    perf_granularity;   /* Streaming performance granularity */
    u32   total_usr_sectors[2]; /* Total number of user addressable sectors */
    u16   transfer_time_pio;    /* Streaming Transfer time PIO */
    u16   reserved105;       /* Word 105 */
    u16   sector_sz;          /* Puysical Sector size / Logical sector size */
    u16   inter_seek_delay;   /* In microseconds */
    u16   words108_116[9];    /*  */
    u32   words_per_sector;    /* words per logical sectors */
    u16   supported_settings; /* continued from words 82-84 */
    u16   command_set_3;       /* continued from words 85-87 */
    u16  words121_126[6];   /* reserved words 121-126 */
    u16   word127;         /* reserved (word 127) */
    u16   security_status;   /* device lock function
                 * 15:9   reserved
                 * 8   security level 1:max 0:high
                 * 7:6   reserved
                 * 5   enhanced erase
                 * 4   expire
                 * 3   frozen
                 * 2   locked
                 * 1   en/disabled
                 * 0   capability
                 */
    u16  csfo;      /* current set features options
                 * 15:4   reserved
                 * 3   auto reassign
                 * 2   reverting
                 * 1   read-look-ahead
                 * 0   write cache
                 */
    u16   words130_155[26];/* reserved vendor words 130-155 */
    u16   word156;
    u16   words157_159[3];/* reserved vendor words 157-159 */
    u16   cfa; /* CFA Power mode 1 */
    u16   words161_175[15]; /* Reserved */
    u8   media_serial[60]; /* words 176-205 Current Media serial number */
    u16   sct_cmd_transport; /* SCT Command Transport */
    u16   words207_208[2]; /* reserved */
    u16   block_align; /* Alignement of logical blocks in larger physical blocks */
    u32   WRV_sec_count; /* Write-Read-Verify sector count mode 3 only */
    u32      verf_sec_count; /* Verify Sector count mode 2 only */
    u16   nv_cache_capability; /* NV Cache capabilities */
    u16   nv_cache_sz; /* NV Cache size in logical blocks */
    u16   nv_cache_sz2; /* NV Cache size in logical blocks */
    u16   rotation_rate; /* Nominal media rotation rate */
    u16   reserved218; /*  */
    u16   nv_cache_options; /* NV Cache options */
    u16   words220_221[2]; /* reserved */
    u16   transport_major_rev; /*  */
    u16   transport_minor_rev; /*  */
    u16   words224_233[10]; /* Reserved */
    u16   min_dwnload_blocks; /* Minimum number of 512byte units per DOWNLOAD MICROCODE
                                command for mode 03h */
    u16   max_dwnload_blocks; /* Maximum number of 512byte units per DOWNLOAD MICROCODE
                                command for mode 03h */
    u16   words236_254[19];   /* Reserved */
    u16   integrity;          /* Cheksum, Signature */
} __attribute__((packed));

//https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/ata/ns-ata-_identify_device_data
struct ahci_sata_ident {
    u16 GeneralConfiguration_Reserved1 : 1;
    u16 GeneralConfiguration_Retired3 : 1;
    u16 GeneralConfiguration_ResponseIncomplete : 1;
    u16 GeneralConfiguration_Retired2 : 3;
    u16 GeneralConfiguration_FixedDevice : 1;
    u16 GeneralConfiguration_RemovableMedia : 1;
    u16 GeneralConfiguration_Retired1 : 7;
    u16 GeneralConfiguration_DeviceType : 1;
    u16 NumCylinders;
    u16 SpecificConfiguration;
    u16 NumHeads;
    u16 Retired1[2];
    u16 NumSectorsPerTrack;
    u16 VendorUnique1[3];
    u8  SerialNumber[20];
    u16 Retired2[2];
    u16 Obsolete1;
    u8  FirmwareRevision[8];
    u8  ModelNumber[40];
    u8  MaximumBlockTransfer;
    u8  VendorUnique2;
    u16 TrustedComputing_FeatureSupported : 1;
    u16 TrustedComputing_Reserved : 15;
    u8  Capabilities_CurrentLongPhysicalSectorAlignment : 2;
    u8  Capabilities_ReservedByte49 : 6;
    u8  Capabilities_DmaSupported : 1;
    u8  Capabilities_LbaSupported : 1;
    u8  Capabilities_IordyDisable : 1;
    u8  Capabilities_IordySupported : 1;
    u8  Capabilities_Reserved1 : 1;
    u8  Capabilities_StandybyTimerSupport : 1;
    u8  Capabilities_Reserved2 : 2;
    u16 Capabilities_ReservedWord50;
    u16 ObsoleteWords51[2];
    u16 TranslationFieldsValid : 3;
    u16 Reserved3 : 5;
    u16 FreeFallControlSensitivity : 8;
    u16 NumberOfCurrentCylinders;
    u16 NumberOfCurrentHeads;
    u16 CurrentSectorsPerTrack;
    u64  CurrentSectorCapacity;
    u8  CurrentMultiSectorSetting;
    u8  MultiSectorSettingValid : 1;
    u8  ReservedByte59 : 3;
    u8  SanitizeFeatureSupported : 1;
    u8  CryptoScrambleExtCommandSupported : 1;
    u8  OverwriteExtCommandSupported : 1;
    u8  BlockEraseExtCommandSupported : 1;
    u64  UserAddressableSectors;
    u16 ObsoleteWord62;
    u16 MultiWordDMASupport : 8;
    u16 MultiWordDMAActive : 8;
    u16 AdvancedPIOModes : 8;
    u16 ReservedByte64 : 8;
    u16 MinimumMWXferCycleTime;
    u16 RecommendedMWXferCycleTime;
    u16 MinimumPIOCycleTime;
    u16 MinimumPIOCycleTimeIORDY;
    u16 AdditionalSupported_ZonedCapabilities : 2;
    u16 AdditionalSupported_NonVolatileWriteCache : 1;
    u16 AdditionalSupported_ExtendedUserAddressableSectorsSupported : 1;
    u16 AdditionalSupported_DeviceEncryptsAllUserData : 1;
    u16 AdditionalSupported_ReadZeroAfterTrimSupported : 1;
    u16 AdditionalSupported_Optional28BitCommandsSupported : 1;
    u16 AdditionalSupported_IEEE1667 : 1;
    u16 AdditionalSupported_DownloadMicrocodeDmaSupported : 1;
    u16 AdditionalSupported_SetMaxSetPasswordUnlockDmaSupported : 1;
    u16 AdditionalSupported_WriteBufferDmaSupported : 1;
    u16 AdditionalSupported_ReadBufferDmaSupported : 1;
    u16 AdditionalSupported_DeviceConfigIdentifySetDmaSupported : 1;
    u16 AdditionalSupported_LPSAERCSupported : 1;
    u16 AdditionalSupported_DeterministicReadAfterTrimSupported : 1;
    u16 AdditionalSupported_CFastSpecSupported : 1;
    u16 ReservedWords70[5];
    u16 QueueDepth : 5;
    u16 ReservedWord75 : 11;
    u16 SerialAtaCapabilities_Reserved0 : 1;
    u16 SerialAtaCapabilities_SataGen1 : 1;
    u16 SerialAtaCapabilities_SataGen2 : 1;
    u16 SerialAtaCapabilities_SataGen3 : 1;
    u16 SerialAtaCapabilities_Reserved1 : 4;
    u16 SerialAtaCapabilities_NCQ : 1;
    u16 SerialAtaCapabilities_HIPM : 1;
    u16 SerialAtaCapabilities_PhyEvents : 1;
    u16 SerialAtaCapabilities_NcqUnload : 1;
    u16 SerialAtaCapabilities_NcqPriority : 1;
    u16 SerialAtaCapabilities_HostAutoPS : 1;
    u16 SerialAtaCapabilities_DeviceAutoPS : 1;
    u16 SerialAtaCapabilities_ReadLogDMA : 1;
    u16 SerialAtaCapabilities_Reserved2 : 1;
    u16 SerialAtaCapabilities_CurrentSpeed : 3;
    u16 SerialAtaCapabilities_NcqStreaming : 1;
    u16 SerialAtaCapabilities_NcqQueueMgmt : 1;
    u16 SerialAtaCapabilities_NcqReceiveSend : 1;
    u16 SerialAtaCapabilities_DEVSLPtoReducedPwrState : 1;
    u16 SerialAtaCapabilities_Reserved3 : 8;
    u16 SerialAtaFeaturesSupported_Reserved0 : 1;
    u16 SerialAtaFeaturesSupported_NonZeroOffsets : 1;
    u16 SerialAtaFeaturesSupported_DmaSetupAutoActivate : 1;
    u16 SerialAtaFeaturesSupported_DIPM : 1;
    u16 SerialAtaFeaturesSupported_InOrderData : 1;
    u16 SerialAtaFeaturesSupported_HardwareFeatureControl : 1;
    u16 SerialAtaFeaturesSupported_SoftwareSettingsPreservation : 1;
    u16 SerialAtaFeaturesSupported_NCQAutosense : 1;
    u16 SerialAtaFeaturesSupported_DEVSLP : 1;
    u16 SerialAtaFeaturesSupported_HybridInformation : 1;
    u16 SerialAtaFeaturesSupported_Reserved1 : 6;
    u16 SerialAtaFeaturesEnabled_Reserved0 : 1;
    u16 SerialAtaFeaturesEnabled_NonZeroOffsets : 1;
    u16 SerialAtaFeaturesEnabled_DmaSetupAutoActivate : 1;
    u16 SerialAtaFeaturesEnabled_DIPM : 1;
    u16 SerialAtaFeaturesEnabled_InOrderData : 1;
    u16 SerialAtaFeaturesEnabled_HardwareFeatureControl : 1;
    u16 SerialAtaFeaturesEnabled_SoftwareSettingsPreservation : 1;
    u16 SerialAtaFeaturesEnabled_DeviceAutoPS : 1;
    u16 SerialAtaFeaturesEnabled_DEVSLP : 1;
    u16 SerialAtaFeaturesEnabled_HybridInformation : 1;
    u16 SerialAtaFeaturesEnabled_Reserved1 : 6;
    u16 MajorRevision;
    u16 MinorRevision;
    u16 CommandSetSupport_SmartCommands : 1;
    u16 CommandSetSupport_SecurityMode : 1;
    u16 CommandSetSupport_RemovableMediaFeature : 1;
    u16 CommandSetSupport_PowerManagement : 1;
    u16 CommandSetSupport_Reserved1 : 1;
    u16 CommandSetSupport_WriteCache : 1;
    u16 CommandSetSupport_LookAhead : 1;
    u16 CommandSetSupport_ReleaseInterrupt : 1;
    u16 CommandSetSupport_ServiceInterrupt : 1;
    u16 CommandSetSupport_DeviceReset : 1;
    u16 CommandSetSupport_HostProtectedArea : 1;
    u16 CommandSetSupport_Obsolete1 : 1;
    u16 CommandSetSupport_WriteBuffer : 1;
    u16 CommandSetSupport_ReadBuffer : 1;
    u16 CommandSetSupport_Nop : 1;
    u16 CommandSetSupport_Obsolete2 : 1;
    u16 CommandSetSupport_DownloadMicrocode : 1;
    u16 CommandSetSupport_DmaQueued : 1;
    u16 CommandSetSupport_Cfa : 1;
    u16 CommandSetSupport_AdvancedPm : 1;
    u16 CommandSetSupport_Msn : 1;
    u16 CommandSetSupport_PowerUpInStandby : 1;
    u16 CommandSetSupport_ManualPowerUp : 1;
    u16 CommandSetSupport_Reserved2 : 1;
    u16 CommandSetSupport_SetMax : 1;
    u16 CommandSetSupport_Acoustics : 1;
    u16 CommandSetSupport_BigLba : 1;
    u16 CommandSetSupport_DeviceConfigOverlay : 1;
    u16 CommandSetSupport_FlushCache : 1;
    u16 CommandSetSupport_FlushCacheExt : 1;
    u16 CommandSetSupport_WordValid83 : 2;
    u16 CommandSetSupport_SmartErrorLog : 1;
    u16 CommandSetSupport_SmartSelfTest : 1;
    u16 CommandSetSupport_MediaSerialNumber : 1;
    u16 CommandSetSupport_MediaCardPassThrough : 1;
    u16 CommandSetSupport_StreamingFeature : 1;
    u16 CommandSetSupport_GpLogging : 1;
    u16 CommandSetSupport_WriteFua : 1;
    u16 CommandSetSupport_WriteQueuedFua : 1;
    u16 CommandSetSupport_WWN64Bit : 1;
    u16 CommandSetSupport_URGReadStream : 1;
    u16 CommandSetSupport_URGWriteStream : 1;
    u16 CommandSetSupport_ReservedForTechReport : 2;
    u16 CommandSetSupport_IdleWithUnloadFeature : 1;
    u16 CommandSetSupport_WordValid : 2;
    u16 CommandSetActive_SmartCommands : 1;
    u16 CommandSetActive_SecurityMode : 1;
    u16 CommandSetActive_RemovableMediaFeature : 1;
    u16 CommandSetActive_PowerManagement : 1;
    u16 CommandSetActive_Reserved1 : 1;
    u16 CommandSetActive_WriteCache : 1;
    u16 CommandSetActive_LookAhead : 1;
    u16 CommandSetActive_ReleaseInterrupt : 1;
    u16 CommandSetActive_ServiceInterrupt : 1;
    u16 CommandSetActive_DeviceReset : 1;
    u16 CommandSetActive_HostProtectedArea : 1;
    u16 CommandSetActive_Obsolete1 : 1;
    u16 CommandSetActive_WriteBuffer : 1;
    u16 CommandSetActive_ReadBuffer : 1;
    u16 CommandSetActive_Nop : 1;
    u16 CommandSetActive_Obsolete2 : 1;
    u16 CommandSetActive_DownloadMicrocode : 1;
    u16 CommandSetActive_DmaQueued : 1;
    u16 CommandSetActive_Cfa : 1;
    u16 CommandSetActive_AdvancedPm : 1;
    u16 CommandSetActive_Msn : 1;
    u16 CommandSetActive_PowerUpInStandby : 1;
    u16 CommandSetActive_ManualPowerUp : 1;
    u16 CommandSetActive_Reserved2 : 1;
    u16 CommandSetActive_SetMax : 1;
    u16 CommandSetActive_Acoustics : 1;
    u16 CommandSetActive_BigLba : 1;
    u16 CommandSetActive_DeviceConfigOverlay : 1;
    u16 CommandSetActive_FlushCache : 1;
    u16 CommandSetActive_FlushCacheExt : 1;
    u16 CommandSetActive_Resrved3 : 1;
    u16 CommandSetActive_Words119_120Valid : 1;
    u16 CommandSetActive_SmartErrorLog : 1;
    u16 CommandSetActive_SmartSelfTest : 1;
    u16 CommandSetActive_MediaSerialNumber : 1;
    u16 CommandSetActive_MediaCardPassThrough : 1;
    u16 CommandSetActive_StreamingFeature : 1;
    u16 CommandSetActive_GpLogging : 1;
    u16 CommandSetActive_WriteFua : 1;
    u16 CommandSetActive_WriteQueuedFua : 1;
    u16 CommandSetActive_WWN64Bit : 1;
    u16 CommandSetActive_URGReadStream : 1;
    u16 CommandSetActive_URGWriteStream : 1;
    u16 CommandSetActive_ReservedForTechReport : 2;
    u16 CommandSetActive_IdleWithUnloadFeature : 1;
    u16 CommandSetActive_Reserved4 : 2;
    u16 UltraDMASupport : 8;
    u16 UltraDMAActive : 8;
    u16 NormalSecurityEraseUnit_TimeRequired : 15;
    u16 NormalSecurityEraseUnit_ExtendedTimeReported : 1;
    u16 EnhancedSecurityEraseUnit_TimeRequired : 15;
    u16 EnhancedSecurityEraseUnit_ExtendedTimeReported : 1;
    u16 CurrentAPMLevel : 8;
    u16 ReservedWord91 : 8;
    u16 MasterPasswordID;
    u16 HardwareResetResult;
    u16 CurrentAcousticValue : 8;
    u16 RecommendedAcousticValue : 8;
    u16 StreamMinRequestSize;
    u16 StreamingTransferTimeDMA;
    u16 StreamingAccessLatencyDMAPIO;
    u64  StreamingPerfGranularity;
    u64  Max48BitLBA[2];
    u16 StreamingTransferTime;
    u16 DsmCap;
    u16 PhysicalLogicalSectorSize_LogicalSectorsPerPhysicalSector : 4;
    u16 PhysicalLogicalSectorSize_Reserved0 : 8;
    u16 PhysicalLogicalSectorSize_LogicalSectorLongerThan256Words : 1;
    u16 PhysicalLogicalSectorSize_MultipleLogicalSectorsPerPhysicalSector : 1;
    u16 PhysicalLogicalSectorSize_Reserved1 : 2;
    u16 InterSeekDelay;
    u16 WorldWideName[4];
    u16 ReservedForWorldWideName128[4];
    u16 ReservedForTlcTechnicalReport;
    u16 WordsPerLogicalSector[2];
    u16 CommandSetSupportExt_ReservedForDrqTechnicalReport : 1;
    u16 CommandSetSupportExt_WriteReadVerify : 1;
    u16 CommandSetSupportExt_WriteUncorrectableExt : 1;
    u16 CommandSetSupportExt_ReadWriteLogDmaExt : 1;
    u16 CommandSetSupportExt_DownloadMicrocodeMode3 : 1;
    u16 CommandSetSupportExt_FreefallControl : 1;
    u16 CommandSetSupportExt_SenseDataReporting : 1;
    u16 CommandSetSupportExt_ExtendedPowerConditions : 1;
    u16 CommandSetSupportExt_Reserved0 : 6;
    u16 CommandSetSupportExt_WordValid : 2;
    u16 CommandSetActiveExt_ReservedForDrqTechnicalReport : 1;
    u16 CommandSetActiveExt_WriteReadVerify : 1;
    u16 CommandSetActiveExt_WriteUncorrectableExt : 1;
    u16 CommandSetActiveExt_ReadWriteLogDmaExt : 1;
    u16 CommandSetActiveExt_DownloadMicrocodeMode3 : 1;
    u16 CommandSetActiveExt_FreefallControl : 1;
    u16 CommandSetActiveExt_SenseDataReporting : 1;
    u16 CommandSetActiveExt_ExtendedPowerConditions : 1;
    u16 CommandSetActiveExt_Reserved0 : 6;
    u16 CommandSetActiveExt_Reserved1 : 2;
    u16 ReservedForExpandedSupportandActive[6];
    u16 MsnSupport : 2;
    u16 ReservedWord127 : 14;
    u16 SecurityStatus_SecuritySupported : 1;
    u16 SecurityStatus_SecurityEnabled : 1;
    u16 SecurityStatus_SecurityLocked : 1;
    u16 SecurityStatus_SecurityFrozen : 1;
    u16 SecurityStatus_SecurityCountExpired : 1;
    u16 SecurityStatus_EnhancedSecurityEraseSupported : 1;
    u16 SecurityStatus_Reserved0 : 2;
    u16 SecurityStatus_SecurityLevel : 1;
    u16 SecurityStatus_Reserved1 : 7;
    u16 ReservedWord129[31];
    u16 CfaPowerMode1_MaximumCurrentInMA : 12;
    u16 CfaPowerMode1_CfaPowerMode1Disabled : 1;
    u16 CfaPowerMode1_CfaPowerMode1Required : 1;
    u16 CfaPowerMode1_Reserved0 : 1;
    u16 CfaPowerMode1_Word160Supported : 1;
    u16 ReservedForCfaWord161[7];
    u16 NominalFormFactor : 4;
    u16 ReservedWord168 : 12;
    u16 DataSetManagementFeature_SupportsTrim : 1;
    u16 DataSetManagementFeature_Reserved0 : 15;
    u16 AdditionalProductID[4];
    u16 ReservedForCfaWord174[2];
    u16 CurrentMediaSerialNumber[30];
    u16 SCTCommandTransport_Supported : 1;
    u16 SCTCommandTransport_Reserved0 : 1;
    u16 SCTCommandTransport_WriteSameSuported : 1;
    u16 SCTCommandTransport_ErrorRecoveryControlSupported : 1;
    u16 SCTCommandTransport_FeatureControlSuported : 1;
    u16 SCTCommandTransport_DataTablesSuported : 1;
    u16 SCTCommandTransport_Reserved1 : 6;
    u16 SCTCommandTransport_VendorSpecific : 4;
    u16 ReservedWord207[2];
    u16 BlockAlignment_AlignmentOfLogicalWithinPhysical : 14;
    u16 BlockAlignment_Word209Supported : 1;
    u16 BlockAlignment_Reserved0 : 1;
    u16 WriteReadVerifySectorCountMode3Only[2];
    u16 WriteReadVerifySectorCountMode2Only[2];
    u16 NVCacheCapabilities_NVCachePowerModeEnabled : 1;
    u16 NVCacheCapabilities_Reserved0 : 3;
    u16 NVCacheCapabilities_NVCacheFeatureSetEnabled : 1;
    u16 NVCacheCapabilities_Reserved1 : 3;
    u16 NVCacheCapabilities_NVCachePowerModeVersion : 4;
    u16 NVCacheCapabilities_NVCacheFeatureSetVersion : 4;
    u16 NVCacheSizeLSW;
    u16 NVCacheSizeMSW;
    u16 NominalMediaRotationRate;
    u16 ReservedWord218;
    u8 NVCacheOptions_NVCacheEstimatedTimeToSpinUpInSeconds;
    u8 NVCacheOptions_Reserved;
    u16 WriteReadVerifySectorCountMode : 8;
    u16 ReservedWord220 : 8;
    u16 ReservedWord221;
    u16 TransportMajorVersion_MajorVersion : 12;
    u16 TransportMajorVersion_TransportType : 4;
    u16 TransportMinorVersion;
    u16 ReservedWord224[6];
    u64  ExtendedNumberOfUserAddressableSectors[2];
    u16 MinBlocksPerDownloadMicrocodeMode03;
    u16 MaxBlocksPerDownloadMicrocodeMode03;
    u16 ReservedWord236[19];
    u16 Signature : 8;
    u16 CheckSum : 8;
} __attribute__((packed));

//Configuration for the ahci driver.
#define MAX_AHCI_DEVICES 32
#define MAX_AHCI_PORTS_PER_DEVICE 32
#define MAX_AHCI_PORTS (MAX_AHCI_DEVICES * MAX_AHCI_PORTS_PER_DEVICE)

//Ioctl codes
#define AHCI_IOCTL_ATAPI_IDENTIFY   0x01
#define AHCI_IOCTL_INIT             0x02
#define AHCI_IOCTL_CTRL_SYNC        0x03
#define AHCI_IOCTL_CTRL_TRIM        0x04
#define AHCI_IOCTL_GET_SECTOR_SIZE  0x05
#define AHCI_IOCTL_GET_SECTOR_COUNT 0x06
#define AHCI_IOCTL_GET_BLOCK_SIZE   0x07

//This is required, export the driver name, major number
//and license type. This is used by the driver manager.
#define AHCI_DD_LICENSE             ODI_LICENSE_TYPE_MIT
#define AHCI_DD_VENDOR              "OPENDRIVERINTERFACE\0"
#define AHCI_DD_NAME                "AHCI DRIVER\0"
#define AHCI_DD_NAME_ATA            "AHCI ATA DRIVER\0"
#define AHCI_DD_NAME_ATAPI          "AHCI ATAPI DRIVER\0"
#define AHCI_DD_MAJOR               252
#define AHCI_DD_MAJOR_ATA           8
#define ACHI_DD_MAJOR_ATAPI         9

//This is a way to pack the initialization and exit
//routines of the driver. It is not required.
void ahci_dd_init(void);
void ahci_dd_exit(void);
#endif
