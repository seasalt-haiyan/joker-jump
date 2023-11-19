#include <stdio.h>
#include <easyx.h> 
#include <stdlib.h>
#include <time.h>
#include "tools.h" //加载透明图片的头文件 

#define LENGTH 800 //窗口长度 
#define WIDTH 500 //窗口宽度 

//全局变量 
ExMessage msg = {0}; //鼠标消息 
int score = 0;//分数 
int highscore = 0;//最高分数 
int gravity = 2; // 重力值
int jumpStrength = -33; // 跳跃的初始速度
int obstacleSpeed = -10; // 障碍物的水平速度
int spawnRate = 100; // 障碍物出现的间隔时间
bool gameRunning;

//图片资源 
IMAGE img_gameover,img,img1,img2,img3,img_platform,img_player,img_obstacle,img_background,img_about;

//玩家，障碍物结构体 
typedef struct {
    int x, y, width, height;
    bool isJumping;
    int velocityY;
    int type;
} GameObject;

GameObject player, obstacle;

//游戏初始化 
void initGameObjects() {
    player.x = 100;
    player.y = WIDTH - 60 - 60; // 地面高度减去player高度
    player.width = 60;
    player.height = 60;
    player.isJumping = false;
    player.velocityY = 0;

    obstacle.x = LENGTH - 220;
    obstacle.y = WIDTH - 60 - 80; // 地面高度减去obstacle高度
    obstacle.width = 220;
    obstacle.height = 80;
}

//检测是否碰撞 
bool checkCollision(GameObject *a, GameObject *b) {
    if (a->x < b->x + b->width && a->x + a->width > b->x && a->y < b->y + b->height && a->y + a->height > b->y) {
        return true;
    }
    return false;
}


//判断mx,my是否在指定的区域 
bool inArea(int mx, int my,int x, int y, int w, int h)
{
	if(mx > x && mx < x + w && my > y && my < y + h)
	{
		return true;
	}
	return false;
}

//创建一个精美的按钮 
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

//显示分数的函数 
void displayScores() {
    char scoreStr[20];
    char highScoreStr[20];

    sprintf(scoreStr, "分数: %d", score);
    sprintf(highScoreStr, "最高分数: %d", highscore);
    
    outtextxy(10, 10, scoreStr);
    outtextxy(10, 30, highScoreStr); 
}

//结束屏幕函数 
void gameOverScreen() {
	
    if (score > highscore) {
        highscore = score; // 更新最高分数
        }
            
    BeginBatchDraw();
    cleardevice();
    putimage(0,0,&img_background);
    displayScores();
    EndBatchDraw();
    FlushBatchDraw(); 

    while (true) {
    	
        BeginBatchDraw(); // 开始批量绘制

        // 绘制按钮
        button(LENGTH / 2 - 100, WIDTH / 2, 200, 50, "重新开始");
        button(LENGTH / 2 - 100, WIDTH / 2 + 70, 200, 50, "返回主菜单");
        settextstyle(50,25,"黑体");
		outtextxy(280, WIDTH / 2 - 70,"GAME OVER!");
		settextstyle(20,10,"黑体");
        EndBatchDraw();
        FlushBatchDraw(); // 刷新屏幕显示以避免闪烁

        if (peekmessage(&msg, PM_REMOVE)) {
            // 如果消息是鼠标左键点击
            if (msg.message == WM_LBUTTONDOWN) {
                // 如果“重新开始”按钮被点击
                if (button(LENGTH / 2 - 100, WIDTH / 2, 200, 50, "重新开始")) {
                    initGameObjects(); // 重新初始化游戏对象
                    score = 0; // 分数重置为0
                    FlushBatchDraw();
                    flushmessage(-1);
                    return; // 退出gameOverScreen，继续执行gameLoop
                }
                // 如果“返回主菜单”按钮被点击
                if (button(LENGTH / 2 - 100, WIDTH / 2 + 70, 200, 50, "返回主菜单")) {
                    gameRunning = false; // 设置游戏运行状态为false
                    while (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                        Sleep(10); // 短暂延迟等待鼠标释放
                    }
                    flushmessage(-1);
                    Sleep(100);
                    return; // 退出gameOverScreen，返回到菜单
                }
            }
    	}
    }
}





