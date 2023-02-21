# 무한 전광판 만들기
![무한전광판](https://github.com/forearth/infinite_message_board/blob/main/imgs/infinite_board.png?raw=true)

## 사용방법
1. 전원 연결하기
2. AP에 연결하기(Wifi에서 "*** BOARD" 연결)
3. AP 연결 비밀번호 입력(기본값 12345678)
4. 브라우저앱을 통해 '192.168.4.1'로 접속
5. 영문으로 메시지 변경


## 회로 구성
![무한전광판 회로](https://github.com/forearth/infinite_message_board/blob/main/imgs/circuit.png?raw=true)

### 전선 연결
#### LED strip
* 5V > 3.3V
* DIN > D2
* GND > GND

#### MAX7219
* VCC > 3.3V
* GND > GND
* DIN > D7
* CS > D8
* CLK > D5