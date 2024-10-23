#include "hw_library_handle.h"

#include <fcntl.h>
#include <cerrno>
#include <string.h>
#include <cstring>
#include <dlfcn.h>
#include <errno.h>

namespace simu_hw {

LibraryHandle::LibraryHandle(const std::string& library) : library_full_path_(library) {
  library_handle_ = dlopen(library_full_path_.c_str(), RTLD_NOW);
  if (!library_handle_) {
    printf("dlopen(%s) failed! Error[%s]\n", library_full_path_.c_str(), dlerror());
    return;
  }

  dlerror();  // Clear dlerror().

  const char* sym_name = "SimuHWCreateBoard";
  handle_create_board_ = reinterpret_cast<HandleSimuHWCreateBoard>(dlsym(library_handle_, sym_name));
  if (dlerror()) {
    printf("dlsym(%s) failed! Error[%d]\n", sym_name, strerror(errno));
    return;  
  }

  sym_name = "SimuHWDestroyBoard";
  handle_destroy_board_ = reinterpret_cast<HandleSimuHWDestroyBoard>(dlsym(library_handle_, sym_name));
  if (dlerror()) {
    printf("dlsym(%s) failed! Error[%d]\n", sym_name, strerror(errno));
    return;
  }

  sym_name = "SimuHWInit";
  handle_init_ = reinterpret_cast<HandleSimuHWInit>(dlsym(library_handle_, sym_name));
  if (dlerror()) {
    printf("dlsym(%s) failed! Error[%d]\n", sym_name, strerror(errno));
    return;
  }

  sym_name = "SimuHWUnInit";
  handle_uninit_ = reinterpret_cast<HandleSimuHWUnInit>(dlsym(library_handle_, sym_name));
  if (dlerror()) {
    printf("dlsym(%s) failed! Error[%d]\n", sym_name, strerror(errno));
    return;
  }

  sym_name = "SimuHWSetCardID";
  handle_set_cardid_ = reinterpret_cast<HandleSimuHWSetCardID>(dlsym(library_handle_, sym_name));
  if (dlerror()) {
    printf("dlsym(%s) failed! Error[%d]\n", sym_name, strerror(errno));
    return;
  }

  sym_name = "SimuHWSetThreadAttr";
  handle_set_thread_attr_ = reinterpret_cast<HandleSimuHWSetThreadAttr>(dlsym(library_handle_, sym_name));
  if (dlerror()) {
    printf("dlsym(%s) failed! Error[%d]\n", sym_name, strerror(errno));
    return;
  }

  sym_name = "SimuHWRegisteReadCB";
  handle_reg_read_cb_ = reinterpret_cast<HandleSimuHWRegisteReadCB>(dlsym(library_handle_, sym_name));
  if (dlerror()) {
    printf("dlsym(%s) failed! Error[%d]\n", sym_name, strerror(errno));
    return;
  }

  sym_name = "SimuHWSetLogCB";
  handle_reg_log_cb_ = reinterpret_cast<HandleSimuHWSetLogCB>(dlsym(library_handle_, sym_name));
  if (dlerror()) {
    printf("dlsym(%s) failed! Error[%d]\n", sym_name, strerror(errno));
    return;
  }

  sym_name = "SimuHWGetChannels";
  handle_get_channel_ = reinterpret_cast<HandleSimuHWGetChannel>(dlsym(library_handle_, sym_name));
  if (dlerror()) {
    printf("dlsym(%s) failed! Error[%d]\n", sym_name, strerror(errno));
    return;
  }

  sym_name = "SimuHWStart";
  handle_start_ = reinterpret_cast<HandleSimuHWStart>(dlsym(library_handle_, sym_name));
  if (dlerror()) {
    printf("dlsym(%s) failed! Error[%d]\n", sym_name, strerror(errno));
    return;
  }

  sym_name = "SimuHWStop";
  handle_stop_ = reinterpret_cast<HandleSimuHWStop>(dlsym(library_handle_, sym_name));
  if (dlerror()) {
    printf("dlsym(%s) failed! Error[%d]\n", sym_name, strerror(errno));
    return;
  }

  sym_name = "SimuHWWriteNoBlock";
  handle_write_noblock_ = reinterpret_cast<HandleSimuHWWriteNoBlock>(dlsym(library_handle_, sym_name));
  if (dlerror()) {
    printf("dlsym(%s) failed! Error[%d]\n", sym_name, strerror(errno));
    return;
  }

  handle_ = handle_create_board_();
}

LibraryHandle::~LibraryHandle() {
  if (handle_ != INVALID_HANDLE && handle_destroy_board_) handle_destroy_board_(handle_);

  if (library_handle_) dlclose(library_handle_);
}

int LibraryHandle::SimuHWInit(char* config_file, char* lib_dir) { return handle_init_(handle_, config_file, lib_dir); }

int LibraryHandle::SimuHWUnInit() { return handle_uninit_(handle_); }

int LibraryHandle::SimuHWSetCardID(int card_id) { return handle_set_cardid_(handle_, card_id); }

int LibraryHandle::SimuHWSetThreadAttr(SimuHWThreadParam param) { return handle_set_thread_attr_(handle_, param); }

int LibraryHandle::SimuHWRegisteReadCB(SimuHWReadCB cb, void* arg) { return handle_reg_read_cb_(handle_, cb, arg); }

int LibraryHandle::SimuHWSetLogCB(SimuHWLogCB cb) { return handle_reg_log_cb_(handle_, cb); }

int LibraryHandle::SimuHWGetChannels(SimuHWChannel* channels, size_t max_channel_num) {
  return handle_get_channel_(handle_, channels, max_channel_num);
}

int LibraryHandle::SimuHWStart() { return handle_start_(handle_); }

int LibraryHandle::SimuHWStop() { return handle_stop_(handle_); }

int LibraryHandle::SimuHWWriteNoBlock(int channel_id, char* data, size_t size) {
  return handle_write_noblock_(handle_, channel_id, data, size);
}

}  // namespace simu_hw
