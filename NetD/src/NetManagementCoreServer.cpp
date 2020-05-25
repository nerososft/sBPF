//
// Created by XingfengYang on 2020/5/12.
//

#include "../include/NetManagementCoreServer.h"
#include "../include/log/Log.h"
#include "unistd.h"
#include "../include/controller/ApiConfig.h"

#include <linux/rtnetlink.h>
#include <thread>


hm::netd::NetManagementCoreServer::~NetManagementCoreServer() noexcept = default;

hm::netd::NetManagementCoreServer::NetManagementCoreServer(hm::netd::NetManagementCoreServer &&) noexcept = default;

hm::netd::NetManagementCoreServer &hm::netd::NetManagementCoreServer::operator=(hm::netd::NetManagementCoreServer &&) noexcept = default;

hm::netd::NetManagementCoreServer::NetManagementCoreServer() :
        commandListener(std::make_shared<CommandListener>()),
        netlinkManager(std::make_shared<NetlinkManager>()),
        networkContext(std::make_shared<NetworkContext>()),
        httpServer(std::make_shared<HttpServer>()) {

}


void hm::netd::NetManagementCoreServer::Init() {
    // init netlink manager

    unsigned int pid = (unsigned int) getpid();

    LogInfo("Pid: %d", pid)

    // set initial
    commandListener->AbstractServiceListener::SetNetlinkManager(netlinkManager);
    commandListener->AbstractServiceListener::SetNetworkContext(networkContext);
    sockaddr_nl netlinkCommandAddr{
            .nl_family=AF_NETLINK,
            .nl_pad=0,
            .nl_pid=pid,
            .nl_groups = RTMGRP_IPV4_IFADDR
    };
    CommandListenerConfiguration commandListenerConfiguration{
            .netdConfiguration={
                    .type=NETLINK_ROUTE,
                    .bindAddr=netlinkCommandAddr
            }
    };
    commandListener->SetConfiguration(commandListenerConfiguration);
}

void hm::netd::NetManagementCoreServer::StartCommandListener() {
    if (commandListener->StartListener()) {
        LogError("Unable to start CommandListener")
        exit(1);
    }
}


void hm::netd::NetManagementCoreServer::StartHttpServer(unsigned int port) {
    httpServer->Configure({
                                  .socketConfiguration={
                                          .port=port
                                  }
                          });

    hm::netd::ApiConfig apiConfig;
    apiConfig.AttachApps(httpServer, networkContext);

    httpServer->Start();
}


void hm::netd::NetManagementCoreServer::Start() {

    this->StartHttpServer(8080);

    std::thread(&NetManagementCoreServer::StartCommandListener, this).join();


    while (true);
}
