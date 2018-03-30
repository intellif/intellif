/*
 * @file    FaceDataItem.cpp
 * @date    2015-06-24
 * @brief   the data item passed through while data goes through the face processing pipeline
 * @author  zhong.bin (zhong.bin@intellif.com)
 *
 * Copyright (c) 2015, Shenzhen Intellifusion Technologies Co., Ltd.
 */

#include "FaceListItem.h"

extern int g_mdwMatOutputCnt;
extern FILE * pfResustFile;

FaceRecoder::FaceRecoder(const string &      sOrgImgPath,
						int64_t      dwFaceCidId,
						const string &      sImgTable,
						const unsigned int  dwFaceSeq,
						float               fCmpScore)
{

	msOrgImgPath = sOrgImgPath;
	mdwFaceCidId = dwFaceCidId;
	msImgTable = sImgTable;
	mdwFaceSeq = dwFaceSeq;
	mfScore = fCmpScore;
}

FaceRecoder::FaceRecoder(const string &      sOrgImgPath,
	const IF_FACERECT & rFaceRect,
	const unsigned int  dwFaceSeq,
	const float  *      pfFaceFeature,
	float               fCmpScore)
{

	msOrgImgPath = sOrgImgPath;
	mdwFaceSeq = dwFaceSeq;
	mFaceRect = rFaceRect;
	mfScore = fCmpScore;

	for (int dwFeatureIdx = 0; dwFeatureIdx < FACE_FEATURE_LEN; dwFeatureIdx++)
	{
		afFeature[dwFeatureIdx] = pfFaceFeature[dwFeatureIdx];
	}
}

FaceRecoder::~FaceRecoder()
{

}

//string &    getOrgImgPath(void);
unsigned int  FaceRecoder::getFaceSeq(void)
{
	return mdwFaceSeq;
}

float *       FaceRecoder::getFeature(void)
{
	return afFeature;
}

string &      FaceRecoder::getImgPath(void)
{
	return msOrgImgPath;
}
string &      FaceRecoder::getImgTable(void)
{
	return msImgTable;
}

int64_t      FaceRecoder::getImgCidId(void)
{
	return mdwFaceCidId;
}

float         FaceRecoder::getCmpScore(void)
{
	return mfScore;
}


IplImage*     FaceRecoder::cvGetSubImage(IplImage *image, CvRect roi)
{
	IplImage *result;
	cvSetImageROI(image, roi);
	result = cvCreateImage(cvSize(roi.width, roi.height), image->depth, image->nChannels);
	cvCopy(image, result);
	cvResetImageROI(image);
	return result;
}


void          FaceRecoder::saveFace2Disk(path & sSave2FolderOrg, path & sSave2FolderFace)
{
	path   orgImgPath(msOrgImgPath);

	path   save2PathOrg = sSave2FolderOrg / orgImgPath.leaf();
	path   save2PathFace = sSave2FolderFace / orgImgPath.leaf();

	//if (!exists(saveDirect))
	//{
	//  printf("a result founded, but folder not exit, can not saved it");
	//  continue;
	//}

	IplImage * pOrgImg = cvLoadImage(msOrgImgPath.c_str());


	if (pOrgImg == NULL)
	{
		printf("%s load failed\n", msOrgImgPath.c_str());
	}

	int retLeft = (mFaceRect.Rect.left > FACE_SAVE_PADDING_H)
		? (mFaceRect.Rect.left - FACE_SAVE_PADDING_H)
		: 0;
	int retRight = (mFaceRect.Rect.right < (pOrgImg->width - FACE_SAVE_PADDING_H))
		? (mFaceRect.Rect.right + FACE_SAVE_PADDING_H)
		: (pOrgImg->width - 1);

	int retTop = (mFaceRect.Rect.top  > FACE_SAVE_PADDING_V)
		? (mFaceRect.Rect.top - FACE_SAVE_PADDING_V)
		: 0;
	int retBottom = (mFaceRect.Rect.bottom < (pOrgImg->height - FACE_SAVE_PADDING_V))
		? (mFaceRect.Rect.bottom + FACE_SAVE_PADDING_V)
		: (pOrgImg->height - 1);

	CvRect     oRoiImg = cvRect(retLeft,
		retTop,
		retRight - retLeft,
		retBottom - retTop);

	//copy_file(srcImgpath, dstFile);


	IplImage * pRoiImg = cvGetSubImage(pOrgImg, oRoiImg);

	//imwrite("test.jpg",subImg);
	int adwJpgPara[3];
	adwJpgPara[0] = CV_IMWRITE_JPEG_QUALITY;
	adwJpgPara[1] = 100;
	adwJpgPara[2] = 0;

	cvSaveImage(save2PathFace.string().c_str(), pRoiImg, adwJpgPara);

	//if (g_dwSaveOrgEn > 0)
	{
		cvSaveImage(save2PathOrg.string().c_str(), pOrgImg, adwJpgPara);
	}

	cvReleaseImage(&pRoiImg);
	cvReleaseImage(&pOrgImg);
}


