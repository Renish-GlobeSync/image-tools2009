// ChildView.cpp : CChildView 类的实现
//

#include "stdafx.h"
#include "ImageTools.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
    ON_WM_PAINT()
    ON_COMMAND(ID_FILE_OPEN, &CChildView::OnFileOpen)
END_MESSAGE_MAP()



// CChildView 消息处理程序

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
    if (!CWnd::PreCreateWindow(cs))
        return FALSE;

    cs.dwExStyle |= WS_EX_CLIENTEDGE;
    cs.style &= ~WS_BORDER;
    cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
        ::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

    return TRUE;
}

void CChildView::OnPaint() 
{
    CPaintDC dc(this); // 用于绘制的设备上下文
    
    CBitmap bm;
    BITMAP b;
    bm.Attach((HBITMAP)::LoadImage(NULL,   
        _T("20084191634391282.bmp"),  
        IMAGE_BITMAP,   
        0,   
        0,   
        LR_LOADFROMFILE));
    bm.GetBitmap(&b);
    CDC dcm;
    dcm.CreateCompatibleDC(GetDC());
    dcm.SelectObject(&bm);
    dc.BitBlt(100, 100, b.bmWidth, b.bmHeight, &dcm, 0, 0, SRCCOPY);
    CBitmap bm1;
    TakeImageTool(bm, b, bm1);
    dcm.SelectObject(&bm1);
    dc.BitBlt(b.bmWidth + 120, 100, b.bmWidth, b.bmHeight, &dcm, 0, 0, SRCCOPY);
    
    // 不要为绘制消息而调用 CWnd::OnPaint()
}

extern "C"
{
#include "operationcolorbalance.h"
#include "operationcolorize.h"

#include <libgimp/gimp.h>

#include "convolution_matrix.h"
#include "value-propagate.h"
#include "deinterlace.h"
#include "nl-filter.h"
#include "red-eye-removal.h"
#include "sharpen.h"
#include "unsharp_mask.h"
#include "lens-flare.h"
#include "nova.h"
#include "lens-apply.h"
#include "lens-distortion.h"
#include "whirl-pinch.h"
#include "cartoon.h"
#include "softglow.h"
#include "oilify.h"
#include "pagecurl.h"
}

color_balance_config cb_config = {
    0, 
    {0.0, 0.0, 0.5}, 
    {0.0, 0.0, 0.0}, 
    {0.0, 0.0, 0.0}
};

colorize_config c_config = {
    0.5, 1.0, .9
};

convolution_matrix_config cm_config = {
  {
    { 0.0, 0.0, 0.0, 0.0, 0.0 },
    { 0.0, 0.0, 0.0, 0.0, 0.0 },
    { 0.0, -1.0,1.0, 0.0, 0.0 },
    { 0.0, 0.0, 0.0, 0.0, 0.0 },
    { 0.0, 0.0, 0.0, 0.0, 0.0 }
  },                 /* matrix */
  1,                 /* divisor */
  0,                 /* offset */
  0,                 /* Alpha-handling algorithm */
  CLEAR,             /* border-mode */
  { TRUE, TRUE, TRUE, TRUE, TRUE }, /* Channels mask */
  FALSE,              /* autoset */
};

deinterlace_config config_deinterlace = {
    FALSE
};

nl_filter_config config_nl_filter = {
  1.0,
  1.0,
  2
};

red_eye_removal_config config_red_eye_removal = {
    50
};

sharpen_config config_sharpen = {
    100
};

unsharp_mask_config config_unsharp_mask = {
    120, /* default radius    */
    6, /* default amount    */
    0    /* default threshold */
};

lens_flare_config config_lens_flare = {
    20, 20
};

nova_config config_nova = {
  128, 128,                 /* xcenter, ycenter */
  { 0.35, 0.39, 1.0, 1.0 }, /* color */
  65,                       /* radius */
  467,                      /* nspoke */
  254                         /* random hue */
};

lens_apply_config config_lens_apply = {
  /* Lens refraction value */
  1.7,
  /* Surroundings options */
  TRUE, FALSE, FALSE
};

lens_distortion_config config_lens_distortion = {
    0.0, 0.0, 0.0, 200.0, 0.0, 100.0
};

whirl_pinch_config config_whirl_pinch = {
    90.0, /* whirl   */
    0.0,  /* pinch   */
    1.0   /* radius  */
};

cartoon_config config_cartoon = {
    50.0,  /* mask_radius */
    1.0,  /* threshold */
    0.2   /* pct_black */
};

softglow_config config_softglow = {
    50.0, /* glow_radius */
    0.75, /* brightness */
    0.85,  /* sharpness */
};

oilify_config config_oilify = {
    8.0,        /* mask size          */
    FALSE,      /* use mask-size map? */
    -1,         /* mask-size map      */
    8.0,        /* exponent           */
    FALSE,      /* use exponent map?  */
    -1,         /* exponent map       */
    MODE_INTEN  /* mode               */
};

pagecurl_config config_pagecurl = {
    CURL_COLORS_FG_BG, 
    1.0, 
    TRUE, 
    CURL_EDGE_LOWER_RIGHT, 
    CURL_ORIENTATION_VERTICAL
};

int CChildView::TakeImageTool(CBitmap & bm, BITMAP & b, CBitmap & bm1)
{
    bm1.CreateBitmapIndirect(&b);

    long n = b.bmWidthBytes * b.bmWidth;
    unsigned char data[102400][4];
    unsigned char dest[102400][4];
    bm.GetBitmapBits(n, data);

    //operation_color_balance(data, dest, b.bmWidth * b.bmHeight, &cb_config);
    //operation_colorize(data, dest, b.bmWidth * b.bmHeight, &c_config);
    //operation_convolution_matrix(data, dest, b.bmWidth, b.bmHeight, &cm_config);
    //operation_dilate(data, dest, b.bmWidth, b.bmHeight, NULL);
    operation_erode(data, dest, b.bmWidth, b.bmHeight, NULL);
    operation_deinterlace(data, dest, b.bmWidth, b.bmHeight, &config_deinterlace);
    operation_nl_filter(data, dest, b.bmWidth, b.bmHeight, &config_nl_filter);
    operation_red_eye_removal(data, dest, b.bmWidth, b.bmHeight, &config_red_eye_removal);
    operation_sharpen(data, dest, b.bmWidth, b.bmHeight, &config_sharpen);
    operation_unsharp_mask(data, dest, b.bmWidth, b.bmHeight, &config_unsharp_mask);
    operation_lens_flare(data, dest, b.bmWidth, b.bmHeight, &config_lens_flare);
    operation_nova(data, dest, b.bmWidth, b.bmHeight, &config_nova);
    operation_lens_apply(data, dest, b.bmWidth, b.bmHeight, &config_lens_apply);
    operation_lens_distortion(data, dest, b.bmWidth, b.bmHeight, &config_lens_distortion);
    operation_whirl_pinch(data, dest, b.bmWidth, b.bmHeight, &config_whirl_pinch);
    operation_cartoon(data, dest, b.bmWidth, b.bmHeight, &config_cartoon);
    operation_softglow(data, dest, b.bmWidth, b.bmHeight, &config_softglow);
    operation_oilify(data, dest, b.bmWidth, b.bmHeight, &config_oilify);
    operation_pagecurl(data, dest, b.bmWidth, b.bmHeight, &config_pagecurl);

    bm1.SetBitmapBits(n, dest);

    return 0;
}


void CChildView::OnFileOpen()
{
}
