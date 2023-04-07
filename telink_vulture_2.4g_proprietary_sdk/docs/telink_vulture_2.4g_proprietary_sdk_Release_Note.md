## V3.4.4

### Version
* SDK version : telink_vulture_2.4g_proprietary_sdk V3.4.4
* This version of SDK supports B87 chip.
* The default configuration of LEDs and KEYs match the following hardware revision:
*	B87  	C1T197A30_V1_2

### Features

* 修改docs文件夹和一些标注。

### Bug Fixes
* Solve the problem that  gotten  payload length is packet length but not real payload length in General fsk fix packet format.
* Solve the problem of ble_ adv routine connection failure that causesd by driver updating rf step from 0.5 to 1.

### Refactoring

* N/A

### BREAKING CHANGES

* N/A

## V3.4.4

### Version
* SDK 版本: telink_vulture_2.4g_proprietary_sdk V3.4.4
* 此版本SDK支持B87芯片。
* LED和KEY的默认配置匹配以下硬件版本:
*	B87  	C1T197A30_V1_2

### Features

* change docs files and some info.

### Bug Fixes
* 解决General fsk定长包获取的rx_payload_len为packet length而不是实际payload length的问题。
* 解决驱动更新rf step从0.5变为1导致ble_adv例程连接失败问题。

### Refactoring

* N/A

### BREAKING CHANGES

* N/A

## V3.4.3

### Version
* SDK version : telink_vulture_2.4g_proprietary_sdk V3.4.3
* This version of SDK supports B87 chip.
* The default configuration of LEDs and KEYs match the following hardware revision:
*	B87  	C1T197A30_V1_2

### Features
* Add the crc function of the firmware update routine and put the relevant script files into the new script folder.
* Add stx/srx/stx2rx/srx2tx routines of tpsll（Telink proprietary stack link layer）.
* Add fast settle function to ptx/prx/stx/srx.

### Bug Fixes
* Sovle the clearing interrupt conflict between irq_handler and rf_tx_finish interface in ble_adv_tx routine .
* Sovle the problem of turning into bricks after power failure when firmware updating.

### Refactoring
* Put all non-public driver files in the drivers/lib folder.

### BREAKING CHANGES

* Change the name of tpll to tpll(Telink primary link layer).

## V3.4.3

### Version
* SDK 版本: telink_vulture_2.4g_proprietary_sdk V3.4.3
* 此版本SDK支持B87芯片。
* LED和KEY的默认配置匹配以下硬件版本:
*	B87  	C1T197A30_V1_2

### Features
* 添加固件更新例程的crc校验功能并将相关脚本文件放入新建的script文件夹中。
* 添加tpsll（Telink proprietary stack link layer）的stx/srx/stx2rx/srx2tx例程。
* 添加ptx/prx/stx/srx的fast settle功能。

### Bug Fixes
* 解决ble_adv_tx中rf_tx_finish接口清中断与中断函数清中断的冲突问题。
* 解决固件更新过程中断电变砖问题。

### Refactoring
* 将不公开驱动文件全部放入drivers/lib文件夹下。

### BREAKING CHANGES

* 将tpll更名为tpll(Telink primary link layer)。

## V3.4.2

### Version
* SDK version : telink_vulture_2.4g_proprietary_sdk V3.4.2
* This version of SDK supports B87 chip.
* The default configuration of LEDs and KEYs match the following hardware revision:
*	B87  	C1T197A30_V1_2

### Bug Fixes
* Modify rx_buf error problem in ptx GetRxPacket interface.

### BREAKING CHANGES

* N/A

## V3.4.2

### Version
* SDK 版本: telink_vulture_2.4g_proprietary_sdk V3.4.2
* 此版本SDK支持B87芯片。
* LED和KEY的默认配置匹配以下硬件版本:
*	B87  	C1T197A30_V1_2

### Bug Fixes
* 修改ptx中GetRxPacket接口里rx_buf错误问题.

