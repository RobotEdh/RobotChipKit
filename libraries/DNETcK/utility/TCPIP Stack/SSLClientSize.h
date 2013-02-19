#ifndef __SSL_RSA_CLIENT_SIZE
#define __SSL_RSA_CLIENT_SIZE

	#define SSL_RSA_CLIENT_SIZE     (1024ul)    // Size of Encryption Buffer (must be larger than key size)
    #if SSL_RSA_CLIENT_SIZE < SSL_RSA_KEY_SIZE
        #error "SSL_RSA_CLIENT_SIZE must be >= SSL_RSA_KEY_SIZE"
    #endif
    #if !defined(__PIC32MX__) && ((SSL_RSA_CLIENT_SIZE > 1024) || (SSL_RSA_KEY_SIZE > 1024))
        #error "SSL Key sizes greater than 1024 only supported on the PIC32"
    #endif


#endif

