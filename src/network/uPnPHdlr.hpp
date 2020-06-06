// UnderStory
// An intuitive Pen & Paper experience
// Copyright (C) 2020 Guillaume Vara

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Any graphical resources available within the source code may
// use a different license and copyright : please refer to their metadata
// for further details. Graphical resources without explicit references to a
// different license and copyright still refer to this GPL.

#pragma once

#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#include <miniupnpc/portlistingparse.h>
#include <miniupnpc/upnperrors.h>

#ifdef _WIN32
    #include <time.h>
    #include <winsock2.h>
    #include <windows.h>
#else
/* for IPPROTO_TCP / IPPROTO_UDP */
    #include <time.h>
    #include <netinet/in.h>
#endif

#include <spdlog/spdlog.h>

#include <string>
#include <future>

namespace UnderStory {

class UPnPHandler {
 public:
    using RetCode = int;

    UPnPHandler(uint16_t portToOpen, std::string requestDescription) :
        _requestedPortToOpen(std::to_string(portToOpen)),
        _requestDescription(requestDescription) {}

    std::string requestedPort() const {
        return _requestedPortToOpen;
    }

    bool hasSucceded() const {
        return this->_redirectSuccessful;
    }

    std::string externalIp() const {
        return this->_externalIp;
    }

    // try to request uPnP port redirection to domestic router
    std::future<void> run() {
        return std::async(
            std::launch::async,
            &UPnPHandler::_queryRedirection,
            this
        );
    }

    // remove redirects
    std::future<RetCode> stop() {
        return std::async(
            std::launch::async,
            &UPnPHandler::_autoRemoveRedirect,
            this
        );
    }

    ~UPnPHandler() {
        // if any redirect succeded
        this->_autoRemoveRedirect();

        /*free*/
        FreeUPNPUrls(&urls);
        freeUPNPDevlist(devlist);
        devlist = 0;

        /*End websock*/
        #ifdef _WIN32
            if(_WSAStarted) WSACleanup();
        #endif
    }

 private:
    static inline std::string _protocol = "TCP";
    std::string _requestedPortToOpen;
    std::string _requestDescription;
    std::string _externalIp;
    bool _redirectSuccessful = false;
    #ifdef _WIN32
        bool _WSAStarted = false;
    #endif

    struct UPNPUrls urls;
    struct IGDdatas data;
    struct UPNPDev * devlist = 0;
    char lanaddr[64] = "unset"; /* my ip address on the LAN */
    int i = -1;
    char * rootdescurl = 0;
    char * multicastif = 0;
    char * minissdpdpath = 0;
    int localport = UPNP_LOCAL_PORT_ANY;
    int error = 0;
    int ipv6 = 0;
    unsigned char ttl = 2; /* defaulting to 2 */

    RetCode _autoRemoveRedirect() {
        if(!_redirectSuccessful) return 0;
        auto failed = this->_RemoveRedirect(
            this->_requestedPortToOpen.c_str(),
            this->_protocol.c_str(),
            NULL
        );
        if(!failed) _redirectSuccessful = false;
        return failed;
    }

    void _queryRedirection() {
        try {
            // init uPnP
            auto initResult = this->_initUPnP();
            if (initResult) return;

            // try to redirect
            auto redirectResult = this->_SetRedirectAndTest(
                this->lanaddr,
                this->_requestedPortToOpen,
                this->_requestedPortToOpen,
                _protocol.c_str(),
                "0",
                0
            );
            if(!redirectResult) this->_redirectSuccessful = true;
        } catch(...) {
            spdlog::debug("UPNP run : exception caught while processing");
        }
    }

    RetCode _initUPnP() {
        #ifdef _WIN32
            /*start websock*/
            WSADATA wsaData;
            auto nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (nResult != NO_ERROR) {
                spdlog::debug("UPNP Inst : Cannot init socket with WSAStartup !");
                return 1;
            }
            _WSAStarted = true;
        #endif

        /*discover*/
        if (rootdescurl || (devlist = upnpDiscover(2000, multicastif, minissdpdpath, localport, ipv6, ttl, &error))) {
            struct UPNPDev * device;

            if (devlist) {
                spdlog::debug("UPNP Inst : List of UPNP devices found on the network :");

                for (device = devlist; device; device = device->pNext) {
                    spdlog::debug("UPNP Inst : desc:{0} st:{1}", device->descURL, device->st);
                }

            } else if (!rootdescurl) {
                spdlog::debug("UPNP Inst : upnpDiscover() error code={0:d}", error);
            }

            i = 1;

            if ((rootdescurl && UPNP_GetIGDFromUrl(rootdescurl, &urls, &data, lanaddr, sizeof(lanaddr))) || (i = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr)))) {
                switch (i) {
                    case 1:
                        spdlog::debug("UPNP Inst : Found valid IGD : {}", urls.controlURL);
                        break;
                    case 2:
                        spdlog::debug("UPNP Inst : Found a (not connected?) IGD : {}", urls.controlURL);
                        spdlog::debug("UPNP Inst : Trying to continue anyway");
                        break;
                    case 3:
                        spdlog::debug("UPNP Inst : UPnP device found. Is it an IGD ? : {}", urls.controlURL);
                        spdlog::debug("UPNP Inst : Trying to continue anyway");
                        break;
                    default:
                        spdlog::debug("UPNP Inst : Found device (igd ?) : {}", urls.controlURL);
                        spdlog::debug("UPNP Inst : Trying to continue anyway");
                }

                spdlog::debug("UPNP Inst : Local LAN ip address {}", lanaddr);

                char externalIPAddress[40];
                int r = UPNP_GetExternalIPAddress(urls.controlURL,
                    data.first.servicetype,
                    externalIPAddress);
                if (r != UPNPCOMMAND_SUCCESS) {
                    spdlog::debug("UPNP AskRedirect : GetExternalIPAddress No IGD UPnP Device. ");
                } else {
                    spdlog::debug("UPNP AskRedirect : ExternalIPAddress = {}", externalIPAddress);
                    this->_externalIp = externalIPAddress;
                }
            } else {
                spdlog::debug("UPNP Inst : No valid UPNP Internet Gateway Device found.");
                return 1;
            }
        } else {
            spdlog::debug("UPNP Inst : No IGD UPnP Device found on the network !");
            return 1;
        }