void  FaceRecoder::setCmpScore(float fScore)
{
	mfScore = fScore;
}


NameListItem::NameListItem(const string& rImgPath,
	const string& rImgTable,
	int64_t       dwFaceCidId,
	float *       pfFeature,
	int           dwFeatureLen)
{
	mdwFeatureLen = dwFeatureLen;
	msImgPath = rImgPath;
	mdwFaceCidId = dwFaceCidId;
	msImgTable = rImgTable;

	mdwRegressStatus = 0;

	for (int dwFeaIdx = 0; dwFeaIdx < dwFeatureLen; dwFeaIdx++)
	{
		mafFeature[dwFeaIdx] = pfFeature[dwFeaIdx];
	}

}

NameListItem::~NameListItem()
{

}

float *  NameListItem::getFeaturePtr(void)
{
	return mafFeature;
}

int      NameListItem::getFeatureLen(void)
{
	return mdwFeatureLen;
}

int64_t NameListItem::getFaceCidId(void)
{
	return mdwFaceCidId;
}

string & NameListItem::getImgPath(void)
{
	return msImgPath;
}

string & NameListItem::getImgTable(void)
{
	return msImgTable;
}

int      NameListItem::getStatus(void)
{
	return mdwRegressStatus;
}

int  NameListItem::OutputMatinfo2File(FILE * pfRstFile, int dwSaveInputPic, string strPathOut)
{
	boost::mutex::scoped_lock autolocker(mNameListLock);

	cout << "NameListItem::OutputMatinfo2File begin============================" << std::endl;

	path   matNameImgPath(msImgPath);

	string strNameImgName = matNameImgPath.filename().string();

	fprintf(pfRstFile, "%s;", strNameImgName.c_str());

	std::vector<float> acScoreRcd;
	std::vector<int>   acScoreIdx;

	for (FaceInfoMap::iterator faceLoop = mNewNameList.begin();
		faceLoop != mNewNameList.end();
		faceLoop++)
	{
		acScoreRcd.push_back(faceLoop->second->getCmpScore());
		acScoreIdx.push_back(faceLoop->first);;
	}

	//order the output
	for (int dwOutIdx = 0; dwOutIdx < g_mdwMatOutputCnt; dwOutIdx++)
	{
		if (dwOutIdx >= acScoreRcd.size())
		{
			break;
		}

		float maxScore = 0;
		int   outputIdx = 0;
		int   clrRcdIdx = 0;

		for (int dwRcdIdx = 0; dwRcdIdx < acScoreRcd.size(); dwRcdIdx++)
		{
			if (maxScore < acScoreRcd[dwRcdIdx])
			{
				maxScore = acScoreRcd[dwRcdIdx];
				outputIdx = acScoreIdx[dwRcdIdx];
				clrRcdIdx = dwRcdIdx;
			}
		}

		path   matPath(mNewNameList[outputIdx]->getImgPath());

		string strFileName = matPath.filename().string();

		int    dwScoreInt = (int)(maxScore * 100);

		fprintf(pfRstFile, "%s,%d;", strFileName.c_str(), dwScoreInt);

		acScoreRcd[clrRcdIdx] = 0;

		// 存储图片
#pragma region savepicture
		if (dwSaveInputPic)
		{
			string inputpath = mNewNameList[outputIdx]->getImgPath();
			cout << "NameListItem::OutputMatinfo2File::save picture : orgpath = " << inputpath.c_str() << std::endl;
			path nameDirect(msImgPath);
			path tempPath(strPathOut);
			path orgImgPath = tempPath / "faceImg";
			path saveDirectFace = orgImgPath / basename(nameDirect);
			if (!exists(tempPath))
			{
				if (!create_directories(tempPath))
				{
					std::cout << "create folder failed:" << tempPath.string().c_str()
						<< ", Please check the FS Permession settings" << std::endl;
					continue;
				}
			}
			if (!exists(orgImgPath))
			{
				if (!create_directories(orgImgPath))
				{
					std::cout << "create folder failed:" << orgImgPath.string().c_str()
						<< ", Please check the FS Permession settings" << std::endl;
					continue;
				}
			}
			if (!exists(saveDirectFace))
			{
				if (!create_directories(saveDirectFace))
				{
					std::cout << "create folder failed:" << saveDirectFace.string().c_str()
						<< ", Please check the FS Permession settings" << std::endl;
					continue;
				}
			}
			//boost::shared_ptr<Performance> rPerformance(new Performance());
			cout << "table name :" << mNewNameList[outputIdx]->getImgTable().c_str() << "::cid:" << mNewNameList[outputIdx]->getImgCidId() << std::endl;
			//string strBaseName = rPerformance->GetFaceXMFromMysql(mNewNameList[outputIdx]->getImgTable(), mNewNameList[outputIdx]->getImgCidId());
			string strBaseName = "";
			if (strBaseName == "" || strBaseName.empty())
			{
				path srcImgPath(inputpath);
				strBaseName = basename(srcImgPath);
			}
			cout << "strBaseName=" << strBaseName << std::endl;
			std::ostringstream ossFaceName;
			int index = dwOutIdx + 1;
			ossFaceName << index << "_" << mNewNameList[outputIdx]->getCmpScore() << "_" << strBaseName << ".jpg";

			path dstFileFace = saveDirectFace / ossFaceName.str();
			cout << "save picture path =" << dstFileFace.string().c_str() << std::endl;
			//区分是mysql还是本地路径
			Mat matLoad;
			if ((inputpath.find("http:") < inputpath.length()))
			{
				string strDownLoad;
				//bool bSuccess = rPerformance->decodeRemotePicCurl(inputpath, strDownLoad);
				std::vector<uchar> data(strDownLoad.begin(), strDownLoad.end());
				matLoad = imdecode(data, CV_LOAD_IMAGE_COLOR);
			}
			else
			{
				matLoad = imread(inputpath);
			}

			if (matLoad.data == NULL)
			{
				printf("Load:%s, failed \n", inputpath.c_str());
				continue;
			}

			IplImage saveImgFace = IplImage(matLoad);

			int adwJpgPara[3];
			adwJpgPara[0] = CV_IMWRITE_JPEG_QUALITY;
			adwJpgPara[1] = 100;
			adwJpgPara[2] = 0;

			cvSaveImage(dstFileFace.string().c_str(), &saveImgFace, adwJpgPara);
		}
#pragma endregion
	}

	fprintf(pfRstFile, "\n");

	//fclose(mpNamelistFileOutput);

	//mpNamelistFileOutput = NULL;

	return 0;
}

