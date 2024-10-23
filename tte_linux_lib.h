#ifndef SIMU_HW_UDP_WINDOWS_LIB_H_
#define SIMU_HW_UDP_WINDOWS_LIB_H_

#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <atomic>
#include <thread>

// #include <unordered_map>


#include "simu_hw.h"

#include <mutex>
#include <xlnt/xlnt.hpp>



#define READ_BUFFER_SIZE 1024 * 1024 * 3

namespace hw_library {
namespace serial_windows {

  // void OnTTEReveive(char *data, size_t len, int TTE_Index);
  class TTELINUXLib
  {
  public:
    // using UdpHandle = std::shared_ptr<hw_library::Udp>;
    
    struct SerialParam
    {
      SimuHWChannel_T channel_info;

      SerialParam() { memset(&channel_info, 0, sizeof(channel_info)); }
    };

    TTELINUXLib();
    ~TTELINUXLib() = default;
    TTELINUXLib(const TTELINUXLib &) = delete;
    TTELINUXLib(TTELINUXLib &&) = delete;
    TTELINUXLib &operator=(const TTELINUXLib &) = delete;
    TTELINUXLib &operator=(TTELINUXLib &&) = delete;
    
    int Init(char *config_file, char *lib_dir);
    int SetCardID(int card_id);
    int SetThreadAttr(const SimuHWThreadParam &param);
    int RegisteReadCB(SimuHWReadCB cb, void *arg);
    int SetLogCB(SimuHWLogCB cb);
    int GetChannels(std::vector<SimuHWChannel> *channels);
    int Start();
    int Stop();
    int Write(int channel_id, char *data, size_t size);
    int ParseConfig(const std::string &config_file);

    static TTELINUXLib& GetInstance();
  

  private:
    // void ReadThread();
    // int ParseConfig(const std::string &config_file);

  private:
    static TTELINUXLib* lib_handle_;

    std::string config_file_;
    std::string lib_dir_;
    
    SimuHWThreadParam thread_param_;
    std::vector<SimuHWChannel> channel_config_;
    int channel_count_;
    int thread_CPU_;
    
    

    public:
    // char read_buff_[READ_BUFFER_SIZE]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
    char read_buff_[READ_BUFFER_SIZE] = {
        0x00, 0x00, 0x00, 0x1d, // 总长度29（字节，不包含自身）
        // 0x00, 0x00, 0x00, 0x00, // 操作标志（有效）
        0xff, 0xff, 0xff, 0xf0, // 操作标志（有效）
        0x0a,                   // 数据单元长度（10字节，不包含自身）
        0x20, 0x21,             // 标识符
        0x00, 0x02,             // 起始索引
        0x00, 0x03,             // 结束索引
        0xFF, 0xFF,             // 掩码
        0xAA, 0xBB,             // 修改内容

        0x0c,                   // 数据单元长度（12字节，不包含自身）
        0x56, 0x78,             // 标识符
        0x00, 0x06,             // 起始索引
        0x00, 0x08,             // 结束索引
        0xFF, 0xFF, 0x0F,       // 掩码
        0xCC, 0xDD, 0xAA,       // 修改内容

        1,                    // 日志位（示例校验和）
        
        0xAB, 0x26              // 额外数据
    };
    

    SimuHWReadCB read_cb_;
    int card_id_;
    void *read_cb_arg_;
    std::mutex Read_buff_buffer_mutex_;
    std::atomic<bool> write_flag_{false};

    bool is_run_=false;


    std::string local_ip_="192.168.1.51";
    std::string group_ip_="229.0.0.1";
    uint16_t group_port_=8888;
  
    std::string device_1_="ES122";
    std::string device_2_="ES120";

    bool error_log_enable_=true;
    bool error_log_to_file_=true;
    bool error_log_output_=true;
    

    bool debug_log_enable_=true;
    bool debug_log_to_file_=false;
    bool debug_log_output_=true;
    bool info_log_enable_=false;
    
    int tte_index_1_=0;
    int tte_index_2_=0;

    std::string log_directory_="./tte_log";
    std::string source_directory_="../es_table";
};

}  // namespace serial_windows
}  // namespace hw_library



#endif  // SIMU_HW_UDP_WINDOWS_LIB_H_
