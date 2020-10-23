// UnderStory
// An intuitive Pen & Paper experience
// Copyright (C) 2020 Guillaume Vara

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

#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/Resource.h>

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Shaders/DistanceFieldVector.h>
#include <Magnum/Text/AbstractFont.h>
#include <Magnum/Text/DistanceFieldGlyphCache.h>
#include <Magnum/Text/Renderer.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Timeline.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/ImageView.h>

#include <Magnum/Math/Complex.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Color.h>

#include <utility>
#include <string>

#include "src/base/understory.h"

#include "utility/UpdateChecker.hpp"

#include "widgets/navigation/MouseMoveHelper.hpp"
#include "widgets/navigation/MapScaleHelper.hpp"
#include "widgets/navigation/KeyboardMoveHelper.hpp"
#include "widgets/navigation/MouseState.hpp"

#include "widgets/AtomSelectorButton.hpp"
#include "widgets/Panel.hpp"

#include "widgets/helpers/SelectionRectangle.hpp"
#include "widgets/helpers/Grid.hpp"
#include "widgets/helpers/Enlighter.hpp"

#include "widgets/text/ShortcutsText.hpp"
#include "widgets/text/StaticText.hpp"

namespace UnderStory {

class USApplication : public Magnum::Platform::Application {
 public:
    explicit USApplication(const Arguments& arguments);

 private:
    void drawEvent() override;
    void viewportEvent(ViewportEvent& event) override;
    void mouseScrollEvent(MouseScrollEvent& event) override;
    void keyPressEvent(KeyEvent& event) override;
    void keyReleaseEvent(KeyEvent& event) override;

    void mousePressEvent(MouseEvent& event) override;
    void mouseReleaseEvent(MouseEvent& event) override;
    void mouseMoveEvent(MouseMoveEvent& event) override;
    void leftMouseDoubleClickEvent(MouseEvent& event);

    void _onViewportChange();
    void _updateDebugText();
    void _resetWorldMatrix();

    Magnum::PluginManager::Manager<Magnum::Text::AbstractFont> _fontManager;
    Magnum::Containers::Pointer<Magnum::Text::AbstractFont> _defaultFont;
    Magnum::Text::DistanceFieldGlyphCache _worldCache;

    Widget::StaticText _worldText;

    Magnum::Containers::Pointer<Magnum::Text::Renderer2D> _debugText;
    Magnum::Shaders::DistanceFieldVector2D _textShader;

    Magnum::Shaders::Flat2D _flatShader;

    Magnum::Vector2 _cursorPosition(const Sdl2Application::MouseMoveEvent& event) const;

    Magnum::Matrix3 _transformationWorld,
                    _projectionWorld,
                    _transformationProjectionDebugText;

    static constexpr int DOUBLE_CLICK_DELAY_MS = 200;
    static constexpr float MAP_SIZE = 1000.f;  // CAREFUL, higher need a better precision
    static constexpr Magnum::Color4 DEBUG_TEXT_COLOR {1.f, .7f, .3f};
    static constexpr int MINIMUM_HEIGHT = 600;
    static constexpr int MINIMUM_WIDTH = 800;

    // mouse state
    Navigation::MouseState _mouseState {DOUBLE_CLICK_DELAY_MS};

    // mouse context
    void* _hoverContext = nullptr;
    void* _lockContext = nullptr;
    void _updateHoverContext(MouseMoveEvent& event);

    Magnum::Utility::Resource _rs;
    Magnum::Timeline _timeline;

    Navigation::MouseMoveHelper _mmh;
    Navigation::MapScaleHelper _msh;
    Navigation::KeyboardMoveHelper _kmh;

    Widget::Helper::SelectionRectangle _selectionRect;
    Widget::Helper::Grid _grid;
    Widget::Helper::Enlighter _enlighter;


    Magnum::Containers::Pointer<Widget::ShortcutsText> _stWidget;
    Widget::AtomSelectorButton _atomSelector;
    Widget::Panel _panel;

    Utility::UpdateChecker _updateChecker;
};

}  // namespace UnderStory
