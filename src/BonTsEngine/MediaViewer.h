// MediaViewer.h: CMediaViewer クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#pragma once


#include <Bdaiface.h>
#include "MediaDecoder.h"
#include "TsUtilClass.h"
#include "../DirectShowFilter/DirectShowUtil.h"
#include "../DirectShowFilter/AudioDecFilter.h"
#include "../DirectShowFilter/VideoRenderer.h"
#include "../DirectShowFilter/ImageMixer.h"
#include "../DirectShowFilter/VideoParser.h"


/////////////////////////////////////////////////////////////////////////////
// メディアビューア(映像及び音声を再生する)
/////////////////////////////////////////////////////////////////////////////
// Input	#0	: CTsPacket		入力データ
/////////////////////////////////////////////////////////////////////////////

class CMediaViewer : public CMediaDecoder
{
public:
	enum EVENTID {
		EID_VIDEO_SIZE_CHANGED	// 映像のサイズが変わった
	};
	enum {
		PID_INVALID=0xFFFF
	};

	CMediaViewer(IEventHandler *pEventHandler = NULL);
	virtual ~CMediaViewer();

// CMediaDecoder
	void Reset() override;
	const bool InputMedia(CMediaData *pMediaData, const DWORD dwInputIndex = 0UL) override;

// CMediaViewer
	bool OpenViewer(HWND hOwnerHwnd, HWND hMessageDrainHwnd,
		CVideoRenderer::RendererType RendererType, BYTE VideoStreamType,
		LPCWSTR pszVideoDecoder = NULL, LPCWSTR pszAudioDevice = NULL);
	void CloseViewer();
	bool IsOpen() const;

	bool Play();
	bool Stop();
	bool Pause();
	bool Flush();

	bool SetVisible(bool fVisible);
	void HideCursor(bool bHide);
	bool RepaintVideo(HWND hwnd,HDC hdc);
	bool DisplayModeChanged();

	void Set1SegMode(bool b1Seg);
	bool Is1SegMode() const;

	bool SetVideoPID(const WORD wPID);
	bool SetAudioPID(const WORD wPID, const bool bUseMap = false);
	WORD GetVideoPID() const;
	WORD GetAudioPID() const;

	bool SetViewSize(const int Width, const int Height);
	bool GetVideoSize(WORD *pwWidth, WORD *pwHeight) const;
	bool GetVideoAspectRatio(BYTE *pbyAspectRatioX, BYTE *pbyAspectRatioY) const;
	bool ForceAspectRatio(int AspectX, int AspectY);
	bool GetForceAspectRatio(int *pAspectX, int *pAspectY) const;
	bool GetEffectiveAspectRatio(BYTE *pAspectX, BYTE *pAspectY) const;
	struct ClippingInfo {
		int Left,Right,HorzFactor;
		int Top,Bottom,VertFactor;
		ClippingInfo() : Left(0), Right(0), HorzFactor(0), Top(0), Bottom(0), VertFactor(0) {}
	};
	bool SetPanAndScan(int AspectX, int AspectY, const ClippingInfo *pClipping = NULL);
	bool GetClippingInfo(ClippingInfo *pClipping) const;
	enum ViewStretchMode {
		STRETCH_KEEPASPECTRATIO,	// アスペクト比保持
		STRETCH_CUTFRAME,			// 全体表示(収まらない分はカット)
		STRETCH_FIT					// ウィンドウサイズに合わせる
	};
	bool SetViewStretchMode(ViewStretchMode Mode);
	ViewStretchMode GetViewStretchMode() const { return m_ViewStretchMode; }
	bool SetNoMaskSideCut(bool bNoMask, bool bAdjust = true);
	bool GetNoMaskSideCut() const { return m_bNoMaskSideCut; }
	bool SetIgnoreDisplayExtension(bool bIgnore);
	bool GetIgnoreDisplayExtension() const { return m_bIgnoreDisplayExtension; }
	bool GetOriginalVideoSize(WORD *pWidth, WORD *pHeight) const;
	bool GetCroppedVideoSize(WORD *pWidth, WORD *pHeight) const;
	bool GetSourceRect(RECT *pRect) const;
	bool GetDestRect(RECT *pRect) const;
	bool GetDestSize(WORD *pWidth, WORD *pHeight) const;

	bool SetVolume(const float fVolume);
	enum {
		AUDIO_CHANNEL_DUALMONO	= 0x00,
		AUDIO_CHANNEL_INVALID	= 0xFF
	};
	BYTE GetAudioChannelNum() const;
	bool SetStereoMode(const int iMode);
	int GetStereoMode() const;
	bool SetSpdifOptions(const CAudioDecFilter::SpdifOptions *pOptions);
	bool GetSpdifOptions(CAudioDecFilter::SpdifOptions *pOptions) const;
	bool IsSpdifPassthrough() const;
	bool SetAutoStereoMode(int Mode);
	bool SetDownMixSurround(bool bDownMix);
	bool GetDownMixSurround() const;
	bool SetAudioGainControl(bool bGainControl, float Gain = 1.0f, float SurroundGain = 1.0f);

