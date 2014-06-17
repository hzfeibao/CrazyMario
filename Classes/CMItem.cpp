#include "CMItem.h"

/************************************************************************/
/* 物品道具基类 */ 
/************************************************************************/
bool CMItemBasic::init( CCPoint ptItemPos,CCSize szItemSize,CMMario *pMario,CMReceiver *pMsgRecver )
{
	do 
	{
		CC_BREAK_IF(!CCNode::init());

		//初始化成员变量
		m_pMario = pMario;
		m_bIsActivation = false;
		m_MoveDirection = enMoveLeft;
		m_fDropSpeedPlus = 0;
		m_pReceiver = pMsgRecver;

		return true;
	} while (false);
	CCLog("fun CMItemBasic::init Error!");
	return false;
}

bool CMItemBasic::OnCollisionMario()
{
	do 
	{

		return true;
	} while (false);
	CCLog("fun CMItemBasic::OnCollisionMario Error!");
	return false;
}

bool CMItemBasic::OnCallPerFrame(float fT)
{
	do 
	{
		OnCollisionMario();

		//判断当道具离开地图则发消息删除自己
		if (getPositionX()<0 || getPositionY()<0)
		{
			TCmd_Remove_Item* pData = new TCmd_Remove_Item;
			pData->pItem = this;
			SendMsg(enMsgItemDisappear,pData,sizeof(TCmd_Remove_Item));
			return true;
		}
		return false;
	} while (false);
	CCLog("fun CMItemBasic::OnCallPerFrame Error!");
	return false;
}

/************************************************************************/
/* 金币类 */ 
/************************************************************************/
CMItemCoin * CMItemCoin::CreateItemIcon( CCPoint ptItemPos,CCSize szItemSize,CMMario *pMario,CMReceiver *pMsgRecver )
{
	do 
	{
		CMItemCoin *pItemCoin = new CMItemCoin();
		if (pItemCoin && pItemCoin->init(ptItemPos,szItemSize,pMario,pMsgRecver))
		{
			pItemCoin->autorelease();
			return pItemCoin;
		}
		CC_SAFE_DELETE(pItemCoin);
		return NULL;
	} while (false);
	CCLog("fun CMItemCoin::CreateItemIcon Error!");
	return NULL;
}

bool CMItemCoin::init( CCPoint ptItemPos,CCSize szItemSize,CMMario *pMario,CMReceiver *pMsgRecver )
{
	do 
	{
		CC_BREAK_IF(!CMItemBasic::init(ptItemPos,szItemSize,pMario,pMsgRecver));

		CCSprite* pCoin = CCSprite::create("coin.png");
		pCoin->setAnchorPoint(ccp(0,0));
		CC_BREAK_IF(pCoin==NULL);
		addChild(pCoin,enZOrderFront,enTagMainNode);

		setContentSize(szItemSize);

		return true;
	} while (false);
	CCLog("fun CMItemCoin::init Error!");
	return false;
}

bool CMItemCoin::OnCollisionMario()
{
	do 
	{
		CCSprite* pCoin = dynamic_cast<CCSprite*>(getChildByTag(enTagMainNode));
		CC_BREAK_IF(pCoin==NULL);

		//判断马里奥与金币的碰撞
		if (m_pMario->boundingBox().intersectsRect(boundingBox()))
		{
			TCmd_Remove_Item* pData = new TCmd_Remove_Item;
			pData->pItem = this;
			SendMsg(enMsgItemDisappear,pData,sizeof(pCoin));
			return true;
		}

		return false;
	} while (false);
	CCLog("fun CMItemCoin::OnCollisionMario Error!");
	return false;
}

bool CMItemCoin::OnCallPerFrame( float fT )
{
	do 
	{
		return (CMItemBasic::OnCallPerFrame(fT)||OnCollisionMario());
	} while (false);
	CCLog("fun CMItemCoin::OnCallPerFrame Error!");
	return false;
}

