
// TrackerAppDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrackerApp.h"
#include "TrackerAppDlg.h"
#include "afxdialogex.h"
#include "CvvImage.h"
#include "CameraDS.h"
#include <io.h>
#include <fcntl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static void redirect_io_to_console()
{
    const WORD MAX_CONSOLE_LINES = 500;

    if (!AttachConsole(-1))
    {
        AllocConsole();

        CONSOLE_SCREEN_BUFFER_INFO coninfo;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
        coninfo.dwSize.Y = MAX_CONSOLE_LINES;
        SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
    }

    long lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    int hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    FILE* fp = _fdopen(hConHandle, "w");
    *stdout = *fp;
    setvbuf(stdout, NULL, _IONBF, 0);

    lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "r");
    *stdin = *fp;
    setvbuf(stdin, NULL, _IONBF, 0);

    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "w");
    *stderr = *fp;
    setvbuf(stderr, NULL, _IONBF, 0);

    std::ios::sync_with_stdio();
}


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTrackerAppDlg 对话框



CTrackerAppDlg::CTrackerAppDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTrackerAppDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTrackerAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTrackerAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(ID_RUN_CAM, &CTrackerAppDlg::OnBnClickedRunCam)
    ON_BN_CLICKED(ID_STOP_CAM, &CTrackerAppDlg::OnBnClickedStopCam)
    ON_BN_CLICKED(ID_LOAD_IMAGE, &CTrackerAppDlg::OnBnClickedLoadImage)
END_MESSAGE_MAP()


// CTrackerAppDlg 消息处理程序

BOOL CTrackerAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
    CvSize ImgSize;
    ImgSize.width = IMAGE_WIDTH;
    ImgSize.height = IMAGE_HEIGHT;
    m_image = cvCreateImage(ImgSize, IPL_DEPTH_8U, IMAGE_CHANNELS);

    redirect_io_to_console();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTrackerAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTrackerAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

void CTrackerAppDlg::ResizeImage(IplImage* img)
{
    int w = img->width;
    int h = img->height;

    int max = (w > h) ? w : h;

    float scale = (float)((float)max / 256.0f);

    int nw = (int)(w / scale);
    int nh = (int)(h / scale);

    int tlx = (nw > nh) ? 0 : (int)(256 - nw) / 2;
    int tly = (nw > nh) ? (int)(256 - nh) / 2 : 0;

    cvSetImageROI(m_image, cvRect(tlx, tly, nw, nh));

    cvResize(img, m_image);

    cvResetImageROI(m_image);
}

void CTrackerAppDlg::ShowImage(IplImage* img, UINT ID)    // ID 是Picture Control控件的ID号
{
    CDC* pDC = GetDlgItem(ID)->GetDC();
    HDC hDC = pDC->GetSafeHdc();

    CRect rect;
    GetDlgItem(ID)->GetClientRect(&rect);
    int rw = rect.right - rect.left;
    int rh = rect.bottom - rect.top;
    int iw = img->width;
    int ih = img->height;
    int tx = (int)(rw - iw) / 2;
    int ty = (int)(rh - ih) / 2;
    SetRect(rect, tx, ty, tx + iw, ty + ih);

    CvvImage cimg;
    cimg.CopyOf(img);
    cimg.DrawToHDC(hDC, &rect);

    ReleaseDC(pDC);
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTrackerAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTrackerAppDlg::OnBnClickedRunCam()
{
    // TODO: Add your control notification handler code here
    // TODO: Add your control notification handler code here
    CStringA camName;
    camName.Format("%s%d", "Camera # ", 0);
    UpdateData(FALSE);
    printf(("open %s\n"), camName.GetBuffer());


    CCameraDS camera;


    //if(! camera.OpenCamera(0, true))
    if ((!camera.OpenCamera(0, false, 960, 540)))
    {
        AfxMessageBox(_T("Can not open camera.\n"));
        return;
    }


    GetDlgItem(ID_RUN_CAM)->EnableWindow(FALSE);
    GetDlgItem(ID_STOP_CAM)->EnableWindow(TRUE);


    IplImage* cFrame;
    cvNamedWindow("camera");
    cvResizeWindow("camera", 1, 1);
    HWND hWnd = (HWND)cvGetWindowHandle("camera");
    HWND hParent = ::GetParent(hWnd);
    HWND hwnd1 = ::FindWindowA("CTrackerAppDlg", "CTrackerApp");
    ::SetParent(hWnd, hwnd1);
    ::ShowWindow(hParent, SW_HIDE);


    if (m_image)
        cvZero(m_image);


    CvFont tFont;
    cvInitFont(&tFont, CV_FONT_HERSHEY_COMPLEX, 0.5f, 0.7f, 0, 1, 8);


    char timestr[26];
    memset(timestr, 0, 26 * sizeof(char));

    m_run = 1;
    while (m_run)
    {
        cFrame = camera.QueryFrame();
        IplImage* newframe = cvCloneImage(cFrame);

        ResizeImage(newframe);
        ShowImage(m_image, IDC_PRV_STATIC);

        if (cvWaitKey(20) == 27) break;
        cvReleaseImage(&newframe);
    }


    if (m_image)
    {
        cvZero(m_image);
        ShowImage(m_image, IDC_PRV_STATIC);
    }

    camera.CloseCamera();
    cvDestroyWindow("camera");


    GetDlgItem(ID_RUN_CAM)->EnableWindow(TRUE);
    GetDlgItem(ID_STOP_CAM)->EnableWindow(FALSE);
}


void CTrackerAppDlg::OnBnClickedStopCam()
{
    // TODO: Add your control notification handler code here
    m_run = 0;
    printf("close camera\n");
}


void WCSTOMBS(WCHAR*buf, CHAR*& mbsPtr, size_t& mbs_len)
{
    mbs_len = WideCharToMultiByte(CP_ACP, 0, buf, -1, NULL, 0, NULL, NULL);
    mbsPtr = new CHAR[mbs_len + 1];  //分配内存空间
    if (mbsPtr != NULL)
    {
        //将char字串转换为wchar_t字串
        WideCharToMultiByte(CP_ACP, 0, buf, lstrlenW(buf) + 1, mbsPtr, mbs_len + 1, NULL, NULL);
    }
}

void CTrackerAppDlg::OnBnClickedLoadImage()
{
    // TODO: Add your control notification handler code here
    CFileDialog dlg(
        TRUE, _T("*.bmp"), NULL,
        OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
        _T("image files (*.bmp; *.jpg) |*.bmp; *.jpg; *.jpeg | All Files (*.*) |*.*||"), NULL
        );
    dlg.m_ofn.lpstrTitle = _T("Open Image");
    if (dlg.DoModal() != IDOK)
        return;

    CString mPath = dlg.GetPathName();

    CHAR* mbsPtr;
    size_t mbs_len;
    WCSTOMBS(mPath.GetBuffer(), mbsPtr, mbs_len);

    IplImage* ipl = cvLoadImage(mbsPtr, 1);
    printf(("load image %s\n"), mbsPtr);

    /*
    try {
        std::vector<std::string> result = api::plate_recognize(mbsPtr,
            "svm.xml",
            "ann.xml",
            true);
        for (std::vector<std::string>::iterator iter = result.begin();
            iter != result.end(); ++iter){
            printf("detected %s\n", iter->c_str());
        }

    }
    catch (...)
    {
        printf("failed to recognize plate\n");
    }*/


    delete[] mbsPtr;
    if (!ipl)
        return;
    if (m_image)
        cvZero(m_image);



    ResizeImage(ipl);
    ShowImage(m_image, IDC_PRV_STATIC);
    cvReleaseImage(&ipl);


}


