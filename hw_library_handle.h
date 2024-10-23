#ifndef SIMU_HW_LIBRARY_HANDLE_H_
#define SIMU_HW_LIBRARY_HANDLE_H_

#include <functional>
#include <string>

#include "simu_hw.h"

namespace simu_hw {

class LibraryHandle {
 public:
  explicit LibraryHandle(const std::string& library);
  ~LibraryHandle();
  LibraryHandle(const LibraryHandle&) = delete;
  LibraryHandle(LibraryHandle&&) = delete;
  LibraryHandle& operator=(const LibraryHandle&) = delete;
  LibraryHandle& operator=(LibraryHandle&&) = delete;

  int SimuHWInit(char* config_file, char* lib_dir);
  int SimuHWUnInit();
  int SimuHWSetCardID(int card_id);
  int SimuHWSetThreadAttr(SimuHWThreadParam param);
  int SimuHWRegisteReadCB(SimuHWReadCB cb, void* arg);
  int SimuHWSetLogCB(SimuHWLogCB cb);
  int SimuHWGetChannels(SimuHWChannel* channels, size_t max_channel_num);
  int SimuHWStart();
  int SimuHWStop();
  int SimuHWWriteNoBlock(int channel_id, char* data, size_t size);

 private:
  std::string library_full_path_;
  void* library_handle_{nullptr};
  SimuHWHandle handle_{INVALID_HANDLE};

  HandleSimuHWCreateBoard handle_create_board_{nullptr};
  HandleSimuHWDestroyBoard handle_destroy_board_{nullptr};
  HandleSimuHWInit handle_init_{nullptr};
  HandleSimuHWUnInit handle_uninit_{nullptr};
  HandleSimuHWSetCardID handle_set_cardid_{nullptr};
  HandleSimuHWSetThreadAttr handle_set_thread_attr_{nullptr};
  HandleSimuHWRegisteReadCB handle_reg_read_cb_{nullptr};
  HandleSimuHWSetLogCB handle_reg_log_cb_{nullptr};
  HandleSimuHWGetChannel handle_get_channel_{nullptr};
  HandleSimuHWStart handle_start_{nullptr};
  HandleSimuHWStop handle_stop_{nullptr};
  HandleSimuHWWriteNoBlock handle_write_noblock_{nullptr};
};

}  // namespace simu_hw

#endif  // SIMU_HW_LIBRARY_HANDLE_H_
