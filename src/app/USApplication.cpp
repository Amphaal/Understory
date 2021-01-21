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

#include "USApplication.h"

#include "src/base/translate.h"

#include "src/base/Context.hpp"
#include <sentry.h>

using namespace Magnum::Math::Literals;

UnderStory::USApplication::USApplication(const Arguments& arguments): Magnum::Platform::Application{
        arguments,
        Configuration{}.setTitle(APP_FULL_DENOM)
                       .setSize(Magnum::Vector2i{MINIMUM_WIDTH, MINIMUM_HEIGHT})
                       .setWindowFlags(Configuration::WindowFlag::Resizable),
        GLConfiguration{}
    },
    _worldCache{Magnum::Vector2i{2048}, Magnum::Vector2i{512}, 22},
    _rs("data"),
    _mmh(&_transformationWorld),
    _msh(&_transformationWorld),
    _kmh(&_transformationWorld),
    _selectionRect(_rs),
    _grid(_rs, MAP_SIZE, MINIMUM_HEIGHT),
    _asPanel(),
    _asGrid(&_asPanel),
    _rounded(_rs) {
    // set minimum size
    SDL_SetWindowMinimumSize(this->window(), MINIMUM_WIDTH, MINIMUM_HEIGHT);

    #ifdef _DEBUG
        // IMGUI initial setup
        _imgui = Magnum::ImGuiIntegration::Context(
            Magnum::Vector2{windowSize()} / dpiScaling(),
            windowSize(),
            framebufferSize()
            );
    #endif

    // i18n
    bindtextdomain(APP_NAME, APP_TRANSLATION_FOLDER);
    /* Only write the following 2 lines if creating an executable */
    setlocale(LC_ALL, ""); /* Set locale based on ENV */
    textdomain(APP_NAME);

    //
    Shaders::distanceField = &_distanceField;
    Shaders::flat = &_flat;
    Shaders::color = &_color;
    Shaders::rounded = &_rounded;

    //
    AppBound::setupApp(this);
    Animation::TimelineBound::setupTimeline(&_timeline);

    //
    _initSentry();

    //
    _updateChecker.start();
    // TODO(amphaal) loop every 3-5 sec until 1min for updates
    

    // define animation mutual exclusions
    _kmh.setExcludedWhenPlaying({&_mmh});
    _mmh.setExcludedWhenPlaying({&_kmh});

    // activate VSync
    setSwapInterval(-1);

    /* Load a TrueTypeFont plugin and open the font */
    _defaultFont = _fontManager.loadAndInstantiate("TrueTypeFont");
    if(!_defaultFont || !_defaultFont->openData(_rs.getRaw("SourceSansPro-Regular.ttf"), 180.f))
        Magnum::Fatal{} << "Cannot open font file";

    /* Glyphs we need to render everything */
    _defaultFont->fillGlyphCache(
        _worldCache,
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789:-+,.!°'()/[] "
        "éèàêâ" // french characters
    );

    // define world text as static
    {
        // text
        auto welcomeText = _(
            "Welcome to the Understory developper early demo !\n"
            "This is a placeholder meant to showcase the text displaying capabilities.\n"
            "Thank you for trying this early stage build and supporting our efforts !\n"
            "More infos available on our Discord server and Github page (https://github.com/Amphaal/Understory)"
        );

        // widget
        Widget::StaticTextFactory textFactory(*_defaultFont, _worldCache, .2f);
        _worldText = textFactory.generate(welcomeText);
    }

    // define shortcut text as static
    {        
        // text
        std::string paddedText;
        paddedText += _("[LeftClick (DoubleClick)] Center screen on cursor\n");
        paddedText += _("[LeftClick (Maintain)] Move camera\n");
        paddedText += _("[RightClick (Maintain / Release)] Snapshot zoom-in\n");
        paddedText += _("[D-Pad] Move camera\n");
        paddedText += _("[Esc] Reset camera\n");
        paddedText += _("[MouseScroll] Zoom\n");
        paddedText += _("[NumPad +/-] Zoom");

        // widget
        Widget::StaticTextFactory textFactory(*_defaultFont, _worldCache, 14.f);
        auto text = textFactory.generate(paddedText, Magnum::Text::Alignment::TopRight);
        _stWidget.reset(new Widget::ShortcutsText(std::move(text)));
    }

    // default renderer behavior
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::ScissorTest);
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);
    Magnum::GL::Renderer::setBlendFunction(Magnum::GL::Renderer::BlendFunction::SourceAlpha, Magnum::GL::Renderer::BlendFunction::OneMinusSourceAlpha);
    Magnum::GL::Renderer::setBlendEquation(Magnum::GL::Renderer::BlendEquation::Add, Magnum::GL::Renderer::BlendEquation::Add);

    // define debug text as dynamic
    _debugText.reset(new Magnum::Text::Renderer2D(*_defaultFont, _worldCache, 14.f, Magnum::Text::Alignment::TopLeft));
    _debugText->reserve(100, Magnum::GL::BufferUsage::DynamicDraw, Magnum::GL::BufferUsage::StaticDraw);
    _updateDebugText();

    // bind widgets to container (order is important)
    this->_initContaining({&_asButton, &_asPanel, _stWidget.get()}),

    // button callback
    _asButton.setToogleCallback([this](bool) {
        _asPanel.toggle();
    });

    //
    _timeline.start();

    //
    spdlog::info("APP OK !");
}

