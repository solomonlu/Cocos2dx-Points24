#include "MainScene.h"
#include "json/prettywriter.h"
#include "json/stringbuffer.h"
#include "SimpleAudioEngine.h"
#include "LayerTagDefine.h"
#include "CardSprite.h"
#include "CalcLayer.h"
#include "VictoryLayer.h"
#include "GameStartSence.h"
#include "Util.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#endif

USING_NS_CC;
using namespace ui;
using namespace CocosDenshion;

extern std::set<std::string> g_SolutionSet;

Scene* MainScene::scene(const std::string& cardsStr,bool multiplay)
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
	MainScene *layer = new MainScene();
    if (layer && layer->init(cardsStr,multiplay))
    {
        layer->autorelease();
    }
    else
    {
        delete layer;
		layer = nullptr;
    }

    // add layer as a child to scene
    scene->addChild(layer,0,LAYER_TAG_BLUETOOTH_MAIN);

    // return the scene
    return scene;
}

bool MainScene::init(const std::string& cardsStr,bool multiplay)
{
	if ( !Layer::init() )
    {
        return false;
    }

	_multiplay = multiplay;
	_cardsStr = cardsStr;

	// 从json中导入ui
	Layout* mainPanel = static_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		FileUtils::getInstance()->fullPathForFilename("ui/MainScene.json").c_str()));

	auto revertButton = static_cast<Button*>(mainPanel->getChildByName("RevertButton"));
	revertButton->addTouchEventListener(this, toucheventselector(MainScene::revertButtonTouched));

	auto backButton = static_cast<Button*>(mainPanel->getChildByName("BackButton"));
	backButton->addTouchEventListener(this, toucheventselector(MainScene::backButtonTouched));

	_cardSlot1 = static_cast<Layout*>(mainPanel->getChildByName("Card1Panel"));
	_cardSlot2 = static_cast<Layout*>(mainPanel->getChildByName("Card2Panel"));
	_cardSlot3 = static_cast<Layout*>(mainPanel->getChildByName("Card3Panel"));
	_cardSlot4 = static_cast<Layout*>(mainPanel->getChildByName("Card4Panel"));

	_cardSlot1->addTouchEventListener(this, toucheventselector(MainScene::cardTouched));
	_cardSlot2->addTouchEventListener(this, toucheventselector(MainScene::cardTouched));
	_cardSlot3->addTouchEventListener(this, toucheventselector(MainScene::cardTouched));
	_cardSlot4->addTouchEventListener(this, toucheventselector(MainScene::cardTouched));

	this->restart(true);

	this->addChild(mainPanel);

	// 处理该界面的按键操作
	auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyReleased = CC_CALLBACK_2(MainScene::onKeyReleased, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	scheduleUpdate();

	return true;
}

void MainScene::update(float dt)
{
	if (_opponentWin)
	{
		_opponentWin = false;

		// 显示失败
		VictoryLayer* layer = new VictoryLayer();
		layer->init(this,_formula,_multiplay,false);
		layer->autorelease();
		this->addChild(layer,0,LAYER_TAG_BLUETOOTH_VICTORY);
	}
}

