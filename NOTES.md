## 설계

- 컨텍스트 핸들 : rsp
- context_switching 인자 : 목적지 rsp
- context_switching 리턴값 : 나를 resume한 쪽 rsp
- cooperaive 방식

## 트라이 기록 
1. printf 내부 ZwAllocateVirtualMemory에서 Break instruction exception - 80000003, WRONG_SYMBOLS 발생
    - PEB 내부 PIB의 StackBase와 StackLimit를 수정하지않아 발생한 것으로 추정
    - 윈도우에서 user 모드의 IA32_GS_BASE는 TEB를 가르키기에 gs+08h 및 gs+10h로 접근하여 인자로 받은 rsp를 StackBase를 계산
    - StackLimit는 목적지 rsp의 8바이트 위에 스택 크기를 저장하는 것으로 해결
2. printf 내부 RtlpAllocateNTHeapInternal에서 Access violation - code c0000005 발생
    - mov qword ptr [r11-30h], rsi 에서 발생
    - !address로 확인결과 r11은 코루틴 생성자에서 정상적으로 할당받은 페이지지만 r11-30h는 할당받지않은 페이지임
    - 정확히 r11-2fh까지만 할당받은 페이지
    - 추가적으로 rsp도 할당받지 않은 페이지, rsp = r11 - 88h임
    - 단순 스택 오버플로우였음. 스택 크기 늘리니 해결
3. 할당받은 페이지 외부를 접근해서 계속 Access violation - code c0000005 발생
   - 아예 다시 설계하여 코루틴 측 StackBase에 CoroutineFrame이란 구조체 배치하기로 결정
   - CoroutineFrame에는 자신의 베이스 주소, 스택 크기, 코루틴의 rsp, 코루틴 호출자의 rsp, 호출자 측 TEB의 StackBase, StackLimit, 종료 여부, 생성자, 소멸자, 클로저 주소 저장
   - C++ 코드에서 TEB 갱신
   - 첫 진입때 switching_context를 그대로 쓰기위해 코루틴 객체 생성할때 코루틴 스택에 트랩 프레임 생성