void UnderStory::USApplication::viewportEvent(ViewportEvent& event) {
    // set viewport
    Magnum::GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

    #ifdef _DEBUG
        // IMGUI
        _imgui.relayout(
            Magnum::Vector2{event.windowSize()}/event.dpiScaling(),
            event.windowSize(),
            event.framebufferSize()
        );
    #endif

    // update constraints
    Shape::setConstraints(windowSize());

    // update shaders
    Shaders::rounded->setResolution(constraints().ws());

    // reset selection for SelectionRect
    _selectionRect.resetSelection();

    // update main project
    _projectionWorld = Magnum::Matrix3::projection(
        Magnum::Vector2::xScale(constraints().ws().aspectRatio())
    );

    // for the debug text, stick to top left corner + 5 pixels padding
    _transformationProjectionDebugText = constraints().pixelProjMatrix() *
        Magnum::Matrix3::translation(
            constraints().ws() *
            (Magnum::Vector2 {-.5f, .5f} - constraints().pixelSize() * 5 * Magnum::Vector2{-1.f, 1.f})
        );

    // assign an allowed space
    auto shape = this->shape();
    AppContainer::onViewportChange(shape);
}

#ifdef _DEBUG
void UnderStory::USApplication::_drawImGui() {
    /* Enable text input, if needed */
    if(ImGui::GetIO().WantTextInput && !isTextInputActive())
        startTextInput();
    else if(!ImGui::GetIO().WantTextInput && isTextInputActive())
        stopTextInput();

    //
    _imgui.newFrame();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
            1000.0/Magnum::Double(ImGui::GetIO().Framerate), Magnum::Double(ImGui::GetIO().Framerate));
    _imgui.drawFrame();
}
#endif

void UnderStory::USApplication::drawEvent() {
    //
    _updateChecker.poll();

    // clear
    Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color);

    // advance animations
    Animation::TimelineBound::advance();

    // world
    auto worldMatrix = _projectionWorld * _transformationWorld;
    auto scale = _transformationWorld.uniformScaling();
    Shaders::distanceField->bindVectorTexture(_worldCache.texture());
    Shaders::distanceField
        ->setTransformationProjectionMatrix(worldMatrix)
        .setColor(0x2f83cc_rgbf)
        .setOutlineColor(0xdcdcdc_rgbf)
        .setOutlineRange(.45f, .35f)
        .setSmoothness(.015f/ scale)
        .draw(_worldText);

    // grid
    _grid.draw(worldMatrix, scale);

    // selection rect
    _selectionRect.mayDraw(_projectionWorld);

    // atom selector panel
    _asPanel.mayDraw();

    // atom selector button
    _asButton.draw();

    // debug text
    _updateDebugText();
    Shaders::distanceField
        ->setColor(DEBUG_TEXT_COLOR)
        .setOutlineColor(DEBUG_TEXT_COLOR)
        .setOutlineRange(.47f, .44f)
        .setSmoothness(.33f)
        .setTransformationProjectionMatrix(_transformationProjectionDebugText)
        .draw(_debugText->mesh());

    // enlighter
    _enlighter.draw(_stWidget->enlighterColor());

    // shortcuts text
    _stWidget->draw();

    #ifdef _DEBUG
        // IMGUI
        _drawImGui();
    #endif

    // buffers swap, automatic redrawing and timeline frame check
    swapBuffers();
    redraw();
    _timeline.nextFrame();
}

void UnderStory::USApplication::mouseScrollEvent(MouseScrollEvent& event) {
    //
    #ifdef _DEBUG
        if(_imgui.handleMouseScrollEvent(event)) {
            /* Prevent scrolling the page */
            event.setAccepted();
            return;
        }
    #endif

    //
    _propagateScrollEvent(event);
}

void UnderStory::USApplication::keyReleaseEvent(KeyEvent& event) {
    //
    #ifdef _DEBUG
        if(_imgui.handleKeyReleaseEvent(event)) return;
    #endif

    //
    _kmh.keyReleaseEvent(event);
}

