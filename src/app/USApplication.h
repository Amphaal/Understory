// UnderStory
// An intuitive Pen & Paper experience
// Copyright (C) 2020-2021 Guillaume Vara <guillaume.vara@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Any graphical resources available within the source code may
// use a different license and copyright : please refer to their metadata
// for further details. Graphical resources without explicit references to a
// different license and copyright still refer to this GPL.

#pragma once

#include <spdlog/spdlog.h>

#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/Resource.h>

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/Text/AbstractFont.h>
#include <Magnum/Text/DistanceFieldGlyphCache.h>
#include <Magnum/Text/Renderer.h>

#include <Magnum/Trade/MeshData.h>
#include <Magnum/ImageView.h>

#include <Magnum/Math/Complex.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Color.h>

#include <Magnum/ImGuiIntegration/Context.hpp>

#include <utility>
#include <string>

#include "src/base/understory.h"

#include "utility/UpdateChecker.hpp"

#include "widgets/navigation/MouseMoveHelper.hpp"
#include "widgets/navigation/MapScaleHelper.hpp"
#include "widgets/navigation/KeyboardMoveHelper.hpp"

#include "widgets/atomSelector/AtomSelectorButton.hpp"
#include "widgets/scroll/ScrollablePanel.h"
#include "widgets/atomSelector/AtomSelectorGrid.hpp"

#include "widgets/helpers/SelectionRectangle.hpp"
#include "widgets/helpers/Grid.hpp"
#include "widgets/helpers/Enlighter.hpp"

#include "widgets/text/ShortcutsText.hpp"
#include "widgets/text/StaticText.hpp"

#include "src/app/utility/AppBound.hpp"
#include "src/app/widgets/animation/TimelineBound.h"
#include "src/app/widgets/base/AppContainer.hpp"

#include "src/app/shaders/Shaders.hpp"

namespace UnderStory {

class USApplication : public Magnum::Platform::Application, public Widget::AppContainer {
 public:
    explicit USApplication(const Arguments& arguments);
    ~USApplication();

 private:
    void drawEvent() final;
    void viewportEvent(ViewportEvent& event) final;

    void keyPressEvent(KeyEvent& event) final;
    void keyReleaseEvent(KeyEvent& event) final;

    void mouseScrollEvent(MouseScrollEvent& event) final;

    void mousePressEvent(MouseEvent& event) final;
    void mouseReleaseEvent(MouseEvent& event) final;
    void mouseMoveEvent(MouseMoveEvent& event) final;

    void _updateDebugText();
    void _resetWorldMatrix();

    void handleScrollEvent(MouseScrollEvent &event) final;
    void handlePressEvent(MouseEvent& event) final;
    void handleLockReleaseEvent(MouseEvent& event) final;
    void handleLockMoveEvent(MouseMoveEvent& event) final;
    void textInputEvent(TextInputEvent& event) final;
    
    #ifdef _DEBUG
      Magnum::ImGuiIntegration::Context _imgui{Magnum::NoCreate};
      void _drawImGui();
    #endif

    Magnum::PluginManager::Manager<Magnum::Text::AbstractFont> _fontManager;
    Magnum::Containers::Pointer<Magnum::Text::AbstractFont> _defaultFont;
    Magnum::Text::DistanceFieldGlyphCache _worldCache;

    Widget::StaticText _worldText;

    Magnum::Containers::Pointer<Magnum::Text::Renderer2D> _debugText;

    Magnum::Vector2 _cursorPosition(const Sdl2Application::MouseMoveEvent& event) const;

    Magnum::Matrix3 _transformationWorld,
                    _projectionWorld,
                    _transformationProjectionDebugText;

    static constexpr float MAP_SIZE = 1000.f;  // CAREFUL, higher need a better precision
    static constexpr Magnum::Color4 DEBUG_TEXT_COLOR {1.f, .7f, .3f};
    static constexpr int MINIMUM_HEIGHT = 600;
    static constexpr int MINIMUM_WIDTH = 800;

    Magnum::Utility::Resource _rs;
    Magnum::Timeline _timeline;

    Navigation::MouseMoveHelper _mmh;
    Navigation::MapScaleHelper _msh;
    Navigation::KeyboardMoveHelper _kmh;

    Magnum::Shaders::DistanceFieldVector2D _distanceField;
    Magnum::Shaders::Flat2D _flat;
    Magnum::Shaders::VertexColor2D _color;
    Shader::Rounded _rounded{Magnum::NoCreate};

    Widget::Helper::SelectionRectangle _selectionRect;
    Widget::Helper::Grid _grid;
    Widget::Helper::Enlighter _enlighter;

    Magnum::Containers::Pointer<Widget::ShortcutsText> _stWidget;
    Widget::AtomSelectorButton _asButton;
    Widget::ScrollablePanel _asPanel;
    Widget::AtomSelectorGrid _asGrid;

    Utility::UpdateChecker _updateChecker;
    
    void _initSentry();
};

}  // namespace UnderStory
