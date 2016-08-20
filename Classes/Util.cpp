#include "Util.h"
#include "cocos2d.h"

USING_NS_CC;

extern std::set<std::string> g_SolutionSet;

std::string getRandom4CardsString()
{
	std::string originalCardNums;
	std::string cardNums;
	int cardNum1,cardNum2,cardNum3,cardNum4;
	int sortCardNum1,sortCardNum2,sortCardNum3,sortCardNum4;

	srand((unsigned)time(0));
	do
	{
		cardNum1 = (rand()%9)+1;
		cardNum2 = (rand()%9)+1;
		cardNum3 = (rand()%9)+1;
		cardNum4 = (rand()%9)+1;

		char str[64] = {0};
		sprintf(str,"%d%d%d%d",cardNum1,cardNum2,cardNum3,cardNum4);
		originalCardNums = std::string(str);

		std::multiset<int> cardSet;
		cardSet.insert(cardNum1);
		cardSet.insert(cardNum2);
		cardSet.insert(cardNum3);
		cardSet.insert(cardNum4);

		auto iter = cardSet.begin();
		sortCardNum1 = *(iter++);
		sortCardNum2 = *(iter++);
		sortCardNum3 = *(iter++);
		sortCardNum4 = *(iter);

		sprintf(str,"%d%d%d%d",sortCardNum1,sortCardNum2,sortCardNum3,sortCardNum4);
		cardNums = std::string(str);

		CCLOG("random 4 card:%d%d%d%d",sortCardNum1,sortCardNum2,sortCardNum3,sortCardNum4);
	}while(g_SolutionSet.find(cardNums) == g_SolutionSet.end());

	CCLOG("return random 4 card string:%s",originalCardNums.c_str());
	return originalCardNums;
}