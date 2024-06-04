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

# Function
- main.c      
  Connect to server, setup wiring Pi, global path planning with greedy algorithm and car control. 
  
- run_qr.cpp     
  Running thread that detect and decode QR Code and send decoded info to server.

- read_map.c     
  Running thread that recieve map info from server.
    
- control.c     
  Consist with go_straight, turn_left, turn_right function.

# Path Planning
control.c에서 한 칸 전진, 제자리에서 좌, 우 방향으로 회전하는 기능만을 제공하므로, map을 기준으로 하는 globla direction과 현재 차량의 방향을 기준으로 하는 local direction 정보를 유지한다.
0, 1, 2, 3은 각각 상, 우, 하, 좌를 의미한다. 
현재 차량의 위치를 기준으로, 상, 하, 좌, 우 방향의 다음 node의 정보를 확인한다. item이 존재하지 않는 경우, score가 0인 item으로 간주하여 주변 node 가장 높은 점수를 갖는 node로 이동한다. 상, 좌, 우 방향이 모두 trap으로 둘러싸인 경우, 180도 회전하여 빠져나간다.

# Set Bomb Strategy

