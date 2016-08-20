#include "CalcLayer.h"
#include "CardSprite.h"
#include "MainScene.h"

USING_NS_CC;
using namespace ui;

bool CalcLayer::init(CardSprite* firstCard,CardSprite* secondCard,MainScene* mainScene)
{
	if ( !LayerColor::initWithColor(Color4B(0, 0, 0, 150)))
    {
        return false;
    }

	// ��json�е���ui
	Layout* mainPanel = static_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		FileUtils::getInstance()->fullPathForFilename("ui/CalcLayer.json").c_str()));

	// ��ť�󶨻ص�
	auto cancelButton = static_cast<Button*>(mainPanel->getChildByName("CancelButton"));
	cancelButton->addTouchEventListener(this, toucheventselector(CalcLayer::cancelButtonTouched));

	_mainScene = mainScene;
	_firstCard = firstCard;
	_secondCard = secondCard;
	int firstCardNum = _firstCard->getNum();
	int secondCardNum = _secondCard->getNum();
	CCLOG("card1:%d,card2:%d",firstCardNum,secondCardNum);

	auto addButton = static_cast<Button*>(mainPanel->getChildByName("AddButton"));
	addButton->addTouchEventListener(this, toucheventselector(CalcLayer::addButtonTouched));

	// �ж��ܲ���������
	auto minusButton = static_cast<Button*>(mainPanel->getChildByName("MinusButton"));
	if (firstCardNum <= secondCardNum)
	{
		//minusButton->setEnabled(false);
		minusButton->setBright(false);
		minusButton->setTouchEnabled(false);
	}
	else
	{
		minusButton->addTouchEventListener(this, toucheventselector(CalcLayer::minusButtonTouched));
	}

	auto multiplyButton = static_cast<Button*>(mainPanel->getChildByName("MultiplyButton"));
	multiplyButton->addTouchEventListener(this, toucheventselector(CalcLayer::multiplyButtonTouched));

	// �ж��ܲ���������
	auto divideButton = static_cast<Button*>(mainPanel->getChildByName("DivideButton"));
	if (firstCardNum != (firstCardNum/secondCardNum)*secondCardNum)
	{
		//divideButton->setEnabled(false);
		divideButton->setBright(false);
		divideButton->setTouchEnabled(false);
	}
	else
	{
		divideButton->addTouchEventListener(this, toucheventselector(CalcLayer::divideButtonTouched));
		divideButton->setBright(true);
	}

	this->addChild(mainPanel);

	// ģ̬Ч��
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	_listener = EventListenerTouchOneByOne::create();
	_listener->onTouchBegan = CC_CALLBACK_2(CalcLayer::onTouchBegan,this);
	_listener->setSwallowTouches(true);
	dispatcher->addEventListenerWithSceneGraphPriority(_listener,this);

	return true;
}

bool CalcLayer::onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent)
{
	return true;
}

void CalcLayer::cancelButtonTouched(Ref *pSender, TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		_mainScene->resetCard(_firstCard);

		auto dispatcher = Director::getInstance()->getEventDispatcher();
		dispatcher->removeEventListener(_listener);

		this->removeFromParentAndCleanup(false);
	}
}

void CalcLayer::addButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		int firstCardNum = _firstCard->getNum();
		int secondCardNum = _secondCard->getNum();

		int newCardNum = firstCardNum + secondCardNum;

		// ������ʽ�ַ���
		std::string formula = _firstCard->getFormula() + "+" + _secondCard->getFormula();
		_secondCard->setMultiCard(true);
		_secondCard->setFormula(formula);

		_secondCard->setCategoryAndNum(_firstCard->getCategory(),newCardNum);
		_mainScene->clearCard(_firstCard);

		// �ر��Լ�
		auto dispatcher = Director::getInstance()->getEventDispatcher();
		dispatcher->removeEventListener(_listener);

		this->removeFromParentAndCleanup(false);
	}
}

void CalcLayer::minusButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		int firstCardNum = _firstCard->getNum();
		int secondCardNum = _secondCard->getNum();

		int newCardNum = firstCardNum - secondCardNum;

		std::string formula = _firstCard->getFormula() + "-" + _secondCard->getFormula();
		_secondCard->setMultiCard(true);
		_secondCard->setFormula(formula);

		_secondCard->setCategoryAndNum(_firstCard->getCategory(),newCardNum);
		_mainScene->clearCard(_firstCard);

		// �ر��Լ�
		auto dispatcher = Director::getInstance()->getEventDispatcher();
		dispatcher->removeEventListener(_listener);

		this->removeFromParentAndCleanup(false);
	}
}

void CalcLayer::multiplyButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		int firstCardNum = _firstCard->getNum();
		int secondCardNum = _secondCard->getNum();

		int newCardNum = firstCardNum * secondCardNum;

		std::string formula = _firstCard->getFormula() + "*" + _secondCard->getFormula();
		_secondCard->setMultiCard(true);
		_secondCard->setFormula(formula);

		_secondCard->setCategoryAndNum(_firstCard->getCategory(),newCardNum);
		_mainScene->clearCard(_firstCard);

		// �ر��Լ�
		auto dispatcher = Director::getInstance()->getEventDispatcher();
		dispatcher->removeEventListener(_listener);

		this->removeFromParentAndCleanup(false);
	}
}

void CalcLayer::divideButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		int firstCardNum = _firstCard->getNum();
		int secondCardNum = _secondCard->getNum();

		int newCardNum = firstCardNum / secondCardNum;

		std::string formula = _firstCard->getFormula() + "/" + _secondCard->getFormula();
		_secondCard->setMultiCard(true);
		_secondCard->setFormula(formula);

		_secondCard->setCategoryAndNum(_firstCard->getCategory(),newCardNum);
		_mainScene->clearCard(_firstCard);

		// �ر��Լ�
		auto dispatcher = Director::getInstance()->getEventDispatcher();
		dispatcher->removeEventListener(_listener);

		this->removeFromParentAndCleanup(false);
	}
}