/************************************************************************/
/* 砖块类 */                              
/************************************************************************/
CMItemBlock * CMItemBlock::CreateItemBlock( CCPoint ptItemPos,CCSize szItemSize,CMMario *pMario,CMReceiver *pMsgRecver ,enumBlockType eBlockType)
{
	do 
	{
		CMItemBlock *pItemBlock = new CMItemBlock();
		if (pItemBlock && pItemBlock->init(ptItemPos,szItemSize,pMario,pMsgRecver,eBlockType))
		{
			pItemBlock->autorelease();
			return pItemBlock;
		}
		CC_SAFE_DELETE(pItemBlock);
		return NULL;
	} while (false);
	CCLog("fun CMItemBlock::create() Error!");
	return NULL;
}

bool CMItemBlock::init( CCPoint ptItemPos,CCSize szItemSize,CMMario *pMario,CMReceiver *pMsgRecver,enumBlockType eBlockType )
{
	do 
	{
		CC_BREAK_IF(!CMItemBasic::init(ptItemPos,szItemSize,pMario,pMsgRecver));

		m_bIsTouched = false;

		//根据砖块类型设定纹理
		CCSprite* pBlock = NULL;
		switch (eBlockType)
		{
		case enBlockTypeNormal:
			{
				pBlock = CCSprite::create("singleblock.png",CCRectMake(0,0,16,16));
			}
			break;
		case enBlockTypeBox:
			{
				pBlock = CCSprite::create("shanshuodecoin.png",CCRectMake(0,0,16,16));
			}
			break;
		case enBlockTypeAddLife:
			{
				pBlock = CCSprite::create("singleblock.png",CCRectMake(0,0,16,16));
			}
			break;
		case enBlockTypeJustBlock:
			{
				pBlock = CCSprite::create("blocktype1.png");
			}
			break;
		}
		pBlock->setAnchorPoint(ccp(0,0));
		CC_BREAK_IF(pBlock==NULL);
		addChild(pBlock,enZOrderFront,enTagMainNode);

		m_eBlockType = eBlockType;

		setContentSize(szItemSize);

		return true;
	} while (false);
	CCLog("fun CMItemBlock::init Error!");
	return false;
}

bool CMItemBlock::OnCollisionMario()
{
	do 
	{
		

		//被顶
		if (m_pMario->boundingBox().intersectsRect(boundingBox()) &&
			getPositionY()>m_pMario->getPositionY() && 
			abs(m_pMario->getPositionY()-getPositionY())>m_pMario->boundingBox().size.height*0.8 &&
			abs(m_pMario->getPositionX()-getPositionX())<m_pMario->boundingBox().size.width*0.5 && m_bIsTouched==false)
		{
			m_bIsTouched = true;
			stopAllActions();

			CCSprite* pBlock = dynamic_cast<CCSprite*>(getChildByTag(enTagMainNode));
			CC_BREAK_IF(pBlock==NULL);

			switch (m_pMario->GetMarioLevel())
			{
			case enMarioLevelSmall:
				{
					//砖块跳动动画
					CCActionInterval *pJumpBy = CCJumpBy::create(0.2f, CCPointZero, 
						16*0.5, 1);
					runAction(pJumpBy);

					if (m_eBlockType==enBlockTypeBox)
					{
						//图片换成被顶过的宝箱
						CCTexture2D *pTextureBlock = CCTextureCache::sharedTextureCache()->addImage("brokencoin.png");
						pBlock->setTexture(pTextureBlock);
						pBlock->setTextureRect(CCRectMake(0, 0, 16, 16));

						//发送砖块宝箱被顶消息
						TCmd_Block_Box_Hitted* pData = new TCmd_Block_Box_Hitted;
						pData->pBlockBox = this;
						SendMsg(enMsgBlockBoxHitted,pData,sizeof(TCmd_Block_Box_Hitted));
						return false;
					}
				}
				break;
			case enMarioLevelBig:
			case enMarioLevelMax:
				{
					if (m_eBlockType==enBlockTypeBox)
					{
						//图片换成被顶过的宝箱
						CCTexture2D *pTextureBlock = CCTextureCache::sharedTextureCache()->addImage("brokencoin.png");
						pBlock->setTexture(pTextureBlock);
						pBlock->setTextureRect(CCRectMake(0, 0, 16, 16));

						//发送砖块宝箱被顶消息
						TCmd_Block_Box_Hitted* pData = new TCmd_Block_Box_Hitted;
						pData->pBlockBox = this;
						SendMsg(enMsgBlockBoxHitted,pData,sizeof(TCmd_Block_Box_Hitted));
						return true;
					}
					else if (m_eBlockType==enBlockTypeNormal)
					{
						pBlock->removeFromParent();

						if (getActionByTag(enTagMainNode)==NULL)
						{
							//ccbi读取
							//构造一个ccbi文件读取器
							CCNodeLoaderLibrary * ccNodeLoaderLibrary = CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary();
							cocos2d::extension::CCBReader * pCcbReader = new cocos2d::extension::CCBReader(ccNodeLoaderLibrary);
							//读取动画文件
							CCNode *pCcbiNode = pCcbReader->readNodeGraphFromFile("ccbResources/boom.ccbi", this);
							pCcbReader->getAnimationManager()->setAnimationCompletedCallback(this,callfunc_selector(CMItemBlock::OnCallDeadAnimationFinished));
							CC_BREAK_IF(pCcbiNode==NULL);
							pCcbReader->getAnimationManager()->runAnimationsForSequenceNamed("boom");	
							//读完之后，立刻释放即可
							pCcbReader->release();
							CC_BREAK_IF(pCcbiNode==NULL);
							pCcbiNode->setAnchorPoint(ccp(0,0));
							addChild(pCcbiNode,enZOrderBack,enTagMainNode);
							pCcbiNode->setPosition(ccp(8,8));
							setContentSize(MONSTER_CCSIZE);
						}
					}
				}
				break;
			}
		}

		return false;
	} while (false);
	CCLog("fun CMItemBlock::OnCollisionMario Error!");
	return false;
}

