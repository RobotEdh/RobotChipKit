
#if defined(WF_CS_TRIS)

#ifndef __WF_DEBUG_STRINGS_H
#define __WF_DEBUG_STRINGS_H


#if defined(STACK_USE_UART)
    extern ROM char *connectionFailureStrings[];
    extern ROM char *connectionLostStrings[];
    
    #if defined(MRF24WG)
        extern ROM char *deauthDisssocReasonCodes[];
        extern ROM char *statusCodes[];
        extern ROM char *wpsState[];
        extern ROM char *wpsConfigErr[];
    #endif /* MRF24WG */
    
#if defined(DISPLAY_FILENAME)
    extern ROM char *moduleName[];
#endif    

/*---------------------------*/
/* Compile-time Error Checks */
/*---------------------------*/
#if defined(WF_EASY_CONFIG_DEMO) 
    #if (MY_DEFAULT_NETWORK_TYPE != WF_ADHOC) && (MY_DEFAULT_NETWORK_TYPE != WF_SOFT_AP)
        #error "MY_DEFAULT_NETWORK_TYPE must be set to WF_ADHOC for this demo (see WF_Config.h)"
    #endif
    
    #if (MY_DEFAULT_PS_POLL == WF_ENABLED)
        #error "MY_DEFAULT_PS_POLL must be set to WF_DISABLED when EZConfig demo (see WF_Config.h)"
    #endif
    
#endif

#if defined(WF_CONSOLE_DEMO)
    #if !defined(ENABLE_DHCP_IP) && !defined(ENABLE_STATIC_IP)
        #error "Must define either ENABLE_DHCP_IP or ENABLE_STATIC_IP (see WF_Config.h)"
    #endif
    
    #if defined(ENABLE_DHCP_IP) && defined(ENABLE_STATIC_IP)
        #error "Cannot define both ENABLE_DHCP_IP and ENABLE_STATIC_IP; select only one of these (see WF_Config.h)"
    #endif
    
    #if (MY_DEFAULT_NETWORK_TYPE == WF_ADHOC)
        #if (MY_DEFAULT_PS_POLL == WF_ENABLED)
            #error "MY_DEFAULT_PS_POLL must be set to WF_DISABLED when running console demo in AdHoc mode (see WF_Config.h)"
        #endif
        
        #if defined(ENABLE_DHCP_IP)
            #error "To start up in AdHoc mode, comment out ENABLE_DHCP_IP and define ENABLE_STATIC_IP"
        #endif
        
    #endif
    
    #if defined(ENABLE_DHCP_IP)
    #endif
    
#endif


#if defined(MRF24WG)      
    // error check WEP key type
    #if (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WEP_40 || MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WEP_104)
        #if (MY_DEFAULT_WIFI_SECURITY_WEP_KEYTYPE != WF_SECURITY_WEP_OPENKEY) && (MY_DEFAULT_WIFI_SECURITY_WEP_KEYTYPE != WF_SECURITY_WEP_SHAREDKEY)
            #error "MY_DEFAULT_WIFI_SECURITY_WEP_KEYTYPE must be defined as WF_SECURITY_WEP_OPENKEY or WF_SECURITY_WEP_SHAREDKEY (see WF_Config.h)"
        #endif  /* MY_DEFAULT_WIFI_SECURITY_WEP_KEYTYPE */          
    #endif /* MY_DEFAULT_WIFI_SECURITY_MODE */
#endif

// WPS only supported with MRF24WG */
#if (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PIN) || (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PUSH_BUTTON)
    #if !defined(MRF24WG)
        #error "Only MRF24WG supports WPS; not supported by MRF24WB (see WF_Config.h)"
    #endif
    
    #if (MY_DEFAULT_NETWORK_TYPE != WF_INFRASTRUCTURE) && (MY_DEFAULT_NETWORK_TYPE != WF_P2P)
       // #error "For WPS mode MY_DEFAULT_NETWORK_TYPE must be set to WF_INFRASTRUCTURE or WF_P2P"
    #endif
#endif


#if (MY_DEFAULT_NETWORK_TYPE == WF_ADHOC)
    #if (MY_DEFAULT_PS_POLL != WF_DISABLED)
        #error "MY_DEFAULT_PS_POLL must be set to WF_DISABLED when MY_DEFAULT_NETWORK_TYPE is set to WF_ADHOC (see WF_Config.h)"
    #endif
    
    #if (MY_DEFAULT_WIFI_SECURITY_MODE != WF_OPEN) && \
        (MY_DEFAULT_WIFI_SECURITY_MODE != WF_SECURITY_WEP_40) && \
        (MY_DEFAULT_WIFI_SECURITY_MODE != WF_SECURITY_WEP_104)
        #error "The only security modes allowed in AdHoc mode are WF_OPEN, WF_SECURITY_WEP_40, or WF_SECURITY_WEP_104"
    #endif
    
    #if (MY_DEFAULT_LIST_RETRY_COUNT == WF_RETRY_FOREVER)
        #error "For AdHoc connections do not set MY_DEFAULT_LIST_RETRY_COUNT to WF_RETRY_FOREVER.  Setting it to 3 is recommended"
    #endif
    
#endif


// if using WPS (push-button or PIN)
#if (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PIN) || (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PUSH_BUTTON)
    #if (MY_DEFAULT_SCAN_TYPE != WF_ACTIVE_SCAN)
        #error "For WPS, MY_DEFAULT_SCAN_TYPE must be set to WF_ACTIVE_SCAN (see WF_Config.h)"
    #endif
#endif


/*---------------------*/
/* Function Prototypes */
/*---------------------*/    
void WF_OutputConnectionInfo(const APP_CONFIG *p_AppConfig);
void WF_OutputConnectionDebugMsg(UINT8 event, UINT16 eventInfo);

#if defined(MRF24WG)
void WF_OutputConnectionContext(void);   
#endif 
    
#endif /* STACK_USE_UART */


#endif /* __WF_DEBUG_STRINGS_H */

#endif /* WF_CS_TRIS */



