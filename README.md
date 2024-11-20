A private and easy to configue messaging program through SSH connection!

Simply buy a cheap VPS, get IP and create users. 

Use the credentials and connection sshchat program to your VPS.

Agree on a encryption key with other parties that you wish to chat with.


---
How to:
update the system using:
```
sudo apt update
```
install the required libs using:
```
sudo apt install qtbase5-dev qtcreator qtbase5-dev qt5-qmake libqt5widgets5 build-essential libcrypto++-dev libcrypto++-doc libcrypto++-utils libboost-all-dev
```
run qmake:
```
qmake
```
install sshpass:
```
sudo apt install sshpass
```
then compile the program using:
```
make
```
then run sshchat-v1:
```
./sshchat-v1
```
---
screenshot:

IP: your VPS ip address (make sure sshd is running)

User: your VPS username

Pass: your VPS password

Port: the ssh port (typically 22 if you have not changed it)

chat name: choose a name to be used in your chat

En. Key: encryption password/key that you agreed upon -- this password must be same for the other person you wish to chat with


![alt text](https://github.com/msbCyricTohoku/sshchat/blob/main/screenshot/img2.png)
---
![alt text](https://github.com/msbCyricTohoku/sshchat/blob/main/screenshot/img.png)


Enjoy the simple and private chat! 



