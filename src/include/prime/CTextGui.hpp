#ifndef PRIMEWATCH_PRIME_CTEXTGUI_HPP
#define PRIMEWATCH_PRIME_CTEXTGUI_HPP

#include "types.h"
#include <rstl/vector.h>
#include <rstl/string.h>
#include "prime/CStateManager.hpp"
#include <PrimeAPI.h>
#include "prime/CTextGui.hpp"
#include "prime/CMemoryCardSys.hpp"

class CRasterFont;
class CTextRenderBuffer;
class CTextExecuteBuffer;

typedef enum EJustification {
    EJustification_Left = 0,
    EJustification_Center,
    EJustification_Right,
    EJustification_Full,
    EJustification_NLeft,
    EJustification_NCenter,
    EJustification_NRight,
    EJustification_LeftMono,
    EJustification_CenterMono,
    EJustification_RightMono
} EJustification;

typedef enum EVerticalJustification {
    EVerticalJustification_Top = 0,
    EVerticalJustification_Center,
    EVerticalJustification_Bottom,
    EVerticalJustification_Full,
    EVerticalJustification_NTop,
    EVerticalJustification_NCenter,
    EVerticalJustification_NBottom,
    EVerticalJustification_TopMono,
    EVerticalJustification_CenterMono,
    EVerticalJustification_RightMono
} EVerticalJustification;

typedef enum ETextDirection {
    ETextDirection_Horizontal,
    ETextDirection_Vertical
} ETextDirection;

class CGuiTextProperties {
public:
    char spacer[0x10];
    CGuiTextProperties(bool wordWrap, bool horizontal, EJustification justification,
                       EVerticalJustification vertJustification,
                       const rstl::vector<CSaveWorld::SScanState> *scanStates);
};
class CGuiWidget {
public:
    typedef enum EGuiModelDrawFlags {
        EGuiModelDrawFlags_Shadeless = 0,
        EGuiModelDrawFlags_Opaque = 1,
        EGuiModelDrawFlags_Alpha = 2,
        EGuiModelDrawFlags_Additive = 3,
        EGuiModelDrawFlags_AlphaAdditiveOverdraw = 4
    } EGuiModelDrawFlags;
};


class CGuiTextSupport {
public:
//    char spacer1[0x10]; // rstl::string x0_string;
//    float x10_curTimeMod900;
//    CGuiTextProperties x14_props;
//    CColor x24_fontColor;
//    CColor x28_outlineColor;
//    CColor x2c_geometryColor;
//    bool x30_imageBaseline;
//    s32 x34_extentX;
//    s32 x38_extentY;
//    float x3c_curTime;
//    char spacer2[0x14]; // std::vector<std::pair<float, int>> x40_primStartTimes;
//    bool x50_typeEnable;
//    float x54_chFadeTime;
//    float x58_chRate;
//    ResId x5c_fontId;
//    CGuiWidget::EGuiModelDrawFlags m_drawFlags;
//    //std::experimental::optional <CTextRenderBuffer> x60_renderBuf;
//    //std::vector <CToken> x2bc_assets;
//    char spacer3[0x26C];
//    TLockedToken<CRasterFont> x2cc_font;
//    CToken x2cc_font;
    //std::pair <zeus::CVector2i, zeus::CVector2i> x2dc_oneBufBounds;

    //std::list <CTextRenderBuffer> x2ec_renderBufferPages;
    //int x304_pageCounter = 0;
    //bool x308_multipageFlag = false;

    //CTextRenderBuffer *GetCurrentPageRenderBuffer() const;
    //bool _GetIsTextSupportFinishedLoading() const;
public:
    ~CGuiTextSupport();
    CGuiTextSupport(ResId fontId,
                    const CGuiTextProperties &props,
                    const CColor &fontCol,
                    const CColor &outlineCol,
                    const CColor &geomCol,
                    int padX, int padY,
                    CSimplePool *store);
//    CGuiTextSupport(ResId fontId, const CGuiTextProperties &props,
//                    const zeus::CColor &fontCol, const zeus::CColor &outlineCol,
//                    const zeus::CColor &geomCol, s32 extX, s32 extY, CSimplePool *store,
//                    CGuiWidget::EGuiModelDrawFlags drawFlags);

    float GetCurrentAnimationOverAge() const;
    float GetNumCharsTotal() const;
    float GetNumCharsPrinted() const;
    float GetTotalAnimationTime() const;
    bool IsAnimationDone() const;
    void SetTypeWriteEffectOptions(bool enable, float chFadeTime, float chRate);
    void Update(float dt);
    void ClearRenderBuffer();
    void CheckAndRebuildTextBuffer();
    bool CheckAndRebuildRenderBuffer();
    //const std::pair<zeus::CVector2i, zeus::CVector2i>& GetBounds();
    void AutoSetExtent();
    void Render();
    void SetGeometryColor(const CColor &col);
    void SetOutlineColor(const CColor &col);
    void SetFontColor(const CColor &col);
//    void AddText(const rstl::wstring &str);
//		void SetText(const std::u16string& str, bool multipage=false);
    void SetText(const rstl::string &str);
    void SetJustification(EJustification j);
    void SetVerticalJustification(EVerticalJustification j);
    void SetImageBaseline(bool b);
    bool GetIsTextSupportFinishedLoading() const;
    float GetCurTime() const;
    void SetCurTime(float t);
    //const std::u16string& GetString();
//		void SetScanStates(const std::vector<CSaveWorld::SScanState>* scanStates);
    int GetPageCounter() const;
    int GetTotalPageCount();
    void SetPage(int page);
};

#endif //PRIMEWATCH_PRIME_CTEXTGUI_HPP