int  NameListItem::SortReslut()
{
	boost::mutex::scoped_lock autolocker(mNameListLock);

	std::vector<float> acScoreRcd;
	std::vector<int>   acScoreIdx;

	for (FaceInfoMap::iterator faceLoop = mNewNameList.begin();
		faceLoop != mNewNameList.end();
		faceLoop++)
	{
		acScoreRcd.push_back(faceLoop->second->getCmpScore());
		acScoreIdx.push_back(faceLoop->first);;
	}

	//order the output
	for (int dwOutIdx = 0; dwOutIdx < g_mdwMatOutputCnt; dwOutIdx++)
	{
		if (dwOutIdx >= acScoreRcd.size())
		{
			break;
		}

		float maxScore = 0;
		int   outputIdx = 0;
		int   clrRcdIdx = 0;

		for (int dwRcdIdx = 0; dwRcdIdx < acScoreRcd.size(); dwRcdIdx++)
		{
			if (maxScore < acScoreRcd[dwRcdIdx])
			{
				maxScore = acScoreRcd[dwRcdIdx];
				outputIdx = acScoreIdx[dwRcdIdx];
				clrRcdIdx = dwRcdIdx;
			}
		}

		int    dwScoreInt = (int)(maxScore * 100);

		acScoreRcd[clrRcdIdx] = 0;
	}

	return 0;
}

void NameListItem::addMatchFace(boost::shared_ptr<FaceRecoder>& rFaceRcd, float fScore)
{
	boost::mutex::scoped_lock autolocker(mNameListLock);

	rFaceRcd->setCmpScore(fScore);

	mNewNameList.insert(FaceInfoMap::value_type(rFaceRcd->getFaceSeq(), rFaceRcd));
}