### BREAKING CHANGES

* N/A

## V3.4.1

### Version
* SDK version : telink_vulture_2.4g_proprietary_sdk V3.4.1
* This version of SDK supports B87 chip.
* The default configuration of LEDs and KEYs match the following hardware revision:
*	B87  	C1T197A30_V1_2

### Features
* change channel step setting from one to 0.5.

### BREAKING CHANGES

* N/A

## V3.4.1

### Version
* SDK 版本: telink_vulture_2.4g_proprietary_sdk V3.4.1
* 此版本SDK支持B87芯片。
* LED和KEY的默认配置匹配以下硬件版本:
*	B87  	C1T197A30_V1_2

### Features
* 修改channel step setting from one to 0.5。

### BREAKING CHANGES

* N/A

## V3.4.0

### Version
* SDK version : telink_vulture_2.4g_proprietary_sdk V3.4.0
* This version of SDK supports B87 chip.
* The default configuration of LEDs and KEYs match the following hardware revision:
*	B87  	C1T197A30_V1_2

### Dependency Updates
* telink_b85m_driver_sdk_V1.5.0

### Features
* Add adc temperature test and random generation functions, and combine them with the original adc gpio sampling and adc Vbat sampling routines to add adc_sample routine.
* Add two-point calibration to adc optimization.
* Add required user_read_flash_value_calib interface after cpu_wakeup_init function for VDDF\ADC and frequency offset calibration.
* Add uart_soft_rx\pwm_count\pwm_ir\pwm_ir_dma_fifo\pwm_ir_fifo\suspend_core_wakeup routines.
* Add the deep retention wakeup function to deep_ gpio_ wakeup and deep_ 32k_ timer routines.
* Add flash operation models P25Q80U and update related routines.
* Add the functions of triggering interrupts on the rising or falling edge of gpio interrupts.
* Add. sdk_ The version section stores version information.
* The following interfaces are added in tpll_ll for software to judge new and old packages.
  * TPLL_EnableCrcfilter
  * TPLL_GetRxPacket
  * TPLL_GetRxPacketId
  * TPLL_GetRxPacketCrc
  * TPLL_IsCrcVaild
  * TPLL_IsPacketLenVaild
  * TPLL_IsPacketEmpty
* update shutdown tx ldo trim param in tpll.
* update TPLL_SetNewRFChannel interface in tpll.

### Bug Fixes
* solve set channel failed when use TPLL_SetNewRFChannel with too big step

### BREAKING CHANGES

* N/A

## V3.4.0

### Version
* SDK 版本: telink_vulture_2.4g_proprietary_sdk V3.4.0
* 此版本SDK支持B87芯片。
* LED和KEY的默认配置匹配以下硬件版本:
*	B87  	C1T197A30_V1_2

### Dependency Updates
* telink_b85m_driver_sdk_V1.5.0

### Features
* 新增adc温度测试与随机生成功能并与原有的adc的gpio采样、adc的Vbat采样例程合并至新增adc_sample例程中。
* 新增adc校准优化的两点校准。
* 在初始化cpu_wakeup_init函数后增加必需的新增user_read_flash_value_calib接口用于 VDDF、ADC、频偏校准。
* 新增uart_soft_rx\pwm_count\pwm_ir\pwm_ir_dma_fifo\pwm_ir_fifo\suspend_core_wakeup例程。
* 在deep_gpio_wakeup与deep_32k_timer_wakeup例程中添加deep retention wakeup功能。
* 新增flash操作型号P25Q80U并更新相关例程。
* 新增gpio上升沿或下降沿触发中断功能。
* 新增.sdk_version段存放版本信息。
* 在tpll中新增如下接口，用于软件判断新旧包。
  * TPLL_EnableCrcfilter
  * TPLL_GetRxPacket
  * TPLL_GetRxPacketId
  * TPLL_GetRxPacketCrc
  * TPLL_IsCrcVaild
  * TPLL_IsPacketLenVaild
  * TPLL_IsPacketEmpty
