/*
* @file    fuzzy_degree.cpp
* @date    2015-11-14
* @brief   initialize pixel weight, calculate fuzzy degree
* @author  zhang.zhaofeng (zhang.zhaofeng@intellif.com)
*
* Copyright (c) 2015, Shenzhen Intellifusion Technologies Co., Ltd.
*/
#include "fuzzy_degree.h"
#define INIT_FUZZY_DEGREE 0;       //��ʼ��fuzzy_degreeֵΪ0
#define MAX_FUZZY_DEGREE 10000.0;  //<31*31��ͼƬ��ģ����ȡ��ֵ
#define MIN_FUZZY_DEGREE 0.00001;  //>=261*261��ͼƬ��ģ����ȡ��ֵ

//��ͬ��СͼƬ��Ȩ�ؾ���ͳһ����Ϊ180X180�����飬����ͼƬ��λ��������
float pixel_weight_30[30*30] = { 0 }; 
float pixel_weight_40[40*40] = { 0 };
float pixel_weight_50[50*50] = { 0 };
float pixel_weight_60[60*60] = { 0 };
float pixel_weight_70[70*70] = { 0 };
float pixel_weight_80[80*80] = { 0 };
float pixel_weight_90[90*90] = { 0 };
float pixel_weight_100[100*100] = { 0 };
float pixel_weight_110[110*110] = { 0 };
float pixel_weight_120[120*120] = { 0 };
float pixel_weight_130[130*130] = { 0 };
float pixel_weight_140[140*140] = { 0 };
float pixel_weight_150[150*150] = { 0 };
float pixel_weight_160[160*160] = { 0 };
float pixel_weight_170[170*170] = { 0 };
float pixel_weight_180[180*180] = { 0 };
float pixel_weight_190[190*190] = { 0 };
float pixel_weight_200[200*200] = { 0 };
float pixel_weight_210[210 * 210] = { 0 };
float pixel_weight_220[220 * 220] = { 0 };
float pixel_weight_230[230 * 230] = { 0 };
float pixel_weight_240[240 * 240] = { 0 };
float pixel_weight_250[250 * 250] = { 0 };

float*pixel_weight_all[23] = { &pixel_weight_30[0],
                               &pixel_weight_40[0],
                               &pixel_weight_50[0],
                               &pixel_weight_60[0],
                               &pixel_weight_70[0],
                               &pixel_weight_80[0],
                               &pixel_weight_90[0],
                               &pixel_weight_100[0],
                               &pixel_weight_110[0],
                               &pixel_weight_120[0],
                               &pixel_weight_130[0],
                               &pixel_weight_140[0],
                               &pixel_weight_150[0],
                               &pixel_weight_160[0],
                               &pixel_weight_170[0],
                               &pixel_weight_180[0],
                               &pixel_weight_190[0],
                               &pixel_weight_200[0],
                               &pixel_weight_210[0],
                               &pixel_weight_220[0],
                               &pixel_weight_230[0],
                               &pixel_weight_240[0],
                               &pixel_weight_250[0]}; 


//�����˹Ȩ��
void init_guass_pixel_weight(float *pixel_weight_i, int pic_gray_size_dif)
{
    float delta = 0.2;                                 //deltaȡ0.2��ͼ���������Ե��Ȩ�ر�Ϊ12��1
    float pic_gray_size_half = pic_gray_size_dif / 2;  //�趨��Ϊfloat��Ϊ�����ǿ��������׼��
    float s = 0;
    float t = 0;
    float weight_sum = 0;

    //���˹Ȩ��
    for (int i = 0; i < pic_gray_size_dif; i++)
    {
        for (int j = 0; j < pic_gray_size_dif; j++)
        {
            s = (i - pic_gray_size_half) / pic_gray_size_dif;
            t = (j - pic_gray_size_half) / pic_gray_size_dif;

            pixel_weight_i[i+pic_gray_size_dif*j] = exp(-(s*s + t*t) / delta);
            weight_sum = weight_sum + pixel_weight_i[i + pic_gray_size_dif*j];
        }
    }

    //Ȩ�ع�һ��
    for (int i = 0; i < pic_gray_size_dif; i++)
    {
        for (int j = 0; j < pic_gray_size_dif; j++)
        {
            pixel_weight_i[i + pic_gray_size_dif*j] = pixel_weight_i[i + pic_gray_size_dif*j] / weight_sum;
        }
    }
}

