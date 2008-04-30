#pragma once
#define CHK_BIN() if(FormatChecker::format!=FormatChecker::BIN){cerr<<__FILE__<<":"<<__LINE__<<": wrong format (should be BIN)!"<<endl;}
#define CHK_XML() if(FormatChecker::format!=FormatChecker::XML){cerr<<__FILE__<<":"<<__LINE__<<": wrong format (should be XML)!"<<endl;}

class FormatChecker{
public:
	enum{XML,BIN};
	static int format;
};
