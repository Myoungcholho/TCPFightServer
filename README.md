# 📌프로젝트 개요
<p align="center">
  <img
    width="800"
    src="https://github.com/user-attachments/assets/fa86d4bc-949f-45dd-81ea-27795a1911d1"
    alt="Server" />
</p>

클라이언트 실행 파일을 WireShark로 직접 분석해 구현한 서버 프로그램입니다.  
패킷을 담는 링 버퍼 컨테이너를 설계하고, 공격·이동 검증부터 Broadcast 처리, 유저 관리까지 수행하고 있습니다.

| 항목 | 내용 |
|------|------|
| 🕒 개발 기간 | 2023.02 ~ 2023.03 (40일) |
| 👤 개발 인원 | 1명 |
| 🧰 개발 환경 | C |
| 🛠 실행 및 디버깅 툴 | VS2022, WireShark |
| 📦 라이브러리 | Socket |

</br>

# 📘목차
- [구현 요약 내용](#구현-내용-목차-이동)
- [핵심 주요 코드](#핵심-주요-코드-목차-이동)
- [문제 해결 경험(트러블 슈팅)](#문제-해결-경험트러블-슈팅-목차-이동)
- [프로젝트에서 얻은 것](#프로젝트에서-얻은-것-목차-이동)
- [개발 계기](#개발-계기-목차-이동)

</br>

# 📘구현 내용 [(목차 이동)](#목차)

| 상세 설명 링크 | 구현 요약 |
|----------------------|------------------|
| [🧱 TCP 통신용 경량 패킷 모듈](#core) | 고정 버퍼 기반 `<< / >>` 연산자 직렬화·역직렬화 처리 |
| [🌍 패킷 처리용 링버퍼 자료구조](#world) | 입력 버퍼 순환 처리 및 안정적인 스트림 관리 |
| [🎨 패킷 헤더 파싱 & 브로드캐스트](#rendering) | 이동·공격·데미지 처리 및 다수 클라이언트 동기화 |
| [🕺 비정상 입력 유효성 검증 로직 ](#animation) | 좌표 연타 등 비정상 패킷 필터링 및 보안 처리 |

</br>

# 📘핵심 주요 코드 [(목차 이동)](#목차)

| 코드 파일 | 코드 설명 |
|----------|-----------|
| RingBuffer [.h](https://github.com/Myoungcholho/TCPFightServer/blob/master/TCPserializationFightProject/RingBuffer.h) / [.cpp](https://github.com/Myoungcholho/TCPFightServer/blob/master/TCPserializationFightProject/RingBuffer.cpp) | 클라이언트에서 들어오는 패킷을 순차적으로 저장·관리하는 링 버퍼 컨테이너입니다. |
| CPacket [.h](https://github.com/Myoungcholho/TCPFightServer/blob/master/TCPserializationFightProject/CPacket.h) / [.cpp](https://github.com/Myoungcholho/TCPFightServer/blob/master/TCPserializationFightProject/CPacket.cpp) | 패킷 헤더·바디를 직렬화/역직렬화해 다루는 패킷 전용 유틸리티 클래스입니다. |
| main [.h](https://github.com/Myoungcholho/TCPFightServer/blob/master/TCPserializationFightProject/main.h) / [.cpp](https://github.com/Myoungcholho/TCPFightServer/blob/master/TCPserializationFightProject/main.cpp) | 플레이어 접속 관리, 서버 바인딩·리스닝, 공격/이동 처리 등 핵심 서버 로직이 구현된 엔트리 포인트입니다. |

</br>

# 📘문제 해결 경험(트러블 슈팅) [(목차 이동)](#목차)

<table style="border-collapse:collapse;">
  <tr>
    <th width="350" style="border:2px solid #ffb3b3; background:#ffe1e1;">
      📂 이동 좌표 지속 송신으로 인한 트래픽 증가 문제
    </th>
    <th width="350" style="border:2px solid #ffd27f; background:#fff1d6;">
      📚 비정상 공격 패킷 반복 전송 문제
    </th>
    <th width="350" style="border:2px solid #c3c3ff; background:#e9e9ff;">
      🧾 삭제된 객체 접근으로 인한 크래시 문제
    </th>
  </tr>

  <tr>
    <td width="350" style="border:2px solid #ffb3b3; background:#ffe1e1; vertical-align:top;">
      많은 유저 트래픽으로 발생한 지연 문제를 트래픽 감소와 서버 유효성 검사로 해결했습니다.
      <br><br>
      <a href="#t0">[상세설명]</a>
    </td>
    <td width="350" style="border:2px solid #ffd27f; background:#fff1d6; vertical-align:top;">
      클라이언트의 비정상 공격·패킷 송신을 서버에서 방어 검증으로 차단했습니다.
      <br><br>
      <a href="#t1">[상세설명]</a>
    </td>
    <td width="350" style="border:2px solid #c3c3ff; background:#e9e9ff; vertical-align:top;">
      중간에 유저가 이탈하거나 삭제될 때 발생하던 크래시 문제를 지연 삭제 방식으로 해결했습니다.
      <br><br>
      <a href="#t2">[상세설명]</a>
    </td>
  </tr>
</table>

## 1. 이동 좌표 지속 송신으로 인한 트래픽 증가 문제 <a id="t0"></a> [(트러블 슈팅 목록 이동)](#문제-해결-경험트러블-슈팅-목차-이동)

<table>
  <tr>
    <td style="border:2px solid #4fa3ff; border-radius:8px; padding:12px 16px; background:#050812;">
      <strong>🧩 문제</strong>
      <ul>
        <li> 동시 접속자가 많은 환경에서 패킷 송·수신이 과도하게 발생 </li>
        <li> 트래픽 포화로 서버 로직이 지연되며 움직임·공격 판정이 늦게 반영되는 문제가 발생 </li>
      </ul>
      <strong>🔍 원인 분석</strong>
      <ul>
        <li> 플레이어 좌표를 매 프레임 송신하는 구조라, 불필요한 트래픽과 헤더 비용이 과도하게 증가함 </li>
      </ul>
      <strong>🛠 해결</strong><br>
      <ul>
        <li> 좌표 변화나 공격 발생이 일어날 때 이벤트 기반 송신 방식으로 전환함 </li>
        <li> 중요 시점에 패킷을 받고 서버가 검증을 통해 상태를 확정하도록 변경함 </li>
      </ul>
      <strong>✅ 결과</strong><br>
      <ul>
        <li> 이벤트 기반 전송으로 송·수신 패킷 수가 크게 감소하고, 응답 지연도 눈에 띄게 개선 </li>
      </ul>
      <strong>📚 배운 점</strong>
      <ul>
        <li>네트워크 설계는 처음부터 전송량을 최소화하는 방향으로 구조를 잡아야 한다는 점 </li>
        <li>로직 지연뿐 아니라, 버퍼 수용량을 초과하면 패킷 유실·트래픽 폭증 등 복합적인 장애로 이어진다는 점 </li>
      </ul>
    </td>
  </tr>
</table>

---

## 2. 비정상 공격 패킷 반복 전송 문제 <a id="t1"></a> [(트러블 슈팅 목록 이동)](#문제-해결-경험트러블-슈팅-목차-이동)

<table>
  <tr>
    <td style="border:2px solid #ffd27f; border-radius:8px; padding:12px 16px; background:#120d05;">
      <strong>🧩 문제</strong>
      <ul>
        <li> 클라이언트가 공격 입력을 매우 빠르게 보내면, 의도와 다르게 공격이 계속 나갈 수 있었음 </li>
      </ul>
      <strong>🔍 원인 분석</strong>
      <ul>
        <li> 공격 패킷 반복 전송 시 이를 검증할 서버 보호 로직이 없었기 때문 </li>
      </ul>
      <strong>🛠 해결</strong><br>
      <ul>
        <li> 플레이어별 쿨타임 저장 → 패킷 수신 시 유효 여부 서버가 직접 검증하도록 변경 </li>
        <li></li>
      </ul>
      <strong>✅ 결과</strong><br>
      <ul>
        <li> 치트성 공격 패킷을 차단해 전투 로직이 의도대로 동작하도록 개선 </li>
      </ul>
      <strong>📚 배운 점</strong>
      <ul>
        <li> 클라이언트 입력을 전적으로 신뢰하면 안 된다는 원칙의 중요성을 깨달음 </li>
        <li> 어떤 패킷이 들어올지 알 수 없으므로, 서버 레벨의 방어·검증 로직이 필수임을 배움 </li>
      </ul>
    </td>
  </tr>
</table>

---

## 3. 삭제된 객체 접근으로 인한 크래시 문제 <a id="t2"></a> [(트러블 슈팅 목록 이동)](#문제-해결-경험트러블-슈팅-목차-이동)

<table>
  <tr>
    <td style="border:2px solid #a8ddff; border-radius:8px; padding:12px 16px; background:#050a12;">
      <strong>🧩 문제</strong><br>
      <ul>
        <li> 객체(유저) 삭제 시 크래시가 발생하는 문제 발생 </li>
      </ul>
      <strong>🔍 원인 분석</strong><br>
      <ul>
        <li> 다른 플레이어가 다른 유저를 참조하고 있는데 객체가 죽어버려서 접근했을 때 크래시가 발생했던 것임</li>
        <li></li>
      </ul>
      <strong>🛠 해결</strong>
      <ul>
        <li> 삭제가 필요한 객체를 즉시 삭제하지 않고 OutQueue에 모아둠 </li>
        <li> 프레임 끝에서 일괄 삭제하도록 처리 방식 변경함 </li>
      </ul>
      <strong>✅ 결과</strong><br>
      <ul>
        <li> 로직 실행 중에는 객체 접근이 안전하게 보장됨 </li>
        <li> 삭제 시점을 프레임 끝으로 고정해 명확하게 관리할 수 있게 됨 </li>
      </ul>
      <strong>📚 배운 점</strong>
      <ul>
        <li> 삭제된 객체 접근 크래시를 통해 객체 수명 관리와 삭제 시점 설계의 중요성을 체감함 </li>
        <li> 지연 삭제 개념을 '이론'이 아니라 직접 문제를 겪고 해결한 '경험'으로 쌓으면서 참조 안전성에 더 민감해짐 </li>
        <li> 이 경험을 계기로 언리얼의 객체 라이프사이클·GC 방식에도 관심을 가지며 더 견고한 구조 설계의 필요성을 깨달음 </li>
      </ul>
    </td>
  </tr>
</table>

</br>

# 📘프로젝트에서 얻은 것 [(목차 이동)](#목차)

| 번호 | 얻은 경험 |
|------|-----------|
| 1 | [TCP 송·수신 예외 처리 경험](#gain-drawcall) |
| 2 | [공격 패킷 연타 등 비정상 입력 검증 로직 구현 경험](#gain-ue-arch) |
| 3 | [삭제 객체의 안전한 관리 전략](#gain-cpp-resource) |
| 4 | [링버퍼 기반 패킷 처리 구조 설계 경험](#gain-gt-rt) |

---

### 1. TCP 송·수신 예외 처리 경험 <a id="gain-drawcall"></a> [(⬆표로 이동)](#프로젝트에서-얻은-것-목차-이동)

직접 소켓을 연결하고 서버를 열어 보면서, 이론으로만 알던 네트워크 흐름을 코드 단위에서 따라가 볼 수 있었습니다.  
임시 헤더를 만들어 패킷을 확인하는 과정에서 TCP 헤더가 왜 필요한지 더 쉽게 이해할 수 있었고,   
평소 접할 일이 거의 없던 빅엔디언·리틀엔디언 개념도 포트 처리에 직접 적용해 보는 좋은 기회가 되었습니다.

---

### 2. 공격 패킷 연타 등 비정상 입력 검증 로직 구현 경험 <a id="gain-ue-arch"></a> [(⬆표로 이동)](#프로젝트에서-얻은-것-목차-이동)

게임 클라이언트·서버는 단순히 입력만 처리하는 것이 아니라, 세밀한 방어 로직이 필수라는 것을 체감했습니다.  
치터, 비정상 패킷, 잘못된 클라이언트 입력까지 모두 상정해 설계해야 한다는 점도 함께 깨달았습니다.

---

### 3. 삭제 객체의 안전한 관리 전략 <a id="gain-cpp-resource"></a> [(⬆표로 이동)](#프로젝트에서-얻은-것-목차-이동)

지연 삭제를 직접 적용해 보며 왜 이런 방식이 필요한지 몸으로 체감했고, 안정성을 지키는 것이 프로그래머의 핵심 역할임을 다시 느꼈습니다.  
또, 이 경험이 상용 엔진의 객체 라이프사이클을 이해하는 데에도 많은 도움이 되었습니다.

---

### 4. 링버퍼 기반 패킷 처리 구조 설계 경험 <a id="project04"></a> [(⬆표로 이동)](#프로젝트에서-얻은-것-목차-이동)

크래시가 났을 때 컨테이너 문제인지 서버 로직 문제인지 구분하기 어렵다는 걸 겪으며, 모듈 단위로 따로 떼어 검증하는 테스트의 중요성을 체감했습니다.  
랜덤 문자열을 1일간 인큐·디큐하는 자동 테스트를 만들면서, 이런 반복·자동 검증을 설계하는 것도 프로그래머의 중요한 역할이라는 걸 배웠습니다.


</br>

# 📘개발 계기 [(목차 이동)](#목차)

온라인 멀티플레이의 구조가 어떻게 동작하는지에 대한 궁금증과, 이론으로만 배우는 네트워크가 아니라, 실제로 클라이언트–서버 구조를 직접 구현하며 데이터가 오가는 흐름을 체감해 보고 싶어 본 프로젝트를 시작했습니다.
