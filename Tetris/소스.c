#include <stdio.h>
#include <stdlib.h>
#include "block.h"
#include <time.h>
#include <Windows.h>
#include <conio.h>

#define INT_HEIGHT 20											//테트리스 스테이지 높이
#define INT_WIDTH 20											//테트리스 스테이지 너비
int stageInfo[INT_HEIGHT + 1][INT_WIDTH + 4] = { 0, };
int g_points = 0;												//점수 전역으로 초기화
int g_difficulty = 1;											//난이도 전역으로 초기화
//--------------------------------------------------------------------------------------------------------
void drawBlock(int x, int y, int n);							//최신화된 커서 위치에 원하는 블록 그림
void eraseBlock(int x, int y, int n);							//현재 그려진 블록 지우기
void setCursorPosition(int x, int y);							//커서 위치 변경
void initStage();												//스테이지 생성
int blockControl(int key, int* x, int* y, int* idx, int* rt);	//키보드 입력 받아서 명령 수행
void cursorInvisible();											//커서 블링킹 제거
int detectBlockCollision(int x, int y, int idx);				//충돌 판별
void initBlockPosition(int* x, int* y, int* idx, int* rt);		//다음 블록 생성
void checkRowComplete();										//한 줄 완성 여부 확인
void deleteRow(int row);										//완성 여부 확인되면 삭제
void redrawStage(int row);										//삭제 후 스테이지 다시 그림
void endGame();													//블록 생성하자마자 충돌 시 게임 종료
void eraseNextBlock(int n);										//다음 블록 그림 지우기
void drawNextBlock(int n, int x, int y);						//다음 블록 그림 그리기
int spawnAndCheckCollision(int* curIdx, int* idx, int* pos_x, int* pos_y, int* rotation);//충돌여부 판단하고 다음 블록 생성
void blockStraightBottom(int* pos_x, int* pos_y, int idx);		//즉시 최하단으로 이동
void updatePoints(int pos_x, int pos_y);						//점수 업데이트
void changeDifficulty(float* time);								//시간 지날 수록 난이도 상승
//--------------------------------------------------------------------------------------------------------
int main() {
	srand(time(NULL));					//시드 랜덤
	cursorInvisible();					//커서 블링킹 제거
	initStage();						//스테이지 생성

	int pos_x = 5;
	int pos_y = 0;
	int idx = (rand() % 7) * 4;
	int curIdx = idx;
	int rotation = 0;
	float time = 60.f;			//시간이 흐를수록 짧아지면 게임 난이도 상승
	initBlockPosition(&pos_x, &pos_y, &idx, &rotation);
	drawNextBlock(idx, pos_x, pos_y);

	while (1)
	{
		drawBlock(pos_x, pos_y, curIdx + (rotation % 4));

		for (int i = 0; i < time; i++)
		{
			if (_kbhit()) {//키 입력 감지
				eraseBlock(pos_x, pos_y, curIdx + (rotation % 4));

				if (blockControl(_getch(), &pos_x, &pos_y, &curIdx, &rotation)) {//아래키 눌렀을 때 바닥에 닿으면 실행
					drawBlock(pos_x, pos_y, curIdx + (rotation % 4));
					Sleep(300);
					checkRowComplete();

					if (spawnAndCheckCollision(&curIdx, &idx, &pos_x, &pos_y, &rotation))//블럭 생성하자마자 충돌나면 게임 종료
						return 0;
					updatePoints(pos_x, pos_y);
					break;//블럭이 키 입력에 의해 바닥에 닿으면 즉시 다음 동작
				}
				updatePoints(pos_x, pos_y);
				drawBlock(pos_x, pos_y, curIdx + (rotation % 4));
				Sleep(20);//연속 키 입력 속도 조절
			}
			Sleep(1);
		}

		eraseBlock(pos_x, pos_y, curIdx + (rotation % 4));
		pos_y += 1;
		if (detectBlockCollision(pos_x, pos_y, curIdx + (rotation % 4))) {//y축 자동 증가시켰을 때 충돌이 나면 다음 블럭 제공
			pos_y -= 1;
			drawBlock(pos_x, pos_y, curIdx + (rotation % 4));
			checkRowComplete();

			if (spawnAndCheckCollision(&curIdx, &idx, &pos_x, &pos_y, &rotation))//블럭 생성하자마자 충돌나면 게임 종료
				return 0;
			updatePoints(pos_x, pos_y);
		}
		changeDifficulty(&time);
	}

	return 0;
}
//----------------------------------------------------------------------------------------------------------
void initStage() {
	HANDLE hConsoleout = GetStdHandle(STD_OUTPUT_HANDLE);
	for (int i = 0; i < INT_HEIGHT; i++)					//양옆 보더 그리기
	{

		COORD pos1 = { 0,i };								//왼쪽 보더
		stageInfo[i][pos1.X] = 1;
		stageInfo[i][pos1.X + 1] = 1;
		SetConsoleCursorPosition(hConsoleout, pos1);
		printf("┃");
		COORD pos2 = { INT_WIDTH + 2,i };					//오른쪽 보더
		stageInfo[i][pos2.X] = 1;
		stageInfo[i][pos2.X + 1] = 1;

		SetConsoleCursorPosition(hConsoleout, pos2);
		printf("┃");
	}

	for (int i = 0; i < INT_WIDTH + 3; i++) {				//하단 보더 그리기
		COORD pos3 = { i,INT_HEIGHT };
		SetConsoleCursorPosition(hConsoleout, pos3);
		if (i == 0)printf("┖");
		if (i == INT_WIDTH + 2)printf("┛");
		else printf("━");
	}
	for (int i = 24; i < 36; i++) {				//다음블럭 네모
		COORD pos3 = { i,0 };
		SetConsoleCursorPosition(hConsoleout, pos3);
		if (i == 24)printf("┎");
		if (i == 35)printf("┒");
		else printf("━");
	}
	for (int i = 24; i < 36; i++) {				//다음블럭 네모
		COORD pos3 = { i,5 };
		SetConsoleCursorPosition(hConsoleout, pos3);
		if (i == 24)printf("┖");
		if (i == 35)printf("┛");
		else printf("━");
	}
	for (int i = 1; i < 5; i++)					//다음블럭 네모
	{

		COORD pos1 = { 24,i };

		SetConsoleCursorPosition(hConsoleout, pos1);
		printf("┃");
		COORD pos2 = { 35,i };


		SetConsoleCursorPosition(hConsoleout, pos2);
		printf("┃");
	}
	setCursorPosition(25, 6);				//점수, 난이도 디폴트 값 출력
	printf("POINTS:%d", g_points);
	setCursorPosition(26, 1);
	printf("NEXTBLOCK");
	setCursorPosition(25, 7);
	printf("LEVEL: %d", (g_difficulty));


	for (int i = 0; i < INT_WIDTH + 4; i++) {
		stageInfo[INT_HEIGHT][i] = 1;
	}
}
void cursorInvisible() {								//커서 블링킹 제거
	HANDLE hConsoleout = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO curCursorinfo;

	GetConsoleCursorInfo(hConsoleout, &curCursorinfo);
	curCursorinfo.bVisible = 0;
	SetConsoleCursorInfo(hConsoleout, &curCursorinfo);
}
void setCursorPosition(int x, int y) {					//커서 위치 최신화
	HANDLE hConsoleout = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos = { x,y };
	SetConsoleCursorPosition(hConsoleout, pos);
}
void drawNextBlock(int n, int pos_x, int pos_y) {//다음 블럭 출력창에 있을 블럭 그리기
	int x = 13;
	int y = 2;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockModel[n][i][j]) {
				setCursorPosition(2 * (x + j), y + i);
				printf("■");
			}
		}
	}
	setCursorPosition(pos_x, pos_y);
}
void eraseNextBlock(int n) {						//다음블럭출력창에 있는 블럭 지우기
	int x = 13;
	int y = 2;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockModel[n][i][j]) {
				setCursorPosition(2 * (x + j), y + i);
				printf("  ");
			}
		}
	}
}
void drawBlock(int x, int y, int n) {					//원하는 인덱스의 블록 출력

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockModel[n][i][j]) {

				setCursorPosition(2 * (x + j), y + i);
				stageInfo[y + i][2 * (x + j)] = 1;
				stageInfo[y + i][2 * (x + j) + 1] = 1;
				printf("■");
			}
		}
	}

	//for (int i = 0; i < INT_WIDTH + 4; i++) {						//for debug
	//	for (int j = 0; j < INT_HEIGHT + 1; j++) {
	//		setCursorPosition(INT_WIDTH + i + 4, j);
	//		printf("%d", stageInfo[j][i]);
	//	}

	//}
}
void eraseBlock(int x, int y, int n) {							//바로 직전에 그렸던 블록 지우기
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockModel[n][i][j]) {
				setCursorPosition(2 * (x + j), y + i);
				stageInfo[y + i][2 * (x + j)] = 0;
				stageInfo[y + i][2 * (x + j) + 1] = 0;
				printf("  ");
			}
		}
	}
	//for (int i = 0; i < INT_WIDTH + 4; i++) {						//for debug
	//	for (int j = 0; j < INT_HEIGHT + 1; j++) {
	//		setCursorPosition(INT_WIDTH + i + 4, j);
	//		printf("%d", stageInfo[j][i]);
	//	}

	//}
}
int detectBlockCollision(int x, int y, int n) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockModel[n][i][j]) {

				//현재 블록 모델이 1이고 stageInfo의 값도 1이면 충돌이라 판단
				if ((stageInfo[y + i][2 * (x + j)] == 1) && (stageInfo[y + i][2 * (x + j) + 1] == 1)) {
					//setCursorPosition(50, 0);
					//printf("collsion!");							//for debug
					return 1;
				}


			}
		}
	}
	return 0;
}

