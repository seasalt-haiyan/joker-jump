#include <stdio.h>
#include <easyx.h> 
#include <stdlib.h>
#include <time.h>
#include "tools.h" //����͸��ͼƬ��ͷ�ļ� 

#define LENGTH 800 //���ڳ��� 
#define WIDTH 500 //���ڿ�� 

//ȫ�ֱ��� 
ExMessage msg = {0}; //�����Ϣ 
int score = 0;//���� 
int highscore = 0;//��߷��� 
int gravity = 2; // ����ֵ
int jumpStrength = -33; // ��Ծ�ĳ�ʼ�ٶ�
int obstacleSpeed = -10; // �ϰ����ˮƽ�ٶ�
int spawnRate = 100; // �ϰ�����ֵļ��ʱ��
bool gameRunning;

//ͼƬ��Դ 
IMAGE img_gameover,img,img1,img2,img3,img_platform,img_player,img_obstacle,img_background,img_about;

//��ң��ϰ���ṹ�� 
typedef struct {
    int x, y, width, height;
    bool isJumping;
    int velocityY;
    int type;
} GameObject;

GameObject player, obstacle;

//��Ϸ��ʼ�� 
void initGameObjects() {
    player.x = 100;
    player.y = WIDTH - 60 - 60; // ����߶ȼ�ȥplayer�߶�
    player.width = 60;
    player.height = 60;
    player.isJumping = false;
    player.velocityY = 0;

    obstacle.x = LENGTH - 220;
    obstacle.y = WIDTH - 60 - 80; // ����߶ȼ�ȥobstacle�߶�
    obstacle.width = 220;
    obstacle.height = 80;
}

//����Ƿ���ײ 
bool checkCollision(GameObject *a, GameObject *b) {
    if (a->x < b->x + b->width && a->x + a->width > b->x && a->y < b->y + b->height && a->y + a->height > b->y) {
        return true;
    }
    return false;
}


//�ж�mx,my�Ƿ���ָ�������� 
bool inArea(int mx, int my,int x, int y, int w, int h)
{
	if(mx > x && mx < x + w && my > y && my < y + h)
	{
		return true;
	}
	return false;
}

//����һ�������İ�ť 
bool button(int x, int y, int w, int h, const char* text)
{
	if(inArea(msg.x,msg.y,x,y,w,h))
	{
		setfillcolor(RGB(255,250,250));
	}else{
		setfillcolor(RGB(220,220,220));		
	}

	fillrectangle(x,y,x+w,y+h);
	int hSpace = (w - textwidth(text)) / 2;
	int vSpace = (h - textheight(text)) / 2;
	outtextxy(x +hSpace,y+vSpace,text);
	
	if(msg.message == WM_LBUTTONDOWN && inArea(msg.x,msg.y,x,y,w,h))
	{
		return true;
	}
	return false;
}

//��ʾ�����ĺ��� 
void displayScores() {
    char scoreStr[20];
    char highScoreStr[20];

    sprintf(scoreStr, "����: %d", score);
    sprintf(highScoreStr, "��߷���: %d", highscore);
    
    outtextxy(10, 10, scoreStr);
    outtextxy(10, 30, highScoreStr); 
}

