//
// Created by newplan001@163.com on 2023/12/25.
//

#ifndef DEBUG_NET_UTIL_H
#define DEBUG_NET_UTIL_H
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <sys/types.h>

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

class NetAdapterHelper {
public:
    static int get_ipv4_linux(
            std::vector<std::vector<std::string>>& out_list_ip4) {
        return get_ip_linux(AF_INET, out_list_ip4);
    }

    static int get_ipv6_linux(
            std::vector<std::vector<std::string>>& out_list_ip6) {
        return get_ip_linux(AF_INET6, out_list_ip6);
    }

    static int get_ip_linux(int ipv4_6,
                            std::vector<std::vector<std::string>>& out_list_ip) {
        int ret_val = 0;

        struct ifaddrs* ifAddrStruct = NULL;
        void* tmpAddrPtr = NULL;

        // 1.
        ret_val = getifaddrs(&ifAddrStruct);
        if (0 != ret_val) {
            ret_val = errno;
            return ret_val;
        }

        // 2.
        std::string str_ipvX;

        int padress_buf_len = 0;
        char addressBuffer[INET6_ADDRSTRLEN] = {0};

        if (AF_INET6 == ipv4_6)
            padress_buf_len = INET6_ADDRSTRLEN;
        else
            padress_buf_len = INET_ADDRSTRLEN;

        while (NULL != ifAddrStruct) {
            if (ipv4_6 == ifAddrStruct->ifa_addr->sa_family)  //
            {
                std::vector<std::string> net_name_ip_pair = {"unset"};
                // is a valid IP4 Address
                if (ipv4_6 == AF_INET)  // ipv4
                    tmpAddrPtr = &((struct sockaddr_in*)ifAddrStruct->ifa_addr)->sin_addr;
                else if (ipv4_6 == AF_INET6) {
                    tmpAddrPtr =
                            &((struct sockaddr_in6*)ifAddrStruct->ifa_addr)->sin6_addr;
                } else {
                    std::cerr << "Invalid ip version: " << ipv4_6 << std::endl;
                }

                inet_ntop(ipv4_6, tmpAddrPtr, addressBuffer, padress_buf_len);
                str_ipvX = std::string(addressBuffer);
                if (ifAddrStruct->ifa_name) {
                    net_name_ip_pair[0] = std::string(ifAddrStruct->ifa_name);
                    //                    std::cout << "dev_info: dev_name=" <<
                    //                    ifAddrStruct->ifa_name << ", addr = "
                    //                              << str_ipvX << std::endl;
                }
                net_name_ip_pair.push_back(str_ipvX);

                out_list_ip.push_back(net_name_ip_pair);

                memset(addressBuffer, 0, padress_buf_len);
            }

            ifAddrStruct = ifAddrStruct->ifa_next;
        }

        return ret_val;
    }

    static std::vector<std::string> is_local_address(std::string target_addr) {

        std::cout<<"We are going to match "<<target_addr<<std::endl;

        std::vector<std::string> ret = {"0", "invalid_dev"};

        std::vector<std::vector<std::string>> ipv4;
        std::vector<std::vector<std::string>> ipv6;


        auto match_addr = [](const std::string& target,
                             const std::vector<std::vector<std::string>>& from_set,
                             std::vector<std::string>& result) -> bool {
            for (auto& addr : from_set) {
                if (addr[1] == target) {
                    result[0] = "1";      //  found one
                    result[1] = addr[0];  // the device name;

                    return true;
                }
            }
            return false;
        };

        int ret_val = NetAdapterHelper::get_ipv4_linux(ipv4);
        if (0 != ret_val) {
            std::cout << "error, ipv4, id = " << ret_val << std::endl;
        }
        ret_val = NetAdapterHelper::get_ipv6_linux(ipv6);
        if (0 != ret_val) {
            std::cout << "error, ipv6, id = " << ret_val << std::endl;
        }


        if (match_addr(target_addr, ipv4, ret) ||
            match_addr(target_addr, ipv6, ret))
            ;
        return ret;
    }
};

static int test_net_adapter_helper(int argc, char** argv) {
    std::vector<std::vector<std::string>> ipv4;
    std::vector<std::vector<std::string>> ipv6;

    int ret_val = NetAdapterHelper::get_ipv4_linux(ipv4);
    if (0 != ret_val) {
        std::cout << "error, ipv4, id = " << ret_val << std::endl;
    } else {
        int index = 0;
        for (auto& item : ipv4) {
            std::cout << "index = " << ++index;
            std::cout << ", ipv4 = " << item[1] << ", dev_name=" << item[0]
                      << std::endl;
        }
    }

    ret_val = NetAdapterHelper::get_ipv6_linux(ipv6);
    if (0 != ret_val) {
        std::cout << "error, ipv6, id = " << ret_val << std::endl;
    } else {
        int index = 0;
        for (auto item : ipv6) {
            std::cout << "index = " << ++index;
            std::cout << ", ipv6 = " << item[1] << ", dev_name=" << item[0]
                      << std::endl;
        }
    }
    if (argc == 2)
    {
        std::string target = argv[1];
//        std::string target = "127.0.0.1";
        auto match_result = NetAdapterHelper::is_local_address(target);
        std::cout << "match_result = " << match_result[0]
                  << ", dev_name=" << match_result[1] << std::endl;
    }
    return 0;
}

#endif  // DEBUG_NET_UTIL_H
