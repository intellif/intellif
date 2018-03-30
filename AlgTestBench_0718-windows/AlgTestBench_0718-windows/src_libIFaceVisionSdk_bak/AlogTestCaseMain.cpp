// FaceSnapIpcCase0.cpp : Defines the entry point for the console application.
//

#include <stdio.h>

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

#include "boost/smart_ptr.hpp"

#ifdef _MSC_VER
#include "IFaceSDK.h"
#else
#include "ifacesdk.h"
#endif

#include "Performance.h"

#include "boost/thread/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread/condition.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>

using namespace cv;
using namespace std;
using namespace boost;
namespace po = boost::program_options;

int   g_mdwMatOutputCnt     = 2;
int   g_mdwAlgVersion		= 0;

int    g_mdwDetectMode       = 0;               
int    g_mdwDetectHfProfile  = 1;
int    g_mdwDetectProfile    = 0;
int    g_mdwDetectRot        = 0;

int    g_mdwIfCompFuncSel = COMPARE_FUNC_TYPE_CONV;
int    g_mdwCompareOrder = COMPARE_FUNC_ORDER_NORMAL;

string g_strConfigFilePath;
string g_strAlgModelPath = "model/gluon";
int g_mdwAlgFeatureLen = 256;
int g_flipExtratFeature = 0;

FILE * pfResustFile;

int g_mdwRefreshMode = 0;

vector<int> g_vecIntDetect;

int main(int argc, char * argv[])
{
	po::options_description desc("Estimate options");
	desc.add_options()
		("help,h", "produce help message")
		("cfgfile,c", po::value<string>(), "configuration file path")
		("checkdate,s", po::value<int>(&g_mdwRefreshMode)->default_value(0), "check only mode");

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		cout << "Usage: options_description [options]\n";
		cout << desc;
		return 0;
	}


	if (vm.count("cfgfile"))
	{
		g_strConfigFilePath = vm["cfgfile"].as<string>();
	}
	else
	{
		g_strConfigFilePath = "IAlgTest.ini";
	}

	printf("#########################################################################\n");
	printf("# --+--  +-------        ++         +-------+  +--------+ (R)           #\n");
	printf("#   |    |              /  \\        |       |  |                        #\n");
	printf("#   |    |             /    \\       |          |                        #\n");
	printf("#   |    +-------     +------+      |          +--------+               #\n");
	printf("#   |    |           /        \\     |          |                        #\n");
	printf("#   |    |          /          \\    |          |                        #\n");
	printf("#   |    |         /            \\   |        | |                        #\n");
	printf("# --+--  |        /              \\  +--------+ +--------+ IntelliFusion #\n");
	printf("#########################################################################\n");

	//日志文件
	//output the result
	string  imgOfTime;
	boost::posix_time::ptime  nowTime(boost::date_time::second_clock<boost::posix_time::ptime>::local_time());
	imgOfTime = to_iso_string(nowTime);

	char   acResultFileName[0x100];
	sprintf(acResultFileName, "result_time_%s.log", imgOfTime.c_str());
	pfResustFile = fopen(acResultFileName, "w");
	if (pfResustFile == NULL)
	{
		printf("Result file:%s, open failed\n", acResultFileName);
		return 0;
	}
	else
	{
		printf("Result file:%s, open success\n", acResultFileName);
	}

	SP_Performance spPerformance(new Performance(g_strConfigFilePath));

	spPerformance->LoadConfigInformation();

	if (g_mdwRefreshMode == 1)
	{
		printf("Program RefreshFeature###!!!\n");
		spPerformance->RefreshFeature();
		printf("Program Over!!!\n");

		fprintf(pfResustFile, "Program Over!!!\n");
		fclose(pfResustFile);
		return 0;
	}

	spPerformance->LoadAndGerateQueryList();

	spPerformance->StartupProcessThreads();

	spPerformance->WaitingPorcessFinished();

	spPerformance->PrintCompareResult();

	spPerformance->Statistics();

	printf("Program Over!!!\n");

	fprintf(pfResustFile, "Program Over!!!\n");
	fclose(pfResustFile);

    return 0;
}