//����ÿ�����ص�Ȩ��
void init_pixel_weight()
{
    for (int i = 0; i <= (25-3); i++)
    {
        init_guass_pixel_weight(pixel_weight_all[i], (i+3)*10);
    }
}


//����ģ����
float calculate_fuzzy_degree(Mat & pic_gray)
{
    #if 0
    //����ǲ�ɫͼ��תΪ�Ҷ�ͼ
    Mat pic_gray;
    if (pic.channels() == 3)
    {
        cvtColor(pic, pic_gray, CV_BGR2GRAY);
    }
    else
    {
        pic_gray = pic;
    }
    #endif

    int pic_gray_row;
    int pic_gray_col;
    int pic_gray_size;     //ͼ��ı߳���ͳһΪ������
    int pic_gray_size_dif; //��ֽ������ı߳���ҲΪ�����ξ���

    pic_gray_row = pic_gray.rows;
    pic_gray_col = pic_gray.cols;
    pic_gray_size = (pic_gray_row > pic_gray_col ? pic_gray_col : pic_gray_row); //��ͼƬ���в�һ�£���������������ı߳�ȡ���еĽ�Сֵ

    //printf("pic size is %d", pic_gray_size);

    uchar * pic_gray_data;
    
    if (pic_gray.data)
    {
        pic_gray_data = pic_gray.data;
        //printf("��ȡͼƬ�ɹ�!");
    }
    else
    {
        printf("��ȡͼƬʧ��!");
        return -1;
    }

    float fuzzy_degree;
    //СͼƬ�Ĵ���
    if (pic_gray_size < 31)
    {
        fuzzy_degree = MAX_FUZZY_DEGREE;
    }
    //��ͼƬ�Ĵ���
    if (pic_gray_size >= 261)
    {
        fuzzy_degree = MIN_FUZZY_DEGREE;
    }
    //�еȴ�СͼƬ�Ĵ���
    if (pic_gray_size >= 31 && pic_gray_size < 261)
    {
        //���붯̬���飬�洢ǿתfloat�͵�ͼ��Ҷ�ֵ 
        //���붯̬���飬�洢ǿתfloat�͵�ͼ��Ҷ�ֵ
        int dwMallocLen = (pic_gray_row * sizeof(float *))
                        + (pic_gray_col * pic_gray_row * sizeof(float));
        
        float ** pic_gray_data_float = (float **)malloc(dwMallocLen);
        
        if (pic_gray_data_float == NULL)
        {
            printf("ͼ��uchar�ͻҶ�ֵתfloat�ͻҶ�ֵʱ�����붯̬�ڴ�ʧ�ܣ�");
            return -1;
        }
        
        float * pfDataBase = (float *)(pic_gray_data_float + pic_gray_row);
        
        for (int i = 0; i<pic_gray_row; i++)
        {
            pic_gray_data_float[i] = pfDataBase;  
            pfDataBase += pic_gray_col;
        }

        //��ȡͼƬ��תΪfloat��ʽ
        for (int i = 0; i < pic_gray_row; i++)
        {
            for (int j = 0; j < pic_gray_col; j++)
            {
                pic_gray_data_float[i][j] = (float)(*pic_gray_data);
                //printf("����ֵΪ%f\n", pic_gray_data_float[i][j]);
                pic_gray_data++;
            }
        }

        //ȷ��ͼ��ѡ�ú���Ȩ��
        int size_index = floor((pic_gray_size-1) / 10) - 3;         //��1�����ּ�����Ҫ
        float*pixel_weight_temp = pixel_weight_all[size_index];
        pic_gray_size_dif = floor((pic_gray_size-1) / 10) * 10 ;    //������ģ���ȵ�����ı߳�

        //��ͼ��ҶȲ��ֵ
        fuzzy_degree = INIT_FUZZY_DEGREE;
        int pic_gray_start = (pic_gray_size - pic_gray_size_dif)/2; //���ͼƬ��С������Ȩ�ؾ���Ĵ�С��ȷ����ʼ�������㣬�к���һ��

        int pix_i = 0; 
        int pix_j = 0;
        float pic_gray_gray_avg = 0; //ͼ��ҶȾ�ֵ
        for (int i = 0; i < pic_gray_size_dif; i++)
        {
            for (int j = 0; j < pic_gray_size_dif; j++)
            {
                pix_i = i + pic_gray_start;
                pix_j = j + pic_gray_start;
                fuzzy_degree = fuzzy_degree + (fabs(pic_gray_data_float[pix_i + 1][pix_j] - pic_gray_data_float[pix_i][pix_j])
                               +fabs(pic_gray_data_float[pix_i][pix_j + 1] - pic_gray_data_float[pix_i][pix_j])) *(*pixel_weight_temp);
                pic_gray_gray_avg = pic_gray_gray_avg + pic_gray_data_float[pix_i][j] * (*pixel_weight_temp);
                pixel_weight_temp++;
            }
        }
        fuzzy_degree = (200/pic_gray_gray_avg) * fuzzy_degree;                //��ͼ��ҶȾ�ֵ����ģ����
        float size_compare = 250;
        fuzzy_degree = fuzzy_degree*pow(pic_gray_size_dif/size_compare,0.35); //����ͼƬ��С��ģ���ȵ�Ӱ��

        if (fuzzy_degree < 0.0001)
        {
            fuzzy_degree = MAX_FUZZY_DEGREE;
        }
        else
        {
            fuzzy_degree = 1 / fuzzy_degree;                                        //��Ҫ������ʹ������ȷ����ģ����
        }

        free(pic_gray_data_float);        //���ͷ�ָ��Ԫ��
    }
    return fuzzy_degree;
}



