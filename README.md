#1. 프로그램 설명
##1.1. SIC ASEEMBLER
어셈블리 언어를 컴파일하고 오브젝트코드를 생성할 수 있는 컴파일러를 적절한 자료구조를 사용하여 구현하였다.

###1.1.1. SIC
가상의 컴퓨터로 실제 컴퓨터 명령어를 단순화시켜 이해하기 수월하도록 만들어 놓은 것으로, 실제의 Machine에서 자주 쓰이는 특징들이 포함 되어있다.
SIC는 8비트가 하나의 Byte로 구성되며 3개의 이어진 Byte를 Word라고 하며 최하위 바이트에 주소를 지정한다. 데이터 포맷은 Character(8bits)와 Integer(24bits)로 표현한다

#2. 생성파일
##2.1. Intermed File
오브젝트 프로그램을 작성하기 위해 사용하기 위한 중간 파일로 코드와 그 주소가 저장된다.

##2.2. Symbol Table
Symbol Table은 컴파일러 같은 언어 변환기에서 사용되는 데이터 구조이며 코드에서 참조되는 심볼들의 이름과 주소가 저장되어 있다. 즉 symbol(operand)과 storage되는 주소를 가진다. 

##2.3. Mnemonic Tree
opcode의 주소 값은 배열을 반복적으로 탐색해야 한다. 이를 해결하기 위해 mnemonic1값과 code1값을 AVL Tree로 구성하였다.

#3. 자료구조
##3.1. Linked List
Intermed 파일을 작성하기 위해 Linked List 구조를 사용

##3.2. AVL Tree
Symbol Tabe과 Mnemonic Tree 작성하기 위해 AVL Tree 사용




