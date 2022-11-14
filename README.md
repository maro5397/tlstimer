# tlstimer
check TLS time for report

### opensocket library
```
$ git clone https://github.com/wifievent/opensocket.git
```

### How to make cert(pem) files
1. Make cert with following command.
```
$ openssl genrsa -aes128 -out server.key 4096
$ openssl genrsa -aes128 -out server.key 2048
$ openssl genrsa -aes256 -out server.key 4096
$ openssl genrsa -aes256 -out server.key 2048
$ openssl genrsa -aria128 -out server.key 4096
$ openssl genrsa -aria128 -out server.key 2048
$ openssl genrsa -aria256 -out server.key 4096
$ openssl genrsa -aria256 -out server.key 2048
```

2. Each time you enter one of the above commands, enter the following command.
```
$ openssl req -new -key server.key -out server.csr
$ cp server.key server.key.origin
$ openssl x509 -req -days 3650 -in server.csr -signkey server.key -out server.crt
$ openssl rsa -in server.key -text ] key.pem
$ openssl x509 -inform PEM -in server.crt ] crt.pem
$ openssl pkcs12 -export -in server.crt -inkey server.key -out cert.p12
$ openssl pkcs12 -in cert.p12 -nodes -out cert.pem
```

### CIPHER LIST
```
ARIA128-GCM-SHA256             TLSv1.2 Kx=RSA      Au=RSA   Enc=ARIAGCM(128)           Mac=AEAD
ARIA256-GCM-SHA384             TLSv1.2 Kx=RSA      Au=RSA   Enc=ARIAGCM(256)           Mac=AEAD
AES256-SHA256                  TLSv1.2 Kx=RSA      Au=RSA   Enc=AES(256)               Mac=SHA256
AES128-SHA256                  TLSv1.2 Kx=RSA      Au=RSA   Enc=AES(128)               Mac=SHA256
AES128-GCM-SHA256              TLSv1.2 Kx=RSA      Au=RSA   Enc=AESGCM(128)            Mac=AEAD
AES128-CCM8                    TLSv1.2 Kx=RSA      Au=RSA   Enc=AESCCM8(128)           Mac=AEAD
AES128-CCM                     TLSv1.2 Kx=RSA      Au=RSA   Enc=AESCCM(128)            Mac=AEAD
AES256-GCM-SHA384              TLSv1.2 Kx=RSA      Au=RSA   Enc=AESGCM(256)            Mac=AEAD
AES256-CCM8                    TLSv1.2 Kx=RSA      Au=RSA   Enc=AESCCM8(256)           Mac=AEAD
AES256-CCM                     TLSv1.2 Kx=RSA      Au=RSA   Enc=AESCCM(256)            Mac=AEAD
```

## How to USE

### Server
* main [server-port] [ciphersuite] [symmetric type] [certificate type]
```
$ main 8080 AES256-GCM-SHA384 aes256 2048
```

### Client
* main [server-ip] [server-port] [ciphersuite] [filename] [count]
```
$ main 127.0.0.1 8080 AES256-GCM-SHA384 testdata.txt 1
```