void CMItemBlock::OnCallDeadAnimationFinished()
{
	TCmd_Remove_Item* pData = new TCmd_Remove_Item;
	pData->pItem = this;
	SendMsg(enMsgItemDisappear,pData,sizeof(TCmd_Remove_Item));
}

bool CMItemBlock::OnCallPerFrame( float fT )
{
	do 
	{
		//与马里奥碰撞导致删除 或者 与 周期导致移开地图 都会导致其被删除
		return (CMItemBasic::OnCallPerFrame(fT)||OnCollisionMario());
	} while (false);
	CCLog("fun CMItemBlock::OnCallPerFrame Error!");
	return false;
}

/************************************************************************/
/* 大人蘑 */                                              
/************************************************************************/
CMItemMashrooms * CMItemMashrooms::CreateItemCMItemMashrooms( CCPoint ptItemPos,CCSize szItemSize,CMMario *pMario,CMGameMap *pGameMap,CMReceiver *pMsgRecver )
{
	do 
	{
		CMItemMashrooms *pItemMashrooms = new CMItemMashrooms();
		if (pItemMashrooms && pItemMashrooms->init(ptItemPos,szItemSize,pMario,pGameMap,pMsgRecver))
		{
			pItemMashrooms->autorelease();
			return pItemMashrooms;
		}
		CC_SAFE_DELETE(pItemMashrooms);
		return NULL;
	} while (false);
	CCLog("fun CMHero::create() Error!");
	return NULL;
}

bool CMItemMashrooms::init( CCPoint ptItemPos,CCSize szItemSize,CMMario *pMario,CMGameMap *pGameMap,CMReceiver *pMsgRecver )
{
	do 
	{
		CC_BREAK_IF(!CMItemBasic::init(ptItemPos,szItemSize,pMario,pMsgRecver));

		CCSprite* pMashrooms = CCSprite::create("rewardMushroomSet.png");
		pMashrooms->setAnchorPoint(ccp(0,0));
		CC_BREAK_IF(pMashrooms==NULL);
		addChild(pMashrooms,enZOrderFront,enTagMainNode);

		m_pGameMap = pGameMap;
		m_MoveDirection = enMoveRight;
		m_bIsTouched = false;

		setContentSize(szItemSize);

		return true;
	} while (false);
	CCLog("fun CMItemMashrooms::init Error!");
	return false;
}

