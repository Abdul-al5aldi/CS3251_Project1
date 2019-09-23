# CS3251_Project1
The goal of this project is to familiarize the student with **sockets programming in C.** Students will create a client that sends a string to a server, and a server that responds with a value, then enhancing the client and server so that total of 3 message types *[BAL - WITHDRAW - TRANSFER]* are supported. as well as adding a timeout function.

#

## Server Arguments Format:
``./server port``
  - port is the port number (e.g., 2019)
  
###

## Client Arguments Format:
``./client CMD [account] [amount] ipAddr port``

  - CMD is one of BAL|WITHDRAW|TRANSFER
  - account is one or two (for TRANSFER) of myChecking|mySavings|myCD|my401k|my529
  - For WITHDRAW and TRANSFER, [amount] follows the acount name
  - idAddr is the ip address for the server (e.g., 127.0.0.1)
  - port is the port number (e.g., 2019)#

###

##

###

### BAL:
Expected output:
> Account [myCD] has balance of = [$553].


###


### WITHDRAW:
Expected outputs:
- If succeed:
> Withdraw done successfully, new balance for account [myCD] = [$409].
- If amount > balance:
> Error: insufficient funds.
- If attempt to do more than 3 withdraws in 1 minute:
> Error: too many withdraws in a minute, please try again later.


###


### TRANSFER
Expected outputs:
- If succeed:
> Transfer done successfully, new balance for account [mySavings] = [$52] & for [myCD] = [$210].
- If amount > balance:
> Error: insufficient funds.

###