* 更新 tpll中shutdown tx ldo trim 参数。
* 更新tpll中的TPLL_SetNewRFChannel接口。

### Bug Fixes
* 解决TPLL_SetNewRFChannel接口因设置新通道跨度太大导致设置失败的问题。

### BREAKING CHANGES

* N/A

## V3.3.0

### Features

* Support zbit flash.

* For 24m clock calibration after waking up in deep_retention mode，called the API **rc_24m_cal** in the demo deep_32K_time_wakeup.

* Added the function of frequency offset calibration value writing to flash for customer reference in the demo ble_beacon demo. 

* Update and improve OTA demo function.

* The voltage detection content is added before the flash operation, and the minimum power supply voltage is 2.0V.

* Added the API **gen_fsk_tx_set_mi** and **gen_fsk_rx_set_mi** in the genfsk_ll.c/genfsk_ll.h

* Added the API **TPLL_Preamble_Detect_Disable** in the tpll\tpll.h to disable the receiver preamble detection banking duiring the first byte of pdu. The defulte value is disable in the SDK.

* The TX Settle time can be set to 108 us using the API **TPLL_TxSettleSet()**.

* Renamed the API **TPLL_SetDatarate()** to **TPLL_Init()** and added a new API **TPLL_SetBitrate()** to serve the purpose of setting the bitrate only in the tpll\tpll.h.

* Added the mode of **TPLL_BITRATE_500KBPS** and **TPLL_BITRATE_250KBPS** in the tpll\tpll.h.

* Added the API **GetVersion** and files of “version.c” && “version.h”.

* The APIs below return an error code if input parameter is out of valid range or the device is busy.


```c
int TPLL_RxWaitSet(unsigned short wait_us)
int TPLL_TxWaitSet(unsigned short wait_us)
int TPLL_TxSettleSet(unsigned short period_us)
int TPLL_RxSettleSet(unsigned short period_us)
int TPLL_RxTimeoutSet(unsigned short period_us);
int TPLL_PTXTrig(void);
int TPLL_PRXTrig(void);
```

* Added the following error codes.

```c
#define TLSR_SUCCESS                            (0)
#define TLSR_ERROR_INVALID_PARAM                (-1)
#define TLSR_ERROR_BUSY                         (-2)
```

* Added a new API **GetVersion()** which returns the SDK revision numbers.

### Bug Fixes

* Fixed the minor bugs in the TPLL_RxWaitSet() and TPLL_TxWaitSet() that the wait time is set to *(n + 1)* us if *n* is passed as the input parameter.

### BREAKING CHANGES

* The TX Settle time can be set to as low as 108 us instead of 113 us.
  
* The API **TPLL_Init()** sets the modulation index to 0.32 by default.
  
* The two enums **TPLL_BR_1M** and **TPLL_BR_2M** have been renamed to **TPLL_BITRATE_1MBPS** and **TPLL_BITRATE_2MBPS**.

* Renamed the API **TPLL_SetDatarate()** to **TPLL_Init()** to better match its intended purpose.

* Changed the function prototype of the API **TPLL_OpenPipe()** to the the following:
```c
  void TPLL_OpenPipe(TPLL_PipeIDTypeDef pipe_id);
```

* Deprecated and removed the following APIs:

```c
TPLL_GetAutoRetrStatus()
TPLL_GetRxPayloadWidth()
TPLL_GetTxFifoStatus()
TPLL_GetReuseTxStatus()
TPLL_WriteTxPayloadNoAck()
TPLL_SetRxPayloadWidth()
TPLL_ReadRxPayloadLength()
TPLL_SetupDynamicPayload()
TPLL_EnableAckPayload()
TPLL_EnableDynamicPayload()
TPLL_SetTimerPeriod()
TPLL_RandGen()
```
<hr style="border-bottom:2.5px solid rgb(146, 240, 161)">

