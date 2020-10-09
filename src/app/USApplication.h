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

#include <utility>
#include <string>

#include "src/base/understory.h"

#include "widgets/UpdateCheckerWidget.hpp"

#include "navigation/MouseMoveHelper.hpp"
#include "navigation/MapScaleHelper.hpp"
#include "navigation/KeyboardMoveHelper.hpp"

#include "navigation/base/states/MouseMovements.h"

#include "navigation/shaders/selectRect/SelectionRectState.h"
#include "navigation/shaders/grid/Grid.hpp"

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
    Magnum::Shaders::DistanceFieldVector2D _textShader;

    Magnum::GL::Texture2D _gridTexture;
    Shader::Grid _gridShader{Magnum::NoCreate};
    Magnum::GL::Mesh _grid{Magnum::GL::MeshPrimitive::Triangles};
    void _defineGrid(Magnum::Utility::Resource &rs);

    Shader::SelectionRect _selectRectShader{Magnum::NoCreate};
    Magnum::GL::Buffer _selectRectBuffer;
    Magnum::GL::Mesh _selectRect{Magnum::GL::MeshPrimitive::Triangles};
    Navigation::SelectionRectState _srs;
    void _defineSelectionRect();

    Magnum::Matrix3 _transformationWorld,
                    _projectionWorld,
                    _transformationProjectionShortcutsText,
                    _transformationProjectionDebugText;

    static const int DOUBLE_CLICK_DELAY_MS = 200;

    Magnum::Timeline _timeline;
    Navigation::MouseMoveHelper _mmh;
    Navigation::MapScaleHelper _msh;
    Navigation::KeyboardMoveHelper _kmh;

    Widget::UpdateCheckerWidget _updateChecker;
};

}  // namespace UnderStory
