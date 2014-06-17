#include "CMGameMap.h"

CMGameMap* CMGameMap::CreateGameMap(const char* pFileName)
{
	do 
	{
		CMGameMap* pPlayer = new CMGameMap;
		if (pPlayer && pPlayer->initWithTMXFile(pFileName))
		{
			pPlayer->Init();
			pPlayer->autorelease();
			return pPlayer;
		}
		delete pPlayer;
	} while (false);
	CCLog("Fun CMGameMap::CreateGameMap Error!");
	return NULL;
}

bool CMGameMap::Init()
{
	do 
	{
		//初始化成员变量
		m_fMapMove = 0;
		m_fDropSpeedPlus = 0;
		m_fJumpSpeed = 0;
		m_bIsLeftKeyDown = false;
		m_bIsRightKeyDown = false;
		m_bIsJumpKeyDown = false;
		m_bIsHeroDead = false;

		//初始化游戏对象数组
		m_pArrayItems = CCArray::create();
		m_pArrayItems->retain();
		m_pArrayMonsters = CCArray::create();
		m_pArrayMonsters->retain();
		m_pArrayBlocks = CCArray::create();
		m_pArrayBlocks->retain();

		//初始化Mario
		CMMario* pMario = CMMario::CreateHero(this);
		CC_BREAK_IF(pMario==NULL);
		pMario->setPosition(TileMapPosToTileMapLayerPos(ccp(2,11)));
		addChild(pMario,enZOrderFront,enTagMario);
		//pMario->SetStatus(enMarioStatusBig);

		//隐藏原落坑判断层
		CCTMXLayer* pTrapLayer = layerNamed("trap");
		CC_BREAK_IF(pTrapLayer==NULL);
		pTrapLayer->setVisible(false);

		//初始化显示金币
		CCTMXLayer* pCoinLayer = layerNamed("coin");
		CC_BREAK_IF(pCoinLayer==NULL);
		pCoinLayer->setVisible(false);
		//获得地图的瓦片数量
		int nMapHorizontalTileNum = pCoinLayer->boundingBox().size.width/getTileSize().width;
		int nMapVerticalTileNum = pCoinLayer->boundingBox().size.height/getTileSize().height;
		//遍历每片瓦片，寻找金币，建立并加入金币集合
		for (int i = 0;i<nMapHorizontalTileNum;i++)
		{
			for (int j = 0;j<nMapVerticalTileNum;j++)
			{
				if (TileMapPosToTileType(ccp(i,j),m_fMapMove)==enTileTypeCoin)
				{
					//将瓦片地图坐标转换为瓦片地图层坐标
					CCPoint CoinTileMapLayerPos = TileMapPosToTileMapLayerPos(ccp(i,j));
					CMItemCoin* pCoin = CMItemCoin::CreateItemIcon(CoinTileMapLayerPos,getTileSize(),pMario,this);
					if (pCoin==NULL)
					{
						CCLog("Coin init Error!");
					}
					pCoin->setPosition(CoinTileMapLayerPos);
					pCoin->setAnchorPoint(ccp(0,0));
					m_pArrayItems->addObject(pCoin);
					addChild(pCoin);
				}
			}
		}

		//初始化怪物显示
		CCTMXObjectGroup* pObjectLayer = objectGroupNamed("objects");
		CC_BREAK_IF(pObjectLayer==NULL);
		CCArray *ObjectArray = pObjectLayer->getObjects();
		CCDictionary *pDic = NULL;
		for (unsigned int i = 0; i < ObjectArray->count(); i++)
		{
			pDic = (CCDictionary *)ObjectArray->objectAtIndex(i);
			int PosX = ((CCString*)pDic->objectForKey("x"))->intValue();
			int PosY = ((CCString*)pDic->objectForKey("y"))->intValue();
			PosY -= this->getTileSize().height;
			CCPoint TileXY = ccp(PosX, PosY);

			CCString *strName = (CCString*)pDic->objectForKey("name");
			CCString *strType = (CCString*)pDic->objectForKey("type");

			// 进行怪物的初始化，先根据名字来判断是不是enemy，再细分enemy类型		
			if (strName->m_sString == "enemy")
			{
				if (strType->m_sString == "mushroom")
				{
					CMMonsterMushrooms *pMonster = CMMonsterMushrooms::CreateMonsterMushrooms(TileXY,pMario,this,this);
					if (pMonster==NULL)
					{
						CCLog("pMonster==NULL!");
					}
					pMonster->setPosition(ccp(TileXY.x,TileXY.y));
					pMonster->setAnchorPoint(ccp(0,0));
					m_pArrayMonsters->addObject(pMonster);
					addChild(pMonster,enZOrderFront);
				}
				if (strType->m_sString == "tortoise")
				{
					CMMonsterTortoise *pMonster = CMMonsterTortoise::CreateMonsterTortoise(TileXY,pMario,this,this);
					if (pMonster==NULL)
					{
						CCLog("pMonster==NULL!");
					}
					pMonster->setPosition(ccp(TileXY.x,TileXY.y));
					pMonster->setAnchorPoint(ccp(0,0));
					m_pArrayMonsters->addObject(pMonster);
					addChild(pMonster,enZOrderFront);
				}
				if (strType->m_sString == "flower")
				{
					CMMonsterFlower *pMonster = CMMonsterFlower::CreateMonsterFlower(TileXY,pMario,this,this);
					if (pMonster==NULL)
					{
						CCLog("pMonster==NULL!");
					}
					pMonster->setPosition(ccp(TileXY.x,TileXY.y));
					pMonster->setAnchorPoint(ccp(0,0));
					m_pArrayMonsters->addObject(pMonster);
					addChild(pMonster,enZOrderFront);
				}
			}
		}

		//初始化砖块显示
		CCTMXLayer* pBlockLayer = layerNamed("block");
		CC_BREAK_IF(pBlockLayer==NULL);
		pBlockLayer->setVisible(false);
		//获得地图的瓦片数量
		// 		int nMapHorizontalTileNum = pCoinLayer->boundingBox().size.width/getTileSize().width;
		// 		int nMapVerticalTileNum = pCoinLayer->boundingBox().size.height/getTileSize().height;
		//遍历每片瓦片，寻找瓦片，建立并加入瓦片集合
		for (int i = 0;i<nMapHorizontalTileNum;i++)
		{
			for (int j = 0;j<nMapVerticalTileNum;j++)
			{
				if (TileMapPosToTileType(ccp(i,j),m_fMapMove)==enTileTypeBlock)
				{
					//解析得到当前砖块的属性
					int GID = pBlockLayer->tileGIDAt(ccp(i,j));
					CCDictionary *pDic = propertiesForGID(GID);
					CC_BREAK_IF(pDic==NULL);
					CCString *strBlockType = (CCString*)pDic->objectForKey("blockType");
					if (strBlockType==NULL)
					{
						continue;
					}
					int nBlockType = strBlockType->intValue();

					//将瓦片地图坐标转换为瓦片地图层坐标
					CCPoint BlockTileMapLayerPos = TileMapPosToTileMapLayerPos(ccp(i,j));
					CMItemBlock* pBlock = CMItemBlock::CreateItemBlock(BlockTileMapLayerPos,getTileSize(),pMario,this,(enumBlockType)nBlockType);
					if (pBlock==NULL)
					{
						CCLog("Block init Error!");
					}
					pBlock->setPosition(BlockTileMapLayerPos);
					pBlock->setAnchorPoint(ccp(0,0));
					m_pArrayBlocks->addObject(pBlock);
					addChild(pBlock);
				}
			}
		}

		return true;
	} while (false);
	CCLog("Fun CMGameMap::Init Error!");
	return false;
}