### 版本
* SDK版本: telink_vulture_2.4g_proprietary_sdk V3.3.0.

### Features
* 支持Zbit flash。
* 在deep_32K_time_wakeup demo中增加了接口rc_24m_cal ();，用于deep_retention模式醒来后进行24M时钟校准。
* 在ble_beacon demo中增加了频偏校准写入flash的操作以供客户参考。
* 更新并完善OTA demo功能。
* 在使用flash的读写功能前加了电压检测内容，最低供电电压为2.0V。 
* 在driver中增加了rf_private.c和rf_private.h,增加了接口pri_set_bitrate。
* 在文件genfsk_ll.c/genfsk_ll.h中增加了gen_fsk_tx_set_mi 和 gen_fsk_rx_set_mi 两个接口。
* 在tpll\tpll.h.中增加了接口TPLL_Preamble_Detect_Disable用于关闭对于payload第一个字节即Header的preamble检测,避免因此丢包的可能, RF的配置中默认为disable。
* 使用接口TPLL_TxSettleSet()可将TX Settle时间设置为108us。
* 将API TPLL_SetDatarate()重命名为TPLL_Init()，并在tpll\tpll.h.中添加了一个新的API TPLL_SetBitrate() 以实现单独设置比特率的目的。
* 增加了"TPLLBITRATE_500KBPS" 和 "TPLL_BITRATE_250KBPS"两种模式。
* 增加了API"GetVersion" 和“version.c” 、 “version.h”两个文件.
* 当输入参数超过临界值或设备正忙，以下接口会返回一个错误值。
```c
int TPLL_RxWaitSet(unsigned short wait_us)
int TPLL_TxWaitSet(unsigned short wait_us)
int TPLL_TxSettleSet(unsigned short period_us)
int TPLL_RxSettleSet(unsigned short period_us)
int TPLL_RxTimeoutSet(unsigned short period_us);
int TPLL_PTXTrig(void);
int TPLL_PRXTrig(void);
```
* 增加了如下错误值

```c
#define TLSR_SUCCESS                            (0)
#define TLSR_ERROR_INVALID_PARAM                (-1)
#define TLSR_ERROR_BUSY                         (-2)
```
* 将API TPLL_SetDatarate()重命名为TPLL_Init()，并在tpll\tpll.h.中添加了一个新的API TPLL_SetBitrate() 以实现单独设置比特率的目的。
* 在drivers\8278\usbhw.h文件中添加了两个API usbhw_enable_endpoints()和 usbhw_disable_endpoints() 来启用或禁用端点。
### Bug Fixes
* 修复了TPLL_RxWaitSet()和TPLL_TxWaitSet()中的错误，即如果将*n*作为输入参数传递，则等待时间设置为*(n+1)*us。


### BREAKING CHANGES
*TX Settle 的最小时间可设置为108 us，而不再是113 us。

*两个枚举**TPLL_BR_1M**和**TPLL_BR_2M**已重命名为**TPLL_BITRATE_1MBPS**和**TPLL_BITRATE_2MBPS**。

* 将API TPLL_SetDatarate()重命名为TPLL_Init()，以更匹配该函数的用途。

* 将API void TPLL_OpenPipe(TPLL_PipeIDTypeDef pipe_id, unsigned char auto_ack_en)改为 void TPLL_OpenPipe(TPLL_PipeIDTypeDef pipe_id)，并删除了无效参数auto_ack_en。

* 弃用并删除以下API：
```c
TPLL_GetAutoRetrStatus()
TPLL_GetRxPayloadWidth()
TPLL_GetTxFifoStatus()
TPLL_GetReuseTxStatus()
TPLL_WriteTxPayloadNoAck()
TPLL_SetRxPayloadWidth()
TPLL_ReadRxPayloadLength()
TPLL_SetupDynamicPayload()
TPLL_EnableAckPayload()
TPLL_EnableDynamicPayload()
TPLL_SetTimerPeriod()
TPLL_RandGen()
```
## V3.2.4

