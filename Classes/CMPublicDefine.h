﻿#ifndef _CM_PUBLIC_DEFINE_H_
#define _CM_PUBLIC_DEFINE_H_
#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;
#define SCREEN_WIDTH CCDirector::sharedDirector()->getWinSize().width
#define SCREEN_HEIGHT CCDirector::sharedDirector()->getWinSize().height

#define CONTROL_UI_HEIGHT				96					//控制UI高度
#define TILE_MAP_VERTICAL				13					//地图瓦片竖直块数
#define JUMP_START_SPEED				8					//跳跃起始速度
#define JUMP_SPEED_MINUS				0.3					//跳跃递减速度
#define DROP_SPEED_PLUS					0.15				//掉落加速度

#define MONSTER_ACTIVE_DISTANCE			SCREEN_WIDTH-100		//怪物激活距离
#define COLLISION_POS_ADJUSTMENT		3					//跳跃与坠落的碰撞检测点调整



enum 
{
	enZOrderBack,
	enZOrderMiddle,
	enZOrderFront,
};

//瓦片类型
enum enumTileType
{
	enTileTypeLand,   // 地板
	enTileTypeBlock,  // 金币等小块块
	enTileTypePipe,   // 管道
	enTileTypeTrap,   // 陷阱
	enTileTypeCoin,   // 金币
	enTileTypeNone,  // 无碰撞物
	enTileTypeFlagpole,  // 旗杆
};

//砖块类型
enum enumBlockType
{
	enBlockTypeBox = 1,			//宝箱砖块
	enBlockTypeNormal,			//普通砖块，可顶碎
	enBlockTypeAddLife,			//隐藏加人砖块
	enBlockTypeJustBlock,		//没有任何互动的砖块
};


//Mario状态
enum enumMarioStatus
{
	enMarioStatusStandLeft,			//待机
	enMarioStatusStandRight,
	//--------------------------
	enMarioStatusRunLeft,			//跑动
	enMarioStatusRunRight,
	//--------------------------
	enMarioStatusOnAirLeft,			//悬空
	enMarioStatusOnAirRight,
};


//Mario等级
enum enumMarioLevel
{
	enMarioLevelSmall,
	enMarioLevelBig,
	enMarioLevelMax,
};

//怪物与道具移动方向
enum enMoveDirection
{
	enMoveLeft,
	enMoveRight,
};

class CMReceiver
{
public:
	virtual void OnMsgReceive(int enMsg,void* pData,int nSize) = 0;
};

class CMSender
{
protected:
	CMReceiver* m_pReceiver;
public:
	void SetReceiver(CMReceiver* pReceiver)
	{
		m_pReceiver = pReceiver;
	}
	void SendMsg(int enMsg,void* pData = NULL,int nSize = 0)
	{
		m_pReceiver->OnMsgReceive(enMsg,pData,nSize);
	}
};

enum enumGameMapMsg			//由CMGameMap处理的消息
{
	enMsgDead,				//Mario死亡
	enMsgMonsterDisappear,	//怪物离开地图消失
	enMsgMushroomsStamp,	//蘑菇被踩消息
	enMsgTortoiseStamp,		//乌龟被踩消息
	enMsgBeHurt,			//Mario被怪物碰到
	enMsgBlockBoxHitted,	//宝箱砖块被顶消息
	enMsgItemDisappear,		//道具消失
	enMsgLevelUp,			//吃道具变大
};

enum enumGameScene
{
	enGameOver,				//游戏结束消息
};

//道具消息数据
class CMItemBasic;
struct TCmd_Remove_Item
{
	CMItemBasic* pItem;
};

//怪物离开地图消失数据
class CMMonsterBasic;
struct TCmd_Remove_Monster
{
	CMMonsterBasic* pMonster;
};

//破坏砖块
struct TCmd_Distroy_Block
{
	CCSprite *pTileToDistroy;
};
//宝箱砖块被顶 出现物品
class CMItemBlock;
struct TCmd_Block_Box_Hitted
{
	CMItemBlock* pBlockBox;
};

#endif