void MainScene::cardTouched(Ref *pSender, TouchEventType type)
{
	auto cardLayer = static_cast<Layout*>(pSender);
	auto childrenCount = cardLayer->getChildrenCount();
	if (childrenCount != 1)
	{
		return;
	}

	auto cardSprite = static_cast<CardSprite*>(cardLayer->getChildren().at(0));
	if (!cardSprite->isVisible())
	{
		return;
	}

	int cardTag = cardSprite->getTag();

    switch (type)
	{
		case TOUCH_EVENT_BEGAN:
			{
				CCLOG("Touch began!");
				_cardOldPosition = cardSprite->getPosition();
				cardSprite->setGlobalZOrder(100);
				break;
			}
		case TOUCH_EVENT_MOVED:
			{
				CCLOG("Touch moved!");

				auto offsetPoint = cardLayer->getTouchMovePos() - cardLayer->getTouchStartPos();
				auto cardNewPosition = _cardOldPosition + offsetPoint;
				cardSprite->setPosition(cardNewPosition);
				break;
			}
		case TOUCH_EVENT_ENDED:
		case TOUCH_EVENT_CANCELED:
			{
				CCLOG("Touch ended!");
				
				// 使用Rect的intersectsRect函数判断移动的Rect与其他3个Rect是否发生碰撞
				auto cardSpriteSize = cardSprite->getTexture()->getContentSize();
				auto cardNewPosition = cardLayer->convertToWorldSpaceAR(cardSprite->getPosition());
				auto cardNewRect = Rect(cardNewPosition.x,cardNewPosition.y,cardSpriteSize.width,cardSpriteSize.height);

				CardSprite* toDOCardSprite = nullptr;
				bool happenCollision = false;
				for(auto iter = _cardsRectMap.begin() ; iter != _cardsRectMap.end() ; ++iter)
				{
					// tag相同表示是自己
					if (cardTag == iter->first->getTag())
					{
						continue;
					}

					// 如果卡片不可见，也不算
					if (!iter->first->isVisible())
					{
						continue;
					}

					if (cardNewRect.intersectsRect(iter->second))
					{
						if (happenCollision == false)
						{
							happenCollision = true;
							toDOCardSprite = iter->first;
						}
						else
						{
							// 如果出现了同时与2个Rect碰撞，那就认为没有碰撞
							happenCollision = false;
							break;
						}
					}
				}
				
				cardSprite->setGlobalZOrder(0);
				if (happenCollision)
				{
					CCLOG("Happen collision!");
					
					CalcLayer* layer = new CalcLayer();
					layer->init(cardSprite,toDOCardSprite,this);
					layer->autorelease();
					this->addChild(layer,0);
				}
				else
				{
					this->resetCard(cardSprite);
				}
				
				break;
			}
	}
}

void MainScene::revertButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		int curStep = 4 - _curCardNum;
		if (curStep == 0)
		{
			return;
		}

		StepInfo& lastStepInfo = _stepInfos[curStep - 1];

		CardSprite* cardSprite1 = (CardSprite*)_cardSlot1->getChildren().at(0);
		CardSprite* cardSprite2 = (CardSprite*)_cardSlot2->getChildren().at(0);
		CardSprite* cardSprite3 = (CardSprite*)_cardSlot3->getChildren().at(0);
		CardSprite* cardSprite4 = (CardSprite*)_cardSlot4->getChildren().at(0);

		lastStepInfo.slotInfos[0].getSlotInfo(cardSprite1);
		lastStepInfo.slotInfos[1].getSlotInfo(cardSprite2);
		lastStepInfo.slotInfos[2].getSlotInfo(cardSprite3);
		lastStepInfo.slotInfos[3].getSlotInfo(cardSprite4);

		_curCardNum += 1;
	}
}

void MainScene::backButtonTouched(Ref *pSender, TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		auto newSence = TransitionSlideInL::create(1.f,GameStartSence::scene());
		if (newSence)
		{
			Director::getInstance()->replaceScene(newSence);
		}
	}
}

