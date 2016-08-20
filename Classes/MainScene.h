#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

class CardSprite;

class MainScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* scene(const std::string& cardsStr = "" ,bool multiplay = false);
    virtual bool init(const std::string& cardsStr = "" ,bool multiplay = false);
	virtual void update(float dt);
    
	void cardTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void revertButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void backButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);

	// 这几个函数受CalcLayer回调
	void restart(bool isFirstTime);
	void resetCard(CardSprite* card);
	void clearCard(CardSprite* card);

	// 这几个函数会在对战时被对方调用
	void setRandom4cardsString(const std::string& cardsStr);
	void setFormula(const std::string& formula);
	void setOpponentWin();
private:
	void showVictory();
	void recordStepInfo();
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode key, cocos2d::Event* event);
	
	bool _multiplay;
	bool _opponentWin;
	std::string _cardsStr;
	std::string _formula;
	int _curCardNum;

	cocos2d::ui::Layout* _cardSlot1;
	cocos2d::ui::Layout* _cardSlot2;
	cocos2d::ui::Layout* _cardSlot3;
	cocos2d::ui::Layout* _cardSlot4;
	cocos2d::Point _cardOldPosition;
    std::map<CardSprite*,cocos2d::Rect> _cardsRectMap;		// 做碰撞检测用的map
	
	/// 记录每一步的情况，用于撤销
	struct SlotInfo		// 卡槽情况
	{
		bool isEmpty;
		int cardCategory;
		int cardNum;
		bool isMultiCard;
		std::string formula;

		void setSlotInfo(CardSprite* card);
		void getSlotInfo(CardSprite* card);
	};
	struct StepInfo		// 每一步的情况
	{
		SlotInfo	slotInfos[4];
	};
	StepInfo _stepInfos[4];

	unsigned long _backKeyPressedTime;
};


#endif // __MAIN_SCENE_H__