CCSprite* CMGameMap::TileMapLayerPosToTileSprite( CCPoint TileMapLayerPos)
{
	do 
	{
		CCPoint TileMapPos = TileMapLayerPosToTileMapPos(TileMapLayerPos);

		//获得地图的各个层
		CCTMXLayer* pCloudLayer = layerNamed("cloud");
		CC_BREAK_IF(pCloudLayer==NULL);
		CCTMXLayer* pBlockLayer = layerNamed("block");
		CC_BREAK_IF(pBlockLayer==NULL);
		CCTMXLayer* pPipeLayer = layerNamed("pipe");
		CC_BREAK_IF(pPipeLayer==NULL);
		CCTMXLayer* pLandLayer = layerNamed("land");
		CC_BREAK_IF(pLandLayer==NULL);
		CCTMXLayer* pTrapLayer = layerNamed("trap");
		CC_BREAK_IF(pTrapLayer==NULL);
		CCTMXObjectGroup* pObjectLayer = objectGroupNamed("objects");
		CC_BREAK_IF(pObjectLayer==NULL);
		CCTMXLayer* pCoinLayer = layerNamed("coin");
		CC_BREAK_IF(pCoinLayer==NULL);
		CCTMXLayer* pFlagpoleLayer = layerNamed("flagpole");
		CC_BREAK_IF(pFlagpoleLayer==NULL);

		//若马里奥超过上边界
		if(TileMapPos.y<0)return NULL;

		CCSprite* pLandSprite = pLandLayer->tileAt(ccp(TileMapPos.x,TileMapPos.y));
		if (pLandSprite!=NULL)
		{
			return pLandSprite;
		}
		CCSprite* pBlockSprite = pBlockLayer->tileAt(ccp(TileMapPos.x,TileMapPos.y));
		if (pBlockSprite!=NULL)
		{
// 			//遍历砖块数组，如果砖块数组中未找到，则说明已被顶坏，返回空
// 			CCObject *pObj = NULL;
// 			CCARRAY_FOREACH(m_pArrayBlocks,pObj)
// 			{
// 				CMItemBlock* pItem = dynamic_cast<CMItemBlock*>(pObj);
// 				CC_BREAK_IF(pItem==NULL);
// 				CCSprite* pSprite = dynamic_cast<CCSprite*>(pItem->getChildByTag(0));
// 				if (pSprite!=NULL)
// 				{
// 					CCPoint CurBlockWorldPos = convertToWorldSpace(pBlockSprite->getPosition());
// 					CCPoint TempBlockWorldPos = convertToWorldSpace(pSprite->getPosition());
// 					CCLog("%f	%f",CurBlockWorldPos.x,CurBlockWorldPos.y);
// 					CCLog("%f	%f",TempBlockWorldPos.x,TempBlockWorldPos.y);
// 					if (abs(CurBlockWorldPos.x-TempBlockWorldPos.x)<getTileSize().width && 
// 						abs(CurBlockWorldPos.y-TempBlockWorldPos.y)<getTileSize().height)
// 					{
// 						return pBlockSprite;
// 					}
// 				}
// 			}
// 			return NULL;
			return pBlockSprite;
		}
		CCSprite* pPipeSprite = pPipeLayer->tileAt(ccp(TileMapPos.x,TileMapPos.y));	
		if (pPipeSprite!=NULL)
		{
			return pPipeSprite;
		}
		return NULL;
	} while (false);
	CCLog("fun CMGameMap::HeroPosToTileType Error!");
	return NULL;
}