### Version
* SDK version : telink_vulture_2.4g_proprietary_sdk V3.2.4.
* This version sdk support vulture A2 chip.
### Features
* PM/ADC/FLASH/BOOTCODE
* Interface:PM/ADC/FLASH
### Known issues
* **pm** 
  - if the time of system power on to deep_ret is less than 500us,need to wait 32k_cal update
  - fix pm_long_suspend flag
* **adc** 
  - add function of adc_update_1p2_vref_calib_value
* **bootcode** 
  - add cstartup_8278_SRAM.S and fix bootlink
* **flash** 
  - fix flash_operation demo

### BREAKING CHANGES
* N/A

<hr style="border-bottom:2.5px solid rgb(146, 240, 161)">

### Version
* SDK版本: telink_vulture_2.4g_proprietary_sdk V3.2.4.
* 此版本SDK支持VultureA2芯片.
### Features
* PM/ADC/FLASH/BOOTCODE
* Interface:PM/ADC/FLASH
### Known issues
* **pm** 
  - 增加了系统初始化500us的判断,等待32k_cal稳定 
  - 修改了pm_long_suspend的标志
* **adc** 
  - 增加了adc_update_1p2_vref_calib_value的接口
* **bootcode** 
  - 增加了cstartup_8278_SRAM.S，同时修改了bootlink文件
* **flash** 
  - 优化了flash_operation demo

### BREAKING CHANGES
* N/A



## V3.2.3

### Version
* SDK version : telink_vulture_2.4g_proprietary_sdk V3.2.3.
* This version sdk support vulture chip.
### Features
* UARTOTA/RFOTA
* Interface:UART/RF
### Known issues
* **ota** 
  - Before using the OTA function, you need to open the corresponding macros in ota.c and fw_update.c

### BREAKING CHANGES
* N/A

<hr style="border-bottom:2.5px solid rgb(146, 240, 161)">

### Version
* SDK版本: telink_vulture_2.4g_proprietary_sdk V3.2.3.
* 此版本SDK支持Vulture芯片.
### Features
* UARTOTA/RFOTA
* Interface:UART/RF
### Known issues
* **ota** 
   - 使用OTA功能之前，需要将对应的ota.c与fw_update.c里面的宏打开

### BREAKING CHANGES
* N/A


## V3.2.2

### Version
* SDK version : telink_vulture_2.4g_proprietary_sdk V3.2.3.
* This version sdk support vulture chip.
### Features
* PM
* Interface:PM
### Known issues
* **pm** 
  - update the content of the new PM.C, support vulture A2 chip

### BREAKING CHANGES
* N/A

<hr style="border-bottom:2.5px solid rgb(146, 240, 161)">

### Version
* SDK版本: telink_vulture_2.4g_proprietary_sdk V3.2.3.
* 此版本SDK支持Vulture芯片.
### Features
* PM
* Interface:PM
### Known issues
* **pm** 
   - 跟新PM.C的内容，支持vulture A2芯片

### BREAKING CHANGES
* N/A



## V3.2.1

### Version
* SDK version : telink_vulture_2.4g_proprietary_sdk V3.2.1.
* This version sdk support vulture chip.
### Features
* RAMCRC/TPLL/PM/USBCDC
* Interface:RAMCRC/TPLL/PM/USBCDC
### Known issues
* **ramcrc** 
  - Added API ram_crc_timeing in pm.h
* **pm** 
  - Added soft_start_dly in pm.h
  - Added pm_long_sleep_wakeup API in pm.h
* **tpll** 
  - Added API TPLL_ModeStop API
  - Added API TPLL_SetTxMI API
  - Added API TPLL_SetRxMI API
* **usbcdc** 
  - Fix usb_cdc_tx_data_to_host API

