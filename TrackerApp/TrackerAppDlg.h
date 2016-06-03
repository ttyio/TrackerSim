
// TrackerAppDlg.h : ͷ�ļ�
//

#pragma once
#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/ml.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv/cvaux.h"

using namespace cv;

#define IMAGE_WIDTH 356
#define IMAGE_HEIGHT 300
#define IMAGE_CHANNELS 3

// CTrackerAppDlg �Ի���
class CTrackerAppDlg : public CDialogEx
{
// ����
public:
	CTrackerAppDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TRACKERAPP_DIALOG };


    IplImage* m_image;
    bool m_run;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


    void ResizeImage(IplImage* img);
    void ShowImage(IplImage* img, UINT ID);

// ʵ��
protected:
	HICON m_hIcon;
    

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedRunCam();
    afx_msg void OnBnClickedStopCam();
    afx_msg void OnBnClickedLoadImage();

};