void CMGameMap::OnCallPerFrame(float fT)
{
	do 
	{
		//遍历调用子节点的循环函数
		CCObject *pObj = NULL;

		CCArray *pItemToDistroy = CCArray::create();
		CCARRAY_FOREACH(m_pArrayItems,pObj)
		{
			//调用基类的Update实际上调用的是子类重写的Update，若想调用基类的Update需要在子类的Update中调用。
			CMItemBasic* pItem = dynamic_cast<CMItemBasic*>(pObj);
			if (pItem==NULL)
			{
				CCLog("pItem==NULL");
			}
			if(pItem->OnCallPerFrame(fT)==true)//执行逻辑周期 并判断是否可以移除
			{
				pItemToDistroy->addObject(pItem);
			}
		}
		//将需要移除的物品从逻辑队列中移除掉
		m_pArrayItems->removeObjectsInArray(pItemToDistroy);
		
		CCArray *pMonsterToDistroy = CCArray::create();
		CCARRAY_FOREACH(m_pArrayMonsters,pObj)
		{
			//调用基类的Update实际上调用的是子类重写的Update，若想调用基类的Update需要在子类的Update中调用。
			CMMonsterBasic* pMonster = dynamic_cast<CMMonsterBasic*>(pObj);
			if (pMonster==NULL)
			{
				CCLog("pMonster==NULL");
				return;
			}
			if(pMonster->OnCallPerFrame(fT)==true)//执行逻辑周期 并判断是否可以移除
			{
				pMonsterToDistroy->addObject(pMonster);
			}
		}
		//将需要移除的怪从逻辑队列中移除掉
		m_pArrayMonsters->removeObjectsInArray(pMonsterToDistroy);
		
		CCArray *pBlockToDistroy = CCArray::create();
		CCARRAY_FOREACH(m_pArrayBlocks,pObj)
		{
			//调用基类的Update实际上调用的是子类重写的Update，若想调用基类的Update需要在子类的Update中调用。
			CMItemBasic* pBlock = dynamic_cast<CMItemBasic*>(pObj);
			if (pBlock==NULL)
			{
				CCLog("pBlock==NULL");
			}
			if(pBlock->OnCallPerFrame(fT)==true)//执行逻辑周期 并判断是否可以移除
			{
				pBlockToDistroy->addObject(pBlock);
			}
		}
		//将需要移除的砖块从逻辑队列中移除掉
		m_pArrayBlocks->removeObjectsInArray(pBlockToDistroy);

		CMMario* pMario = dynamic_cast<CMMario*>(getChildByTag(enTagMario));
		CC_BREAK_IF(pMario==NULL);
		pMario->OnCallPerFrame(fT);		

		MarioMove(fT);
		return;
	} while (false);
	CCLog("fun CMGameMap::OnCallPerFrame Error!");
}