	bool GetVideoDecoderName(LPWSTR pszName,int Length) const;
	bool GetVideoRendererName(LPTSTR pszName, int Length) const;
	bool GetAudioRendererName(LPWSTR pszName, int Length) const;
	CVideoRenderer::RendererType GetVideoRendererType() const;
	BYTE GetVideoStreamType() const;
	enum PropertyFilter {
		PROPERTY_FILTER_VIDEODECODER,
		PROPERTY_FILTER_VIDEORENDERER,
		PROPERTY_FILTER_MPEG2DEMULTIPLEXER,
		PROPERTY_FILTER_AUDIOFILTER,
		PROPERTY_FILTER_AUDIORENDERER
	};
	bool DisplayFilterProperty(PropertyFilter Filter, HWND hwndOwner);
	bool FilterHasProperty(PropertyFilter Filter);

	bool SetUseAudioRendererClock(bool bUse);
	bool GetUseAudioRendererClock() const { return m_bUseAudioRendererClock; }
	bool SetAdjustAudioStreamTime(bool bAdjust);
	bool SetAudioStreamCallback(CAudioDecFilter::StreamCallback pCallback, void *pParam = NULL);
	bool SetAudioFilter(LPCWSTR pszFilterName);
	bool GetCurrentImage(BYTE **ppDib);
	bool DrawText(LPCTSTR pszText,int x,int y,HFONT hfont,COLORREF crColor,int Opacity);
	bool IsDrawTextSupported() const;
	bool ClearOSD();
	bool EnablePTSSync(bool bEnable);
	bool IsPTSSyncEnabled() const;
	bool SetAdjust1SegVideoSampleTime(bool bAdjust);
	bool SetAdjust1SegFrameRate(bool bAdjust);
	void ResetBuffer();
	bool SetBufferSize(size_t Size);
	bool SetInitialPoolPercentage(int Percentage);
	int GetBufferFillPercentage() const;
	bool SetPacketInputWait(DWORD Wait);
	DWORD GetAudioBitRate() const;
	DWORD GetVideoBitRate() const;

protected:
	static void CALLBACK OnVideoInfo(const CVideoParser::VideoInfo *pVideoInfo, const LPVOID pParam);
	bool AdjustVideoPosition();
	bool CalcSourceRect(RECT *pRect) const;
	void ConnectVideoDecoder(LPCTSTR pszCodecName, const GUID &MediaSubType,
							 LPCTSTR pszDecoderName, IPin **ppOutputPin);
	bool MapVideoPID(WORD PID);
	bool MapAudioPID(WORD PID);

	bool m_bInit;

	// DirectShowインタフェース
	IGraphBuilder *m_pFilterGraph;
	IMediaControl *m_pMediaControl;

	// DirectShowフィルタ
	IBaseFilter *m_pVideoDecoderFilter;
	// Source
	class CBonSrcFilter *m_pSrcFilter;
	// 音声デコーダ
	CAudioDecFilter *m_pAudioDecoder;
	// 音声フィルタ
	LPWSTR m_pszAudioFilterName;
	IBaseFilter *m_pAudioFilter;
	// 映像レンダラ
	CVideoRenderer *m_pVideoRenderer;
	// 音声レンダラ
	IBaseFilter *m_pAudioRenderer;
	// 映像パーサ
	IBaseFilter *m_pVideoParserFilter;
	CVideoParser *m_pVideoParser;

	LPWSTR m_pszVideoDecoderName;

	// MPEG2Demultiplexerインタフェース
	IBaseFilter *m_pMp2DemuxFilter;

	// PIDマップ
	IMPEG2PIDMap *m_pMp2DemuxVideoMap;
	IMPEG2PIDMap *m_pMp2DemuxAudioMap;

	// Elementary StreamのPID
	WORD m_wVideoEsPID;
	WORD m_wAudioEsPID;
	WORD m_MapAudioPID;

	WORD m_wVideoWindowX;
	WORD m_wVideoWindowY;
	CVideoParser::VideoInfo m_VideoInfo;
	HWND m_hOwnerWnd;

	mutable CCriticalLock m_ResizeLock;
	CVideoRenderer::RendererType m_VideoRendererType;
	LPWSTR m_pszAudioRendererName;
	BYTE m_VideoStreamType;
	int m_ForceAspectX,m_ForceAspectY;
	ClippingInfo m_Clipping;
	ViewStretchMode m_ViewStretchMode;
	bool m_bNoMaskSideCut;
	bool m_bIgnoreDisplayExtension;
	bool m_bUseAudioRendererClock;
	bool m_b1SegMode;
	bool m_bAdjustAudioStreamTime;
	bool m_bEnablePTSSync;
	bool m_bAdjust1SegVideoSampleTime;
	bool m_bAdjust1SegFrameRate;
	size_t m_BufferSize;
	int m_InitialPoolPercentage;
	DWORD m_PacketInputWait;
	CAudioDecFilter::StreamCallback m_pAudioStreamCallback;
	void *m_pAudioStreamCallbackParam;
	CImageMixer *m_pImageMixer;
	CTracer *m_pTracer;

#ifdef _DEBUG
private:
	HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) const;
	void RemoveFromRot(const DWORD pdwRegister) const;
	DWORD m_dwRegister;
#endif
};