void MainScene::restart(bool isFirstTime)
{
	_curCardNum = 4;
	_opponentWin = false;
	std::string random4CardsString = _cardsStr;
	_cardsStr.clear();
	_formula.clear();

	int cardNum1,cardNum2,cardNum3,cardNum4;
	cardNum1 = strTo<int>(toStr(random4CardsString[0]));
	cardNum2 = strTo<int>(toStr(random4CardsString[1]));
	cardNum3 = strTo<int>(toStr(random4CardsString[2]));
	cardNum4 = strTo<int>(toStr(random4CardsString[3]));

	if (isFirstTime)
	{
		// 初始化4张牌
		srand((unsigned)time(0));
		CardSprite* cardSprite1 = CardSprite::create(rand()%4,cardNum1);
		CardSprite* cardSprite2 = CardSprite::create(rand()%4,cardNum2);
		CardSprite* cardSprite3 = CardSprite::create(rand()%4,cardNum3);
		CardSprite* cardSprite4 = CardSprite::create(rand()%4,cardNum4);

		// 随便找张卡，取其大小做碰撞检测
		auto cardTexture = Director::getInstance()->getTextureCache()->addImage(FileUtils::getInstance()->fullPathForFilename("card/00.png"));
		auto cardSpriteSize = cardTexture->getContentSize();

		cardSprite1->setTag(1);
		cardSprite2->setTag(2);
		cardSprite3->setTag(3);
		cardSprite4->setTag(4);

		auto card1WorldPosition = _cardSlot1->convertToWorldSpaceAR(cardSprite1->getPosition());
		_cardsRectMap[cardSprite1] = Rect(card1WorldPosition.x,card1WorldPosition.y,cardSpriteSize.width,cardSpriteSize.height);
		auto card2WorldPosition = _cardSlot2->convertToWorldSpaceAR(cardSprite2->getPosition());
		_cardsRectMap[cardSprite2] = Rect(card2WorldPosition.x,card2WorldPosition.y,cardSpriteSize.width,cardSpriteSize.height);
		auto card3WorldPosition = _cardSlot3->convertToWorldSpaceAR(cardSprite3->getPosition());
		_cardsRectMap[cardSprite3] = Rect(card3WorldPosition.x,card3WorldPosition.y,cardSpriteSize.width,cardSpriteSize.height);
		auto card4WorldPosition = _cardSlot4->convertToWorldSpaceAR(cardSprite4->getPosition());
		_cardsRectMap[cardSprite4] = Rect(card4WorldPosition.x,card4WorldPosition.y,cardSpriteSize.width,cardSpriteSize.height);
	
		cardSprite1->setAnchorPoint(Point(0,0));
		cardSprite2->setAnchorPoint(Point(0,0));
		cardSprite3->setAnchorPoint(Point(0,0));
		cardSprite4->setAnchorPoint(Point(0,0));

		cardSprite1->resetFormula();
		cardSprite2->resetFormula();
		cardSprite3->resetFormula();
		cardSprite4->resetFormula();

		_cardSlot1->addChild(cardSprite1);
		_cardSlot2->addChild(cardSprite2);
		_cardSlot3->addChild(cardSprite3);
		_cardSlot4->addChild(cardSprite4);
	}
	else
	{
		CardSprite* cardSprite1 = (CardSprite*)_cardSlot1->getChildren().at(0);
		CardSprite* cardSprite2 = (CardSprite*)_cardSlot2->getChildren().at(0);
		CardSprite* cardSprite3 = (CardSprite*)_cardSlot3->getChildren().at(0);
		CardSprite* cardSprite4 = (CardSprite*)_cardSlot4->getChildren().at(0);

		cardSprite1->setVisible(true);
		cardSprite2->setVisible(true);
		cardSprite3->setVisible(true);
		cardSprite4->setVisible(true);

		srand((unsigned)time(0));
		cardSprite1->setCategoryAndNum(rand()%4,cardNum1);
		cardSprite2->setCategoryAndNum(rand()%4,cardNum2);
		cardSprite3->setCategoryAndNum(rand()%4,cardNum3);
		cardSprite4->setCategoryAndNum(rand()%4,cardNum4);

		cardSprite1->resetFormula();
		cardSprite2->resetFormula();
		cardSprite3->resetFormula();
		cardSprite4->resetFormula();
	}

	recordStepInfo();
}

void MainScene::resetCard(CardSprite* card)
{
	card->setPosition(_cardOldPosition);
}

void MainScene::clearCard(CardSprite* card)
{
	card->setPosition(_cardOldPosition);
	card->setVisible(false);
	_curCardNum -= 1;
	recordStepInfo();

	// 删除一张后要检查是否胜利
	// 胜利条件为剩下最后一张卡片，且其为24
	if (_curCardNum == 1)
	{
		CardSprite* cardSprite1 = (CardSprite*)_cardSlot1->getChildren().at(0);
		CardSprite* cardSprite2 = (CardSprite*)_cardSlot2->getChildren().at(0);
		CardSprite* cardSprite3 = (CardSprite*)_cardSlot3->getChildren().at(0);
		CardSprite* cardSprite4 = (CardSprite*)_cardSlot4->getChildren().at(0);

		CardSprite* lastCard = nullptr;

		if (cardSprite1->isVisible())
		{
			lastCard = cardSprite1;
		}
		else if (cardSprite2->isVisible())
		{
			lastCard = cardSprite2;
		}
		else if (cardSprite3->isVisible())
		{
			lastCard = cardSprite3;
		}
		else if (cardSprite4->isVisible())
		{
			lastCard = cardSprite4;
		}
		else
		{
		}

		if (lastCard && lastCard->getNum() == 24)
		{
			//auto delay1SecAction = DelayTime::create(1.f);
			//auto victorySeq = Sequence::create(delay1SecAction
			//	,CallFunc::create(CC_CALLBACK_0(MainScene::showVictory,this))
			//	,NULL);
			//this->runAction(victorySeq);
			_formula = lastCard->getOriginalFormula() + std::string("=24");
			showVictory();
		}
	}
}

