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

	// ��ʽ��غ��������ڼ�¼�ϳɵ�ǰ���ſ��Ѿ��õ�����Щ��ʽ
	void resetFormula();
	bool isMultiCard() {return _isMultiCard;}
	void setMultiCard(bool isMultiCard) {_isMultiCard = isMultiCard;}
	//bool isLowPriority() {return _islowPriority;}
	//void setLowPriority(bool islowPriority) {_islowPriority = islowPriority;}
	std::string getFormula();
	std::string getOriginalFormula() {return _formula;}
	void setFormula(const std::string& formula) {_formula = formula;}
private:
	int _cardCategory;		// 0Ϊ���ң�1Ϊ���ң�2Ϊ�ݻ���3Ϊ����
	int _cardNum;
	cocos2d::Label* _cardNumLabel1;
	cocos2d::Label* _cardNumLabel2;
	std::string _formula;
	bool _isMultiCard;	// ���ڱ�ʶ��ǰ�����Ƿ�����2�Ż�2�����Ͽ�����϶�����
	bool _islowPriority;	// ����_isMultiCardΪtrueʱ��Ч�����ڱ�ʶ��ǰ��formula��ʲô���ȼ��ģ�PS �����ͣ��ǵ����ȼ��ģ����͡��Ǹ����ȼ���

	
};


#endif // __CARD_SPRITE_H__