void initBlockPosition(int* x, int* y, int* idx, int* rt) {				//블록이 굳어진 후 다음 동작
	*x = 5;
	*y = 0;
	*idx = (rand() % 7) * 4;
	*rt = 0;
}

int blockControl(int key, int* x, int* y, int* idx, int* rt) {

	switch (key) {
	case 72: {						//블록체인지 (키보드 방향키: 상)
		*rt += 1;
		if (detectBlockCollision(*x, *y, *idx + (*rt % 4))) {//충돌시 전 동작 복귀
			*rt -= 1;
		}
	}
		   break;

	case 75: {						//좌로 이동	 (키보드 방향키: 좌)
		*x -= 1;
		if (detectBlockCollision(*x, *y, *idx + (*rt % 4))) {//충돌시 전 동작 복귀
			*x += 1;
		}
	}
		   break;
	case 77: {						//우로 이동	 (키보드 방향키: 우)
		*x += 1;
		if (detectBlockCollision(*x, *y, *idx + (*rt % 4))) {//충돌시 전 동작 복귀
			*x -= 1;
		}
	}
		   break;
	case 80: {						//하단 이동  (키보드 방향키: 하)
		*y += 1;
		g_points++;//본인 의지로 내려간 y축값만큼 점수 추가
		if (detectBlockCollision(*x, *y, *idx + (*rt % 4))) {//충돌시 전 동작 복귀
			*y -= 1;
			g_points--;
			return 1;//바닥 충돌 후 동작
		}
	}
		   break;
	case 32: {						//최하단 이동  (키보드 : 스페이스바 )
		blockStraightBottom(x, y, *idx + (*rt % 4));
		return 1;//바닥 충돌 후 동작
	}
		   break;
	default: break;
	}
	return 0;
}
void redrawStage(int row) {//줄 삭제 후 다시 그리기
	for (int i = row; i >= 0; i--) {
		for (int j = 2; j < 22; j += 2) {
			setCursorPosition(j, i);
			if (stageInfo[i][j] == 0)printf("  ");
			else printf("■");
		}
	}
}
void checkRowComplete() {//한 줄이 완성 됐는지 판단
	int cnt;
	for (int i = 19; i >= 0; i--) {
		cnt = 0;
		for (int j = 2; j < 22; j += 2) {//양 옆 보더를 제외하고 stageInfo가 1인지 확인
			if (stageInfo[i][j] == 1)
				cnt++;
			else continue;
		}
		if (cnt == 10) {//한 줄에 1이 10개면 완성됐다 판단
			g_points += 100;//한 줄 완성시 100점 추가
			deleteRow(i++);
		}
	}
}
void deleteRow(int row) {//줄 제거(위에 아래로 내리기)
	for (int i = row; i >= 0; i--) {
		for (int j = 2; j < 22; j++) {
			if (i == 0)stageInfo[i][j] = 0;
			else {
				stageInfo[i][j] = stageInfo[i - 1][j];
			}
		}
	}
	redrawStage(row);//변경된 stageInfo기반으로 스테이지 다시그리기
}
void endGame() {//stageInfo의 모든 값을 0으로 바꿈
	for (int i = 19; i >= 0; i--) {
		for (int j = 2; j < 22; j++) {
			stageInfo[i][j] = 0;
		}
	}
	redrawStage(19);//스테이지를 업데이트
	for (int i = 0; i < 3; i++) {//gameover멘트가 3번 깜빡이게 설정
		setCursorPosition(7, 9);
		printf("               ");
		Sleep(500);
		setCursorPosition(7, 9);
		printf("GAME OVER!!!");
		Sleep(500);
	}
	Sleep(3000);
	setCursorPosition(0, INT_HEIGHT + 1);
}
int spawnAndCheckCollision(int* curIdx, int* idx, int* pos_x, int* pos_y, int* rotation) {
	*curIdx = *idx;

	// 블록을 지우고, 위치를 초기화한 후, 새 블록을 그림
	eraseNextBlock(*idx);
	initBlockPosition(pos_x, pos_y, idx, rotation);
	drawNextBlock(*idx, *pos_x, *pos_y);

	// 충돌 감지: 충돌 발생 시 게임 종료
	if (detectBlockCollision(*pos_x, *pos_y, *curIdx + (*rotation % 4))) {
		endGame();
		return 1;  // 게임 종료 처리
	}

	return 0;  // 정상적으로 블록 처리 완료
}
void blockStraightBottom(int* pos_x, int* pos_y, int idx) {
	while (detectBlockCollision(*pos_x, *pos_y, idx) == 0) {//충돌이 날때까지 계속 y축 증가
		*pos_y += 1;
		g_points += 2;//본인 의지로 내려간 y축값만큼 점수 추가
	}
	g_points -= 2;
	*pos_y -= 1;


}
void updatePoints(int x, int y) {//점수 업데이트
	setCursorPosition(32, 6);
	printf("              ");
	setCursorPosition(32, 6);
	printf("%d", g_points);
	setCursorPosition(x, y);
}
void changeDifficulty(float* time) {

	if ((((g_points) / 500) >= g_difficulty) && (0 < g_points) && (0 < g_difficulty && g_difficulty < 9)) {//500점 단위로 난이도 상승
		setCursorPosition(32, 7);
		printf("              ");
		setCursorPosition(32, 7);
		g_difficulty++;
		printf("%d", g_difficulty);
		(*time) -= 5.5;					//1단계 time = 60, 9단계 time = 16
	}

	return;
}
//스테이지 그리기 파트 설명
//크게 사용자에게 보여지는 스테이지와 게임 뒤에서 돌아가고 있는 스테이지로 나뉜다. 모두 initStage()라는 함수를 사용하여 초기화 했다.
//인터넷에 여러 테트리스 게임을 서핑해서 찾은 결과 세로 20블럭, 가로 10블럭이 적당하다는 결과가 나왔다. stageInfo배열 같은 경우는 최
// 하단 및 양 옆 보더를 생각해서 세로는 한 줄 더, 가로는 블럭 하나가 두개의 인덱스를 차지하기 때문에 4줄 더 추가했다.

