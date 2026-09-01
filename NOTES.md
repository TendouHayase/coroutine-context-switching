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
3. 