void gameLoop() {
    initGameObjects(); // 初始化游戏对象
    bool gameRunning = true;
    bool gameStarted = false;
    int scoreCounter = 0; // 计分器
    int frameCounter = 0; // 控制障碍物生成的帧计数器

	BeginBatchDraw();
    cleardevice();
    putimage(0, 0, &img_background); // 背景
    // 绘制地面和角色
    for (int i = 0; i < LENGTH; ++i) {
        putimage(i * 60, WIDTH - 60, &img_platform); // 地面
    }
    drawImg(player.x, player.y, &img_player); // 角色
    EndBatchDraw();
    FlushBatchDraw();
    Sleep(100);
    flushmessage(-1);
    while (!gameStarted) {
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {//鼠标左键按下开始游戏 
            gameStarted = true;
        }
        Sleep(50); 
    }
    
    while (gameRunning && gameStarted) {
    
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) { // 鼠标左键按下跳跃 
    if (!player.isJumping) {
        player.isJumping = true;
        player.velocityY = jumpStrength; // 赋予一个向上的初速度
        msg.message = 0;
    }
}

        // 跳跃和重力逻辑
        if (player.isJumping) {
            player.y += player.velocityY; // 应用当前速度
            player.velocityY += gravity; // 应用重力，使速度向下

            // 检查是否落地
            if (player.y >= WIDTH - 60 - player.height) {
                player.y = WIDTH - 60 - player.height; // 重置到地面位置
                player.isJumping = false; // 停止跳跃
                player.velocityY = 0;
            }
        }
        // 障碍物生成逻辑
       obstacle.x += obstacleSpeed;
        frameCounter++;
        if (frameCounter >= spawnRate) {
        	spawnRate = 60 + (rand() % 30);// 随机生成间隔 
        	obstacleSpeed = -25 - (rand() % 6); // 随机速度 
            obstacle.x = LENGTH; // 将障碍物放置在屏幕右侧
            frameCounter = 0;
            obstacle.type = rand() % 2;// 随机障碍物类型
         if (obstacle.type == 1) {
                obstacle.y = 170;//空中障碍物 
            }else if(obstacle.type == 0){
            	obstacle.y = WIDTH - 60 - 80;
			}
        }
         
        // 如果检测到碰撞，游戏结束 
        if (checkCollision(&player, &obstacle)) {
        	flushmessage(-1);
            gameOverScreen(); // 显示游戏结束界面
            if (!gameRunning) {
        break; // 跳出gameLoop循环，返回到主菜单界面
    }
            break; // 退出游戏循环
        }
    
        // 更新分数
        scoreCounter++;
        if (scoreCounter >= 1) { // 每过1帧增加1分
            score++;
            scoreCounter = 0;
        }
        
        BeginBatchDraw();
        cleardevice(); // 清屏
        putimage(0,0,&img_background);

        // 绘制地面
        for (int i = 0; i < LENGTH ; ++i) {
            putimage(i * 60, WIDTH - 60, &img_platform); 
        }

        // 绘制player
            drawImg(player.x, player.y, &img_player);

        // 绘制obstacle
            drawImg(obstacle.x, obstacle.y, &img_obstacle);
            
        displayScores();//显示分数 

        EndBatchDraw();
        FlushBatchDraw();
        
        Sleep(14); // 控制帧率 

    }
}

//主函数 
int main()
{
	//随机数 
	srand((unsigned)time(NULL));
	
	//加载图片资源 
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
	
	
	//开始绘图 
	initgraph(LENGTH,WIDTH);
	MENU://结束画面选择回到主菜单goto到这里
	//设置参数 
	setbkcolor(WHITE);
	setlinecolor(BLACK) ;
	cleardevice();
	settextstyle(100,50,"黑体");

	putimage(650,20,&img);
	putimage(20,20,&img1);
	settextcolor(BLACK); 
	setbkmode(TRANSPARENT); 
	outtextxy(150,30,"小丑跳跳乐"); 
	
	int rx = 250;
	int ry = 200;
	int rw = 300;
	int rh = 60;
	
	settextstyle(20,10,"黑体");
	outtextxy(680, 480,"version 1.0");
	
	//菜单界面，显示出开始游戏，关于，退出游戏选项 
	while(true)
	{
		peekmessage(&msg,EX_MOUSE);
		BeginBatchDraw();
		if(button(rx,ry,rw,rh,"开始游戏"))
		{
			cleardevice();
			putimage(0,0,&img2);
			EndBatchDraw();
			settextstyle(20,10,"黑体");
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
            cleardevice(); // 清屏准备开始游戏
            break; // 跳出菜单循环，进入游戏主循环
		}
		if(button(rx,ry+90,rw,rh,"关    于"))
		{
			cleardevice(); // 清除屏幕
    		putimage(0, 0, &img_about); // 显示关于页面的图片
    		EndBatchDraw();
    		Sleep(30); 

 		while (true) {
        	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {//鼠标左键按下
			flushmessage(-1); 
            break; // 退出循环
        	}
    	}
    goto MENU; // 跳转回主菜单
	}
			 
		 if(button(rx,ry+180,rw,rh,"退出游戏")) 
		 {
		 	return 0;//退出游戏 
		} 
		msg.message = 0;
		EndBatchDraw();
	}
	
	//游戏主循环
	 gameRunning = true;
	 while(gameRunning)//检测游戏是否开始 
	 {
	 	 gameLoop();//执行游戏主循环 
		if (!gameRunning) {
			EndBatchDraw();
			cleardevice();
            goto MENU;//不执行则返回主界面 
        }	
	 }

	return 0;
}