//블럭 생성하기 & 다음 블럭 생성하기
//하단에 쌓인 블럭을 제외하면 화면에 보여지는 블럭은 현재 내려오는 블럭과 다음 블럭 두개다. 그러기 위해서는 가장 초기에 블럭 두개를 랜
//덤으로 뽑아야한다. idx라는 변수를 먼저 랜덤으로 초기화 해 curIdx에 값 복사를 해줬고 다시 initBlockPosition()함수르 통해 idx를 최신화
//시켰다. 다음에 나와야될 블럭을 idx에 버퍼를 뒀다고 보면 된다. 블럭 출력은 매우 간단한데 현재 pos를 기준으로 반복문을 돌면서 blockModel[]
//배열이 1이면 네모 상자를 출력 시켰고 stageInfo[]같은 경우는 해당위치를 1로 바꿨다. 유의할 점은 가로는 두개의 인덱스를 차지하기 때문에
//2개의 인덱스를 1로 변경해야한다. 다음에 나올 블럭 출력하는 것도 원리는 동일하다.

//블럭 삭제하기
//블럭을 출력하려면 전에 있던 블럭을 화면에서 삭제해야한다. 마찬가지로 현재 pos를 기준으로 blockModel[]의 값이 1이면 지우고 그렇지 않으면
//안지우면 된다. stageInfo[]도 역시 값을 0으로 바꿔줘야한다. 이후에 들어온 입력에 따라서 해당 pos에 블럭을 출력 시키면 된다.