void UnderStory::USApplication::keyPressEvent(KeyEvent& event) {
    //
    #ifdef _DEBUG
        if(_imgui.handleKeyPressEvent(event)) return;
    #endif

    //
    _kmh.keyPressEvent(event);
    _msh.keyPressEvent(event);

    switch (event.key()) {
    case KeyEvent::Key::Esc:
            _resetWorldMatrix();
        break;
    default:
        break;
    }
}

void UnderStory::USApplication::mouseMoveEvent(MouseMoveEvent& event) {
    //
    #ifdef _DEBUG
        if(_imgui.handleMouseMoveEvent(event)) return;
    #endif

    // find OpenGL cursor pos
    auto cursorPos = _cursorPosition(event);

    // propagate event
    _propagateMouseMoveEvent(event, cursorPos);
}

void UnderStory::USApplication::mousePressEvent(MouseEvent& event) {
    //
    #ifdef _DEBUG
        if(_imgui.handleMousePressEvent(event)) return;
    #endif

    //
    _propagateMousePressEvent(event);
}

void UnderStory::USApplication::mouseReleaseEvent(MouseEvent& event) {
    //
    #ifdef _DEBUG
        if(_imgui.handleMouseReleaseEvent(event)) return;
    #endif

    //
    _propagateMouseReleaseEvent(event);
}

//
//
//

void UnderStory::USApplication::textInputEvent(TextInputEvent& event) {
    #ifdef _DEBUG
        if(_imgui.handleTextInputEvent(event)) return;
    #endif
}

//
//
//

void UnderStory::USApplication::handleScrollEvent(MouseScrollEvent &event) {
    _msh.mouseScrollEvent(event, this->framebufferSize());
}

void UnderStory::USApplication::handlePressEvent(MouseEvent& event) {
    switch (event.button()) {
        case MouseEvent::Button::Left: {
            _mmh.mousePressEvent();

            if (_mouseState.hasLeftDoubleClick())
                _mmh.mouseDoubleClickEvent(event, this->framebufferSize());
        }
        break;

        case MouseEvent::Button::Right:
            _selectionRect.init(event, this->framebufferSize());
        break;

        default:
        break;
    }
}

void UnderStory::USApplication::handleLockReleaseEvent(MouseEvent& event) {
    switch (event.button()) {
        case MouseEvent::Button::Left: {
            if (_mouseState.isLeftClickDragging()) {
                _mmh.mouseMoveReleaseEvent();
                this->setCursor(Cursor::Arrow);
            }
        }
        break;

        case MouseEvent::Button::Right: {
            if (_selectionRect.isSelecting()) {
                _selectionRect.end(event, this->framebufferSize());
                _msh.snapshotZoom(_selectionRect.asRectangle());
            }
        }
        break;

        default:
        break;
    }
}

void UnderStory::USApplication::handleLockMoveEvent(MouseMoveEvent& event) {
    if (_mouseState.isLeftClickDragging()) {
        //
        this->setCursor(Cursor::ResizeAll);

        //
        _mmh.mouseMoveEvent(event, this->framebufferSize());

    } else if (_selectionRect.isSelecting()) {
        // if selection, update rect
        _selectionRect.update(event, this->framebufferSize());
    }
}

//
//
//

void UnderStory::USApplication::_resetWorldMatrix() {
    _mmh.stopAnim();
    _msh.stopAnim();
    _kmh.stopAnim();
    _transformationWorld = Magnum::Matrix3();
}

void UnderStory::USApplication::_updateDebugText() {
    auto us = _transformationWorld.uniformScaling();
    auto tr = _transformationWorld.translation() / Magnum::Vector2 {us};

    auto formatedText = Magnum::Utility::formatString(
        "x: {:.2}\ny: {:.2}\nZoom: x{:.2}",
        tr[0],
        tr[1],
        us
    );

    _debugText->render(formatedText);
}

Magnum::Vector2 UnderStory::USApplication::_cursorPosition(const Sdl2Application::MouseMoveEvent& event) const {
    Magnum::Vector2 fs{this->framebufferSize()};
    return (Magnum::Vector2{event.position()} - fs / 2.f) / fs * Magnum::Vector2{2.f, -2.f};
}

//
//
//

void UnderStory::USApplication::_initSentry() {
    auto options = sentry_options_new();
    sentry_options_set_dsn(options, SENTRY_ENDPOINT);

    #ifdef _DEBUG
        auto environement = "Debug";
    #else
        auto environement = "Production";
    #endif
    sentry_options_set_environment(options, environement);

    sentry_options_set_release(options, GITHUB_VERSION_NAME);
    sentry_options_set_debug(options, 1);

    // integration
    auto context = Context::normal();
    auto dbStr = context.path().string() + "/sentry_db";
    sentry_options_set_database_path(options, dbStr.c_str());

    sentry_init(options);
}

MAGNUM_APPLICATION_MAIN(UnderStory::USApplication)
