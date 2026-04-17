/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>

#include "draw/canvas.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_director.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"
#include "common/rs_vector2.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"

using namespace OHOS;
using namespace OHOS::Rosen;

constexpr uint64_t SCREEN_ID = 0;

namespace {
// Window configuration
constexpr int WINDOW_OFFSET_Y = 200;
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 2560;

// Node dimensions
constexpr int NODE_WIDTH = 400;
constexpr int NODE_HEIGHT = 400;

// Node positions
constexpr int NODE1_POS_X = 200;
constexpr int NODE1_POS_Y = 200;
constexpr int NODE2_POS_X = 700;
constexpr int NODE2_POS_Y = 200;
constexpr int NODE3_POS_X = 200;
constexpr int NODE3_POS_Y = 700;
constexpr int NODE4_POS_X = 700;
constexpr int NODE4_POS_Y = 700;

// Light parameters
constexpr float DEFAULT_LIGHT_INTENSITY = 1.0f;
constexpr float DEFAULT_LIGHT_ATTENUATION = 0.5f;
constexpr float LIGHT_DEPTH = 50.0f;

// Light positions (centered in each node)
constexpr float LIGHT_CENTER_OFFSET_X = 200.0f;
constexpr float LIGHT_CENTER_OFFSET_Y = 200.0f;

// RippleMask parameters
constexpr float RIPPLE_MASK_RADIUS = 5.0f;
constexpr float RIPPLE_MASK_WIDTH = 5.0f;
constexpr float RIPPLE_MASK_CENTER_X = 0.5f;
constexpr float RIPPLE_MASK_CENTER_Y = 0.5f;
constexpr float RIPPLE_MASK_OFFSET = 0.0f;

// RadialGradientMask parameters
constexpr float RADIAL_MASK_CENTER_X = 0.5f;
constexpr float RADIAL_MASK_CENTER_Y = 0.5f;
constexpr float RADIAL_MASK_RADIUS_X = 0.3f;
constexpr float RADIAL_MASK_RADIUS_Y = 0.3f;
constexpr float RADIAL_MASK_COLOR_START = 1.0f;
constexpr float RADIAL_MASK_COLOR_END = 0.0f;
constexpr float RADIAL_MASK_POSITION_START = 0.0f;
constexpr float RADIAL_MASK_POSITION_END = 1.0f;

// WaveGradientMask parameters
constexpr float WAVE_MASK_CENTER_X = 0.5f;
constexpr float WAVE_MASK_CENTER_Y = 0.5f;
constexpr float WAVE_MASK_WIDTH = 0.2f;
constexpr float WAVE_MASK_PROPAGATION_RADIUS = 0.5f;
constexpr float WAVE_MASK_BLUR_RADIUS = 0.1f;
constexpr float WAVE_MASK_TURBULENCE = 0.5f;

// DoubleRippleMask parameters
constexpr float DOUBLE_RIPPLE_MASK_RADIUS = 5.0f;
constexpr float DOUBLE_RIPPLE_MASK_WIDTH = 5.0f;
constexpr float DOUBLE_RIPPLE_MASK_CENTER1_X = 0.3f;
constexpr float DOUBLE_RIPPLE_MASK_CENTER1_Y = 0.3f;
constexpr float DOUBLE_RIPPLE_MASK_CENTER2_X = 0.7f;
constexpr float DOUBLE_RIPPLE_MASK_CENTER2_Y = 0.7f;
constexpr float DOUBLE_RIPPLE_MASK_TURBULENCE = 0.5f;
constexpr float DOUBLE_RIPPLE_MASK_HALO_THICKNESS = 2.0f;

// Animation parameters
constexpr int ANIMATION_FRAME_COUNT = 60;
constexpr int ANIMATION_INTERVAL_US = 50000;
constexpr float ANIMATION_PHASE_STEP = 0.05f;
constexpr float ANIMATION_INTENSITY_BASE = 0.5f;
constexpr float ANIMATION_INTENSITY_RANGE = 0.5f;

// Timing
constexpr float WAIT_TIME_SHORT = 1.0f;
constexpr float WAIT_TIME_LONG = 2.0f;

// Colors
constexpr uint32_t COLOR_BLACK = 0xFF000000;
constexpr uint32_t COLOR_GRAY = 0xFF808080;
constexpr float COLOR_WHITE_R = 1.0f;
constexpr float COLOR_WHITE_G = 1.0f;
constexpr float COLOR_WHITE_B = 1.0f;
constexpr float COLOR_WHITE_A = 1.0f;

const std::string SURFACE_NODE_NAME = "spatial_point_light_demo";

std::shared_ptr<RSNGSpatialPointLight> CreateSpatialPointLightShader(
    const Vector3f& lightPos, const Vector4f& lightColor, float intensity, float attenuation)
{
    auto shader = std::make_shared<RSNGSpatialPointLight>();
    shader->Setter<SpatialPointLightLightIntensityTag>(intensity);
    shader->Setter<SpatialPointLightLightPositionTag>(lightPos);
    shader->Setter<SpatialPointLightLightColorTag>(lightColor);
    shader->Setter<SpatialPointLightAttenuationTag>(attenuation);
    return shader;
}

std::shared_ptr<RSNGRippleMask> CreateRippleMask()
{
    auto mask = std::make_shared<RSNGRippleMask>();
    mask->Setter<RippleMaskCenterTag>(Vector2f(RIPPLE_MASK_CENTER_X, RIPPLE_MASK_CENTER_Y));
    mask->Setter<RippleMaskRadiusTag>(RIPPLE_MASK_RADIUS);
    mask->Setter<RippleMaskWidthTag>(RIPPLE_MASK_WIDTH);
    mask->Setter<RippleMaskOffsetTag>(RIPPLE_MASK_OFFSET);
    return mask;
}

std::shared_ptr<RSNGRadialGradientMask> CreateRadialGradientMask()
{
    auto mask = std::make_shared<RSNGRadialGradientMask>();
    mask->Setter<RadialGradientMaskCenterTag>(Vector2f(RADIAL_MASK_CENTER_X, RADIAL_MASK_CENTER_Y));
    mask->Setter<RadialGradientMaskRadiusXTag>(RADIAL_MASK_RADIUS_X);
    mask->Setter<RadialGradientMaskRadiusYTag>(RADIAL_MASK_RADIUS_Y);
    mask->Setter<RadialGradientMaskColorsTag>(
        std::vector<float>{RADIAL_MASK_COLOR_START, RADIAL_MASK_COLOR_END});
    mask->Setter<RadialGradientMaskPositionsTag>(
        std::vector<float>{RADIAL_MASK_POSITION_START, RADIAL_MASK_POSITION_END});
    return mask;
}

std::shared_ptr<RSNGWaveGradientMask> CreateWaveGradientMask()
{
    auto mask = std::make_shared<RSNGWaveGradientMask>();
    mask->Setter<WaveGradientMaskWaveCenterTag>(Vector2f(WAVE_MASK_CENTER_X, WAVE_MASK_CENTER_Y));
    mask->Setter<WaveGradientMaskWaveWidthTag>(WAVE_MASK_WIDTH);
    mask->Setter<WaveGradientMaskPropagationRadiusTag>(WAVE_MASK_PROPAGATION_RADIUS);
    mask->Setter<WaveGradientMaskBlurRadiusTag>(WAVE_MASK_BLUR_RADIUS);
    mask->Setter<WaveGradientMaskTurbulenceStrengthTag>(WAVE_MASK_TURBULENCE);
    return mask;
}

std::shared_ptr<RSNGDoubleRippleMask> CreateDoubleRippleMask()
{
    auto mask = std::make_shared<RSNGDoubleRippleMask>();
    mask->Setter<DoubleRippleMaskCenter1Tag>(
        Vector2f(DOUBLE_RIPPLE_MASK_CENTER1_X, DOUBLE_RIPPLE_MASK_CENTER1_Y));
    mask->Setter<DoubleRippleMaskCenter2Tag>(
        Vector2f(DOUBLE_RIPPLE_MASK_CENTER2_X, DOUBLE_RIPPLE_MASK_CENTER2_Y));
    mask->Setter<DoubleRippleMaskRadiusTag>(DOUBLE_RIPPLE_MASK_RADIUS);
    mask->Setter<DoubleRippleMaskWidthTag>(DOUBLE_RIPPLE_MASK_WIDTH);
    mask->Setter<DoubleRippleMaskTurbulenceTag>(DOUBLE_RIPPLE_MASK_TURBULENCE);
    mask->Setter<DoubleRippleMaskHaloThicknessTag>(DOUBLE_RIPPLE_MASK_HALO_THICKNESS);
    return mask;
}

class SpatialPointLightDemo {
public:
    SpatialPointLightDemo()
    {
        rsUiDirector_ = RSUIDirector::Create();
        if (rsUiDirector_ == nullptr) {
            std::cout << "Failed to create ui director!" << std::endl;
            return;
        }
        rsUiDirector_->Init(true, true);
        uiContext_ = rsUiDirector_->GetRSUIContext();
        if (uiContext_ == nullptr) {
            std::cout << "ui context is nullptr!" << std::endl;
            return;
        }

        if (!InitSurfaceNode()) {
            std::cout << "Failed to init surface node!" << std::endl;
            return;
        }

        if (!InitRootNode()) {
            std::cout << "Failed to init root node!" << std::endl;
            return;
        }

        if (!InitCanvasNodes()) {
            std::cout << "Failed to init canvas nodes!" << std::endl;
            return;
        }

        transaction_ = uiContext_->GetRSTransaction();
        if (transaction_ == nullptr) {
            std::cout << "rs transaction is nullptr" << std::endl;
            return;
        }

        transaction_->FlushImplicitTransaction();
        std::cout << "SpatialPointLight demo init success" << std::endl;
        initSuccess_ = true;
    }