//블럭 컨트롤하기(충돌판단)
//스페이스바, 방향키는 고유의 ASCII코드가 있다. 이를 이용해서 blockControl()에서 블럭을 조종했다. 상키를 누를 경우 블럭을 회전 시킬 수 있고
//하키를 누르면 pos_y++, 좌,우키를 누르면 pos_x--,pos_x++동작을 할 수 있다. 스페이스바를 누를 경우 최하단으로 즉시 이동한다. 모든 동작에는
//충돌판단 함수 detectBlockCollision()이 존재한다. 매우 간단한 원리로 작동하는데 블럭을 그리는 함수와 그 원리가 동일하다. 사용자가 입력을
//주면 그에 맞게 값이 바뀌는데 어떤 값이 변경됐을 때 상태가 blockModel[]의 값도 1이고 동시에 stageInfo[]의 값도 1이라면 충돌이라
//판단하고 전 동작으로 복귀한다. 스페이스바를 누르면 실행되는 blockStraightBottom() 함수도 이를 이용한것인데 블럭이 충돌판단 날때까지pos_y를
//증가시키면서 무한반복을 돈다.

//한 줄 완성 판단
//checkRowComplete()로 판단을 하게 되는데 이는 블럭이 시간에 의해 바닥에 닿았을 때, 사용자가 하키 및 스페이스바를 눌러서 바닥에 닿았을 때 실행된다.
//논리상 최하단에 있는 stageInfo[]부터 검사를 하며 검사된 1의 값이 10이면 한줄이 완성됐다 판단해 deleteRow()를 실행시킨다. 지워질 배열부터 논리상 위에
// 있는 배열의 값을 복사하고 가장 양옆 두개의 배열은 보더이므로 무시한다. 재완성된 stageInfo를 바탕으로 redrawStage()를 실행시킨다. cpu time을 조금이나마
// 줄이기 위해서 전체를 다시 그리는 것이 아니라 지워질 배열의 위치부터 다시 그렸다. 배열이 지워지면 인덱스가 하나 줄기 때문에 i++를 무조건 해야된다.
// 이 작업을 하지 않으면 두줄 연속으로 지워지지 않는다.

