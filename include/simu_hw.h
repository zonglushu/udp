#include <stdio.h>

#ifndef SIMU_HW_H_
#define SIMU_HW_H_

#ifdef _MSC_VER
#define SIMU_HW_API __declspec(dllexport)
#else
#define SIMU_HW_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define SIMUHW_LOGLEVEL_DEBUG (1)
#define SIMUHW_LOGLEVEL_INFO (1 << 1)
#define SIMUHW_LOGLEVEL_WARN (1 << 2)
#define SIMUHW_LOGLEVEL_ERROR (1 << 3)

#define SIMUHW_MAX_CPU_NUM (64)
#define SIMUHW_MAX_NAME_LEN (128)
#define SIMUHW_MAX_THREADNAME_LEN (12)
#define SIMUHW_MIN_NICE (-20)
#define SIMUHW_MAX_NICE (19)
#define SIMUHW_MIN_PRIORITY (0)
#define SIMUHW_MAX_PRIORITY (99)

#define INVALID_HANDLE (nullptr)

typedef void* SimuHWHandle;

// Description:
//  Callback of logging.
// Args:
//  level: log level, SIMU_HW_LOGLEVEL_XXX.
//  file: file name.
//  function: function name.
//  line: line number.
//  message: logging message.
typedef void (*SimuHWLogCB)(int level, char* file, char* function, int line, char* message);

// Description:
//  Callback when receiving data.
// Args:
//  card_id: card id.
//  channel_id: channel id.
//  data: received data.
//  size: data size.
//  arg: argument.
typedef void (*SimuHWReadCB)(int card_id, int channel_id, char* data, size_t size, void* arg);

// Thread scheduling type.
typedef enum SimuHWSchedType_E { SimuHWSchedType_Normal, SimuHWSchedType_RR, SimuHWSchedType_FIFO } SimuHWSchedType;

// Thread params.
typedef struct SimuHWThreadParam_T {
  char thread_name[SIMUHW_MAX_THREADNAME_LEN];
  int nice;  // SIMUHW_MIN_NICE - SIMUHW_MAX_NICE
  SimuHWSchedType sched;
  int priority;  // SIMUHW_MIN_PRIORITY - SIMUHW_MAX_PRIORITY
  int cpu_affinity[SIMUHW_MAX_CPU_NUM];
} SimuHWThreadParam;

typedef struct SimuHWChannel_T {
  int id;
  char card[SIMUHW_MAX_NAME_LEN];
  char module[SIMUHW_MAX_NAME_LEN];
  char channel[SIMUHW_MAX_NAME_LEN];
} SimuHWChannel;

typedef SimuHWHandle (*HandleSimuHWCreateBoard)();
typedef int (*HandleSimuHWDestroyBoard)(SimuHWHandle handle);
typedef int (*HandleSimuHWInit)(SimuHWHandle handle, char* config_file, char* lib_dir);
typedef int (*HandleSimuHWUnInit)(SimuHWHandle handle);
typedef int (*HandleSimuHWSetCardID)(SimuHWHandle handle, int card_id);
typedef int (*HandleSimuHWSetThreadAttr)(SimuHWHandle handle, SimuHWThreadParam param);
typedef int (*HandleSimuHWRegisteReadCB)(SimuHWHandle handle, SimuHWReadCB cb, void* arg);
typedef int (*HandleSimuHWSetLogCB)(SimuHWHandle handle, SimuHWLogCB cb);
typedef int (*HandleSimuHWGetChannel)(SimuHWHandle handle, SimuHWChannel* channels, size_t max_channel_num);
typedef int (*HandleSimuHWStart)(SimuHWHandle handle);
typedef int (*HandleSimuHWStop)(SimuHWHandle handle);
typedef int (*HandleSimuHWWriteNoBlock)(SimuHWHandle handle, int channel_id, char* data, size_t size);

// Description:
//  Create a board.
// Return: SimuHWHandle:success INVALID_HANDLE:failed
SIMU_HW_API SimuHWHandle SimuHWCreateBoard();

// Description:
//  Destroy a board.
// Args:
//  handle: A handle of a board by SimuHWCreateBoard().
// Return: 0:success -1:failed
SIMU_HW_API int SimuHWDestroyBoard(SimuHWHandle handle);

// Description:
//  Initializes hardware.
// Args:
//  handle: A handle of a board by SimuHWCreateBoard().
//  config_file: Configuration file of this hardware.
//  lib_dir: A directory stores libraries that this hardware may load.
// Return: 0:success -1:failed
SIMU_HW_API int SimuHWInit(SimuHWHandle handle, char* config_file, char* lib_dir);

// Description:
//  Uninitializes hardware.
// Args:
//  handle: A handle of a board by SimuHWCreateBoard().
// Return: 0:success -1:failed
SIMU_HW_API int SimuHWUnInit(SimuHWHandle handle);

// Description:
//  Set card ID.
// Args:
//  handle: A handle of a board by SimuHWCreateBoard().
//  card_id: Card ID.
// Return: 0:success -1:failed
SIMU_HW_API int SimuHWSetCardID(SimuHWHandle handle, int card_id);

// Description:
//  Set internal receiving thread attribute.
// Args:
//  handle: A handle of a board by SimuHWCreateBoard().
//  param: Internal thread attribute.
// Return: 0:success -1:failed
SIMU_HW_API int SimuHWSetThreadAttr(SimuHWHandle handle, SimuHWThreadParam param);

// Description:
//  Set callback function for receiving data.
// Args:
//  handle: A handle of a board by SimuHWCreateBoard().
//  cb: Callback function of receiving data.
// Return: 0:success -1:failed
SIMU_HW_API int SimuHWRegisteReadCB(SimuHWHandle handle, SimuHWReadCB cb, void* arg);

// Description:
//  Set callback function for logging.
// Args:
//  handle: A handle of a board by SimuHWCreateBoard().
//  cb: Callback function of logging.
// Return: 0:success -1:failed
SIMU_HW_API int SimuHWSetLogCB(SimuHWHandle handle, SimuHWLogCB cb);

// Description:
//  Get all of the channels of this hardware.
// Args:
//  handle: A handle of a board by SimuHWCreateBoard().
//  channels: An array of channels.
//  channel_num: Max channel number.
// Return: channel_num:success -1:failed
SIMU_HW_API int SimuHWGetChannels(SimuHWHandle handle, SimuHWChannel* channels, size_t max_channel_num);

// Description:
//  Start hardware.
// Args:
//  handle: A handle of a board by SimuHWCreateBoard().
// Return: 0:success -1:failed
SIMU_HW_API int SimuHWStart(SimuHWHandle handle);

// Description:
//  Stop hardware.
// Args:
//  handle: A handle of a board by SimuHWCreateBoard().
// Return: 0:success -1:failed
SIMU_HW_API int SimuHWStop(SimuHWHandle handle);

// Description:
//  Write data to a channel.This function returns immediately whether the write
//  operation is finished or not.
// Args:
//  handle: A handle of a board by SimuHWCreateBoard().
//  channel_id: Which channel to write.
//  data: Data to write.
//  size: Size of the data.
// Return: 0:success -1:failed
// Memo: Thread-safe.
SIMU_HW_API int SimuHWWriteNoBlock(SimuHWHandle handle, int channel_id, char* data, size_t size);

#ifdef __cplusplus
}
#endif

#endif  // SIMU_HW_H_