        return 0;
    }

    RetCode _SetRedirectAndTest(const char * iaddr, const std::string &iport, std::string eport, const char * proto, const char * leaseDuration, int addAny) {
        char externalIPAddress[40];
        char intClient[40];
        char intPort[6];
        char reservedPort[6];
        char duration[16];
        int r;

        if (!iaddr || !iport.length() || !eport.length() || !proto) {
            spdlog::debug("UPNP AskRedirect : Wrong arguments");
            return -1;
        }
        proto = _protofix(proto);
        if (!proto) {
            spdlog::debug("UPNP AskRedirect : invalid protocol");
            return -1;
        }

        r = UPNP_GetExternalIPAddress(urls.controlURL,
                        data.first.servicetype,
                        externalIPAddress);
        if (r != UPNPCOMMAND_SUCCESS)
            spdlog::debug("UPNP AskRedirect : GetExternalIPAddress No IGD UPnP Device.");

        if (addAny) {
            r = UPNP_AddAnyPortMapping(
                        urls.controlURL,
                        data.first.servicetype,
                        eport.c_str(),
                        iport.c_str(),
                        iaddr,
                        this->_requestDescription.c_str(),
                        proto,
                        0,
                        leaseDuration,
                        reservedPort
                );
            if (r == UPNPCOMMAND_SUCCESS)
                eport = reservedPort;
            else
                spdlog::debug("UPNP AskRedirect : AddAnyPortMapping({0}, {1}, {2}) failed with code {3:d} ({4})",
                    eport,
                    iport,
                    iaddr,
                    r,
                    strupnperror(r)
                );
        } else {
            r = UPNP_AddPortMapping(
                    urls.controlURL,
                    data.first.servicetype,
                    eport.c_str(),
                    iport.c_str(),
                    iaddr,
                    this->_requestDescription.c_str(),
                    proto,
                    NULL /*remoteHost*/,
                    leaseDuration
                );
            if (r != UPNPCOMMAND_SUCCESS) {
                spdlog::debug("UPNP AskRedirect : AddPortMapping({0}, {1}, {2}) failed with code {3:d} ({4})",
                    eport,
                    iport,
                    iaddr,
                    r,
                    strupnperror(r)
                );
                return -2;
            }
        }

        r = UPNP_GetSpecificPortMappingEntry(
                urls.controlURL,
                data.first.servicetype,
                eport.c_str(),
                proto,
                NULL /*remoteHost*/,
                intClient,
                intPort,
                NULL /*desc*/,
                NULL /*enabled*/,
                duration
            );
        if (r != UPNPCOMMAND_SUCCESS) {
            spdlog::debug("UPNP AskRedirect : GetSpecificPortMappingEntry() failed with code {0:d} ({1})", r, strupnperror(r));
            return -2;
        } else {
            spdlog::debug("UPNP AskRedirect : external {0} : {1} {2} is redirected to internal {3} : {4} (duration={5})",
                externalIPAddress,
                eport,
                proto,
                intClient,
                intPort,
                duration
            );
        }
        return 0;
    }

    RetCode _RemoveRedirect(const std::string &eport,
                const char * proto,
                const char * remoteHost) {
        int r;
        if (!proto || !eport.length()) {
            spdlog::debug("UPNP _RemoveRedirect : invalid arguments");
            return -1;
        }
        proto = _protofix(proto);
        if (!proto) {
            spdlog::debug("UPNP _RemoveRedirect : protocol invalid");
            return -1;
        }
        r = UPNP_DeletePortMapping(
            urls.controlURL,
            data.first.servicetype,
            eport.c_str(),
            proto,
            remoteHost
        );
        if (r != UPNPCOMMAND_SUCCESS) {
            spdlog::debug("UPNP _RemoveRedirect : UPNP_DeletePortMapping() failed with code : {0:d}", r);
            return -2;
        } else {
            spdlog::debug("UPNP _RemoveRedirect : UPNP_DeletePortMapping() returned : {0:d}", r);
        }
        return 0;
    }

    /* protofix() checks if protocol is "UDP" or "TCP"
    * returns NULL if not */
    static const char * _protofix(const char * proto) {
        static const char proto_tcp[4] = { 'T', 'C', 'P', 0};
        static const char proto_udp[4] = { 'U', 'D', 'P', 0};
        int i, b;
        for (i = 0, b = 1; i < 4; i++)
            b = b && (
                        (proto[i] == proto_tcp[i])
                        || (proto[i] == (proto_tcp[i] | 32))
                    );
        if (b)
            return proto_tcp;
        for (i = 0, b = 1; i < 4; i++)
            b = b && (
                        (proto[i] == proto_udp[i])
                        || (proto[i] == (proto_udp[i] | 32))
                    );
        if (b)
            return proto_udp;
        return 0;
    }
};

}  // namespace UnderStory
