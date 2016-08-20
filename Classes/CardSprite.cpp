#include "CardSprite.h"
#include "Util.h"

USING_NS_CC;

CardSprite::CardSprite()
	:_cardNumLabel1(nullptr)
	,_cardNumLabel2(nullptr)
{

}

CardSprite::~CardSprite()
{

}

CardSprite* CardSprite::create(int cardCategory,int cardNum)
{
	CardSprite* pCard = new CardSprite();
	pCard->init(cardCategory,cardNum);
    pCard->autorelease();

    return pCard;
}

bool CardSprite::init(int cardCategory,int cardNum)
{
	std::string cardTextureStr = std::string("card/") + toStr(cardCategory) + toStr(cardNum) + std::string(".png");
	auto cardTexture = Director::getInstance()->getTextureCache()->addImage(FileUtils::getInstance()->fullPathForFilename(cardTextureStr));

	if( Sprite::initWithTexture(cardTexture) )
	{
		if (cardCategory > 3)
		{
			_cardCategory = 3;
		}
		else if (cardCategory < 0)
		{
			_cardCategory = 0;
		}
		else
		{
			_cardCategory = cardCategory;
		}

		if (cardNum > 9)
		{
			_cardNum = 9;
		}
		else if (cardNum < 1)
		{
			_cardNum = 1;
		}
		else
		{
			_cardNum = cardNum;
		}

		if (_cardNumLabel1)
		{
			_cardNumLabel1->setVisible(false);
		}
		if (_cardNumLabel2)
		{
			_cardNumLabel2->setVisible(false);
		}
	}

	return true;
}

void CardSprite::setCategoryAndNum(int cardCategory,int cardNum)
{
	if (_cardCategory == cardCategory && _cardNum == cardNum)
	{
		return;
	}

	_cardCategory = cardCategory;
	_cardNum = cardNum;

	if (_cardNum > 9)
	{
		std::string cardTextureStr = std::string("card/") + toStr(cardCategory) + std::string("0.png");
		auto cardTexture = Director::getInstance()->getTextureCache()->addImage(FileUtils::getInstance()->fullPathForFilename(cardTextureStr));
		setTexture(cardTexture);

		std::string cardNumStr = toStr(_cardNum);

		if (_cardNumLabel1)
		{
			_cardNumLabel1->setVisible(true);
			_cardNumLabel1->setString(cardNumStr);
		}
		else
		{
			_cardNumLabel1 = Label::createWithBMFont(FileUtils::getInstance()->fullPathForFilename("fonts/card_num.fnt"),cardNumStr);
			_cardNumLabel1->setAnchorPoint(Point(0.5,0.5));
			_cardNumLabel1->setScale(1.1f);
			_cardNumLabel1->setPosition(27,229);
			this->addChild(_cardNumLabel1);
		}

		if (_cardNumLabel2)
		{
			_cardNumLabel2->setVisible(true);
			_cardNumLabel2->setString(cardNumStr);
		}
		else
		{
			_cardNumLabel2 = Label::createWithBMFont(FileUtils::getInstance()->fullPathForFilename("fonts/card_num.fnt"),cardNumStr);
			_cardNumLabel2->setAnchorPoint(Point(0.5,0.5));
			_cardNumLabel2->setScale(1.1f);
			auto cardSize = cardTexture->getContentSizeInPixels();
			_cardNumLabel2->setPosition(cardSize.width - 27,cardSize.height - 232);
			_cardNumLabel2->setRotation(180);
			this->addChild(_cardNumLabel2);
		}

		if (_cardCategory == 1 || _cardCategory == 3)
		{
			_cardNumLabel1->setColor(Color3B(196,24,28));
			_cardNumLabel2->setColor(Color3B(196,24,28));
		}
		else
		{
			_cardNumLabel1->setColor(Color3B(62,59,60));
			_cardNumLabel2->setColor(Color3B(62,59,60));
		}
	}
	else if (_cardNum > 0 && _cardNum < 10)
	{
		std::string cardTextureStr = std::string("card/") + toStr(cardCategory) + toStr(cardNum) + std::string(".png");
		auto cardTexture = Director::getInstance()->getTextureCache()->addImage(FileUtils::getInstance()->fullPathForFilename(cardTextureStr));
		setTexture(cardTexture);

		if (_cardNumLabel1)
		{
			_cardNumLabel1->setVisible(false);
		}
		if (_cardNumLabel2)
		{
			_cardNumLabel2->setVisible(false);
		}
	}
	else
	{

	}
}

void CardSprite::setGlobalZOrder(float globalZOrder)
{
	if (_cardNumLabel1)
	{
		_cardNumLabel1->setGlobalZOrder(globalZOrder);
	}
	if (_cardNumLabel2)
	{
		_cardNumLabel2->setGlobalZOrder(globalZOrder);
	}

	Sprite::setGlobalZOrder(globalZOrder);
}

void CardSprite::resetFormula()
{
	_formula = toStr(_cardNum);

	_isMultiCard = false;
}

std::string CardSprite::getFormula()
{
	if (_isMultiCard)
	{
		return std::string("(") + _formula + std::string(")");
	}
	else
	{
		return _formula;
	}
}
