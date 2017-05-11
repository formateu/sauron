# Wireshark sauron plugin

### Uruchomienie
```
wireshark -X lua_script:plugin.lua
```

### Testowanie
Po uruchomieniu odpalamy nasłuchiwanie na interfejsie lo.  
Następnie wysyłamy przykładowy komunikat na port 7777:
```
echo -n -e '\x00\x00\x00\x01\x00\x00\x00\x10' >> /dev/udp/localhost/7777
```
W Wiresharku powinien pojawić się przechwycony pakiet.
Wprowadzamy modyfikację do pluginu i przeładowywujemy go skrótem Ctrl+Shift+L
(nie ma potrzeby po przeładowaniu wysyłania kolejnego, tego samego pakietu; zostanie on w logu).