enumTileType CMGameMap::TileMapPosToTileType( CCPoint HeroPos,float fMapMove )
{
	do 
	{
		//地图瓦片坐标
		int nHeroTilePosX = HeroPos.x;
		int nHeroTilePosY = HeroPos.y;

		//获得地图的各个层
		CCTMXLayer* pCloudLayer = layerNamed("cloud");
		CC_BREAK_IF(pCloudLayer==NULL);
		CCTMXLayer* pBlockLayer = layerNamed("block");
		CC_BREAK_IF(pBlockLayer==NULL);
		CCTMXLayer* pPipeLayer = layerNamed("pipe");
		CC_BREAK_IF(pPipeLayer==NULL);
		CCTMXLayer* pLandLayer = layerNamed("land");
		CC_BREAK_IF(pLandLayer==NULL);
		CCTMXLayer* pTrapLayer = layerNamed("trap");
		CC_BREAK_IF(pTrapLayer==NULL);
		CCTMXLayer* pCoinLayer = layerNamed("coin");
		CC_BREAK_IF(pCoinLayer==NULL);
		CCTMXLayer* pFlagpoleLayer = layerNamed("flagpole");
		CC_BREAK_IF(pFlagpoleLayer==NULL);

		CCSprite* pLandSprite = pLandLayer->tileAt(ccp(nHeroTilePosX,nHeroTilePosY));
		if (pLandSprite!=NULL)
		{
			return enTileTypeLand;
		}
		CCSprite* pBlockSprite = pBlockLayer->tileAt(ccp(nHeroTilePosX,nHeroTilePosY));
		if (pBlockSprite!=NULL)
		{
			return enTileTypeBlock;
		}
		CCSprite* pPipeSprite = pPipeLayer->tileAt(ccp(nHeroTilePosX,nHeroTilePosY));	
		if (pPipeSprite!=NULL)
		{
			return enTileTypePipe;
		}
		CCSprite* pCoinSprite = pCoinLayer->tileAt(ccp(nHeroTilePosX,nHeroTilePosY));	
		if (pCoinSprite!=NULL)
		{
			return enTileTypeCoin;
		}
		return enTileTypeNone;
	} while (false);
	CCLog("fun CMGameMap::PosToTileType Error!");
	return enTileTypeNone;
}

void CMGameMap::onExit()
{
	m_pArrayItems->removeAllObjects();
	CC_SAFE_RELEASE(m_pArrayItems);
	m_pArrayMonsters->removeAllObjects();
	CC_SAFE_RELEASE(m_pArrayMonsters);
	m_pArrayBlocks->removeAllObjects();
	CC_SAFE_RELEASE(m_pArrayBlocks);
	CCTMXTiledMap::onExit();
}

cocos2d::CCPoint CMGameMap::TileMapPosToTileMapLayerPos( CCPoint TilePos )
{
	float fPositionX = TilePos.x * getTileSize().width;
	float fPositionY = SCREEN_HEIGHT - CONTROL_UI_HEIGHT - (TilePos.y * getTileSize().height + getTileSize().height);
	return ccp(fPositionX,fPositionY);
}

