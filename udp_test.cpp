
#include <cstdio>
#include <cstring>
#include <string>

#include "simu_hw.h"
#include "hw_library_handle.h"
#include <unistd.h>

#include "thread"
#include "mutex"

//
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <iostream>

bool is_run = true;

static simu_hw::LibraryHandle lib_handle("libmyTTE.so");

void OnLogging(int level, char *file, char *function, int line, char *message)
{
    printf("[%s:%s:%d]%s", file, function, line, message);
}

void onReceive(int card_id, int channel_id, char *data, size_t size, void *arg)
{
    ("平台接收%d字节\n", size);
    for (int i = 0; i < size; i++)
    {

        printf("%d", data[i]);
    }
    printf("\n");
    // is_run=false;
}

// std::mutex m_mutex;
// TTECardState state;
// void OnTTEReveive(TTERcvRawMessage & tteRcvRawdata) {
//     printf("1");
//   }

//   void CardStateCallbackFunc(TTECardState& cardstate) {
//     static int i = 0;
//     if (i == 0) {
//         i++;  // 只打印一次，表明链路通了
//         printf("收到TTE CardState数据\n");
//     }
//     std::lock_guard<std::mutex> lock(m_mutex);

//     state = cardstate;
//     // printf("%d\n",cardstate.card().at(0).pci_tte_present() );

// }

// void ServiceStateCallbackFunc(TTEServiceState& cardstate_list) {
//     static int i = 0;
//     if (i == 0) {
//         i++;  // 只打印一次，表明链路通了
//         printf("收到TTE ServiceState数据\n");
//     }
// }

// int run_card(){
//    TTEAccess tte_;
//     tte_.run(0);

//     std::string localip = "192.168.1.51";
//     std::string groupip = "229.0.0.1";
//     uint16_t groupport = 8888;
//     int rs =tte_.init_board(localip.c_str(),groupip.c_str(),groupport);
//     if(rs<0){
//         printf("加载板卡失败\n");
//     }

//     std::thread th([&]{
//         tte_.RegisterDataCallback(OnTTEReveive);
//         tte_.RegisterCardStateCallback(CardStateCallbackFunc);
//         tte_.RegisterServiceStateCallback(ServiceStateCallbackFunc);
//         printf("callback register\n");
//         while (true) {
//             std::lock_guard<std::mutex> lock(m_mutex);
//             // printf("state.card().at(0).pci_tte_present()%d\n",state.card().at(0).pci_tte_present() );
//             if(state.card().at(0).pci_tte_present() == 1 &&  state.card().at(1).pci_tte_present() == 1){
//                 printf("加载业务规划表\n");
//                 break;
//             }
//         }
//         printf("加载业务规划表\n");
//         int mode = 1;
//         int rs = tte_.load_config(1, mode);
//         while (true) {
//             std::lock_guard<std::mutex> lock(m_mutex);
//             // std::cout<< "-"<<state.card().at(0).schedule_table_length() <<std::endl;
//             // std::cout<<"="<< state.card().at(1).schedule_table_length() <<std::endl;

//             if(state.card().at(0).schedule_table_length() > 0 &&  state.card().at(1).schedule_table_length() > 0){
//                 printf("加载配置完毕\n");
//                 break;
//             }
//             printf("2");
//         }
//         printf("111");
//         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//         tte_.quit();

//         printf("退出\n");

//     });
//     tte_.run(2);

//     //std::this_thread::sleep_for(std::chrono::milliseconds(10000));
//     if (th.joinable()){
//         th.join();
//     }
//     return 0;
// }

int main()
{
    SimuHWThreadParam param;
    param.sched = SimuHWSchedType_Normal;
    snprintf(param.thread_name, sizeof(param.thread_name), "1622_thread");

    lib_handle.SimuHWSetLogCB(OnLogging);
    lib_handle.SimuHWInit(const_cast<char *>("../config.json"), const_cast<char *>("lib_dir"));
    lib_handle.SimuHWSetCardID(6);
    lib_handle.SimuHWRegisteReadCB(onReceive, nullptr);
    lib_handle.SimuHWSetThreadAttr(param);

    SimuHWChannel channels[8];
    int channel_num = lib_handle.SimuHWGetChannels(channels, sizeof(channels) / sizeof(channels[0]));
    for (int i = 0; i < channel_num; i++)
        printf("ID[%d] Card[%s] Module[%s] Channel[%s]\n", channels[i].id, channels[i].card, channels[i].module,
               channels[i].channel);

    lib_handle.SimuHWStart();

    // char buffer[1024] ;
    // int tem=0x1;
    // while (is_run)
    // {
    //     //  tte.quit();
    //   // printf("while");
    //   // const char *str = "hello";
    //   // lib_handle.SimuHWWriteNoBlock(channels[0].id, const_cast<char *>(str), strlen(str));
    //   // Sleep(3000);
    //   for (int i = 0; i < 12; i++){
    //     buffer[i]=tem;
    //     // printf("%d",buffer[i]);
    //   }
    //     buffer[12]=tem++;
    //     if(tem>10){
    //       tem=0;
    //     }
    //   lib_handle.SimuHWWriteNoBlock(channels[0].id, buffer, 13);
    //   // sleep(3000);
    // }
    //  sleep(3000);
    lib_handle.SimuHWStop();
    lib_handle.SimuHWUnInit();
    return 0;
}
