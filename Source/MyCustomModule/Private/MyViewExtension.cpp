#include "../Public/MyViewExtension.h"
#include "../Public/KuwaharaFilter.h"

#include "PixelShaderUtils.h"
#include "PostProcess/PostProcessing.h"

// Yoinked from Engine\Plugins\Experimental\ColorCorrectRegions\Source\ColorCorrectRegions\Private\ColorCorrectRegionsSceneViewExtension.cpp
// This is how it appears in Unreal 5.0.3 - in UE4 it uses FVector2D instead of FVector2f but is otherwise identical
FScreenPassTextureViewportParameters GetTextureViewportParameters(const FScreenPassTextureViewport& InViewport) {
	const FVector2f Extent(InViewport.Extent);
	const FVector2f ViewportMin(InViewport.Rect.Min.X, InViewport.Rect.Min.Y);
	const FVector2f ViewportMax(InViewport.Rect.Max.X, InViewport.Rect.Max.Y);
	const FVector2f ViewportSize = ViewportMax - ViewportMin;

	FScreenPassTextureViewportParameters Parameters;

	if (!InViewport.IsEmpty()) {
		Parameters.Extent = FVector2f(Extent);
		Parameters.ExtentInverse = FVector2f(1.0f / Extent.X, 1.0f / Extent.Y);

		Parameters.ScreenPosToViewportScale = FVector2f(0.5f, -0.5f) * ViewportSize;
		Parameters.ScreenPosToViewportBias = (0.5f * ViewportSize) + ViewportMin;

		Parameters.ViewportMin = InViewport.Rect.Min;
		Parameters.ViewportMax = InViewport.Rect.Max;

		Parameters.ViewportSize = ViewportSize;
		Parameters.ViewportSizeInverse = FVector2f(1.0f / Parameters.ViewportSize.X, 1.0f / Parameters.ViewportSize.Y);

		Parameters.UVViewportMin = ViewportMin * Parameters.ExtentInverse;
		Parameters.UVViewportMax = ViewportMax * Parameters.ExtentInverse;

		Parameters.UVViewportSize = Parameters.UVViewportMax - Parameters.UVViewportMin;
		Parameters.UVViewportSizeInverse = FVector2f(1.0f / Parameters.UVViewportSize.X, 1.0f / Parameters.UVViewportSize.Y);

		Parameters.UVViewportBilinearMin = Parameters.UVViewportMin + 0.5f * Parameters.ExtentInverse;
		Parameters.UVViewportBilinearMax = Parameters.UVViewportMax - 0.5f * Parameters.ExtentInverse;
	}

	return Parameters;
}


FMyViewExtension::FMyViewExtension(const FAutoRegister& AutoRegister, FLinearColor CustomColor) : FSceneViewExtensionBase(AutoRegister) {
	HighlightColor = CustomColor;
}

void FMyViewExtension::PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessingInputs& Inputs) {
	if ((*Inputs.SceneTextures)->CustomDepthTexture->Desc.Format != PF_DepthStencil) return; // This check was only necessary in UE5 - see below

	checkSlow(View.bIsViewInfo); // can't do dynamic_cast because FViewInfo doesn't have any virtual functions.
	const FIntRect Viewport = static_cast<const FViewInfo&>(View).ViewRect;
	FScreenPassTexture SceneColor((*Inputs.SceneTextures)->SceneColorTexture, Viewport);
	FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

	RDG_EVENT_SCOPE(GraphBuilder, "My Render Pass");

	// Viewport parameters
	const FScreenPassTextureViewport SceneColorTextureViewport(SceneColor);
	const FScreenPassTextureViewportParameters SceneTextureViewportParams = GetTextureViewportParameters(SceneColorTextureViewport);

	// Render targets
	FScreenPassRenderTarget SceneColorCopyRenderTarget;
	SceneColorCopyRenderTarget.Texture = GraphBuilder.CreateTexture((*Inputs.SceneTextures)->SceneColorTexture->Desc, TEXT("Scene Color Copy"));
	FScreenPassRenderTarget UVMaskRenderTarget;
	UVMaskRenderTarget.Texture = GraphBuilder.CreateTexture((*Inputs.SceneTextures)->SceneColorTexture->Desc, TEXT("UV Mask"));

	// Shader setup
	TShaderMapRef<FUVMaskShaderPS> UVMaskPixelShader(GlobalShaderMap);
	FUVMaskShaderPS::FParameters* UVMaskParameters = GraphBuilder.AllocParameters<FUVMaskShaderPS::FParameters>();
	UVMaskParameters->SceneColor = (*Inputs.SceneTextures)->SceneColorTexture;
	UVMaskParameters->InputStencilTexture = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::CreateWithPixelFormat((*Inputs.SceneTextures)->CustomDepthTexture, PF_X24_G8));
	UVMaskParameters->InputSampler = TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	UVMaskParameters->ViewParams = SceneTextureViewportParams;
	UVMaskParameters->RenderTargets[0] = UVMaskRenderTarget.GetRenderTargetBinding();
	UVMaskParameters->RenderTargets[1] = SceneColorCopyRenderTarget.GetRenderTargetBinding();

	FPixelShaderUtils::AddFullscreenPass(
		GraphBuilder,
		GlobalShaderMap,
		FRDGEventName(TEXT("UV Mask")),
		UVMaskPixelShader,
		UVMaskParameters,
		Viewport);

	// Shader setup
	TShaderMapRef<FCombineShaderPS> CombinePixelShader(GlobalShaderMap);
	FCombineShaderPS::FParameters* CombineParameters = GraphBuilder.AllocParameters<FCombineShaderPS::FParameters>();
	CombineParameters->SceneColor = SceneColorCopyRenderTarget.Texture;
	CombineParameters->InputTexture = UVMaskRenderTarget.Texture;
	CombineParameters->InputSampler = TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	CombineParameters->Color = HighlightColor;
	CombineParameters->ViewParams = SceneTextureViewportParams;
	CombineParameters->RenderTargets[0] = FRenderTargetBinding(SceneColor.Texture, ERenderTargetLoadAction::ELoad);

	FPixelShaderUtils::AddFullscreenPass(
		GraphBuilder,
		GlobalShaderMap,
		FRDGEventName(TEXT("Combine")),
		CombinePixelShader,
		CombineParameters,
		Viewport);
}