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

#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/GL/TransformFeedback.h>

#include <utility>
#include <string>

#include "src/base/understory.h"

#include "utility/UpdateChecker.hpp"

#include "navigation/MouseMoveHelper.hpp"
#include "navigation/MapScaleHelper.hpp"
#include "navigation/KeyboardMoveHelper.hpp"
#include "navigation/ShortcutsTextHelper.hpp"

#include "navigation/base/states/MouseMovements.h"

#include "shaders/selectRect/SelectionRectState.h"
#include "shaders/grid/Grid.hpp"

#include "widgets/AtomSelectorButton.hpp"

namespace UnderStory {

class USApplication: public Magnum::Platform::Application {
 public:
    explicit USApplication(const Arguments& arguments);

 private:
    void viewportEvent(ViewportEvent& event) override;
    void drawEvent() override;
    void mouseScrollEvent(MouseScrollEvent& event) override;
    void keyPressEvent(KeyEvent& event) override;
    void keyReleaseEvent(KeyEvent& event) override;

    void mousePressEvent(MouseEvent& event) override;
    void mouseReleaseEvent(MouseEvent& event) override;
    void mouseMoveEvent(MouseMoveEvent& event) override;

    // left click
      bool _lMousePressed = false;
      bool _lMousePressedMoved = false;
      std::chrono::system_clock::time_point _lMousePressedT;

    // right click
      bool _rMousePressed = false;

    void _updateProjections();
    void _updateDebugText();
    void _resetWorldMatrix();

    Magnum::PluginManager::Manager<Magnum::Text::AbstractFont> _fontManager;
    Magnum::Containers::Pointer<Magnum::Text::AbstractFont> _font;

    Magnum::Text::DistanceFieldGlyphCache _cache;
    Magnum::GL::Mesh _worldText{Magnum::NoCreate};
    Magnum::GL::Buffer _textVertices, _textIndices;
    Magnum::Containers::Pointer<Magnum::Text::Renderer2D> _debugText, _shortcutsText;
    Magnum::Range2D _shortcutsTextRect;
    Magnum::Shaders::DistanceFieldVector2D _textShader;

    Magnum::Shaders::Flat2D _flatShader;
    Magnum::GL::Buffer _haulderBuffer;
    Magnum::GL::Mesh _haulder{Magnum::GL::MeshPrimitive::TriangleFan};
    void _defineHaulder();

    Magnum::GL::Texture2D _gridTexture;
    Shader::Grid _gridShader{Magnum::NoCreate};
    Magnum::GL::Mesh _grid{Magnum::GL::MeshPrimitive::Triangles};
    void _defineGrid(const Magnum::Utility::Resource &rs);

    Shader::SelectionRect _selectRectShader{Magnum::NoCreate};
    Magnum::GL::Buffer _selectRectBuffer;
    Magnum::GL::Mesh _selectRect{Magnum::GL::MeshPrimitive::Triangles};
    Navigation::SelectionRectState _srs;
    void _defineSelectionRect(const Magnum::Utility::Resource &rs);

    Widget::AtomSelectorButton _atomSelector{Magnum::NoCreate};
    Magnum::Vector2 _cursorPosition(const Sdl2Application::MouseMoveEvent& event) const;

    Magnum::Matrix3 _transformationWorld,
                    _projectionWorld,
                    _transformationProjectionShortcutsText,
                    _scaleMatrixShortcutsText,
                    _transformationProjectionDebugText;

    static constexpr int DOUBLE_CLICK_DELAY_MS = 200;
    static constexpr float MAP_SIZE = 1000.f;  // CAREFUL, higher need a better precision
    static constexpr Magnum::Color4 DEBUG_TEXT_COLOR {1.f, .7f, .3f};

    Magnum::Timeline _timeline;
    Navigation::MouseMoveHelper _mmh;
    Navigation::MapScaleHelper _msh;
    Navigation::KeyboardMoveHelper _kmh;
    Navigation::ShortcutsTextHelper _sth;

    Utility::UpdateChecker _updateChecker;
};

}  // namespace UnderStory
