# tlstimer
check TLS time for report

## opensocket library
```
    git clone https://github.com/wifievent/opensocket.git
```

## How to make cert(pem) files
1. Make cert with following command.
```
    openssl genrsa -aes128 -out server.key 4096
    openssl genrsa -aes128 -out server.key 2048
    openssl genrsa -aes192 -out server.key 4096
    openssl genrsa -aes192 -out server.key 2048
    openssl genrsa -aes256 -out server.key 4096
    openssl genrsa -aes256 -out server.key 2048
    openssl genrsa -aria128 -out server.key 4096
    openssl genrsa -aria128 -out server.key 2048
    openssl genrsa -aria192 -out server.key 4096
    openssl genrsa -aria192 -out server.key 2048
    openssl genrsa -aria256 -out server.key 4096
    openssl genrsa -aria256 -out server.key 2048
```

2. Each time you enter one of the above commands, enter the following command.
```
    openssl req -new -key server.key -out server.csr
    cp server.key server.key.origin
    openssl x509 -req -days 3650 -in server.csr -signkey server.key -out server.crt
    openssl rsa -in server.key -text > key.pem
    openssl x509 -inform PEM -in server.crt > crt.pem
    openssl pkcs12 -export -in server.crt -inkey server.key -out cert.p12
    openssl pkcs12 -in cert.p12 -nodes -out cert.pem
```
