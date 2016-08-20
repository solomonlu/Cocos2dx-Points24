#ifndef __CARD_SPRITE_H__
#define __CARD_SPRITE_H__

#include "cocos2d.h"

class CardSprite : public cocos2d::Sprite
{
public:
	static CardSprite* create(int cardCategory,int cardNum);

	CardSprite();
	virtual ~CardSprite();

	bool init(int cardCategory,int cardNum);

	int getNum() {return _cardNum;}
	int getCategory() {return _cardCategory;}
	void setCategoryAndNum(int cardCategory,int cardNum);
	virtual void setGlobalZOrder(float globalZOrder);

	// 算式相关函数，由于记录合成当前这张卡已经用到了哪些算式
	void resetFormula();
	bool isMultiCard() {return _isMultiCard;}
	void setMultiCard(bool isMultiCard) {_isMultiCard = isMultiCard;}
	//bool isLowPriority() {return _islowPriority;}
	//void setLowPriority(bool islowPriority) {_islowPriority = islowPriority;}
	std::string getFormula();
	std::string getOriginalFormula() {return _formula;}
	void setFormula(const std::string& formula) {_formula = formula;}
private:
	int _cardCategory;		// 0为黑桃，1为红桃，2为草花，3为方块
	int _cardNum;
	cocos2d::Label* _cardNumLabel1;
	cocos2d::Label* _cardNumLabel2;
	std::string _formula;
	bool _isMultiCard;	// 用于标识当前卡牌是否是由2张或2张以上卡牌组合而来的
	bool _islowPriority;	// 仅当_isMultiCard为true时有效，用于标识当前的formula是什么优先级的，PS ：＋和－是低优先级的，×和÷是高优先级的

	
};


#endif // __CARD_SPRITE_H__