//게임 종료 및 게임 스태터스
//테트리스의 게임 종료 조건은 모든 블럭이 쌓여서 더 이상 다음 블럭이 쌓일 수 없을 때이다. 이를 이용해 종료조건을 만들었다. 사용자가 현재 사용하고 있는
// 블럭이 바닥에 닿게 되면 다음 블럭이 나온다. 이때 detectBlockCollision()를 실행시켰다. 다음에 나올 블럭이 나오기도 전에 충돌판단이 나면 게임을 종료
// 시키는 것이다. 해당 조건에 부합해서 게임이 종료되게 되면 GAME OVER!!!메시지가 3회 깜빡이면서 게임이 종료가 된다.
// 레벨 및 점수같은 게임 스태터스는 initStage()에서 출력해줬고 게임을 하면서 실시간으로 값들만 업데이트하게 했다. 점수는 사용자에 의해서 블럭의 y값이
// 증가했을 때 1점, 블럭을 스페이스바에 의해서 최하단으로 내려갔을 때 내려간 칸 수의 2배점, 한 줄이 완성 됐을 때 100점을 부여했다. 레벨은 최초에 1레벨이고
// 사용자의 점수가 500점 오를 때 마다 1레벨씩 오르고 가장 어려운 레벨은 9로 했다. 레벨이 오를 때마다 time은 5.5f씩 줄어든다. 여러 테스트를 한 결과 이게
// 가장 적당한 난이도인 것 같아 설정했다.