void CMGameMap::OnMsgReceive( int enMsg,void* pData,int nSize )
{
	switch (enMsg)
	{
	case enMsgItemDisappear://道具移除处理
		{
			OnSubMsgItemRemove(pData,nSize);
			return;
		}
		break;
	case enMsgLevelUp:
		{
			OnSubMsgMarioLevelUp();
			return;
		}
		break;
	case enMsgBeHurt:
		{
			OnSubMsgMarioBeHurt();
			return;
		}
		break;
	case enMsgMonsterDisappear:
		{
			OnSubMsgMonsterDisappear(pData,nSize);
			return;
		}
		break;
	case enMsgMushroomsStamp:
		{
			OnSubMsgStamp(pData,nSize);
			return;
		}
		break;
	case enMsgBlockBoxHitted:
		{
			OnSubMsgBlockBoxHitted(pData,nSize);
			return;
		}
		break;
	}
}

cocos2d::CCPoint CMGameMap::TileMapLayerPosToWorldPos( CCPoint TileMapLayerPos,float m_fMapMove )
{
	float fPositionY = TileMapLayerPos.y;
	float fPositionX = TileMapLayerPos.x - m_fMapMove;
	return ccp(fPositionX,fPositionY);
}

float CMGameMap::GetMapMove()
{
	return m_fMapMove;
}

void CMGameMap::MarioMove(float fT)
{
	do 
	{
		CMMario* pMario = dynamic_cast<CMMario*>(getChildByTag(enTagMario));
		CC_BREAK_IF(pMario==NULL);
		//CCPoint CurMarioPos = pMario->getPosition();

		//如果左键按下
		if(m_bIsLeftKeyDown)
		{
			pMario->OnCtrlMove(fT,false);
		}
		//如果右键按下
		if (m_bIsRightKeyDown)
		{
			CCPoint CurMarioPos = pMario->getPosition();
			pMario->OnCtrlMove(fT,true);
			//如果Mario的位置变化了，则地图才会卷动
			if (convertToWorldSpace(pMario->getPosition()).x>120 && abs(pMario->getPositionX()-CurMarioPos.x)>1 &&
				pMario->getPositionX() < (getContentSize().width - SCREEN_WIDTH + 100))
			{
				setPositionX(getPositionX()-100*fT);
			}
		}
		//如果跳跃键按下
		if (m_bIsJumpKeyDown)
		{
			pMario->OnCtrlJump();
		}
		//如果没有键按下
		if (m_bIsLeftKeyDown==false && m_bIsRightKeyDown==false && m_bIsJumpKeyDown==false)
		{
			pMario->OnCtrlNoAction();
		}

#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)    

		// 		CCMenu* pMenu = dynamic_cast<CCMenu*>(getChildByTag(enTagMenu));
		// 		CC_BREAK_IF(pMenu==NULL);
		// 		CCMenuItemImage* pLeftKey = dynamic_cast<CCMenuItemImage*>(pMenu->getChildByTag(enTagLeftKey));
		// 		CC_BREAK_IF(pLeftKey==NULL);
		// 		CCMenuItemImage* pRightKey = dynamic_cast<CCMenuItemImage*>(pMenu->getChildByTag(enTagLeftKey));
		// 		CC_BREAK_IF(pRightKey==NULL);
		// 		CCMenuItemImage* pJumpKey = dynamic_cast<CCMenuItemImage*>(pMenu->getChildByTag(enTagLeftKey));
		// 		CC_BREAK_IF(pJumpKey==NULL);
		// 		CCMenuItemImage* pFireKey = dynamic_cast<CCMenuItemImage*>(pMenu->getChildByTag(enTagLeftKey));
		// 		CC_BREAK_IF(pFireKey==NULL);

		if(KEY_DOWN(KEY_KEY_K))
		{

		}
		if (KEY_DOWN(KEY_KEY_J))
		{

		}

		if(KEY_DOWN(KEY_KEY_A))
		{
			m_bIsLeftKeyDown = true;
		}
		if(KEY_UP(KEY_KEY_A))
		{
			m_bIsLeftKeyDown = false;
		}
		if(KEY_DOWN(KEY_KEY_D))
		{
			m_bIsRightKeyDown = true;
		}
		if(KEY_UP(KEY_KEY_D))
		{
			m_bIsRightKeyDown = false;
		}
		if(KEY_DOWN(KEY_KEY_W))
		{
			m_bIsJumpKeyDown = true;
		}
		if(KEY_UP(KEY_KEY_W))
		{
			m_bIsJumpKeyDown = false;
		}
		if(KEY_DOWN(KEY_KEY_S))
		{

		}
#endif

		
		return;
	} while (false);
	CCLog("fun CMGameMap::MarioMove Error!");
	return;
}

