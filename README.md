# 小丑跳跳乐

## 项目简介

小丑跳跳乐是一个简单的2D平台跳跃游戏，玩家需要控制一个小丑角色跳过来自右侧的障碍物。随着时间的推移，游戏的难度会逐渐增加，障碍物的出现速度会变快。玩家的目标是尽可能长时间地存活下去，同时获得尽可能高的分数。

## 项目使用方法

## 游戏玩法
- 只需要鼠标左键，选择开始游戏后进入准备阶段，再次点击左键开始正是游戏，点击鼠标左键跳跃或者原地不动以避开障碍物尽可能的获得更高分数
# 第三方库
<easyx.h> 一个第三方图形库，用于支持图形化界面
<tools.h>输出透明背景图片的库
# 项目难点
1. 障碍物的类型，速度，生成间隔随机化
我使用了一个随机数生成器加在他们的初始值上实现这个效果
2. 透明图片的输出
在网络上寻找到一个通过贝叶斯变换实现透明背景图的头文件，直接引用并且使用里面的drawImg函数输出透明的图片
# 制作人
20232850 徐皓阳，该项目由本人一人完成
