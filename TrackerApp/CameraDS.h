#ifndef CCAMERA_H
#define CCAMERA_H

#define WIN32_LEAN_AND_MEAN


#include <atlbase.h>
#include "Include/qedit.h"
#include "Include/dshow.h"
#include <windows.h>
#include <opencv/cxcore.h>

#define MYFREEMEDIATYPE(mt)	{if ((mt).cbFormat != 0)		\
					{CoTaskMemFree((PVOID)(mt).pbFormat);	\
					(mt).cbFormat = 0;						\
					(mt).pbFormat = NULL;					\
				}											\
				if ((mt).pUnk != NULL)						\
				{											\
					(mt).pUnk->Release();					\
					(mt).pUnk = NULL;						\
				}}


class CCameraDS
{
private:
	IplImage * m_pFrame;
	bool m_bConnected;
	int m_nWidth;
	int m_nHeight;
	bool m_bLock;
	bool m_bChanged;
	long m_nBufferSize;

	CComPtr<IGraphBuilder> m_pGraph;
	CComPtr<IBaseFilter> m_pDeviceFilter;
	CComPtr<IMediaControl> m_pMediaControl;
	CComPtr<IBaseFilter> m_pSampleGrabberFilter;
	CComPtr<ISampleGrabber> m_pSampleGrabber;
	CComPtr<IPin> m_pGrabberInput;
	CComPtr<IPin> m_pGrabberOutput;
	CComPtr<IPin> m_pCameraOutput;
	CComPtr<IMediaEvent> m_pMediaEvent;
	CComPtr<IBaseFilter> m_pNullFilter;
	CComPtr<IPin> m_pNullInputPin;

private:
	bool BindFilter(int nCamIDX, IBaseFilter **pFilter);
	void SetCrossBar();

public:
	CCameraDS();
	virtual ~CCameraDS();

	bool CCameraDS::OpenCamera(int nCamID, bool bDisplayProperties=true, int nWidth=320, int nHeight=240);

	void CloseCamera();

	static int CameraCount();

	static int CCameraDS::CameraName(int nCamID, char* sName, int nBufferSize);

	int GetWidth(){return m_nWidth;}

	int GetHeight(){return m_nHeight;}

	IplImage * QueryFrame();
};

#endif