cocos2d::CCPoint CMGameMap::TileMapLayerPosToTileMapPos( CCPoint TileMapLayerPos )
{
	//将层坐标转换为地图瓦片坐标
	int nTileMapPosX = (TileMapLayerPos.x)/getTileSize().width;
	int nTileMapPosY = TILE_MAP_VERTICAL - (int)((TileMapLayerPos.y)/getTileSize().height);

	return ccp(nTileMapPosX,nTileMapPosY);
}

void CMGameMap::OnSubMsgItemRemove( void *pData ,int nSize )
{
	do 
	{
		TCmd_Remove_Item *pCmd = (TCmd_Remove_Item*)pData;
		//参数校验
		CC_BREAK_IF(pCmd!=NULL&&pCmd->pItem==NULL);	
		CC_BREAK_IF(sizeof(TCmd_Remove_Item)!=nSize);
		//从画面移除
		pCmd->pItem->removeFromParent();
		return;
	} while (false);
	CCLOG("Error:  CMGameMap::OnSubMsgItemRemove Run Error!");
}

void CMGameMap::OnSubMsgMarioLevelUp()
{
	do 
	{
		CMMario* pMario = dynamic_cast<CMMario*>(getChildByTag(enTagMario));
		
		CC_BREAK_IF(pMario==NULL);
		
		pMario->MarioLevelUp();

		return;
	} while (false);
	CCLOG("Error: CMGameMap::OnSubMsgMarioGrowUp");
}

void CMGameMap::OnSubMsgMonsterDisappear( void *pData,int nSize )
{
	do 
	{
		TCmd_Remove_Monster *pCmd = (TCmd_Remove_Monster*)pData;
		//参数校验
		CC_BREAK_IF(pCmd!=NULL&&pCmd->pMonster==NULL);	
		CC_BREAK_IF(sizeof(TCmd_Remove_Monster)!=nSize);
		
		//从画面移除
		pCmd->pMonster->removeFromParent();
		
		return;
	} while (false);
	CCLog("fun CMGameMap::OnSubMsgMonsterDisappear Error!");
}

void CMGameMap::OnSubMsgStamp( void *pData,int nSize )
{
	do 
	{
		TCmd_Remove_Monster *pCmd = (TCmd_Remove_Monster*)pData;
		//参数校验
		CC_BREAK_IF(pCmd!=NULL&&pCmd->pMonster==NULL);	
		CC_BREAK_IF(sizeof(TCmd_Remove_Monster)!=nSize);
		//从画面移除
		pCmd->pMonster->removeFromParent();
		
		return;
	} while (false);
	CCLog("fun CMGameMap::OnSubMsgStamp Error!");
}

void CMGameMap::OnSubMsgBlockBoxHitted( void *pData,int nSize )
{
	do 
	{
		CMMario* pMario = dynamic_cast<CMMario*>(getChildByTag(enTagMario));
		CC_BREAK_IF(pMario==NULL);
		
		TCmd_Block_Box_Hitted *pCmd = (TCmd_Block_Box_Hitted*)pData;
		//参数校验
		CC_BREAK_IF(pCmd!=NULL&&pCmd->pBlockBox==NULL);	
		CC_BREAK_IF(sizeof(TCmd_Remove_Monster)!=nSize);

		//建立宝箱顶出的物品
		CCPoint pMashroomsPos = ccp(pCmd->pBlockBox->getPosition().x,pCmd->pBlockBox->getPosition().y+getTileSize().height+10);
		CMItemMashrooms* pMashrooms = CMItemMashrooms::CreateItemCMItemMashrooms(pMashroomsPos,getTileSize(),pMario,this,this);
		CC_BREAK_IF(pMashrooms==NULL);
		pMashrooms->setPosition(pMashroomsPos);
		addChild(pMashrooms,enZOrderFront);
		m_pArrayItems->addObject(pMashrooms);
		return;
	} while (false);
	CCLog("fun CMGameMap::OnSubMsgBlockBoxHitted Error!");
}

void CMGameMap::OnSubMsgMarioBeHurt()
{
	do 
	{
		CMMario* pMario = dynamic_cast<CMMario*>(getChildByTag(enTagMario));
		CC_BREAK_IF(pMario==NULL);

		pMario->MarioGotHurt();

		return;
	} while (false);
	CCLog("fun CMGameMap::OnSubMsgMarioBeHurt error!");
}