//@briefͨ������ֵ�ҳ�����ͼƬ�е��������������������ⶼ�Ǵ���ɫ��
//@param orig_image ͼƬ
int face_area_detect(Mat  & imgMat, CvRect & face_area)
{
    unsigned int image_width   = imgMat.cols;
    unsigned int image_height  = imgMat.rows;
    uchar *image_data = (uchar *)imgMat.data;
    
    //img,Դ��ɫͼ��;
    //s == 0,����B��s==1������G��s==2������R;
    int step     = (int)imgMat.step;
    int channels = imgMat.channels();
 
    int dwRectTop    = image_height - 1;
    int dwRectBottom = 0;
    int dwRectLeft   = image_width  - 1;
    int dwRectRight  = 0;

    for (unsigned int i = 0; i < image_height; i++)
    {
        for (unsigned int j = 1; j < image_width; j++)
        {
            int b_right = image_data[i*step + j*channels + 0];
            int b_left  = image_data[i*step + (j-1)*channels + 0];
            
            if (b_right != b_left)
            {
                if (i < dwRectTop)
                {
                    dwRectTop = i;
                }

                if (i > dwRectBottom)
                {
                    dwRectBottom = i;
                }

                if (j < dwRectLeft)
                {
                    dwRectLeft = j;
                }

                if (j > dwRectRight)
                {
                    dwRectRight = j;
                }
            }
        }
    }

    face_area.x     = dwRectLeft;
    face_area.y     = dwRectTop;
    face_area.width = dwRectRight  - dwRectLeft;
    face_area.height= dwRectBottom - dwRectTop;
    
    return 0;
}

