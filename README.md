# SFTP Client and Server

This project consists of an SFTP (Simple File Transfer Protocol) client and server that exchange one file using TCP. The client accepts 4 arguments: the name of the two files(<input> and <output>), and the IP address (or Internet name) and the port number of the server. The server starts first and waits for a connection request. The client requests a connection and then sends the name of the file <output> for output and the data in file <input> to the server, which saves the info received in the file <output>. The client reads the file and sends the data in chunks of 10 bytes. After sending the file, the client closes the connection and exits. The server receives the data and writes the file in chunks of 5 bytes. 

### Installing

First copy the contents of this folder to a directory.

Then use make to compile:

```
$ make
```

### Running on one machine(for testing)

Run the server in a terminal window to start listening for connections:

```
$ ./server 5000
```

Then, in a separate terminal window run the client to send a file:

```
$ ./client FILE_TO_TRANSFER NEW_FILENAME 127.0.0.1 5000
```

If the file transfer is a success, you should see a copied file in the directory named your NEW_FILENAME.

### Running on separate machines

On machine #1, use this command to check IP address:

```
$ ip addr show
```

Take note of the IP address under eth0.

In the same window, start your server:

```
$ ./server PORT_NO
```

Where PORT_NO is between 1024 and 65535.

Then, on machine #2:

```
$ ./client FILE_TO_TRANSFER NEW_FILENAME IP_MACHINE_1 PORT_NO
```

Where IP_MACHINE_1 is the ip obtained from the first step and PORT_NO matches the port number from the previous step.

If the file transfer is a success, you should see a copied file on machine #1 named your NEW_FILENAME.

