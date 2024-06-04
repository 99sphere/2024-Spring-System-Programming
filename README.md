# 2024 Spring System Programming

# Team
- 201811118 이 구 
- 202211061 남민영
- 202211211 최한슬
- 202211222 황지영

# Compile & Execution
## Client
```
git clone {repo}
cd {repo_name}/client
g++ -Wall main.c read_map.c control.c run_qr.cpp -o runfile -lpthread -lwiringPi $(pkg-config opencv4 --libs --cflags)
./runfile <Server IP> <Port Number> <Local Direction>
```

## Server
```
git clone {repo}
cd {repo_name}/server
make
./Server <Port Number>
```

# Code
- main.c
  Connect to server, setup wiring Pi, global path planning and car control. 
  
- run_qr.cpp
  Running thread that detect and decode QR Code and send decoded info to server.

- read_map.c
  Running thread that recieve map info from server.
    
- control.c
  Consist with go_straight, turn_left, turn_right function.