    bool InitSurfaceNode()
    {
        if (rsUiDirector_ == nullptr) {
            return false;
        }
        RSSurfaceNodeConfig surfaceNodeConfig;
        surfaceNodeConfig.SurfaceNodeName = SURFACE_NODE_NAME;
        RSSurfaceNodeType surfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
        surfaceNode_ = RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType, true, false, uiContext_);
        if (!surfaceNode_) {
            std::cout << "Failed to create surface node!" << std::endl;
            return false;
        }
        surfaceNode_->SetFrameGravity(Gravity::RESIZE_ASPECT_FILL);
        surfaceNode_->SetBounds(0, WINDOW_OFFSET_Y, WINDOW_WIDTH, WINDOW_HEIGHT);
        surfaceNode_->SetFrame(0, WINDOW_OFFSET_Y, WINDOW_WIDTH, WINDOW_HEIGHT);
        surfaceNode_->SetPositionZ(RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
        surfaceNode_->SetBackgroundColor(SK_ColorWHITE);
        surfaceNode_->AttachToDisplay(SCREEN_ID);
        rsUiDirector_->SetRSSurfaceNode(surfaceNode_);
        return true;
    }

    bool InitRootNode()
    {
        if (rsUiDirector_ == nullptr) {
            return false;
        }
        rootNode_ = RSRootNode::Create(false, false, uiContext_);
        if (rootNode_ == nullptr) {
            std::cout << "Failed to create root node!" << std::endl;
            return false;
        }
        rootNode_->SetBounds(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        rootNode_->SetFrame(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        rootNode_->SetBackgroundColor(COLOR_BLACK);
        rsUiDirector_->SetRSRootNode(rootNode_->ReinterpretCastTo<RSRootNode>());
        return true;
    }

    bool InitCanvasNodes()
    {
        if (rootNode_ == nullptr) {
            return false;
        }

        // Node 1: SpatialPointLight with RippleMask
        auto node1 = CreateCanvasNode(NODE1_POS_X, NODE1_POS_Y, NODE_WIDTH, NODE_HEIGHT);
        auto shader1 = CreateSpatialPointLightShader(
            Vector3f(NODE1_POS_X + LIGHT_CENTER_OFFSET_X, NODE1_POS_Y + LIGHT_CENTER_OFFSET_Y, LIGHT_DEPTH),
            Vector4f(COLOR_WHITE_R, COLOR_WHITE_G, COLOR_WHITE_B, COLOR_WHITE_A),
            DEFAULT_LIGHT_INTENSITY,
            DEFAULT_LIGHT_ATTENUATION);
        auto mask1 = std::static_pointer_cast<RSNGMaskBase>(CreateRippleMask());
        shader1->Setter<SpatialPointLightMaskTag>(mask1);
        node1->SetBackgroundNGShader(shader1);
        canvasNodes_.push_back(node1);
        rootNode_->AddChild(node1, -1);

        // Node 2: SpatialPointLight with RadialGradientMask
        auto node2 = CreateCanvasNode(NODE2_POS_X, NODE2_POS_Y, NODE_WIDTH, NODE_HEIGHT);
        auto shader2 = CreateSpatialPointLightShader(
            Vector3f(NODE2_POS_X + LIGHT_CENTER_OFFSET_X, NODE2_POS_Y + LIGHT_CENTER_OFFSET_Y, LIGHT_DEPTH),
            Vector4f(COLOR_WHITE_R, COLOR_WHITE_G, COLOR_WHITE_B, COLOR_WHITE_A),
            DEFAULT_LIGHT_INTENSITY,
            DEFAULT_LIGHT_ATTENUATION);
        auto mask2 = std::static_pointer_cast<RSNGMaskBase>(CreateRadialGradientMask());
        shader2->Setter<SpatialPointLightMaskTag>(mask2);
        node2->SetBackgroundNGShader(shader2);
        canvasNodes_.push_back(node2);
        rootNode_->AddChild(node2, -1);

        // Node 3: SpatialPointLight with WaveGradientMask
        auto node3 = CreateCanvasNode(NODE3_POS_X, NODE3_POS_Y, NODE_WIDTH, NODE_HEIGHT);
        auto shader3 = CreateSpatialPointLightShader(
            Vector3f(NODE3_POS_X + LIGHT_CENTER_OFFSET_X, NODE3_POS_Y + LIGHT_CENTER_OFFSET_Y, LIGHT_DEPTH),
            Vector4f(COLOR_WHITE_R, COLOR_WHITE_G, COLOR_WHITE_B, COLOR_WHITE_A),
            DEFAULT_LIGHT_INTENSITY,
            DEFAULT_LIGHT_ATTENUATION);
        auto mask3 = std::static_pointer_cast<RSNGMaskBase>(CreateWaveGradientMask());
        shader3->Setter<SpatialPointLightMaskTag>(mask3);
        node3->SetBackgroundNGShader(shader3);
        canvasNodes_.push_back(node3);
        rootNode_->AddChild(node3, -1);

        // Node 4: SpatialPointLight with DoubleRippleMask
        auto node4 = CreateCanvasNode(NODE4_POS_X, NODE4_POS_Y, NODE_WIDTH, NODE_HEIGHT);
        auto shader4 = CreateSpatialPointLightShader(
            Vector3f(NODE4_POS_X + LIGHT_CENTER_OFFSET_X, NODE4_POS_Y + LIGHT_CENTER_OFFSET_Y, LIGHT_DEPTH),
            Vector4f(COLOR_WHITE_R, COLOR_WHITE_G, COLOR_WHITE_B, COLOR_WHITE_A),
            DEFAULT_LIGHT_INTENSITY,
            DEFAULT_LIGHT_ATTENUATION);
        auto mask4 = std::static_pointer_cast<RSNGMaskBase>(CreateDoubleRippleMask());
        shader4->Setter<SpatialPointLightMaskTag>(mask4);
        node4->SetBackgroundNGShader(shader4);
        canvasNodes_.push_back(node4);
        rootNode_->AddChild(node4, -1);

        return true;
    }

    RSCanvasNode::SharedPtr CreateCanvasNode(int x, int y, int width, int height)
    {
        auto node = RSCanvasNode::Create(false, false, uiContext_);
        if (node == nullptr) {
            return nullptr;
        }
        node->SetBounds(x, y, width, height);
        node->SetFrame(x, y, width, height);
        node->SetBackgroundColor(COLOR_GRAY);
        return node;
    }

    ~SpatialPointLightDemo() noexcept
    {
        if (initSuccess_) {
            surfaceNode_->DetachToDisplay(SCREEN_ID);
            transaction_->FlushImplicitTransaction();
        }
    }

    void TestSpatialPointLight()
    {
        if (!initSuccess_) {
            std::cout << "SpatialPointLight demo init failed, can't run" << std::endl;
            return;
        }
        transaction_->FlushImplicitTransaction();
        rsUiDirector_->SendMessages();
        rsUiDirector_->FlushModifier();
        std::cout << "test spatial point light with masks success" << std::endl;
        sleep(WAIT_TIME_LONG);
    }

    void RunIntensityAnimation()
    {
        if (!initSuccess_ || canvasNodes_.empty()) {
            std::cout << "SpatialPointLight demo init failed, can't run animation" << std::endl;
            return;
        }

        std::cout << "Running intensity animation..." << std::endl;
        float phase = 0.0f;
        for (int frame = 0; frame < ANIMATION_FRAME_COUNT; frame++) {
            phase += ANIMATION_PHASE_STEP;
            float intensity = ANIMATION_INTENSITY_BASE + ANIMATION_INTENSITY_RANGE * sin(phase);

            auto shader = CreateSpatialPointLightShader(
                Vector3f(NODE1_POS_X + LIGHT_CENTER_OFFSET_X, NODE1_POS_Y + LIGHT_CENTER_OFFSET_Y, LIGHT_DEPTH),
                Vector4f(COLOR_WHITE_R, COLOR_WHITE_G, COLOR_WHITE_B, COLOR_WHITE_A),
                intensity,
                DEFAULT_LIGHT_ATTENUATION);
            auto mask = std::static_pointer_cast<RSNGMaskBase>(CreateRippleMask());
            shader->Setter<SpatialPointLightMaskTag>(mask);
            canvasNodes_[0]->SetBackgroundNGShader(shader);

            transaction_->FlushImplicitTransaction();
            rsUiDirector_->SendMessages();
            usleep(ANIMATION_INTERVAL_US);
        }
        std::cout << "Animation completed" << std::endl;
        sleep(WAIT_TIME_SHORT);
    }

    void Run()
    {
        TestSpatialPointLight();
        RunIntensityAnimation();
    }

private:
    std::shared_ptr<RSUIDirector> rsUiDirector_;
    std::shared_ptr<RSUIContext> uiContext_;
    std::shared_ptr<RSTransactionHandler> transaction_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_;
    std::shared_ptr<RSNode> rootNode_;
    std::vector<RSCanvasNode::SharedPtr> canvasNodes_;
    bool initSuccess_ = false;
};
} // namespace

int main()
{
    {
        SpatialPointLightDemo demo;
        demo.Run();
    }
    return 0;
}