//������Ļ���� 
void gameOverScreen() {
	
    if (score > highscore) {
        highscore = score; // ������߷���
        }
            
    BeginBatchDraw();
    cleardevice();
    putimage(0,0,&img_background);
    displayScores();
    EndBatchDraw();
    FlushBatchDraw(); 

    while (true) {
    	
        BeginBatchDraw(); // ��ʼ��������

        // ���ư�ť
        button(LENGTH / 2 - 100, WIDTH / 2, 200, 50, "���¿�ʼ");
        button(LENGTH / 2 - 100, WIDTH / 2 + 70, 200, 50, "�������˵�");
        settextstyle(50,25,"����");
		outtextxy(280, WIDTH / 2 - 70,"GAME OVER!");
		settextstyle(20,10,"����");
        EndBatchDraw();
        FlushBatchDraw(); // ˢ����Ļ��ʾ�Ա�����˸

        if (peekmessage(&msg, PM_REMOVE)) {
            // �����Ϣ�����������
            if (msg.message == WM_LBUTTONDOWN) {
                // ��������¿�ʼ����ť�����
                if (button(LENGTH / 2 - 100, WIDTH / 2, 200, 50, "���¿�ʼ")) {
                    initGameObjects(); // ���³�ʼ����Ϸ����
                    score = 0; // ��������Ϊ0
                    FlushBatchDraw();
                    flushmessage(-1);
                    return; // �˳�gameOverScreen������ִ��gameLoop
                }
                // ������������˵�����ť�����
                if (button(LENGTH / 2 - 100, WIDTH / 2 + 70, 200, 50, "�������˵�")) {
                    gameRunning = false; // ������Ϸ����״̬Ϊfalse
                    while (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                        Sleep(10); // �����ӳٵȴ�����ͷ�
                    }
                    flushmessage(-1);
                    Sleep(100);
                    return; // �˳�gameOverScreen�����ص��˵�
                }
            }
    	}
    }
}





void gameLoop() {
    initGameObjects(); // ��ʼ����Ϸ����
    bool gameRunning = true;
    bool gameStarted = false;
    int scoreCounter = 0; // �Ʒ���
    int frameCounter = 0; // �����ϰ������ɵ�֡������

	BeginBatchDraw();
    cleardevice();
    putimage(0, 0, &img_background); // ����
    // ���Ƶ���ͽ�ɫ
    for (int i = 0; i < LENGTH; ++i) {
        putimage(i * 60, WIDTH - 60, &img_platform); // ����
    }
    drawImg(player.x, player.y, &img_player); // ��ɫ
    EndBatchDraw();
    FlushBatchDraw();
    Sleep(100);
    flushmessage(-1);
    while (!gameStarted) {
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {//���������¿�ʼ��Ϸ 
            gameStarted = true;
        }
        Sleep(50); 
    }
    
    while (gameRunning && gameStarted) {
    
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) { // ������������Ծ 
    if (!player.isJumping) {
        player.isJumping = true;
        player.velocityY = jumpStrength; // ����һ�����ϵĳ��ٶ�
        msg.message = 0;
    }
}

        // ��Ծ�������߼�
        if (player.isJumping) {
            player.y += player.velocityY; // Ӧ�õ�ǰ�ٶ�
            player.velocityY += gravity; // Ӧ��������ʹ�ٶ�����

            // ����Ƿ����
            if (player.y >= WIDTH - 60 - player.height) {
                player.y = WIDTH - 60 - player.height; // ���õ�����λ��
                player.isJumping = false; // ֹͣ��Ծ
                player.velocityY = 0;
            }
        }
        // �ϰ��������߼�
       obstacle.x += obstacleSpeed;
        frameCounter++;
        if (frameCounter >= spawnRate) {
        	spawnRate = 60 + (rand() % 30);// ������ɼ�� 
        	obstacleSpeed = -25 - (rand() % 6); // ����ٶ� 
            obstacle.x = LENGTH; // ���ϰ����������Ļ�Ҳ�
            frameCounter = 0;
            obstacle.type = rand() % 2;// ����ϰ�������
         if (obstacle.type == 1) {
                obstacle.y = 170;//�����ϰ��� 
            }else if(obstacle.type == 0){
            	obstacle.y = WIDTH - 60 - 80;
			}
        }
         
        // �����⵽��ײ����Ϸ���� 
        if (checkCollision(&player, &obstacle)) {
        	flushmessage(-1);
            gameOverScreen(); // ��ʾ��Ϸ��������
            if (!gameRunning) {
        break; // ����gameLoopѭ�������ص����˵�����
    }
            break; // �˳���Ϸѭ��
        }
    
        // ���·���
        scoreCounter++;
        if (scoreCounter >= 1) { // ÿ��1֡����1��
            score++;
            scoreCounter = 0;
        }
        
        BeginBatchDraw();
        cleardevice(); // ����
        putimage(0,0,&img_background);

        // ���Ƶ���
        for (int i = 0; i < LENGTH ; ++i) {
            putimage(i * 60, WIDTH - 60, &img_platform); 
        }

        // ����player
            drawImg(player.x, player.y, &img_player);

        // ����obstacle
            drawImg(obstacle.x, obstacle.y, &img_obstacle);
            
        displayScores();//��ʾ���� 

        EndBatchDraw();
        FlushBatchDraw();
        
        Sleep(14); // ����֡�� 

    }
}