bool CMItemMashrooms::OnCollisionMario()
{
	do 
	{
		CCSprite* pMashrooms = dynamic_cast<CCSprite*>(getChildByTag(enTagMainNode));
		CC_BREAK_IF(pMashrooms==NULL);

		//与马里奥的碰撞
		if (m_pMario->boundingBox().intersectsRect(boundingBox()) && m_bIsTouched==false)
		{
			TCmd_Remove_Item* pData = new TCmd_Remove_Item;
			pData->pItem = this;
			m_bIsTouched = true;
			//马里奥变大
			SendMsg(enMsgLevelUp);

			//移除道具
			SendMsg(enMsgItemDisappear,pData,sizeof(TCmd_Remove_Item));

			return true;
			
		}

		return false;
	} while (false);
	CCLog("fun CMItemMashrooms::OnCollisionMario Error!");
	return false;
}

bool CMItemMashrooms::OnCallPerFrame( float fT )
{
	do 
	{
		//移动与碰撞
		if (m_MoveDirection == enMoveLeft)
		{
			//用怪物左方的2个瓦片来判断移动碰撞
			CCSprite* pTileSpriteLeftTop = m_pGameMap->TileMapLayerPosToTileSprite(ccp(getPositionX(),getPositionY()+getContentSize().height));
			CCSprite* pTileSpriteLeftMid = m_pGameMap->TileMapLayerPosToTileSprite(ccp(getPositionX(),getPositionY()+getContentSize().height/2));
			if (pTileSpriteLeftTop!=NULL || pTileSpriteLeftMid!=NULL)
			{
				m_MoveDirection = enMoveRight;
			}
			else
			{
				setPositionX(getPositionX()-1);
			}
		}
		else if(m_MoveDirection == enMoveRight)
		{
			//用怪物右方的2个瓦片来判断移动碰撞
			CCSprite* pTileSpriteRightTop = m_pGameMap->TileMapLayerPosToTileSprite(ccp(getPositionX()+getContentSize().width,getPositionY()+getContentSize().height));
			CCSprite* pTileSpriteRightMid = m_pGameMap->TileMapLayerPosToTileSprite(ccp(getPositionX()+getContentSize().width,getPositionY()+getContentSize().height/2));
			if (pTileSpriteRightTop!=NULL || pTileSpriteRightMid!=NULL)
			{
				m_MoveDirection = enMoveLeft;
			}
			else
			{
				setPositionX(getPositionX()+1);
			}
		}

		//用怪物下方的三个瓦片来判断掉落碰撞
		CCSprite* pTileSpriteBottomMid = m_pGameMap->TileMapLayerPosToTileSprite(ccp(getPositionX()+boundingBox().size.width/2,getPositionY()));
		CCSprite* pTileSpriteBottomLeft = m_pGameMap->TileMapLayerPosToTileSprite(ccp(getPositionX()+COLLISION_POS_ADJUSTMENT,getPositionY()));
		CCSprite* pTileSpriteBottomRight = m_pGameMap->TileMapLayerPosToTileSprite(ccp(getPositionX()+boundingBox().size.width-COLLISION_POS_ADJUSTMENT,getPositionY()));
		if (pTileSpriteBottomLeft!=NULL || pTileSpriteBottomMid!=NULL || pTileSpriteBottomRight!=NULL)
		{
			//掉落速度归零
			m_fDropSpeedPlus = 0;
		}
		else
		{
			setPositionY(getPositionY()-m_fDropSpeedPlus);
			//掉落加速度
			m_fDropSpeedPlus += DROP_SPEED_PLUS;
		}

		return (CMItemBasic::OnCallPerFrame(fT)||OnCollisionMario());
	} while (false);
	CCLog("fun CMItemMashrooms::OnCallPerFrame Error!");
	return false;
}