void MainScene::setRandom4cardsString(const std::string& cardsStr)
{
	_cardsStr = cardsStr;
}
void MainScene::setFormula(const std::string& formula)
{
	_formula = formula;
}
void MainScene::setOpponentWin()
{
	_opponentWin = true;
}

void MainScene::showVictory()
{
	// 将新卡牌事先准备好
	// 多人游戏时，由胜利方下发给失败方
	_cardsStr = getRandom4CardsString();

	// 对战时
	if (_multiplay)
	{
		// 告知对方我已经赢了，对方弹出失败界面
		rapidjson::Document d1;
		d1.SetObject();
		rapidjson::Document::AllocatorType& allocator = d1.GetAllocator();
		d1.AddMember("cmd", "showLose", allocator);
		d1.AddMember("cards", _cardsStr.c_str(), allocator);
		d1.AddMember("formula", _formula.c_str(), allocator);
  
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> write(buffer);
		d1.Accept(write);

		std::string jsonStr = StringUtils::format("%s",buffer.GetString());

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		JniMethodInfo minfo;
		jobject jobj;    
		bool b = JniHelper::getStaticMethodInfo(minfo,     
			"org/cocos2dx/cpp/BluetoothHelper",  
			"sendBluetoothMessage",
			"(Ljava/lang/String;)V");
		jstring jJsonStr = minfo.env->NewStringUTF(jsonStr.c_str());
		jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID, jJsonStr);
		minfo.env->DeleteLocalRef(jJsonStr);
#endif
	}
	
	// 显示胜利
	VictoryLayer* layer = new VictoryLayer();
	layer->init(this,_formula,_multiplay,true);
	layer->autorelease();
	this->addChild(layer,0,LAYER_TAG_BLUETOOTH_VICTORY);
}

void MainScene::recordStepInfo()
{
	int curStep = 4 - _curCardNum;

	StepInfo& curStepInfo = _stepInfos[curStep];

	CardSprite* cardSprite1 = (CardSprite*)_cardSlot1->getChildren().at(0);
	CardSprite* cardSprite2 = (CardSprite*)_cardSlot2->getChildren().at(0);
	CardSprite* cardSprite3 = (CardSprite*)_cardSlot3->getChildren().at(0);
	CardSprite* cardSprite4 = (CardSprite*)_cardSlot4->getChildren().at(0);

	curStepInfo.slotInfos[0].setSlotInfo(cardSprite1);
	curStepInfo.slotInfos[1].setSlotInfo(cardSprite2);
	curStepInfo.slotInfos[2].setSlotInfo(cardSprite3);
	curStepInfo.slotInfos[3].setSlotInfo(cardSprite4);
}

void MainScene::onKeyReleased(EventKeyboard::KeyCode key, Event* event)
{
	if (EventKeyboard::KeyCode::KEY_RETURN == key
		|| EventKeyboard::KeyCode::KEY_ESCAPE == key
		|| EventKeyboard::KeyCode::KEY_BACKSPACE == key)
    {
		auto newSence = TransitionSlideInL::create(1.f,GameStartSence::scene());
		if (newSence)
		{
			Director::getInstance()->replaceScene(newSence);
		}
    }
}

void MainScene::SlotInfo::setSlotInfo(CardSprite* card)
{
	if (card->isVisible())
	{
		isEmpty = false;
		cardCategory = card->getCategory();
		cardNum = card->getNum();
		isMultiCard = card->isMultiCard();
		formula = card->getOriginalFormula();
	}
	else
	{
		isEmpty = true;
	}
}
void MainScene::SlotInfo::getSlotInfo(CardSprite* card)
{
	if (isEmpty)
	{
		card->setVisible(false);
	}
	else
	{
		card->setVisible(true);
		card->setCategoryAndNum(cardCategory,cardNum);
		card->setMultiCard(isMultiCard);
		card->setFormula(formula);
	}
}