### BREAKING CHANGES
* N/A

<hr style="border-bottom:2.5px solid rgb(146, 240, 161)">

### Version
* SDK版本: telink_vulture_2.4g_proprietary_sdk V3.2.1.
* 此版本SDK支持Vulture芯片.
### Features
* RAMCRC/TPLL/PM/USBCDC
* Interface:RAMCRC/TPLL/PM/USBCDC
### Known issues
* **ramcrc** 
  - 在pm.h里面增加ram_crc_timeing接口
* **pm** 
  - 在pm.h里面增加soft_start_dly接口
  - 在pm.h里面增加pm_long_sleep_wakeup接口
* **tpll** 
  - 增加TPLL_ModeStop接口
  - 增加TPLL_SetTxMI接口
  - 增加TPLL_SetRxMI接口
* **usbcdc** 
  - 修改usb_cdc_tx_data_to_host接口

### BREAKING CHANGES
* N/A


## V3.2.0

### Version
* SDK version : telink_vulture_2.4g_proprietary_sdk V3.2.0.
* This version sdk support vulture chip.
### Features
* FLASH/TPLL/PM/ADC
* Interface:FLASH/TPLL/PM/ADC
### Known issues
* **flash** 
  - Add API flash_write_page in flash demo
* **tpll** 
  - Added API TPLL_GetTimestamp 
  - Added API TPLL_GetRxRssiValue
* **pm** 
  - Fix pm issues and reduce sleep current
* **adc** 
  - Improve adc sampling accuracy

### BREAKING CHANGES
* N/A

<hr style="border-bottom:2.5px solid rgb(146, 240, 161)">

### Version
* SDK版本: telink_vulture_2.4g_proprietary_sdk V3.2.0.
* 此版本SDK支持Vulture芯片.
### Features
* FLASH/TPLL/PM/ADC
* Interface:FLASH/TPLL/PM/ADC
### Known issues
* **flash** 
  - 在flash demo里面增加flash_write_page接口
* **tpll** 
  - 增加TPLL_GetTimestamp接口 
  - 增加TPLL_GetRxRssiValue接口
* **pm** 
  - 减小pm的睡眠电流
* **adc** 
  - 提高ADC采样精度

### BREAKING CHANGES
* N/A


## V3.1.1

### Version
* SDK version : telink_vulture_2.4g_proprietary_sdk V3.1.1.
* This version sdk support vulture chip.
### Features
* USB
* Interface:USB
### Known issues
* **usb** 
  - use new usb demo architecture

### BREAKING CHANGES
* N/A

<hr style="border-bottom:2.5px solid rgb(146, 240, 161)">

### Version
* SDK版本: telink_vulture_2.4g_proprietary_sdk V3.1.1.
* 此版本SDK支持Vulture芯片.
### Features
* USB
* Interface:USB
### Known issues
* **usb** 
   - 使用新的USB的Demo架构

### BREAKING CHANGES
* N/A


## V3.1.0

### Version
* SDK version : telink_vulture_2.4g_proprietary_sdk V3.1.0.
* This version sdk support vulture chip.
### Features
* MDEC/USB 
* Interface:MDEC/USB
### Known issues
* **mdec** 
  - Add mdec wakeup demo
* **usb** 
  - Add usb cdc demo 
  - Add usb mouse demo
  - Add usb keyboard demo   

### BREAKING CHANGES
* N/A

<hr style="border-bottom:2.5px solid rgb(146, 240, 161)">

### Version
* SDK版本: telink_vulture_2.4g_proprietary_sdk V3.1.0.
* 此版本SDK支持Vulture芯片.
### Features
* MDEC/USB 
* Interface:MDEC/USB
### Known issues
* **mdec** 
  - 增加mdec唤醒demo
* **usb** 
  - 增加usb cdc demo 
  - 增加usb mouse demo
  - 增加usb keyboard demo   

### BREAKING CHANGES
* N/A