//������ 
int main()
{
	//����� 
	srand((unsigned)time(NULL));
	
	//����ͼƬ��Դ 
	loadimage(&img,_T("assest/xiaochou.png")) ;
	loadimage(&img1,_T("assest/xiaochou.jpg")) ;
	loadimage(&img2,_T("assest/1.jpg")) ;
	loadimage(&img3,_T("assest/2.jpg")) ;
	loadimage(&img_platform,_T("assest/platform60x60.jpg"));
	loadimage(&img_player,_T("assest/60x60player.png"));
	loadimage(&img_obstacle,_T("assest/220x80bat.png"));
	loadimage(&img_gameover,_T("assest/gameover.jpg"));
	loadimage(&img_background,_T("assest/background.jpg"));
	loadimage(&img_about,_T("assest/about.jpg"));
	
	
	//��ʼ��ͼ 
	initgraph(LENGTH,WIDTH);
	MENU://��������ѡ��ص����˵�goto������
	//���ò��� 
	setbkcolor(WHITE);
	setlinecolor(BLACK) ;
	cleardevice();
	settextstyle(100,50,"����");

	putimage(650,20,&img);
	putimage(20,20,&img1);
	settextcolor(BLACK); 
	setbkmode(TRANSPARENT); 
	outtextxy(150,30,"С��������"); 
	
	int rx = 250;
	int ry = 200;
	int rw = 300;
	int rh = 60;
	
	settextstyle(20,10,"����");
	outtextxy(680, 480,"version 1.0");
	
	//�˵����棬��ʾ����ʼ��Ϸ�����ڣ��˳���Ϸѡ�� 
	while(true)
	{
		peekmessage(&msg,EX_MOUSE);
		BeginBatchDraw();
		if(button(rx,ry,rw,rh,"��ʼ��Ϸ"))
		{
			cleardevice();
			putimage(0,0,&img2);
			EndBatchDraw();
			settextstyle(20,10,"����");
			flushmessage(-1);
			while(true){
				if(peekmessage(&msg,PM_REMOVE)) {
        if(msg.message == WM_LBUTTONDOWN) {
            break;
        }
    }
				}
				Sleep(500);
				flushmessage(-1);
				cleardevice();
				putimage(0,0,&img3);
				EndBatchDraw();
	
			while(true) {
                if(peekmessage(&msg,PM_REMOVE) && msg.message == WM_LBUTTONDOWN) {
                    break;
                }
            }
			Sleep(500);
			flushmessage(-1);
            cleardevice(); // ����׼����ʼ��Ϸ
            break; // �����˵�ѭ����������Ϸ��ѭ��
		}
		if(button(rx,ry+90,rw,rh,"��    ��"))
		{
			cleardevice(); // �����Ļ
    		putimage(0, 0, &img_about); // ��ʾ����ҳ���ͼƬ
    		EndBatchDraw();
    		Sleep(30); 

 		while (true) {
        	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {//����������
			flushmessage(-1); 
            break; // �˳�ѭ��
        	}
    	}
    goto MENU; // ��ת�����˵�
	}
			 
		 if(button(rx,ry+180,rw,rh,"�˳���Ϸ")) 
		 {
		 	return 0;//�˳���Ϸ 
		} 
		msg.message = 0;
		EndBatchDraw();
	}
	
	//��Ϸ��ѭ��
	 gameRunning = true;
	 while(gameRunning)//�����Ϸ�Ƿ�ʼ 
	 {
	 	 gameLoop();//ִ����Ϸ��ѭ�� 
		if (!gameRunning) {
			EndBatchDraw();
			cleardevice();
            goto MENU;//��ִ���򷵻������� 
        }	
	 }

	return 0;
}
