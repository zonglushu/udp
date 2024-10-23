#include <assert.h>

#include <memory>
#include <string>

#include "tte_linux_lib.h"
#include "simu_hw.h"

using hw_library::serial_windows::TTELINUXLib;

#ifdef __cplusplus
extern "C" {
#endif

SimuHWHandle SimuHWCreateBoard() {
  TTELINUXLib* handle = new TTELINUXLib();
  return handle;
}

int SimuHWDestroyBoard(SimuHWHandle handle) {
  assert(INVALID_HANDLE != handle);
  delete (TTELINUXLib*)handle;
  return 0;
}

int SimuHWInit(SimuHWHandle handle, char* config_file, char* lib_dir) {
  assert(INVALID_HANDLE != handle);
  ((TTELINUXLib*)handle)->Init(config_file, lib_dir);
  return 0;
}

int SimuHWUnInit(SimuHWHandle handle) { return 0; }

int SimuHWSetCardID(SimuHWHandle handle, int card_id) {
  assert(INVALID_HANDLE != handle);
  ((TTELINUXLib*)handle)->SetCardID(card_id);
  return 0;
}

int SimuHWSetThreadAttr(SimuHWHandle handle, SimuHWThreadParam param) {
  assert(INVALID_HANDLE != handle);
  ((TTELINUXLib*)handle)->SetThreadAttr(param);
  return 0;
}

int SimuHWRegisteReadCB(SimuHWHandle handle, SimuHWReadCB cb, void* arg) {
  assert(INVALID_HANDLE != handle);
  ((TTELINUXLib*)handle)->RegisteReadCB(cb, arg);
  return 0;
}

int SimuHWSetLogCB(SimuHWHandle handle, SimuHWLogCB cb) {
  assert(INVALID_HANDLE != handle);
  ((TTELINUXLib*)handle)->SetLogCB(cb);
  return 0;
}

int SimuHWGetChannels(SimuHWHandle handle, SimuHWChannel* channels, size_t max_channel_num) {
  assert(INVALID_HANDLE != handle);
  assert(channels);

  std::vector<SimuHWChannel> temp_channels;
  ((TTELINUXLib*)handle)->GetChannels(&temp_channels);
  if (temp_channels.size() > max_channel_num) return -1;

  for (size_t i = 0; i < temp_channels.size(); i++) channels[i] = temp_channels[i];

  return static_cast<int>(temp_channels.size());
}

int SimuHWStart(SimuHWHandle handle) {
  assert(INVALID_HANDLE != handle);
  ((TTELINUXLib*)handle)->Start();
  return 0;
}

int SimuHWStop(SimuHWHandle handle) {
  assert(INVALID_HANDLE != handle);
  ((TTELINUXLib*)handle)->Stop();
  return 0;
}

int SimuHWWriteNoBlock(SimuHWHandle handle, int channel_id, char* data, size_t size) {
  assert(INVALID_HANDLE != handle);
  return ((TTELINUXLib*)handle)->Write(channel_id, data, size);
}

#ifdef __cplusplus
}
#endif
